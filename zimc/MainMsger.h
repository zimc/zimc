#ifndef __MainMsger_H__
#define __MainMsger_H__


#include "NetFrame.h"
#include "DuiWindowBase.h"
#include "json\json.h"


class CDuiWindowBase;
class CMainMsger
	: public CMsgLayer
{
public:
	CMainMsger(CDuiWindowBase * pMainWnd)
		: m_pMsgWindow(pMainWnd)
	{}

	virtual int OnDispatch(int nMsg, LPARAM lp, WPARAM wp)
	{
		Assert(m_pMsgWindow && m_pMsgWindow->GetHWND());
		::PostMessage(m_pMsgWindow->GetHWND(), nMsg, wp, lp);
		return 0;
	}

	int LocalToNet(int nMsg, void   * pLocalData, int nLocalDataLen, Byte_t ** ppbNetData,   int * pnNetDataLen);
	int NetToLocal(int nMsg, Byte_t * pbNetData,  int nNetDataLen,   void   ** ppbLocalData, int * pnLocalDataLen);
	int FreeData  (int nMsg, void   * pLocalData);
	int FreeDataEx(int nMsg, void   * pNetData);


private:
	int ParserSearchUser    (NetMsg_t * pNetMsg, Json::Value & jsRoot, void ** ppbLocalData, void * pUserData);
	int ParserQueryVerify   (NetMsg_t * pNetMsg, Json::Value & jsRoot, void ** ppbLocalData, void * pUserData);
	int ParserResponseVerify(NetMsg_t * pNetMsg, Json::Value & jsRoot, void ** ppbLocalData, void * pUserData);
	int ParserTextChat      (NetMsg_t * pNetMsg, Json::Value & jsRoot, void ** ppbLocalData, void * pUserData);


private:
	CDuiWindowBase * m_pMsgWindow;
};


#endif
