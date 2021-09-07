#include "CMsgQueue.h"

CMsgQueue::CMsgQueue(int key)
{
	this->Init(key);
	this->mutex = new CMutexLock();
}

CMsgQueue::~CMsgQueue()
{
	//在析构函数中，如果调用msgctl函数删除消息队列，
	//那么有可能别的进程正在使用，因此谁创建，应该由谁调用Destory函数删除
}

/*
	函数说明: 消息队列初始化--利用字符串初始化
*/
int CMsgQueue::Init(char* pathName)
{
	int ret;
	//把从pathname导出的信息与id的低序8位组合成一个整数IPC键
	this->msg_key = ftok(pathName, 1);
	if (msg_key == -1)
	{
		perror("密钥初始化出错\n");
		exit(-1);
	}
	else
	{
		//如果类型为0,且消息不存在,则创建
		ret = msgget(msg_key, IPC_CREAT | IPC_EXCL | 0666);
	}
	//判断消息队列是否获取成功
	if (ret == -1)
	{
		perror("消息队列创建失败\n");
		system("pause");
		exit(-1);
	}
	//消息队列ID赋值
	this->msg_queue_id = ret;
	return 0;
}

/*
	函数说明: 消息队列初始化--利用密钥key初始化
*/
int CMsgQueue::Init(int key)
{
	//用以结果判断
	int ret;
	this->msg_key = key;
	//有时候进程非正常退出时，消息队列没有删除，如果里面还有消息，
	//若有消息, 则删除后重新创建消息队列
	ret = msgget(key, IPC_EXCL | IPC_CREAT | 0666);
	//如果原本消息存在
	if (ret == -1)
	{
		//重新连接
		this->msg_queue_id = msgget(key, IPC_CREAT | 0666);
		//销毁
		Destroy();
		//再次创建
		ret = msgget(key, IPC_CREAT | 0666);
		//判断消息队列是否获取成功
		if (ret == -1)
		{
			perror("消息队列创建失败");
			system("pause");
			exit(-1);
		}
	}
	//消息队列赋值
	this->msg_queue_id = ret;
	printf("新消息队列初始化完毕\n");
	return 0;
}

/*
	函数说明: 接收消息--利用msg_t结构体接收消息
	参数说明: msg表示消息队列结构体
			  n_bytes表示消息的长度
			  type表示从消息队列读取的消息形态: 0表示读取所有消息
			  flag:0以阻塞方式读取/IPC_NOWAIT表示非阻塞读取
*/
bool CMsgQueue::ReceiveMsg(void* msg, size_t n_bytes, long type, int flag)
{
	this->mutex->Lock();
	//接收消息队列
	int ret = (int)msgrcv(this->msg_queue_id, msg, n_bytes, type, flag);
	if (ret == -1)
	{
		this->mutex->Unlock();
		perror("消息队列接收失败");
		return false;
	}
	this->mutex->Unlock();
	return true;
}

/*
	函数说明: 发送消息--利用msg_t结构体发送消息
	参数说明: msg表示消息队列结构体
			  n_bytes表示消息的长度
			  flag:0以阻塞方式读取/IPC_NOWAIT表示非阻塞写入
*/
bool CMsgQueue::SendMsg(const void* msg, size_t n_bytes, int flag)
{
	this->mutex->Lock();
	//发送消息
	int ret = msgsnd(this->msg_queue_id, msg, n_bytes, flag);
	if (ret == -1)
	{
		this->mutex->Unlock();
		perror("消息队列发送失败");
		return false;
	}
	this->mutex->Unlock();

	return true;
}

/*
	函数说明:销毁消息队列
*/
bool CMsgQueue::Destroy()
{
	int ret = msgctl(this->msg_queue_id, IPC_RMID, NULL);
	if (ret == -1)
	{
		perror("消息队列销毁失败");
		exit(-1);
	}
	printf("原消息队列销毁成功");
	this->msg_queue_id = 0;
	return true;
}