#pragma once
#include <iostream>
#include <cstdio>
#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <assert.h>

#define CHECK(exp) \
	if(!exp) \
{ \
	fprintf(stderr, "File:%s, Line:%d Exp:[" #exp "] is true, abort.\n",__FILE__, __LINE__); abort();\
}

class CMutexLock : public boost::noncopyable
{
	//条件变量友元声明
	friend class CCond;
public:
	CMutexLock();
	~CMutexLock();
	//上锁
	void Lock();
	//解锁
	void Unlock();
	//尝试上锁
	bool IsLocking() const { return is_locking; }
	//得到线程锁
	pthread_mutex_t* GetMutexPtr() { return &mutex; }
private:
	void RestoreMutexStatus() { is_locking = true; }
	pthread_mutex_t mutex;//互斥锁
	bool is_locking;      //判断锁的状态
};

/*
	类说明：
		对MutexLock初始化和析构进行处理
		初始化的时候加锁
		析构的时候解锁
*/
class CMutexLockGuard :public boost::noncopyable
{
public:
	//构造时加锁
	CMutexLockGuard(CMutexLock& mutex) :mutex(mutex) { mutex.Lock(); }
	//析构时解锁
	~CMutexLockGuard() { mutex.Unlock(); }
private:
	CMutexLock& mutex;
};
