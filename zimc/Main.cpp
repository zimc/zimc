#include "stdafx.h"
#include "NetFrame.h"
#include "LoginWindow.h"
#include "MainWindow.h"
#include "ZimcHelper.h"
#include "WinDump.h"

#include <shellapi.h>


class CZimcInit
{
public:
	CZimcInit(HINSTANCE hInstance)
	{
		// Duilib Init
		CPaintManagerUI::SetInstance(hInstance);

		HRESULT hR = ::CoInitialize(NULL);
		Assert(SUCCEEDED(hR));

		// ws net
		WSADATA wsaData;
	    WORD    dwVersion = MAKEWORD(2, 0);
		int     nRet      = ::WSAStartup(dwVersion, &wsaData);
		Assert(!nRet);
	}

	~CZimcInit()
	{
		// ws net
		::WSACleanup();

		// Duilib Uninit. 
		::CoUninitialize();
	}
};


static HINSTANCE g_hInstanceHandle = 0;

HINSTANCE GetInstanceHandle() { 
	return g_hInstanceHandle; }

#define DuilibInitD(_hins)        \
	CZimcInit g_zimcInit(_hins);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	// other
	DeclareDumpFile();
	::setlocale(LC_ALL, "chs");

	// 1 Duilib 库初始化. 
	g_hInstanceHandle = hInstance;
	DuilibInitD(hInstance);

	// 2 初始化网络引擎. 
	int      nRet      = 0;
	LPWSTR * szArgList = 0;

#ifdef _TEST
	int      nArgs = 0;
	szArgList = ::CommandLineToArgvW(::GetCommandLine(), &nArgs);
	if(!szArgList || 
		nArgs != 3)
	{
		ZiLogger(RSZLOG_ERROR, "WinMain(), %d, %ws", nArgs, ::GetCommandLine());
		Assert(0);
		return 1;
	}
#endif

	nRet = CNetLayer::GetInstance()->Init(szArgList);
	if(nRet)
	{
		ZiLogger(RSZLOG_ERROR, "WinMain(), Init Failed, %d", nRet);
		CZimcHelper::ErrorMessageBox(0, 0, nRet);
		return nRet;
	}


//#define _TestMainFrame
#ifndef _TestMainFrame

	CZiLogin * pLogin = new CZiLogin();
	if(!pLogin) return 1;

	pLogin->Create(0, _T("登录"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW);

	if(!pLogin->IsExit())
	{
		pLogin->CenterWindow();
		pLogin->ShowWindow(true);
	}
#else

	CZiMainFrame * pMainFrame = new CZiMainFrame();
	if(!pMainFrame) return 1;

	pMainFrame->Create(0, _T("主界面"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW);
	pMainFrame->SetWindowPosition();
	pMainFrame->ReadSelfInfo();
	//pMainFrame->SetMainInfo(0);
	pMainFrame->ShowWindow(true);
#endif

	CPaintManagerUI::MessageLoop();


	// 退出, 退出时网络层最后退出. 
	CNetLayer::GetInstance()->Uninit();
	return 0;
}
