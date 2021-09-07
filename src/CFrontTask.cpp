#include "CFrontTask.h"
//前置服务器
CFrontTask::CFrontTask(list<int>& all_Fd, int fd, unsigned int* pack_num) :CTaskBase(nullptr)
{
	//套接字链表
	this->allFd = all_Fd;
	this->srcSocket = fd;
	this->pack_num = pack_num;
}

CFrontTask::~CFrontTask()
{
}

void CFrontTask::Run()
{
	//数值初始化
	t_Login* logMsg = nullptr;                   //登陆结构体
	t_Register* regMsg = nullptr;                //注册结构体
	t_RequireProgress* reqPro = nullptr;         //请求获取数据
	t_VideoInfo* videoInfo = nullptr;            //插入视频信息包
	t_FilePack* filePack = nullptr;              //文件包包头
	t_FirstFilePack* fileFirstPack = nullptr;    //文件第一个包

	//获取套接字的值
	std::list<int>::iterator it = allFd.begin();
	int len = 0;

	//获取connfd表示的连接上的对端地址
	socklen_t sockLen = sizeof(peer_addr);
	int ret = getsockname(srcSocket, (struct sockaddr*)&peer_addr, &sockLen);
	if (ret != 0)
	{
		journal->ErrMsgWriteLog("[获取IP地址]前置服务器,getsockname出错");
	}

	//字符串计算
	char str_tmp[MAX_CHAR_LEN] = { '\0' };

	//接收客户端数据--非阻塞接收(用心跳来判断是否有超时)
	unsigned char chr_head[sizeof(t_PackHead)];        //接收包头
	unsigned char chr_body[PACK_MAX_LEN];              //接收包体
	memset(chr_head, '\0', sizeof(chr_head));
	memset(chr_body, '\0', sizeof(chr_body));

	//接收包头数据
	len = (int)recv(srcSocket, chr_head, sizeof(t_PackHead), 0);
	memset(str_tmp, '\0', MAX_CHAR_LEN);
	sprintf(str_tmp, "[前置服务器]接收到客户端[%s:%d][%d]的数据", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), srcSocket);

	journal->WriteLog(str_tmp);
	if (len < 0)
	{
		memset(str_tmp, '\0', MAX_CHAR_LEN);
		sprintf(str_tmp, "客户端[%s:%d][%d]", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), srcSocket);
		switch (errno)
		{
		case EINTR:
			journal->ErrMsgWriteLog(str_tmp, "接收数据发生中断,移除该连接");
			break;
		case ECONNRESET:
			journal->ErrMsgWriteLog(str_tmp, "已崩溃,移除该连接");
			break;
		case ETIMEDOUT:
			journal->ErrMsgWriteLog(str_tmp, "连接超时,移除该连接");
			break;
		case EHOSTUNREACH:
			journal->ErrMsgWriteLog(str_tmp, "对端主机不可达,移除该连接");
			break;
		default:
			journal->ErrMsgWriteLog(str_tmp, "发生错误,移除该连接");
			break;
		}
		//从Fd链表内移除该套接字标识符
		allFd.remove(srcSocket);
	}
	else if (len == 0)
	{
		//从Fd链表内移除该链表套接字标识符
		allFd.remove(srcSocket);
		memset(str_tmp, '\0', MAX_CHAR_LEN);
		sprintf(str_tmp, "[前置服务器]客户端[%s:%d][%d]已退出", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), srcSocket);
		journal->WriteLog(str_tmp);
		return;
	}
	//给后端服务器去处理
	else
	{
		//类型强制转换
		pack_head = (t_PackHead*)chr_head;
		memset(str_tmp, '\0', MAX_CHAR_LEN);
		sprintf(str_tmp, "[前置服务器]收到的包头长度为%d", len);
		journal->WriteLog(str_tmp);
		memset(str_tmp, '\0', MAX_CHAR_LEN);
		sprintf(str_tmp, "[前置服务器]收到的包头类型为%d", pack_head->packet_type);
		journal->WriteLog(str_tmp);
		memset(str_tmp, '\0', MAX_CHAR_LEN);
		sprintf(str_tmp, "[前置服务器]收到的包头CRC校验码为%d", pack_head->packet_CRC);
		journal->WriteLog(str_tmp);
		memset(str_tmp, '\0', MAX_CHAR_LEN);
		sprintf(str_tmp, "[前置服务器]收到的包头的编号为%d", pack_head->packet_num);
		journal->WriteLog(str_tmp);
		memset(str_tmp, '\0', MAX_CHAR_LEN);
		sprintf(str_tmp, "[前置服务器]收到的包体长度为%d", pack_head->packet_size);
		journal->WriteLog(str_tmp);

		//判断需不需要丢包
		if (pack_head->packet_num < pack_num[srcSocket])
		{
			memset(str_tmp, '\0', MAX_CHAR_LEN);
			sprintf(str_tmp, "[前置服务器]收到编号%d,判断为重复包,丢弃", pack_head->packet_num);
			journal->ErrMsgWriteLog(str_tmp);
			return;
		}
		printf("重复包检测收到包号%d, 本地包号:%d\n", pack_head->packet_num, pack_num[srcSocket]);
		//否则不需要丢包,该连接收到的包数加1
		pack_num[srcSocket]++;

		//解析业务
		printf("开始解析数据包业务\n");
		//超时设置
		recv(srcSocket, chr_body, pack_head->packet_size - sizeof(t_PackHead), 0);
		switch (pack_head->packet_type)
		{
		case FILE_FIRST_PACK:
			//文件发包, 第一个文件
			fileFirstPack = (t_FirstFilePack*)chr_body;
			//进行CRC校验
			memset(str_tmp, '\0', sizeof(str_tmp));
			len = sprintf(str_tmp, "%d%d%s%s", fileFirstPack->user_id, fileFirstPack->fileSize,
				fileFirstPack->file_name, fileFirstPack->md5);
			//CRC校验码出错, 发送请求重发包
			if (CrcCheck((unsigned char*)str_tmp, len, srcSocket, RESEND_FILE_FIRST_PACK) == false)
				return;
			//[文件包首包]接收成功
			memset(str_tmp, '\0', sizeof(str_tmp));
			sprintf(str_tmp, "[前置服务器]接收客户端[%s:%d]的[文件首包]业务,用户ID:%d",
				inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), fileFirstPack->user_id);
			journal->WriteLog(str_tmp);
			//共享内存和消息队列操作
			this->TaskSendMsg((void*)fileFirstPack, sizeof(fileFirstPack), FILE_FIRST_PACK, srcSocket);
			break;
		case FILE_SEG_EXIST:
			//文件发包, 片有包
			filePack = (t_FilePack*)chr_body;
			printf("文件包流水号:%d", filePack->packer_num);
			FileSegOpt(filePack, srcSocket, FILE_SEG_EXIST);
			break;
		case FILE_SEG_NULL:
			//文件发包, 片无包
			filePack = (t_FilePack*)chr_body;
			printf("文件包流水号:%d", filePack->packer_num);
			FileSegOpt(filePack, srcSocket, FILE_SEG_NULL);
			break;
		case LOGIN:
			//收到登陆包, 强制指针类型转换
			logMsg = (t_Login*)chr_body;
			//CRC校验码出错, 发送请求重发包
			memset(str_tmp, '\0', sizeof(str_tmp));
			len = sprintf(str_tmp, "%d%s", logMsg->user_id, logMsg->pwd);
			if (CrcCheck((unsigned char*)str_tmp, len, srcSocket, RESEND_LOGIN) == false)
				return;
			//登陆业务, 获取数据
			memset(str_tmp, '\0', sizeof(str_tmp));
			sprintf(str_tmp, "[前置服务器]接收客户端[%s:%d]的[登陆]业务,账号:%d, 密码:%s",
				inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), logMsg->user_id, logMsg->pwd);
			journal->WriteLog(str_tmp);
			//共享内存和消息队列操作
			this->TaskSendMsg((void*)logMsg, sizeof(logMsg), LOGIN, srcSocket);
			break;
		case REGISTER:
			//收到注册包强制指针转换
			regMsg = (t_Register*)chr_body;
			//CRC校验码出错, 发送请求重发包
			memset(str_tmp, '\0', sizeof(str_tmp));
			len = sprintf(str_tmp, "%d%s%s", regMsg->user_id, regMsg->name, regMsg->pwd);
			if (CrcCheck((unsigned char*)str_tmp, len, srcSocket, RESEND_REGISTER) == false)
				return;
			//注册业务, 获取数据
			memset(str_tmp, '\0', sizeof(str_tmp));
			sprintf(str_tmp, "[前置服务器]接收客户端[%s:%d][注册]业务,名称:%s, 账号:%d, 密码:%s",
				inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), regMsg->name, regMsg->user_id, regMsg->pwd);
			journal->WriteLog(str_tmp);
			//共享内存和信号队列操作
			this->TaskSendMsg((void*)regMsg, sizeof(regMsg), REGISTER, srcSocket);
			break;
		case REQUIRE_PREPGRORESS:
			//收到请求获取视频帧数包, 强制指针类型转换
			reqPro = (t_RequireProgress*)chr_body;
			//CRC校验码出错, 发送请求重发包
			memset(str_tmp, '\0', sizeof(str_tmp));
			len = sprintf(str_tmp, "%d%s", reqPro->user_id, reqPro->video_id);
			if (CrcCheck((unsigned char*)str_tmp, len, srcSocket, RESEND_REQUIRE_PREPGRORESS) == false)
				return;
			//请求获取视频当前帧数包
			memset(str_tmp, '\0', sizeof(str_tmp));
			sprintf(str_tmp, "[前置服务器]接收到[获取帧数]业务, 视频ID:%s, 用户ID:%d",
				reqPro->video_id, reqPro->user_id);
			journal->WriteLog(str_tmp);
			//共享内存和信号队列操作
			this->TaskSendMsg((void*)reqPro, sizeof(reqPro), REQUIRE_PREPGRORESS, srcSocket);
			break;
		case DELETE_VIDEO_INFO:
			//收到删除视频信息包, 强制指针类型转换
			reqPro = (t_RequireProgress*)chr_body;
			//CRC校验码出错, 发送请求重发包
			memset(str_tmp, '\0', sizeof(str_tmp));
			len = sprintf(str_tmp, "%d%s", reqPro->user_id, reqPro->video_id);
			if (CrcCheck((unsigned char*)str_tmp, len, srcSocket, RESEND_DELETE) == false)
				return;
			//请求获取视频当前帧数包
			memset(str_tmp, '\0', sizeof(str_tmp));
			sprintf(str_tmp, "[前置服务器]接收到[删除视频信息]业务, 视频ID:%s, 用户ID:%d",
				reqPro->video_id, reqPro->user_id);
			journal->WriteLog(str_tmp);
			//共享内存和信号队列操作
			this->TaskSendMsg((void*)reqPro, sizeof(reqPro), DELETE_VIDEO_INFO, srcSocket);
			break;
		case INSERT_VIDEO_INFO:
			//获取插入视频信息包
			videoInfo = (t_VideoInfo*)chr_body;
			//CRC校验码出错, 发送请求重发包
			memset(str_tmp, '\0', sizeof(str_tmp));
			len = sprintf(str_tmp, "%d%s%d", videoInfo->user_id, videoInfo->video_id, videoInfo->current_frame);
			if (CrcCheck((unsigned char*)str_tmp, len, srcSocket, RESEND_INSERT) == false)
				return;
			//请求获取视频当前帧数包
			memset(str_tmp, '\0', sizeof(str_tmp));
			sprintf(str_tmp, "[前置服务器]接收到[插入视频信息]业务, 视频ID:%s, 用户ID:%d, 帧数:%d",
				videoInfo->video_id, videoInfo->user_id, videoInfo->current_frame);
			journal->WriteLog(str_tmp);
			//共享内存和信号队列操作
			this->TaskSendMsg((void*)videoInfo, sizeof(videoInfo), INSERT_VIDEO_INFO, srcSocket);
			break;
		default:
			journal->WriteLog("[前置服务器]收到包头类型未知的包, 丢弃");
			break;
		}
	}
}

