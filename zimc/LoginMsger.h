#ifndef __LoginMsger_H__
#define __LoginMsger_H__


#include "NetFrame.h"
#include "DuiWindowBase.h"
#include "json\json.h"


// 
// ���ģʽ��Ҫ Window ʹ�����Ժ��Լ��ͷ���������, �Ƚ� exin. 
// CMsgWindow ���Լ���������, ����û�б���ô�. 
// 
//class CMsgWindow
//	: public CDuiWindowBase
//{
//public:
//	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
//	{
//		#ifdef _DEBUG
//			if(uMsg > Msg_NetCsBase && uMsg < Msg_NetUnknown)
//			{
//				// must free net msg data. 
//				Assert(lParam == 0);
//			}
//		#endif
//
//		return __supper::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
//	}
//};


class CLoginMsger
	: public CMsgLayer
{
public:
	CLoginMsger(CDuiWindowBase * pLoginWnd)
		: m_pMsgWindow(pLoginWnd)
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
	int ParserTeams (void * jsTeams,   NetItemInfo_t ** ppNetTeams,  int * pnNetTeamsSize);
	int ParserGroups(void * jsGroups,  NetItemInfo_t ** ppNetGroups, int * pnNetGroupsSize);
	int ParserTalks (void * jsTalks,   void * jsFriends, void * jsSelf, NetItemInfo_t ** ppNetTalks,  int * pnNetTalksSize);


private:
	CDuiWindowBase * m_pMsgWindow;
};


#endif
