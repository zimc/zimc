#ifndef __MsgRecordWindow_H__
#define __MsgRecordWindow_H__

#include <vector>
using namespace std;

#include "DuiWindowBase.h"
#include "ChatWindow.h"


#define  PAGE_COUNT  20

class CMsgRecordWindow
	: public CDuiWindowBase
{
public:
	CMsgRecordWindow(CChatDialog * pChatDialog, int suffix_num);
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
	
	//·­Ò³
	int    OnUp(TNotifyUI & msg);
    int    OnNext(TNotifyUI & msg);

	int    loadMsgRecord();
	void   showMsgRecord();


private:
	CChatDialog *      m_pChatDialog;
	int                m_nPage;
	int                m_nSuffixNum;
	int                m_nSuffixNum_now;
	vector <string>    m_vecMsgRecord;
};


#endif
