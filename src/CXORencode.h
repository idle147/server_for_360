#pragma once
class CXORencode
{
	/*异或加密解密算法
	 *i_block:输入数据
	 *o_block:输出数据
	 *key:密钥
	 *len:输入数据长度
	 *如果输入是明文，则输出是密文
	 *如果输入是密文，则输出是明文
	 */
	int XORencode(void* i_block, void* o_block, int key, int len);
};
