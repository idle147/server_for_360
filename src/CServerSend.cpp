#include "CServerSend.h"

CServerSend::CServerSend(CThreadPool* m_thread_pool)
{
	this->thread_pool = m_thread_pool;
	this->CreatThead();
}

CServerSend::~CServerSend()
{
}

void* CServerSend::thfunc(void* arg)
{
	CServerSend* server_send = (CServerSend*)arg;
	CSendTask* send_task = nullptr;
	msg_t* recv_msg = (msg_t*)malloc(sizeof(msg_t));           //收到消息的结构体
	int pack_num = 1; //初始发送包
	while (true)
	{
		//监听信息
		printf("[发送端]线程[tid:% lu]监听数据中...\n", pthread_self());
		memset(recv_msg, 0, sizeof(msg_t));
		//收到后端发往前端的消息队列
		b2f_msg_queue->ReceiveMsg(recv_msg, msg_len, 2, 0);
		printf("[发送端]收到[来自]%d, [类型]%d,[索引]%d, [fd值]%d的消息队列\n",
			recv_msg->m_type, recv_msg->fun_typ, recv_msg->index, recv_msg->socketFd);
		send_task = new CSendTask(*recv_msg, pack_num);
		server_send->thread_pool->AddTask(send_task);
		printf("[发送端]线程任务创建完毕\n");
	}
	free(recv_msg);
	return (void*)0;
}

void CServerSend::CreatThead()
{
	pthread_t tidp;
	int ret;
	char str_tmp[MAX_CHAR_LEN];
	//消息队列创建
	ret = pthread_create(&tidp, NULL, thfunc, this);
	if (ret)
	{
		memset(str_tmp, '\0', MAX_CHAR_LEN);
		sprintf(str_tmp, "前置服务器, [发送端]线程创建失败:%d,退出程序", ret);
		journal->ErrMsgWriteLog(str_tmp);
		system("pause");
		exit(-1);
	}
}