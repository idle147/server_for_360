#include "CXORencode.h"

//len是明文的长度
int CXORencode::XORencode(void* i_block, void* o_block, int key, int len)
{
	int* p_src = (int*)i_block;
	int* p_dst = (int*)o_block;
	int i;
	long temp = len / sizeof(int);
	for (i = 0; i < temp; i++)
	{
		p_dst[i] = p_src[i] ^ key;
	}

	int j;
	char* pKey = (char*)&key;
	char* p8Src = (char*)&p_src[i];
	char* p8Dst = (char*)&p_dst[i];
	int temp2 = (int)(len % sizeof(int));
	for (j = 0; j < temp2; j++)
	{
		p8Dst[j] = p8Src[j] ^ pKey[j];
	}

	return 1;
}