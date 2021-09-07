#pragma once
#include <iostream>
#include <fstream>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "CMutexLock.h"

typedef struct log_info
{
	const char* fun;
	const char* type;
	const char* user_id;
	std::string data;
}t_log_info;

class CLogger
{
public:
	//构造函数
	CLogger(const std::string& path);

	//析构函数
	~CLogger();

	//创建日志文件
	bool CreatLogFile();

	//结束日志文件
	bool EndLogFile();

	//修改日志文件
	bool ModiLogFile();

	//获取当前时间
	std::string GetCurrTime();
	//纯获取当前时间
	char* GetTime();

	//设置当前的时间
	void SetCurrTime();

	//输出正常的文本到文件里面
	void WriteLog(const char* content);
	void WriteLog(const char* content1, const char* content2);

	//输出报错的文本信息到文件里面
	void ErrMsgWriteLog(const char* content, bool no_erro);
	void ErrMsgWriteLog(const char* content);
	void ErrMsgWriteLog(const char* content1, const char* content2);

private:
	std::ofstream m_outfile;                                    //将日志输出到文件的流对象
	std::string m_path;                                         //日志文件路径

	int flag_day;                                               //用来存储当前的天数
	int flag_mon;                                               //用来存储当前的月数
	int flag_year;                                              //用来存储当前的年数

	time_t ptime;                                               //获取当前时间，并规范表示
	struct tm* tim;                                             //时间接收结构体

	CMutexLock* mutex;                                           //线程锁
};