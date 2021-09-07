#pragma once
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

typedef struct repeatIndex_st
{
	//信号量必须放在共享内存头部才行
	int flag;                   //判断是否是可写内存
	sem_t sem_mutex;			//用来互斥用的信号量
}repeatIndex_t;

typedef struct shmhead_st
{
	//信号量必须放在共享内存头部才行
	sem_t sem_full;				// 用来控制共享内存是否满的信号量
	sem_t sem_empty;			// 用来控制共享内存是否空的信号量
	repeatIndex_t* repeatIndex;			//用来互斥用的信号量
}shmindex_t;

class CShareMemory
{
public:
	CShareMemory(int index, int block_size, int block_num);
	CShareMemory();
	~CShareMemory();

	//创建和销毁
	bool Init(int index, int block_size, int block_num);
	void Destory();
	static void Destroy(int key); //静态删除共享内存方法

	//断开共享内存
	void CloseShm();

	//读取和存储
	int WriteShm(void* buf, int buf_len);
	void ReadShm(void* buf, int buf_len, int rd_index);

protected:
	int block_size;	    //块大小
	int block_num;		//总块数
	int index_size;     //索引区的大小

	int shm_id;			        //共享内存ID
	bool m_open;                //判断共享内存是否打开
	void* m_shmhead;		    // 共享内存头部指针

	shmindex_t shm_index;      //索引区数组指针
};
