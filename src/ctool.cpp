#include "ctool.h"
#include <sys/stat.h>
#include <unistd.h>

/************************************
	函数说明:初始化构造函数,无需使用
	函数输入:无
	函数输出:无
*************************************/
CTool::CTool()
{
}

/**********************************************
	函数功能:判断所输入的字符串是否是中英文字符
	函数输入:字符串
	函数输出:bool, 是则输出true, 不是则输出为false
	函数名称:judgeCNorEN(string& str)
**********************************************/
bool CTool::judgeCNorEN(string str)
{
	//基本思想: 输入一串字符串,如果是中英文字符,则删除, 最后长度为0,则返回true
	//utf-8情况下一个中文字符占3位

	int len = (int)str.length();//字符串长度
	int i = 0;             //遍历的变量
	int num = 0;           //记录删除的字符数
	char key, key2;       //记录字符的信息
	string word;           //判断一个字符

	//先匹配中文字符的个数
	while (i < len)
	{
		key = str.c_str()[i];
		key2 = str.c_str()[i + 1];
		//判断是否是中文字符
		if ((unsigned char)key >= 0Xa1 && (unsigned char)key2 <= 0XFE)
		{
			if ((unsigned char)key >= 0Xa1 && (unsigned char)key2 <= 0XFE)
			{
				num += 3; //记录字符数为2
			}
		}
		i = i + 3;
	}

	//在查询所有的英文字符的个数
	i = 0;
	while (i < len)
	{
		if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z'))
		{
			num++;
		}
		i++;
	}

	//相等则为true, 否则返回false
	return (len == num);
}

/********************************************
	函数说明:char类型转成int类型
	函数输入:const char*类型字符串(可直接赋值)
	函数输出:int类型
*********************************************/
int CTool::char2int(const char* str)
{
	const char* p = str;
	int res = 0;
	if (*str == '-' || *str == '+')
	{
		str++;
	}

	//循环转换
	while (*str != 0)
	{
		if (*str < '0' || *str > '9')
		{
			break;
		}
		res = res * 10 + *str - '0';
		str++;
	}

	if (*p == '-')
	{
		res = -res;
	}
	return res;
}

int32_t CTool::TimerStart(uint64_t interval_ms)
{
	{
		int32_t timerfd = 0;
		struct itimerspec its = { 0 };
		struct itimerspec itsTest = { 0 };

		timerfd = timerfd_create(CLOCK_MONOTONIC, 0);
		if (timerfd < 0)
		{
			return -1;
		}

		/* Start the timer */
		its.it_value.tv_sec = interval_ms / 1000;
		its.it_value.tv_nsec = (interval_ms % 1000) * 1000000;
		its.it_interval = its.it_value;

		// if (timerfd_settime(timerfd, 0, &its, NULL) < 0)
		if (timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &its, NULL) < 0)
		{
			close(timerfd);
			return -1;
		}
		return timerfd;
	}
}