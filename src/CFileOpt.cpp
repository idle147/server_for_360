#include "CFileOpt.h"

CFileOpt::CFileOpt(string& m_file_path, FILE_OPENTYPE RW_mode)
{
	this->file_path = m_file_path;
	this->file_mode = this->getType(RW_mode);
	this->fp = nullptr;
	isOpen = false;
	this->FileOpen();
}

bool CFileOpt::FileOpen()
{
	this->fp = fopen(file_path.c_str(), file_mode);
	if (this->fp == nullptr)
	{
		printf("file cannot open \n");
		return false;
	}
	isOpen = true;
	return true;
}

bool CFileOpt::FileOpen(std::string path, FILE_OPENTYPE RW_mode, FILE* file_fp)
{
	file_fp = fopen(path.c_str(), this->getType(RW_mode));
	if (file_fp == nullptr)
	{
		printf("file cannot open \n");
		return false;
	}
	isOpen = true;
	return true;
}

bool CFileOpt::FileClose()
{
	if (this->fp != nullptr)
	{
		fclose(this->fp);
		isOpen = false;
	}
}

bool CFileOpt::WriteFile(char* str, int len)
{
	if (this->fp == nullptr)
	{
		return false;
	}
	int retLen = fwrite(str, 1, len, this->fp);
	if (retLen == len)
	{
		//文件标识符设置为开头
		if (this->file_mode == "ab+")
		{
			rewind(fp);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CFileOpt::WriteFile(FILE* file_fp, char* str, int len)
{
	if (file_fp == nullptr)
	{
		return false;
	}
	int retLen = fwrite(str, len, 1, this->fp);
	if (retLen == len)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int CFileOpt::ReadFile(char* str, int len)
{
	if (this->fp == nullptr)
	{
		return false;
	}
	int retLen = fread(str, 1, len, fp);
	return retLen;
}

bool CFileOpt::IsExist()
{
	if (!isOpen)//未打开
	{
		if ((this->fp = fopen(this->file_path.c_str(), "r")) == NULL)
		{
			return false;
		}
		else
		{
			fclose(this->fp);
			return true;
		}
	}
	return true;
}

bool CFileOpt::IsExist(string& str)
{
	FILE* temp_fp;
	if ((temp_fp = fopen(str.c_str(), "r")) == NULL)
	{
		return false;
	}
	else
	{
		fclose(temp_fp);
		return true;
	}
}

bool CFileOpt::DeleteFile()
{
	if (remove(this->file_path.c_str()) == 0)

		return true;
	else
		return false;
}

bool CFileOpt::DeleteFile(string& path)
{
	if (remove(path.c_str()) == 0)

		return true;
	else
		return false;
}

int CFileOpt::DeleteDir(const char* path)
{
	DIR* dir;
	struct dirent* dirinfo;
	struct stat statbuf;
	char filepath[256] = { 0 };
	lstat(path, &statbuf);

	if (S_ISREG(statbuf.st_mode))//判断是否是常规文件
	{
		remove(path);
	}
	else if (S_ISDIR(statbuf.st_mode))//判断是否是目录
	{
		if ((dir = opendir(path)) == NULL)
			return 1;
		while ((dirinfo = readdir(dir)) != NULL)
		{
			Getfilepath(path, dirinfo->d_name, filepath);
			if (strcmp(dirinfo->d_name, ".") == 0 || strcmp(dirinfo->d_name, "..") == 0)//判断是否是特殊目录
				continue;
			DeleteDir(filepath);
			rmdir(filepath);
		}
		closedir(dir);
	}
	return 0;
}

int32_t CFileOpt::createDirectory(const std::string& directoryPath)
{
	uint32_t dirPathLen = directoryPath.length();
	if (dirPathLen > MAX_PATH_LEN)
	{
		journal->ErrMsgWriteLog("文件路径超过最大长度, 创建失败", true);
		return -1;
	}
	char tmpDirPath[MAX_PATH_LEN] = { 0 };
	for (uint32_t i = 0; i < dirPathLen; ++i)
	{
		tmpDirPath[i] = directoryPath[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (access(tmpDirPath, 0) != 0)
			{
				int32_t ret = mkdir(tmpDirPath, 0777);
				if (ret != 0)
				{
					return ret;
				}
			}
		}
	}
	return 0;
}

bool CFileOpt::Mkdir(const char* pathname)
{
	//判断文件夹是否存在,且可读写
	int res = access(pathname, 0777);
	if (!res)
	{
		//文件存在且具有操作权限
		return true;
	}
	res = mkdir(pathname, 0777);
	if (!res)
	{
		journal->WriteLog("[临时文件夹]创建成功,[路径]:", pathname);
		return true;
	}
	journal->WriteLog("[临时文件夹]创建失败,[路径]:", pathname);
	return false;
}

char* CFileOpt::getType(FILE_OPENTYPE type)
{
	if (type == ReadOnly)
	{
		return "rb";
	}
	else if (type == WriteOnly)
	{
		return "ab";
	}
	else if (type == ReadWrite)
	{
		return "ab+";
	}
}