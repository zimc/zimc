#ifndef __MsgRecordWindow_H__
#define __MsgRecordWindow_H__


#include "DuiWindowBase.h"
#include "MainWindow.h"

class CMsgRecordWindow
	: public CDuiWindowBase
{
public:
	CMsgRecordWindow(CZiMainFrame * pMainWnd);
	~CMsgRecordWindow();


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
	//È¡ÄÚÈÝ
	char ReadMsgRecord(TNotifyUI & msg) ;
	//·­Ò³
	int    OnUp(TNotifyUI & msg);
    int    OnNext(TNotifyUI & msg);


private:
	CZiMainFrame * m_pMainWindow;
	int MsgCount, page, PageCount ;
};


#endif
