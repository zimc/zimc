#include "stdafx.h"
#include "MainWindow.h"
#include "SearchWindow.h"
#include "AddFriendWindow.h"
#include "ConcreteItemListUI.h"


CZiSearchWindow::CZiSearchWindow(CZiMainFrame * pMainWnd)
	: m_pMainWindow(pMainWnd)
	, m_nWindowIndex(0)
	, m_nSearchType(Type_MatchFriend)
{}

CZiSearchWindow::~CZiSearchWindow()
{}

LPCTSTR CZiSearchWindow::GetWindowClassName() const
{
	return _T("SearchWnd");
}

tstring CZiSearchWindow::GetSkinFile() 
{
	return tstring(_T("zisearch.xml"));
}

CControlUI * CZiSearchWindow::CreateControl(LPCTSTR pstrClass)
{
	if(_tcsicmp(pstrClass, _T("NewUserList")) == 0)
	{
		return new CNewUserItemUI(m_pmUi);
	}

	return 0;
}

void    CZiSearchWindow::Notify(TNotifyUI & msg)
{
	if(msg.sType == _T("click"))
	{
		DuiClickButtonMap(_T("CloseBtn"),          OnExit);
		DuiClickButtonMap(_T("ExpandBtn"),         OnDownWindow);
		DuiClickButtonMap(_T("ContractBtn"),       OnUpWindow);
		DuiClickButtonMap(_T("logo"),              OnNewFriendBtnClick);
	}
	else if(msg.sType == _T("setfocus"))
	{
		DuiGenericMessageMap(_T("AccountEdit1"),   OnSetFocusAccount);
		DuiGenericMessageMap(_T("AccountEdit2"),   OnSetFocusAccount);
	}
	else if(msg.sType == _T("killfocus"))
	{
		DuiGenericMessageMap(_T("AccountEdit1"),   OnKillFocusAccount);
		DuiGenericMessageMap(_T("AccountEdit2"),   OnKillFocusAccount);
	}
	else if(msg.sType == _T("return"))
	{
		DuiGenericMessageMap(_T("AccountEdit1"),   OnReturnAccount);
		DuiGenericMessageMap(_T("AccountEdit2"),   OnReturnAccount);
	}
	else if(msg.sType == _T("selectchanged"))
	{
		DuiGenericMessageMap(_T("ExactMatchTab1"), OnChangeMatch);
		DuiGenericMessageMap(_T("ExactMatchTab2"), OnChangeMatch);
		DuiGenericMessageMap(_T("GroupMatchTab1"), OnChangeMatch);
		DuiGenericMessageMap(_T("GroupMatchTab2"), OnChangeMatch);
	}
	else if(msg.sType == _T("itemclick"))
	{
		OnNewFriendItemClick(msg);
	}
}

void    CZiSearchWindow::OnFinalMessage(HWND hWnd)
{
	m_pMainWindow->m_pSearchWindow = 0;

	CDuiWindowBase::OnFinalMessage(hWnd);	
	delete this;
}

LRESULT CZiSearchWindow::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CZiSearchWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	CDuiWindowBase::OnCreate(uMsg, wParam, lParam, bHandled);

	// 异形窗口. 
	COLORREF colorMask = RGB(0, 0x0, 0);
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, 
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(m_hWnd, colorMask, 255, LWA_COLORKEY);

	// 固定大小
	::SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);

	// 修正 Tab. 
	DuiControl(COptionUI, _T("ExactMatchTab1"))->Selected(TRUE);
	DuiControl(COptionUI, _T("GroupMatchTab1"))->Selected(FALSE);

	// 测试: 修改 chat 的窗口. 
	// 559, 486. 
	//RECT rcClient = {0};
	//::GetWindowRect(m_hWnd, &rcClient);
	//::SetWindowPos(m_hWnd, 0, 0, 0, 559, 486, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	// test resposne list. 
	//SearchFriendListTest();
	return S_OK;
}

