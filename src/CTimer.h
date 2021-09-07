#pragma once
#include <map>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

//定义当前时间的数据类型
typedef long time_value;

//定义时间事件结构体
struct timer_event
{
	time_value times;
	void* arg;
	int event_type;
};

time_value getNow();

class CTimer {
	static const int64_t min_timer = 1000 * 1;

public:
	CTimer() {}

	virtual ~CTimer() {}

	int AddTimer(void*, time_value);

	void* GetTimer(time_value tnow);

	void RemoteTimer(void*);

	time_value GetMinTimer() const;

	int GetArgTimeSize() const { return (int)timer_arg_time.size(); }
	int GetTimeArgSize() const { return (int)timer_time_arg.size(); }

	void Show() const;

private:
	std::multimap<void*, time_value> timer_arg_time;
	std::multimap<time_value, void*> timer_time_arg;
};
