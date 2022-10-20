#include "WebSocketEventLoopThread.h"
#include "WebSocketEventLoopThreadImpl.h"

WebSocketEventLoopThread::WebSocketEventLoopThread()
	: impl_(new WebSocketEventLoopThreadImpl)
{
	
}

WebSocketEventLoopThread::~WebSocketEventLoopThread()
{

}

WebSocketEventLoop* WebSocketEventLoopThread::startLoop()
{
	return impl_->startLoop();
}