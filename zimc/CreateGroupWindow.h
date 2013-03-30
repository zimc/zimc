#ifndef __CreateGroup_Window_H__
#define __CreateGroup_Window_H__


#include "DuiWindowBase.h"
#include "MainWindow.h"

class CCreateGroupWindow
	: public CDuiWindowBase
{
public:
	CCreateGroupWindow(CZiMainFrame * pMainWnd);
	~CCreateGroupWindow();


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
	int    OnCreateGroup(TNotifyUI & msg);


private:
	CZiMainFrame * m_pMainWindow;
};


#endif //__CreateGroupWindow_H__
