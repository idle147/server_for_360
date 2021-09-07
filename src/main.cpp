#include "FrontTcpServer.h"
#include "BackEndServer.h"
#include "global.h"
#include "CSendTask.h"
#include "CCipher.h"

void SetKey(char* src_key, char* des_key)
{
	AesEncryptor* aes = nullptr;
	string des;
	int i = 5;
	//对密码进行加密
	aes = new AesEncryptor((unsigned char*)src_key);
	des = aes->EncryptString(src_key);
	//设置秘密
	if (p_database->ReSetKey((char*)des.c_str()) == false)
	{
		perror("重设密码失败, 请检查");
		system("pause");
		exit(-1);
	}
	delete aes;
}

void checkInfo(const char* info, char* db_key)
{
	AesEncryptor* aes = nullptr;
	string des;
	int i = 5;
	while (i--)
	{
		if (i == 0)
		{
			cout << "========== ========== ========== ==========" << endl;
			cout << "          输入错误次数太多,再见            " << endl;
			cout << "========== ========== ========== ==========" << endl;
			exit(-1);
		}
		cout << info;
		cin >> db_key;
		//对密码进行加密
		aes = new AesEncryptor((unsigned char*)db_key);
		des = aes->EncryptString(db_key);
		//判断密码是否正确
		if (p_database->checkKey((char*)des.c_str()) == false)
		{
			cout << "[Warning]密码输入错误, 请重试!" << endl;
			cin.clear();
			delete aes;
			continue;
		}
		delete aes;
		break;
	}
}

int main()
{
	int i_function;
	pid_t fpid;
	char* des_key = nullptr;
	char* db_key = new (char);
	cout << "========== ========== ========== ==========" << endl;
	cout << "                 欢迎使用                  " << endl;
	cout << "              随时翻车服务器               " << endl;
	cout << "========== ========== ========== ==========" << endl;

	while (true)
	{
		cin.clear();
		cout << "请选择服务器功能(1.打开服务器, 2.更改数据库密码, 其余按键退出):";
		cin >> i_function;
		switch (i_function)
		{
		case 1:
			checkInfo("请输入密码(初始密码123456):", db_key);
			//开辟进程
			fpid = fork();
			if (fpid < 0)
			{
				journal->ErrMsgWriteLog("进程开启失败");
			}
			//前置服务器
			else if (fpid == 0)
			{
				//写入日志文件临时变量
				journal->WriteLog("前置服务器,启动中……");
				TcpServer* my_Server = new TcpServer();
			}
			//后置服务器
			else
			{
				//写入日志文件临时变量
				journal->WriteLog("后置服务器,启动中……");
				BackEndServer* back_Server = new BackEndServer();
			}
			journal->~CLogger();
			break;
		case 2:
			cout << "请注意! 现阶段更改密码, 原先数据库的数据将全部无法读取!请谨慎操作!" << endl;
			cout << "请注意! 现阶段更改密码, 原先数据库的数据将全部无法读取!请谨慎操作!" << endl;
			cout << "[确认]请输入1, [返回]任意键" << endl;
			cin >> i_function;
			if (i_function != 1)
			{
				continue;
			}
			checkInfo("请输入原密码(初始密码123456):", db_key);
			cout << "请输入新密码:";
			cin >> db_key;
			des_key = new (char);
			SetKey(db_key, des_key);
			break;
		default:
			system("pause");
			exit(0);
			break;
		}
	}
	return 0;
}