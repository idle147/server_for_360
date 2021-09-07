#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "CMutexLock.h"

/*

	消息队列的特点
		1、异步通信，消息队列会保存进程发送的消息，其他进程不一定要及时取走消息。
		2、可以发送不同类型的消息，消息的头部用long类型的字段标记。
		3、取消息时，不一定按先进先出的方式，可以按消息的类型来取。
		4、消息队列内部是用一个链表来保存消息，当消息被取走后，这个消息就从链表中删除了，
		   而且这个链表的长度是有限制的，当消息存满后，不能再存入消息。

	消息队列的使用
		1.异步处理:例如短信通知、终端状态推送、App推送、用户注册等
		2.数据同步:业务数据推送同步
		3.重试补偿:记账失败重试
		4.系统解耦:通讯上下行、终端异常监控、分布式事件中心
		5.流量消峰：秒杀场景下的下单处理
		6.发布订阅：HSF的服务状态变化通知、分布式事件中心
		7.数据流处理：日志服务、监控上报
		8.分布式事务

*/
class CMsgQueue
{
public:
	CMsgQueue(int key);
	virtual ~CMsgQueue();
	//消息队列初始化
	int Init(char* pathName);
	int Init(int key);
	//消息队列收发
	bool ReceiveMsg(void* msg, size_t n_bytes, long type, int flag);
	bool SendMsg(const void* msg, size_t n_bytes, int flag);
	//消息队列销毁
	bool Destroy();
	//获得消息队列ID
	int GetMsgId() { return this->msg_queue_id; };
	//获得消息队列关键字
	key_t GetMsgKey() { return this->msg_key; };
private:
	//消息队列ID
	int msg_queue_id;
	//消息队列关键字
	key_t msg_key;
	//消息队列锁
	CMutexLock* mutex;
};
