#include "WebSocketEventLoop.h"
#include "WebSocketEventLoopImpl.h"

WebSocketEventLoop::WebSocketEventLoop()
	: impl_(new WebSocketEventLoopImpl)
{

}

WebSocketEventLoop::~WebSocketEventLoop()
{

}

void WebSocketEventLoop::loop()
{
	impl_->loop();
}

void WebSocketEventLoop::quit()
{
	impl_->quit();
}

void WebSocketEventLoop::runAfter(TimerCallback cb, int delay)
{
	return impl_->runAfter(cb, delay);
}

void* WebSocketEventLoop::getEventBase()
{
	return impl_->getBase();
}

void WebSocketEventLoop::runInLoop(Functor cb)
{
	impl_->runInLoop(cb);
}

bool WebSocketEventLoop::isInLoopThread() const
{
	return impl_->isInLoopThread();
}