#ifndef __TrayWindow_H__
#define __TrayWindow_H__


#include <shellapi.h>
#include "resource.h"


class CTrayWindow
{
public:
	CTrayWindow(HWND hWnd)
		: m_hWnd(hWnd)
		, m_dwIconId(IDR_MAINFRAME)
	{}

	~CTrayWindow()
	{}

	void SetHwnd(HWND hWnd)
	{
		m_hWnd = hWnd;
	}

	int AddTray()
	{
		return DoTray(NIM_ADD);
	}

	int DeleteTray()
	{
		return DoTray(NIM_DELETE);
	}

	int FlashTray(BOOL bSet = FALSE, BOOL bNewId = 1)
	{
		static DWORD dwId        = 0;
		static DWORD dwIconId[2] = { IDR_MAINFRAME, IDI_ICON2 };

		if(bSet) dwId = bNewId;
		DoTray(NIM_MODIFY, dwIconId[dwId]);
		dwId = !dwId;
		return 0;
	}

	int ShowMenu()
	{
	}

	int OnClickMenu()
	{
	}


public:
	int DoTray(DWORD dwTrayMsg, DWORD dwIconId = 0, const char *trayName = NULL)
	{
		Assert(m_hWnd);

		extern HINSTANCE  GetInstanceHandle();
		if(dwIconId == 0) dwIconId = m_dwIconId;
		HICON hIcon = (HICON)::LoadIcon(GetInstanceHandle(), MAKEINTRESOURCE(dwIconId));

		NOTIFYICONDATA trayData;
		trayData.cbSize  = (DWORD)sizeof(trayData);
		trayData.hWnd    = m_hWnd;
		trayData.uID     = IDR_MAINFRAME;
		trayData.uFlags  = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		trayData.hIcon   = hIcon;
		trayData.uCallbackMessage = Msg_InTray;
	
		if (trayName == NULL) {
			::_tcscpy(trayData.szTip, _T("chat"));
		}
		else {
			::_tcscpy(trayData.szTip, CA2T(trayName));
		}
		BOOL bRet = ::Shell_NotifyIcon(dwTrayMsg, &trayData);

		return 0;
	}


private:
	HWND  m_hWnd;
	DWORD m_dwIconId;
	string m_strName;
};


#endif