void CFrontTask::TaskSendMsg(void* buf, int buf_len, int type, int srcSocket)
{
	/*
		告诉后端服务器, 你要处理业务了
		flag来自前端为1
		重发设置为2
	*/
	//发送消息的结构体
	msg_t send_msg;
	memset(&send_msg, 0, sizeof(msg_t));
	//发送消息队列结构体初始化
	send_msg.index = f2b_share_memory->WriteShm(buf, buf_len);
	send_msg.m_type = 1;
	send_msg.fun_typ = type;
	send_msg.socketFd = srcSocket;
	//前段服务器发往后端服务器
	f2b_msg_queue->SendMsg(&send_msg, msg_len, 0);
	printf("消息队列发送完毕,来自%d发送的类型%d的消息,标识符为%d\n", send_msg.m_type, send_msg.fun_typ, send_msg.socketFd);
}

void CFrontTask::TaskSendMsg(int type, int srcSocket, int index)
{
	/*
		告诉后端服务器, 你要处理业务了
	*/
	//发送消息的结构体
	msg_t send_msg;
	memset(&send_msg, 0, sizeof(msg_t));
	//定义索引指针
	//发送消息队列结构体初始化
	send_msg.m_type = 2; //重发设置为2
	send_msg.fun_typ = type;
	send_msg.socketFd = srcSocket;
	send_msg.index = index;
	//发送消息
	b2f_msg_queue->SendMsg(&send_msg, msg_len, 0);
	printf("重发包消息队列发送完毕, 类型%d的消息,标识符为%d\n", send_msg.fun_typ, send_msg.socketFd);
}

