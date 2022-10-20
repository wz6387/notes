#include "WebSocketClientImpl.h"
#include "WebSocketClient.h"
#include <map>
#include <sstream>
#include <chrono>
#include "base64.h"
#include "SHA1.h"
#include "WebSocketEventLoop.h"
#include "WebSocketConnection.h"

//websocket组合包最大不得超过4MB(防止内存爆炸)
#define MAX_WS_PACKET (4 * 1024 * 1024)

WebSocketClientImpl::WebSocketClientImpl(WebSocketEventLoop* loop)
	: loop_(loop)
	, port_(0)
	, connection_(new WebSocketConnection(loop))
	, connectionCallback_(defaultConnectionCallback)
	, closeCallback_(defaultCloseCallback)
	, messageCallback_(defaultMessageCallback)
	, state_(kDisconnected)
	, threadStart_(false)
	, thread_(NULL)
	, retry_(false)
	, time_(NULL)

{
	

}

WebSocketClientImpl::~WebSocketClientImpl()
{

}

void WebSocketClientImpl::connect(const std::string& hostname, int port, const std::string& clientId, const std::string& uri)
{
	host_ = hostname;
	port_ = port;
	uri_ = uri;
	clientId_ = clientId;
	connection_->setConnectionCallback(std::bind(&WebSocketClientImpl::onConnect, shared_from_this(), _1));
	connection_->setCloseCallback(std::bind(&WebSocketClientImpl::onClose, shared_from_this(), _1));
	connection_->setMessageCallback(std::bind(&WebSocketClientImpl::onMessage, shared_from_this(), _1, _2));
	connection_->connect(hostname, port);
}

void WebSocketClientImpl::disconnect()
{
	retry_ = false;
	removeConnection();
}

void WebSocketClientImpl::onConnect(const WebSocketConnectionPtr& conn)
{
	setState(kHandshaking);
	handshake();
}

void WebSocketClientImpl::onMessage(const WebSocketConnectionPtr& conn, const std::string& msg)
{
	if (state_ == kHandshaking) {
		if (validateHandshake(msg)) {
			newConnection(conn);
		} else {
			removeConnection();
		}
	} else if (state_ == kHandshaked) {
		WebSocketSplitter::decode((uint8_t*)msg.c_str(), msg.size());
	}
}

void WebSocketClientImpl::onClose(const WebSocketConnectionPtr& conn)
{
	removeConnection();
	closeCallback_(conn);
}

