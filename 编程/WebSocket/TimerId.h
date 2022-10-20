#ifndef _TIMER_ID_H_
#define _TIMER_ID_H_

#ifdef WIN32
#ifdef __DLL_EXPORTS__
#define DLLAPI  __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif
#else
#define DLLAPI  __attribute__ ((visibility("default")))
#endif

class Timer;
class DLLAPI TimeId
{
public:
	TimeId()
		: timer_(nullptr)
	{
	}
	TimeId(Timer* timer)
		: timer_(timer)
	{
	}
	bool isEmpty() { return timer_ == nullptr; }
	friend class TimerQueue;
	
private:
	Timer* timer_;
	
};

#endif
