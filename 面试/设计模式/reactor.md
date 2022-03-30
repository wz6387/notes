reactor模式是“非阻塞IO+IO多路复用”这种模型，IO多路复用即select、poll、epoll等系统调用，程序的基本结构是一个事件循环，以事件驱动和事件回调的方式实现业务逻辑

muduo的简化类图

![image-20220330112620547](C:\Users\A10727\AppData\Roaming\Typora\typora-user-images\image-20220330112620547.png)

one loop per thread + thread pool 模型

每个线程只有一个EventLoop，一个EventLoop相当于一个reactor。一个main Reactor负责accept连接，然后把连接挂在某个sub Reactor（muduo采用round-robin方式选择sub Reactor），这样该连接的所有操作都在那个sub Reactor所处的线程中完成。线程池用来完成连接上的计算任务，见图

![image-20220330143011577](C:\Users\A10727\AppData\Roaming\Typora\typora-user-images\image-20220330143011577.png)

EventLoop的事件循环在loop函数

```c++
void EventLoop::loop()
{
    quit_ = false; 
    while (!quit_)
    {
      activeChannels_.clear();
      pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
      for (Channel* channel : activeChannels_)
      {
        currentActiveChannel_ = channel;
        currentActiveChannel_->handleEvent(pollReturnTime_);
      }
      currentActiveChannel_ = NULL;
      doPendingFunctors();
    }
}
```

它调用poll监听事件，当有事件发生时，将活动事件填入activeChannels_（它是一个vector），然后由handleEvent处理。来看看文件描述符是如何被监听起来的。

1、监听

假如我们是服务端，在服务启动时，会创建TcpServer对象，TcpServer对象会创建Acceptor对象去监听连接，Acceptor初始化会创建监听套接字，并将套接字与一个channel绑定，channel是用来干嘛的？我们可以理解为channel是用来做事件分发，根据不同的事件回调不同函数。

```c++
Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
  : loop_(loop),
    acceptSocket_(sockets::createNonblockingOrDie(listenAddr.family())),
    acceptChannel_(loop, acceptSocket_.fd()),
    listening_(false),
    idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
  assert(idleFd_ >= 0);
  acceptSocket_.setReuseAddr(true);
  acceptSocket_.setReusePort(reuseport);
  acceptSocket_.bindAddress(listenAddr);
  acceptChannel_.setReadCallback(
      std::bind(&Acceptor::handleRead, this));
}
```

Acceptor有listen接口，在接口中监听连接

```c++
void Acceptor::listen()
{
  loop_->assertInLoopThread();
  listening_ = true;
  acceptSocket_.listen();
  acceptChannel_.enableReading();
}
```

其中还调用了Channel::enableReading

```c++
void enableReading() { events_ |= kReadEvent; update(); }
```

里面设置了需要监听的事件类型，然后update()

```c++
void Channel::update()
{
  addedToLoop_ = true;
  loop_->updateChannel(this);
}
```

可以看到，它调用了EventLoop的updateChannel接口

```c++
void EventLoop::updateChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->updateChannel(channel);
}
```

最终又调用了poll的updateChannel

```c++
void PollPoller::updateChannel(Channel* channel)
{
  Poller::assertInLoopThread();
  LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
  if (channel->index() < 0)
  {
    // a new one, add to pollfds_
    assert(channels_.find(channel->fd()) == channels_.end());
    struct pollfd pfd;
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    pollfds_.push_back(pfd);
    int idx = static_cast<int>(pollfds_.size())-1;
    channel->set_index(idx);
    channels_[pfd.fd] = channel;
  }
  else
  {
    // update existing one
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
    struct pollfd& pfd = pollfds_[idx];
    assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    if (channel->isNoneEvent())
    {
      // ignore this pollfd
      pfd.fd = -channel->fd()-1;
    }
  }
}
```

这里就是监听文件描述符的地方了

2、连接

有连接事件发生时，EventLoop会调用Channel的handleEvent，这时会回调到注册到Channel的readCallback_。上一步的监听，Acceptor在构造时将Acceptor::handleRead注册到了Channel的读回调，因此将调用Acceptor::handleRead

```c++
void Acceptor::handleRead()
{
  loop_->assertInLoopThread();
  InetAddress peerAddr;
  //FIXME loop until no more
  int connfd = acceptSocket_.accept(&peerAddr);
  if (connfd >= 0)
  {
    // string hostport = peerAddr.toIpPort();
    // LOG_TRACE << "Accepts of " << hostport;
    if (newConnectionCallback_)
    {
      newConnectionCallback_(connfd, peerAddr);
    }
    else
    {
      sockets::close(connfd);
    }
  }
  else
  {
    LOG_SYSERR << "in Acceptor::handleRead";
    // Read the section named "The special problem of
    // accept()ing when you can't" in libev's doc.
    // By Marc Lehmann, author of libev.
    if (errno == EMFILE)
    {
      ::close(idleFd_);
      idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
      ::close(idleFd_);
      idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
}
```

接受连接成功，会回调到TcpServer::newConnection

```c++
void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
  loop_->assertInLoopThread();
  EventLoop* ioLoop = threadPool_->getNextLoop();
  char buf[64];
  snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
  ++nextConnId_;
  string connName = name_ + buf;

  LOG_INFO << "TcpServer::newConnection [" << name_
           << "] - new connection [" << connName
           << "] from " << peerAddr.toIpPort();
  InetAddress localAddr(sockets::getLocalAddr(sockfd));
  // FIXME poll with zero timeout to double confirm the new connection
  // FIXME use make_shared if necessary
  TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                          connName,
                                          sockfd,
                                          localAddr,
                                          peerAddr));
  connections_[connName] = conn;
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  conn->setCloseCallback(
      std::bind(&TcpServer::removeConnection, this, _1)); // FIXME: unsafe
  ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}
```

可以看到，它从线程池中取出了一个EventLoop，将新连接交由该EventLoop接管，这正是开头提到的sub reactor

