#ifndef _WEBSOCKET_EVENT_LOOP_THREAD_IMPL_H_
#define _WEBSOCKET_EVENT_LOOP_THREAD_IMPL_H_

#include <string>
#include <thread>
#include <condition_variable>

class WebSocketEventLoop;
class WebSocketEventLoopThreadImpl
{
public:
	WebSocketEventLoopThreadImpl();
	~WebSocketEventLoopThreadImpl();
	WebSocketEventLoop* startLoop();
private:
	WebSocketEventLoop* loop_;
	std::thread* thread_;
	static void threadFunc(void* arg);
	std::mutex mutex_;
	std::condition_variable cond_;
};
#endif
