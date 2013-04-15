#ifndef __SETTING_H__
#define __SETTING_H__


#include "DuiWindowBase.h"
#include "MainWindow.h"

class CSettingWindow
    : public CDuiWindowBase
{
public:
    CSettingWindow(CZiMainFrame * pMainWnd);
    ~CSettingWindow();


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
    int    OnSetting(TNotifyUI & msg);
	int    OnSetAddFriend(TNotifyUI & msg);


private:
    CZiMainFrame * m_pMainWindow;
	int            m_nSetType;
};


#endif
