#ifndef SRC_UTILS_AES_ENCRYPTOR_H
#define SRC_UTILS_AES_ENCRYPTOR_H

#include <string>
#include <fstream>
#include "aes.h"
using namespace std;
class AES;

class AesEncryptor
{
public:
	AesEncryptor(unsigned char* key);
	~AesEncryptor(void);

	std::string EncryptString(std::string strInfor);
	std::string DecryptString(std::string strMessage);

	void EncryptTxtFile(const char* inputFileName);
	void EncryptTxtFile(const char* inputFileName, const char* outputFileName);
	void DecryptTxtFile(const char* inputFileName, const char* outputFileName);
	void DecrypToEncryp(const char* inputFileName, const char* outputFileName, unsigned char* key);

private:
	void Byte2Hex(const unsigned char* src, int len, char* dest);
	void Hex2Byte(const char* src, int len, unsigned char* dest);
	int  Char2Int(char c);

private:
	AES* m_pEncryptor;
};

#endif        // SRC_UTILS_AES_ENCRYPTOR_H