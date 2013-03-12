#include "stdafx.h"
#include "ReportWindow.h"
#include "ChatWindow.h"
#include "MainWindow.h"


CReportWindow::CReportWindow(CChatDialog * pChatWnd)
	: m_pChatWindow(pChatWnd)
{}

CReportWindow::~CReportWindow()
{}


LPCTSTR CReportWindow::GetWindowClassName() const
{
	return _T("ReportWnd");
}

tstring CReportWindow::GetSkinFile()
{
	return _T("zireport.xml");
}

void    CReportWindow::Notify(TNotifyUI & msg)
{
	if(msg.sType == _T("click"))
	{
		DuiClickButtonMap(_T("CloseBtn"),   OnExit);
		DuiClickButtonMap(_T("CancelBtn"),  OnExit);
		DuiClickButtonMap(_T("SubmitBtn"),  OnReport);
	}
}

void    CReportWindow::OnFinalMessage(HWND hWnd)
{
	m_pChatWindow->m_pReportWindow = 0;

	CDuiWindowBase::OnFinalMessage(hWnd);
	delete this;
}

int    CReportWindow::OnExit(TNotifyUI & msg)
{
	Close();
	return 0;
}

int    CReportWindow::OnReport(TNotifyUI & msg)
{
	// 获取 
	// EvilAccountEdit, EvilRecordEdit, EvilDescriptEdit. 
	CEditUI     * pAccountUi  = DuiControl(CEditUI,     _T("EvilAccountEdit"));
	CRichEditUI * pRecordUi   = DuiControl(CRichEditUI, _T("EvilRecordEdit"));
	CRichEditUI * pDescriptUi = DuiControl(CRichEditUI, _T("EvilDescriptEdit"));

	Assert(pAccountUi && pRecordUi && pDescriptUi);

	DuiLib::CDuiString & stAccount  = pAccountUi->GetText();
	DuiLib::CDuiString & stRecords  = pRecordUi->GetText();
	DuiLib::CDuiString & stDescript = pDescriptUi->GetText();
	if(stAccount.IsEmpty() || stAccount.GetLength() == 0)
	{
		::MessageBox(m_hWnd, _T("帐号不能为空"), _T("错误"), 0);
		return 1;
	}

	if(stRecords.IsEmpty() || stRecords.GetLength() == 0)
	{
		::MessageBox(m_hWnd, _T("请输入举报的证据"), _T("错误"), 0);
		return 2;
	}

	ReportCsEvilData_t reportData;
	CT2A pAData1(stAccount), pAData2(stRecords), pAData3(stDescript);
	CT2A pszName(m_pChatWindow->GetSelfInfo()->tstrNickName.c_str());

	reportData.szAccount    = pAData1; reportData.szRecords = pAData2; reportData.szDescript = pAData3;
	reportData.nSenderId    = NetId_t(m_pChatWindow->GetSelfInfo()->chType, m_pChatWindow->GetSelfInfo()->nId);
	reportData.szSenderName = pszName;
	reportData.szRecverName = pAData1;
	//reportData.nRecverId    = -1;   // ???

	Assert(m_pChatWindow && m_pChatWindow->MainWindow());
	m_pChatWindow->MainWindow()->SendImMessageX(Msg_CsEvilReport, (LPARAM)&reportData, sizeof(reportData));

	// 需要关闭吗. ?
	OnExit(msg);
	return 0;
}

//int    CReportWindow::LocalToNet(int nMsg, void * pLocalData, int nLocalDataLen, Byte_t ** ppbNetData, int * pnNetDataLen)
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
//int    CReportWindow::FreeData(int nMsg, void * pbData)
//{
//	// ...
//	return 0;
//}
