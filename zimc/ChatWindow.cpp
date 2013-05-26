#include "stdafx.h" 
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <atlstr.h>
#include <fstream>
#include <string>
#include <direct.h>
#include <WinUser.h>
#include <fstream>
#include  <io.h>

#include "ChatWindow.h"
#include "ConcreteItemListUI.h"
#include "ZimcHelper.h"
#include "ReportWindow.h"
#include "MainWindow.h"
#include "MsgRecordWindow.h"
#include "NotifyWindow.h"
//added by tian
#include "FileDialogEx.h"
#include "IImageOle.h"
#include "RichEditUtil.h"
#include "common/utils2.h"
#include "common/curl/curl.h"
#include "common/HTTPRequest.h"
#include "common/utils2.h"

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
//added by tian
const TCHAR* const g_tstrFontColor				= _T("Colorbtn");

const TCHAR* const g_tstrChatFontSetButtonName   = _T("FontSetBtn");
const TCHAR* const g_tstrChatViewRichEditName    = _T("ViewRichEdit");
const TCHAR* const g_tstrChatInputRichEditName   = _T("InputRichEdit");
const TCHAR* const g_tstrChatSendButtonlName     = _T("SendBtn");

#define TEXT_MAX_LENGTH 500


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
	, m_pMsgRecordWindow(0)
	, m_nTimer(0)
	, m_inputEditKillFocus(false)
{}

CChatDialog::~CChatDialog()
{
}

LRESULT CChatDialog::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT ret = S_OK;
	switch(uMsg) {
		case WM_TIMER:
			if(wParam == 1){
				TNotifyUI  msg;
				OnTimer(msg);
			}
			break;
	}
	return ret;
}

LRESULT CChatDialog::OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	// 利用 WS_EX_TOOLWINDOW 不能够成功的隐藏任务栏图标, 
	// 有可能会重新出现在任务栏中. 不知道为什么. ???

	CDuiWindowBase::OnCreate(nMsg, wParam, lParam, bHandled);
	SetIcon(128);

	if (m_nTimer == 0) {
		m_nTimer = ::SetTimer(m_hWnd, 1, 2000, 0);
	}

	return S_OK;
}

ItemNodeInfo_t * CChatDialog::GetSelfInfo()
{
	return &m_myselfInfo;
}

ItemNodeInfo_t *CChatDialog::GetFriendInfo() {
	return &m_friendInfo;
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
		//return OnTimer(msg);
	}
	else if (_tcsicmp(msg.sType, _T("setfocus")) == 0) {
		if (msg.pSender->GetName() == g_tstrChatInputRichEditName) {
			m_inputEditKillFocus = false;
		}
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
		//added by tian
		DuiClickButtonMap(g_tstrChatBoldButtonName,   OnFontBold);
		DuiClickButtonMap(g_tstrChatItalicButtonName,   OnFoutItalic);
		DuiClickButtonMap(g_tstrUnderlineButtonName,   OnFontUnderLine);
		DuiClickButtonMap(g_tstrFontColor,   OnFontColor);
		DuiClickButtonMap(_T("GroupTab1"),  OnCreateGroup);
	}
}

void CChatDialog::OnCreateGroup(TNotifyUI &msg) {
	m_inputEditKillFocus = true;
	TNotifyUI msgui = msg;
	msgui.wParam = Event_CreateGroup;
	m_pMainWindow->OnCreateGroup(msgui);
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
			_tcscpy_s(m_pChatFont->tszFontName, font_type->GetText());
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
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(g_tstrChatInputRichEditName));
	if (pRichEdit && !m_inputEditKillFocus) {
		pRichEdit->SetFocus();
	}

	CTabLayoutUI *pPicture = DuiControl(CTabLayoutUI, _T("FriendPicture"));
	if (pPicture) {
		check_file_path(m_strPicFile);
		pPicture->SetBkImage(CA2T(m_strPicFile.c_str()));
	}
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
//added by tian
void	CChatDialog::OnFontBold			(TNotifyUI & msg)
{
	COptionUI* m_pBoldBtn = static_cast<COptionUI*>(m_pmUi.FindControl(_T("boldbtn")));
	m_pChatFont->bBold = !m_pChatFont->bBold;
	
}
void	CChatDialog::OnFoutItalic		(TNotifyUI & msg)
{
	CContainerUI* pFontbar = static_cast<CContainerUI*>(m_pmUi.FindControl(_T("italicbtn")));
	m_pChatFont->bItalic = !m_pChatFont->bItalic;
}
void	CChatDialog::OnFontUnderLine		(TNotifyUI & msg)
{
	CContainerUI* pFontbar = static_cast<CContainerUI*>(m_pmUi.FindControl(_T("underlinebtn")));
	m_pChatFont->bUnderline = !m_pChatFont->bUnderline;
}

