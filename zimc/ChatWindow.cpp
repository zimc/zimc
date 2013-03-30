#include "stdafx.h" 
#include <windows.h>
#include <shellapi.h>
#include<stdio.h>
#include <atlstr.h>
#include<fstream>
#include<string>

#include "ChatWindow.h"
#include "ConcreteItemListUI.h"
#include "ZimcHelper.h"
#include "ReportWindow.h"
#include "MainWindow.h"

using namespace std;
const TCHAR* const g_tstrChatCloseButtonName     = _T("CloseBtn");
const TCHAR* const g_tstrChatMinButtonName       = _T("MinBtn");
const TCHAR* const g_tstrChatMaxButtonName       = _T("MaxBtn");
const TCHAR* const g_tstrChatRestoreButtonName   = _T("RestoreBtn");

const TCHAR* const g_tstrChatLogoButtonName      = _T("logo");
const TCHAR* const g_tstrChatNickNameTextName    = _T("nickname");
const TCHAR* const g_tstrChatDescriptionEditName = _T("description");

const TCHAR* const g_tstrChatFontBarName         = _T("FontBar");
const TCHAR* const g_tstrChatFontTypeComboName   = _T("FontType");
const TCHAR* const g_tstrChatFontSizeComboName   = _T("FontSize");

const TCHAR* const g_tstrChatBoldButtonName      = _T("boldbtn");
const TCHAR* const g_tstrChatItalicButtonName    = _T("italicbtn");
const TCHAR* const g_tstrUnderlineButtonName     = _T("underlinebtn");

const TCHAR* const g_tstrChatFontSetButtonName   = _T("FontSetBtn");
const TCHAR* const g_tstrChatViewRichEditName    = _T("ViewRichEdit");
const TCHAR* const g_tstrChatInputRichEditName   = _T("InputRichEdit");
const TCHAR* const g_tstrChatSendButtonlName     = _T("SendBtn");


CChatDialog::CChatDialog(
	int                    nChatType, 
	const ItemNodeInfo_t & myselfInfo, 
	CNodeList            * pFriendsInfo, 
	ChatFont_t           * pChatFont, 
	CZiMainFrame         * pMainWindow
	)
	: m_nChatType(nChatType)
	, m_myselfInfo(myselfInfo)
	, m_friendInfo(pFriendsInfo->GetNodeData())
	, m_pGroupInfo(pFriendsInfo)
	, m_pChatFont(pChatFont)
	, m_nChatFlag(Flag_TextChat)
	, m_pMainWindow(pMainWindow)
{}

CChatDialog::~CChatDialog()
{
}
LRESULT CChatDialog::OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	// 利用 WS_EX_TOOLWINDOW 不能够成功的隐藏任务栏图标, 
	// 有可能会重新出现在任务栏中. 不知道为什么. ???

	CDuiWindowBase::OnCreate(nMsg, wParam, lParam, bHandled);
	SetIcon(128);
	return S_OK;
}

ItemNodeInfo_t * CChatDialog::GetSelfInfo()
{
	return &m_myselfInfo;
}

LPCTSTR CChatDialog::GetWindowClassName() const
{
	return _T("ChatDialog");
}

CControlUI * CChatDialog::CreateControl(LPCTSTR pstrClass)
{
	if(_tcsicmp(pstrClass, _T("ChatGroupList")) == 0)
	{
		return new CGroupFriendItemUI(m_pmUi);
	}

	return NULL;
}

void CChatDialog::OnFinalMessage(HWND hWnd)
{
	Assert(m_pMainWindow);
	m_pMainWindow->CancelWatch(m_friendInfo.nId);

	CDuiWindowBase::OnFinalMessage(hWnd);	
	delete this;
}

tstring CChatDialog::GetSkinFile()
{
	return _T("zichatbox.xml");
}

