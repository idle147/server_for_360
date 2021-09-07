#include "CVideoInfo.h"

CVideoInfo::CVideoInfo()
{
}

int CVideoInfo::GetVideoProgress(int user_id, char* video_id)
{
	//对账号和密码进行加密
	AesEncryptor* aes = new AesEncryptor((unsigned char*)p_database->GetKey());
	string en_id = aes->EncryptString(std::to_string(user_id));
	string en_video_id = aes->EncryptString(video_id);
	delete aes;

	//sql语句太长可能会导致字符数组sql[]溢出, 从输入的用户名进行长度控制
	int res;
	char sql[255];

	//可否查到此数据
	snprintf(sql, sizeof(sql), "SELECT currentFrame FROM videoTable WHERE id = '%s' AND uId = '%s';",
		en_video_id.c_str(), en_id.c_str());
	char** dbResult; //是 char ** 类型，两个*号
	int nRow, nColumn;
	res = p_database->GetSqlValue(sql, dbResult, nRow, nColumn);
	if (res == true)
	{
		if (nRow != 0)
		{
			//res = CTool::char2int(dbResult[1]);
			return atoi(dbResult[1]);
		}
		else
		{
			//查无此数据
			return ANSWER_NAME_ERROR;
		}
	}
	else
	{
		journal->ErrMsgWriteLog("[数据库操作]查询帧数sql语句执行失败", true);
		return SQLOPT_ERROR;
	}
}

int CVideoInfo::DeleteVideoInfo(int user_id, char* video_id)
{
	//对账号和密码进行加密
	AesEncryptor* aes = new AesEncryptor((unsigned char*)p_database->GetKey());
	string en_id = aes->EncryptString(std::to_string(user_id));
	string en_video_id = aes->EncryptString(video_id);
	delete aes;

	char sql[SQL_LEN];
	//查询有无此包
	int res = this->GetVideoProgress(user_id, video_id);
	if (res == ANSWER_NAME_ERROR)
	{
		return ANSWER_NAME_ERROR;
	}
	snprintf(sql, sizeof(sql), "DELETE FROM videoTable WHERE id = '%s' AND uId = '%s';",
		en_video_id.c_str(), en_id.c_str());
	res = p_database->SqlOpt(sql);
	if (res == false)
	{
		sprintf(sql, "[数据库操作]数据删除失败:视频ID[%s],用户ID[%d]", video_id, user_id);
		journal->ErrMsgWriteLog(sql);
		return SQLOPT_ERROR;
	}
	sprintf(sql, "[数据库操作]数据删除成功:视频ID[%s],用户ID[%d]", video_id, user_id);
	journal->WriteLog(sql);
	return SQLOPT_SUCCESS;
}

int CVideoInfo::InsertVideoInfo(int user_id, char* video_id, int frame_num)
{
	//对账号和密码进行加密
	AesEncryptor* aes = new AesEncryptor((unsigned char*)p_database->GetKey());
	string en_id = aes->EncryptString(std::to_string(user_id));
	//string en_frame_num = aes->EncryptString(std::to_string(frame_num));
	string en_video_id = aes->EncryptString(video_id);
	delete aes;

	char sql[SQL_LEN];
	snprintf(sql, sizeof(sql), "INSERT INTO videoTable VALUES('%s', '%s', %d);",
		en_video_id.c_str(), en_id.c_str(), frame_num);
	int res = p_database->SqlOpt(sql);
	if (res == false)
	{
		sprintf(sql, "[数据库操作]数据插入失败:视频ID[%s],用户ID[%d],帧数[%d]",
			video_id, user_id, frame_num);
		journal->ErrMsgWriteLog(sql);
		return ANSWER_NAME_ERROR;
	}
	sprintf(sql, "[数据库操作]数据插入成功:视频ID[%s],用户ID[%d],帧数[%d]", video_id, user_id, frame_num);
	journal->WriteLog(sql);
	return SQLOPT_SUCCESS;
}