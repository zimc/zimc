#include "stdafx.h"
#include "MainWindow.h"
#include "ConcreteItemListUI.h"
#include "ChatWindow.h"
#include "MenuWindow.h"
#include "SearchWindow.h"
#include "ZimcHelper.h"
#include "TrayWindow.h"
#include "NotifyWindow.h"
#include "MainMsger.h"
#include "ReportWindow.h"
#include "CreateGroupWindow.h"
#include "Setting.h"
#include "common/HTTPRequest.h"

#include <time.h>


CZiMainFrame::CZiMainFrame()
	: m_chatFont()
	, m_pSearchWindow(0)
	, m_pTrayWindow(0)
	, m_bFlash(FALSE)
	, m_nTimer(0)
	, m_nMaxTeamId(0)
	, m_pMainMsger(0)
    , m_nlastKeepAlive_time_(0)
	, m_pReportWindow(0)
	, m_pCreateGroupWindow(0)
	, m_pModifyGroupWindow(0)
    , m_pSettingWindow(0)
{}

CZiMainFrame::~CZiMainFrame()
{}

int     CZiMainFrame::Init()
{
	if(!m_pMainMsger)
	{
		m_pMainMsger = new CMainMsger(this);
		if(!m_pMainMsger) return Error_OutOfMemory;
		m_pMainMsger->Init();
	}

	return 0;
}

int     CZiMainFrame::Uninit()
{
	if(m_pMainMsger)
	{
		m_pMainMsger->Uninit();
		delete m_pMainMsger;
		m_pMainMsger = 0;
	}
    if (m_nTimer) {
        ::KillTimer(m_hWnd, m_nTimer);
    }
	if (m_pTrayWindow) {
		m_pTrayWindow->DeleteTray();
		delete m_pTrayWindow;
		m_pTrayWindow = 0;
	}

	return 0;
}

LPCTSTR CZiMainFrame::GetWindowClassName() const
{
	return _T("UserMainUI");
}

tstring CZiMainFrame::GetSkinFile()
{
	return tstring(_T("ziusermain.xml"));
}

CControlUI * CZiMainFrame::CreateControl(LPCTSTR pstrClass)
{
	if(_tcsicmp(pstrClass, _T("FriendList")) == 0)
	{
		return new CFirendItemUI(m_pmUi);
	}
	else if(_tcsicmp(pstrClass, _T("GroupList")) == 0)
	{
		return new CGroupItemUI(m_pmUi);
	}

	return NULL;
}

LRESULT CZiMainFrame::OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	// 利用 WS_EX_TOOLWINDOW 不能够成功的隐藏任务栏图标, 
	// 有可能会重新出现在任务栏中. 不知道为什么. ???

	CDuiWindowBase::OnCreate(nMsg, wParam, lParam, bHandled);
	int nType    = ::GetWindowLong(m_hWnd, GWL_STYLE);
	int nExtType = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	nType    &= ~WS_MAXIMIZEBOX;
	nExtType |= WS_EX_TOOLWINDOW;
	nExtType &= ~WS_EX_APPWINDOW;

	::SetWindowLong(m_hWnd, GWL_STYLE,   nType);
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, nExtType);

	// tray
	if(!m_pTrayWindow)
	{
		m_pTrayWindow = new CTrayWindow(0);
		if(!m_pTrayWindow) return Error_OutOfMemory;
	}

	m_pTrayWindow->SetHwnd(m_hWnd);
	m_pTrayWindow->AddTray();

	SetIcon(128);

    if(m_nTimer == 0) {
        m_nTimer = ::SetTimer(m_hWnd, 1, 600, 0);
    }

	return S_OK;
}

