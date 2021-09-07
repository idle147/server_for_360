#pragma once
#include "CJson.h"
#include "CSigleton.h"

class CServerJson : public CJson
{
public:
	CServerJson(const char* json_add) :CJson(json_add) {};
	bool ReadJson();

private:
	CServerJson(const CServerJson&);
};
