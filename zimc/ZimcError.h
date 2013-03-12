#ifndef __ZimcError_H__
#define __ZimcError_H__


#include "UiData.h"
#include <map>


enum enumZiError
{
	Error_No                          = 0x0, 

	Error_Base                        = 0x1000, 
	Error_Fail, 
	Error_Unknown, 

	Error_NoImplement                 = Error_Base + 0x6, 
	Error_OutOfMemory, 
	Error_InvalidUser, 
	Error_InvalidNetData, 
	Error_NoPermission, 
	Error_NetworkFail, 

	// 登陆错误消息: 
	// 1 网络连接失败.   (包括服务器连接失败)
	// 2 帐号或密码错误. (服务器返回失败)
	// 3 未知错误. 
	Error_LoginNoAccount              = Error_Base + 0x10, 
	Error_LoginNoPassword, 
	Error_LoginPasswordError, 
	Error_LoginAccountOrPasswordError, 
	Error_LoginNetworkFailed, 
	Error_LoginConnectServerFailed, 
	Error_LoginDuplication, 

	// 查找好友信息
	// 
	Error_SearchNoType                = Error_Base + (0x10 << 2), 
	Error_SearchNoValidUser, 
};


static const 
struct ZiErrorCodeText_t
{
	int    nCode;
	char * szText;
}
g_errorTextTable[] = 
{
	// ok
	// 0x0, 0x0F
	{ 0x0,                               "成功. " }, 

	// msg. 
	// 0x10, 0x1000
	{ Msg_ScLogin,                       "登陆请求. " }, 
	{ Msg_CsQueryUsers,                  "搜索用户. " }, 

	// error
	// 0x1000, 0x1xxx
	{ Error_NoImplement,                 "未实现" }, 
	{ Error_OutOfMemory,                 "内存不足" }, 
	{ Error_InvalidUser,                 "无效用户" }, 
	{ Error_InvalidNetData,              "无效网络包" }, 
	{ Error_NoPermission,                "没有权限" }, 
	{ Error_NetworkFail,                 "网络异常" },  

	{ Error_LoginNoAccount,              "帐号为空" }, 
	{ Error_LoginNoPassword,             "密码为空" }, 
	{ Error_LoginPasswordError,          "帐号或密码错误" }, 
	{ Error_LoginAccountOrPasswordError, "帐号或密码错误" }, 
	{ Error_LoginNetworkFailed,          "网络连接失败" }, 
	{ Error_LoginConnectServerFailed,    "网络连接失败" }, 
	{ Error_LoginDuplication,            "此帐号已经被登陆" }, 

	{ Error_SearchNoType,                "查找条件为空" }, 
	{ Error_SearchNoValidUser,           "无有效用户" }, 

	{ Error_Unknown,                     "未知错误" }, 
};


class CZiError
{
public:
	const char * GetErrorMsgText(int nMsg, int nErrorCode)
	{
		// msg ???
		return GetErrorText(nErrorCode);
	}


private:
	const char * GetErrorText(int nErrorCode)
	{
		typedef std::map<int, char *> ErrorInfoTable_t;
		static ErrorInfoTable_t g_mapErrorInfoTable;
		if(g_mapErrorInfoTable.empty())
		{
			int sz = sizeof(g_errorTextTable)/sizeof(g_errorTextTable[0]);
			for(int i = 0; i < sz; i++)
			{
				g_mapErrorInfoTable.insert(
					std::make_pair(g_errorTextTable[i].nCode, g_errorTextTable[i].szText));
			}
		}

		ErrorInfoTable_t::iterator it = g_mapErrorInfoTable.find(nErrorCode);
		if(it != g_mapErrorInfoTable.end())
		{
			return it->second;
		}

		_ASSERT(0);
		return "未知错误";
	}
};


#endif
