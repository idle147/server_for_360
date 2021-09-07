#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "global.h"
#include "CThreadPool.h"
#include "Packet.h"
#include "CSendTask.h"
class CServerSend
{
public:
	CServerSend(CThreadPool* m_thread_pool);
	~CServerSend();
private:
	static void* thfunc(void* arg); //线程监听函数
	void CreatThead();
	CThreadPool* thread_pool;
};