void	CChatDialog::OnFontType			(TNotifyUI & msg)
{

}
void	CChatDialog::OnFontSize			(TNotifyUI & msg)\
{
	CContainerUI* pFontbar = static_cast<CContainerUI*>(m_pmUi.FindControl(_T("underlinebtn")));
	m_pChatFont->bUnderline = !m_pChatFont->bUnderline;
}

void	CChatDialog::OnFontColor		(TNotifyUI & msg)\
{
	static COLORREF rgbCustomColors[16] =
	{
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), RGB(255, 255, 255), 
	};

	CHOOSECOLOR cc = {0};
	cc.lStructSize = sizeof(cc);
	cc.lpCustColors = rgbCustomColors;
	cc.hwndOwner = m_hWnd;
	cc.Flags = CC_RGBINIT;

	BOOL bRet = ::ChooseColor(&cc);
	if (bRet)
	{
		COLORREF clrText = cc.rgbResult;
		if (m_pChatFont->dwTextColor != clrText)
		{
			m_pChatFont->dwTextColor = clrText;
			m_pSendEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(_T("InputRichEdit")));
			//m_pSendEdit->SetTextColor(m_pChatFont->dwTextColor);//不晓得为什么不好使，只好用下面笨重的方法来做了。
			ITextServices * pTextServices = m_pSendEdit->GetTextServices();
			RichEdit_SetDefFont(pTextServices, m_pChatFont->tszFontName,
			m_pChatFont->dwFontSize, m_pChatFont->dwTextColor, m_pChatFont->bBold,
			m_pChatFont->bBold, m_pChatFont->bUnderline, FALSE);
			pTextServices->Release();
			//g_buddyFontInfo.m_clrText = m_fontInfo.m_clrText;
		}
	}
}

int     CChatDialog::OnReportEvil(TNotifyUI & msg)
{
	//TODO 
	m_pMainWindow->reportEvil(m_friendInfo.tstrNickName);
	
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
	if (sText.GetLength() >= TEXT_MAX_LENGTH) {
		CNotifyWindow::MessageBoxX(m_hWnd, _T("提示"), _T("抱歉，您一次发送消息过长，请分成多次发送"));
		return 0;
	}

	// 缺少发送失败的消息回显 ???
	ChatCcTextData_t chatData;
	std::string      strData;
	strData.resize(sizeof(ChatFont_t));
	memcpy(&strData[0], (void*)m_pChatFont, sizeof(ChatFont_t));

	ZI_T2A(sText.GetData(), strData);
//#define _TestMainFrame
#ifndef _TestMainFrame
	int nRet = NetSendMsg(chatData, strData.c_str(), strData.length());
	CT2A szTime(chatData.tsTime);
	chatData.szTime = szTime;
	OnTextMsgShow(&chatData);
#else
	OnTextMsgShow_Test(CA2T(strData.c_str()));
#endif
	return 0;
}

int     CChatDialog::OnTextMsgShow(ChatCcTextData_t * pTextData)
{
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(g_tstrChatViewRichEditName));
	Assert(pRichEdit);
    	
	//save msg
	//SaveMsgRecord(*pTextData);
	recordMsg(pTextData);
    
    //设置字体 TODO 
    ChatFont_t cht;
    memset(&cht, 0, sizeof(cht));
    //cht = *m_pChatFont;
	memcpy(&cht, pTextData->szData, sizeof(ChatFont_t));
	int textOffset = sizeof(ChatFont_t);
    
	::OutputDebugStringA("-------------------------------------1\n");
	long lSelBegin = 0, lSelEnd = 0;
	CHARFORMAT2 cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT2));

	CA2T    tsSenderName(pTextData->szSenderName ? pTextData->szSenderName : pTextData->szSenderNamex);
	CA2T    tsTime(pTextData->szTime);
	CA2T    tsText(pTextData->szData + textOffset);
	tstring tstrTime = _T(":  "); tstrTime += (pTextData->tsTime[0] ? pTextData->tsTime : tsTime); tstrTime += _T("\n");
	LPCTSTR tszSenderName = pTextData->tsSenderName ? pTextData->tsSenderName : tsSenderName;
	LPCTSTR tszTime       = tstrTime.c_str();
	LPCTSTR tszText       = tsText;

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
	
    //设置聊天内容字体样式
    //LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic
    //TODO 大小 未解决 ??? 
    cf.dwMask = CFM_COLOR| CFM_FACE | CFM_SIZE;
    cf.yHeight = cht.dwFontSize;
    cf.crTextColor = cht.dwTextColor;
	cf.yHeight = cht.dwFontSize*cht.dwFontSize;
	_tcscpy_s(cf.szFaceName, cht.tszFontName);
    if (cht.bBold) {
        cf.dwMask |= CFM_BOLD;
        cf.dwEffects |= CFE_BOLD;
    }
    if (cht.bUnderline) {
        cf.dwMask |= CFM_UNDERLINE;
        cf.dwEffects |= CFE_UNDERLINE;
    }
    if (cht.bItalic) {
        cf.dwMask |= CFM_ITALIC;
        cf.dwEffects |= CFE_ITALIC;
    }
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

