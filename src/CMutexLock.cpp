#include "CMutexLock.h"

CMutexLock::CMutexLock()
{
	//判断是否上锁,未上锁则加锁
	if (IsLocking() == false)
	{
		//创建锁
		CHECK(!pthread_mutex_init(&mutex, NULL));
	}
}

CMutexLock::~CMutexLock()
{
	assert(!IsLocking());
	//销毁锁
	CHECK(!pthread_mutex_destroy(&mutex));
}

void CMutexLock::Lock()
{
	//先加锁再修改状态，保证以下赋值操作的原子性。
	CHECK(!pthread_mutex_lock(&mutex));
	is_locking = true;
}

void CMutexLock::Unlock()
{
	//先修改状态在解锁，保证赋值操作的原子性。
	is_locking = false;
	CHECK(!pthread_mutex_unlock(&mutex));
}