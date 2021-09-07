#include "CCipher.h"

AesEncryptor::AesEncryptor(unsigned char* key)
{
	m_pEncryptor = new AES(key);
}

AesEncryptor::~AesEncryptor(void)
{
	delete m_pEncryptor;
}

void AesEncryptor::Byte2Hex(const unsigned char* src, int len, char* dest) {
	for (int i = 0; i < len; ++i) {
		sprintf(dest + i * 2, "%02X", src[i]);
	}
}

void AesEncryptor::Hex2Byte(const char* src, int len, unsigned char* dest) {
	int length = len / 2;
	for (int i = 0; i < length; ++i) {
		dest[i] = Char2Int(src[i * 2]) * 16 + Char2Int(src[i * 2 + 1]);
	}
}

int AesEncryptor::Char2Int(char c) {
	if ('0' <= c && c <= '9') {
		return (c - '0');
	}
	else if ('a' <= c && c <= 'f') {
		return (c - 'a' + 10);
	}
	else if ('A' <= c && c <= 'F') {
		return (c - 'A' + 10);
	}
	return -1;
}

string AesEncryptor::EncryptString(string strInfor) {
	int nLength = strInfor.length();
	int spaceLength = 16 - (nLength % 16);
	unsigned char* pBuffer = new unsigned char[nLength + spaceLength];
	memset(pBuffer, '\0', nLength + spaceLength);
	memcpy(pBuffer, strInfor.c_str(), nLength);
	m_pEncryptor->Cipher(pBuffer, nLength + spaceLength);

	// 这里需要把得到的字符数组转换成十六进制字符串
	char* pOut = new char[2 * (nLength + spaceLength)];
	memset(pOut, '\0', 2 * (nLength + spaceLength));
	Byte2Hex(pBuffer, nLength + spaceLength, pOut);

	string retValue(pOut);
	delete[] pBuffer;
	delete[] pOut;
	return retValue;
}

string AesEncryptor::DecryptString(string strMessage) {
	int nLength = strMessage.length() / 2;
	unsigned char* pBuffer = new unsigned char[nLength * 2];
	memset(pBuffer, '\0', nLength);
	Hex2Byte(strMessage.c_str(), strMessage.length(), pBuffer);

	m_pEncryptor->InvCipher(pBuffer, nLength);
	string retValue((char*)pBuffer);
	delete[] pBuffer;
	return retValue;
}

void AesEncryptor::EncryptTxtFile(const char* inputFileName)
{
	ifstream ifs;

	// Open file:
	ifs.open(inputFileName);
	if (!ifs)
	{
		perror("AesEncryptor::EncryptTxtFile() - Open input file failed!");
		return;
	}

	// Read config data:
	string strInfor;
	string strLine;
	while (!ifs.eof()) {
		char temp[1024];
		memset(temp, '\0', 1024);
		ifs.read(temp, 1000);
		strInfor += temp;
	}
	ifs.close();

	// Encrypt
	strLine = EncryptString(strInfor);

	// Writefile
	ofstream ofs;
	ofs.open(inputFileName, ios::in | ios::out | ios::trunc);
	if (!ofs)
	{
		perror("AesEncryptor::EncryptTxtFile() - Open output file failed!");
		return;
	}
	ofs << strLine;
	ofs.close();
}

void AesEncryptor::EncryptTxtFile(const char* inputFileName, const char* outputFileName)
{
	ifstream ifs;

	// Open file:
	ifs.open(inputFileName);
	if (!ifs)
	{
		perror("AesEncryptor::EncryptTxtFile() - Open input file failed!");
		return;
	}

	// Read config data:
	string strInfor;
	string strLine;
	while (!ifs.eof()) {
		char temp[1024];
		memset(temp, '\0', 1024);
		ifs.read(temp, 1000);
		strInfor += temp;
	}
	ifs.close();

	// Encrypt
	strLine = EncryptString(strInfor);

	// Writefile
	ofstream ofs;
	ofs.open(outputFileName, ios::in | ios::out | ios::trunc);
	if (!ofs)
	{
		perror("AesEncryptor::EncryptTxtFile() - Open output file failed!");
		return;
	}
	ofs << strLine;
	ofs.close();
}

void AesEncryptor::DecryptTxtFile(const char* inputFile, const char* outputFile)
{
	ifstream ifs;

	// Open file:
	ifs.open(inputFile);
	if (!ifs)
	{
		perror("AesEncryptor::DecryptTxtFile() - Open input file failed!");
		return;
	}

	// Read config data:
	string strInfor;
	string strLine;
	while (!ifs.eof()) {
		char temp[1024];
		memset(temp, '\0', 1024);
		ifs.read(temp, 1000);
		strInfor += temp;
	}
	ifs.close();

	// Encrypt
	strLine = DecryptString(strInfor);

	// Writefile
	ofstream ofs;
	ofs.open(outputFile, ios::in | ios::out | ios::trunc);
	if (!ofs)
	{
		perror("AesEncryptor::DecryptTxtFile() - Open output file failed!");
		return;
	}
	ofs << strLine;
	ofs.close();
}

void AesEncryptor::DecrypToEncryp(const char* inputFileName, const char* outputFileName, unsigned char* key)
{
	ifstream ifs;

	// Open file:
	ifs.open(inputFileName);
	if (!ifs)
	{
		perror("AesEncryptor::DecryptTxtFile() - Open input file failed!");
		return;
	}

	// Read config data:
	string strInfor;
	string strLine;
	while (!ifs.eof()) {
		char temp[1024];
		memset(temp, '\0', 1024);
		ifs.read(temp, 1000);
		strInfor += temp;
	}
	ifs.close();

	//先解密
	strLine = DecryptString(strInfor);
	//后加密
	delete this->m_pEncryptor;
	this->m_pEncryptor = nullptr;
	this->m_pEncryptor = new AES(key);
	string enLine = EncryptString(strLine);
	strLine.clear();
	strLine = EncryptString(enLine);

	//输出文件
	ofstream ofs;
	ofs.open(outputFileName, ios::in | ios::out | ios::trunc);
	if (!ofs)
	{
		perror("AesEncryptor::DecryptTxtFile() - Open output file failed!");
		return;
	}
	ofs << strLine;
	ofs.close();
}