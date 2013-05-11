#ifndef __ReportWindow_H__
#define __ReportWindow_H__


#include "DuiWindowBase.h"
#include "MainWindow.h"

class CReportWindow
	: public CDuiWindowBase
{
public:
	CReportWindow(CZiMainFrame * pMainWnd, tstring &m_nickName);
	~CReportWindow();


public:
	// pure virtual
	LPCTSTR GetWindowClassName() const;
	tstring GetSkinFile();

	// INotiyUI
	void    Notify(TNotifyUI & msg);

	// message
	void    OnFinalMessage(HWND hWnd);

	// extend
	int    OnExit(TNotifyUI & msg);
	int    OnReport(TNotifyUI & msg);


private:
	CZiMainFrame * m_pMainWindow;
	tstring        m_nickName;
};


#endif
