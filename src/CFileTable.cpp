#include "CFileTable.h"

int CFileTable::WriteFileInfo(int user_id, int fileSize, char* file_name, char* md5)
{
	//对账号和密码进行加密
	AesEncryptor* aes = new AesEncryptor((unsigned char*)p_database->GetKey());
	string en_id = aes->EncryptString(std::to_string(user_id));
	string en_fileSize = aes->EncryptString(std::to_string(fileSize));
	delete aes;

	//查无账号, 进行注册
	char sql[255];
	snprintf(sql, sizeof(sql), "INSERT INTO pictureTable VALUES(NULL,'%s', '%s', '%s', '%s');",
		file_name, en_id.c_str(), en_fileSize.c_str(), md5);
	bool res = p_database->SqlOpt(sql);
	if (res == true)
	{
		sprintf(sql, "[数据库操作]文件数据插入成功, 账号ID:%d, 文件大小:%d, 文件名:%s, MD5:%s",
			user_id, fileSize, file_name, md5);
		journal->WriteLog(sql);
		return SQLOPT_SUCCESS;
	}
	else
	{
		//数据库内有名字,存在该名字
		sprintf(sql, "[数据库操作]文件数据插入失败, 账号ID:%d, 文件大小:%d, 文件名:%s, MD5:%s",
			user_id, fileSize, file_name, md5);
		journal->ErrMsgWriteLog(sql);
		return ANSWER_NAME_ERROR;
	}

}

int CFileTable::DeleteFileInfo(char* file_name)
{
	//判断数据内是否有此数据
	char sql[SQL_LEN];
	snprintf(sql, sizeof(sql), "DELETE FROM pictureTable WHERE picName = '%s';", file_name);
	int res = p_database->SqlOpt(sql);
	if (res == true)
	{
		sprintf(sql, "[数据库操作]删除文件数据成功, 文件名ID:%d", file_name);
		journal->WriteLog(sql);
		return SQLOPT_SUCCESS;
	}
	else
	{
		sprintf(sql, "[数据库操作]删除文件数据失败, 文件名ID:%d", file_name);
		journal->WriteLog(sql);
		return SQLOPT_ERROR;
	}
}

bool CFileTable::ReadMd5(char* file_name, char* md5)
{
	//sql语句太长可能会导致字符数组sql[]溢出, 从输入的用户名进行长度控制
	int res;
	char sql[255];

	//可否查到此数据
	snprintf(sql, sizeof(sql), "SELECT MD5 FROM pictureTable WHERE picName = '%s';", file_name);
	char** dbResult; //是 char ** 类型，两个*号
	int nRow, nColumn;
	res = p_database->GetSqlValue(sql, dbResult, nRow, nColumn);
	if (res == true)
	{
		if (dbResult[1] != nullptr)
		{
			md5 = dbResult[1];
			return true;
		}
		else
		{
			//查无此数据
			return ANSWER_NAME_ERROR;
		}
	}
	else
	{
		journal->ErrMsgWriteLog("[数据库操作]查询视频表sql语句执行失败", true);
		return SQLOPT_ERROR;
	}
}

bool CFileTable::ReadFileName(char* file_name, char* md5)
{
	//sql语句太长可能会导致字符数组sql[]溢出, 从输入的用户名进行长度控制
	int res;
	char sql[255];

	//可否查到此数据
	snprintf(sql, sizeof(sql), "SELECT picName FROM pictureTable WHERE MD5 = '%s';", md5);
	char** dbResult; //是 char ** 类型，两个*号
	int nRow, nColumn;
	res = p_database->GetSqlValue(sql, dbResult, nRow, nColumn);
	if (res == true)
	{
		if (nRow != 0)
		{
			file_name = dbResult[1];
			return true;
		}
		else
		{
			//查无此数据
			return ANSWER_NAME_ERROR;
		}
	}
	else
	{
		journal->ErrMsgWriteLog("[数据库操作]查询视频表sql语句执行失败", true);
		return SQLOPT_ERROR;
	}
}

bool CFileTable::ReadFileNameAndUid(char* user_id, char* file_name, char* md5)
{
	//sql语句太长可能会导致字符数组sql[]溢出, 从输入的用户名进行长度控制
	int res;
	char sql[255];

	//可否查到此数据
	snprintf(sql, sizeof(sql), "SELECT * FROM pictureTable WHERE MD5 = '%s';", md5);
	char** dbResult; //是 char ** 类型，两个*号
	int nRow, nColumn;
	res = p_database->GetSqlValue(sql, dbResult, nRow, nColumn);
	if (res == true)
	{
		if (dbResult[4] != nullptr)
		{
			file_name = dbResult[4];
			user_id = dbResult[5];
			return true;
		}
		else
		{
			//查无此数据
			return ANSWER_NAME_ERROR;
		}
	}
	else
	{
		journal->ErrMsgWriteLog("[数据库操作]查询视频表sql语句执行失败", true);
		return SQLOPT_ERROR;
	}
}