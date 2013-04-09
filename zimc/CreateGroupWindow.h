#ifndef __CreateGroup_Window_H__
#define __CreateGroup_Window_H__


#include "DuiWindowBase.h"
#include "MainWindow.h"

class CCreateGroupWindow
	: public CDuiWindowBase
{
public:
	CCreateGroupWindow(CZiMainFrame * pMainWnd, int type = 0, char *group_name=NULL,int group_id=0);
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
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	int    OnCreateGroup(TNotifyUI & msg);


private:
	CZiMainFrame * m_pMainWindow;
	int m_nType;  //0 创建群, 1:修改群名称
	char* m_chName;
	int m_nGroupId;
};


#endif //__CreateGroupWindow_H__
