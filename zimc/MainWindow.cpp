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
#include "MsgRecordWindow.h"
#include "CreateGroupWindow.h"

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
	, m_pMsgRecordWindow(0)
	, m_pCreateGroupWindow(0)
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
	// ���� WS_EX_TOOLWINDOW ���ܹ��ɹ�������������ͼ��, 
	// �п��ܻ����³�������������. ��֪��Ϊʲô. ???

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
        m_nTimer = ::SetTimer(m_hWnd, 1, 1000, 0);
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
		// �ж��Ƿ��Ǻ����б����. 
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
	else if(msg.sType == _T("return"))
	{
		DuiClickButtonMap(_T("SearchEdit"),    OnReturnSearch);
	}
	else if(msg.sType == _T("click"))
	{
		DuiClickButtonMap(_T("CloseBtn"),      OnClickClose);
		DuiClickButtonMap(_T("SearchTip"),     OnClickMatchButton);
		DuiClickButtonMap(_T("SettingBtn"),    OnClickSettingButton);
		DuiClickButtonMap(_T("SearchBtn"),     OnClickSearchButton);
	}
	else if(msg.sType == _T("selectchanged"))
	{
		// tab �л�. ( friends �� group )
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
		// ���� select ��Ӧ˫������. 
		OnItemChat(msg);
		OnSelectItemList(msg);
	}
	else if(msg.sType == _T("menu"))
	{
		// �Ҽ�֧��
		//char szLog[256] = {0};
		//sprintf(szLog, "Class: %ws, Name = %ws\n", 
		//	msg.pSender->GetClass(), msg.pSender->GetName());
		//::OutputDebugStringA(szLog);
		DuiClickButtonMap(_T("FriendsList"),    OnClickRightButton);
		DuiClickButtonMap(_T("GroupsList"),     OnClickRightButton);
	}
	else if(msg.sType == _T("menuclick"))
	{
		// �Ҽ����
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
	// ��ʼλ����Ļ�� (3/4, 1/8) ��. 
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

	// ���� login ���ص���Ϣ: ������Ϣ�ͺ�����Ϣ. 
	// 1 *** ����ʹ�÷�����������Ϣ. 
	// 2 *** ���ݸ�ʽ. 
	//   �μ�: LoginScData_t
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

    //��ȡ������Ϣ
    LoginCsData_t loginQuery;
    CT2A pAData1(m_itemSelfInfo.tstrNickName.c_str());
    loginQuery.szAccount = pAData1;
    SendImMessageX(Msg_LoadMessage, (LPARAM)&loginQuery, sizeof(loginQuery));

	return nRet;
}

int     CZiMainFrame::ReadSelfInfo()
{
	// ���������ݿ��������Ϣ, ������ʱʹ�� login ������. 
	// ***** ��ʱ **** ����ʱʹ��. 
	// ʵ��ʹ�����������������ȡ. 

	m_itemSelfInfo.chType       = Type_ImcFriend;
	m_itemSelfInfo.nId          = LocalId_F(101);
	m_itemSelfInfo.tstrLogo     = _T("man.png");
	m_itemSelfInfo.tstrNickName = _T("zhaocg");
	m_itemSelfInfo.tstrDescription = _T("tianyamingyuedao");

	return 0;
}

int     CZiMainFrame::ParserSelfInfo(tstring & tstrSelfInfo)
{
	// �����󱣴汾�����ݿ�. 
	// ...???

	return 0;
}

int     CZiMainFrame::UpdateState()
{
	// ... 
	// ��ʼ״̬ʱδ���غ����б�. 
	// ����״̬ʱ���غ����б�. 

	return 0;
}

void    CZiMainFrame::OnPrepare(TNotifyUI & msg)
{
	// ��ȡ������Ϣ ...
	// ���� **************
	CFirendItemUI * pFriendsList = DuiControl(CFirendItemUI, _T("FriendsList"));
	CGroupItemUI  * pGroupsList  = DuiControl(CGroupItemUI,  _T("GroupsList"));
	Assert(pFriendsList);
	Assert(pGroupsList);
}

