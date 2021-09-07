#pragma once
#include <sys/sem.h>
#include <semaphore.h>
#include <cstdio>
#include <cerrno>
#include <string.h>
#include <iostream>
using namespace std;

/*
	PV操作:
	函数:int semop（int semid，struct sembuf  *sops，size_t nsops）;
	参数:semid为信号量集的标识符；
		 sops指向进行操作的结构体数组的首地址；
		 nsops 指出将要进行操作的信号的个数。
	返回:semop 函数调用成功返回 0，失败返回 -1
	----------------------------------------------------------------------------------------
	结构体说明struct sembuf  *sop
		 struct  sembuf
		 {
			unsigned short   sem_num;     //信号在信号集中的索引，0代表第一个信号，1代表第二个信号
			short            sem_op;      //操作类型
			short            sem_flg;     //操作标志
		  };
	----------------------------------------------------------------------------------------
	sem_op参数说明:
		sem_op > 0   信号加上 sem_op 的值，表示进程释放控制的资源；
		sem_op = 0   如果没有设置 IPC_NOWAIT，则调用进程进入睡眠状态，直到信号量的值为0；
					 否则进程不回睡眠，直接返回 EAGAIN
		sem_op < 0   信号加上 sem_op 的值。也就是需要几个信号值，若没有设置 IPC_NOWAIT ，
					 否则调用进程阻塞，直到资源可用；否则进程直接返回EAGAIN
	----------------------------------------------------------------------------------------
	sem_flg参数：该参数可设置为 IPC_NOWAIT 或 SEM_UNDO 两种状态。
		IPC_NOWAIT //对信号的操作不能满足时，semop()不会阻塞，并立即返回，同时设定错误信息。
		SEM_UNDO   //程序结束时(不论正常或不正常)，保证信号值会被重设为semop()调用前的值。
					 这样做的目的在于避免程序在异常情况下结束时未将锁定的资源解锁，造成该资源永远锁定。
	----------------------------------------------------------------------------------------
*/

/*
	类说明:system v信号量，常用于进程间的同步。
	在这个锁实现中，涉及到两个信号量：
		信号量1：表示互斥锁的信号数目。
				 每次 "获得锁"的时候，会要求该信号量为0，
				 每次 "上锁" 的时候会要求该信号量为0，并且新增一次，
				 每次 "解锁" 会减去1。

		信号量2：表示共享锁的进程数目。
				 每次 "获得锁" 会增加1，
				 每次 "上锁" 会要求该信号量为0。
				 每次 "解锁" 会减去1，

	opt数组之间都是原子操作，保证SEM_UNDO，就可以保证一致性和回滚。这是该信号锁的实现原理
*/

class CSemMutex
{
public:
	//构造函数
	CSemMutex();
	//带参数构造函数
	CSemMutex(key_t i_Key);
	//析构函数
	~CSemMutex();
	//初始化密钥信号
	void Init(key_t i_Key);

	//获取信号量的key
	key_t getkey() const { return sem_key; };
	//获取信号量ID
	int getid() const { return sem_id; };

	//上共享锁
	int ShareLock() const;
	//解共享锁
	int UnShareLock() const;
	//尝试加共享锁
	bool TryShareLock() const;

	//加互斥锁
	int MutexLock() const;
	//解互斥锁
	int UnMutexLock() const;
	//尝试互斥锁
	bool TryMutexLock() const;

protected:
	//信号量id
	int sem_id;
	//信号量key值
	key_t sem_key;
};