#ifndef __ZiCoder_H__
#define __ZiCoder_H__


#include <string>


class CZiDataCoder
{
public:
	static void Encrypt(const char* src, std::string& dst);
	static void Decrypt(const char* src, std::string & dst);
};


#endif
