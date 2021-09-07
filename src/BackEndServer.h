#pragma once
//后置服务器--思路: 从共享内存读取一块数据出来,读到数据后添加到任务队列里面,线程池调取任务队列进行操作
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include "CThreadPool.h"
#include "BackTask.h"

class BackEndServer
{
public:
	//构造函数
	BackEndServer();//后置服务器
	//析构函数
	~BackEndServer();
	//运行函数
	void Run();
private:
	CThreadPool* thread_pool;          //线程池类指针
};
