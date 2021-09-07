#pragma once
#include "CTask.h"
#include "CCrc.h"
#include "global.h"

class CSendTask : public CTaskBase
{
public:
	CSendTask(msg_t m_recv_msg, int& pack_num);
	~CSendTask();
	void Run();
	int SendAns(int fd, t_Answer ans_pack);
	int SendGetPack(int fd, t_GetProgress get_pack);
private:
	msg_t recv_msg;
	int pack_num;
	struct sockaddr_in localaddr;
};
