#include "WebSocketClient.h"
#include "WebSocketClientImpl.h"


WebSocketClient::WebSocketClient(WebSocketEventLoop* loop)
	: impl_(new WebSocketClientImpl(loop))
{

}

void WebSocketClient::connect(const std::string& hostname, int port, const std::string& clientId, const std::string& uri)
{ 
	impl_->connect(hostname, port, clientId, uri); 
}

void WebSocketClient::disconnect() 
{ 
	impl_->disconnect(); 
}

void WebSocketClient::setConnectionCallback(ConnectionCallback cb) 
{ 
	impl_->setConnectionCallback(cb); 
}

void WebSocketClient::setCloseCallback(CloseCallback cb) 
{ 
	impl_->setCloseCallback(cb); 
}

void WebSocketClient::setMessageCallback(MessageCallback cb) 
{ 
	impl_->setMessageCallback(cb); 
}

bool WebSocketClient::retry() const 
{ 
	return impl_->retry(); 
}

void WebSocketClient::enableRetry() 
{ 
	impl_->enableRetry(); 
}

void WebSocketClient::write(const std::string& message) 
{ 
	impl_->write(message); 
}