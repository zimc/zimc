#ifndef __MsgRecordWindow_H__
#define __MsgRecordWindow_H__


#include "DuiWindowBase.h"
#include "ChatWindow.h"

class CMsgRecordWindow
	: public CDuiWindowBase
{
public:
	CMsgRecordWindow(CChatDialog * pChatDialog);
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
	char* ReadMsgRecord(char *filename) ;
	//·­Ò³
	int    OnUp(char *filename,int page);
    int    OnNext(char *filename,int page);


private:
	CChatDialog * m_pChatDialog;
	int MsgCount, page, PageCount ;
};


#endif
