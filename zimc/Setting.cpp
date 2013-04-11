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
    if(msg.sType == _T("click"))
    {
        DuiClickButtonMap(_T("CloseBtn"),   OnExit);
        DuiClickButtonMap(_T("BtnCancel"),  OnExit);
        DuiClickButtonMap(_T("BtnOk"),  OnSetting);
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
