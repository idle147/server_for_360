#pragma once
#include "CDataBaseOpt.h"
class CUserOpt : public CDataBaseOpt
{
public:
	//构造函数
	CUserOpt(const char* db_add, char* pKey) :CDataBaseOpt(db_add, pKey)
	{
		printf("数据库初始化完毕\n");
	}
};
