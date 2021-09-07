#pragma once
#include "Packet.h"
#include "global.h"
#include "ctool.h"
#include "CCipher.h"
class CVideoInfo
{
public:
	CVideoInfo();
	int GetVideoProgress(int user_id, char* video_id);
	int DeleteVideoInfo(int user_id, char* video_id);
	int InsertVideoInfo(int user_id, char* video_id, int frame_num);
};
