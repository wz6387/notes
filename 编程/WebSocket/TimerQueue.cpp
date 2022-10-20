#include "TimerQueue.h"
#include "Timer.h"


TimerQueue::TimerQueue()
{

}

TimerQueue::~TimerQueue()
{
	std::lock_guard<std::mutex> lock(mutex_);
	for (auto &pt : activeTimers_) {
		delete pt;
	}
}

void TimerQueue::removeExpiredTimer()
{
	std::lock_guard<std::mutex> lock(mutex_);
	for (auto it = activeTimers_.begin(); it != activeTimers_.end();) {
		if ((*it)->isExpired()) {
			delete *it;
			activeTimers_.erase(it++);
		} else {
			++it;
		}
	}
}

TimeId TimerQueue::addTimer(TimerCallback cb, int delay)
{
	removeExpiredTimer();
	Timer* pt = new Timer();
	pt->runAfter(cb, delay);
	std::lock_guard<std::mutex> lock(mutex_);
	activeTimers_.insert(pt);
	return TimeId(pt);
}

void TimerQueue::cancel(TimeId timerId)
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = activeTimers_.find(timerId.timer_);
	if (it != activeTimers_.end()) {
		delete *it;
		activeTimers_.erase(it);
	}
}