#include "CEpollBase.h"

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  CEpollBase(int maxConnectNum, int epollEventNum)
// @函数说明:  构造函数,创建epoll和相关数值初始化
// @参数说明:  参数1(int)最大连接数， 参数2(int)epoll事件数
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
CEpollBase::CEpollBase(int maxConnectNum, int epollEventNum)
{
	//创建epoll
	this->epoll_fd = epoll_create(maxConnectNum);
	//每个连接创建一个标识符
	this->pack_num = new unsigned int[maxConnectNum * 5];
	for (int i = 0; i <= maxConnectNum; i++)
	{
		pack_num[i] = 1;
	}
	//数值初始化
	this->epoll_event_num = epollEventNum;
	//新建epoll事件结构体
	this->events = new struct epoll_event[this->epoll_event_num];
	if (events == nullptr)
	{
		journal->ErrMsgWriteLog("epoll数组开辟失败");
	}
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  CEpollBase(int maxConnectNum)
// @函数说明:  构造函数,创建epoll和相关数值初始化
//             epoll事件为最大连接数
// @参数说明:  (int)最大连接数
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
CEpollBase::CEpollBase(int maxConnectNum)
{
	//创建epoll
	this->epoll_fd = epoll_create(maxConnectNum);
	this->epoll_event_num = maxConnectNum;
	this->events = new struct epoll_event[this->epoll_event_num];
	if (events == nullptr)
	{
		journal->ErrMsgWriteLog("epoll数组开辟失败");
	}
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  ~CEpollBase()
// @函数说明:  释放epoll事件结构体对象
// @参数说明:  无
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
CEpollBase::~CEpollBase()
{
	if (events != nullptr)
	{
		delete[]this->events;
		events = nullptr;
	}
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  CEpollBase::DoEvent(int listenFd, int op, int state)
// @函数说明:  执行epoll事件
// @参数说明:  参数1:socket套接字地址  | 参数2:执行的epoll操作 | 参数3: epoll事件类型
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void CEpollBase::DoEvent(int listenFd, int op, int state)
{
	//初始化epoll事件
	struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));

	//epoll事件类型events是epoll注册的事件，比如EPOLLIN、EPOLLOUT等等，
	//目的:这个参数在epoll_ctl注册事件时，可以明确告知注册事件的类型。
	ev.events = state;

	//绑定该事件(data是一个联合体),起到一个传递句柄的作用
	//目的:a.可以得知, 等到的事件是不是我们想要的socket产生的
	//     b.帮助我们管理不同的网络连接---建立不同的socket管理不同的IP地址字段
	ev.data.fd = listenFd;

	//该函数用于控制某个epoll文件描述符上的事件，可以注册事件，修改事件，删除事件。
	int res = epoll_ctl(this->epoll_fd, op, listenFd, &ev);
	if (res == -1)
	{
		journal->ErrMsgWriteLog("epoll操作失败");
	}
}

//= = = = = = = = = = = = = = = = = = = =
// @函数名称:  AddEpoll(int listenFd)
// @函数说明:  添加套接字到epoll并侦听
// @参数说明:  参数1: 服务器的套接字标识符
// @返回值:    无
//= = = = = = = = = = = = = = = = = = = =
void CEpollBase::AddEpoll(int server_fd)
{
	//创建epoll事件
	//EPOLLIN：表示对应的文件描述符可以读
	//EPOLLET：将EPOLL设为边缘触发(ET模式的效率要比 LT模式高，它只支持非阻塞套接字)
	this->DoEvent(server_fd, EPOLL_CTL_ADD, EPOLLIN | EPOLLET);
	//服务器循环监听
	while (true)
	{
		//epoll进行侦听
		int nfds = epoll_wait(epoll_fd, events, epoll_event_num, -1);
		if (nfds < 0)
		{
			journal->ErrMsgWriteLog("epoll_wait发生错误");
			return;
		}
		//socket的for循环
		for (int i = 0; i < nfds; ++i)
		{
			//处理新连接, 发现有连接到该服务器的套接字
			if (events[i].data.fd == server_fd)
			{
				journal->WriteLog("发现新的连接");
				this->NewConnection(server_fd);
				journal->WriteLog("新连接添加完成");
			}
			//处理已经存在的连接
			else if (events[i].events & EPOLLIN)
			{
				journal->WriteLog("已经存在的连接发布新任务");
				this->ExitsConnection(events[i].data.fd, pack_num);
			}
		}
	}
}

//处理新连接--纯虚函数
void CEpollBase::NewConnection(int client_fd) {}
//处理已经存在的连接--纯虚函数
void CEpollBase::ExitsConnection(int socket_fd, unsigned int* pack_num) {}

//设置非阻塞监听
int CEpollBase::SetNonBlocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}