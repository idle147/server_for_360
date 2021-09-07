#pragma once
#include "Packet.h"
#include "global.h"
#include "CCipher.h"

class CLoginAndReg
{
public:
	CLoginAndReg();
	int LoginOpt(int user_name, char* user_key);
	int RegOpt(int user_id, char* user_name, char* user_key);
	int DeletOpt(int user_id);
};
