#pragma once
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "CServerJson.h"
#include "CSigleton.h"
#include "global.h"

using namespace std;

class CHeartbeat
{
public:
	//初始化相关参数
	CHeartbeat();
	//设置TCP保活, 网络连接描述符
	int SetTcpKeepAlive(int fd);
	//在send(),recv()过程中有时由于网络状况等原因，发收不能预期进行,而设置收发时限
	int ReSendAndRecvPack(int fd, int time);
	//TCP四次挥手关闭套接字连接
	int CloseSocket(int fd);

private:
	int tcp_keepalive_intvl;   //保活探测消息的发送频率。
	int tcp_keepalive_probes;  //TCP发送保活探测消息以确定连接是否已断开的次数。
	int tcp_keepalive_time;    //允许的持续空闲时间。
	int tcp_keepalive_on;      //是否启动保活机制
};
