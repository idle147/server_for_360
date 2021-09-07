#pragma once
#include "global.h"
class CCrc
{
public:
	static unsigned int crc16(unsigned char* puchMsg, unsigned char usDataLen)
	{
		unsigned char uchCRCHi = 0xFF; /* 高CRC字节初始化 */
		unsigned char uchCRCLo = 0xFF; /* 低CRC字节初始化 */
		unsigned int uIndex; /* CRC循环中的索引 */

		while (usDataLen--)              /* 传输消息缓冲区  */
		{
			uIndex = uchCRCLo ^ *puchMsg++; /* 计算CRC */
			uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
			uchCRCHi = auchCRCLo[uIndex];
		}
		//return (uchCRCLo << 8 | uchCRCHi); //低字节在前，高字节在后
		return (uchCRCHi << 8 | uchCRCLo); //高字节在前，低字节在后
	};
};