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

	// ��½������Ϣ: 
	// 1 ��������ʧ��.   (��������������ʧ��)
	// 2 �ʺŻ��������. (����������ʧ��)
	// 3 δ֪����. 
	Error_LoginNoAccount              = Error_Base + 0x10, 
	Error_LoginNoPassword, 
	Error_LoginPasswordError, 
	Error_LoginAccountOrPasswordError, 
	Error_LoginNetworkFailed, 
	Error_LoginConnectServerFailed, 
	Error_LoginDuplication, 

	// ���Һ�����Ϣ
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
	{ 0x0,                               "�ɹ�. " }, 

	// msg. 
	// 0x10, 0x1000
	{ Msg_ScLogin,                       "��½����. " }, 
	{ Msg_CsQueryUsers,                  "�����û�. " }, 

	// error
	// 0x1000, 0x1xxx
	{ Error_NoImplement,                 "δʵ��" }, 
	{ Error_OutOfMemory,                 "�ڴ治��" }, 
	{ Error_InvalidUser,                 "��Ч�û�" }, 
	{ Error_InvalidNetData,              "��Ч�����" }, 
	{ Error_NoPermission,                "û��Ȩ��" }, 
	{ Error_NetworkFail,                 "�����쳣" },  

	{ Error_LoginNoAccount,              "�ʺ�Ϊ��" }, 
	{ Error_LoginNoPassword,             "����Ϊ��" }, 
	{ Error_LoginPasswordError,          "�ʺŻ��������" }, 
	{ Error_LoginAccountOrPasswordError, "�ʺŻ��������" }, 
	{ Error_LoginNetworkFailed,          "��������ʧ��" }, 
	{ Error_LoginConnectServerFailed,    "��������ʧ��" }, 
	{ Error_LoginDuplication,            "���ʺ��Ѿ�����½" }, 

	{ Error_SearchNoType,                "��������Ϊ��" }, 
	{ Error_SearchNoValidUser,           "����Ч�û�" }, 

	{ Error_Unknown,                     "δ֪����" }, 
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
		return "δ֪����";
	}
};


#endif
