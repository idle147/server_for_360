#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <regex>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string>
#include <future>
#include <cstring>
#include <netdb.h>
#include "CServerJson.h"
#include "global.h"
using namespace std;

//socket套接字基类
class CSocketBase
{
public:
	//构造函数--设置IP地址及其相关信息
	CSocketBase();
	//析构函数
	virtual ~CSocketBase();

	//判断IPv4地址的合法性
	bool JudgeIpv4();
	//判断IPv6地址的合法性
	bool JudgeIpv6();

	//查看端口是否被占用
	std::string isPortOpen(const std::string& domain, const std::string& port);
	//如果返回true，则说明，某个服务处于监听状态，同时说明这个端口号已经被这个服务所占用，
	//不能再被其他服务所用
	bool isListening(const std::string& ip, const std::string& port);
	//杀死端口
	bool killPort();

	//设置结构体信息
	void SetIpv4();  //设置ipv4协议类型
	void SetIpv6();  //设置ipv6协议类型

	//内联函数,获取相关信息
	int GetsocketFd() { return this->socket_fd; }        //获取套接字标识符
	struct sockaddr* GetSockAddr() { return (struct sockaddr*)this->this_addr; }	//获取结构体地址
	void* GetAddr() { return this->this_addr; }          //获取套接字ip地址
	int GetAddrLen() { return sizeof(&this->this_addr); }//获取套接字ip地址长度

	//判断端口地址的合法性--默认为1024--65535
	virtual bool JudgePort();
	//虚启动函数--用以重写所需协议类型(默认TCP/IPV4)
	virtual void Start();
	//纯虚运行函数--说明运行情况
	virtual void Run() = 0;
	//纯虚停止函数--说明停止运行的情况
	virtual void Stop() = 0;

protected:
	int socket_fd;           //套接字标识符
	void* this_addr;         //ip地址
};

//服务器（基于ipv4）
class CServerSocket : public CSocketBase
{
public:
	CServerSocket();
	void Run();
	void Stop();

private:
	int maxNum;
};