std::string WebSocketClientImpl::generateKey(const std::string& key)
{
	//sha-1
	std::string sha1_encode = SHA1::encode_bin("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

	//base64 encode
	return base64_encode((const uint8_t*)sha1_encode.c_str(), sha1_encode.size());
}

void WebSocketClientImpl::handshake()
{
	// 设置为 handshaking， 等解析完服务的返回，验证之后，调整为handshaked
	// 组装handshake请求，并尝试发送请求
	std::string reqStr = "GET ";
	if (uri_.length() > 0) {
		if (uri_[0] != '/') {
			uri_ = "/" + uri_;
		}
	}
	else {
		uri_ = "/";
	}

	reqStr += uri_;

	std::string key = "key_1";
	reqStr += " HTTP/1.1\r\n";
	reqStr.append("Host: " + host_ + "\r\n");
	reqStr.append("ClientId:" + clientId_ + "\r\n");
	reqStr.append("Origin: http://www.example.com/ws \r\n");
	reqStr.append("Upgrade: websocket\r\n");
	reqStr.append("Connection: upgrade\r\n");
	reqStr.append("Sec-WebSocket-Key: " + generateKey(key) + "\r\n");
	reqStr.append("Sec-WebSocket-Version: 13\r\n\r\n");

	connection_->send(reqStr);
}

bool WebSocketClientImpl::validateHandshake(const std::string& msg)
{
	std::string handsharkMsg;
	handsharkMsg.append(msg);
	std::string tag = "\r\n\r\n";
	int pos = handsharkMsg.find(tag);
	if (pos == std::string::npos) {
		return false;
	}
	std::string buf = handsharkMsg.substr(0, pos + tag.length());
	handsharkMsg = handsharkMsg.substr(pos + tag.length());

	std::map<std::string, std::string> headerMap;
	std::istringstream s(buf);
	std::string request;

	std::getline(s, request);
	if (request[request.size() - 1] == '\r') {
		request.erase(request.end() - 1);
	}
	else {
		return false;
	}

	std::string header;
	std::string::size_type end;
	while (std::getline(s, header) && header != "\r") {
		if (header[header.size() - 1] != '\r') {
			continue; //end
		}
		else {
			header.erase(header.end() - 1); //remove last char
		}

		end = header.find(": ", 0);
		if (end != std::string::npos) {
			std::string key = header.substr(0, end);
			std::string value = header.substr(end + 2);
			headerMap[key] = value;
		}
	}

	auto iter = headerMap.find("Sec-WebSocket-Accept");
	if (headerMap.end() == iter) {
		return false;
	}
	else {
		return true;
	}
}

void WebSocketClientImpl::pong()
{
	WebSocketHeader header;
	header.m_fin = true;
	header.m_reserved = 0;
	header.m_opcode = PONG;
	header.m_mask_flag = true;
	WebSocketSplitter::encode(header, NULL, 0);
}

void WebSocketClientImpl::ping()
{
	WebSocketHeader header;
	header.m_fin = true;
	header.m_reserved = 0;
	header.m_opcode = PING;
	header.m_mask_flag = true;
	WebSocketSplitter::encode(header, NULL, 0);
}

void WebSocketClientImpl::startPingThread()
{
	if (thread_ == NULL) {
		threadStart_ = true;
		thread_ = new std::thread([this]() {
			while (threadStart_) {
				if (state_ == kHandshaked) {
					for (auto i = 0; i < 20; ++i) {
						std::this_thread::sleep_for(std::chrono::seconds(1));
						if (!threadStart_) {
							return;
						}
					}
					printf("send ping\n");
					ping();
				}
			}
		});
	}
}

void WebSocketClientImpl::stopPingThread()
{
	if (thread_) {
		threadStart_ = false;
		thread_->join();
		delete thread_;
		thread_ = NULL;
	}
}

void WebSocketClientImpl::newConnection(const WebSocketConnectionPtr& conn)
{
	setState(kHandshaked);
	//startPingThread();
	connectionCallback_(conn);
	//conn->setCloseCallback(std::bind(&WebSocketClientImpl::onClose, this, conn));
}

void WebSocketClientImpl::removeConnection()
{
	setState(kDisconnected);
	connection_->disconnect();
	if (retry_) {
		//20s重试一次
		loop_->runAfter(std::bind(&WebSocketClientImpl::connect, shared_from_this(), host_, port_, clientId_, uri_), 20);
	}
}

void WebSocketClientImpl::write(const std::string& message)
{
	if (state_ == kHandshaked) {
		WebSocketHeader header;
		header.m_fin = true;
		header.m_reserved = 0;
		header.m_opcode = TEXT;
		header.m_mask_flag = true;
		WebSocketSplitter::encode(header, message.c_str(), message.size());
	}
}

void WebSocketClientImpl::onWebSocketEncodeData(const char* buf, int len)
{
	connection_->send(std::string(buf, len));
}

void WebSocketClientImpl::onWebSocketDecodePayload(const WebSocketHeader &header, const uint8_t *ptr, uint64_t len, uint64_t recved)
{
	m_payload_section.append((char *)ptr, (const unsigned int)len);
}

void WebSocketClientImpl::onWebSocketDecodeComplete(const WebSocketHeader &header_in)
{
	WebSocketHeader& header = const_cast<WebSocketHeader&>(header_in);
	auto  flag = header.m_mask_flag;
	header.m_mask_flag = false;

	switch (header.m_opcode) {
	case WebSocketHeader::CLOSE: {
		removeConnection();
		break;
	}

	case WebSocketHeader::PING: {
		std::cout << "recv ping" << std::endl;
		pong();
		//header.m_opcode = WebSocketHeader::PONG;
		//WebsocketSplitter::encode(header, m_payload_section.c_str(), m_payload_section.length());
		break;
	}

	case WebSocketHeader::CONTINUATION:
	case WebSocketHeader::TEXT:
	case WebSocketHeader::BINARY: {
		if (!header.m_fin) {
			//还有后续分片数据, 我们先缓存数据，所有分片收集完成才一次性输出
			m_payload_cache.append(std::move(m_payload_section));
			if (m_payload_cache.size() < MAX_WS_PACKET) {
				//还有内存容量缓存分片数据
				break;
			}
			else {
				//分片缓存太大，需要清空
				m_payload_cache.clear();
				removeConnection();
			}
		}

		//最后一个包
		if (m_payload_cache.empty()) {
			messageCallback_(connection_, m_payload_section);
			//这个包是唯一个分片
			//_session->onRecv(std::make_shared<WebSocketBuffer>(header._opcode, header._fin, std::move(_payload_section)));
			break;
		}

		//这个包由多个分片组成
		m_payload_cache.append(std::move(m_payload_section));
		messageCallback_(connection_, m_payload_cache);
		m_payload_cache.clear();
		break;
	}

	default: break;
	}
	m_payload_section.clear();
	header.m_mask_flag = flag;
}
