#include "BackTask.h"

/*****/
//后置服务器, ---登陆注册业务
CLoginAndRegTask::CLoginAndRegTask(bool isLogin, msg_t buf)
{
	//参数初始化
	this->log_and_Reg = new CLoginAndReg();
	this->isLogin = isLogin;
	this->msg_info = buf;
}

CLoginAndRegTask::~CLoginAndRegTask()
{
	if (this->log_and_Reg != nullptr)
	{
		delete this->log_and_Reg;
		this->log_and_Reg = nullptr;
	}
}

void CLoginAndRegTask::Run()
{
	t_Login logMsg;           //登陆结构体

	int res;
	char str_tmp[MAX_CHAR_LEN];
	if (this->isLogin == true)
	{
		//从共享内存里获取数据
		memset(str_tmp, 0, MAX_CHAR_LEN);
		sprintf(str_tmp, "[后置服务器]收到[登陆]业务,[来源]%d, [类型]%d,[索引]%d,[套接字]%d",
			msg_info.m_type, msg_info.fun_typ, msg_info.index, msg_info.socketFd);
		journal->WriteLog(str_tmp);
		//从前置服务器内读取业务
		memset(&logMsg, 0, sizeof(logMsg));
		f2b_share_memory->ReadShm(&logMsg, sizeof(logMsg), msg_info.index);
		//进行业务判断
		memset(str_tmp, 0, MAX_CHAR_LEN);
		sprintf(str_tmp, "[后置服务器]进行[读取用户表]操作:账号[%d], 密码[%s]", logMsg.user_id, logMsg.pwd);
		journal->WriteLog(str_tmp);
		//进行数据库的处理
		res = this->log_and_Reg->LoginOpt(logMsg.user_id, logMsg.pwd);
		ResJudege(res);
	}
	else
	{
		//从共享内存里获取数据
		memset(str_tmp, 0, MAX_CHAR_LEN);
		sprintf(str_tmp, "[后置服务器]收到[注册]业务,[来源]%d, [类型]%d,[索引]%d,[套接字]%d",
			msg_info.m_type, msg_info.fun_typ, msg_info.index, msg_info.socketFd);
		journal->WriteLog(str_tmp);
		//从共享内存里获取数据
		memset(&regMsg, 0, sizeof(regMsg));
		f2b_share_memory->ReadShm(&regMsg, sizeof(regMsg), msg_info.index);
		//调用注册业务
		memset(str_tmp, 0, MAX_CHAR_LEN);
		sprintf(str_tmp, "[后置服务器]进行[写入注册表]操作:账号[%d],名称[%s],密码[%s]\n",
			regMsg.user_id, regMsg.name, regMsg.pwd);
		journal->WriteLog(str_tmp);
		res = this->log_and_Reg->RegOpt(regMsg.user_id, regMsg.name, regMsg.pwd);
		//结果判断
		ResJudege(res);
	}
}

void CLoginAndRegTask::ResJudege(int res)
{
	printf("[登录/注册]业务处理完毕,进行结果判断\n");
	//进行结果判断
	t_Answer ans_pack;
	memset(&ans_pack, 0, sizeof(t_Answer));
	if (isLogin == true)
	{
		ans_pack.head.packet_type = LOGIN_ANSWER;
	}
	else
	{
		ans_pack.head.packet_type = REG_ANSWER;
	}
	//判断类型
	if (res == SQLOPT_SUCCESS && isLogin == true)
	{
		//登录成功
		ans_pack.anser_type = ANSWER_LOGIN_SUCCESS;
	}
	else if (res == SQLOPT_SUCCESS && isLogin == false)
	{
		//注册成功
		ans_pack.anser_type = ANSWER_REG_SUCCESS;
	}
	else
	{
		//其余选项
		ans_pack.anser_type = res;
	}
	//将响应包写入共享内存, 并获取索引信息
	msg_info.index = b2f_share_memory->WriteShm(&ans_pack, sizeof(ans_pack));
	//写入消息队列数组内
	msg_info.m_type = 2;                             //后端发给前段, 标志位为2
	msg_info.fun_typ = ANSWER;                                 //应答包
	//发送消息队列数据
	b2f_msg_queue->SendMsg(&msg_info, msg_len, 0);
	printf("[登陆注册应答包]消息队列发送完毕,来自%d发送的类型%d的消息\n", msg_info.m_type, msg_info.fun_typ);
	//判断是否需要回滚操作
	if (ans_pack.anser_type == ANSWER_REG_SUCCESS)
	{
		f2b_msg_queue->ReceiveMsg(&msg_info, msg_len, 3, 0);
		//进行回滚
		if (msg_info.fun_typ == RollBack)
		{
			this->log_and_Reg->DeletOpt(regMsg.user_id);
			journal->WriteLog("[后端服务器]注册信息,回滚成功");
		}
	}
}

