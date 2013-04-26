#include "stdafx.h"
#include "Setting.h"


CSettingWindow::CSettingWindow(CZiMainFrame* pMainWindow)
: m_pMainWindow(pMainWindow)
{}

CSettingWindow::~CSettingWindow()
{}


LPCTSTR CSettingWindow::GetWindowClassName() const
{
    return _T("SettingWnd");
}

tstring CSettingWindow::GetSkinFile()
{
    return _T("ziconfig.xml");
}

HRESULT    CSettingWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	
	HRESULT ret = CDuiWindowBase::OnCreate(uMsg, wParam, lParam, bHandled);

	Assert(m_pMainWindow);
	CCheckBoxUI *pCheck1 = DuiControl(CCheckBoxUI, _T("addfriend1"));
	CCheckBoxUI *pCheck2 = DuiControl(CCheckBoxUI, _T("addfriend2"));
	Assert(pCheck1 && pCheck2);
	m_nSetType = m_pMainWindow->GetSelfInfo()->nAddFriendType;
	if (m_nSetType == 0) {
		pCheck1->SetCheck(true);
	}
	else {
		pCheck2->SetCheck(true);
	}
	return ret;
}

void    CSettingWindow::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("windowinit")) {
	}
    else if(msg.sType == _T("click"))
    {
        DuiClickButtonMap(_T("CloseBtn"),   OnExit);
        DuiClickButtonMap(_T("BtnCancel"),  OnExit);
        DuiClickButtonMap(_T("BtnOk"),  OnSetting);
    }
	else if (msg.sType == _T("selectchanged")) {
		DuiClickButtonMap(_T("addfriend1"), OnSetAddFriend);
		DuiClickButtonMap(_T("addfriend2"), OnSetAddFriend);
	}
}

void    CSettingWindow::OnFinalMessage(HWND hWnd)
{
    m_pMainWindow->m_pSettingWindow = 0;
    CDuiWindowBase::OnFinalMessage(hWnd);
    delete this;
}

int    CSettingWindow::OnExit(TNotifyUI & msg)
{
    Close();
    return 0;
}

int    CSettingWindow::OnSetting(TNotifyUI & msg)
{
	if (m_pMainWindow->GetSelfInfo()->nAddFriendType == m_nSetType) {
		return 0;
	}
	m_pMainWindow->GetSelfInfo()->nAddFriendType = m_nSetType;
	SetInfo_t setinfo;
	setinfo.nId = IdLocalToNet(Type_ImcFriend, m_pMainWindow->GetSelfInfo()->nId);
	setinfo.nInvited = m_nSetType;
	setinfo.type = SET_INFO_INVITE;
	m_pMainWindow->SendImMessageX(Msg_CsSetInfo, (LPARAM)&setinfo, sizeof(setinfo));
	OnExit(msg);
    return 0;
}

int    CSettingWindow::OnSetAddFriend(TNotifyUI & msg) {
	if (msg.pSender->GetName() == _T("addfriend2")) {
		m_nSetType = 1;//验证后才能加入
	}
	else if (msg.pSender->GetName() == _T("addfriend1")) {
		m_nSetType = 0;
	}
	return 0;
}
