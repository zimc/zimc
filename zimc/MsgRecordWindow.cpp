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
	return _T("msgrecord.xml");
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
//上一页
int   CMsgRecordWindow::OnUp(TNotifyUI & msg)
{
	// 需要关闭吗. ?
	OnExit(msg);
	return 0;
}
//下一页
int CMsgRecordWindow::OnNext(TNotifyUI & msg)
{	
	
	return 0;
}

char CMsgRecordWindow::ReadMsgRecord(TNotifyUI & msg){
	string filenam = "qqq.txt" ;
	
	return 0 ;
}

//int    CMsgRecordWindow::LocalToNet(int nMsg, void * pLocalData, int nLocalDataLen, Byte_t ** ppbNetData, int * pnNetDataLen)
//{
//	Assert(nMsg != 0 && pLocalData && nLocalDataLen > 0);
//	int nRet = 0;
//
//	switch(nMsg)
//	{
//	case Msg_CsEvilReport:
//		{
//			ReportCsEvilData_t * pReportData = (ReportCsEvilData_t*)pLocalData;
//
//			// ...
//		}
//		break;
//	}
//
//	return 0;
//}
//
//int    CMsgRecordWindow::FreeData(int nMsg, void * pbData)
//{
//	// ...
//	return 0;
//}
