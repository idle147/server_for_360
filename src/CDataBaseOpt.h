#pragma once
#include <cstdio>
#include <cstring>
#include <assert.h>
#include "sqlite3.h"

class CDataBaseOpt
{
public:
	CDataBaseOpt(const char* db_add, char* pKey)
	{
		this->pKey = pKey;
		assert(OpenDB(db_add));
	};
	//析构函数,关系数据库
	~CDataBaseOpt();

	//打开数据库
	bool OpenDB(const char* db_add);
	//打开加密的数据库
	bool checkKey(char* ciphertext);
	//重设密钥密码
	bool ReSetKey(char* new_key);
	//过去密钥密码
	char* GetKey() { return pKey; };
	//关闭数据库
	bool CloseDB();
	//注册表执行操作
	bool SqlOpt(char* sql);
	//获取sql结果的值
	bool GetSqlValue(const char* sql, char**& dataRes, int& row, int& col);
	//获得数据库指针
	sqlite3* getDB() {
		return this->db;
	}
	//获取状态标识
	sqlite3_stmt* getStmt() {
		return this->pStmt;
	}

protected:
	char* pKey;
	sqlite3* db;         //数据库指针
	sqlite3_stmt* pStmt; //状态标识
};
