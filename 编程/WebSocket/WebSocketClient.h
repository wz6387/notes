#ifndef _WEB_SOCKET_CLIENT_H_
#define _WEB_SOCKET_CLIENT_H_
#include <thread>
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


class WebSocketEventLoop;
class WebSocketClientImpl;
class DLLAPI WebSocketClient
{
public:
	WebSocketClient(WebSocketEventLoop* loop);
	~WebSocketClient(){}
	/**
	 * 连接，连接成功会进入连接回调
	 * @param hostname 域名或ip
	 * @param port 端口
	 * @param clientId 应用程序id
	 * @param uri 
	 */
	void connect(const std::string& hostname, int port, const std::string& clientId, const std::string& uri);

	/**
	 * 断开连接
	 */
	void disconnect();

	/**
	 * 连接成功回调
	 * @param cb 回调函数
	 */
	void setConnectionCallback(ConnectionCallback cb);

	/**
	 * 连接断开回调
	 * @param cb 回调函数
	 */
	void setCloseCallback(CloseCallback cb);

	/**
	 * 消息回调
	 * @param cb 回调函数
	 */
	void setMessageCallback(MessageCallback cb);

	/**
	 * 返回当前重试状态
	 * @return true设置了重试标志；false未设置
	 */
	bool retry() const;

	/**
	 * 重试使能，连接断开时重试
	 */
	void enableRetry();

	/**
	 * 发送消息
	 * @param message消息内容
	 */
	void write(const std::string& message);

private:
	std::shared_ptr<WebSocketClientImpl> impl_;	
};

#endif

#ifdef WIN32
#pragma warning(pop)
#endif