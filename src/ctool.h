#ifndef CTOOL_H
#define CTOOL_H
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/timerfd.h>
#include "global.h"
using namespace std;

class CTool
{
public:
	CTool();
	static bool judgeCNorEN(string str); //判断是否只包含中文和英文
	static int char2int(const char* str); //char*转int
	static int32_t TimerStart(uint64_t interval_ms);//创建定时器
};

template <class T>
//获取字符串数组的长度
int getArrayLen(T& array)
{
	return (sizeof(array) / sizeof(array[0] - 1)); // 存储字符串的字符数组末尾有一个'\0'字符，需要去掉它
}

#endif // CTOOL_H
