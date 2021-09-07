#include "CThreadPool.h"

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  CThreadPool(int max_count)
// @函数说明:  构造函数,创建线程池
// @参数说明:  max_count最大的线程数量
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
CThreadPool::CThreadPool(int maxCount)
{
	if (maxCount >= 200)
	{
		journal->ErrMsgWriteLog("线程池数量超过200", true);
	}
	//相关数值初始化
	this->maxCount = maxCount;
	this->m_isQuit = false;
	//创建线程同步锁
	this->m_mutex = new CMutexLock();
	//创建条件变量
	this->m_cond = new CCond(*this->m_mutex);
	//创建线程
	this->CreateThread();
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  ~CThreadPool()
// @函数说明:  析构函数, 停止所有的线程任务
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
CThreadPool::~CThreadPool()
{
	StopAll();
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  AddTask(CTaskBase* pTask)
// @函数说明:  往任务队列里添加任务并发出线程同步信号
// @参数说明:  参数1:线程的任务
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void CThreadPool::AddTask(CTaskBase* pTask)
{
	//上锁
	this->m_mutex->Lock();
	m_taskVec.push_back(pTask);
	//发出线程同步信号
	this->m_cond->Notify();
	//解锁
	this->m_mutex->Unlock();
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  StopAll()
// @函数说明:  停止所有的线程
// @参数说明:  参数1:线程的任务
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
int CThreadPool::StopAll()
{
	//避免重复调用
	if (m_isQuit)
	{
		return -1;
	}
	journal->WriteLog("开始结束所有线程\n");
	m_isQuit = true;

	//唤醒所有等待线程
	this->m_cond->NotifyAll();

	//清理僵尸线程
	for (int i = 0; i < maxCount; i++)
	{
		pthread_join(pthreadId[i], NULL);
	}

	//删除线程Id
	delete[] pthreadId;
	pthreadId = nullptr;

	//清空任务列表
	m_taskVec.clear();

	//销毁互斥锁和条件变量
	delete this->m_cond;
	this->m_cond = nullptr;
	delete this->m_mutex;
	this->m_mutex = nullptr;
	return 0;
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  GetTask()
// @函数说明:  从任务池中取出任务
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
CTaskBase* CThreadPool::GetTask()
{
	vector<CTaskBase*>::iterator iter;
	CTaskBase* task = nullptr;
	//线程上锁
	this->m_mutex->Lock();
	//如果任务列表为空,且线程处于运行状态则等待新任务进入任务队列
	//我循环任务队列, 一直循环, 直到任务队列有任务
	while (m_taskVec.empty() && !m_isQuit)
	{
		//循环等待,直到收到一个解除等待信号
		printf("没有任务,线程[tid:% lu]阻塞等待...\n", pthread_self());
		this->m_cond->Wait();
	}
	//如果处于线程处于退出状态,则退出线程
	if (m_isQuit)
	{
		this->m_mutex->Unlock();
		char str_tmp[MAX_CHAR_LEN] = { "\0" };
		sprintf(str_tmp, "[tid: %lu]\t 线程退出 \n", pthread_self());
		journal->WriteLog(str_tmp);
		system("pause");
		pthread_exit(NULL);
		exit(-1);
	}
	//从线程池取出一个任务并处理
	printf("收到任务,线程[tid: %lu]运行任务\n", pthread_self());

	//采用迭代器保证一定能够取出一个任务
	iter = m_taskVec.begin();
	if (iter != m_taskVec.end())
	{
		task = *iter;
		m_taskVec.erase(iter);
	}
	/*
	else
	{
		journal->ErrMsgWriteLog("任务队列内无任务,且无法阻塞等待, 程序退出", true);
		exit(-1);
	}
	*/
	//线程解锁
	this->m_mutex->Unlock();
	return task;
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  GetTaskSize()
// @函数说明:  获取任务的大小
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
int CThreadPool::GetTaskSize()
{
	this->m_mutex->Lock();
	int size = (int)this->m_taskVec.size();
	this->m_mutex->Unlock();
	return size;
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  ThreadFun(void* arg)
// @函数说明:  类指针,用以访问非静态成员变量
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void* CThreadPool::ThreadFun(void* arg)
{
	//thread为一个线程池的指针，指向整个线程池
	CThreadPool* thread = (CThreadPool*)arg;
	pthread_t tid = pthread_self();
	char str_tmp[MAX_CHAR_LEN];
	//如果线程池不退出
	while (!thread->m_isQuit)
	{
		//执行任务
		CTaskBase* task = thread->GetTask();
		if (task == nullptr)
		{
			memset(str_tmp, '\0', MAX_CHAR_LEN);
			sprintf(str_tmp, "任务获取出错, 线程[tid: %lu]无法执行该任务", tid);
			journal->ErrMsgWriteLog(str_tmp, true);
			break;
		}
		task->Run();
		if (task != nullptr)
		{
			delete task;
			task = nullptr;
		}
		printf("[tid: %lu]线程任务执行完毕, 空闲\n", tid);
	}
	return (void*)0;
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  CreateThread()
// @函数说明:  创建线程池中的线程
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
int CThreadPool::CreateThread()
{
	cout << "创建的最大线程数为: " << this->maxCount << endl;
	this->pthreadId = new pthread_t[this->maxCount];
	for (int i = 0; i < maxCount; i++)
	{
		pthread_create(&pthreadId[i], NULL, ThreadFun, this);
	}
	return 0;
}