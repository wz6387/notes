#ifndef _WEBSOCKET_EVENT_LOOP_THREAD_H_
#define _WEBSOCKET_EVENT_LOOP_THREAD_H_
#include <memory>

#ifdef WIN32
#ifdef __DLL_EXPORTS__
#define DLLAPI  __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif
#else
#define DLLAPI  __attribute__ ((visibility("default")))
#endif

#ifdef WIN32
#pragma warning(push)
#pragma warning( disable:4251 )
#endif

class WebSocketEventLoop;
class WebSocketEventLoopThreadImpl;
class DLLAPI WebSocketEventLoopThread
{
public:
	WebSocketEventLoopThread();
	~WebSocketEventLoopThread();
	/**
	 * 在线程中开启事件循环，对象销毁时自动结束事件循环并停止线程
	 */
	WebSocketEventLoop* startLoop();
private:
	std::shared_ptr<WebSocketEventLoopThreadImpl> impl_;
};
#endif // !_WEBSOCKET_EVENT_LOOP_THREAD_H_

#ifdef WIN32
#pragma warning(pop)
#endif