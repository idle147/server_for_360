#include "CSemMutex.h"

CSemMutex::CSemMutex()
{
}

CSemMutex::CSemMutex(key_t i_Key)
{
	//初始化,信号密钥
	Init(i_Key);
}

CSemMutex::~CSemMutex()
{
}

/*
	函数说明:利用key初始化信号量
*/
void CSemMutex::Init(key_t i_key)
{
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
	/* union semun is defined by including <sys/sem.h> */
#else
	/* according to X/OPEN we have to define it ourselves */
	union semun
	{
		int val;                  /* value for SETVAL */
		struct semid_ds* buf;     /* buffer for IPC_STAT, IPC_SET */
		unsigned short* array;    /* array for GETALL, SETALL */
		struct seminfo* __buf;    /* buffer for IPC_INFO */
	};
#endif

	int i_sem_id; //定义信号ID
	union semun arg;//定义信号结构体
	u_short array[2] = { 0, 0 }; //定义信号量,用以上锁

	//生成信号量集, 包含两个信号量
	i_sem_id = semget(i_key, 2, IPC_CREAT | IPC_EXCL | 0666);
	if (i_sem_id != -1)
	{
		arg.array = &array[0];
		//SETVAL:将所有信号量的值设置为0
		if (semctl(i_sem_id, 0, SETALL, arg) == -1)
		{
			throw ("[CSemMutex::init]信号量初始化失败:" + string(strerror(errno)));
		}
	}
	else
	{
		//信号量已经存在
		if (errno != EEXIST)
		{
			throw ("[CSemMutex::init]信号量已经存在:" + string(strerror(errno)));
		}
		//连接信号量
		i_sem_id = semget(i_key, 2, 0666);
		if (i_sem_id == -1)
		{
			throw ("[CSemMutex::init]连接信号量错误:" + string(strerror(errno)));
		}
		else
		{
			printf("信号创建,连接成功\n");
		}
	}

	//参数赋值
	this->sem_id = i_sem_id;
	this->sem_key = i_key;
}

/*
	函数说明:对信号进行上锁
*/
int CSemMutex::ShareLock() const
{
	//进入共享锁, 第二个信号量的值表示当前使用信号量的进程个数
	//等待第一个信号量变为0(互斥锁没有使用)
	//占用第二个信号量(第二个信号量值+1, 表示被共享锁使用)
	struct sembuf sops[2] = { {0, 0, SEM_UNDO},
							  {1, 1, SEM_UNDO} };
	size_t nsops = 2;
	//返回当前可用的资源数量
	return semop(this->sem_id, &sops[0], nsops);
}

/*
	函数说明:对信号进行解锁
*/
int CSemMutex::UnShareLock() const
{
	//解除共享锁, 有进程使用过第二个信号量
	//等到第二个信号量可以使用(第二个信号量的值>=1)
	struct sembuf sops[1] = { {1, -1, SEM_UNDO} };
	size_t nsops = 1;
	//返回当前可用的资源数量
	return semop(this->sem_id, &sops[0], nsops);
}

/*
	函数说明:对信号尝试上锁
*/
bool CSemMutex::TryShareLock() const
{
	struct sembuf sops[2] = { {0, 0, SEM_UNDO | IPC_NOWAIT},
							  {1, 1, SEM_UNDO | IPC_NOWAIT} };
	size_t nsops = 2;
	int res = semop(this->sem_id, &sops[0], nsops);
	if (res == -1)
	{
		if (errno == EAGAIN)
		{
			//无法获得锁
			return false;
		}
		else
		{
			throw ("[CSemMutex::TryLock]信号操作出错: " + string(strerror(errno)));
		}
	}
	return true;
}

/*
	函数说明:进入互斥锁
*/
int CSemMutex::MutexLock() const
{
	//进入互斥锁, 第一个信号量和第二个信号都没有被使用过(即, 两个锁都没有被使用)
	//等待第一个信号量变为0
	//等待第二个信号量变为0
	//释放第一个信号量(第一个信号量+1, 表示有一个进程使用第一个信号量)
	struct sembuf sops[3] = { {0, 0, SEM_UNDO},
							  {1, 0, SEM_UNDO},
							  {0, 1, SEM_UNDO} };
	size_t nsops = 3;
	return semop(this->sem_id, &sops[0], nsops);
}

/*
	函数说明:解除互斥锁
*/
int CSemMutex::UnMutexLock() const
{
	//解除互斥锁, 有进程使用过第一个信号量
	//等待第一个信号量(信号量值>=1)
	struct sembuf sops[1] = { {0, -1, SEM_UNDO} };
	size_t nsops = 1;
	return semop(this->sem_id, &sops[0], nsops);
}

/*
	函数说明:尝试上锁
*/
bool CSemMutex::TryMutexLock() const
{
	struct sembuf sops[3] = { {0, 0, SEM_UNDO | IPC_NOWAIT},
							  {1, 0, SEM_UNDO | IPC_NOWAIT},
							  {0, 1, SEM_UNDO | IPC_NOWAIT} };
	size_t nsops = 3;

	int res = semop(this->sem_id, &sops[0], nsops);
	if (res == -1)
	{
		if (errno == EAGAIN)
		{
			//无法获得锁
			return false;
		}
		else
		{
			throw ("[CSemMutex::TryWLock]锁操作出错: " + string(strerror(errno)));
		}
	}
	return true;
}