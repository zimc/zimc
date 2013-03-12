#include "stdafx.h"
#include "resource.h"
#include "DuiWindowBase.h"


#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#if USE(ZIP_SKIN)
	static const TCHAR* const kResourceSkinZipFileName = _T("zimc.dat");
#endif


LPBYTE CDuiWindowBase::m_pbResourceZip = NULL;


CDuiWindowBase::CDuiWindowBase()
{}

CDuiWindowBase::~CDuiWindowBase()
{}

int  CDuiWindowBase::Init()
{
	return 0;
}

int  CDuiWindowBase::Uninit()
{
	return 0;
}

UINT CDuiWindowBase::GetClassStyle() const
{
	return CS_DBLCLKS;
}

CControlUI* CDuiWindowBase::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void CDuiWindowBase::OnFinalMessage(HWND /*hWnd*/)
{
	Uninit();

	m_pmUi.RemovePreMessageFilter(this);
	m_pmUi.RemoveNotifier(this);
	m_pmUi.ReapObjects(m_pmUi.GetRoot());
}

LRESULT CDuiWindowBase::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if(::IsIconic(*this)) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CDuiWindowBase::OnNcCalcSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CDuiWindowBase::OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CDuiWindowBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if(!::IsZoomed(*this))
	{
		RECT rcSizeBox = m_pmUi.GetSizeBox();
		if(pt.y < rcClient.top + rcSizeBox.top)
		{
			if(pt.x < rcClient.left + rcSizeBox.left)   return HTTOPLEFT;
			if(pt.x > rcClient.right - rcSizeBox.right) return HTTOPRIGHT;
			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom)
		{
			if(pt.x < rcClient.left + rcSizeBox.left)   return HTBOTTOMLEFT;
			if(pt.x > rcClient.right - rcSizeBox.right) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}
		if(pt.x < rcClient.left + rcSizeBox.left)   return HTLEFT;
		if(pt.x > rcClient.right - rcSizeBox.right) return HTRIGHT;
	}

	RECT rcCaption = m_pmUi.GetCaptionRect();
	if(pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom)
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_pmUi.FindControl(pt));
		if(pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
			_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 /*&&
			_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 */)
			return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CDuiWindowBase::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pmUi.GetRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) )
	{
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}

	bHandled = FALSE;
	return 0;
}


LRESULT CDuiWindowBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 注意: 此处将 Close 的消息屏蔽了. 
	//       子类继承后需要重写 OnSysCommand 消息. 
	//       为什么要将 Close 消息屏蔽 ???
	if(wParam == SC_CLOSE)
	{
		bHandled = TRUE;
		return 0;
	}

	BOOL    bZoomed = ::IsZoomed(*this);
	LRESULT lRes    = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if(::IsZoomed(*this) != bZoomed )
	{
	}

	return lRes;
}

LRESULT CDuiWindowBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG lStyleValue  = ::GetWindowLong(*this, GWL_STYLE);
	lStyleValue      &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, lStyleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos (*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	m_pmUi.Init(m_hWnd);
	m_pmUi.AddPreMessageFilter(this);

	CDialogBuilder builder;
	m_pmUi.SetResourcePath(GetSkinFolder().c_str());

#if USE(ZIP_SKIN)
	tstring tstrSkin = GetSkinFile();
	if(m_pmUi.GetResourceZip().IsEmpty())
	{
#if USE(EMBEDED_RESOURCE)
		HRSRC hResource = ::FindResource(m_pmUi.GetResourceDll(), MAKEINTRESOURCE(IDR_ZIPRES), _T("ZIPRES"));
		if(hResource == NULL)
			return 0L;
		DWORD dwSize = 0;
		HGLOBAL hGlobal = ::LoadResource(m_pmUi.GetResourceDll(), hResource);
		if(hGlobal == NULL)
		{
			FreeResource(hResource);
			return 0L;
		}
		dwSize = ::SizeofResource(m_pmUi.GetResourceDll(), hResource);
		if(dwSize == 0)
			return 0L;
		m_pbResourceZip = new BYTE[ dwSize ];
		if(m_pbResourceZip != NULL)
		{
			::CopyMemory(m_pbResourceZip, (LPBYTE)::LockResource(hGlobal), dwSize);
		}
		::FreeResource(hResource);
		m_pmUi.SetResourceZip(m_pbResourceZip, dwSize);
#else
		m_pmUi.SetResourceZip(kResourceSkinZipFileName, true);
#endif
	}

#else
	// DuiLib中用的是相对路径，绝对路径会出错，最好能够内部判断一下。
	//tstring tstrSkin = m_pmUi.GetResourcePath();
	//tstrSkin += GetSkinFile();
	tstring tstrSkin = GetSkinFile();
#endif

	CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0, this, &m_pmUi);
	if(!pRoot) { ASSERT(0); return -1; }

	m_pmUi.AttachDialog(pRoot);
	m_pmUi.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CDuiWindowBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDuiWindowBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes     = 0;
	BOOL    bHandled = TRUE;
	switch(uMsg)
	{
	case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
//	case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;	
	case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONUP:		lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONDOWN:	lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEMOVE:		lRes = OnMouseMove(uMsg, wParam, lParam, bHandled); break;
	default:				bHandled = FALSE; break;
	}
	if(bHandled) return lRes;

	lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
	if(bHandled) return lRes;

	if(m_pmUi.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CDuiWindowBase::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/)
{
	if(uMsg == WM_KEYDOWN)
	{
		switch(wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return ResponseDefaultKeyEvent(wParam);

		default:
			break;
		}
	}

	return FALSE;
}

LRESULT CDuiWindowBase::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if(wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if(wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}

	return FALSE;
}

void    CDuiWindowBase::Cleanup()
{
	if(m_pbResourceZip != NULL)
	{
		delete[] m_pbResourceZip;
		m_pbResourceZip = NULL;
	}
}

tstring CDuiWindowBase::GetSkinFolder()
{
#if USE(ZIP_SKIN)
	return tstring(CPaintManagerUI::GetInstancePath());
#else
	return tstring(CPaintManagerUI::GetInstancePath()) + _T("zimc.skin\\");
#endif
}


// -------
int     CDuiWindowBase::OnClickClose(TNotifyUI & notifyUi)
{
	::PostQuitMessage(0);
	return 0;
}

int     CDuiWindowBase::OnClickMin(TNotifyUI & notifyUi)
{
	::ShowWindow(m_hWnd, SW_MINIMIZE);
	return 0;
}
