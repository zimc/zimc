#include "stdafx.h"
#include "MsgRecordWindow.h"
#include<iostream>

CMsgRecordWindow::CMsgRecordWindow(CZiMainFrame* pMainWindow)
	: m_pMainWindow(pMainWindow)
	  ,MsgCount(0)
	  ,page(1)
	  ,PageCount(0)

{}

CMsgRecordWindow::~CMsgRecordWindow()
{}


LPCTSTR CMsgRecordWindow::GetWindowClassName() const
{
	return _T("MsgRecordWnd");
}

tstring CMsgRecordWindow::GetSkinFile()
{
	return _T("zichatconfig.xml");
}

void    CMsgRecordWindow::Notify(TNotifyUI & msg)
{
	if(msg.sType == _T("click"))
	{
		DuiClickButtonMap(_T("CloseBtn"),   OnExit);
		DuiClickButtonMap(_T("UpBtn"),  OnUp);
		DuiClickButtonMap(_T("NextBtn"),  OnNext);
	}
}

void    CMsgRecordWindow::OnFinalMessage(HWND hWnd)
{
	//m_pChatWindow->m_pMsgRecordWindow = 0;
	m_pMainWindow->m_pMsgRecordWindow = 0;

	CDuiWindowBase::OnFinalMessage(hWnd);
	delete this;
}

int    CMsgRecordWindow::OnExit(TNotifyUI & msg)
{
	Close();
	return 0;
}
//��һҳ
int   CMsgRecordWindow::OnUp(TNotifyUI & msg)
{
	// ��Ҫ�ر���. ?
	OnExit(msg);
	return 0;
}
//��һҳ
int CMsgRecordWindow::OnNext(TNotifyUI & msg)
{	
	
	return 0;
}

char CMsgRecordWindow::ReadMsgRecord(TNotifyUI & msg){
	string filename = "qqq.txt" ;	
	return 0 ;
}
