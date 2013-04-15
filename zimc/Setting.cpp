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

void    CSettingWindow::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("windowinit")) {
		Assert(m_pMainWindow);
		CCheckBoxUI *pCheck1 = DuiControl(CCheckBoxUI, _T("addfriend1"));
		CCheckBoxUI *pCheck2 = DuiControl(CCheckBoxUI, _T("addfriend2"));
		Assert(pCheck1 && pCheck2);
		
		if (m_pMainWindow->GetSelfInfo()->nAddFriendType == 0) {
			pCheck1->SetCheck(true);
		}
		else {
			pCheck2->SetCheck(true);
		}
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
    OnExit(msg);
    return 0;
}

int    CSettingWindow::OnSetAddFriend(TNotifyUI & msg) {
	if (msg.pSender->GetName() == _T("addfriend2")) {
		m_pMainWindow->GetSelfInfo()->nAddFriendType = 1;//验证后才能加入
	}
	else if (msg.pSender->GetName() == _T("addfriend1")) {
		m_pMainWindow->GetSelfInfo()->nAddFriendType = 0;
	}
	return 0;
}