void   CChatDialog::recordMsg(ChatCcTextData_t *pChatData) {
	//string &strSender, string &strTime, int nSenderId, int nRecverId, const char *szData, int nDataLen
	char filename[256] = {0};
	char filepath[256] = {0};

	string tmp_file;
	CZimcHelper::WideToMulti2(GetPaintManagerUI()->GetInstancePath().GetData(), tmp_file);
	sprintf(filepath, "%s\\%d", TMP_DATA_PREFIX, m_myselfInfo.nId);
	sprintf(filename, "%s\\%s%d.%d", filepath, 
		(m_friendInfo.chType == Type_ImcGroup ? "group" : "friend"), 
		m_friendInfo.nId, getFileSuffixNum(time(NULL)));
	tmp_file.append("\\");
	tmp_file.append(filename);
	createDirectory(tmp_file.c_str());

	FILE *fp = fopen(tmp_file.c_str(), "ab+");
	if (fp) {
		//TODO 错误处理
		char *start_token = "textbegin@";
		char *end_token = "@textend\r\n";
		//start token
		fwrite(start_token, strlen(start_token)*sizeof(char), 1, fp);
		int s = sizeof(int) + strlen(pChatData->szTime)*sizeof(char) + sizeof(int) + strlen(pChatData->szSenderName)*sizeof(char) + sizeof(int)+ pChatData->nDataLen*sizeof(char);
		fwrite(&s, sizeof(s), 1, fp);
		//szTime
		s = strlen(pChatData->szTime)*sizeof(char);
		fwrite(&s, sizeof(s), 1, fp);
		fwrite(pChatData->szTime, s, 1, fp);
		//szSender
		s = strlen(pChatData->szSenderName)*sizeof(char);
		fwrite(&s, sizeof(s), 1, fp);
		fwrite(pChatData->szSenderName, s, 1, fp);
		//szData
		fwrite(&(pChatData->nDataLen), sizeof(pChatData->nDataLen), 1, fp);
		fwrite(pChatData->szData, (pChatData->nDataLen)*sizeof(char), 1, fp);
		//end token
		fwrite(end_token, strlen(end_token)*sizeof(char), 1, fp);
		fclose(fp);
	}
}