CGetVideoFrame::CGetVideoFrame(msg_t buf)
{
	//参数初始化
	this->video_info = new CVideoInfo();
	this->msg_info = buf;      //消息队列
}

CGetVideoFrame::~CGetVideoFrame()
{
	if (video_info != nullptr)
	{
		delete video_info;
		video_info == nullptr;
	}
}

void CGetVideoFrame::Run()
{
	char str_tmp[MAX_CHAR_LEN];

	//从共享内存里获取数据
	f2b_share_memory->ReadShm(&reqPro, sizeof(reqPro), msg_info.index);
	memset(str_tmp, 0, MAX_CHAR_LEN);
	sprintf(str_tmp, "[后置服务器]收到[获取视频帧数包]业务,[来源]%d, [类型]%d,[索引]%d,[套接字]%d",
		msg_info.m_type, msg_info.fun_typ, msg_info.index, msg_info.socketFd);
	journal->WriteLog(str_tmp);

	//进行业务判断
	memset(str_tmp, 0, MAX_CHAR_LEN);
	sprintf(str_tmp, "[后置服务器]进行[查找视频表]操作:视频id[%s],用户id[%d]\n",
		reqPro.video_id, reqPro.user_id);
	journal->WriteLog(str_tmp);

	int res = this->video_info->GetVideoProgress(reqPro.user_id, reqPro.video_id);
	if (res < 0)
	{
		t_Answer ans_pack;
		memset(&ans_pack, 0, sizeof(t_Answer));
		//若数据库操作失败
		ans_pack.head.packet_type = SELECT_VIDEO_ANSWER;
		ans_pack.anser_type = ANSWER_QUERY_ERROR;
		//将响应包写入共享内存, 写入消息队列数组内
		msg_info.index = b2f_share_memory->WriteShm((void*)&ans_pack, sizeof(ans_pack));
		msg_info.fun_typ = ANSWER;                       //应答包
		msg_info.m_type = 2;                             //后端发给前段, 标志位为2

		//发送消息队列数据
		b2f_msg_queue->SendMsg(&this->msg_info, msg_len, 0);
		printf("[查无视频帧数]消息队列发送完毕,来自%d发送的类型%d的消息\n", msg_info.m_type, msg_info.fun_typ);
		return;
	}
	//若操作成功, 写入操作成功数据包
	t_GetProgress get_pack;
	memset(&get_pack, 0, sizeof(t_GetProgress));
	get_pack.user_id = reqPro.user_id;
	strcpy(get_pack.video_id, reqPro.video_id);
	get_pack.video_progress = res;

	//将响应包写入共享内存,写入消息队列数组内
	msg_info.index = b2f_share_memory->WriteShm((void*)&get_pack, sizeof(get_pack));
	msg_info.fun_typ = GET_PREGRORESS;                       //应答包
	msg_info.m_type = 2;                             //后端发给前段, 标志位为2

	//发送消息队列数据
	b2f_msg_queue->SendMsg(&this->msg_info, msg_len, 0);
	printf("[获取视频帧数]消息队列发送完毕,来自%d发送的类型%d的消息\n", msg_info.m_type, msg_info.fun_typ);
}

CDeleteVideoFrame::CDeleteVideoFrame(msg_t buf)
{
	//参数初始化
	this->video_info = new CVideoInfo();
	this->msg_info = buf;
}

CDeleteVideoFrame::~CDeleteVideoFrame()
{
	if (video_info != nullptr)
	{
		delete video_info;
		video_info == nullptr;
	}
}

