#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include "CEpollBase.h"
#include "CThreadPool.h"
#include "CFrontTask.h"
#include "CSocket.h"
#include "CServerSend.h"
#include "Packet.h"
#include "global.h"

//前置服务器
class TcpServer : public CEpollBase
{
public:
	//构造函数
	TcpServer();    //前置服务器

	//析构函数
	~TcpServer();

	//处理新连接
	void NewConnection(int clt_fd);

	//处理已经存在的连接
	void ExitsConnection(int socket_fd_num, unsigned int* pack_num);

private:
	CFrontTask* tcp_Task;

private:

	struct sockaddr_in client_addr;    //客户端套接字地址
	socklen_t client_addr_len;         //客户端套接字地址长度
	int client_fd;                     //客户端套接字地址索引
	int* pack_num;                     //记录包号
	CHeartbeat* heart_beat;            //心跳包
	CThreadPool* thread_pool;          //线程池类指针
	CServerSocket* server_socket;      //服务器套接字类指针
	CServerSend* send_Info;            //向客户端发送信息指针
};