LRESULT CChatDialog::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL    bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes    = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if(::IsZoomed(m_hWnd) != bZoomed)
	{
		if(!bZoomed)
		{
			CControlUI * pControl = static_cast<CControlUI*>(m_pmUi.FindControl(g_tstrChatMaxButtonName));
			if(pControl) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_pmUi.FindControl(g_tstrChatRestoreButtonName));
			if(pControl) pControl->SetVisible(true);
		}
		else 
		{
			CControlUI * pControl = static_cast<CControlUI*>(m_pmUi.FindControl(g_tstrChatMaxButtonName));
			if(pControl) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_pmUi.FindControl(g_tstrChatRestoreButtonName));
			if(pControl) pControl->SetVisible(false);
		}
	}

	return 0;
}

LRESULT CChatDialog::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if(wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDuiWindowBase::ResponseDefaultKeyEvent(wParam);
}

void    CChatDialog::Notify(TNotifyUI& msg)
{
	if(_tcsicmp(msg.sType, _T("windowinit")) == 0)
	{
		return OnPrepare(msg);
	}
    else if(_tcsicmp(msg.sType, _T("return")) == 0) 
    {
        if(_tcsicmp(msg.pSender->GetName(), g_tstrChatInputRichEditName) == 0) OnSendMsg(msg);
    }
	else if(_tcsicmp(msg.sType, _T("timer")) == 0)
	{
		return OnTimer(msg);
	}
	else if(_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		// ...???
		// 
		//OnSelectTab();                    // 音视频, 群, 文本转换. 
		return OnFontStyleChanged1(msg);    // 字体设置
	}
	else if(_tcsicmp(msg.sType, _T("itemselect")) == 0)
	{
		return OnFontStyleChanged2(msg);
	}
	else if(msg.sType == _T("itemactivate"))
	{
		// 点击 group 的组员信息
		return OnGroupItemClick(msg);
	}
	else if(_tcsicmp(msg.sType, _T("click")) == 0)
	{
		DuiClickButtonMap(g_tstrChatCloseButtonName,   OnExit);
		DuiClickButtonMap(g_tstrChatMinButtonName,     OnMinBtn);
		DuiClickButtonMap(g_tstrChatMaxButtonName,     OnMaxBtn);
		DuiClickButtonMap(g_tstrChatRestoreButtonName, OnRestoreBtn);

		DuiClickButtonMap(g_tstrChatFontSetButtonName, OnFontSetBtn);
		DuiClickButtonMap(g_tstrChatSendButtonlName,   OnSendMsg);
		DuiClickButtonMap(_T("RecordBtn"),             OnMsgRecord);
		DuiClickButtonMap(_T("CrimeReportBtn"),        OnReportEvil);
	}
}

void    CChatDialog::OnPrepare(TNotifyUI & msg)
{
	// 设置 log, name, desc
	CButtonUI * log_button = DuiControl(CButtonUI, g_tstrChatLogoButtonName);
	if(log_button != NULL)
		log_button->SetNormalImage(m_friendInfo.tstrLogo.c_str());

	CControlUI * nick_name = m_pmUi.FindControl(_T("NickName"));
	if(nick_name != NULL)
		nick_name->SetText(m_friendInfo.tstrNickName.c_str());

	CControlUI * description = m_pmUi.FindControl(_T("Description"));
	if(description != NULL)
		description->SetText(m_friendInfo.tstrDescription.c_str());

	// 隐藏字体设置 bar. 
	CContainerUI * pFontbar = DuiControl(CContainerUI, g_tstrChatFontBarName);
	if(pFontbar != NULL)
		pFontbar->SetVisible(!pFontbar->IsVisible());

	// 设置右边的窗口
	UpdateRightChatWindow();
}

void    CChatDialog::OnFontStyleChanged1(TNotifyUI & msg)
{
	if(_tcsicmp(msg.pSender->GetName(), g_tstrChatBoldButtonName) == 0)
	{
		COptionUI * bold_button = static_cast<COptionUI*>(msg.pSender);
		if (bold_button != NULL)
		{
			m_pChatFont->bBold = bold_button->IsSelected();
			FontStyleChanged();
		}
	}
	else if(_tcsicmp(msg.pSender->GetName(), g_tstrChatItalicButtonName) == 0)
	{
		COptionUI * italic_button = static_cast<COptionUI*>(msg.pSender);
		if(italic_button != NULL)
		{
			m_pChatFont->bItalic = italic_button->IsSelected();
			FontStyleChanged();
		}
	}
	else if(_tcsicmp(msg.pSender->GetName(), g_tstrUnderlineButtonName) == 0)
	{
		COptionUI * underline_button = static_cast<COptionUI*>(msg.pSender);
		if (underline_button != NULL)
		{
			m_pChatFont->bUnderline = underline_button->IsSelected();
			FontStyleChanged();
		}
	}
}

