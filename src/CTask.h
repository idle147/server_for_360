#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <list>
#include <iostream>
#include <sys/syscall.h>
#include "global.h"
#include "Packet.h"
#include "CLoginAndReg.h"
#include "CHeartbeat.h"
#include "CShareMemory.h"
#include "CMsgQueue.h"
#include "CVideoInfo.h"
#include "CCrc.h"

/*
	线程任务父类
*/
class CTaskBase
{
public:
	//构造函数任务参数指针初始化
	CTaskBase() {};
	CTaskBase(void* arg) { this->ptrData = arg; }
	virtual ~CTaskBase() {};
	//设置任务数据
	void SetData(void* data) { this->ptrData = data; }
	virtual void Run() = 0;
protected:
	void* ptrData;      //执行任务的具体参数
};