LRESULT CZiSearchWindow::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	CControlUI* pControl = DuiControl(CControlUI, pt);
	if(pControl && 
		_tcsicmp(pControl->GetClass(), _T("ButtonUI"))    != 0 && 
		_tcsicmp(pControl->GetClass(), _T("ScrollBarUI")) != 0 && 
		_tcsicmp(pControl->GetClass(), _T("OptionUI"))    != 0 && 
		_tcsicmp(pControl->GetClass(), _T("TextUI"))      != 0 && 
		_tcsicmp(pControl->GetClass(), _T("CheckBoxUI"))  != 0 && 
		_tcsicmp(pControl->GetClass(), _T("ListUI"))      != 0 && 
		_tcsicmp(pControl->GetClass(), _T("ComboUI"))     != 0 && 
		_tcsicmp(pControl->GetClass(), _T("LabelUI"))     != 0 && 
		_tcsicmp(pControl->GetClass(), _T("RichEditUI"))  != 0 && 
		_tcsicmp(pControl->GetClass(), _T("EditUI"))      != 0)
	{
		// 判断是否是查询结果列表框中. ***
		CTabLayoutUI * pTabs = DuiControl(CTabLayoutUI, _T("Tabs"));
		Assert(pTabs);

		const RECT & rcSize = pTabs->GetPos();
		if(pt.x > rcSize.left && pt.x < rcSize.right && 
			pt.y > rcSize.top && pt.y < rcSize.bottom)
		{
			return HTCLIENT;
		}

		//if(pControl) ::OutputDebugStringA(CT2A(pControl->GetClass()));
		return HTCAPTION;
	}

	return HTCLIENT;
}

int     CZiSearchWindow::OnExit(TNotifyUI & msg)
{
	Close();
	return 0;
}

int     CZiSearchWindow::OnDownWindow(TNotifyUI & msg)
{
	Assert(m_nWindowIndex == 0);
	m_nWindowIndex  = 1;
	return SyncWndState(true);
}

int     CZiSearchWindow::OnUpWindow(TNotifyUI & msg)
{
	Assert(m_nWindowIndex == 1);
	m_nWindowIndex  = 0;
	return SyncWndState(false);
}

int     CZiSearchWindow::OnSetFocusAccount(TNotifyUI & msg)
{
	return UpdateAccount(true);
}

int     CZiSearchWindow::OnKillFocusAccount(TNotifyUI & msg)
{
	return UpdateAccount(false);
}

int     CZiSearchWindow::OnChangeMatch(TNotifyUI & msg)
{
	LPCTSTR tszSendName = msg.pSender->GetName().GetData();

	if(::_tcscmp(tszSendName, _T("ExactMatchTab1")) == 0 || 
		::_tcscmp(tszSendName, _T("ExactMatchTab2")) == 0)
	{
		m_nSearchType = Type_MatchFriend;
	}

	if(::_tcscmp(tszSendName, _T("GroupMatchTab1")) == 0 || 
		::_tcscmp(tszSendName, _T("GroupMatchTab2")) == 0)
	{
		m_nSearchType = Type_MatchGroup;
	}

	return 0;
}

int     CZiSearchWindow::OnReturnAccount(TNotifyUI & msg)
{
	CEditUI * pAccountEdit = DuiControl(CEditUI, 
		m_nWindowIndex == 0 ? _T("AccountEdit1") : _T("AccountEdit2"));
	LPCTSTR   ptsAccount   = pAccountEdit->GetText().GetData();
	Assert(pAccountEdit);

	if(ptsAccount && ptsAccount[0] && 
		::_tcscmp(ptsAccount, _T("")) != 0 && 
		::_tcscmp(ptsAccount, _T("输入帐号")) != 0)
	{
		pAccountEdit->SetEnabled(false);
		::SendMessage(::GetFocus(), WM_KILLFOCUS, 0, 0);
		return QuerySearchFriend(ptsAccount, m_nSearchType);
	}

	return Error_SearchNoType;
}

int     CZiSearchWindow::OnNewFriendItemClick(TNotifyUI & msg)
{
	if(msg.wParam != UIEVENT_BUTTONDOWN) return 0;

	CBaseItemListUI * pItemListUi = DuiControl(CBaseItemListUI, _T("UserInfoList"));
	Assert(pItemListUi);

	if(pItemListUi->GetItemIndex(msg.pSender) != -1 && (
		::_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0))
	{
		CNodeList * pNode = (CNodeList *)msg.pSender->GetTag();

		// 判断是否点击了 New Friend Btn. 
		CButtonUI * pNewFriendBtn = static_cast<CButtonUI*>(
			m_pmUi.FindSubControlByName(pNode->GetNodeData().pListElement, _T("logo")));
		Assert(pNewFriendBtn);

		const POINT & pt    = msg.ptMouse;
		const RECT  & rcBtn = pNewFriendBtn->GetPos();
		if(pt.x > rcBtn.left && pt.x < rcBtn.right && 
			pt.y > rcBtn.top && pt.y < rcBtn.bottom)
		{
			AddNewFriend(&pNode->GetNodeData());
		}
	}
	
	return 0;
}