void    CChatDialog::OnFontStyleChanged2(TNotifyUI & msg)
{
	if(_tcsicmp(msg.pSender->GetName(), g_tstrChatFontTypeComboName) == 0)
	{
		CComboUI* font_type = static_cast<CComboUI*>(msg.pSender);
		if (font_type != NULL)
		{
			m_pChatFont->tstrFontName = font_type->GetText();
			FontStyleChanged();
		}
	}
	else if(_tcsicmp(msg.pSender->GetName(), g_tstrChatFontSizeComboName) == 0)
	{
		CComboUI* font_size = static_cast<CComboUI*>(msg.pSender);
		if (font_size != NULL)
		{
			m_pChatFont->dwFontSize = _ttoi(font_size->GetText());
			FontStyleChanged();
		}
	}
}

void    CChatDialog::OnTimer(TNotifyUI & msg)
{
	// RichEdit 当出现屏幕切换时 (满屏时), 会触发 Timer. 
	//          当鼠标在上面拖动选择时, 也会触发 Timer. 
	//::OutputDebugStringA("--------timer-----------\n");
}

void    CChatDialog::OnGroupItemClick(TNotifyUI & msg)
{
	// 创建新的文本聊天对话框. 
	CGroupFriendItemUI * pFriendsList = DuiControl(CGroupFriendItemUI, _T("FriendInfoList"));
	Assert(pFriendsList);

	if(pFriendsList && (
		pFriendsList->GetItemIndex(msg.pSender) != -1) && (
		::_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0))
	{
		Assert(m_pMainWindow);

		CNodeList   * pNode = (CNodeList *)msg.pSender->GetTag();

        //TODO 如果是打开自己 则打开个人信息页面
        if (pNode->GetNodeData().nId == m_myselfInfo.nId) {
            CZimcHelper::ErrorMessageBox(m_hWnd, "抱歉，不能给自己发送信息哦！");
            return ;
        }
		m_pMainWindow->CreateChatDailog(pNode);
	}
}

int     CChatDialog::OnExit(TNotifyUI & msg)
{
	/*
	if(m_pReportWindow)
	{
		m_pReportWindow->OnExit(msg);
	}
	*/

	// 将 ViewEdit 中的消息记录添加到消息记录数据库中. 
	Close();
	return 0;
}

int     CChatDialog::OnMinBtn(TNotifyUI & msg)
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	return 0;
}

int     CChatDialog::OnMaxBtn(TNotifyUI & msg)
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	return 0;
}

int     CChatDialog::OnRestoreBtn(TNotifyUI & msg)
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	return 0;
}

int     CChatDialog::OnFontSetBtn(TNotifyUI & msg)
{
	CContainerUI* pFontbar = static_cast<CContainerUI*>(m_pmUi.FindControl(_T("fontbar")));
	if(pFontbar) pFontbar->SetVisible(!pFontbar->IsVisible());
	return 0;
}

int     CChatDialog::OnReportEvil(TNotifyUI & msg)
{
	//TODO 
	m_pMainWindow->reportEvil();
	/*
	if(m_pReportWindow) 
	{
		::SetForegroundWindow(m_pReportWindow->GetHWND());
		return 0;
	}

	m_pReportWindow = new CReportWindow(this);
	if(!m_pReportWindow) return -1;
	m_pReportWindow->Create(NULL, _T("ReportWndX"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	m_pReportWindow->CenterWindow();
	m_pReportWindow->ShowWindow(true);
	*/
	return 0;
}

int     CChatDialog::OnMsgRecord(TNotifyUI & msg)
{
	// ... ???
	m_pMainWindow->MsgRecord();
	return 0;
}


