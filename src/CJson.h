#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <unistd.h>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#define PRINT(x) std::cout << #x " = " << (x) << std::endl
class CJson
{
public:
	//构造析构函数
	CJson(const char* json_add);
	~CJson();

	//打开JSON文件
	void OpenJson(const char* json_add);
	//解析JSON文件
	void ParseJson();
	//读取JSON文件
	virtual bool ReadJson() = 0;
	//查询所有JSON键值对
	void QueryAll();
	//检查成员是否存在并返回其值
	const char* CheckObj(const char* obj_name);
	rapidjson::Document doc;

private:
	std::string str_from_stream;//字符串流
	std::ifstream in;	        //文件流
};
