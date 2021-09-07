#include "CServerJson.h"

bool CServerJson::ReadJson()
{
	assert(doc.HasMember("server_ip_address"));
	assert(doc["server_set"]["server_ip_address"].IsString());

	assert(doc.HasMember("server_port"));
	assert(doc["server_set"]["server_port"].IsInt());

	assert(doc.HasMember("server_protocol_type"));
	assert(doc["server_set"]["server_protocol_type"].IsString());

	assert(doc.HasMember("max_connectNum"));
	assert(doc["server_set"]["max_connectNum"].IsInt());

	assert(doc.HasMember("max_threadNum"));
	assert(doc["server_set"]["max_threadNum"].IsInt());

	//获取心跳的设置相关设置
	return true;
}