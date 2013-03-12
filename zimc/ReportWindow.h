#ifndef __ReportWindow_H__
#define __ReportWindow_H__


#include "DuiWindowBase.h"


class CChatDialog;

class CReportWindow
	: public CDuiWindowBase
{
public:
	CReportWindow(CChatDialog * pMainWnd);
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
	CChatDialog * m_pChatWindow;
};


#endif