int     CChatDialog::OnSendMsg(TNotifyUI & msg)
{
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(g_tstrChatInputRichEditName));
	if( pRichEdit == NULL ) return 1;
	pRichEdit->SetFocus();
	CDuiString sText = pRichEdit->GetTextRange(0, pRichEdit->GetTextLength());
	if( sText.IsEmpty() )  return 1;

	pRichEdit->SetText(_T(""));

	// 缺少发送失败的消息回显 ???
	ChatCcTextData_t chatData;
	std::string      strData;

	ZI_T2A(sText.GetData(), strData);
//#define _TestMainFrame
#ifndef _TestMainFrame
	int nRet = NetSendMsg(chatData, strData.c_str(), strData.length());
	OnTextMsgShow(&chatData);
#else
	OnTextMsgShow_Test(CA2T(strData.c_str()));
#endif
	return 0;
}

int     CChatDialog::OnTextMsgShow(ChatCcTextData_t * pTextData)
{
	ChatCcTextData_t sMsgData ;	
	sMsgData = *pTextData ;
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(g_tstrChatViewRichEditName));
	if( pRichEdit == NULL ) return 1;

	::OutputDebugStringA("-------------------------------------1\n");
	long lSelBegin = 0, lSelEnd = 0;
	CHARFORMAT2 cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT2));

	CA2T    tsSenderName(pTextData->szSenderName ? pTextData->szSenderName : pTextData->szSenderNamex);
	CA2T    tsTime(pTextData->szTime);
	CA2T    tsText(pTextData->szData);
	tstring tstrTime = _T(":  "); tstrTime += (pTextData->tsTime[0] ? pTextData->tsTime : tsTime); tstrTime += _T("\n");
	LPCTSTR tszSenderName = pTextData->tsSenderName ? pTextData->tsSenderName : tsSenderName;
	LPCTSTR tszTime       = tstrTime.c_str();
	LPCTSTR tszText       = tsText;
	//save msg
	SaveMsgRecord(sMsgData) ;

	// ------------------------------------------------------
	// name + time
	cf.cbSize         = sizeof(cf);
	cf.dwReserved     = 0;
	cf.dwMask         = CFM_COLOR | CFM_LINK | CFM_UNDERLINE | CFM_UNDERLINETYPE;
	cf.dwEffects      = CFE_LINK;
	cf.bUnderlineType = CFU_UNDERLINE;
	cf.crTextColor    = RGB(220, 0, 0);
	lSelEnd =  lSelBegin = pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelEnd, lSelEnd);
	pRichEdit->ReplaceSel(tszSenderName, false);
	lSelEnd =  pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelBegin, lSelEnd);
	pRichEdit->SetSelectionCharFormat(cf);

	lSelEnd =  lSelBegin = pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelEnd, lSelEnd);
	pRichEdit->ReplaceSel(tszTime, false);
	cf.dwMask         = CFM_COLOR;
	cf.crTextColor    = RGB(220, 0, 0);
	cf.dwEffects      = 0;
	lSelEnd =  pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelBegin, lSelEnd);
	pRichEdit->SetSelectionCharFormat(cf);

	// ------------------------------------------------------
	// text.
	PARAFORMAT2 pf;
	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize         = sizeof(pf);
	pf.dwMask         = PFM_STARTINDENT;
	pf.dxStartIndent  = 0;
	pRichEdit->SetParaFormat(pf);

	lSelEnd = lSelBegin = pRichEdit->GetTextLength();
	pRichEdit->SetSel(-1, -1);
	pRichEdit->ReplaceSel(tszText, false);
	pRichEdit->SetSel(-1, -1);
	pRichEdit->ReplaceSel(_T("\n"), false);
	cf.crTextColor    = RGB(0, 0, 0);
	lSelEnd = pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelBegin, lSelEnd);
	pRichEdit->SetSelectionCharFormat(cf);

	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize         = sizeof(pf);
	pf.dwMask         = PFM_STARTINDENT;
	pf.dxStartIndent  = 220;
	pRichEdit->SetParaFormat(pf);

	pRichEdit->EndDown();

	::OutputDebugStringA("-------------------------------------8\n");
	return 0;
}

