#ifndef _WORKING_QUEUE_H_
#define _WORKING_QUEUE_H_

#include <queue>
#include <mutex>
#include <condition_variable>
template<typename T>
class WorkingQueue
{
public:
	WorkingQueue() : close_(false) {}
	~WorkingQueue(){}

	void put(const T& task) {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			queue_.push(task);
		}
		cond_.notify_all();
	}

	bool get(T& data) {
		std::unique_lock<std::mutex> lock(mutex_);
		//pred为false的时候阻塞；收到通知并且pred为true时解除阻塞
		cond_.wait(lock, [this]() {return !queue_.empty() || close_;});
		if (close_) {
			return false;
		}
		data = queue_.front();
		queue_.pop();
		return true;
	}

	bool empty() {
		std::lock_guard<std::mutex> lock(mutex_);
		return queue_.empty();
	}

	size_t size() {
		std::lock_guard<std::mutex> lock(mutex_);
		return queue_.size();
	}

	void close() {
		close_ = true;
		cond_.notify_all();
	}

private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
	bool close_;
};


#endif