void CDeleteVideoFrame::Run()
{
	char str_tmp[MAX_CHAR_LEN];

	//从共享内存里获取数据
	f2b_share_memory->ReadShm(&reqPro, sizeof(reqPro), msg_info.index);
	memset(str_tmp, 0, MAX_CHAR_LEN);
	sprintf(str_tmp, "[后置服务器]进行[删除视频条目]操作:视频id[%s],用户id[%d]\n",
		reqPro.video_id, reqPro.user_id);
	journal->WriteLog(str_tmp);

	//进行业务判断
	int frame_num = this->video_info->GetVideoProgress(reqPro.user_id, reqPro.video_id);
	int res = this->video_info->DeleteVideoInfo(reqPro.user_id, reqPro.video_id);

	//写入回应包
	t_Answer ans_pack;
	ans_pack.head.packet_type = DELETE_VIDEO_ANSWER;
	memset(&ans_pack, 0, sizeof(t_Answer));
	if (res == SQLOPT_SUCCESS)
	{
		ans_pack.anser_type = ANSWER_DELETE_SUCCESS;                       //包的结果为操作失败
	}
	else
	{
		ans_pack.anser_type = ANSWER_DELETE_ERROR;                         //包的结果为删除失败
	}

	//将响应包写入共享内存, 写入消息队列数组内
	msg_info.index = b2f_share_memory->WriteShm((void*)&ans_pack, sizeof(ans_pack));
	msg_info.fun_typ = ANSWER;                       //应答包
	msg_info.m_type = 2;                             //后端发给前段, 标志位为2

	//发送消息队列数据
	b2f_msg_queue->SendMsg(&this->msg_info, msg_len, 0);
	printf("[删除视频应答包]消息队列发送完毕,来自%d发送的类型%d的消息\n", msg_info.m_type, msg_info.fun_typ);
	//判断是否需要回滚操作
	if (ans_pack.anser_type == ANSWER_DELETE_SUCCESS)
	{
		f2b_msg_queue->ReceiveMsg(&msg_info, msg_len, 3, 0);
		//进行回滚
		if (msg_info.fun_typ == RollBack)
		{
			this->video_info->InsertVideoInfo(reqPro.user_id, reqPro.video_id, frame_num);
			journal->WriteLog("[后端服务器]删除视频应答信息,消息回滚成功!");
		}
	}
}

CInsertVideo::CInsertVideo(msg_t buf)
{
	this->msg_info = buf;
	this->video_info = new CVideoInfo();
}

CInsertVideo::~CInsertVideo()
{
	if (video_info != nullptr)
	{
		delete video_info;
		video_info == nullptr;
	}
}

void CInsertVideo::Run()
{
	char str_tmp[MAX_CHAR_LEN];
	//从共享内存里获取数据
	f2b_share_memory->ReadShm(&t_video, sizeof(t_video), msg_info.index);

	memset(str_tmp, '\0', sizeof(str_tmp));
	sprintf(str_tmp, "[后置服务器]接收到[插入视频信息]业务, 视频ID:%s, 用户ID:%d, 帧数:%d",
		t_video.video_id, t_video.user_id, t_video.current_frame);
	journal->WriteLog(str_tmp);

	//进行业务判断
	int res = this->video_info->InsertVideoInfo(t_video.user_id, t_video.video_id, t_video.current_frame);

	//写入回应包
	t_Answer ans_pack;
	memset(&ans_pack, 0, sizeof(t_Answer));
	if (res == SQLOPT_SUCCESS)
	{
		ans_pack.anser_type = ANSWER_INSERT_SUCCESS;                       //包的结果为操作成功
	}
	else
	{
		ans_pack.anser_type = ANSWER_INSERT_ERROR;                       //包的结果为操作失败
	}
	ans_pack.head.packet_type = INSERT_VIDEO_ANSWER;
	//将响应包写入共享内存, 写入消息队列数组内
	msg_info.index = b2f_share_memory->WriteShm((void*)&ans_pack, sizeof(ans_pack));
	msg_info.fun_typ = ANSWER;                       //应答包
	msg_info.m_type = 2;                             //后端发给前段, 标志位为2

	//发送消息队列数据
	b2f_msg_queue->SendMsg(&this->msg_info, msg_len, 0);
	printf("[插入视频应答包]消息队列发送完毕,来自%d发送的类型%d的消息\n", msg_info.m_type, msg_info.fun_typ);
}
/*/////////////////////////
//////////////////////////
		文件任务
//////////////////////////
////////////////////////*/
CFileTask::CFileTask(string m_file_path, bool m_end, msg_t buf)
{
	this->file_path = m_file_path;
	this->isEnd = m_end;
	this->msg_info = buf;
}

