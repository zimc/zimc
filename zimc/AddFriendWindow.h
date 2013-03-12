#ifndef __AddFriendWindow_H__
#define __AddFriendWindow_H__


#include "DuiWindowBase.h"


struct NetItemInfo_t;
class  CNodeList;
class  CZiMainFrame;

class  CAddFriendWindow
	: public CDuiWindowBase
{
public:
	CAddFriendWindow(CZiMainFrame * pMainWnd, ItemNodeInfo_t * pUserInfo, CNodeList * pParentNode);
	~CAddFriendWindow();


private:
	// virtual function. 
	LPCTSTR GetWindowClassName() const;
	tstring GetSkinFile();

	// INotifyUI
	void    Notify(TNotifyUI & msg);

	// message. 
	void    OnFinalMessage(HWND hWnd);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);

	// extend. 
	int     OnExit(TNotifyUI & msg);
	int     OnAddFriend(TNotifyUI & msg);


private:
	CZiMainFrame   * m_pMainWindow;
	CNodeList      * m_pParentNode;
	ItemNodeInfo_t * m_pUserInfo;
};


#endif
