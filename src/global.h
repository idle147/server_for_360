#pragma once
#include "CLogger.h"
#include "CServerJson.h"
#include "CShareMemory.h"
#include "CMsgQueue.h"
#include "CUserOpt.h"
#include "global.h"

#define DOC(X) p_cserver_config->doc[X]
#define DOC_GET_INT(X,Y) p_cserver_config->doc[X][Y].GetInt()
#define DOC_GET_STR(X,Y) p_cserver_config->doc[X][Y].GetString()

//消息结构体
typedef struct msg
{
	long m_type;               //定义消息来源, 前段为1, 后端为2
	int fun_typ;               //定义功能的类型
	int index;                 //定义获取的索引地址信息
	int socketFd;              //套接字标识符
}msg_t;

extern CServerJson* p_cserver_config;
extern CLogger* journal;

extern CShareMemory* f2b_share_memory;     //前段发往后端共享内存
extern CShareMemory* b2f_share_memory;     //前段发往后端共享内存

extern CMsgQueue* f2b_msg_queue;          //前段发往后端消息队列
extern CMsgQueue* b2f_msg_queue;          //后端发往前段消息队列

extern int msg_len;                       //消息队列的长度
extern int b2f_msg_len;                       //消息队列的长度

extern CUserOpt* p_database;              //数据库操作
/**************************************************
  * CRC 高位字节值表
**************************************************/
extern const unsigned char auchCRCHi[];
/**************************************************
* CRC 低位字节值表
**************************************************/
extern const unsigned char auchCRCLo[];