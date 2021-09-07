#include "CLoginAndReg.h"
CLoginAndReg::CLoginAndReg()
{
}

int CLoginAndReg::LoginOpt(int user_name, char* user_key)
{
	//对账号和密码进行加密
	AesEncryptor* aes = new AesEncryptor((unsigned char*)p_database->GetKey());
	string en_name = aes->EncryptString(std::to_string(user_name));
	string en_key = aes->EncryptString(user_key);

	delete aes;

	//sql语句太长可能会导致字符数组sql[]溢出, 从输入的用户名进行长度控制
	char sql[SQL_LEN];
	memset(sql, '\0', sizeof(sql));
	sprintf(sql, "SELECT * FROM usr WHERE uId = '%s';", en_name.c_str());
	char** dbResult; //是 char ** 类型，两个*号
	int nRow, nColumn;
	if (p_database->GetSqlValue(sql, dbResult, nRow, nColumn) == true)
	{
		//等于null表示查无此账号
		if (nRow == 0)
		{
			sprintf(sql, "[数据库操作]账号查询出错, 账号ID:%d", user_name);
			journal->WriteLog(sql);
			return ANSWER_NAME_ERROR;
		}
	}
	//查找到账号后判断密码是否正确
	memset(sql, '\0', sizeof(sql));
	sprintf(sql, "SELECT * FROM usr WHERE uId = '%s' AND pwd = \'%s\';", en_name.c_str(), en_key.c_str());
	if (p_database->GetSqlValue(sql, dbResult, nRow, nColumn) == true)
	{
		//判断是否能查到数据
		if (nRow == 0)
		{
			sprintf(sql, "[数据库操作]密码查询错误, 账号ID:%d,密码:%s", user_name, user_key);
			journal->WriteLog(sql);
			return ANSWER_KEY_ERROR;
		}
		else
		{
			sprintf(sql, "[数据库操作]查询成功, 账号ID:%d, 密码:%s", user_name, user_key);
			journal->WriteLog(sql);
			return SQLOPT_SUCCESS;
		}
	}
	journal->ErrMsgWriteLog("[数据库操作]sql语句执行失败:", sql);
	return SQLOPT_ERROR;
}

int CLoginAndReg::RegOpt(int user_id, char* user_name, char* user_key)
{
	//对账号和密码进行加密
	AesEncryptor* aes = new AesEncryptor((unsigned char*)p_database->GetKey());
	string en_id = aes->EncryptString(std::to_string(user_id));
	string en_name = aes->EncryptString(user_name);
	string en_key = aes->EncryptString(user_key);
	delete aes;

	//判断数据内是否有此数据
	int res = LoginOpt(user_id, user_key);
	char sql[SQL_LEN];
	//返回名字错误,表示不存在该名字,可以进行注册操作
	if (res == ANSWER_NAME_ERROR)
	{
		//查无账号, 进行注册
		snprintf(sql, sizeof(sql), "INSERT INTO usr VALUES('%s', '%s', '%s', 1);",
			en_id.c_str(), en_name.c_str(), en_key.c_str());
		res = p_database->SqlOpt(sql);
		if (res == true)
		{
			sprintf(sql, "[数据库操作]用户数据插入成功, 账号ID:%d, 用户名:%s, 密码:%s",
				user_id, user_name, user_key);
			journal->WriteLog(sql);
			return SQLOPT_SUCCESS;
		}
	}
	//数据库内有名字,存在该名字
	sprintf(sql, "[数据库操作]用户数据插入失败, 账号ID:%d, 用户名:%s, 密码:%s",
		user_id, user_name, user_key);
	journal->ErrMsgWriteLog(sql);
	return ANSWER_NAME_ERROR;
}

int CLoginAndReg::DeletOpt(int user_id)
{
	//对账号和密码进行加密
	AesEncryptor* aes = new AesEncryptor((unsigned char*)p_database->GetKey());
	string en_id = aes->EncryptString(std::to_string(user_id));
	delete aes;

	//判断数据内是否有此数据
	char sql[SQL_LEN];
	snprintf(sql, sizeof(sql), "DELETE FROM usr WHERE uId = '%s';", en_id.c_str());
	int res = p_database->SqlOpt(sql);
	if (res == true)
	{
		sprintf(sql, "[数据库操作]删除用户数据成功, 账号ID:%d", user_id);
		journal->WriteLog(sql);
		return SQLOPT_SUCCESS;
	}
	else
	{
		return SQLOPT_ERROR;
	}
}