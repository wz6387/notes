### 1、网络七层协议模型

![img](https://images2015.cnblogs.com/blog/983980/201611/983980-20161121173924409-1128983178.png)



### 2、TCP封装和协议头格式

![img](https://images2015.cnblogs.com/blog/740952/201611/740952-20161107132806686-1082325930.png)

如上图，TCP头紧接着IP头，IPv4头=<u>20</u>字节，IPv6=40字节，不携带选项(option)的TCP头为<u>20</u>字节，携带选项的TCP头最长可达60字节。

完整的以太网帧结构：

![img](data:image/png;base64,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)

​      (CRC并不计数到链路层数据长度)

TCP头部结构如下图：

![img](https://img-blog.csdn.net/20171125165426282?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMjkzNDQ3NTc=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

**16位窗口大小：**是流量控制的一个手段，这里的窗口指的是接收窗口，告诉对方本端还能缓存多少字节的数据，这样对方就能控制发送数据的速度

**32位序号：**表示字节在数据流中的序号，比如初始序号为1，发送了12字节，那么下一次的序号就是12+1=13

**32位确认号：**表示期望接收的字节序列，比如接收端收到一个SN=1，长度为12字节的数据包，它回复确认时，ACK Number应该设置为1+12=13

**SYN：**表示请求建立连接

**FIN：**表示要关闭连接

**选项信息：**有一个窗口扩大因子，假设窗口大小为N，扩大因子为M，则实际的接收窗口大小为N*2^M，即N左移M位



### 3、MTU和MSS

+ MTU：最大传输单元，即数据链路层最大一次传输数据的大小，以普遍的以太网接口为例，缺省MTU=1500字节。如果有大于1500字节数据需要发送，需要分片才行，这些分片有个共同点，即IP Header ID相同
+ MSS：TCP提交给IP层的最大分段大小，不包含TCP Header和TCP Option，只包含TCP Payload。如果物理接口MDU=1500字节，则MSS=1500 - 20(IP Header) - 20(TCP Header) = 1460字节。SYN消息，MSS的值取决于发送端和接收端两者较小的值



### 4、三次握手与四次挥手

![img](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9qaWFudHVrdS1saXdlbmJpbi5vc3MtY24tc2hhbmdoYWkuYWxpeXVuY3MuY29tLyVFOSU5RCVBMiVFNyVCQiU4RiVFNSU5MCU4OCVFOSU5QiU4Ni8lRTQlQjglODklRTYlQUMlQTElRTYlOEYlQTElRTYlODklOEIucG5n?x-oss-process=image/format,png)

![img](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9qaWFudHVrdS1saXdlbmJpbi5vc3MtY24tc2hhbmdoYWkuYWxpeXVuY3MuY29tLyVFOSU5RCVBMiVFNyVCQiU4RiVFNSU5MCU4OCVFOSU5QiU4Ni8lRTUlOUIlOUIlRTYlQUMlQTElRTYlOEMlQTUlRTYlODklOEIucG5n?x-oss-process=image/format,png)

**为什么是三次握手不是二次或四次？**

三次握手的目的是为了防止已失效的连接请求突然又传到服务端，因而产生错误。比如存在下面情况：client发出的第一个连接请求在网络上长时间滞留，以致延误到连接释放后才到达server。本来这是已失效的报文，但server收到此报文后，误认为client发出了一个新的连接请求，于是就向client发送确认报文，同意建立连接。如果不采用三次握手，那么新的连接就建立了。由于client并没有发出建立连接请求，因此不会理睬server的确认，但server却一直等待client发来数据，因此server的资源就浪费了。采用三次握手，client不会向server的确认发出确认，server由于收不到确认，就知道client没有要求连接。

**TIME_WAIT的意义**

TIME_WAIT是指四次挥手中客户端接收了服务端的FIN报文并发送ACK报文给服务端后，仍然需要等待2MSL(Linux中2MSL默认是60秒)的过程。原因是假设网络是不可靠的，有可能最后一个ACK丢失，如果客户端发送的ACK丢失，服务端会再次发送FIN给客户端，所以TIME_WAIT状态就是用来重发可能丢失的ACK报文。

**服务器出现大量close_wait的原因**

close_wait是服务端收到FIN，但自己没有发送FIN出现的，原因有两种：

+ 内部业务占用过多时间；或者还有数据要发送；或者业务逻辑有问题，没有执行close方法(使用close()，只有当套接字的引用计数为0时才会终止连接，而用shutdown()可以直接关闭连接)
+ 服务器的父进程派生出子进程，子进程继承了socket，收到FIN时子进程处理但是父进程没有处理该信号，导致socket引用不为0无法回收



### 5、滑动窗口

滑动窗口用于流量控制，以避免拥塞的发生。可以理解为接收端能提供的缓冲区大小，由于窗口是16为bit，所以接收端TCP能最大提供65535个字节的缓冲。发送方根据这个值来计算自己最多能发送多少的数据

![img](https://images2017.cnblogs.com/blog/1232796/201710/1232796-20171016103123177-958324725.jpg)



### 6、拥塞控制

拥塞控制有四种算法：**慢启动**、**拥塞避免**、**快速重传**和**快速恢复**

**慢启动**：发送方维持一个“拥塞窗口”的变量cwnd，该变量和接收窗口共同决定了发送者的发送窗口。当主机开始发送数据时，避免一下子将大量字节注入网络，造成拥塞，先发送1个字节的试探报文，当收到确认后，就发送2个字节的报文，当再次收到2个字节的确认后，就发送4个字节，即指数增长。当拥塞窗口大于慢启动阈值(ssthresh)时，停止慢启动算法改用拥塞避免算法

**拥塞避免**：每经过一个往返时间RTT就把发送方的拥塞窗口+1，让拥塞窗口缓慢增大。当出现网络拥塞时，将慢启动阈值设为原先的一半，然后将cwnd设为1，执行慢启动算法

![img](https://images2017.cnblogs.com/blog/1232796/201710/1232796-20171016103341052-1149390924.png)

**快速重传**：当发送端连续收到三个重复的ack时，表示该数据已经丢失，需要立刻重传该包。此时慢启动阈值减半，cwnd设为慢启动阈值，执行拥塞避免算法

**快速恢复**：当超过设定时间没有收到某个报文的ack时，表示网络拥塞，慢启动阈值减半，拥塞窗口cwnd=1，进入慢启动阶段

