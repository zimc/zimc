#include "stdafx.h"
#include "ZiCoder.h"


int  RsUpgradeEncrypt(const char * szDst, int nSize, const char * szSrc)
{
	return 0;
}

int  RsUpgradeDecrypt(const char * szDst, int nSize, const char * szSrc)
{
	return 0;
}


void CZiDataCoder::Encrypt(const char* src, std::string & dst)
{
	dst = src;

	//�˺�����ʵ����������Ť��������Ҫ��Buffer����ʱ����Ҫ�ȴ����ǿ�Buffer
	char lpDumpBuf[4] = {0};
	int iLen = RsUpgradeEncrypt(lpDumpBuf, 0, src);
	if (0 == iLen)
	{
		return;
	}

	Assert(iLen < 0);
	iLen = -iLen;
	dst.resize(iLen);
	RsUpgradeEncrypt(dst.c_str(), iLen, src);
}

void CZiDataCoder::Decrypt(const char* src, std::string & dst)
{
	dst = src;

	//�˺�����ʵ����������Ť��������Ҫ��Buffer����ʱ����Ҫ�ȴ����ǿ�Buffer
	char lpDumpBuf[4];
	INT iLen = RsUpgradeDecrypt(lpDumpBuf, 0, src);
	if (0 == iLen)
	{
		return;
	}

	Assert(iLen < 0);
	iLen = -iLen;
	dst.resize(iLen);
	RsUpgradeDecrypt(dst.c_str(), iLen, src);
}
