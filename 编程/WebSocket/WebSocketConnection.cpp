#include "WebSocketConnection.h"
#include <iostream>
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/dns.h"
#include "WebSocketEventLoop.h"

#ifndef WIN32
#define EVENT__HAVE_STRUCT_ADDRINFO
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#endif

void defaultConnectionCallback(const WebSocketConnectionPtr& conn)
{
	std::cout <<"default connection call back" << std::endl;
}

void defaultCloseCallback(const WebSocketConnectionPtr& conn)
{
	std::cout <<"default close call back" << std::endl;
}

void defaultMessageCallback(const WebSocketConnectionPtr& conn, const std::string& msg)
{
	std::cout << "default msg callback" << std::endl;
}

WebSocketConnection::WebSocketConnection(WebSocketEventLoop* loop)
	: loop_(loop)
	, bev_(NULL)
	, state_(kDisconnected)
	, port_(0)
{
	//dns_ = evdns_base_new(base, 1);
}

WebSocketConnection::~WebSocketConnection()
{
	//evdns_base_free(dns_, 0);
}

void WebSocketConnection::connect(const std::string& hostname, int port)
{
	int err = 0;
	struct evutil_addrinfo *answer = NULL;
	char port_buf[32] = { 0 };

	if (state_ != kDisconnected) {
		return;
	}
	host_ = hostname;
	port_ = port;
	struct evutil_addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	//hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	//阻塞式域名解析
	evutil_snprintf(port_buf, sizeof(port_buf), "%d", port);
	err = evutil_getaddrinfo(hostname.c_str(), port_buf, &hints, &answer);
	if (err != 0) {
		//retry();
		std::cout << "dns failed:" << err << std::endl;
		return;
	}
	setState(kConnecting);
	connect_addr(answer->ai_addr);

	//非阻塞式
	//setState(kConnecting);
	//evdns_getaddrinfo(dns_, hostname.c_str(), NULL, &hints, dns_resolve_cb, this);
}

void WebSocketConnection::connect_addr(const sockaddr* addr)
{
	//std::lock_guard<std::mutex> lock(bevlock_);
	loop_->runInLoop([&, this, addr]() {
		bev_ = bufferevent_socket_new((event_base*)(loop_->getEventBase()), -1, BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE /*| BEV_OPT_DEFER_CALLBACKS*/);
		//设定bufferevent接收数据回调函数
		bufferevent_setcb(bev_, WebSocketConnection::bev_read_cb, WebSocketConnection::bev_write_cb, WebSocketConnection::bev_event_cb, this);
		//将bev事件添加到epoll
		bufferevent_enable(bev_, EV_WRITE | EV_READ);
		//调用socket_connect连接，此时设定了fd为非阻塞
		bufferevent_socket_connect(bev_, addr, sizeof(sockaddr));
	});
}

void WebSocketConnection::disconnect()
{
	//std::lock_guard<std::mutex> lock(bevlock_);
	loop_->runInLoop([&]() {
		if (bev_) {
			setState(kDisconnected);
			bufferevent_disable(bev_, EV_WRITE | EV_READ);
			bufferevent_free(bev_);
			bev_ = NULL;
		}
	});
}

//connection close callback
void WebSocketConnection::bev_event_cb(struct bufferevent* bev, short event, void* ctx)
{
	WebSocketConnection* ptr = (WebSocketConnection*)ctx;
	if (ptr == NULL) {
		return;
	}
	// closed
	if (event & BEV_EVENT_READING) { //读错误
		std::cout << "reading error" << std::endl;;
	} else if (event & BEV_EVENT_WRITING) { //写错误
		std::cout << "writing error" << std::endl;
	} else if (event & BEV_EVENT_EOF) { //连接关闭
		std::cout << "connection closed" << std::endl;
		ptr->setState(kDisconnected);
	} else if (event & BEV_EVENT_ERROR) { //连接错误 
		std::cout << "some other error" << std::endl;
		ptr->setState(kDisconnected);
	} else if (event & BEV_EVENT_TIMEOUT) { //超时
		std::cout << "timeout" << std::endl;
	} else if (event & BEV_EVENT_CONNECTED) { //连接成功
		ptr->setState(kConnected);
		ptr->connectionCallBack();
		return;
	}
	
	//释放资源
	ptr->disconnect();
	ptr->closeCallBack();
}

//frame read callback
void WebSocketConnection::bev_read_cb(struct bufferevent* bev, void* ctx)
{
	WebSocketConnection* ptr = (WebSocketConnection*)ctx;
	if (ptr == NULL) {
		return;
	}

	std::string msg;
	while (true) {
		auto len = 0;
		char buffer[4096] = { 0 };
		//std::lock_guard<std::mutex> lock(ptr->bevlock_);
		if (ptr->bev_ == NULL) {
			return;
		}
		len = bufferevent_read(bev, buffer, sizeof(buffer));
		if (len == 0) {
			break;
		}
		msg.append(buffer, len);
	}

	ptr->messageCallBack(msg);
}

//websocket write callback
void WebSocketConnection::bev_write_cb(struct bufferevent* bev, void* ctx)
{
	/*WebSocketConnection* ptr = (WebSocketConnection*)ctx;
	if (ptr == NULL) {
		return;
	}

	std::string writemsg;
	ptr->buffer_.read(writemsg);
	if (!writemsg.empty()) {
		pthread_mutex_lock(&ptr->bevlock_);
		if (ptr->bev_ == NULL) {
			pthread_mutex_unlock(&ptr->bevlock_);
			return;
		}
		bufferevent_write(bev, writemsg.c_str(), writemsg.size());
		pthread_mutex_unlock(&ptr->bevlock_);
	}*/
}

void WebSocketConnection::dns_resolve_cb(int result, struct evutil_addrinfo* res, void* ctx)
{
	WebSocketConnection* ptr = (WebSocketConnection*)ctx;
	if (ptr == NULL) {
		return;
	}

	if (result != DNS_ERR_NONE) {
		ptr->setState(kDisconnected);
		std::cout << "dns failed, errorCode=" << result << ", errmsg=" << evutil_gai_strerror(result) << std::endl;
	} else {
		// 连接第一个 ip, 这里可以做一些优化, 以后再说
		// winner take all 策略: 尝试连接所有addr， 第一个连接上的进行处理，其他连接丢弃
		struct sockaddr_in* sin = (struct sockaddr_in*)res->ai_addr;
		char buf[128] = { 0 };
		auto s = evutil_inet_ntop(AF_INET, &sin->sin_addr, buf, 128);
		sin->sin_port = htons(ptr->port_);
		std::cout << "resolved ip = " << s << std::endl;
		ptr->connect_addr((const sockaddr*)sin);
		evutil_freeaddrinfo(res);
	}
}

void WebSocketConnection::writeMsg(const std::string& message)
{
	if (state_ == kConnected) {
		//std::lock_guard<std::mutex> lock(bevlock_);
		if (bev_ == NULL) {
			return;
		}
		bufferevent_write(bev_, message.c_str(), message.size());
		//buffer_.write(message);
	}
}

void WebSocketConnection::send(const std::string& message)
{
	loop_->runInLoop(std::bind(&WebSocketConnection::writeMsg, this, message));
}