int     CChatDialog::OnMsgRecord(TNotifyUI & msg)
{
	if(m_pMsgRecordWindow) 
	{
		m_pMsgRecordWindow->PostMessage(WM_CLOSE, (WPARAM)0, 0L);
		return 0;
	}
	m_pMsgRecordWindow = new CMsgRecordWindow(this, getFileSuffixNum(time(NULL)));

	if(!m_pMsgRecordWindow) return 0;
	m_pMsgRecordWindow->Create(m_hWnd, _T("聊天记录"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	//控制显示位置
	m_pMsgRecordWindow->CenterWindow();
	m_pMsgRecordWindow->ShowWindow(true) ;
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
		_F(_L, _W, Position)
		/*_F(_L, _W, Height) \
		_F(_L, _W, NativePlace) \
		_F(_L, _W, Birthday) \*/
		

	//_F(_E, _W, Studies)
	#define V3(_F, _E, _W)	\
		_F(_E, _W, Evaluation)
		//_F(_E, _W, Experience) 
		


	//ItemNodeInfo_t & Myself = m_myselfInfo;
	ItemNodeInfo_t & Friend = m_friendInfo;

	//V1(DuiControl_SetBkImage, VerticalLayout, Myself);
	//V1(DuiControl_SetBkImage, VerticalLayout, Friend);
	//V2(DuiControl_AddText,    Label,          Myself);
	V2(DuiControl_AddText,    Label,          Friend);
	//V3(DuiControl_SetText,    RichEdit,       Myself);
	V3(DuiControl_SetText,    RichEdit,       Friend);

	// TODO 测试代码
	char picfile[512] = {0};
	string uripath = "style/default/images/head_default.jpg";
	if (m_friendInfo.tstrPicture.size() > 0) {
		uripath = CT2A(m_friendInfo.tstrPicture.c_str());
	}
	sprintf(picfile, "%s\\%s", TMP_DATA_DIR, uripath.c_str());
	m_strPicFile = picfile;

	string tmp_file;
	CZimcHelper::WideToMulti2(GetPaintManagerUI()->GetInstancePath().GetData(), tmp_file);
	tmp_file.append("\\");
	tmp_file.append(m_strPicFile);
	string pic_url = "www.arebank.com/";
	pic_url.append(uripath);
	check_file_path(tmp_file);

	if( (_access(tmp_file.c_str(), 0 )) != -1 ) {
		CTabLayoutUI *pPicture = DuiControl(CTabLayoutUI, _T("FriendPicture"));
		if (pPicture) {
			pPicture->SetBkImage(CA2T(m_strPicFile.c_str()));
		}
	}
	else {
		createDirectory(tmp_file.c_str());
		tmp_file.append(".bak");
		HTTPRequest::instance()->send_request(EVENT_DOWNLOAD_FILE, pic_url.c_str(), tmp_file.c_str(),  httpCompelte, NULL);
	}

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
			m_pChatFont->tszFontName, m_pChatFont->dwFontSize, m_pChatFont->bBold, m_pChatFont->bUnderline, m_pChatFont->bItalic);
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
	strcpy(textData.szSenderNamex, szSenderName);

	textData.nDataLen     = nDataLen;
	textData.szData       = (char*)pchData;
	textData.nRecvType    = m_nChatType;

	//textData.nSenderId    = MagicId_F(Msg_CsTextChat, m_myselfInfo.nId);
	textData.nSenderId    = MagicId_t(m_myselfInfo.nId);
	//textData.nRecverId    = MagicId_t(Msg_CsTextChat, m_nChatType, m_friendInfo.nId);
	textData.nRecverId    = MagicId_t(m_friendInfo.nId);
	textData.szSenderName = textData.szSenderNamex;
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
    string strSenderName,strSenderTime;
    char cfilename[200] ;
	if( Msg.nRecvType == Type_ImcFriend || Msg.nRecvType == Type_ImcFriendX)
	{
		//发消息者是本人，以收消息ID命名
		if( IdNetToLocal(Type_ImcFriend,Msg.nRecverId.nId) == m_myselfInfo.nId )
		{
			sprintf(cfilename, "friend%d", Msg.nSenderId.nId) ;
            //filename=string(Msg.nRecverId.nId);//
		}
		else
		{
			sprintf(cfilename, "friend%d", Msg.nRecverId.nId) ;
            //filename=Msg.nSenderId.nId ;
		}
        s_filename=cfilename ;
	}
	else
	{
		//群组id+本地发送者id+group 作为文件名
        //filename = "group" ;
        //s_filename = "group" ;
        sprintf(cfilename, "group%d", Msg.nRecverId.nId) ;
	}
    if( IdNetToLocal(Type_ImcFriend,Msg.nSenderId.nId) == m_myselfInfo.nId ) {
        strSenderName = CT2A(Msg.tsSenderName);
        strSenderTime = CT2A(Msg.tsTime);
    }
    else {
        strSenderName = Msg.szSenderNamex;
        strSenderTime = Msg.szTime ;
    }
	s_filename = "data/"+s_filename +".txt" ;
    fs.open(s_filename.c_str(),fstream::in|fstream::app) ;
	msgBody = string(Msg.szData, Msg.nDataLen); 
	int pos;
	pos = msgBody.find('\n');
	while (pos != -1)
	{
		 msgBody.replace(pos,1,"\r\r") ; //用新的串替换掉指定的串
		 pos = msgBody.find(pos, '\n');
	}
    fs<<strSenderName.c_str()<<'\t'<<strSenderTime<<'\t'<<msgBody<<'\n';
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
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(g_tstrChatViewRichEditName));
	if (pRichEdit) {
		pRichEdit->SetFocus();
		pRichEdit->HomeUp();
	}

	CRichEditUI* pRichEditSend = static_cast<CRichEditUI*>(m_pmUi.FindControl(g_tstrChatInputRichEditName));
	if( pRichEditSend)  {
		pRichEditSend->SetFocus();
	}
}
