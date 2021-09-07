#include "CDataBaseOpt.h"

CDataBaseOpt::~CDataBaseOpt()
{
	this->CloseDB();
}

bool CDataBaseOpt::OpenDB(const char* db_add)
{
	//打开数据库
	int res = sqlite3_open(db_add, &this->db);
	if (res)
	{
		fprintf(stderr, "打开数据库出错： %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return false;
	}
	return true;
}

bool CDataBaseOpt::checkKey(char* ciphertext)
{
	char sql[255];
	sprintf(sql, "SELECT * From key_table WHERE key_num = '%s';", ciphertext);
	char** dataRes = nullptr;
	int row, col;
	this->GetSqlValue(sql, dataRes, row, col);
	if (row != 0)
	{
		this->pKey = ciphertext;
		return true;
	}
	return false;
}

bool CDataBaseOpt::ReSetKey(char* new_key)
{
	char sql[255];
	sprintf(sql, "UPDATE key_table SET key_num = '%s';", new_key);
	if (this->SqlOpt(sql))
	{
		this->pKey = new_key;
		return true;
	}
	return false;
}

bool CDataBaseOpt::CloseDB()
{
	int res = sqlite3_close(this->db);
	if (res == SQLITE_OK)
	{
		printf("数据库关闭成功\n");
	}
	fprintf(stderr, "数据库关闭成功： %s\n", sqlite3_errmsg(db));
	return false;
}

bool CDataBaseOpt::SqlOpt(char* sql)
{
	char* zErrMsg = 0;
	int res = sqlite3_exec(this->db, sql, 0, 0, &zErrMsg);
	if (res != SQLITE_OK)
	{
		printf("[数据库操作]SQL语句操作失败:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

bool CDataBaseOpt::GetSqlValue(const char* sql, char**& dataRes, int& row, int& col)
{
	char* zErrMsg = 0;
	int res = sqlite3_get_table(this->db, sql, &dataRes, &row, &col, &zErrMsg);
	if (res == SQLITE_OK)
	{
		return true;
	}
	printf("[数据库操作]SQL语句操作失败:%s\n", zErrMsg);
	sqlite3_free(zErrMsg);
	return false;
}