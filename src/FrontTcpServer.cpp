#include "FrontTcpServer.h"

//前置服务器
TcpServer::TcpServer() :CEpollBase(DOC_GET_INT("server_set", "max_connectNum"), DOC_GET_INT("server_set", "max_threadNum"))
{
	//参数数值初始化
	client_addr_len = sizeof(struct sockaddr_in);
	client_fd = 0;
	tcp_Task = nullptr;
	//设置相应的地址信息
	this->server_socket = new CServerSocket();
	journal->WriteLog("[前置服务器]套接字设置完毕!");

	//创建线程池
	thread_pool = new CThreadPool(DOC_GET_INT("server_set", "max_threadNum"));
	journal->WriteLog("[前置服务器]线程池创建完毕!");

	//心跳包
	this->heart_beat = CSingleton<CHeartbeat>::GetInstance().get();
	journal->WriteLog("[前置服务器]心跳功能初始化完毕!");

	//发送线程初始化
	this->send_Info = new CServerSend(this->thread_pool);
	journal->WriteLog("[发送监听线程]启动完毕!");

	//套接字加入epoll,并进行监听
	journal->WriteLog("[前置服务器]正在等待客户端连接…");
	this->AddEpoll(server_socket->GetsocketFd());
}

TcpServer::~TcpServer()
{
	delete thread_pool;
	thread_pool = nullptr;
	delete server_socket;
	server_socket = nullptr;
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  NewConnection(int clt_fd)
// @函数说明:  纯虚函数实现, 收到新连接后的操作
// @参数说明:  参数1: socket套接字标识符
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void TcpServer::NewConnection(int clt_fd)
{
	//accpet 返回已连接套接字
	client_fd = accept(clt_fd, (struct sockaddr*)&client_addr, &client_addr_len);
	if (client_fd == -1)
	{
		journal->ErrMsgWriteLog("[前置服务器]accept函数接受新客户端出错");
		return;
	}
	//将连接的客户端网络添加到epoll中
	DoEvent(client_fd, EPOLL_CTL_ADD, EPOLLIN | EPOLLET);

	//讲套接字加入fd链表中
	all_fd.push_back(client_fd);
	char str_temp[256] = { '\0' };
	sprintf(str_temp, "[前置服务器]客户端%s:%d, 连接成功!加入套接字地址池", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	journal->WriteLog(str_temp);

	//执行心跳操作
	heart_beat->SetTcpKeepAlive(client_fd);

	//超时设置
	heart_beat->ReSendAndRecvPack(client_fd, 100);
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  exitsConnection(long socket_fd)
// @函数说明:  纯虚函数实现, 处理已经存在的连接
// @参数说明:  参数1: socket套接字标识符
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void TcpServer::ExitsConnection(int socket_fd_num, unsigned int* pack_num)
{
	tcp_Task = new CFrontTask(all_fd, socket_fd_num, pack_num);
	//加入任务队列
	thread_pool->AddTask(tcp_Task);
}