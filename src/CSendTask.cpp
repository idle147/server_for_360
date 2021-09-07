#include "CSendTask.h"

CSendTask::CSendTask(msg_t m_recv_msg, int& pack_num)
{
	this->recv_msg = m_recv_msg;
	this->pack_num = pack_num;
}

CSendTask::~CSendTask()
{
}

void CSendTask::Run()
{
	char str_tmp[MAX_CHAR_LEN] = { '\0' };
	t_GetProgress get_pack;        //视频包
	t_Answer ans_pack;             //应答包

	//获取connfd表示的连接上的对端地址
	socklen_t sockLen = sizeof(localaddr);
	//发送应答包
	int res = getsockname(recv_msg.socketFd, (struct sockaddr*)&localaddr, &sockLen);
	if (res != 0)
	{
		journal->ErrMsgWriteLog("[发送端]getsockname出错");
	}
	switch (recv_msg.fun_typ)
	{
	case GET_PREGRORESS:
		printf("[发送端]收到发送[视频信息包]的命令\n");
		memset(&get_pack, 0, sizeof(t_GetProgress));
		//前段服务器从后端服务器内读取应答包信息(后端共享内存)
		b2f_share_memory->ReadShm(&get_pack, sizeof(get_pack), recv_msg.index);
		printf("[发送端]从共享内存中读取了视频帧数包\n");
		//发送应答包
		this->SendGetPack(recv_msg.socketFd, get_pack);
		break;
	case ANSWER:
		printf("[发送端]收到发送[应答包]的命令\n");
		//从共享内存内读取应答包信息
		memset(&ans_pack, 0, sizeof(t_Answer));
		b2f_share_memory->ReadShm(&ans_pack, sizeof(ans_pack), recv_msg.index);
		printf("[发送端]从共享内存中读取了应答包\n");
		//发送应答包
		this->SendAns(recv_msg.socketFd, ans_pack);
		break;
	case CRC_ERR_RESEND:
		//CRC错误, 重发包
		printf("[发送端]收到发送[CRC校验码错误包]的命令\n");
		memset(&ans_pack, 0, sizeof(t_Answer));
		ans_pack.anser_type = recv_msg.index;
		ans_pack.head.packet_type = CRC_ERR_RESEND;
		//发送应答包
		this->SendAns(recv_msg.socketFd, ans_pack);
		break;
	default:
		//发送给客户端应答包CRC计算
		memset(str_tmp, '\0', sizeof(str_tmp));
		sprintf(str_tmp, "[发送端]收到未知类型的消息:%d", recv_msg.m_type);
		journal->ErrMsgWriteLog(str_tmp, true);
		break;
	}
}


int CSendTask::SendAns(int fd, t_Answer ans_pack)
{
	char str_tmp[MAX_CHAR_LEN * 2] = { '\0' };
	//发送给客户端应答包CRC计算
	memset(str_tmp, '\0', sizeof(str_tmp));

	//设置发送包信息
	int crc_len = sprintf(str_tmp, "%d", ans_pack.anser_type);
	ans_pack.head.packet_CRC = CCrc::crc16((unsigned char*)str_tmp, crc_len);
	ans_pack.head.packet_size = sizeof(t_Answer);
	ans_pack.head.packet_num = pack_num++;

	//发送给客户端
	int res = send(fd, &ans_pack, sizeof(ans_pack), MSG_NOSIGNAL);
	if (res == -1)
	{
		//写入发送包信息
		memset(str_tmp, '\0', MAX_CHAR_LEN);
		sprintf(str_tmp, "[发送端]客户端[%s:%d][%d]的应答包[%d]发送失败",
			inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port), fd, ans_pack.head.packet_type);
		journal->ErrMsgWriteLog(str_tmp);
		//如果是注册or删除已经成功操作了,则进行回退操作
		msg_t msg_buf;
		msg_buf.fun_typ = RollBack;
		msg_buf.m_type = 3;
		msg_buf.socketFd = fd;
		msg_buf.index = 0;
		if (ans_pack.head.packet_type = REG_ANSWER && ans_pack.anser_type == ANSWER_LOGIN_SUCCESS)
		{
			f2b_msg_queue->SendMsg(&msg_buf, msg_len, 0);
		}
		else if (ans_pack.head.packet_type = DELETE_VIDEO_ANSWER && ans_pack.anser_type == ANSWER_DELETE_SUCCESS)
		{
			f2b_msg_queue->SendMsg(&msg_buf, msg_len, 0);
		}
		else if (ans_pack.head.packet_type = INSERT_VIDEO_ANSWER && ans_pack.anser_type == ANSWER_INSERT_SUCCESS)
		{
			f2b_msg_queue->SendMsg(&msg_buf, msg_len, 0);
		}
		return -1;
	}
	//写入发送包信息
	memset(str_tmp, '\0', MAX_CHAR_LEN * 2);
	sprintf(str_tmp, "[发送端]客户端[%s:%d]的应答包发送成功\n\
					  [发送端]应答包CRC:%d\n   \
					  [发送端]应答包长度:%d\n  \
					  [发送端]应答包类型:%d\n  \
					  [发送端]应答包数据:%d\n",
		inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port),
		ans_pack.head.packet_CRC, ans_pack.head.packet_size, ans_pack.head.packet_type, ans_pack.anser_type);
	journal->WriteLog(str_tmp);
	return res;
}

int CSendTask::SendGetPack(int fd, t_GetProgress get_pack)
{
	char str_tmp[MAX_CHAR_LEN * 2] = { '\0' };

	//设置获取视频帧数包信息
	memset(str_tmp, '\0', sizeof(str_tmp));
	int crclen = sprintf(str_tmp, "%d%d%d", get_pack.user_id, get_pack.video_id, get_pack.video_progress);
	get_pack.head.packet_CRC = CCrc::crc16((unsigned char*)str_tmp, crclen);
	get_pack.head.packet_type = GET_PREGRORESS;
	get_pack.head.packet_size = sizeof(t_GetProgress);
	get_pack.head.packet_num = pack_num++;

	//发送应答包
	int res = send(fd, &get_pack, sizeof(get_pack), MSG_NOSIGNAL);
	if (res < 0)
	{
		//写入发送包信息
		memset(str_tmp, '\0', MAX_CHAR_LEN * 2);
		sprintf(str_tmp, "[发送端]客户端[%s:%d][%d]的视频帧数包[%d]发送失败",
			inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port), fd, get_pack.head.packet_type);
		journal->ErrMsgWriteLog(str_tmp);
		return -1;
	}
	//写入发送包信息
	memset(str_tmp, '\0', MAX_CHAR_LEN * 2);
	sprintf(str_tmp, "[发送端]客户端[%s:%d]视频帧数包发送成功\n\
					  [发送端]视频帧数CRC:%d\n   \
					  [发送端]视频帧数长度:%d\n  \
					  [发送端]视频帧数类型:%d\n  \
					  [发送端]视频帧数用户ID:%d\n\
					  [发送端]视频帧数视频ID:%s\n\
					  [发送端]视频帧数视频帧数:%d\n",
		inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port),
		get_pack.head.packet_CRC, get_pack.head.packet_size, get_pack.head.packet_type,
		get_pack.user_id, get_pack.video_id, get_pack.video_progress);
	journal->WriteLog(str_tmp);
	return 0;
}