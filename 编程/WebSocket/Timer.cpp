#include "Timer.h"
#include <chrono>

Timer::Timer()
	: thread_(NULL)
	, expired_(true)
	, tryexpired_(false)
	
{

}

Timer::~Timer()
{
	tryexpired_ = true;
	deleteThread();
}

void Timer::runAfter(TimerCallback cb, int delay)
{
	//当前有任务或任务已过期，返回
	if (!expired_ || tryexpired_) {
		return;
	}
	expired_ = false;//任务在进行中

	deleteThread();
	thread_ = new std::thread([this, delay, cb]() {
		std::unique_lock<std::mutex> lck(mutex_);
		threadcond_.wait_for(lck, std::chrono::milliseconds(delay));
		if (!tryexpired_) {
			cb();
		}
		tryexpired_ = false;//用于加载下一次任务
		expired_ = true;//当前任务结束
	});
}

void Timer::deleteThread()
{
	if (thread_) {
		threadcond_.notify_all();
		thread_->join();
		delete thread_;
		thread_ = NULL;
	}
}

void Timer::cancel()
{
	if (expired_ || tryexpired_ || !thread_) {
		return;
	}
	tryexpired_ = true;
}