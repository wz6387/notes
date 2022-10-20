#ifndef _WEBSOCKET_EVENTLOOP_IMPL_H_
#define _WEBSOCKET_EVENTLOOP_IMPL_H_
#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include "event.h"
#include "Callbacks.h"


class WebSocketEventLoopImpl
{
public:
	struct EventFifo {
		struct event* ev;
		evutil_socket_t fd[2];
	};

	struct Timer {
		struct event* ev;
		struct timeval tv;
		TimerCallback cb;
	};
public:
	typedef std::function<void()> Functor;

	WebSocketEventLoopImpl();
	~WebSocketEventLoopImpl();
	void loop();
	void quit();
	void runAfter(TimerCallback cb, int delay);
	void cancel(void* timer);
	event_base* getBase() const { return base_; }
	void runInLoop(Functor cb);
	bool isInLoopThread() const { return threadId_ == std::this_thread::get_id(); }
private:
	int notifyEventInit();
	void notifyEventDestory();
	void doPendingFunctors();
	static void do_read_cb(evutil_socket_t listener, short event, void *arg);
	static void do_timeout_cb(evutil_socket_t fd, short event, void *arg);

	event_base *base_;
	std::thread* thread_;
	std::thread::id threadId_;
	EventFifo fifo_;
	std::mutex mutex_;
	std::vector<Functor> pendingFunctors_;
};
#endif