int     CChatDialog::OnTextMsgShow_Test(TCHAR * tszText)
{
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(g_tstrChatViewRichEditName));
	if( pRichEdit == NULL ) return 1;

	tstring   tstrTime = _T(":  ") + 
		CZimcHelper::GetCurrentTimeString() + _T("\n");

	long lSelBegin = 0, lSelEnd = 0;
	CHARFORMAT2 cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT2));

	// ------------------------------------------------------
	cf.cbSize         = sizeof(cf);
	cf.dwReserved     = 0;
	cf.dwMask         = CFM_COLOR | CFM_LINK | CFM_UNDERLINE | CFM_UNDERLINETYPE;
	cf.dwEffects      = CFE_LINK;
	cf.bUnderlineType = CFU_UNDERLINE;
	cf.crTextColor    = RGB(220, 0, 0);
	lSelEnd = lSelBegin = pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelEnd, lSelEnd);
	pRichEdit->ReplaceSel(m_myselfInfo.tstrNickName.c_str(), false);
	lSelEnd = pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelBegin, lSelEnd);
	pRichEdit->SetSelectionCharFormat(cf);

	lSelEnd = lSelBegin = pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelEnd, lSelEnd);
	pRichEdit->ReplaceSel(tstrTime.c_str(), false);
	cf.dwMask         = CFM_COLOR;
	cf.crTextColor    = RGB(0, 0, 0);
	cf.dwEffects      = 0;
	lSelEnd = pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelBegin, lSelEnd);
	pRichEdit->SetSelectionCharFormat(cf);

	// ------------------------------------------------------
	PARAFORMAT2 pf;
	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize         = sizeof(pf);
	pf.dwMask         = PFM_STARTINDENT;
	pf.dxStartIndent  = 0;
	pRichEdit->SetParaFormat(pf);

	lSelEnd = lSelBegin = pRichEdit->GetTextLength();
	pRichEdit->SetSel(-1, -1);
	pRichEdit->ReplaceSel(tszText, false);
	pRichEdit->SetSel(-1, -1);
	pRichEdit->ReplaceSel(_T("\n"), false);
	cf.crTextColor    = RGB(0, 0, 0);
	lSelEnd = pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelBegin, lSelEnd);
	pRichEdit->SetSelectionCharFormat(cf);

	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize         = sizeof(pf);
	pf.dwMask         = PFM_STARTINDENT;
	pf.dxStartIndent  = 220;
	pRichEdit->SetParaFormat(pf);

	pRichEdit->EndDown();

	return 0;
}

