#pragma once
#include "CTask.h"

/*
	线程任务--前段服务器
*/
class CFrontTask : public CTaskBase
{
public:
	//前置服务器--无需数据库操作
	CFrontTask(list<int>& all_fd, int fd, unsigned int* pack_num);
	~CFrontTask();
	void Run();
private:
	//共享内存和消息队列的操作
	void TaskSendMsg(void* buf, int buf_len, int type, int srcSocket);
	//仅消息队列操作
	void TaskSendMsg(int type, int srcSocket, int index);
	//文件操作
	void FileSegOpt(t_FilePack* filePack, int socketfd, int type);
	//CRC校验
	bool CrcCheck(unsigned char* str, int len, int srcSocket, int resent_type);

private:
	unsigned int* pack_num;
	int srcSocket;                       //套接字标识符
	list<int>allFd;                      //链表存储所有的epoll标识符
	struct sockaddr_in peer_addr;        //监听的客户端地址
	t_PackHead* pack_head = nullptr;     //结构体包头
};
