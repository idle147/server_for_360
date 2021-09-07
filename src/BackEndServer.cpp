#include "BackEndServer.h"

//###############################################################
//后置服务器
//###############################################################
BackEndServer::BackEndServer()
{
	journal->WriteLog("[后置服务器]启动!");
	//创建线程池
	thread_pool = new CThreadPool(DOC_GET_INT("server_set", "max_threadNum"));
	journal->WriteLog("[后置服务器]线程池创建完毕!");

	this->Run();
}
BackEndServer::~BackEndServer()
{
}

void BackEndServer::Run()
{
	//登陆业务
	CLoginAndRegTask* t_Loginask = nullptr;
	//注册业务--myTask子类的指针
	CLoginAndRegTask* reg_task = nullptr;
	//获取视频帧数业务
	CGetVideoFrame* get_videoFram = nullptr;
	//删除视频信息
	CDeleteVideoFrame* del_video_info = nullptr;
	//插入视频信息包
	CInsertVideo* insert_video_info = nullptr;
	//第一个文件包
	CFileFirstTask* file_first_task = nullptr;
	//文件包包头
	CFileTask* file_task = nullptr;
	//消息队列--收到消息的结构体
	msg_t recv_msg;
	//文件路径
	string m_file_path;

	//获取connfd表示的连接上的对端地址
	struct sockaddr_in peer_addr; //监听的客户端地址
	socklen_t sockLen = sizeof(peer_addr);

	while (true)
	{
		//阻塞等待接受来自后端的消息队列---来自前段为1
		memset(&recv_msg, 0, sizeof(msg_t));
		f2b_msg_queue->ReceiveMsg(&recv_msg, msg_len, 1, 0);

		//判断业务逻辑
		switch (recv_msg.fun_typ)
		{
		case LOGIN:
			//登陆业务
			t_Loginask = new CLoginAndRegTask(true, recv_msg);
			thread_pool->AddTask(t_Loginask);
			break;
		case REGISTER:
			//注册业务
			reg_task = new CLoginAndRegTask(false, recv_msg);
			thread_pool->AddTask(reg_task);
			break;
		case REQUIRE_PREPGRORESS:
			//获取视频当前帧数
			get_videoFram = new CGetVideoFrame(recv_msg);
			thread_pool->AddTask(get_videoFram);
			break;
		case DELETE_VIDEO_INFO:
			//删除视频信息
			del_video_info = new CDeleteVideoFrame(recv_msg);
			thread_pool->AddTask(del_video_info);
			break;
		case INSERT_VIDEO_INFO:
			//插入视频信息包
			insert_video_info = new CInsertVideo(recv_msg);
			thread_pool->AddTask(insert_video_info);
			break;
		case  FILE_FIRST_PACK:
			//写入文件表
			m_file_path.clear();
			m_file_path = DOC_GET_STR("picture", "save_path");
			file_first_task = new CFileFirstTask(m_file_path, recv_msg);
			thread_pool->AddTask(file_first_task);
			break;
		case FILE_SEG_EXIST:
			m_file_path.clear();
			m_file_path = DOC_GET_STR("picture", "save_path");
			file_task = new CFileTask(m_file_path, false, recv_msg);
			thread_pool->AddTask(file_task);
			break;
		case FILE_SEG_NULL:
			//片无包业务
			file_task = new CFileTask(m_file_path, true, recv_msg);
			thread_pool->AddTask(file_task);
			break;
		default:
			journal->ErrMsgWriteLog("[后置服务器]收到不明真相的任务请求，结束", true);
			break;
		}
	}
}