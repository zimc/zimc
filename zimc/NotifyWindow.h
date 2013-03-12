#ifndef __NotifyWindow_H__
#define __NotifyWindow_H__


class CNotifyWindow 
	: public DuiLib::CWindowWnd
	, public DuiLib::INotifyUI
{
public:
	CNotifyWindow()
		: m_hParent(0)
		, m_nMsg(0)
		, m_pUserData(0)
	{}

	~CNotifyWindow()
	{}

	LPCTSTR GetWindowClassName() const
	{
		return _T("NotifyWindow");
	}

	LPCTSTR GetSkinFolder()
	{
		static tstring strPath = tstring(
			CPaintManagerUI::GetInstancePath()) + _T("zimc.skin\\");

		return strPath.c_str();
	}

	LPCTSTR GetSkinFile()
	{
		return _T("zinotify.xml");
	}

	int Init(HWND hParent, LPCTSTR tsText, LPCTSTR tsButton1, LPCTSTR tsButton2, 
		int nMsg, void * pUserData)
	{
		m_hParent      = hParent;
		m_nMsg         = nMsg;
		m_pUserData    = pUserData;

		DWORD dwType   = ::GetWindowLong(m_hWnd, GWL_STYLE);
		dwType        &= ~WS_MAXIMIZEBOX;
		dwType        &= ~WS_MINIMIZEBOX;
		::SetWindowLong(m_hWnd, GWL_STYLE, dwType);

		Center();
		SetContent(tsText, tsButton1, tsButton2);
		return 0;
	}

	static int MessageBoxX(HWND hWnd, LPCTSTR tsCaption, LPCTSTR tsText, 
		LPCTSTR tsButton1 = 0, LPCTSTR tsButton2 = 0, 
		int nMsg = 0, void * pUserData = 0)
	{
		CNotifyWindow * pNotifyWnd = new CNotifyWindow;
		if(!pNotifyWnd) return 1;

		pNotifyWnd->Create(hWnd, tsCaption, UI_WNDSTYLE_FRAME | WS_POPUP, 0, 0, 0, 0, 0);
		pNotifyWnd->Init(hWnd, tsText, tsButton1, tsButton2, nMsg, pUserData);
		pNotifyWnd->ShowWindow(TRUE);
		return 0;
	}

	void Notify(TNotifyUI & msg)
	{
		if(msg.sType == _T("click"))
		{
			DuiClickButtonMap(_T("BtnOk"),     OnClickButton1);
			DuiClickButtonMap(_T("BtnCancel"), OnClickButton2);
		}
	}


protected:
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(uMsg == WM_CREATE) 
		{
			return OnCreate(wParam, lParam);
		}
		else if(uMsg == WM_DESTROY)
		{
			Close();
		}
		else if(uMsg == WM_ERASEBKGND)
		{
			return 1;
		}

		LRESULT lRes = 0;
		if(m_pmUi.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	LRESULT OnCreate(WPARAM wParam, LPARAM lParam)
	{
		m_pmUi.Init(m_hWnd);
		m_pmUi.SetResourcePath(GetSkinFolder());

		CDialogBuilder dlgBuilder;
		CControlUI * pCtrlUi = dlgBuilder.Create(GetSkinFile(), 0, 0, &m_pmUi);
		if(!pCtrlUi) { Assert(0); return -1; }

		m_pmUi.AttachDialog(pCtrlUi);
		m_pmUi.AddNotifier(this);
		return 0;
	}

	void    Center()
	{
		RECT rcWnd  = {0};
		RECT rcArea = {0};
		::GetWindowRect(m_hWnd, &rcWnd);
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcArea, 0);

		int nX = (rcArea.right  - rcArea.left) / 2 - (rcWnd.right  - rcWnd.left) / 2;
		int nY = (rcArea.bottom - rcArea.top ) / 2 - (rcWnd.bottom - rcWnd.top ) / 2;

		::SetWindowPos(m_hWnd, 0, nX, nY, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void    SetContent(LPCTSTR tsText, LPCTSTR tsButton1, LPCTSTR tsButton2)
	{
		CRichEditUI * pTextUi  = DuiControl(CRichEditUI, _T("EditText"));
		CButtonUI   * pButton1 = DuiControl(CButtonUI,   _T("BtnOk"));
		CButtonUI   * pButton2 = DuiControl(CButtonUI,   _T("BtnCancel"));
		Assert(pTextUi && pButton1 && pButton2);

		if(tsText)    pTextUi ->SetText(tsText);
		if(tsButton1) pButton1->SetText(tsButton1);
		if(tsButton2) pButton2->SetText(tsButton2);
	}

	int     OnClickButton1(TNotifyUI & notifyUi)
	{
		if(m_nMsg && m_hParent)
		{
			::SendMessage(m_hParent, m_nMsg, 1, (LPARAM)m_pUserData);
		}

		Close();
		return 0;
	}

	int     OnClickButton2(TNotifyUI & notifyUi)
	{
		if(m_nMsg && m_hParent)
		{
			::SendMessage(m_hParent, m_nMsg, 2, (LPARAM)m_pUserData);
		}

		Close();
		return 0;
	}


public:
	CPaintManagerUI m_pmUi;

	HWND    m_hParent;
	int     m_nMsg;
	void  * m_pUserData;
};


#endif
