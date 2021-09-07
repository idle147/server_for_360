/*******************************************************************************
类名总结：CEpollBase 类
文件名称：CEpollBase.h
功能说明：线程池的封装操作
文件时间：2021/1/20/
*******************************************************************************/
#pragma once
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <list>
#include <cstring>
#include <iostream>
#include "global.h"
using namespace std;

class CEpollBase
{
public:
	//构造与析构函数,创建epoll
	CEpollBase(int max_connect_num, int epoll_event_num);
	CEpollBase(int max_connect_num);
	~CEpollBase();
	//添加套接字到epoll并侦听
	void AddEpoll(int server_fd);
	//设置非阻塞监听
	int SetNonBlocking(int fd);
	//处理新连接
	virtual void NewConnection(int client_fd) = 0;
	//处理已经存在的连接
	virtual void ExitsConnection(int socket_fd, unsigned int* pack_num) = 0;
	//添加、删除、修改事件到epoll
	void DoEvent(int listen_fd, int op, int state);
	//获取epollfd标识符
	int GetEpollFd() { return epoll_fd; }
	//获取所有标识符
	list<int>& GetAllFd() { return all_fd; }

protected:
	unsigned int* pack_num;      //定义包的编号
	int epoll_fd;       //定义epoll标识符
	int epoll_event_num;//定义epoll的最大事件数
	struct epoll_event* events;//定义接收的最大事件
	list<int>all_fd;    //链表存储所有的epoll标识符
};
