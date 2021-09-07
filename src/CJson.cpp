#include "CJson.h"

CJson::CJson(const char* json_add)
{
	this->OpenJson(json_add);
	this->ParseJson();
}

CJson::~CJson()
{
}

void CJson::OpenJson(const char* json_add)
{
	//以只读的方式打这个文件
	printf(json_add);
	in.open(json_add, std::ifstream::in);
	//如果打开成功,则一行一行读取数据
	if (in.is_open())
	{
		std::string line;
		while (getline(in, line))
		{
			//把读取到的数据添加到另一个string中,并且在结尾上加上换行符
			str_from_stream.append(line + '\n');
		}
		std::cout << str_from_stream << std::endl;
	}
	//如果打开失败,则返回
	else
	{
		perror("读取配置文件错误, 退出程序\n");
		system("pause");
		exit(-1);
	}
}

void CJson::ParseJson()
{
	//解析json
	doc.Parse<0>(str_from_stream.c_str());

	//如果解析错误的话
	if (doc.HasParseError())
	{
		perror("解析Json失败, 退出程序\n");
		system("pause");
		exit(-1);
	}
	printf("解析Json成功\n");
}

void CJson::QueryAll()
{
	static const char* kTypeNames[] =
	{ "Null", "False", "True", "Object", "Array", "String", "Number" };

	for (auto& m : doc.GetObject())
	{
		printf("Type of member %s is %s\n",
			m.name.GetString(), kTypeNames[m.value.GetType()]);
	}
}

const char* CJson::CheckObj(const char* obj_name)
{
	rapidjson::Value::ConstMemberIterator itr = doc.FindMember(obj_name);
	if (itr != doc.MemberEnd())
	{
		return itr->value.GetString();
	}
	return "is_null";
}