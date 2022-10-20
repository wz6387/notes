#ifndef _TIME_H_
#define _TIME_H_

#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "Callbacks.h"

class Timer
{
public:
	Timer();
	~Timer();
	void runAfter(TimerCallback cb, int delay);
	void cancel();
	bool isExpired() { return expired_; }

private:
	void deleteThread();
	std::thread* thread_;
	std::mutex mutex_;
	std::condition_variable_any threadcond_;
	std::atomic_bool expired_;//任务过期
	std::atomic_bool tryexpired_;//使任务过期
	
};
#endif
