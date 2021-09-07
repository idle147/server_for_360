#include "CHeartbeat.h"

CHeartbeat::CHeartbeat()
{
	//如果您想关闭，将keepAlive置零即可
	this->tcp_keepalive_on = DOC_GET_INT("tcp_keepalive", "tcp_keepalive_on");
	//保活探测消息的发送频率
	this->tcp_keepalive_intvl = DOC_GET_INT("tcp_keepalive", "tcp_keepalive_intvl");
	//TCP发送保活探测消息以确定连接是否已断开的次数。
	this->tcp_keepalive_probes = DOC_GET_INT("tcp_keepalive", "tcp_keepalive_probes");
	//启用心跳机制开始到首次心跳侦测包发送之间的空闲时间
	this->tcp_keepalive_time = DOC_GET_INT("tcp_keepalive", "tcp_keepalive_time");
}

int CHeartbeat::SetTcpKeepAlive(int fd)
{
	//入口参数检查
	if (fd < 0 || tcp_keepalive_time < 0 || tcp_keepalive_intvl < 0 || tcp_keepalive_probes < 0)
		return -2;
	//启用心跳机制,
	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&tcp_keepalive_on, sizeof(tcp_keepalive_on)) == -1)
	{
		journal->ErrMsgWriteLog("setsockopt心跳包启动失败");
		return -1;
	}
	//探测发包间隔
	if (setsockopt(fd, SOL_TCP, TCP_KEEPINTVL,
		&tcp_keepalive_intvl, sizeof(tcp_keepalive_intvl)) < 0) {
		journal->ErrMsgWriteLog("setsockopt心跳包\"保活时间信息\"初始化失败, 退出程序");
		system("pause");
		exit(-1);
	}
	//探测次数，即将几次探测失败判定为TCP断开
	//尝试探测的次数.如果第1次探测包就收到响应了,则后2次的不再发
	if (setsockopt(fd, SOL_TCP, TCP_KEEPCNT,
		&tcp_keepalive_probes, sizeof(tcp_keepalive_probes)) < 0) {
		journal->ErrMsgWriteLog("setsockopt心跳包\"断开次数信息\"初始化失败, 退出程序");
		system("pause");
		exit(-1);
	}
	//发送心跳包之间的空闲时间
	//如该连接在X秒内没有任何数据往来,则进行此TCP层的探测
	if (setsockopt(fd, SOL_TCP, TCP_KEEPIDLE,
		&tcp_keepalive_time, sizeof(tcp_keepalive_time)) < 0) {
		journal->ErrMsgWriteLog("setsockopt心跳包\"空闲时间信息\"初始化失败, 退出程序");
		system("pause");
		exit(-1);
	}
	return 0;
}

int CHeartbeat::ReSendAndRecvPack(int fd, int time)
{
	struct timeval tv_timeout;
	tv_timeout.tv_sec = time;
	tv_timeout.tv_usec = 0;
	//发送时限
	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv_timeout, sizeof(int));
	//接收时限
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv_timeout, sizeof(int));
	return 0;
}

int CHeartbeat::CloseSocket(int fd)
{
	bool b_reuse_addr = true;
	//一般不会立即关闭而经历TIME_WAIT的过程
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&b_reuse_addr, sizeof(bool));
	return 0;
}