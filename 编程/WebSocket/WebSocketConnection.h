#ifndef _WEB_SOCKET_CONNECTION_H_
#define _WEB_SOCKET_CONNECTION_H_

#include <string>
#include <mutex>
#include "event.h"
#include "Callbacks.h"
#include "TimerId.h"

class Timer;
class WebSocketEventLoop;
class WebSocketConnection : public std::enable_shared_from_this<WebSocketConnection>
{
public:
	WebSocketConnection(WebSocketEventLoop* loop);
	~WebSocketConnection();
	void connect(const std::string& hostname, int port);
	void disconnect();
	void setConnectionCallback(ConnectionCallback cb) { connectionCallback_ = cb; }
	void setCloseCallback(CloseCallback cb) { closeCallback_ = cb; }
	void setMessageCallback(MessageCallback cb) { messageCallback_ = cb;	}
	bool connected() const { return state_ == kConnected; }
	void send(const std::string& message);

private:
	enum StateE { kDisconnected, kConnecting, kConnected };
	void setState(StateE s) { state_ = s; }
	void connectionCallBack() {	connectionCallback_(shared_from_this()); }
	void closeCallBack() { closeCallback_(shared_from_this()); }
	void messageCallBack(const std::string& msg) { messageCallback_(shared_from_this(), msg); }
	void connect_addr(const sockaddr* addr);
	void retry();
	void writeMsg(const std::string& message);

	//frame read callback
	static void bev_read_cb(struct bufferevent *bev, void *ctx);

	//websocket write callback
	static void bev_write_cb(struct bufferevent *bev, void *ctx);

	//connection close callback
	static void bev_event_cb(struct bufferevent *bev, short what, void *ctx);

	static void dns_resolve_cb(int result, struct evutil_addrinfo *res, void *arg);

	WebSocketEventLoop*  loop_;
	bufferevent* bev_;
	StateE state_;
	std::string host_;
	int port_;
	ConnectionCallback connectionCallback_;
	CloseCallback closeCallback_;
	MessageCallback messageCallback_;
};


#endif
