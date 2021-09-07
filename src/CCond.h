#pragma once
#include "CMutexLock.h"

class CCond :public boost::noncopyable
{
public:
	//构造和析构函数
	CCond(CMutexLock& new_mutex);
	~CCond();

	//封装pthread_cond_wait
	bool Wait();

	//等待固定时间
	bool WaitForSecond(int second);

	//封装pthread_cond_signal
	bool Notify() {
		return pthread_cond_signal(&cond) == 0 ? true : false;
	}

	//封装pthread_cond_broadcast
	bool NotifyAll() {
		return pthread_cond_broadcast(&cond);
	}

private:
	CMutexLock& mutex;
	pthread_cond_t cond;
};