int     CZiSearchWindow::OnNewFriendBtnClick(TNotifyUI & msg)
{
	CBaseItemListUI * pItemListUi = DuiControl(CBaseItemListUI, _T("UserInfoList"));
	Assert(pItemListUi);

	CControlUI * pItemUi = 0;
	CControlUI * pTmpUi  = msg.pSender;

	do
	{
		pItemUi = pTmpUi->GetParent();
		if(!pItemUi || (
			::_tcsicmp(pItemUi->GetClass(), _T("ListContainerElementUI")) == 0))
		{
			break;
		}

		pTmpUi = pItemUi;
	}while(1);

	if(pItemUi && 
		pItemListUi->GetItemIndex(pItemUi) != -1)
	{
		CNodeList * pNode = (CNodeList *)pItemUi->GetTag();
		Assert(pNode);
		AddNewFriend(&(pNode->GetNodeData()));
	}

	return 0;
}

int     CZiSearchWindow::SyncWndState(bool bUpToDown)
{
	// down is 1, up is 0. 
	CContainerUI * pSrcWnd        = DuiControl(CContainerUI, _T("SearchWnd1")); 
	CContainerUI * pDstWnd        = DuiControl(CContainerUI, _T("SearchWnd2"));
	COptionUI    * pSrcExactMatch = DuiControl(COptionUI,    _T("ExactMatchTab1"));
	COptionUI    * pDstExactMatch = DuiControl(COptionUI,    _T("ExactMatchTab2"));
	COptionUI    * pSrcGroupMatch = DuiControl(COptionUI,    _T("GroupMatchTab1"));
	COptionUI    * pDstGroupMatch = DuiControl(COptionUI,    _T("GroupMatchTab2"));
	CEditUI      * pSrcAccount    = DuiControl(CEditUI,      _T("AccountEdit1"));
	CEditUI      * pDstAccount    = DuiControl(CEditUI,      _T("AccountEdit2"));

	Assert(pSrcWnd && pDstWnd);
	Assert((bUpToDown && pSrcWnd->IsVisible() && !pDstWnd->IsVisible()) || ( 
		!bUpToDown && !pSrcWnd->IsVisible() && pDstWnd->IsVisible()));
	Assert(pSrcExactMatch && pDstExactMatch);
	Assert(pSrcGroupMatch && pDstGroupMatch);
	Assert(pSrcAccount    && pDstAccount);

	if(!bUpToDown)
	{
		CContainerUI * pTmp1 = pSrcWnd;        pSrcWnd        = pDstWnd;        pDstWnd        = pTmp1;
		COptionUI    * pTmp2 = pSrcExactMatch; pSrcExactMatch = pDstExactMatch; pDstExactMatch = pTmp2;
		COptionUI    * pTmp3 = pSrcGroupMatch; pSrcGroupMatch = pDstGroupMatch; pDstGroupMatch = pTmp3;
		CEditUI      * pTmp4 = pSrcAccount;    pSrcAccount    = pDstAccount;    pDstAccount    = pSrcAccount;
	}

	// data
	pDstExactMatch->Selected(pSrcExactMatch->IsSelected());
	pDstGroupMatch->Selected(pSrcGroupMatch->IsSelected());
	pDstAccount   ->SetEnabled(pSrcAccount ->IsEnabled());
	pDstAccount   ->SetText (pSrcAccount   ->GetText());

	// 
	pSrcWnd->SetVisible(false);
	pDstWnd->SetVisible(true);

	return 0;
}

int     CZiSearchWindow::UpdateAccount(bool bSetFocus)
{
	// setfocus is 1, killfocus is 0. 
	CEditUI* pAccountEdit = DuiControl(CEditUI, 
		m_nWindowIndex == 0 ? _T("AccountEdit1") : _T("AccountEdit2"));
	LPCTSTR  ptsAccount   = pAccountEdit->GetText().GetData();

	if(bSetFocus && ptsAccount && 
		::_tcscmp(ptsAccount, _T("输入帐号")) == 0)
	{
		pAccountEdit->SetText(_T(""));
	}

	if(!bSetFocus && ptsAccount && 
		::_tcscmp(ptsAccount, _T("")) == 0)
	{
		pAccountEdit->SetText(_T("输入帐号"));
	}

	return 0;
}

