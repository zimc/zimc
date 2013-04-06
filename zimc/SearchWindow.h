#ifndef __SearchWindow_H__
#define __SearchWindow_H__


#include "DuiWindowBase.h"


class CZiMainFrame;

class CZiSearchWindow
	: public CDuiWindowBase
{
public:
	CZiSearchWindow(CZiMainFrame * pMainWnd);
	~CZiSearchWindow();

	int     HandleResponseResult(SearchScResponse_t * pSearchResult);
	int     HandleResponseResultForGroup(SearchGroup_t *pSearchGroup);


private:
	// virtual function. 
	LPCTSTR GetWindowClassName() const;
	tstring GetSkinFile();

	// INotifyUI
	void    Notify(TNotifyUI & msg);

	// new control. 
	CControlUI * CreateControl(LPCTSTR pstrClass);

	// message. 
	void    OnFinalMessage(HWND hWnd);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);


	// extend. 
	int     OnExit(TNotifyUI & msg);
	int     OnDownWindow(TNotifyUI & msg);
	int     OnUpWindow(TNotifyUI & msg);
	int     OnSetFocusAccount(TNotifyUI & msg);
	int     OnKillFocusAccount(TNotifyUI & msg);
	int     OnChangeMatch(TNotifyUI & msg);
	int     OnReturnAccount(TNotifyUI & msg);
	int     OnNewFriendItemClick(TNotifyUI & msg);
	int     OnNewFriendBtnClick(TNotifyUI & msg);


private:
	int     SyncWndState(bool bUpToDown);
	int     UpdateAccount(bool bFocus);
	int     QuerySearchFriend(LPCTSTR tszAccount, int nMatchType);
	int     AddNewFriend(NodeData_t * pNode);

	int     SearchFriendListTest();


private:
	int                m_nSearchType;
	int                m_nWindowIndex;

	CZiMainFrame     * m_pMainWindow;
};


#endif