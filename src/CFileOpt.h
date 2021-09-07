#pragma once
#include <string>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

#include "global.h"
#include "Packet.h"
using namespace std;

enum FILE_OPENTYPE
{
	ReadOnly = 0,//仅读 rb
	WriteOnly,//仅写 //ab 不存在会创建文件
	ReadWrite,//读写 ab+ 不存在会创建文件
	UnKnow
};

class CFileOpt
{
public:
	CFileOpt() {};
	CFileOpt(string& m_file_path, FILE_OPENTYPE RW_mode);
	//设置参数
	void SetFilePara(std::string path, FILE_OPENTYPE RW_mode)
	{
		this->file_path = path;
		this->file_mode = this->getType(RW_mode);
	}
	//设置路径
	void SetFilePath(std::string path)
	{
		this->file_path = path;
	}
	//判断文件打开, 未打开就打开,打开就创建文件
	bool FileOpen();
	bool FileOpen(std::string path, FILE_OPENTYPE RW_mode, FILE* file_fp);

	//关闭文件
	bool FileClose();
	//写文件
	bool WriteFile(char* str, int len);
	bool WriteFile(FILE* file_fp, char* str, int len);

	//读文件
	int ReadFile(char* str, int len);
	//判断文件是否存在
	bool IsExist();
	bool IsExist(string& str);
	//删除文件
	bool DeleteFile();
	bool DeleteFile(string& path);
	//删除目录及其目录下的所有文件
	static int DeleteDir(const char* path);

	//创建文件夹
	bool Mkdir(const char* pathname);
	//创建多级目录
	int32_t createDirectory(const std::string& directoryPath);

private:
	//获取文件标识符
	char* getType(FILE_OPENTYPE type);
	//遍历目录
	static void Getfilepath(const char* path, const char* filename, char* filepath)
	{
		strcpy(filepath, path);
		if (filepath[strlen(path) - 1] != '/')
			strcat(filepath, "/");
		strcat(filepath, filename);
		printf("path is = %s\n", filepath);
	}
	string file_path;    //文件路径
	char* file_mode;          //文件打开类型
	FILE* fp;            //文件描述符号
	bool isOpen;         //文件打开标识符
};
