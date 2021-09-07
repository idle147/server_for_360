#pragma once
#include "CCipher.h"
#include "global.h"
#include "Packet.h"
class CFileTable
{
public:
	static int WriteFileInfo(int user_id, int fileSize, char* file_name, char* md5);
	static int DeleteFileInfo(char* file_name);
	static bool ReadMd5(char* file_name, char* md5);
	static bool ReadFileName(char* file_name, char* md5);
	static bool ReadFileNameAndUid(char* user_id, char* file_name, char* md5);
};
