#include "WebSocketEventLoopThreadImpl.h"
#include "WebSocketEventLoop.h"

WebSocketEventLoopThreadImpl::WebSocketEventLoopThreadImpl()
	: loop_(NULL)
	, thread_(NULL)
{

}

WebSocketEventLoopThreadImpl::~WebSocketEventLoopThreadImpl()
{
	if (loop_ && thread_) {
		loop_->quit();
		thread_->join();
		delete thread_;
		thread_ = NULL;
	}
}

WebSocketEventLoop* WebSocketEventLoopThreadImpl::startLoop()
{
	thread_ = new std::thread(&WebSocketEventLoopThreadImpl::threadFunc, this);
	std::unique_lock<std::mutex> locker(mutex_);
	cond_.wait(locker, [=]() {return loop_ != NULL;});
	return loop_;
}

void WebSocketEventLoopThreadImpl::threadFunc(void* arg)
{
	WebSocketEventLoop loop;
	WebSocketEventLoopThreadImpl* ptr = (WebSocketEventLoopThreadImpl*)arg;
	ptr->loop_ = &loop;
	ptr->cond_.notify_one();
	ptr->loop_->loop();
	ptr->loop_ = NULL;
}