void    CZiMainFrame::OnKillFocus(TNotifyUI & msg)
{
	// �� search �� edit �� tip ����ת��. 
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
	// ����֧��������. 
	if(msg.wParam != UIEVENT_BUTTONDOWN) return;

	CTabLayoutUI * pTabs = DuiControl(CTabLayoutUI, _T("Tabs"));
	if(!pTabs) return ;

	CBaseItemListUI * pItemListUi = DuiControl(CBaseItemListUI, GetCurListName(pTabs->GetCurSel()));
	if(pItemListUi && (
		pItemListUi->GetItemIndex(msg.pSender) != -1) && (
		::_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0))
	{
		CNodeList * pNode = (CNodeList *)msg.pSender->GetTag();
		if(pItemListUi->CanExpand(pNode))  // �ܷ�չ��. 
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
		// �˴���������ĺ�����Ϣ, ע��������Ⱥ��˵����һ��. 
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
		// �˴���Ҫ�� chat dialog ���뵽 main frame �� chat ����. ??
		// ע�ⲻ��Ҫ���� chat dialog �Ļ���. 
		pChatDialog->Create(NULL, _T("ChatDialog"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
		pChatDialog->CenterWindow();
		::ShowWindow(*pChatDialog, SW_SHOW);

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
	Assert(pNode);

	return CreateChatDailog(pNode, ppChatDialog);
}

void    CZiMainFrame::UpdateBackground()
{
	// ��������ͼƬ. 
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
		// ����Ϊ��. 
		return ;
	}

	int nTeamSize = pFriendsInfo->nTeamSize;
	NetItemInfo_t * pTeamInfo = pFriendsInfo->pTeamInfo;

	for(int i = 0; i < nTeamSize; i++)
	{
		// 1 ������: pTeamInfo[i]
		AddItem(pTeamInfo + i, (NetItemInfo_t *)0);

		// 2 
		int nItemSize = pTeamInfo[i].nFriendSize;
		NetItemInfo_t * pItemInfo = pTeamInfo[i].pFriendInfo;

		for(int j = 0; j < nItemSize; j++)
		{
			// ���Ӻ���: pItemInfo[j];
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
		// ����Ϊ��. 
		return ;
	}

	int nGroupSize = pGroupsInfo->nGroupSize;
	NetItemInfo_t * pGroupInfo = pGroupsInfo->pGroupInfo;

	for(int i = 0; i < nGroupSize; i++)
	{
		// 1 ������: pTeamIno[i]
		AddItem(pGroupInfo + i, (NetItemInfo_t *)0);

		// 2 
		int nItemSize = pGroupInfo[i].nFriendSize;
		NetItemInfo_t * pItemInfo = pGroupInfo[i].pFriendInfo;

		for(int j = 0; j < nItemSize; j++)
		{
			// ���Ӻ���: pItemInfo[j];
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
		// ����Ϊ��. 
		return ;
	}

	int nGroupSize = pTalksInfo->nTalkSize;
	NetItemInfo_t * pTalkInfo = pTalksInfo->pTalkInfo;

	for(int i = 0; i < nGroupSize; i++)
	{
		// 1 ������: pTeamIno[i]
		AddItem(pTalkInfo + i, (NetItemInfo_t *)0);

		// 2 
		int nItemSize = pTalkInfo[i].nFriendSize;
		NetItemInfo_t * pItemInfo = pTalkInfo[i].pFriendInfo;

		for(int j = 0; j < nItemSize; j++)
		{
			// ���Ӻ���: pItemInfo[j];
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
		// TODO ��ģ�ⷢ�ʹ���Ⱥ������
		
		if (m_pCreateGroupWindow) {
			::SetForegroundWindow(m_pCreateGroupWindow->GetHWND());
			return 0;
		}
		
		m_pCreateGroupWindow = new CCreateGroupWindow(this);
		m_pCreateGroupWindow->Create(NULL, _T("����Ⱥ��"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
		m_pCreateGroupWindow->CenterWindow();
		m_pCreateGroupWindow->ShowWindow(true);		
	}
	else {
		Assert(0);
	}
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

	if (dfi.type == Type_ImcTeam && dfi.strdelName.compare("�ҵĺ���") == 0) {
		//MessageBox(m_hWnd, _T("����ɾ��Ĭ�Ϸ���"), _T("����"), 0);
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

	// �Ҽ�����
	pNode = (CNodeList *)pList->GetItemAt(nSel)->GetTag();
	Assert(pNode);

    //TODO 
    //����������������ˣ��ȷ������֤�ɹ�����������
    //������������
	switch(msg.wParam)
	{
	case Event_OpenItem:   { /* ... ??? */  break; }
	case Event_ModifyItem: { ModifyItem(pNode); break; }
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

	// ���� pSearchCombo �б���ʾ. 
	// ���Դ��� ***
	// AddComboItem(_T("111111"));

	ImcNodeList_t nodeList;
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

	// ���ܹ���ʾ���� list ��������, ��֪��Ϊʲô ?
	// HWND hF = ::GetFocus();
	// ::SendMessage(hF, WM_KILLFOCUS, 0, 0);
	//pSearchCombo->Activate();
	//pSearchCombo->Invalidate();
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
	// ... ???
	//CNotifyWindow::MessageBox(
	//	m_hWnd, _T("����"), _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaa"));

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

	for(int i = 0; i < pSearchCombo->GetCount(); i++)
	{
		CListLabelElementUI * pItem = static_cast<CListLabelElementUI*>(
			pSearchCombo->GetItemAt(i));
		delete pItem;
	}

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
		MessageBox(m_hWnd, _T("��������Լ�"), _T("��ʾ"), 0);
		return false;
	}

	CNodeList * pNode = GetNodeInfo(nId);
	if(pNode && 
		pNode->GetNodeData().chType == Type_ImcFriend)
	{
		MessageBox(m_hWnd, _T("���Ѿ�����˴˺���"), _T("��ʾ"), 0);
		return false;
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
	// 1 ���� Team,  ʱ, parent Ϊ 0. 
	// 2 ���� Group  ʱ, parent Ϊ 0. 
	// 3 ���� Friend ��Ա, parent ����. 
	// 4 ���� Group  ��Ա, ...
	// 
	// Ⱥ��������ݲ����� (CNodeList). ??? 
	// Ⱥ ID ������ m_mapNodeTable ��. 
	// -- ����
	// -- ��ô�޸�ʱ���޸Ķ��, ����һ��, Ⱥ����ܶ��, 
	//    ���޸��� ???
	// -- ɾ��ʱ ??
	//    ɾ��Ⱥ���޸���, ֻ���Ⱥ. 
	//    ɾ����, Ⱥ���Ӧ�ĵ����ݾ���ɾ�� (����Ϊ������) ???

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
		// ����Ⱥ��Ա [4]
		// ���ܺ�����Ч(��ɾ����)�ĺ���
		// ���ڴ������, �������⴦�� ...???
		//if(pNodeInfoParent->GetNodeData().chType == Type_ImcTalk)
		//{
		//	Assert(m_itemSelfInfo.nId == pItemInfo->nId || 
		//		m_mapNodeTable.find(pItemInfo->nId) != m_mapNodeTable.end());
		//}
		//else
		//{
		//	// ����������
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
		// ��Ⱥ��Ա. [1, 2, 3]
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
		// ɾ��Ⱥ��Ա. 
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
	// parent �������, Ҳ���Ǹ��ڵ㲻���ƶ�. 
	// ����Ⱥ��Ա, ���ܵ��� MoveItem, ������� AddItem. 
	Assert(pNodeInfo);
	Assert(pNodeInfoSrcParent && pNodeInfoDesParent);
	Assert(pNodeInfo->GetNodeData().chType == Type_ImcFriend);
	Assert(pNodeInfoSrcParent->GetNodeData().chType == Type_ImcTeam);
	Assert(pNodeInfoSrcParent->GetNodeData().chType == pNodeInfoDesParent->GetNodeData().chType);

	CBaseItemListUI * pItemListUi = GetNodeListUi(pNodeInfoSrcParent->GetNodeData().chType);
	ItemNodeInfo_t    itemInfo    = pNodeInfo->GetNodeData();

	DelItem(pNodeInfo, pNodeInfoSrcParent);
	AddItem(&itemInfo, pNodeInfoDesParent);

	// ���緢�� ... ???
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
	// 2 ���緢�� ... ???
	return 0;
}

CNodeList * CZiMainFrame::GetFriendTeamNode()
{
	// ��ȡ "����" ���б� Node ��Ϣ. 
	// ��ԭ����������û�����
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
		delete (it->second);
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
		// ��
		Assert(it->second);
		if(nId == it->second->GetNodeData().nId)
		{
			m_mapNodeTable.erase(it);
			it = m_mapNodeTable.begin();
			continue;
		}

		// ���Ա
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

void    CZiMainFrame::FlashTray(BOOL bFlashOrNot)
{
	if(bFlashOrNot == m_bFlash) return ; 

	// ... ???

	m_bFlash = bFlashOrNot;
}

void    CZiMainFrame::OnTimer()
{
    OnKeepAlive();
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

void    CZiMainFrame::DelayDispatchNetCmd(int nMsg, void * pNetData)
{
	CGenericLockHandler h(m_mainLock);
	m_listNetData.push_back(std::make_pair(nMsg, pNetData));

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

		if(!m_listNetData.empty())
		{
			bDo     = TRUE;
			netData = m_listNetData.front();
			m_listNetData.pop_front();
		}

		if(m_listNetData.empty() && m_nTimer != 0)
		{
			//::KillTimer(m_hWnd, m_nTimer);
			//m_nTimer = 0;
            m_pTrayWindow->FlashTray(TRUE, FALSE);
		}
	}while(0);

	if(bDo) HandleNetMessage(netData.first, netData.second);
	else    m_pTrayWindow->FlashTray(TRUE, FALSE);
	return bDo;
}

int     CZiMainFrame::HandleNetMessage(int nMsg, void * pNetData)
{
	// δ�������Ⱥ�� ... ???
	Assert(pNetData);
	int  nRet  = 0;
	bool bFree = TRUE;

	switch(nMsg)
	{
	case Msg_ScQueryVerify:
		{
			// ��ʾ��֤�����, �����û�����. 
			// ����Ҫ��ʾ�����ߵ���Ϣ, δʵ�� ... ???
			VerifyScQuery_t * pVerifyQuery = (VerifyScQuery_t*)pNetData;
			char              szText[1024] = {0};
			sprintf_s(szText, sizeof(szText)/sizeof(szText[0]), 
				"'%s'(%d) ����Ϊ����, ��֤��Ϣ: '%s'", 
				pVerifyQuery->szSenderNamex, 
				pVerifyQuery->nSenderId, 
				pVerifyQuery->szVerifyDatax);

			CNotifyWindow::MessageBoxX(m_hWnd, _T("��֤��Ϣ"), 
				CA2T(szText), _T("ͬ��"), _T("��ͬ��"), 
				Msg_InQueryVerify, pVerifyQuery);
			bFree = FALSE;
		}
		break;

	case Msg_ScResponseVerify: 
		{
			// ��ʾ��֤�ظ���, ֪ͨ�û�. 
			// ���ʧ�ܿ��Բ��ظ��Է�. 
			VerifyCcResponse_t * pVerifyResp  = (VerifyCcResponse_t*)pNetData;
			TCHAR                tsText[1024] = {0};
			_stprintf_s(tsText, sizeof(tsText)/sizeof(tsText[0]), 
				_T("'%d' %s����Ϊ����"), 
				pVerifyResp->nSenderId, 
				pVerifyResp->bIsAgree ? _T("ͬ��") : _T("��ͬ��"));

			CNotifyWindow::MessageBoxX(m_hWnd, _T("��֤��Ϣ"), tsText);

			if(pVerifyResp->nRecvType == Type_ImcFriend)
			{
				Assert(pVerifyResp->pSenderNetRInfo && 
					pVerifyResp->pSenderNetRInfo->nItemType == Type_ImcFriend);
				CNodeList * pTeamInfo = GetTeamInfo(Team_DefaultNameT);

				Assert(pTeamInfo);
				if(pTeamInfo) AddItem(pVerifyResp->pSenderNetRInfo, pTeamInfo);
			}
		}
		break;

	case Msg_ScTextChat:
		{
			// û������Ի���, ֪ͨ�û�. 
			// ��ʾ����Ի���. 
			// ����Ϣ��ʾ����Ϣ����. ???
			ChatCcTextData_t * pChatText   = (ChatCcTextData_t*)pNetData;
			CChatDialog      * pChatDialog = 0;

			//Assert ...??
			//if(pChatText->nRecvType == 1)
			//{
			//	// ����
			//	Assert(pChatText->nRecverId == m_itemSelfInfo.nId);
			//}
			//else
			//{
			//	// Ⱥ, ������
			//	// ��֤ nSenderId ���ڴ���. 
			//}

            /*
			CreateChatDailog(pChatText->nSenderId, &pChatDialog);
			if(pChatDialog)  // ������Ч. 
			{
				pChatDialog->OnTextMsgShow(pChatText);
				pChatDialog->ActiveWindow();
			}
            */
            if (pChatText->nRecvType == Type_ImcFriend) { //������Ϣ
                CreateChatDailog(pChatText->nSenderId, &pChatDialog);
                if(pChatDialog)  // ������Ч. 
                {
                    pChatDialog->OnTextMsgShow(pChatText);
                    pChatDialog->ActiveWindow();
                }
            }
            else { //Ⱥ��Ϣ
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
            //TODO ִ��ɾ�����ѵĲ���
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
						sprintf_s(tsText, sizeof(tsText)/sizeof(tsText[0]), "%s �����Ƴ������б�", CA2T((pDelFriend->strSendName.c_str())));
						popMsgBox = true;
					}
				}
				else {
					sprintf_s(tsText, sizeof(tsText)/sizeof(tsText[0]), "ɾ������ %s %s", CA2T((pDelFriend->strdelName.c_str())), (pDelFriend->succ == 0 ? "�ɹ�" : "ʧ��"));
					popMsgBox = true;
				}		
			}
			else if (pDelFriend->type == Type_ImcFriendX) {
			}
			else if (pDelFriend->type == Type_ImcGroup) {
				if (nSendId == m_itemSelfInfo.nId) {
					sprintf_s(tsText, sizeof(tsText)/sizeof(tsText[0]), "ɾ��Ⱥ %s %s", CA2T((pDelFriend->strdelName.c_str())), (pDelFriend->succ == 0 ? "�ɹ�" : "ʧ��"));
					popMsgBox = true;
				}
				else if (pDelFriend->succ == 0) {
				    sprintf_s(tsText, sizeof(tsText)/sizeof(tsText[0]), "%s �����Ƴ�Ⱥ %s", CA2T((pDelFriend->strdelName.c_str())), CA2T((pDelFriend->strdelName.c_str())));
					popMsgBox = true;
				}
			}
			else {
			}
			bFree = TRUE;
        }
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
					// �Ҽ����� -> �����Ҽ��˵�. 
					// ... 
				}
				break;

			case WM_LBUTTONDBLCLK:
				{
					// ���˫�� -> 1 ��Ϣ, 2 ��ʾ. 
					BOOL bNoMsg  = true;
					BOOL bHide   = ::IsWindowVisible(m_hWnd);

					bNoMsg = HandleNetCmd();
					if(bNoMsg)
					{
						break;
					}

					//if(bHide)
					//{
					//	::ShowWindow(m_hWnd, SW_HIDE);
					//}
					//else
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
		// ������֤�ظ���Ϣ���Է�. 
		Assert(lParam);
		OnClickVerifyResponse(wParam, lParam);
		break;

	case Msg_ScResponseUsers:
		{
			SearchScResponse_t * pResponse = (SearchScResponse_t*)lParam;
			if(wParam != 0 && wParam != Error_SearchNoValidUser)
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

	case Msg_ScQueryVerify:
	case Msg_ScResponseVerify:
		Assert(lParam != 0);
		DelayDispatchNetCmd(nMsg, (void*)lParam);
		break;

	case Msg_ScTextChat:
		{
			Assert(lParam != 0);
			CChatDialog * pChatDialog = 0;
            ChatCcTextData_t *pChatText = (ChatCcTextData_t *)lParam;
            if (pChatText->nRecvType == Type_ImcFriend) { //������Ϣ
                ImcSubWindowTable_t::iterator it = m_mapSubWindows.find(pChatText->nSenderId);
                if(it != m_mapSubWindows.end()) pChatDialog = static_cast<CChatDialog*>(it->second);
            }
            else { //Ⱥ��Ϣ
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
				DelayDispatchNetCmd(nMsg, (void*)lParam);
			}
		}
		break;
    case Msg_ScDelFriend:
        {
            Assert(lParam != 0);
			DelFriendItem *pDelFriend = (DelFriendItem *)lParam;
			if (pDelFriend->succ == 0) {				
				int nDelId = IdNetToLocal(pDelFriend->type, pDelFriend->nDelId);
				int nSendId = IdNetToLocal(Type_ImcFriend, pDelFriend->nSendId);
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
					DeleteGroupNode(nDelId);
				}
				else {
				}	
			}
			DelayDispatchNetCmd(nMsg, (void*)lParam);
        }
        break;
	case  Msg_ScEvilReport:
		{
			// ��ʾ��֤�����, �����û�����. 
			// ����Ҫ��ʾ�����ߵ���Ϣ, δʵ�� ... ???
			ReportCsEvilData_t * pReport = (ReportCsEvilData_t*)lParam;
			char              szText[1024] = {0};
			sprintf_s(szText, sizeof(szText)/sizeof(szText[0]), 
				"�ٱ� %s", pReport->succ == 0 ? "�ɹ�" : "ʧ��");

			CNotifyWindow::MessageBoxX(m_hWnd, _T("֪ͨ"), CA2T(szText));
			bFree = TRUE;
		}
		break;
	case Msg_ScCreateGroup:
		handleGreateGroup((GroupInfoData_t *)lParam);
		bFree = TRUE;
		break;
	}

	if(bFree) m_pMainMsger->FreeDataEx(nMsg, (void*)lParam);
	return 0;
}

void CZiMainFrame::handleGreateGroup(GroupInfoData_t *pGroupInfoData) {
	if (pGroupInfoData->succ == 0) {
		//TODO ���Ⱥ
		ItemNodeInfo_t item;
		ItemDataNetToLocal(*pGroupInfoData, item);
		AddItem(&item, 0);

		//��ӳ�Ա
		CNodeList *pNodeParent = GetNodeInfo(item.nId);
		Assert(pNodeParent);
		for (size_t i = 0; i < pGroupInfoData->groupinfo.members.size(); i++) {
			ItemNodeInfo_t group_member;
			ItemDataNetToLocal(pGroupInfoData->groupinfo.members[i], group_member, Type_ImcFriendX);
			AddItem(&group_member, pNodeParent);
		}
	}
	char szText[1024] = {0};
	sprintf_s(szText, sizeof(szText)/sizeof(char), "���� %s %s", pGroupInfoData->groupinfo.name.c_str(),
		(pGroupInfoData->succ == 0 ? "�ɹ�" : "ʧ��"));
	CNotifyWindow::MessageBoxX(m_hWnd, _T("֪ͨ"), CA2T(szText));
}

//report window
void    CZiMainFrame::reportEvil()
{
	//TODO 
	if(m_pReportWindow) 
	{
		::SetForegroundWindow(m_pReportWindow->GetHWND());
		return ;
	}
	m_pReportWindow = new CReportWindow(this);
	if(!m_pReportWindow) return ;
	m_pReportWindow->Create(NULL, _T("ReportWndX"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	m_pReportWindow->CenterWindow();
	m_pReportWindow->ShowWindow(true);
	
	return ;
}

//msgrecord window
void    CZiMainFrame::MsgRecord()
{
	//TODO 
	if(m_pMsgRecordWindow) 
	{
		::SetForegroundWindow(m_pMsgRecordWindow->GetHWND());
		return ;
	}
	m_pMsgRecordWindow = new CMsgRecordWindow(this);
	if(!m_pMsgRecordWindow) return ;
	m_pMsgRecordWindow->Create(NULL, _T("MsgRecordWndX"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	m_pMsgRecordWindow->CenterWindow();
	m_pMsgRecordWindow->ShowWindow(true) ;	
	return ;
}