CFileTask::~CFileTask()
{
}

void CFileTask::reqResend(int i)
{
	char str_tmp[MAX_CHAR_LEN];
	memset(str_tmp, 0, MAX_CHAR_LEN);
	sprintf(str_tmp, "[后置服务器]进行[请求重发文件包]操作, 包号[%d]\n", i);
	journal->WriteLog(str_tmp);

	//初始化变量赋值
	t_Answer ans_pack;
	memset(&ans_pack, 0, sizeof(t_Answer));

	//数据包赋值
	ans_pack.head.packet_type = FILE_RESEND;
	ans_pack.head.packet_size = sizeof(t_Answer);
	ans_pack.anser_type = i;

	//将响应包写入共享内存, 并获取索引信息
	msg_info.index = b2f_share_memory->WriteShm(&ans_pack, sizeof(ans_pack));
	msg_info.m_type = 2;                             //后端发给前段, 标志位为2
	msg_info.fun_typ = ANSWER;                       //应答包
	//发送消息队列数据
	b2f_msg_queue->SendMsg(&msg_info, msg_len, 0);
	printf("[文件重发应答包]消息队列发送完毕,来自%d发送的类型%d的消息\n", msg_info.m_type, msg_info.fun_typ);
}

bool CFileTask::MergeFile()
{
	//相关变量定义
	string temp;
	ifstream finput;
	ofstream fout;
	string check_path = file_path + "/0.tmp";

	//打开首个文件
	fout.open(check_path, ios::app);
	if (fout.fail())
	{
		journal->ErrMsgWriteLog("[合并文件]打开文件[输出流]失败,文件路径:", check_path.c_str());
		return false;
	}
	//循环持续写入
	for (int i = 1;i < file_pack.packer_num;i++)
	{
		check_path = file_path + '/' + std::to_string(i) + ".tmp";
		finput.open(check_path);
		if (finput.fail())
		{
			journal->ErrMsgWriteLog("[合并文件]打开文件[输入流]失败,文件路径:", check_path.c_str());
			return false;
		}
		//一行行写入
		while (finput >> temp)
		{
			fout << temp;
		}
		//删除文件
		if (remove(check_path.c_str()) == false)
		{
			journal->ErrMsgWriteLog("[合并文件]删除旧文件失败,文件路径:", check_path.c_str());
		}
		finput.close();
	}
	//关闭输出流
	fout.close();
	//重命名文件
	string newPath = file_path;
	//将'_'替换为'.'
	replace(file_path.end() - 5, file_path.end(), '_', '.');
	rename(check_path.c_str(), newPath.c_str());
	//删除文件夹
	CFileOpt::DeleteDir(file_path.c_str());
	//计算MD5校验码
	char pMD5[128];
	MD5_CTX* md5_ctx = new MD5_CTX();
	md5_ctx->GetFileMd5(pMD5, file_path.c_str());
	//查表
	char* fileName;
	//写入回应包
	t_Answer ans_pack;
	if (CFileTable::ReadFileName(fileName, pMD5) == true)
	{
		return true;
	}
	else
	{
		//写入MD5校验码错误回应包
		journal->ErrMsgWriteLog(file_path.c_str(), ",MD5校验出错");
		memset(&ans_pack, 0, sizeof(t_Answer));
		ans_pack.anser_type = ANSWER_MD5_ERROR;          //包的结果
		ans_pack.head.packet_type = FILE_ANSWER;
		//将响应包写入共享内存, 写入消息队列数组内
		msg_info.index = b2f_share_memory->WriteShm((void*)&ans_pack, sizeof(ans_pack));
		msg_info.fun_typ = ANSWER;                       //应答包
		msg_info.m_type = 2;                             //后端发给前段, 标志位为2

		//发送消息队列数据
		b2f_msg_queue->SendMsg(&this->msg_info, msg_len, 0);
		printf("[文件合并失败]发往前端的消息队列发送完毕\n");
		return false;
	}
	//写入文件接收成功回应包
	memset(&ans_pack, 0, sizeof(t_Answer));
	ans_pack.anser_type = ANSWER_FILE_SUCCESS;             //表示文件传输成功
	ans_pack.head.packet_type = FILE_ANSWER;

	//将响应包写入共享内存, 写入消息队列数组内
	msg_info.index = b2f_share_memory->WriteShm((void*)&ans_pack, sizeof(ans_pack));
	msg_info.fun_typ = ANSWER;                       //应答包
	msg_info.m_type = 2;                             //后端发给前段, 标志位为2

	//发送消息队列数据
	b2f_msg_queue->SendMsg(&this->msg_info, msg_len, 0);
	printf("[文件合并成功]发往前端的消息队列发送完毕\n", fileName);
	return false;
}
bool CFileTask::WriteInfo()
{
	char str_tmp[MAX_CHAR_LEN];
	string check_path = file_path + '/' + std::to_string(file_pack.user_id) + '/' + file_pack.file_name + '/' + std::to_string(file_pack.packer_num) +".tmp";
	//写入文件
	ofstream _File(check_path.c_str());
	_File.open(check_path, ios::out | ios::trunc | ios::binary);
	if (!_File)
	{
		memset(str_tmp, 0, MAX_CHAR_LEN);
		sprintf(str_tmp, "文件[%s][创建/打开]错误", check_path.c_str());
		journal->ErrMsgWriteLog(str_tmp);
		return false;
	}
	//写入文件
	_File << file_pack.data;
	//关闭文件
	_File.close();
	return true;
}