int     CZiSearchWindow::SearchFriendListTest()
{
	// ?????
	CBaseItemListUI * pUserInfoListUi = DuiControl(CNewUserItemUI, _T("UserInfoList"));
	Assert(pUserInfoListUi);

	ItemNodeInfo_t item;
	item.nId              = LocalId_t(Type_ImcFriend, 20001);
	item.chType           = Type_ImcFriend;
	item.bIsFolder        = 0;
	item.bIsHasChild      = 0;
	item.tstrLogo         = _T("search_btn_add_friend_1.png");
	item.tstrNickName     = _T("111");
	item.tstrDescription  = _T("---111---");

	pUserInfoListUi->AddNode(item, 0);

	//item.nId              = 20002;
	//item.chType           = Type_ImcFriend;
	//item.bIsFolder        = 0;
	//item.bIsHasChild      = 0;
	//item.tstrLogo         = _T("search_btn_add_friend_1.png");
	//item.tstrNickName     = _T("112");
	//item.tstrDescription  = _T("---112---");

	//pUserInfoListUi->AddNode(item, 0);

	return 0;
}

// --------------------------------------
int     CZiSearchWindow::AddNewFriend(NodeData_t * pNode)
{
	// 点击增加好友. 
	Assert(pNode);
	Assert(m_pMainWindow);

	CNodeList        * pTabNode      = m_pMainWindow->GetFriendTeamNode();
	CAddFriendWindow * pAddFriendWnd = new CAddFriendWindow(
		m_pMainWindow, pNode, pTabNode);
	if(!pAddFriendWnd) return -1;

	pAddFriendWnd->Create(NULL, _T("AddFriendWndX"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAddFriendWnd->CenterWindow();
	pAddFriendWnd->ShowWindow(true);

	return 0;
}

int     CZiSearchWindow::QuerySearchFriend(LPCTSTR tszAccount, int nMatchType)
{
	// net send. 
	CT2A pAData(tszAccount);

	SearchCsQuery_t searchQuery;
	searchQuery.szNickName = pAData;
	searchQuery.nMatchType = nMatchType;

	CT2A pstrSelfName(m_pMainWindow->GetSelfInfo()->tstrNickName.c_str());
	searchQuery.szSelfName = pstrSelfName;

	Assert(m_pMainWindow);
	return m_pMainWindow->SendImMessageX(Msg_CsQueryUsers, (LPARAM)&searchQuery, sizeof(SearchCsQuery_t));
}

int     CZiSearchWindow::HandleResponseResult(SearchScResponse_t * pSearchResult)
{
	Assert(pSearchResult);

	CBaseItemListUI * pUserInfoListUi = DuiControl(CNewUserItemUI, _T("UserInfoList"));
	Assert(pUserInfoListUi);

	// ... ???
	if(pUserInfoListUi->GetCount() > 0)
	{
		ZiLogger(RSZLOG_TEST, "SearchWindow Remove All, Begin.");
		pUserInfoListUi->RemoveAll();
		ZiLogger(RSZLOG_TEST, "SearchWindow Remove All, End.  ");
	}

	if(pSearchResult->nItemSize == 0)
	{
		MessageBox(m_hWnd, _T("未查询到任何相关信息"), _T("提示"), 0);
	}

	// 空间不够, 难道仅仅允许显示两个好友. ???
	for(int i = 0; i < pSearchResult->nItemSize; i++)
	{
		NetItemInfo_t itemInfo = pSearchResult->pItemInfo[i];
		Assert(itemInfo.nItemType == Type_ImcFriend);

		// 由于使用了 button, 不知道该如何实现背景变换. ???
		// 改为使用 CBaseItemListUI. 
		ItemNodeInfo_t item;
		ItemDataNetToLocal(itemInfo, item);
		item.tstrLogo = _T("search_btn_add_friend_1.png");

		pUserInfoListUi->AddNode(item, 0);
	}

	TNotifyUI nf;
	OnDownWindow(nf);

	CEditUI * pAccountEdit = DuiControl(CEditUI, 
		m_nWindowIndex == 0 ? _T("AccountEdit1") : _T("AccountEdit2"));
	pAccountEdit->SetEnabled(true);
	return 0;
}


// ---------------------------------------------------
// 1 暂时对于搜索, 仅支持精确搜索. 搜索类型如下: 
//   -- 精确搜索好友, 和精确搜索群. 
//      相对应的, 其数据结构也是简单的. 返回的好友信息最多一个. 
// 2 其它搜索类型暂不考虑. 以后扩展. 
// 
