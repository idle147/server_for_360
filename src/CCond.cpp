#include "CCond.h"

CCond::CCond(CMutexLock& new_mutex) :mutex(new_mutex)
{
	pthread_cond_init(&cond, NULL);
}

CCond::~CCond()
{
	pthread_cond_destroy(&cond);
}

bool CCond::Wait()
{
	int rs = 0;
	if (mutex.is_locking == false)
	{
		//如果没上锁,则上锁
		mutex.Lock();
	}
	rs = pthread_cond_wait(&cond, mutex.GetMutexPtr());
	//解锁
	mutex.Unlock();
	return rs == 0 ? true : false;
}

bool CCond::WaitForSecond(int second)
{
	struct timespec timeout {};
	clock_getres(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += second;
	return pthread_cond_timedwait(&cond, mutex.GetMutexPtr(), &timeout) == ETIMEDOUT;
}