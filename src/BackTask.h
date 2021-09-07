#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "CTask.h"
#include "CFileOpt.h"
#include "CFileTable.h"
#include "md5.h"

using namespace std;
/*
	线程任务--后端服务器任务--登陆注册
*/
class CLoginAndRegTask : public CTaskBase
{
public:
	//后置服务器
	CLoginAndRegTask(bool isLogin, msg_t buf);
	~CLoginAndRegTask();
	void Run();

private:
	void ResJudege(int res);
	t_Register regMsg;        //注册结构体
private:
	CLoginAndReg* log_and_Reg;    //登陆和注册指针
	msg_t msg_info;               //接发送消息的结构体
	bool isLogin;                 //判断是否是登陆
};

/*
	线程任务--后端服务器,获取视频信息任务
*/
class CGetVideoFrame :public CTaskBase
{
public:
	//后置服务器
	CGetVideoFrame(msg_t buf);
	~CGetVideoFrame();
	void Run();

private:
	msg_t msg_info;               //接发送消息的结构体
	CVideoInfo* video_info;       //视频信息指针
	t_RequireProgress reqPro;    //请求获取信息结构体
};

/*
	线程任务--后端服务器,删除视频信息任务
*/
class CDeleteVideoFrame :public CTaskBase
{
public:
	//后置服务器
	CDeleteVideoFrame(msg_t buf);
	~CDeleteVideoFrame();
	void Run();

private:
	msg_t msg_info;               //接发送消息的结构体

	CVideoInfo* video_info;       //视频信息指针
	t_RequireProgress reqPro;    //请求获取信息结构体
};

/*
	线程任务--后端服务器,插入视频信息
*/
class CInsertVideo :public CTaskBase
{
public:
	//后置服务器
	CInsertVideo(msg_t buf);
	~CInsertVideo();
	void Run();

private:
	msg_t msg_info;               //接发送消息的结构体

	CVideoInfo* video_info;       //视频信息指针
	t_VideoInfo t_video;         //请求获取信息结构体
};

/*
	线程任务--后端服务器,片有包任务
*/
class CFileTask :public CTaskBase
{
public:
	//片有包
	CFileTask(string m_file_path, bool m_end, msg_t buf);
	~CFileTask();
	bool WriteInfo();
	void reqResend(int i);
	bool MergeFile();
	void Run();

private:
	string file_path;             //文件路径
	bool isEnd;                   //是否是尾包
	t_FilePack file_pack;         //片有包
	msg_t msg_info;               //接发送消息的结构体
};

/*
	线程任务--后端服务器,第一个包任务
*/
class CFileFirstTask :public CTaskBase
{
public:
	//片有包
	CFileFirstTask(string m_file_path, msg_t buf);
	~CFileFirstTask();
	void Run();

private:
	t_FirstFilePack first_file_pack;   //文件第一个包
	msg_t msg_info;                    //接发送消息的结构体
	string file_path;
};