void CFileTask::Run()
{
	//从共享内存里获取数据
	char str_tmp[MAX_CHAR_LEN];
	memset(str_tmp, 0, MAX_CHAR_LEN);
	sprintf(str_tmp, "[后端服务器]收到[写入文件片]业务,[来源]%d, [类型]%d,[索引]%d,[套接字]%d",
		msg_info.m_type, msg_info.fun_typ, msg_info.index, msg_info.socketFd);
	journal->WriteLog(str_tmp);
	//从前置服务器内读取业务
	memset(&file_pack, 0, sizeof(file_pack));
	f2b_share_memory->ReadShm(&file_pack, sizeof(file_pack), msg_info.index);
	//进行业务判断
	memset(str_tmp, 0, MAX_CHAR_LEN);
	sprintf(str_tmp, "[后端服务器]进行[写入文件片]业务:包号[%d]", file_pack.packer_num);
	journal->WriteLog(str_tmp);

	//设置路径
	this->file_path += '/' + std::to_string(file_pack.user_id) + '/' + file_pack.file_name;

	//遍历检查是否都存在
	string check_path;

	//如果是最后一个包,则首先遍历检查,然后将所有文件合并成一个文件
	if (this->isEnd == true)
	{
		//写入最后一个包
		WriteInfo();
		bool flag = true;
		//检验是否有丢包
		for (int i = 0;i < file_pack.packer_num;i++)
		{
			check_path = file_path + '/' + std::to_string(i) + ".tmp";
			//判断文件存不存在
			struct stat buffer;
			if (stat(check_path.c_str(), &buffer) != 0)
			{
				//文件不存在, 告诉前端,要请求重发该包
				reqResend(i);
				flag = false;
			}
		}
		//如果有包要重发, 直接结束该任务
		if (flag == false)
		{
			return;
		}
		//合并文件
		if (MergeFile() == false)
		{
			//打印错误信息
			memset(str_tmp, 0, MAX_CHAR_LEN);
			sprintf(str_tmp, "[后端服务器]合并文件操作失败,文件任务强制结束。路径:", file_path.c_str());
			journal->ErrMsgWriteLog(str_tmp, true);

			//进行回滚操作
			CVideoInfo* video_info = new CVideoInfo();       //视频信息指针
			video_info->DeleteVideoInfo(file_pack.user_id, file_pack.file_name);
			journal->WriteLog("[后端服务器]删除视频插入信息,回滚成功");
			return;
		}
	}
	this->WriteInfo();

	/*
	//关闭文件
	if (this->fileOpt->FileClose() == false)
	{
		memset(str_tmp, 0, MAX_CHAR_LEN);
		sprintf(str_tmp, "[后端服务器]文件[关闭]失败,[路径]:", file_path.c_str());
		journal->ErrMsgWriteLog(str_tmp, true);
		return;
	}
	*/
}

