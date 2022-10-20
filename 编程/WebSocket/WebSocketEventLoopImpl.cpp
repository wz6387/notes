#include "WebSocketEventLoopImpl.h"
#include "event2/thread.h"
#ifdef WIN32
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

WebSocketEventLoopImpl::WebSocketEventLoopImpl()
	: base_(NULL)
	, thread_(NULL)
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif
	base_ = event_base_new();
}

WebSocketEventLoopImpl::~WebSocketEventLoopImpl()
{
#ifdef WIN32
	WSACleanup();
#endif
}

void WebSocketEventLoopImpl::loop()
{
	notifyEventInit();
	event_base_dispatch(base_);
	event_base_free(base_);
}

void WebSocketEventLoopImpl::quit()
{
	runInLoop([=]() {
		notifyEventDestory();
		event_base_loopbreak(base_);//必须要在io线程调用
	});
}

void WebSocketEventLoopImpl::runAfter(TimerCallback cb, int delay)
{
	Timer* t1 = new Timer;
	t1->cb = std::move(cb);
	t1->tv.tv_sec = delay;
	t1->tv.tv_usec = 0;
	t1->ev = evtimer_new(base_, WebSocketEventLoopImpl::do_timeout_cb, t1);
	runInLoop([t1]() {
		evtimer_add(t1->ev, &t1->tv);
	});
}

void WebSocketEventLoopImpl::cancel(void* timer)
{
	Timer* t1 = (Timer*)timer;
	if (t1) {
		runInLoop([t1]() {
			evtimer_del(t1->ev);
			delete t1;
		});
	}
}

int WebSocketEventLoopImpl::notifyEventInit()
{
	//fd[0] connector
	//fd[1] acceptor
	if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, fifo_.fd) < 0) {
		return -1;
	}
	struct event *ev = event_new(base_, fifo_.fd[1], EV_READ | EV_PERSIST, WebSocketEventLoopImpl::do_read_cb, this);
	event_add(ev, NULL);
	fifo_.ev = ev;
	return 0;
}

void WebSocketEventLoopImpl::notifyEventDestory()
{
	evutil_closesocket(fifo_.fd[0]);
	evutil_closesocket(fifo_.fd[1]);
	event_free(fifo_.ev);
}

void WebSocketEventLoopImpl::do_read_cb(evutil_socket_t listener, short event, void *arg)
{
	WebSocketEventLoopImpl *ptr = (WebSocketEventLoopImpl*)arg;
	char msg[512] = { 0 };
#ifdef WIN32
	::recv(listener, msg, sizeof(msg), 0);
#else
	::read(listener, msg, sizeof(msg));
#endif
	ptr->doPendingFunctors();
}

void WebSocketEventLoopImpl::do_timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	Timer *t = (Timer*)arg;
	t->cb();
	event_free(t->ev);
	delete t;
}

void WebSocketEventLoopImpl::runInLoop(Functor cb)
{
	if (isInLoopThread()) {
		cb();
	} else {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			pendingFunctors_.push_back(std::move(cb));
		}
#ifdef WIN32
		::send(fifo_.fd[0], "c", 1, 0);
#else
		::write(fifo_.fd[0], "c", 1);
#endif
	}
}

void WebSocketEventLoopImpl::doPendingFunctors()
{
	std::vector<Functor> functors;

	{
		std::lock_guard<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (const Functor& functor : functors)
	{
		functor();
	}
}