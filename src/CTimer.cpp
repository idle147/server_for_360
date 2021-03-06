#include "CTimer.h"

//返回值为毫秒
time_value getNow()
{
	//tv_usec微妙,tv_sec秒
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//加入时间计时器
int CTimer::AddTimer(void* arg, time_value t)
{
	timer_time_arg.insert(std::make_pair(t, arg));
	timer_arg_time.insert(std::make_pair(arg, t));
	return 0;
}

void* CTimer::GetTimer(time_value tnow)
{
	std::multimap<time_value, void*>::iterator pos_time_arg =
		timer_time_arg.begin();
	void* arg = pos_time_arg->second;

	if (pos_time_arg == timer_time_arg.end())
	{
		return NULL;
	}

	if (pos_time_arg->first > tnow)
	{
		Show();
		return NULL;
	}

	timer_time_arg.erase(pos_time_arg);

	std::multimap<void*, time_value>::iterator pos_arg_time;

	for (pos_arg_time = timer_arg_time.lower_bound(arg);
		pos_arg_time != timer_arg_time.upper_bound(arg); ++pos_arg_time)
	{
		if (pos_arg_time->second == pos_time_arg->first)
		{
			timer_arg_time.erase(pos_arg_time);
			Show();
			return pos_time_arg->second;
		}
	}

	assert(0);
	Show();
	return pos_time_arg->second;
}
void CTimer::Show() const
{
	return;
	printf("arg_time %zu   time_arg %zu \n", timer_arg_time.size(),
		timer_time_arg.size());
}

void CTimer::RemoteTimer(void* arg)
{
	std::multimap<void*, time_value>::iterator pos_arg_time;

	for (pos_arg_time = timer_arg_time.lower_bound(arg);
		pos_arg_time != timer_arg_time.upper_bound(arg);)
	{
		std::multimap<time_value, void*>::iterator pos_time_arg;
		time_value t = pos_arg_time->second;

		for (pos_time_arg = timer_time_arg.lower_bound(t);
			pos_time_arg != timer_time_arg.upper_bound(t);)
		{
			if (pos_time_arg->second == arg)
			{
				timer_time_arg.erase(pos_time_arg++);
			}
			else
			{
				pos_time_arg++;
			}
		}

		timer_arg_time.erase(pos_arg_time++);
	}

	Show();
}

time_value CTimer::GetMinTimer() const
{
	if (timer_time_arg.size() == 0)
	{
		return CTimer::min_timer;
	}

	time_value tnow = getNow();
	std::multimap<time_value, void*>::const_iterator pos_time_arg =
		timer_time_arg.begin();

	time_value t = tnow - pos_time_arg->first;

	if (t >= 0)
	{
		return 0;
	}
	return -t;
}