LRESULT CZiMainFrame::OnNcHitTest(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	CControlUI* pControl = DuiControl(CControlUI, pt);
	if(pControl && 
		_tcsicmp(pControl->GetClass(), _T("ButtonUI"))   != 0 && 
		_tcsicmp(pControl->GetClass(), _T("OptionUI"))   != 0 &&
		_tcsicmp(pControl->GetClass(), _T("TextUI"))     != 0 && 
		_tcsicmp(pControl->GetClass(), _T("CheckBoxUI")) != 0 && 
		_tcsicmp(pControl->GetClass(), _T("ListUI"))     != 0 && 
		_tcsicmp(pControl->GetClass(), _T("ComboUI"))    != 0 && 
		_tcsicmp(pControl->GetClass(), _T("LabelUI"))    != 0 && 
		_tcsicmp(pControl->GetClass(), _T("EditUI"))     != 0)
	{
		// 判断是否是好友列表框中. 
		CTabLayoutUI * pTabs = DuiControl(CTabLayoutUI, _T("Tabs"));
		Assert(pTabs);

		const RECT & rcSize = pTabs->GetPos();
		if(pt.x > rcSize.left && pt.x < rcSize.right && 
			pt.y > rcSize.top && pt.y < rcSize.bottom)
		{
			return HTCLIENT;
		}

		//if(pControl) ::OutputDebugStringW(pControl->GetClass());
		return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CZiMainFrame::OnSysCommand(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if(wParam == SC_CLOSE)
	{
		Close();
		::PostQuitMessage(0);
		return 0;
	}

	return CDuiWindowBase::OnSysCommand(nMsg, wParam, lParam, bHandled);
}

void    CZiMainFrame::Notify(TNotifyUI & msg)
{
	if(msg.sType == _T("windowinit"))
	{
		OnPrepare(msg);
	}
	else if(msg.sType == _T("killfocus"))
	{
		OnKillFocus(msg);
	}
	else if (msg.sType == _T("setfocus")) {
		/*
		int a= 1;
		if (msg.pSender->GetName() == _T("SearchCombo")) {
			CComboUI * pSearchCombo = DuiControl(CComboUI, _T("SearchCombo"));
			pSearchCombo->Activate();
		}
		*/
	}
	else if(msg.sType == _T("return"))
	{
		DuiClickButtonMap(_T("SearchEdit"),    OnReturnSearch);
	}
	else if(msg.sType == _T("click"))
	{
		DuiClickButtonMap(_T("MinBtn"),        OnClickMin);
		DuiClickButtonMap(_T("CloseBtn"),      OnClickClose);
		DuiClickButtonMap(_T("SearchTip"),     OnClickMatchButton);
		DuiClickButtonMap(_T("SettingBtn"),    OnClickSettingButton);
		DuiClickButtonMap(_T("SearchBtn"),     OnClickSearchButton);
	}
	else if(msg.sType == _T("selectchanged"))
	{
		// tab 切换. ( friends 和 group )
		OnSelectTab(msg);
	}
	else if(msg.sType == _T("itemselect"))
	{
		// search item click. 
		OnSelectSearch(msg);
	}
	else if(msg.sType == _T("itemclick"))
	{
		// List . 
		OnSelectItemList(msg);
	}
	else if(msg.sType == _T("itemactivate"))
	{
		// 增加 select 响应双击操作. 
		OnItemChat(msg);
		OnSelectItemList(msg);
	}
	else if(msg.sType == _T("menu"))
	{
		// 右键支持
		//char szLog[256] = {0};
		//sprintf(szLog, "Class: %ws, Name = %ws\n", 
		//	msg.pSender->GetClass(), msg.pSender->GetName());
		//::OutputDebugStringA(szLog);
		DuiClickButtonMap(_T("FriendsList"),    OnClickRightButton);
		DuiClickButtonMap(_T("GroupsList"),     OnClickRightButton);
	}
	else if(msg.sType == _T("menuclick"))
	{
		// 右键点击
		//char szLog[256] = {0};
		//sprintf(szLog, "Class: %ws, Name = %ws\n", 
		//	msg.pSender->GetClass(), msg.pSender->GetName()
		//	);
		//::OutputDebugStringA(szLog);

		DuiClickMenuMap(Event_OpenItem,         OnClickRightMenu);
		DuiClickMenuMap(Event_ModifyItem,       OnClickRightMenu);
		DuiClickMenuMap(Event_DeleteItem,       OnClickRightMenu);
		DuiClickMenuMap(Event_CreateGroup,      OnCreateGroup);
	}
}


// ------------------------------------------------------------------------
// 
int     CZiMainFrame::SetWindowPosition()
{
	// 初始位于屏幕的 (3/4, 1/8) 处. 
	ASSERT(::IsWindow(m_hWnd));
	ASSERT((GetWindowStyle(m_hWnd)&WS_CHILD)==0);

	RECT rcArea     = { 0 };
	RECT rcCenter   = { 0 };
	HWND hWndCenter = 0;

	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
	hWndCenter = ::GetWindowOwner(m_hWnd);
	if(hWndCenter == NULL) rcCenter = rcArea; 
	else                   ::GetWindowRect(hWndCenter, &rcCenter);

	int nLeftX = (rcCenter.right  - rcCenter.left) * 3 / 4;
	int nTopY  = (rcCenter.bottom - rcCenter.top ) / 8;
	::SetWindowPos(m_hWnd, NULL, nLeftX, nTopY, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	return 0;
}

ItemNodeInfo_t * CZiMainFrame::GetSelfInfo()
{
	return &m_itemSelfInfo;
}

int     CZiMainFrame::SetMainInfo(LoginScData_t * pLoginData)
{
	Assert(pLoginData);

	// 解析 login 返回的信息: 自身信息和好友信息. 
	// 1 *** 总是使用服务器返回信息. 
	// 2 *** 数据格式. 
	//   参见: LoginScData_t
	// ...
	int nRet = 0;

	CFirendItemUI * pFriendsList = DuiControl(CFirendItemUI, _T("FriendsList"));
	CGroupItemUI  * pGroupsList  = DuiControl(CGroupItemUI,  _T("GroupsList"));
	Assert(pFriendsList);
	Assert(pGroupsList);

	if(pFriendsList->GetCount() > 0) pFriendsList->RemoveAll();
	if(pGroupsList->GetCount()  > 0) pGroupsList ->RemoveAll();

	if(pLoginData)
	{
		UpdateSelfInfo(pLoginData);
		UpdateFriendsList(pLoginData);
		UpdateGroupsList(pLoginData);
		UpdateTalksList(pLoginData);
	}

    //获取留言信息
    LoginCsData_t loginQuery;
    CT2A pAData1(m_itemSelfInfo.tstrNickName.c_str());
    loginQuery.szAccount = pAData1;
    SendImMessageX(Msg_LoadMessage, (LPARAM)&loginQuery, sizeof(loginQuery));


	//
	m_pTrayWindow->DoTray(NIM_MODIFY, 0, CT2A(m_itemSelfInfo.tstrNickName.c_str()));

	return nRet;
}

int     CZiMainFrame::ReadSelfInfo()
{
	// 读本地数据库的自身信息, 不存在时使用 login 的数据. 
	// ***** 暂时 **** 测试时使用. 
	// 实际使用中由网络服务器获取. 

	m_itemSelfInfo.chType       = Type_ImcFriend;
	m_itemSelfInfo.nId          = LocalId_F(101);
	m_itemSelfInfo.tstrLogo     = _T("man.png");
	m_itemSelfInfo.tstrNickName = _T("zhaocg");
	m_itemSelfInfo.tstrDescription = _T("tianyamingyuedao");

	return 0;
}

int     CZiMainFrame::ParserSelfInfo(tstring & tstrSelfInfo)
{
	// 解析后保存本地数据库. 
	// ...???

	return 0;
}

int     CZiMainFrame::UpdateState()
{
	// ... 
	// 初始状态时未加载好友列表. 
	// 运行状态时加载好友列表. 

	return 0;
}

void    CZiMainFrame::OnPrepare(TNotifyUI & msg)
{
	// 获取好友信息 ...
	// 测试 **************
	CFirendItemUI * pFriendsList = DuiControl(CFirendItemUI, _T("FriendsList"));
	CGroupItemUI  * pGroupsList  = DuiControl(CGroupItemUI,  _T("GroupsList"));
	Assert(pFriendsList);
	Assert(pGroupsList);
}

void    CZiMainFrame::OnKillFocus(TNotifyUI & msg)
{
	// 将 search 的 edit 和 tip 进行转换. 
	//if(_tcsicmp(msg.pSender->GetName(), _T("SearchEdit")) == 0)
	//{
	//	CRichEditUI * pSearchEdit = static_cast<CRichEditUI*>(msg.pSender);
	//	CRichEditUI * pSearchTip  = DuiControl(CRichEditUI, _T("SearchTip"));
	//	ExchangeEditFocus(pSearchEdit, pSearchTip);
	//}
}

void    CZiMainFrame::OnSelectTab(TNotifyUI & msg)
{
	CTabLayoutUI * pTabs = DuiControl(CTabLayoutUI, _T("Tabs"));
	if(!pTabs) return ;

	if(msg.pSender->GetName() == _T("FriendBtn"))
	{
		if(pTabs->GetCurSel() != 0)
		{
			pTabs->SelectItem(0);
			//UpdateFriendsList();
		}

		return;
	}

	if(msg.pSender->GetName() == _T("GroupBtn"))
	{
		if(pTabs->GetCurSel() != 1)
		{
			pTabs->SelectItem(1);
			//UpdateGroupsList();
		}

		return;
	}
}

void    CZiMainFrame::OnSelectItemList(TNotifyUI & msg)
{
	// 仅仅支持左键点击. 
	if(msg.wParam != UIEVENT_BUTTONDOWN) return;

	CTabLayoutUI * pTabs = DuiControl(CTabLayoutUI, _T("Tabs"));
	if(!pTabs) return ;

	CBaseItemListUI * pItemListUi = DuiControl(CBaseItemListUI, GetCurListName(pTabs->GetCurSel()));
	if(pItemListUi && (
		pItemListUi->GetItemIndex(msg.pSender) != -1) && (
		::_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0))
	{
		CNodeList * pNode = (CNodeList *)msg.pSender->GetTag();
		if(pItemListUi->CanExpand(pNode))  // 能否展开. 
		{
			pItemListUi->SetChildVisible(pNode, !pNode->GetNodeData().bIsChildVisible);
		}
	}
}

void    CZiMainFrame::OnItemChat(TNotifyUI & msg)
{
	// 
	CTabLayoutUI * pTabs = DuiControl(CTabLayoutUI, _T("Tabs"));
	if(!pTabs) return ;

	CBaseItemListUI * pItemListUi = DuiControl(CBaseItemListUI, GetCurListName(pTabs->GetCurSel()));
	if(pItemListUi && (
		pItemListUi->GetItemIndex(msg.pSender) != -1) && (
		::_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0))
	{
		CNodeList   * pNode = (CNodeList *)msg.pSender->GetTag();
		CreateChatDailog(pNode);
	}
}

int     CZiMainFrame::CreateChatDailog(CNodeList * pNodeList, CChatDialog ** ppChatDialog)
{
	CChatDialog * pChatDialog = 0;

	if(pNodeList->GetNodeData().IsInvalid())
	{
		CZimcHelper::ErrorMessageBox(m_hWnd, Msg_OpenChatWindow, Error_InvalidUser);
		return 1;
	}

	ImcSubWindowTable_t::iterator it = m_mapSubWindows.find(pNodeList->GetNodeData().nId);
	if(it != m_mapSubWindows.end()) 
	{
		pChatDialog = static_cast<CChatDialog*>(it->second);

		if(pChatDialog)
		{
			if(ppChatDialog) *ppChatDialog = pChatDialog;
			::FlashWindow(pChatDialog->GetHWND(), TRUE);
			return 0;
		}
	}

	int nType = pNodeList->GetNodeData().Type();
	switch(nType)
	{
	case Type_ImcFriend:
	case Type_ImcFriendX:
	case Type_ImcGroup:
	case Type_ImcTalk:
		// 此处加载聊天的好友信息, 注意它对于群来说不是一个. 
		// ...
		pChatDialog = new CChatDialog(
			nType, m_itemSelfInfo, pNodeList, &m_chatFont, this);
		break;

	case Type_ImcTeam:
		break;

	default:
		Assert(0);
		break;
	}

	if(pChatDialog)
	{
		// 此处需要将 chat dialog 加入到 main frame 的 chat 表中. ??
		// 注意不需要进行 chat dialog 的回收. 
		
		pChatDialog->Create(NULL, pNodeList->GetNodeData().tstrNickName.c_str(), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
		pChatDialog->CenterWindow();
		::ShowWindow(pChatDialog->GetHWND(), SW_SHOW);

		//::SetForegroundWindow(pChatDialog->GetHWND());

		m_mapSubWindows.insert(std::make_pair(pNodeList->GetNodeData().nId, pChatDialog));
	}

	if(ppChatDialog) 
	{
		*ppChatDialog = pChatDialog;
	}

	return 0;
}

int     CZiMainFrame::CreateChatDailog(int nId, CChatDialog ** ppChatDialog)
{
	CNodeList * pNode = GetNodeInfo(nId);
    if (!pNode) {
        return 1;
    }
	return CreateChatDailog(pNode, ppChatDialog);
}

void    CZiMainFrame::UpdateBackground()
{
	// 更换背景图片. 
	//CControlUI * pBackground = m_pmUi.FindControl(_T("BackgroudImg"));

	//if(pBackground)
	//{
	//	TCHAR  tszBuf[MAX_PATH] = {0};
	//	_stprintf_s(tszBuf, MAX_PATH - 1, _T("file='bg%d.png' corner='600,200,1,1'"), m_nBackgroundImgIndex);
	//	pBackground->SetBkImage(tszBuf);
	//}
}

void    CZiMainFrame::UpdateSelfInfo(LoginScData_t * pLoginInfo)
{
	Assert(pLoginInfo);
	ItemDataNetToLocal(pLoginInfo->selfInfo, m_itemSelfInfo);
}

void    CZiMainFrame::UpdateFriendsList(LoginScData_t * pFriendsInfo)
{
	Assert(pFriendsInfo);

	if(!pFriendsInfo->pTeamInfo || 
		pFriendsInfo->nTeamSize <= 0)
	{
		// 可以为空. 
		return ;
	}

	int nTeamSize = pFriendsInfo->nTeamSize;
	NetItemInfo_t * pTeamInfo = pFriendsInfo->pTeamInfo;

	for(int i = 0; i < nTeamSize; i++)
	{
		// 1 增加组: pTeamInfo[i]
		AddItem(pTeamInfo + i, (NetItemInfo_t *)0);

		// 2 
		int nItemSize = pTeamInfo[i].nFriendSize;
		NetItemInfo_t * pItemInfo = pTeamInfo[i].pFriendInfo;

		for(int j = 0; j < nItemSize; j++)
		{
			// 增加好友: pItemInfo[j];
			AddItem(pItemInfo + j, pTeamInfo + i);
		}
	}

	m_nMaxTeamId  = nTeamSize;
}

void    CZiMainFrame::UpdateGroupsList(LoginScData_t *  pGroupsInfo)
{
	Assert(pGroupsInfo);

	if(!pGroupsInfo->pGroupInfo || 
		pGroupsInfo->nGroupSize <= 0)
	{
		// 可以为空. 
		return ;
	}

	int nGroupSize = pGroupsInfo->nGroupSize;
	NetItemInfo_t * pGroupInfo = pGroupsInfo->pGroupInfo;

	for(int i = 0; i < nGroupSize; i++)
	{
		// 1 增加组: pTeamIno[i]
		AddItem(pGroupInfo + i, (NetItemInfo_t *)0);

		// 2 
		int nItemSize = pGroupInfo[i].nFriendSize;
		NetItemInfo_t * pItemInfo = pGroupInfo[i].pFriendInfo;

		for(int j = 0; j < nItemSize; j++)
		{
			// 增加好友: pItemInfo[j];
			AddItem(pItemInfo + j, pGroupInfo + i);
		}
	}
}

void    CZiMainFrame::UpdateTalksList(LoginScData_t  * pTalksInfo)
{
	Assert(pTalksInfo);

	if(!pTalksInfo->pTalkInfo || 
		pTalksInfo->nTalkSize <= 0)
	{
		// 可以为空. 
		return ;
	}

	int nGroupSize = pTalksInfo->nTalkSize;
	NetItemInfo_t * pTalkInfo = pTalksInfo->pTalkInfo;

	for(int i = 0; i < nGroupSize; i++)
	{
		// 1 增加组: pTeamIno[i]
		AddItem(pTalkInfo + i, (NetItemInfo_t *)0);

		// 2 
		int nItemSize = pTalkInfo[i].nFriendSize;
		NetItemInfo_t * pItemInfo = pTalkInfo[i].pFriendInfo;

		for(int j = 0; j < nItemSize; j++)
		{
			// 增加好友: pItemInfo[j];
			AddItem(pItemInfo + j, pTalkInfo + i);
		}
	}
}



// ------------------------------------------------------------------------
// return, click. 
int     CZiMainFrame::OnClickRightButton(TNotifyUI & msg)
{
	CMenuWnd * pMenu = new CMenuWnd();
	if(!pMenu)  return -1;

	POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
	::ClientToScreen(*this, &pt);
	pMenu->Init(msg.pSender, pt);

	return 0;
}

int     CZiMainFrame::OnCreateGroup(TNotifyUI &msg) {
	if (Event_CreateGroup == msg.wParam) {
		// TODO 先模拟发送创建群的命令
		if (m_pCreateGroupWindow) {
			::SetForegroundWindow(m_pCreateGroupWindow->GetHWND());
			return 0;
		}
		m_pCreateGroupWindow = new CCreateGroupWindow(this);
		m_pCreateGroupWindow->Create(NULL, _T("创建群组"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
		m_pCreateGroupWindow->CenterWindow();
		m_pCreateGroupWindow->ShowWindow(true);	
		::SetForegroundWindow(m_pCreateGroupWindow->GetHWND());
	}
	else {
		Assert(0);
	}
	return 0;
}

int CZiMainFrame::OnModifyGroupName(CNodeList *pNode) {	
	if (pNode->GetNodeData().chType != Type_ImcGroup ) return -1;
	
	if (m_pModifyGroupWindow) {
		::SetForegroundWindow(m_pModifyGroupWindow->GetHWND());
		return 0;
	}
	CT2A pszRecverName(pNode->GetNodeData().tstrNickName.c_str());
	m_pModifyGroupWindow = new CCreateGroupWindow(this, 1, pszRecverName, pNode->GetNodeData().nId);
	m_pModifyGroupWindow->Create(NULL, _T("修改群名称"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	m_pModifyGroupWindow->CenterWindow();
	m_pModifyGroupWindow->ShowWindow(true);	
	return 0;
}

int     CZiMainFrame::DelItem(CNodeList *pNode) {

	Assert(pNode);
	DelFriendItem dfi;
	dfi.type = pNode->GetNodeData().chType;
	dfi.nSendId = IdLocalToNet(Type_ImcFriend, m_itemSelfInfo.nId);
	dfi.strSendName = CT2A(m_itemSelfInfo.tstrNickName.c_str());

	dfi.nDelId = IdLocalToNet(dfi.type, pNode->GetNodeData().nId);
	dfi.strdelName = CT2A(pNode->GetNodeData().tstrNickName.c_str());

	if (dfi.type == Type_ImcTeam && dfi.strdelName.compare("我的好友") == 0) {
		//MessageBox(m_hWnd, _T("不能删除默认分组"), _T("错误"), 0);
		return 0;
	}

	SendImMessageX(Msg_CsDelFriend, (LPARAM)&dfi, sizeof(dfi));
	return 0;
}

int     CZiMainFrame::OnClickRightMenu(TNotifyUI & msg)
{
	CNodeList * pNode  = 0;
	CNodeList * pNodeP = 0;
	CListUI   * pList  = static_cast<CListUI*>(msg.pSender);
	int         nSel   = pList->GetCurSel();

	if(nSel < 0 || !pList->GetItemAt(nSel))
	{
		return 0;
	}

	// 右键操作
	pNode = (CNodeList *)pList->GetItemAt(nSel)->GetTag();
	Assert(pNode);

    //TODO 
    //发送命令道服务器端，等服务端验证成功后再做操作
    //发送网络命令
	switch(msg.wParam)
	{
	case Event_OpenItem:   { CreateChatDailog(pNode);  break; }
	case Event_ModifyItem: { OnModifyGroupName(pNode); break; }
	case Event_DeleteItem: 
        { 
			DelItem(pNode);
        }
        break;
	default:               
		{ Assert(0); break; }
	}

	return 0;
}

int     CZiMainFrame::OnReturnSearch(TNotifyUI & msg)
{
	CEditUI  * pSearchEdit  = DuiControl(CEditUI,  _T("SearchEdit"));
	CComboUI * pSearchCombo = DuiControl(CComboUI, _T("SearchCombo"));

	Assert(pSearchEdit && pSearchCombo);

	LPCTSTR    ptsItemName = pSearchEdit->GetText().GetData();
	if(!ptsItemName) return 0;

	if (wcscmp(ptsItemName, m_itemSelfInfo.tstrNickName.c_str()) == 0) {
		return 0;
	}

	// 增加 pSearchCombo 列表并显示. 
	// 测试代码 ***
	//AddComboItem(_T("111111"));
	//return 0;

	ImcNodeList_t nodeList;
	int size = pSearchCombo->GetCount();
	ClearComboItem();
	m_searchNodes.clear();
	SearchFriendsNode(ptsItemName, m_searchNodes);

	for(ImcNodeList_t::iterator it = m_searchNodes.begin(); 
		it != m_searchNodes.end(); it++)
	{
		ItemNodeInfo_t & itemInfo = (*it)->GetNodeData();
		if(itemInfo.Type() == Type_ImcFriend || 
			itemInfo.Type() == Type_ImcFriendX)
		{
			Assert(!itemInfo.IsInvalid());
			if(::_tcsstr(itemInfo.tstrNickName.c_str(), ptsItemName))
			{
				AddComboItem(itemInfo.tstrNickName.c_str());
			}
		}
	}

	// 不能够显示出来 list 的内容来, 不知道为什么 ?
	// HWND hF = ::GetFocus();
	::SendMessage(::GetFocus(), WM_KILLFOCUS, 0, 0);
	pSearchCombo->SetFocus();
	pSearchCombo->Activate();
	return 0;
}

int     CZiMainFrame::OnSelectSearch(TNotifyUI & msg)
{
	CEditUI  * pSearchEdit   = DuiControl(CEditUI,  _T("SearchEdit"));
	CComboUI * pSearchCombo  = DuiControl(CComboUI, _T("SearchCombo"));
	LPCTSTR    ptsSearchName = pSearchEdit->GetText().GetData();

	Assert(pSearchEdit && pSearchCombo);
	Assert(ptsSearchName);

	int        nIndex        = pSearchCombo->GetCurSel();
	if(nIndex < 0) return 0;

	CListLabelElementUI * pItem = static_cast<CListLabelElementUI*>(
		pSearchCombo->GetItemAt(nIndex));

	Assert(pItem);
	Assert(nIndex < (int)m_searchNodes.size());
	pSearchEdit->SetText(pItem->GetText());
	CreateChatDailog(m_searchNodes[nIndex]);

	return 0;
}

int     CZiMainFrame::OnClickMatchButton(TNotifyUI & msg)
{
	CRichEditUI * pSearchTip  = static_cast<CRichEditUI*>(msg.pSender);
	CRichEditUI * pSearchEdit = DuiControl(CRichEditUI, _T("SearchEdit"));
	ExchangeEditFocus(pSearchTip, pSearchEdit);

	return 0;
}

int     CZiMainFrame::OnClickSettingButton(TNotifyUI & msg)
{
    if (m_pSettingWindow) {
        ::SetForegroundWindow(m_pSettingWindow->GetHWND());
        return 0;
    }
    m_pSettingWindow = new CSettingWindow(this);
    if(!m_pSettingWindow) return -1;
    m_pSettingWindow->Create(NULL, _T("SeetingWndx"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
    m_pSettingWindow->CenterWindow();
    m_pSettingWindow->ShowWindow(true);
   
	return 0;
}

int     CZiMainFrame::OnClickSearchButton(TNotifyUI & msg)
{
	if(m_pSearchWindow) 
	{
		::SetForegroundWindow(m_pSearchWindow->GetHWND());
		return 0;
	}

	m_pSearchWindow = new CZiSearchWindow(this);
	if(!m_pSearchWindow) return -1;

	m_pSearchWindow->Create(NULL, _T("SearchWndX"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	m_pSearchWindow->CenterWindow();
	m_pSearchWindow->ShowWindow(true);

	return 0;
}

int     CZiMainFrame::AddComboItem(LPCTSTR lpctsName)
{
	Assert(lpctsName);

	CComboUI * pSearchCombo = DuiControl(CComboUI, _T("SearchCombo"));

	CListLabelElementUI * pItemUi = new CListLabelElementUI;
	if(!pItemUi) return -1;

	pItemUi->SetAttribute(_T("text"),   lpctsName);
	pItemUi->SetAttribute(_T("font"),   _T("2"));
	pItemUi->SetAttribute(_T("height"), _T("23"));
	RECT rect = {8,1,1,1};
	pItemUi->SetPadding(rect);

	if(!pSearchCombo->Add(pItemUi))
	{
		delete pItemUi;
		return -1;
	}

	return 0;
}

int     CZiMainFrame::ClearComboItem()
{
	CComboUI * pSearchCombo = DuiControl(CComboUI, _T("SearchCombo"));
	Assert(pSearchCombo);

	/*
	for(int i = 0; i < pSearchCombo->GetCount(); i++)
	{
		CListLabelElementUI * pItem = static_cast<CListLabelElementUI*>(
			pSearchCombo->GetItemAt(i));
		delete pItem;
	}
	*/

	pSearchCombo->RemoveAll();
	return 0;
}


// ------------------------------------------------------------------------
// other. 
const   TCHAR * CZiMainFrame::GetCurListName(int nIndex)
{
	switch(nIndex)
	{
	case 0:  return _T("FriendsList");
	case 1:  return _T("GroupsList");
	default: Assert(0);
	}

	return 0;
}

void    CZiMainFrame::ExchangeEditFocus(CRichEditUI * pEditUi1, CRichEditUI * pEditUi2)
{
	//Assert(pEditUi1 && pEditUi2);

	//if(pEditUi1 && pEditUi2)
	//{
	//	pEditUi2->SetText(pEditUi1->GetText());
	//	pEditUi1->SetVisible(false);
	//	pEditUi2->SetVisible(true);
	//}
}


// ------------------------------------------------------------------------
// action. 
CBaseItemListUI * CZiMainFrame::GetNodeListUi(int nNodeType)
{
	switch(nNodeType)
	{
	case Type_ImcFriend:
	case Type_ImcTeam:
		return DuiControl(CFirendItemUI, _T("FriendsList"));

	case Type_ImcFriendX:
	case Type_ImcGroup:
	case Type_ImcTalk:
		return DuiControl(CGroupItemUI,  _T("GroupsList"));

	default:
		Assert(0);
		return 0;
	}
}

CNodeList       * CZiMainFrame::GetNodeInfo(int nId)
{
	ImcNodeTable_t::iterator it = m_mapNodeTable.find(nId);
	if(it != m_mapNodeTable.end())
	{
		return it->second;
	}

	return 0;
}

ItemNodeInfo_t  * CZiMainFrame::GetNodeData(int nId)
{
	if(nId == m_itemSelfInfo.nId)
	{
		return &m_itemSelfInfo;
	}

	CNodeList * pNodeList = GetNodeInfo(nId);
	if(pNodeList)
	{
		return &pNodeList->GetNodeData();
	}

	return 0;
}


CNodeList       * CZiMainFrame::GetTeamInfo(LPCTSTR tsTeamName)
{
	if(!tsTeamName) return 0;

	for(int i = 1; i < (m_nMaxTeamId + 1); i++)
	{
		ImcNodeTable_t::iterator it = m_mapNodeTable.find(i);
		if(it != m_mapNodeTable.end())
		{
			if(::_tcscmp(tsTeamName, (*it).second->GetNodeData().tstrNickName.c_str()) == 0)
			{
				return (*it).second;
			}
		}
	}

	return 0;
}

int     CZiMainFrame::FindTeamId(LPCTSTR tsTeamName)
{
	if(!tsTeamName) return 0;

	for(int i = 0; i < m_nMaxTeamId; i++)
	{
		ImcNodeTable_t::iterator it = m_mapNodeTable.find(i);
		if(it != m_mapNodeTable.end())
		{
			if(::_tcscmp(tsTeamName, (*it).second->GetNodeData().tstrNickName.c_str()) == 0)
			{
				return i;
			}
		}
	}

	return 0;
}

bool    CZiMainFrame::IsUnknownFriend(int nId)
{
	bool bRet = true;

	if(nId == m_itemSelfInfo.nId)
	{
		MessageBox(m_hWnd, _T("不能添加自己"), _T("提示"), 0);
		return false;
	}

	CNodeList * pNode = GetNodeInfo(nId);
	if(pNode && 
		pNode->GetNodeData().chType == Type_ImcFriend)
	{
		MessageBox(m_hWnd, _T("您已经添加了此好友"), _T("提示"), 0);
		return false;
	}

	return bRet;
}

bool    CZiMainFrame::IsUnknownGroup(int nid) {
	bool bRet = true;
	CNodeList *pNode = GetNodeInfo(nid);
	if (pNode && pNode->GetNodeData().chType == Type_ImcGroup) {
		MessageBox(m_hWnd, _T("您已经加入了该群组"), _T("提示"), 0);
		bRet = false;
	}
	return bRet;
}

int     CZiMainFrame::SearchFriendsNode(LPCTSTR lptsMask,  ImcNodeList_t & nodeList)
{
	Assert(lptsMask);

	for(ImcNodeTable_t::iterator it = m_mapNodeTable.begin(); 
		it != m_mapNodeTable.end(); it++)
	{
		Assert(it->second);
		ItemNodeInfo_t & itemInfo = it->second->GetNodeData();
		if(itemInfo.chType == Type_ImcFriend || 
			itemInfo.chType == Type_ImcFriendX)
		{
			if(::_tcsstr(itemInfo.tstrNickName.c_str(), lptsMask))
			{
				nodeList.push_back(it->second);
			}
		}
	}

	return 0;
}

int     CZiMainFrame::AddItem (NetItemInfo_t  * pItemInfo, NetItemInfo_t * pItemInfoParent)
{
	Assert(pItemInfo);

	CNodeList * pNodeParent = 0;

	if(pItemInfoParent)
	{
		pNodeParent = GetNodeInfo(pItemInfoParent->nId);
	}

	return AddItem(pItemInfo, pNodeParent);
}

int     CZiMainFrame::AddItem (NetItemInfo_t  * pItemInfo, CNodeList * pNodeInfoParent)
{
	Assert(pItemInfo);

	ItemNodeInfo_t item;
	ItemDataNetToLocal(*pItemInfo, item);
	return AddItem(&item, pNodeInfoParent);
}

int     CZiMainFrame::AddItem (ItemNodeInfo_t * pItemInfo, CNodeList * pNodeInfoParent)
{
	// 1 增加 Team,  时, parent 为 0. 
	// 2 增加 Group  时, parent 为 0. 
	// 3 增加 Friend 成员, parent 存在. 
	// 4 增加 Group  成员, ...
	// 
	// 群和组的数据不共用 (CNodeList). ??? 
	// 群 ID 存在于 m_mapNodeTable 中. 
	// -- 创建
	// -- 那么修改时得修改多份, 组里一份, 群里可能多份, 
	//    能修改吗 ???
	// -- 删除时 ??
	//    删除群不修改组, 只针对群. 
	//    删除组, 群里对应的的内容均被删除 (设置为不可用) ???

	Assert(pItemInfo && !pItemInfo->IsInvalid());
	Assert(!pNodeInfoParent || !pNodeInfoParent->GetNodeData().IsInvalid());

	CBaseItemListUI * pItemListUi = GetNodeListUi(
		pNodeInfoParent ? pNodeInfoParent->GetNodeData().chType : pItemInfo->chType);
	Assert(pItemListUi);

	CNodeList * pNodeInfo = pItemListUi->AddNode(*pItemInfo, pNodeInfoParent);
	if(!pNodeInfo) return -1;

	if(pNodeInfoParent && (
		pNodeInfoParent->GetNodeData().chType == Type_ImcTalk || 
		pNodeInfoParent->GetNodeData().chType == Type_ImcGroup))
	{
		// 增加群成员 [4]
		// 可能含有无效(被删除了)的好友
		// 对于此类好友, 可以特殊处理 ...???
		//if(pNodeInfoParent->GetNodeData().chType == Type_ImcTalk)
		//{
		//	Assert(m_itemSelfInfo.nId == pItemInfo->nId || 
		//		m_mapNodeTable.find(pItemInfo->nId) != m_mapNodeTable.end());
		//}
		//else
		//{
		//	// 仅仅是增加
		//	//Assert(pNodeInfoParent->GetNodeData().nGroupAdminId == m_itemSelfInfo.nId);
		//}

		ImcGroupTable_t::iterator it = m_mapGroupTable.find(pNodeInfoParent); 
		if(it != m_mapGroupTable.end())
		{
			it->second.insert(std::make_pair(pItemInfo->nId, pNodeInfo));
		}
		else
		{
			ImcNodeTable_t groupList;
			groupList.insert(std::make_pair(pItemInfo->nId, pNodeInfo));
			m_mapGroupTable.insert(std::make_pair(pNodeInfoParent, groupList));
		}
	}
	else
	{
		// 非群成员. [1, 2, 3]
		//Assert(m_mapNodeTable.find(pItemInfo->nId) == m_mapNodeTable.end());
		m_mapNodeTable.insert(std::make_pair(pItemInfo->nId, pNodeInfo));
	}
	return 0;
}

int     CZiMainFrame::DelItem (CNodeList * pNodeInfo, CNodeList * pNodeInfoParent)
{
	Assert(pNodeInfo);
	Assert(pNodeInfoParent);
	Assert(!pNodeInfo->GetNodeData().IsInvalid());
	Assert(!pNodeInfoParent->GetNodeData().IsInvalid());

	int nType       = pNodeInfo->GetNodeData().chType;
	int nId         = pNodeInfo->GetNodeData().nId;
	int nParentType = pNodeInfoParent->GetNodeData().chType;

	if(nParentType == Type_ImcGroup || nParentType == Type_ImcTalk)
	{
		// 删除群成员. 
		if(nParentType == Type_ImcGroup)
		{
			Assert(nType == Type_ImcFriendX);
			if(pNodeInfoParent->GetNodeData().nGroupAdminId != m_itemSelfInfo.nId)
			{
				return Error_NoPermission;
			}
		}

		DeleteGroupFriendNode(pNodeInfoParent, nId);
	}
	else
	{
		Assert(m_mapNodeTable.find(nId) != m_mapNodeTable.end());
		
		if(nType == Type_ImcFriend)
		{
			DeleteTeamFriendNode(nId);
		}
		else if(nType == Type_ImcGroup || nType == Type_ImcTalk)
		{
			if(nType == Type_ImcGroup && 
				pNodeInfo->GetNodeData().nGroupAdminId != m_itemSelfInfo.nId)
			{
				return Error_NoPermission;
			}

			DeleteGroupNode(nId);
		}
		else if(nType == Type_ImcTeam)
		{
			DeleteTeamNode(nId);
		}
	}

	ItemNodeInfo_t  & itemInfo    = pNodeInfo->GetNodeData();
	CBaseItemListUI * pItemListUi = GetNodeListUi((
		itemInfo.chType == Type_ImcFriend || itemInfo.chType == Type_ImcFriendX) ? 
		pNodeInfoParent->GetNodeData().chType : itemInfo.chType);

	Assert(pItemListUi);
	pItemListUi->RemoveNode(pNodeInfo);

	return 0;
}

int     CZiMainFrame::MoveItem(CNodeList * pNodeInfo, CNodeList * pNodeInfoSrcParent, CNodeList * pNodeInfoDesParent)
{
	// parent 必须存在, 也就是根节点不能移动. 
	// 增加群成员, 不能调用 MoveItem, 必须调用 AddItem. 
	Assert(pNodeInfo);
	Assert(pNodeInfoSrcParent && pNodeInfoDesParent);
	Assert(pNodeInfo->GetNodeData().chType == Type_ImcFriend);
	Assert(pNodeInfoSrcParent->GetNodeData().chType == Type_ImcTeam);
	Assert(pNodeInfoSrcParent->GetNodeData().chType == pNodeInfoDesParent->GetNodeData().chType);

	CBaseItemListUI * pItemListUi = GetNodeListUi(pNodeInfoSrcParent->GetNodeData().chType);
	ItemNodeInfo_t    itemInfo    = pNodeInfo->GetNodeData();

	DelItem(pNodeInfo, pNodeInfoSrcParent);
	AddItem(&itemInfo, pNodeInfoDesParent);

	// 网络发送 ... ???
	return 0;
}

int     CZiMainFrame::ModifyItem(CNodeList * pNodeInfo)
{
	Assert(pNodeInfo);

	if(pNodeInfo->GetNodeData().IsInvalid())
	{
		// invalid. 
		return  1;
	}

	if(!(pNodeInfo->GetNodeData().chStatus & State_Write))
	{
		// can not write. 
		return 2;
	}

	// 1 ???
	// 2 网络发送 ... ???
	return 0;
}

CNodeList * CZiMainFrame::GetFriendTeamNode()
{
	// 获取 "好友" 组列表 Node 信息. 
	// 在原来的设置中没有这个
	// ...???
	#ifdef _DEBUG
		return GetNodeInfo(501);
	#endif

	return 0;
}

void    CZiMainFrame::DeleteGroupNode(int nId)
{
	//for(ImcGroupTable_t::iterator it = m_mapGroupTable.begin(); 
	//	it != m_mapGroupTable.end(); it++)
	//{
	//	it->second.clear();
	//}

	//m_mapGroupTable.clear();

	ImcNodeTable_t::iterator it = m_mapNodeTable.find(nId);
	if(it != m_mapNodeTable.end())
	{
		//delete (it->second);
		m_mapNodeTable.erase(it);
	}
}

void    CZiMainFrame::DeleteGroupFriendNode(CNodeList * pParent, int nId)
{
	Assert(pParent);

	ImcGroupTable_t::iterator it = m_mapGroupTable.find(pParent);
	if(it != m_mapGroupTable.end())
	{
		it->second.erase(it->second.find(nId));
	}
}

void    CZiMainFrame::DeleteTeamNode(int nId)
{
	ImcNodeTable_t::iterator it = m_mapNodeTable.begin();
	while(it != m_mapNodeTable.end())
	{
		// 组
		Assert(it->second);
		if(nId == it->second->GetNodeData().nId)
		{
			m_mapNodeTable.erase(it);
			it = m_mapNodeTable.begin();
			continue;
		}

		// 组成员
		Assert(it->second->GetParent());
		if(nId == it->second->GetParent()->GetNodeData().nId)
		{
			InvalidGroupFriendNode(it->second->GetNodeData().nId);
			m_mapNodeTable.erase(it);
			it = m_mapNodeTable.begin();
			continue;
		}

		it++;
	}
}

void    CZiMainFrame::DeleteTeamFriendNode(int nId)
{
	ImcNodeTable_t::iterator it = m_mapNodeTable.find(nId);
	if(it != m_mapNodeTable.end())
	{
		m_mapNodeTable.erase(it);
	}

	InvalidGroupFriendNode(nId);
}

void    CZiMainFrame::InvalidGroupFriendNode(int nId)
{
	for(ImcGroupTable_t::iterator it1 = m_mapGroupTable.begin(); 
		it1 != m_mapGroupTable.end(); it1++)
	{
		for(ImcNodeTable_t::iterator it2 = it1->second.begin(); 
			it2 != it1->second.end(); it2++)
		{
			Assert(it2->second);
			if(it2->second->GetNodeData().nId == nId)
			{
				// ******
				it2->second->GetNodeData().chStatus |= State_Invalid;
				it2->second->GetNodeData().chType   |= Type_ImcInvalid;
			}
		}
	}
}

void    CZiMainFrame::CancelWatch(int nChatId)
{
	ImcSubWindowTable_t::iterator it = m_mapSubWindows.find(nChatId);
	Assert(it != m_mapSubWindows.end());
	m_mapSubWindows.erase(it);
}


// -----------------------------------------------------------------------------
// Net Msg, Delay Handle, OnTimer
int     CZiMainFrame::SendImMessageX(int nMsg, LPARAM lp, WPARAM wp)
{
	Assert(m_pMainMsger);
	return m_pMainMsger->SendImMessage(nMsg, lp, wp);
}

int     CZiMainFrame::OnClickVerifyResponse(WPARAM wp, LPARAM lp)
{
	Assert(lp);
	VerifyScQuery_t    * pVerifyq = (VerifyScQuery_t *)lp;
	VerifyCcResponse_t   verifyp;

	if(wp == 1)
	{
		if(pVerifyq->nRecvType == Type_ImcFriend)
		{
			Assert(pVerifyq->pSenderNetQInfo && 
				pVerifyq->pSenderNetQInfo->nItemType == Type_ImcFriend);
			CNodeList * pTeamInfo = GetTeamInfo(Team_DefaultNameT);

			Assert(pTeamInfo);
			if(pTeamInfo) 
			{
				pVerifyq->pSenderNetQInfo->strAdminName = Team_DefaultNameA;
				AddItem(pVerifyq->pSenderNetQInfo, pTeamInfo);
			}
		}
	}

	// Msg_ScQueryVerify -> Msg_CsResponseVerify ..??
	verifyp.bIsAgree     = wp == 1 ? 1 : 0;
	verifyp.nSenderId    = MagicId_t(Msg_CsResponseVerify, pVerifyq->nRecvType, pVerifyq->nRecverId);
	verifyp.nRecverId    = MagicId_F(Msg_CsResponseVerify, pVerifyq->nSenderId);
	verifyp.szSenderName = pVerifyq->szRecverNamex;
	verifyp.szRecverName = pVerifyq->szSenderNamex;
	verifyp.nRecvType    = pVerifyq->nRecvType;
	verifyp.pSenderLocalRInfo = GetNodeData(pVerifyq->nRecverId);

	Assert(m_pMainMsger);
	m_pMainMsger->SendImMessage(Msg_CsResponseVerify, (LPARAM)&verifyp, sizeof(verifyp));
	m_pMainMsger->FreeDataEx(Msg_ScQueryVerify, pVerifyq);
	return 0;
}

int CZiMainFrame::OnClickAddGroupResponse(WPARAM wp, LPARAM lp) {
	Assert(lp);
	AddGroupInfo_t *pAddgroup = (AddGroupInfo_t *)lp;

	if(wp == 1) { //ok
		//TODD 获取组 然后加入组
		CNodeList *pNodeParent = GetNodeInfo(IdNetToLocal(Type_ImcGroup,pAddgroup->groupinfo.group_id));
		if (pNodeParent) {
			ItemNodeInfo_t item;
			ItemDataNetToLocal(pAddgroup->userinfo, item, Type_ImcFriend);
			//TODO
			ImcGroupTable_t::iterator it = m_mapGroupTable.find(pNodeParent); 
			if(it != m_mapGroupTable.end()) {
				if (it->second.find(item.nId) == it->second.end()) {
					AddItem(&item, pNodeParent);
				}
			}
			else {
				AddItem(&item, pNodeParent);
			}
		}
	}
	// Msg_ScQueryVerify -> Msg_CsResponseVerify ..??
	//100 拒绝
	pAddgroup->succ = wp == 1 ? 0 : 100;
	pAddgroup->type = GROUP_INFO_REPLY;
	Assert(m_pMainMsger);
	m_pMainMsger->SendImMessage(Msg_CsAddGroupVerify, (LPARAM)pAddgroup, sizeof(AddGroupInfo_t));
	m_pMainMsger->FreeDataEx(Msg_ScAddGroupVerify, pAddgroup);
	return 0;
}

void    CZiMainFrame::FlashTray(BOOL bFlashOrNot)
{
	if(bFlashOrNot == m_bFlash) return ; 

	// ... ???

	m_bFlash = bFlashOrNot;
}

void    CZiMainFrame::OnTimer()
{
    OnKeepAlive();
	HTTPRequest::instance()->dispatch();
	//Assert(!m_listNetData.empty());
    CGenericLockHandler h(m_mainLock);
    if (!m_listNetData.empty()) {
        Assert(m_pTrayWindow);
	    m_pTrayWindow->FlashTray();
    }
}

void CZiMainFrame::OnKeepAlive() {
    time_t now = time(NULL);
    if (now >= m_nlastKeepAlive_time_ + 5) {
        m_nlastKeepAlive_time_ = now;
        m_pMainMsger->SendImMessage(Msg_KeepAlive, NULL, 0);
    }
}

void    CZiMainFrame::DelayDispatchNetCmd(int nMsg, int friend_id, void * pNetData)
{
	CGenericLockHandler h(m_mainLock);
	pair <int,int> mapkey = make_pair<int,int>(nMsg, friend_id);
	m_listNetData[mapkey].push_back(pNetData);
	//m_listNetData.push_back(make_pair<pair <int,int>, void*>(mapkey, pNetData));

    /*
	if(m_nTimer != 0) return;
	m_nTimer = ::SetTimer(m_hWnd, 1, 1000, 0);
    */
	return;
}

BOOL    CZiMainFrame::HandleNetCmd()
{
	if(m_listNetData.empty() && 
		m_nTimer == 0)
	{
		return FALSE;
	}

	bool         bDo = FALSE;
	ImcNetData_t netData;
	do
	{
		CGenericLockHandler h(m_mainLock);
		if (m_listNetData.size() > 0) {
			ImcNetDataMap_t::iterator iter = m_listNetData.begin();
			for (size_t i = 0; i < iter->second.size(); i++) {
				HandleNetMessage(iter->first.first, iter->second[i]);
			}
			iter->second.clear();
			bDo = true;
			m_listNetData.erase(iter);
		}
		if(m_listNetData.empty() && m_nTimer != 0)
		{
			//::KillTimer(m_hWnd, m_nTimer);
			//m_nTimer = 0;
            m_pTrayWindow->FlashTray(TRUE, FALSE);
		}

	}while(0);
	if (!bDo) { m_pTrayWindow->FlashTray(TRUE,FALSE);}
	return bDo;
}

int     CZiMainFrame::HandleNetMessage(int nMsg, void * pNetData)
{
	// 未处理添加群组 ... ???
	Assert(pNetData);
	int  nRet  = 0;
	bool bFree = TRUE;

	switch(nMsg)
	{
	case Msg_ScQueryVerify:
		{
			// 显示验证请求框, 交给用户操作. 
			// 还需要显示请求者的信息, 未实现 ... ???
			VerifyScQuery_t * pVerifyQuery = (VerifyScQuery_t*)pNetData;
			char              szText[1024] = {0};
			if (m_itemSelfInfo.nAddFriendType == 1) {
				sprintf_s(szText, sizeof(szText)/sizeof(szText[0]), 
					"'%s'(%d) 请求为好友, 验证信息: '%s'", 
					pVerifyQuery->szSenderNamex, 
					pVerifyQuery->nSenderId, 
					pVerifyQuery->szVerifyDatax);
				CNotifyWindow::MessageBoxX(m_hWnd, _T("验证消息"), 
					CA2T(szText), _T("同意"), _T("不同意"), 
					Msg_InQueryVerify, pVerifyQuery);
			}
			else {
				sprintf_s(szText, sizeof(szText)/sizeof(szText[0]), 
					"'%s' 将您添加为好友,附加信息: %s", 
					pVerifyQuery->szSenderNamex, 
					pVerifyQuery->szVerifyDatax);
				CNotifyWindow::MessageBoxX(m_hWnd, _T("通知"), CA2T(szText));
			}
			bFree = FALSE;
		}
		break;

	case Msg_ScResponseVerify: 
		{
			// 显示验证回复框, 通知用户. 
			// 如果失败可以不回复对方. 
			VerifyCcResponse_t * pVerifyResp  = (VerifyCcResponse_t*)pNetData;
			char szText[1024] = {0};
			if (pVerifyResp->nAddFriendType == 1) {
				sprintf_s(szText, sizeof(szText), "'%s' %s  加您为好友",
					pVerifyResp->szSenderName2, 
					(pVerifyResp->bIsAgree ? "同意" : "不同意"));
				CNotifyWindow::MessageBoxX(m_hWnd, _T("验证消息"), CA2T(szText));

				if(pVerifyResp->nRecvType == Type_ImcFriend)
				{
					Assert(pVerifyResp->pSenderNetRInfo && 
						pVerifyResp->pSenderNetRInfo->nItemType == Type_ImcFriend);
					CNodeList * pTeamInfo = GetTeamInfo(Team_DefaultNameT);

					Assert(pTeamInfo);
					if(pTeamInfo) AddItem(pVerifyResp->pSenderNetRInfo, pTeamInfo);
				}
			}
			else {
				sprintf_s(szText, sizeof(szText), "添加好友 '%s' %s", 
					pVerifyResp->szRecverName2, 
					pVerifyResp->bIsAgree? "成功" : "失败");
				CNotifyWindow::MessageBoxX(m_hWnd, _T("通知"), CA2T(szText));
			}
			bFree = TRUE;
		}
		break;

	case Msg_ScTextChat:
		{
			// 没有聊天对话框, 通知用户.  
			// 显示聊天对话框. 
			// 将消息显示到消息框中. ???
			ChatCcTextData_t * pChatText   = (ChatCcTextData_t*)pNetData;
			CChatDialog      * pChatDialog = 0;
            if (pChatText->nRecvType == Type_ImcFriend || pChatText->nRecvType == Type_ImcFriendX) { //好友消息
                CreateChatDailog(pChatText->nSenderId, &pChatDialog);
                if(pChatDialog)  // 可能无效. 
                {
                    pChatDialog->OnTextMsgShow(pChatText);
                    pChatDialog->ActiveWindow();
                }
            }
            else { //群消息
                CreateChatDailog(pChatText->nRecverId, &pChatDialog);
                if (pChatDialog) {
                    pChatDialog->OnTextMsgShow(pChatText);
                    pChatDialog->ActiveWindow();
                }
            }
            bFree = TRUE;
		}
		break;
    case Msg_ScDelFriend:
        {
            //TODO 执行删除好友的操作
			bool popMsgBox = false;
			DelFriendItem *pDelFriend = (DelFriendItem *)pNetData;
			char  tsText[1024] = {0};
			int nDelId = IdNetToLocal(pDelFriend->type, pDelFriend->nDelId);
			int nSendId = IdNetToLocal(Type_ImcFriend, pDelFriend->nSendId);
			//TODO
			if (pDelFriend->type == Type_ImcTeam) {
			}
			else if (pDelFriend->type == Type_ImcFriend) {
				if (nDelId == m_itemSelfInfo.nId) {
					if (pDelFriend->succ == 0) {
						sprintf_s(tsText, sizeof(tsText)/sizeof(tsText[0]), "%s 将您移除好友列表", ((pDelFriend->strSendName.c_str())));
						popMsgBox = true;
					}
				}
				else {
					sprintf_s(tsText, sizeof(tsText)/sizeof(tsText[0]), "删除好友 %s %s", ((pDelFriend->strdelName.c_str())), (pDelFriend->succ == 0 ? "成功" : "失败"));
					popMsgBox = true;
				}		
			}
			else if (pDelFriend->type == Type_ImcFriendX) {
			}
			else if (pDelFriend->type == Type_ImcGroup) {
				if (nSendId == m_itemSelfInfo.nId) {
					sprintf_s(tsText, sizeof(tsText)/sizeof(tsText[0]), "删除群 %s %s", ((pDelFriend->strdelName.c_str())), (pDelFriend->succ == 0 ? "成功" : "失败"));
					popMsgBox = true;
				}
				else if (pDelFriend->succ == 0) {
				    sprintf_s(tsText, sizeof(tsText)/sizeof(tsText[0]), "%s 将您移除群 %s", ((pDelFriend->strSendName.c_str())), (pDelFriend->strdelName.c_str()));
					popMsgBox = true;
				}
			}
			else {
			}
			if (popMsgBox) {
				CNotifyWindow::MessageBoxX(m_hWnd, _T("通知"), CA2T(tsText));
			}
			bFree = TRUE;
        }
        break;
	case Msg_ScAddGroupVerify:
		handlerAddGroupVerify((AddGroupInfo_t *)pNetData);
		break;
	}

	if(bFree) m_pMainMsger->FreeDataEx(nMsg, pNetData);
	return nRet;
}

LRESULT CZiMainFrame::HandleCustomMessage(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bFree = FALSE;

	switch(nMsg)
	{
	case WM_TIMER:
		if(wParam == 1) OnTimer();
		break;

	case Msg_InTray:
		{
			Assert(wParam == IDR_MAINFRAME);

			switch(lParam)
			{
			case WM_RBUTTONUP:
				{
					// 右键单击 -> 弹出右键菜单. 
					// ... 
				}
				break;

			case WM_LBUTTONDBLCLK:
				{
					// 左键双击 -> 1 消息, 2 显示. 
					BOOL bNoMsg  = true;
					BOOL bHide   = ::IsWindowVisible(m_hWnd);

					bNoMsg = HandleNetCmd();
					if(bNoMsg)
					{
						break;
					}
					{
						::ShowWindow(m_hWnd, SW_SHOW);
						::SetForegroundWindow(m_hWnd);
					}
				}
				break;
			}
		}
		break;

	case Msg_InQueryVerify:
		// 发送验证回复消息给对方. 
		Assert(lParam);
		OnClickVerifyResponse(wParam, lParam);
		break;

	case Msg_ScResponseUsers:
		{
			SearchScResponse_t * pResponse = (SearchScResponse_t*)lParam;
			if(wParam != 0)
			{
				CZimcHelper::ErrorMessageBox(m_hWnd, Msg_CsQueryUsers, wParam);
			}
			else if(m_pSearchWindow)
			{
				m_pSearchWindow->HandleResponseResult(pResponse);
			}

			bFree = TRUE;
		}
		break;
	case Msg_ScResponseGroup:
		{
			SearchGroup_t *pSearchGroup = (SearchGroup_t *)lParam;
			if (wParam != 0) {
				CZimcHelper::ErrorMessageBox(m_hWnd, nMsg, wParam);
			}
			else if (m_pSearchWindow) {
				m_pSearchWindow->HandleResponseResultForGroup(pSearchGroup);
			}
			bFree = TRUE;
		}
		break;
	case Msg_ScQueryVerify:
	case Msg_ScResponseVerify:
	case Msg_ScAddGroupVerify: 
		{
			Assert(lParam != 0);
			AddGroupInfo_t *pAddgroupinfo = (AddGroupInfo_t *)lParam;
			DelayDispatchNetCmd(nMsg, pAddgroupinfo->nSenderId, (void*)lParam);
		}
		break;
	case Msg_ScTextChat:
		{
			Assert(lParam != 0);
			CChatDialog * pChatDialog = 0;
            ChatCcTextData_t *pChatText = (ChatCcTextData_t *)lParam;
            if (pChatText->nRecvType == Type_ImcFriend) { //好友消息
                ImcSubWindowTable_t::iterator it = m_mapSubWindows.find(pChatText->nSenderId);
                if(it != m_mapSubWindows.end()) pChatDialog = static_cast<CChatDialog*>(it->second);
            }
            else { //群消息
                ImcSubWindowTable_t::iterator it = m_mapSubWindows.find(pChatText->nRecverId);
                if(it != m_mapSubWindows.end()) pChatDialog = static_cast<CChatDialog*>(it->second);
            }
			if(pChatDialog)
			{
				pChatDialog->OnTextMsgShow(pChatText);
				bFree = TRUE;
			}
			else
			{
				DelayDispatchNetCmd(nMsg, pChatText->nSenderId, (void*)lParam);
			}
		}
		break;
    case Msg_ScDelFriend:
        {
            Assert(lParam != 0);
			
			DelFriendItem *pDelFriend = (DelFriendItem *)lParam;
			int nDelId = IdNetToLocal(pDelFriend->type, pDelFriend->nDelId);
			int nSendId = IdNetToLocal(Type_ImcFriend, pDelFriend->nSendId);
			if (pDelFriend->succ == 0) {				
				//TODO
				if (pDelFriend->type == Type_ImcTeam) {
				}
				else if (pDelFriend->type == Type_ImcFriend) {
					CNodeList *pNodeinfo = NULL;
					if (nDelId == m_itemSelfInfo.nId) {
						pNodeinfo = GetNodeInfo(nSendId);
					}
					else {
						pNodeinfo = GetNodeInfo(nDelId);
					}
					if (pNodeinfo) {
						DelItem(pNodeinfo, pNodeinfo->GetParent());
					}
				}
				else if (pDelFriend->type == Type_ImcFriendX) {

				}
				else if (pDelFriend->type == Type_ImcGroup) {
					if (nDelId != m_itemSelfInfo.nId) {
						CNodeList *pNodeInfo = GetNodeInfo(nDelId);
						m_mapGroupTable.erase(pNodeInfo);
						Assert(pNodeInfo);
						ItemNodeInfo_t  & itemInfo    = pNodeInfo->GetNodeData();
						CBaseItemListUI * pItemListUi = GetNodeListUi(itemInfo.chType);
						Assert(pItemListUi);
						pItemListUi->RemoveNode(pNodeInfo);
						DeleteGroupNode(nDelId);
					}
				}
				else {
				}	
			}
			if (nDelId == m_itemSelfInfo.nId) {
				DelayDispatchNetCmd(nMsg, nSendId, (void*)lParam);
			}
			else {
				DelayDispatchNetCmd(nMsg, nDelId, (void*)lParam);
			}
        }
        break;
	case  Msg_ScEvilReport:
		{
			// 显示验证请求框, 交给用户操作. 
			// 还需要显示请求者的信息, 未实现 ... ???
			ReportCsEvilData_t * pReport = (ReportCsEvilData_t*)lParam;
			char              szText[1024] = {0};
			sprintf_s(szText, sizeof(szText)/sizeof(szText[0]), 
				"举报 %s", pReport->succ == 0 ? "成功" : "失败");

			CNotifyWindow::MessageBoxX(m_hWnd, _T("通知"), CA2T(szText));
			bFree = TRUE;
		}
		break;
	case Msg_ScCreateGroup:
		handleGreateGroup((GroupInfoData_t *)lParam);
		bFree = TRUE;
		break;
	case Msg_InAddGroupVerify:
		Assert(lParam);
		OnClickAddGroupResponse(wParam, lParam);
		break;
	case Msg_ScModifyGroup:
		Assert(lParam);
		handlerModifyGroup((GroupInfoData_t *)lParam);
		break;
	}

	if(bFree) m_pMainMsger->FreeDataEx(nMsg, (void*)lParam);
	return 0;
}

void CZiMainFrame::handleGreateGroup(GroupInfoData_t *pGroupInfoData) {
	if (pGroupInfoData->succ == 0) {
		//TODO 添加群
		ItemNodeInfo_t item;
		ItemDataNetToLocal(*pGroupInfoData, item);
		AddItem(&item, 0);

		//添加成员
		CNodeList *pNodeParent = GetNodeInfo(item.nId);
		Assert(pNodeParent);
		for (size_t i = 0; i < pGroupInfoData->groupinfo.members.size(); i++) {
			ItemNodeInfo_t group_member;
			ItemDataNetToLocal(pGroupInfoData->groupinfo.members[i], group_member, Type_ImcFriendX);
			AddItem(&group_member, pNodeParent);
		}
	}
	char szText[1024] = {0};
	sprintf_s(szText, sizeof(szText)/sizeof(char), "创建 %s %s", pGroupInfoData->groupinfo.name.c_str(),
		(pGroupInfoData->succ == 0 ? "成功" : "失败"));
	CNotifyWindow::MessageBoxX(m_hWnd, _T("通知"), CA2T(szText));
}

void CZiMainFrame::handlerModifyGroup(GroupInfoData_t *pGroupInfoData) {
	if (pGroupInfoData->succ == 0) {
		//TODO 修改群名称
		CNodeList *pNode = GetNodeInfo(pGroupInfoData->groupinfo.group_id);
		Assert(pNode);
		tstring      strHtmlText;
		NodeData_t & itemNode = pNode->GetNodeData();
		itemNode.tstrNickName = CA2T(pGroupInfoData->groupinfo.name.c_str());
		// Team or Group' logo
		if(itemNode.bIsHasChild)
		{
			if(itemNode.bIsChildVisible)
				strHtmlText += _T("<i list_icon_b.png>");
			else
				strHtmlText += _T("<i list_icon_a.png>");
			TCHAR szBuf[MAX_PATH] = {0};
			_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), 10);
			strHtmlText += szBuf;
		}
		// nick name
		strHtmlText += itemNode.tstrNickName;
		CLabelUI* tstrNickName = static_cast<CLabelUI*>(
			m_pmUi.FindSubControlByName(itemNode.pListElement, _T("nickname")));
		if (tstrNickName != NULL)
		{
			if (itemNode.bIsFolder)
				tstrNickName->SetFixedWidth(0);

			tstrNickName->SetShowHtml(true);
			tstrNickName->SetText(strHtmlText.c_str());
		}
	}
	char szText[1024] = {0};
	sprintf_s(szText, sizeof(szText)/sizeof(char), "修改群名称 %s %s", pGroupInfoData->groupinfo.name.c_str(),
		(pGroupInfoData->succ == 0 ? "成功" : "失败"));
	CNotifyWindow::MessageBoxX(m_hWnd, _T("通知"), CA2T(szText));
}

void CZiMainFrame::handlerAddGroupVerify(AddGroupInfo_t *pAddgroup) {
	Assert(pAddgroup);
	// 显示验证请求框, 交给用户操作. 
	// 还需要显示请求者的信息, 未实现 ... ???
	char              szText[1024] = {0};
	if (pAddgroup->type == GROUP_INFO_VERIFY) {
		sprintf_s(szText, sizeof(szText)/sizeof(szText[0]), 
			"'%s' 请求加入群 <%s>, 验证信息: '%s'", 
			pAddgroup->strSenderName.c_str(),
			pAddgroup->groupinfo.name.c_str(),
			pAddgroup->strVerify.c_str());

		CNotifyWindow::MessageBoxX(m_hWnd, _T("验证消息"), 
			CA2T(szText), _T("同意"), _T("不同意"), 
			Msg_InAddGroupVerify, pAddgroup);
	}
	else { //GROUP_INFO_REPLY	
		//TODO 如果添加者 需要弹框提示
		if (IdNetToLocal(Type_ImcFriend, pAddgroup->nSenderId) == m_itemSelfInfo.nId) {
			sprintf_s(szText, sizeof(szText)/sizeof(szText[0]), 
				"加入群 %s %s", 
				pAddgroup->groupinfo.name.c_str(), (pAddgroup->succ == 0 ? "成功" : "失败"));
			CNotifyWindow::MessageBoxX(m_hWnd, _T("通知"),CA2T(szText), 0);
			if (pAddgroup->succ == 0) {
				//TODO 添加群
				ItemNodeInfo_t item;
				ItemDataNetToLocal(pAddgroup->groupinfo, item);
				AddItem(&item, 0);

				//添加成员
				CNodeList *pNodeParent = GetNodeInfo(item.nId);
				Assert(pNodeParent);
				for (size_t i = 0; i < pAddgroup->groupinfo.members.size(); i++) {
					ItemNodeInfo_t group_member;
					ItemDataNetToLocal(pAddgroup->groupinfo.members[i], group_member, Type_ImcFriendX);
					AddItem(&group_member, pNodeParent);
				}
			}
		}
		else if (IdNetToLocal(Type_ImcFriend, pAddgroup->nAdminId) != m_itemSelfInfo.nId && pAddgroup->succ == 0) {
			CNodeList *pNodeParent = GetNodeInfo(IdNetToLocal(Type_ImcGroup,pAddgroup->groupinfo.group_id));
			if (pNodeParent) {
				ItemNodeInfo_t item;
				ItemDataNetToLocal(pAddgroup->userinfo, item, Type_ImcFriend);
				AddItem(&item, pNodeParent);
			}
		}
		m_pMainMsger->FreeDataEx(Msg_ScAddGroupVerify, pAddgroup);
	}	
}

//report window
void    CZiMainFrame::reportEvil(tstring &nickname)
{
	//TODO 
	if(m_pReportWindow) 
	{
		::SetForegroundWindow(m_pReportWindow->GetHWND());
		return ;
	}
	m_pReportWindow = new CReportWindow(this, nickname);
	if(!m_pReportWindow) return ;
	m_pReportWindow->Create(NULL, _T("ReportWndX"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	m_pReportWindow->CenterWindow();
	m_pReportWindow->ShowWindow(true);
	
	return ;
}

