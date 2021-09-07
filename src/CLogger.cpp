#include "CLogger.h"

CLogger::CLogger(const std::string& path)
{
	this->m_path = path;
	this->mutex = new CMutexLock();

	//获取当前系统时间,同时赋值
	time(&ptime);

	//设置当天的天数
	this->tim = localtime(&ptime);

	this->SetCurrTime();
	//创建日志文件
	this->CreatLogFile();
}

CLogger::~CLogger()
{
	this->EndLogFile();
}

//创建日志文件
bool CLogger::CreatLogFile()
{
	std::string path = this->m_path + "/" + GetCurrTime().substr(0, 10) + ".log";
	//上锁
	this->mutex->Lock();
	m_outfile.clear();
	m_outfile.open(path, std::ios::out | std::ios::app);
	m_outfile << "=== 服务器日志文件 开始记录 ===\n";
	//输出日期
	std::cout << "=== 服务器日志文件 开始记录 ===\n";
	//解锁
	this->mutex->Unlock();
	return true;
}

//结束日志文件
bool CLogger::EndLogFile()
{
	//上锁
	this->mutex->Lock();
	if (m_outfile.is_open())
	{
		m_outfile << "=== 服务器日志文件 结束记录 ===\r\n";
	}
	m_outfile.flush();
	m_outfile.close();
	//解锁
	this->mutex->Unlock();
	return true;
}

//创建一个新的日志文件
bool CLogger::ModiLogFile()
{
	//上锁
	this->mutex->Lock();
	EndLogFile();
	CreatLogFile();
	//解锁
	this->mutex->Unlock();
	return false;
}

//获取当前的时间
std::string CLogger::GetCurrTime()
{
	//上锁
	this->mutex->Lock();
	//获取当前时间，并规范表示
	char tmp[30];

	//获取本地时间
	struct tm* tim = localtime(&ptime);
	int sec = tim->tm_sec;
	int min = tim->tm_min;
	int hour = tim->tm_hour;
	int mon = tim->tm_mon + 1;
	int mday = tim->tm_mday;
	int year = tim->tm_year + 1900;

	//如果是新的一天, 则保存新的文件
	if (this->flag_day != mday || this->flag_mon != mon || this->flag_year != year)
	{
		ModiLogFile();
	}

	//存储的数值进行更新
	this->SetCurrTime();

	//格式化输出字符串
	sprintf(tmp, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, mday, hour, min, sec);
	//解锁
	this->mutex->Unlock();
	return tmp;
}

char* CLogger::GetTime()
{
	//获取当前时间，并规范表示
	char tmp[30];

	//获取本地时间
	struct tm* tim = localtime(&ptime);
	int sec = tim->tm_sec;
	int min = tim->tm_min;
	int hour = tim->tm_hour;
	int mon = tim->tm_mon + 1;
	int mday = tim->tm_mday;
	int year = tim->tm_year + 1900;
	//格式化输出字符串
	sprintf(tmp, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, mday, hour, min, sec);
	return tmp;
}

//设置当前时间
void CLogger::SetCurrTime()
{
	//设置相关的数值
	this->flag_day = tim->tm_mday;
	this->flag_mon = tim->tm_mon + 1;
	this->flag_year = tim->tm_year + 1900;
}

//写入结构体函数
void CLogger::WriteLog(const char* content)
{
	std::string outputContent = "[Info][时间:" + GetCurrTime() + "]" + content + "\n";
	//上锁
	this->mutex->Lock();
	//输出到终端, 输出到文件当中
	std::cout << outputContent;
	m_outfile << outputContent;
	//刷新缓冲区
	m_outfile.flush();
	//解锁
	this->mutex->Unlock();
}

void CLogger::WriteLog(const char* content1, const char* content2)
{
	std::string outputContent = "[Info][时间:" + GetCurrTime() + "]" + content1 + content2 + "\n";
	//上锁
	this->mutex->Lock();
	//输出到终端, 输出到文件当中
	std::cout << outputContent;
	m_outfile << outputContent;
	//刷新缓冲区
	m_outfile.flush();
	//解锁
	this->mutex->Unlock();
}

void CLogger::ErrMsgWriteLog(const char* content)
{
	std::string outputContent = "[Error][时间:" + GetCurrTime() + "]" + content +
		":错误码[" + strerror(errno) + "]\n";
	this->mutex->Lock();
	//输出到终端, 输出到文件当中
	std::cout << outputContent;
	m_outfile << outputContent;
	//刷新缓冲区
	m_outfile.flush();
	//解锁
	this->mutex->Unlock();
}

void CLogger::ErrMsgWriteLog(const char* content1, const char* content2)
{
	std::string outputContent = "[Error][时间:" + GetCurrTime() + "]" + content1 + content2 +
		":错误码[" + strerror(errno) + "]\n";
	this->mutex->Lock();
	//输出到终端, 输出到文件当中
	std::cout << outputContent;
	m_outfile << outputContent;
	//刷新缓冲区
	m_outfile.flush();
	//解锁
	this->mutex->Unlock();
}

void CLogger::ErrMsgWriteLog(const char* content, bool no_erro)
{
	std::string outputContent = "[Error][时间:" + GetCurrTime() + "]" + content + "\n";
	this->mutex->Lock();
	//输出到终端, 输出到文件当中
	std::cout << outputContent;
	m_outfile << outputContent;
	//刷新缓冲区
	m_outfile.flush();
	//解锁
	this->mutex->Unlock();
}