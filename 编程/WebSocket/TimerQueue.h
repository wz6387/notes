#ifndef _TIMER_QUEUE_H_
#define _TIMER_QUEUE_H_

#include <set>
#include <mutex>
#include "Callbacks.h"
#include "TimerId.h"

class Timer;
class TimerQueue
{
public:
	TimerQueue();
	~TimerQueue();
	TimeId addTimer(TimerCallback cb, int delay);
	void cancel(TimeId timerId);
private:
	void removeExpiredTimer();
	std::set<Timer*> activeTimers_;
	std::mutex mutex_;
};

#endif
