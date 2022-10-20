#ifndef _WEBSOCKET_CLIENT_IMPL_H_
#define _WEBSOCKET_CLIENT_IMPL_H_
#include <thread>
#include "Callbacks.h"
#include "WebSocketSplitter.h"

class WebSocketEventLoop;
class WebSocketConnection;
class WebSocketClientImpl : public WebSocketSplitter, public std::enable_shared_from_this<WebSocketClientImpl>
{
public:
	WebSocketClientImpl(WebSocketEventLoop* loop);
	~WebSocketClientImpl();
	void connect(const std::string& hostname, int port, const std::string& clientId, const std::string& uri);
	void disconnect();
	void setConnectionCallback(ConnectionCallback cb) { connectionCallback_ = cb; }
	void setCloseCallback(CloseCallback cb) { closeCallback_ = cb; }
	void setMessageCallback(MessageCallback cb) { messageCallback_ = cb; }
	bool retry() const { return retry_; }
	void enableRetry() { retry_ = true; }
	void write(const std::string& message);

private:
	enum HandShakeStateE { kDisconnected, kHandshaking, kHandshaked };
	void setState(HandShakeStateE s) { state_ = s; }
	void handshake();
	void ping();
	void pong();
	bool validateHandshake(const std::string& msg);
	std::string generateKey(const std::string& key);
	void onConnect(const WebSocketConnectionPtr& conn);
	void onMessage(const WebSocketConnectionPtr& conn, const std::string& msg);
	void onClose(const WebSocketConnectionPtr& conn);
	void startPingThread();
	void stopPingThread();
	void newConnection(const WebSocketConnectionPtr& conn);
	void removeConnection();


	virtual void onWebSocketEncodeData(const char* buf, int len);
	virtual void onWebSocketDecodePayload(const WebSocketHeader &header, const uint8_t *ptr, uint64_t len, uint64_t recved);
	virtual void onWebSocketDecodeComplete(const WebSocketHeader &header);

	WebSocketEventLoop* loop_;
	std::string host_;
	std::string uri_;
	int port_;
	WebSocketConnectionPtr connection_;
	ConnectionCallback connectionCallback_;
	CloseCallback closeCallback_;
	MessageCallback messageCallback_;
	HandShakeStateE state_;
	bool threadStart_;
	std::thread* thread_;
	bool retry_;
	std::string m_payload_cache;
	std::string m_payload_section;
	void* time_;
	std::string clientId_;
};
#endif // _WEBSOCKET_CLIENT_IMPL_H_