void CFrontTask::FileSegOpt(t_FilePack* filePack, int socketfd, int type)
{
	char str_tmp[MAX_CHAR_LEN] = { '\0' };
	//[文件包]接收成功
	memset(str_tmp, '\0', sizeof(str_tmp));
	sprintf(str_tmp, "[前置服务器]接收客户端[%s:%d]的[文件包]业务,包号:%d",
		inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), filePack->packer_num);
	journal->WriteLog(str_tmp);
	//共享内存和消息队列操作
	this->TaskSendMsg((void*)filePack, sizeof(filePack), type, socketfd);
}

bool CFrontTask::CrcCheck(unsigned char* str, int len, int srcSocket, int resent_type)
{
	//CRC校验
	char str_tmp[MAX_CHAR_LEN] = { '\0' };
	int crc_num = CCrc::crc16(str, len);
	printf("本地计算CRC的值为:%d\n", crc_num);
	//CRC校验码出错
	if (pack_head->packet_CRC != crc_num)
	{
		memset(str_tmp, '\0', sizeof(str_tmp));
		sprintf(str_tmp, "[前置服务器]接收客户端[%s:%d]的[%d]业务,CRC校验出错",
			inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), resent_type);
		journal->ErrMsgWriteLog(str_tmp, true);
		//消息队列操作, 请求重发相对应的包
		this->TaskSendMsg(CRC_ERR_RESEND, srcSocket, resent_type);
		return false;
	}
	return true;
}