/*
	文件首包
*/
CFileFirstTask::CFileFirstTask(string m_file_path, msg_t buf)
{
	this->msg_info = buf;
	this->file_path = m_file_path;
}

CFileFirstTask::~CFileFirstTask()
{
}

void CFileFirstTask::Run()
{
	int res = -1;

	//从共享内存里获取数据
	char str_tmp[MAX_CHAR_LEN];
	memset(str_tmp, 0, MAX_CHAR_LEN);
	sprintf(str_tmp, "[后端服务器]收到[发送文件请求]业务,[来源]%d, [类型]%d,[索引]%d,[套接字]%d",
		msg_info.m_type, msg_info.fun_typ, msg_info.index, msg_info.socketFd);
	journal->WriteLog(str_tmp);

	//从前置服务器内读取业务
	memset(&this->first_file_pack, 0, sizeof(first_file_pack));
	f2b_share_memory->ReadShm(&first_file_pack, sizeof(first_file_pack), msg_info.index);

	//进行业务判断
	memset(str_tmp, 0, MAX_CHAR_LEN);
	sprintf(str_tmp, "[后端服务器]进行[写入文件数据库]业务:文件MD5[%s]", first_file_pack.md5);
	journal->WriteLog(str_tmp);

	//如果是第一个包则创建文件夹
	CFileOpt* fileOpt = new CFileOpt();
	file_path += '/' + std::to_string(first_file_pack.user_id) + '/' + first_file_pack.file_name + '/';
	//将.替换为'_'
	replace(file_path.end() - 5, file_path.end(), '.', '_');
	//判断路径在不在
	fstream _file;
	_file.open(file_path, ios::in);
	if (_file || fileOpt->createDirectory(file_path) == 0)
	{
		//写入数据库
		res = CFileTable::WriteFileInfo(first_file_pack.user_id, first_file_pack.fileSize,
			first_file_pack.file_name, first_file_pack.md5);
		if (res != SQLOPT_SUCCESS)
		{
			memset(str_tmp, 0, MAX_CHAR_LEN);
			sprintf(str_tmp, "[后端服务器]文件插入数据库错误, ID:[%d], 文件名[%s]:",
				first_file_pack.user_id, first_file_pack.file_name);
			journal->ErrMsgWriteLog(str_tmp, true);
		}
	}
	else
	{
		memset(str_tmp, 0, MAX_CHAR_LEN);
		sprintf(str_tmp, "[后端服务器]文件路径[创建]失败,[路径]:", file_path.c_str());
		journal->ErrMsgWriteLog(str_tmp, true);
	}

	//写入回应包
	t_Answer ans_pack;
	memset(&ans_pack, 0, sizeof(t_Answer));
	ans_pack.head.packet_type = FILE_ANSWER;
	if (res == SQLOPT_SUCCESS)
	{
		//包的结果设为开始传输文件
		ans_pack.anser_type = ANSWER_FILE_START;
	}
	else
	{
		//包结果设为不要传输文件
		ans_pack.anser_type = ANSWER_FILE_START_ERROR;
	}

	//将响应包写入共享内存, 写入消息队列数组内
	msg_info.index = b2f_share_memory->WriteShm((void*)&ans_pack, sizeof(ans_pack));
	msg_info.fun_typ = ANSWER;                       //应答包
	msg_info.m_type = 2;                             //后端发给前段, 标志位为2

	//发送消息队列数据
	b2f_msg_queue->SendMsg(&this->msg_info, msg_len, 0);
	printf("[准备写入文件应答包]消息队列发送完毕,来自%d发送的类型%d的消息\n", msg_info.m_type, msg_info.fun_typ);

	//判断是否需要回滚操作--文件操作失败,需要回滚
	if (ans_pack.anser_type == ANSWER_FILE_START)
	{
		f2b_msg_queue->ReceiveMsg(&msg_info, msg_len, 3, 0);
		//进行回滚
		if (msg_info.fun_typ == RollBack)
		{
			CFileTable::DeleteFileInfo(first_file_pack.file_name);
			journal->WriteLog("[后端服务器]删除视频插入信息,回滚成功");
		}
	}
}