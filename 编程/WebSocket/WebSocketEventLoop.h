#ifndef _WEB_SOCKET_EVENT_LOOP_H_
#define _WEB_SOCKET_EVENT_LOOP_H_
#include "Callbacks.h"

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

class WebSocketEventLoopImpl;
class DLLAPI WebSocketEventLoop
{
public:
	typedef std::function<void()> Functor;

	WebSocketEventLoop();
	~WebSocketEventLoop();
	/**
	 * 开启事件循环，没有事件发生时，循环处于阻塞状态
	 */
	void loop();

	/**
	 * 停止事件循环
	 */
	void quit();

	/**
	 * 定时任务，每隔一段时间运行一次，在非IO线程运行
	 * @param cb 任务函数
	 * @param delay 间隔时间
	 * @return
	 */
	void runAfter(TimerCallback cb, int delay);

	/**
	 * 获取libevent的base
	 * @return base指针
	 */
	void* getEventBase();

	/**
	 * 在IO线程执行函数，保证了函数在事件循环的线程中运行。任何IO操作都应该放在IO线程中执行，比如发送数据，断开连接
	 * @param cb 任务函数
	 */
	void runInLoop(Functor cb);

	/**
	 * 判断当前线程是否在IO线程
	 * @param true 在IO线程; false 不在
	 */
	bool isInLoopThread() const;
private:
	std::shared_ptr<WebSocketEventLoopImpl> impl_;
};

#endif

#ifdef WIN32
#pragma warning(pop)
#endif