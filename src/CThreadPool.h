#pragma once
#include <pthread.h>
#include <vector>
#include <string>
#include "CCond.h"
#include "CTask.h"
using namespace std;

/*
	线程池类
*/
class CThreadPool
{
public:
	CThreadPool(int maxCount);
	~CThreadPool();
	void AddTask(CTaskBase* pTask); //把任务添加到任务队列中
	int StopAll();                  //退出所有的线程
	CTaskBase* GetTask();           //从任务池内得到一个任务
	int GetTaskSize();              //获取当前任务队列中的任务数

protected:
	int CreateThread();                   //创建线程池中的线程
	static void* ThreadFun(void* arg);    //新线程的线程回调函数

	//未用到,未实现
	//static int MoveToIdle(pthread_t tid); //线程执行结束后,进入空闲线程
	//static int MoveToBusy(pthread_t tid); //移入到忙碌线程中

private:
	int maxCount;//最大线程数
	CMutexLock* m_mutex;//线程同步锁
	CCond* m_cond;//线程状态条件变量
	bool m_isQuit;//判断退出标识符
	pthread_t* pthreadId; //线程ID
	vector<CTaskBase*>m_taskVec;//任务列表
};
