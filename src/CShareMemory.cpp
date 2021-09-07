#include "CShareMemory.h"
#define ERR_EXIT(m) \
	do { \
			perror(m); \
			fprintf(stderr, "Error: (%s:%s:%d)", __FILE__, __func__, __LINE__); \
			exit(EXIT_FAILURE); \
	} while(0)

CShareMemory::CShareMemory(int index, int block_size, int block_num)
{
	if (block_size <= 0 || block_num <= 0)
	{
		printf("共享内存的值不能设为非正整数\n");
		exit(-1);
	}
	//打开共享内存
	this->Init(index, block_size, block_num);
}

CShareMemory::CShareMemory()
{
	m_shmhead = NULL;
	m_open = false;
}

CShareMemory::~CShareMemory()
{
	//释放结构体数组
	delete[] shm_index.repeatIndex;
	//关闭套接字结构体
	this->CloseShm();
}

/*
	函数说明: 连接共享内存
*/
bool CShareMemory::Init(int index, int block_size, int block_num)
{
	//定义索引区数组
	shm_index.repeatIndex = new repeatIndex_t[block_num];
	index_size = sizeof(sem_t) * 2 + sizeof(shmindex_t) * block_num;
	//共享内存的总大小
	int all_num = block_size * block_num + index_size;

	//创建共享内存
	/* 内核是以页为单位分配内存,
	   当参数size的值不是系统内存页长的整数倍时,
	   系统会分配给进程最小的可以满足size长的页数,
	   但是最后一页的剩余部分是不可用的
	*/

	//1. 查看是否已经存在共享内存，如果有则删除旧的
	int shm_id = shmget((key_t)index, 0, 0);
	if (shm_id != -1)
	{
		shmctl(shm_id, IPC_RMID, NULL); 	//删除已经存在的共享内存
	}

	//2. 创建共享内存
	shm_id = shmget((key_t)index, all_num, 0666 | IPC_CREAT);
	if (shm_id == -1)
	{
		ERR_EXIT("shmget error");
	}

	//3. 连接共享内存
	//调用成功时返回一个指向共享内存第一个字节的指针，如果调用失败返回 - 1.
	this->m_shmhead = shmat(shm_id, NULL, 0);
	if (this->m_shmhead == (void*)-1)
	{
		ERR_EXIT("shmat error");
	}
	memset(m_shmhead, 0, all_num);		       //初始化共享内存大小

	//4.初始化信息
	this->block_size = block_size;			   //共享信息写入
	this->block_num = block_num;		       //写入每块大小
	this->m_open = true;

	//5.初始化信号量
	sem_init(&shm_index.sem_empty, 1, 0);	    // 第一个1表示可以跨进程共享，第二个0表示初始值
	sem_init(&shm_index.sem_full, 1, block_num);// 第一个1表示可以跨进程共享，第二个blocks表示初始值
	for (int i = 0; i < block_num; i++)
	{
		sem_init(&shm_index.repeatIndex[i].sem_mutex, 1, 1);//第一个1表示可以跨进程共享，第二个1表示初始值
		shm_index.repeatIndex[i].flag = true;
	}

	//6.写入索引区
	memcpy(m_shmhead, &shm_index, index_size);

	printf("共享内存初始化完毕\n");

	return true;
}

/*
	函数说明:销毁共享内存
*/
void CShareMemory::Destory()
{
	//删除信号量
	sem_destroy(&shm_index.sem_empty);
	sem_destroy(&shm_index.sem_full);
	for (int i = 0; i < this->block_num; i++)
	{
		sem_destroy(&shm_index.repeatIndex[i].sem_mutex);
		shm_index.repeatIndex[i].flag = false;
	}

	//销毁共享内存
	if (shmctl(this->shm_id, IPC_RMID, 0) == -1)
	{
		printf("Delete shm_id=%d \n", this->shm_id);
		ERR_EXIT("shmctl rm");
	}
	//相关值初始化
	m_shmhead = NULL;
	m_open = false;
}

/*
	函数说明: 静态销毁共享内存
*/
void CShareMemory::Destroy(int key)
{
	int shm_id = 0;
	//查看是否已经存在共享内存，如果有则删除旧的
	shm_id = shmget((key_t)key, 0, 0);
	if (shm_id != -1)
	{
		printf("Delete shm_id=%d \n", shm_id);
		//	删除已经存在的共享内存
		shmctl(shm_id, IPC_RMID, NULL);
	}
}

/*
	函数说明: 断开与共享内存的连接
*/
void CShareMemory::CloseShm()
{
	if (m_open)
	{
		//共享内存脱离
		Destory();
		m_shmhead = NULL;
		m_open = false;
	}
}

/*
	函数说明:往共享内存内写数据
*/
int CShareMemory::WriteShm(void* buf, int buf_len)
{
	if (buf_len >= block_size)
	{
		printf("写入共享内存的东西超过了共享内存所能容纳的最大长度;\n");
		exit(-1);
	}
	//获取索引区的地址
	shmindex_t* pHead = (shmindex_t*)m_shmhead;
	//是否有资源写
	sem_wait(&pHead->sem_full);
	//循环获取可写资源头
	int i;
	for (i = 0; i < block_num; i++)
	{
		if (pHead->repeatIndex[i].flag == true)
		{
			break;
		}
		i++;
	}
	sem_wait(&pHead->repeatIndex[i].sem_mutex);
	pHead->repeatIndex[i].flag = false; //标识符设置为不能写入
	//连接共享内存
	printf("写入第[%d]块共享内存\n", i);
	memcpy(m_shmhead + i * (this->block_size) + index_size, buf, this->block_size);
	//读取位置标识符
	printf("数据写入共享内存成功\n");
	//解除互斥锁
	sem_post(&pHead->repeatIndex[i].sem_mutex);	//解除互斥
	sem_post(&pHead->sem_empty);				//可用读资源+1
	//返回索引位置
	return i;
}

/*
	函数说明: 读取共享内存的数据
*/
void CShareMemory::ReadShm(void* buf, int buf_len, int rd_index)
{
	if (buf_len >= block_size)
	{
		printf("读取共享内存的东西超过了共享内存所能容纳的最大长度;\n");
		exit(-1);
	}
	//获取索引区的地址
	shmindex_t* pHead = (shmindex_t*)m_shmhead;
	//检测写资源是否可用
	sem_wait(&pHead->sem_empty);
	//互斥上锁
	sem_wait(&pHead->repeatIndex[rd_index].sem_mutex);
	//读取信息, 如果是读取最后1块, 有多少写多少
	memcpy(buf, this->m_shmhead + rd_index * (this->block_size) + index_size, buf_len);
	memset(this->m_shmhead + rd_index * (this->block_size) + index_size, 0, buf_len);
	//标识符设置为可以写入
	printf("读取共享内存[%d],索引为:%d\n", this->shm_id, rd_index);
	pHead->repeatIndex[rd_index].flag = true;
	//解除互斥
	sem_post(&pHead->repeatIndex[rd_index].sem_mutex);
	//增加可写资源
	sem_post(&pHead->sem_full);
}