int     CChatDialog::UpdateRightChatWindow()
{
	// TextVideoChatTitle1, GroupChatTitle1. 
	// TextChatDesc, GroupChatDesc, AvChatDesc
	CContainerUI * pTextChatTitle  = DuiControl(CContainerUI, _T("TextVideoChatTitle1"));
	CContainerUI * pGroupChatTitle = DuiControl(CContainerUI, _T("GroupChatTitle1"));
	Assert(pTextChatTitle && pGroupChatTitle);

	CContainerUI * pTextChat       = DuiControl(CContainerUI, _T("TextChatDesc"));
	CContainerUI * pAvChat         = DuiControl(CContainerUI, _T("AvChatDesc"));
	CContainerUI * pGroupChat      = DuiControl(CContainerUI, _T("GroupChatDesc"));
	Assert(pTextChat && pAvChat && pGroupChat);

	if(m_nChatType == Type_ImcGroup || m_nChatType == Type_ImcTalk) 
	{
		m_nChatFlag = Flag_GroupChat;
	}

	switch(m_nChatFlag)
	{
	case Flag_TextChat:
		pTextChatTitle->SetVisible(true); pGroupChatTitle->SetVisible(false);
		pTextChat->SetVisible(true); pAvChat->SetVisible(false); pGroupChat->SetVisible(false);
		::SetWindowPos(m_hWnd, 0, 0, 0, 464, 486, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		UpdateTextChatWindow(pTextChat);
		break;

	case Flag_GroupChat:
		pTextChatTitle->SetVisible(false); pGroupChatTitle->SetVisible(true);
		pTextChat->SetVisible(false); pAvChat->SetVisible(false); pGroupChat->SetVisible(true);
		::SetWindowPos(m_hWnd, 0, 0, 0, 464, 486, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		UpdateGroupChatWindow(pGroupChat);
		break;

	case Flag_AvChat:
		pTextChatTitle->SetVisible(true); pGroupChatTitle->SetVisible(false);
		pTextChat->SetVisible(false); pAvChat->SetVisible(true); pGroupChat->SetVisible(true);
		::SetWindowPos(m_hWnd, 0, 0, 0, 559, 486, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		UpdateAvChatWindow(pAvChat);
		break;
	}

	return 0;
}

int     CChatDialog::UpdateTextChatWindow(CContainerUI * pChatUi)
{
	// 它在 OnPrepare 之前. 
	// 如果没有图片怎么办? 是否有缺省图片. 暂时如果没有图片, 则不管了. 
	#define DuiControl_Set(_ctrl, _name, _text, _func)	\
		DuiControl(C##_ctrl##UI, _T(#_name))->_func(_text);
	#define DuiControl_SetX(_ctrl, _who, _item, _func) \
		DuiControl_Set(_ctrl, _who##_item, _who.tstr##_item.c_str(), _func)

	#define DuiControl_SetBkImage(_ctrl, _who, _item)	\
		DuiControl_SetX(_ctrl, _who, _item, SetBkImage)
	#define DuiControl_SetText(_ctrl, _who, _item)	\
		DuiControl_SetX(_ctrl, _who, _item, SetText)
	#define DuiControl_AddText(_ctrl, _who, _item)	\
		DuiControl_SetX(_ctrl, _who, _item, AddText)

	#define V1(_F, _P, _W)	\
		_F(_P, _W, Picture)

	#define V2(_F, _L, _W)	\
		_F(_L, _W, RealName) \
		_F(_L, _W, Sex) \
		_F(_L, _W, Height) \
		_F(_L, _W, NativePlace) \
		_F(_L, _W, Birthday) \
		_F(_L, _W, Position)

	//_F(_E, _W, Studies)
	#define V3(_F, _E, _W)	\
		_F(_E, _W, Experience) \
		_F(_E, _W, Evaluation)


	//ItemNodeInfo_t & Myself = m_myselfInfo;
	ItemNodeInfo_t & Friend = m_friendInfo;

	//V1(DuiControl_SetBkImage, VerticalLayout, Myself);
	V1(DuiControl_SetBkImage, VerticalLayout, Friend);
	//V2(DuiControl_AddText,    Label,          Myself);
	V2(DuiControl_AddText,    Label,          Friend);
	//V3(DuiControl_SetText,    RichEdit,       Myself);
	V3(DuiControl_SetText,    RichEdit,       Friend);

	return 0;
}

int     CChatDialog::UpdateAvChatWindow(CContainerUI * pChatUi)
{
	// ... ???
	return 0;
}

int     CChatDialog::UpdateGroupChatWindow(CContainerUI * pChatUi)
{
	#define V8(_F, _L, _W)	\
		_F(_L, _W, Notify)

	ItemNodeInfo_t & Group = m_myselfInfo;
	V1(DuiControl_SetBkImage, Control,       Group);
	V8(DuiControl_SetText,    RichEdit,      Group);

	// 设置群好友列表, 包括自己 ??? 是的
	// 位置可能需要调整 ...???
	AddGroupFriendsList();
	return 0;
}

void    CChatDialog::FontStyleChanged()
{
	// 修改字体. 
	CRichEditUI * pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(g_tstrChatInputRichEditName));
	if(pRichEdit)
	{
		pRichEdit->SetFont(
			m_pChatFont->tstrFontName.c_str(), m_pChatFont->dwFontSize, m_pChatFont->bBold, m_pChatFont->bUnderline, m_pChatFont->bItalic);
		pRichEdit->SetTextColor(m_pChatFont->dwTextColor);
	}
}

void    CChatDialog::AddGroupFriendsList()
{
	Assert((m_nChatType == Type_ImcGroup || 
		m_nChatType == Type_ImcTalk ) && 
		m_pGroupInfo && 
		m_pGroupInfo->GetChildrenSize() > 0);

	CGroupFriendItemUI * pFriendsList = DuiControl(CGroupFriendItemUI, _T("FriendInfoList"));
	Assert(pFriendsList);

	pFriendsList->RemoveAll();
	for(int i = 0; i < m_pGroupInfo->GetChildrenSize(); i++)
	{
		CNodeList * pNode = m_pGroupInfo->GetChild(i);
		Assert(pNode);
		pFriendsList->AddNode(pNode->GetNodeData());
	}
}

int     CChatDialog::NetSendMsg(ChatCcTextData_t & textData, const char * pchData, int nDataLen)
{
	// 网络发送数据. 
	// ??? 如果服务器不负责群分发, 此处需要特殊处理群的消息发送. 
	// 
	Assert(pchData);
	Assert(m_pMainWindow);

	textData.tsSenderName = &m_myselfInfo.tstrNickName[0];
	::_tcscpy_s(textData.tsTime, sizeof(textData.tsTime)/sizeof(textData.tsTime[0]), 
		CZimcHelper::GetCurrentTimeString().c_str());

	CT2A szSenderName(m_myselfInfo.tstrNickName.c_str());
	CT2A szRecverName(m_friendInfo.tstrNickName.c_str());
	CT2A szTime(textData.tsTime);

	textData.nDataLen     = nDataLen;
	textData.szData       = (char*)pchData;
	textData.nRecvType    = TypeLocalToNet(m_nChatType);

	textData.nSenderId    = MagicId_F(Msg_CsTextChat, m_myselfInfo.nId);
	textData.nRecverId    = MagicId_t(Msg_CsTextChat, m_nChatType, m_friendInfo.nId);
	textData.szSenderName = szSenderName;
	textData.szRecverName = szRecverName;
	textData.szTime       = szTime;

	m_pMainWindow->SendImMessageX(Msg_CsTextChat, (LPARAM)&textData, sizeof(textData));
	return 0 ;
}

int  CChatDialog::SaveMsgRecord(ChatCcTextData_t & Msg )
{
	// 将 ViewEdit 中的消息记录添加到消息记录数据库中. 
	// ...???
	 //存到聊天记录
     //创建文件对象：
	fstream fs ;
	string msgBody,s_filename ; 
    char cfilename[200] ;
	if( Msg.nRecvType == Type_ImcFriend )
	{
		//发消息者是本人，以收消息ID命名
		if( Msg.nRecverId.nId == m_myselfInfo.nId )
		{
			sprintf(cfilename, "%d", Msg.nRecverId.nId);
            //filename=string(Msg.nRecverId.nId);//
		}
		else
		{
			sprintf(cfilename, "%d", Msg.nSenderId.nId);
            //filename=Msg.nSenderId.nId ;
		}
        s_filename=cfilename;
	}
	else
	{
		//filename = "group" ;
	}
	//s_filename = "data/"+s_filename +".txt" ; 
    /*
    char test[1024]={0};
    strcat(test,"data/");
    strcat(test, filename);
    strcat(test,".txt");
    */
    //string file;
    //stringstream idstr;
    //idstr<<id;


    string file = "data/" + s_filename + ".txt";
    //strcat("data/",cfilename) ;
    //strcat(cfilename,".txt") ;
    //cfilename = static_cast<>(s_filename) ;
	fs.open(cfilename,fstream::in|fstream::app) ;
	msgBody = string(Msg.szData) ; 
	int pos;
	pos = msgBody.find('\n');
	while (pos != -1)
	{
		 msgBody.replace(pos,1,"\r\r") ; //用新的串替换掉指定的串
		 pos = msgBody.find('\n');
	}
	fs<<CT2A(Msg.tsSenderName)<<'\t'<<CT2A(Msg.tsTime)<<'\t'<<msgBody<<'\n';
	fs.close() ;
	return 1;
}

void    CChatDialog::ActiveWindow()
{
	HWND hWnd = ::GetForegroundWindow();
	if(hWnd == m_hWnd)
	{
		return ;
	}

	::SetActiveWindow(m_hWnd);
}
