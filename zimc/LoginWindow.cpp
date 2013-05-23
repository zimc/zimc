#include "stdafx.h"
#include "DuiWindowBase.h"
#include "LoginWindow.h"
#include "MainWindow.h"
#include "LoginMsger.h"

#include "ZiCoder.h"
#include "ZimcHelper.h"
#include "shellapi.h"
#include <atlbase.h>
#include <list>
#include <string>

#include "common/utils2.h"


using namespace std;

#define  g_szLoginFile				"zius.dat"
#define  g_tsRegisterUrl			_T("www.arebank.com/?app=reg_gr")
#define  g_tsFindPwdUrl				_T("www.arebank.com/index.php?app=findpwd")


const TCHAR * g_tsAccountDefault  = _T("帐 号");
const TCHAR * g_tsPasswordDefault = _T("密 码");
const TCHAR * g_tsEmpty           = _T("");


// ------------------------------------------------------------------------
// 
CZiLogin::CZiLogin()
	: m_nLoginType(Type_LoginDefault)
	, m_nLastLoginType(Type_LoginDefault)
	, m_bIsExit(FALSE)
	, m_nWindowIndex(0)
	, m_pLoginMsger(0)
{
}

CZiLogin::~CZiLogin()
{
}

int  CZiLogin::Init()
{
	if(!m_pLoginMsger)
	{
		m_pLoginMsger = new CLoginMsger(this);
		if(!m_pLoginMsger) return Error_OutOfMemory;
		m_pLoginMsger->Init();
	}

	// 获取登陆模式, 判断是否自动登陆. 
	int     nRet       = 0;
	tstring tstrAccount, 
		    tstrPassword;

	// 设置自动登录. 
	nRet = GetAutoLogin(m_nLastLoginType, tstrAccount, tstrPassword);
	if(!nRet)
	{
		m_nLoginType = m_nLastLoginType;
	}

	// 设置自动保存密码. 
	if(m_nLoginType != Type_LoginDefault && 
		(tstrAccount.empty() || tstrPassword.empty()))
	{
		nRet = GetAutoPassword(tstrAccount, tstrPassword);
	}

	if(!tstrAccount.empty() && !tstrPassword.empty())
	{
		CEditUI * pAccount  = DuiControl(CEditUI, AccountControlName());
		CEditUI * pPassword = DuiControl(CEditUI, PasswordControlName());

		Assert(pAccount && pPassword);
		Assert(!tstrAccount.empty() && !tstrPassword.empty());

		pPassword->SetPasswordMode(true);
		pAccount ->SetText(tstrAccount.c_str());
		pPassword->SetText(tstrPassword.c_str());
	}

	if(m_nLastLoginType & Type_LoginAutoLogin)
	{
		CEditUI * pAccount  = DuiControl(CEditUI, AccountControlName());
		CEditUI * pPassword = DuiControl(CEditUI, PasswordControlName());

		Assert(pAccount && pPassword);
		Assert(!tstrAccount.empty() && !tstrPassword.empty());

		// 如果是自动登陆
		// 确认焦点不再 account 和 pwd 上. ******
		pAccount ->SetEnabled(false);
		pPassword->SetEnabled(false);
		AsyncLogin(tstrAccount.c_str(), tstrPassword.c_str());
	}

	return 0;
}

int  CZiLogin::Uninit()
{
	if(m_pLoginMsger)
	{
		m_pLoginMsger->Uninit();
		delete m_pLoginMsger;
		m_pLoginMsger = 0;
	}

	return 0;
}

BOOL CZiLogin::IsExit()
{
	return m_bIsExit;
}

void CZiLogin::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("itemselect"))
	{
		DuiGenericMessageMap(_T("AccountCombo"), OnSelectAcount);
	}
	else if(msg.sType == _T("setfocus"))
	{
		DuiGenericMessageMap(_T("AccountEdit1"), OnFocusAccount);
		DuiGenericMessageMap(_T("PwdEdit1"),     OnFocusPwd);
		DuiGenericMessageMap(_T("AccountEdit2"), OnFocusAccount);
		DuiGenericMessageMap(_T("PwdEdit2"),     OnFocusPwd);
	}
	else if(msg.sType == _T("killfocus"))
	{
		DuiGenericMessageMap(_T("AccountEdit1"), OnFocusAccount);
		DuiGenericMessageMap(_T("PwdEdit1"),     OnFocusPwd);
		DuiGenericMessageMap(_T("AccountEdit2"), OnFocusAccount);
		DuiGenericMessageMap(_T("PwdEdit2"),     OnFocusPwd);

	}
	else if(msg.sType == _T("return"))
	{
		DuiGenericMessageMap(_T("AccountEdit1"), OnReturnAccount);
		DuiGenericMessageMap(_T("PwdEdit1"),     OnReturnPwd);
		DuiGenericMessageMap(_T("AccountEdit2"), OnReturnAccount);
		DuiGenericMessageMap(_T("PwdEdit2"),     OnReturnPwd);
	}
	else if(msg.sType == _T("click"))
	{
		DuiClickButtonMap(_T("WinPushBtn"),     OnWindowPush)
		DuiClickButtonMap(_T("WinUpBtn"),       OnWindowUp)

		DuiClickButtonMap(_T("LoginBtn"),       OnLogin)
		DuiClickButtonMap(_T("RegisterBtn"),    OnRegister)
		DuiClickButtonMap(_T("FindPwdBtn"),     OnFindPwd)
		DuiClickButtonMap(_T("MinBtn"),         OnClickMin)
		DuiClickButtonMap(_T("CloseBtn"),       OnClickClose)
	}
	else if(msg.sType == DUI_MSGTYPE_SELECTCHANGED)
	{
		DuiClickButtonMap(_T("SavePwdBtn"),     OnSavePwd)
		DuiClickButtonMap(_T("AutoLoginBtn"),   OnAutoLogin)
	}

	return;
}

LPCTSTR CZiLogin::GetWindowClassName() const
{
	return _T("LoginUI");
}

LRESULT CZiLogin::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam == SC_CLOSE)
	{
		Close();
		::PostQuitMessage(0);
		return 0;
	}

	return CDuiWindowBase::OnSysCommand(uMsg, wParam, lParam, bHandled);
}

LRESULT CZiLogin::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	CDuiWindowBase::OnCreate(uMsg, wParam, lParam, bHandled);
	SetIcon(128);

	// bg tranf
	//#define WS_EX_LAYERED		0x00080000
	//#define LWA_COLORKEY		0x00000001
	COLORREF colorMask = RGB(0, 255, 0);

	::SetWindowLong(m_hWnd, GWL_EXSTYLE, 
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(m_hWnd, colorMask, 0, LWA_COLORKEY);
	//::RedrawWindow(m_hWnd, 0, 0, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);

	::SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);

	TNotifyUI notify;
	OnWindowUp(notify);

	//确保缓存文件夹存在
	string tmp_path;
	CZimcHelper::WideToMulti2(GetPaintManagerUI()->GetInstancePath().GetData(), tmp_path);
	//tmp_path.append("\\");
	tmp_path.append(TMP_DATA_DIR);
	createDirectory(tmp_path.c_str());

	return S_OK;
}

LRESULT CZiLogin::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	CControlUI* pControl = DuiControl(CControlUI, pt);
	if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
		_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
		_tcsicmp(pControl->GetClass(), _T("TextUI"))   != 0 && 
		_tcsicmp(pControl->GetClass(), _T("CheckBoxUI")) != 0 && 
		_tcsicmp(pControl->GetClass(), _T("EditUI"))   != 0)
	{
		return HTCAPTION;
	}

	return HTCLIENT;
}

tstring CZiLogin::GetSkinFile()
{
	return tstring(_T("zilogin.xml"));
}


// ------------------------------------------------------------------------
// 
int     CZiLogin::OnWindowPush(TNotifyUI & notifyUi)
{
	CContainerUI * pWinUp   = DuiControl(CContainerUI, _T("LoginBg1"));
	CContainerUI * pWinPush = DuiControl(CContainerUI, _T("LoginBg2"));

	Assert(pWinUp && pWinPush);
	pWinUp  ->SetVisible(false);
	pWinPush->SetVisible(true);

	// auto password. 
	if(m_nLoginType & Type_LoginAutoPassword)
	{
		CCheckBoxUI * pAutoPwd = DuiControl(CCheckBoxUI, _T("SavePwdBtn"));
		pAutoPwd->SetCheck(true);
	}

	// auto login.
	if(m_nLoginType & Type_LoginAutoLogin)
	{
		CCheckBoxUI * pAutoLogin = DuiControl(CCheckBoxUI, _T("AutoLoginBtn"));
		pAutoLogin->SetCheck(true);
	}

	// 0 --> 1
	if(m_nWindowIndex == 0) ChangeWindow(0, 1);
	return 0;
}

int     CZiLogin::OnWindowUp(TNotifyUI & notifyUi)
{
	CContainerUI * pWinUp   = DuiControl(CContainerUI, _T("LoginBg1"));
	CContainerUI * pWinPush = DuiControl(CContainerUI, _T("LoginBg2"));

	Assert(pWinUp && pWinPush);
	pWinPush->SetVisible(false);
	pWinUp  ->SetVisible(true);

	// 1 --> 0
	if(m_nWindowIndex == 1) ChangeWindow(1, 0);

	return 0;
}

int     CZiLogin::OnSelectAcount(TNotifyUI & notifyUi)
{
	//CEditUI* pAccountEdit = DuiControl(CEditUI, AccountControlName());

	//if(pAccountEdit && 
	//	notifyUi.pSender)
	//{
	//	pAccountEdit->SetText(notifyUi.pSender->GetText());
	//}

	return 0;
}

int     CZiLogin::OnFocusAccount(TNotifyUI & notifyUi)
{
	Assert(notifyUi.sType == _T("setfocus") || 
		notifyUi.sType == _T("killfocus"));

	BOOL bSetFocus = notifyUi.sType == _T("setfocus");

	// 字符串居中输入时比较麻烦, 暂时不居中写入. 
	// ...............
	CEditUI* pAccountEdit   = DuiControl(CEditUI, AccountControlName());
	Assert(pAccountEdit);
	LPCTSTR  ptsAccount     = pAccountEdit->GetText().GetData();

	if(bSetFocus && ptsAccount && 
		_tcsicmp(ptsAccount, g_tsAccountDefault) == 0)
	{
		pAccountEdit->SetText(g_tsEmpty);
	}

	if(!bSetFocus && ptsAccount && 
		_tcsicmp(ptsAccount, g_tsEmpty) == 0)
	{
		pAccountEdit->SetText(g_tsAccountDefault);
	}

	return 0;
}

int     CZiLogin::OnFocusPwd(TNotifyUI & notifyUi)
{
	Assert(notifyUi.sType == _T("setfocus") || 
		notifyUi.sType == _T("killfocus"));

	BOOL bSetFocus = notifyUi.sType == _T("setfocus");

	CEditUI* pPasswordEdit   = DuiControl(CEditUI, PasswordControlName());
	Assert(pPasswordEdit);
	LPCTSTR  ptsPassword     = pPasswordEdit->GetText().GetData();

	if(bSetFocus && 
		_tcsicmp(ptsPassword, g_tsPasswordDefault) == 0)
	{
		// 当使用 TAB 切换时使用 SetPasswordMode() 会显示明文 
		pPasswordEdit->SetPasswordModeEx(true);
		pPasswordEdit->SetText(g_tsEmpty);
	}

	if(!bSetFocus && 
		_tcsicmp(ptsPassword, g_tsEmpty) == 0)
	{
		pPasswordEdit->SetPasswordMode(false);
		pPasswordEdit->SetText(g_tsPasswordDefault);
	}

	return 0;
}

int     CZiLogin::OnReturnAccount(TNotifyUI & notifyUi)
{
	CEditUI* pAccountEdit   = DuiControl(CEditUI, AccountControlName());
	CEditUI* pPasswordEdit  = DuiControl(CEditUI, PasswordControlName());
	Assert(pAccountEdit && pPasswordEdit);

	LPCTSTR  strAccount  = pAccountEdit->GetText();
	LPCTSTR  strPassword = pPasswordEdit->GetText();

	if(strAccount && strAccount[0] && 
		strPassword && strPassword[0] && 
		_tcscmp(strAccount, g_tsAccountDefault) != 0 && 
		_tcscmp(strPassword, g_tsPasswordDefault) != 0) 
	{
		// 开始登陆
		// ...
		::SendMessage(::GetFocus(), WM_KILLFOCUS, 0, 0);
		pAccountEdit->SetEnabled(false);
		pPasswordEdit->SetEnabled(false);

		// test -----------------------------------
		//LoginScData_t stData = {0};
		//AsyncLoginCall(0, &stData);
		//return 2;

		AsyncLogin(strAccount, strPassword);
		return 1;
	}

	return 0;
}

int     CZiLogin::OnReturnPwd(TNotifyUI & notifyUi)
{
	return OnReturnAccount(notifyUi);
}

int     CZiLogin::OnClickAccount(TNotifyUI & notifyUi)
{
	Assert(0);
	return 0;
}

int     CZiLogin::OnClickPwd(TNotifyUI & notifyUi)
{
	// 以前的设计是: 可以有 N 个帐号, 每个帐号都可以设置
	// 保存密码功能, 用户选择帐号, 自动设置了密码. 
	// 
	Assert(0);
	//if(m_nLastLoginType & Type_LoginAutoPassword)
	//{
	//	CEditUI     * pAccoutEdit = DuiControl(CEditUI,     _T("AccountEdit"));
	//	CEditUI     * pPwdEdit    = DuiControl(CEditUI,     _T("PwdEdit"));
	//	CCheckBoxUI * pPwdCheck   = DuiControl(CCheckBoxUI, _T("SavePwdBtn"));

	//	if(pAccoutEdit && pPwdEdit && pPwdCheck)
	//	{
	//		int     nRet = 0;
	//		tstring strPassword;

	//		nRet = GetAutoPassword(pAccoutEdit->GetText(), strPassword);
	//		if(!nRet)
	//		{
	//			Assert(!strPassword.empty());
	//			pPwdEdit ->SetText(strPassword.c_str());
	//			pPwdCheck->SetCheck(true);
	//		}
	//	}
	//}

	return 0;
}

int     CZiLogin::OnLogin(TNotifyUI & /*notifyUi*/)
{
	Assert(0);

	int nRet = 0;
	CDuiString dstrAccount, 
			   dstrPassword;

	nRet = GetCurrentLoginInfo(dstrAccount, dstrPassword);
	if(nRet)
	{
		CZimcHelper::ErrorMessageBox(m_hWnd, Msg_ScLogin, nRet);
		return nRet;
	}

	nRet = SyncNormalLogin(dstrAccount.GetData(), dstrPassword.GetData());
	if(nRet)
	{
		CZimcHelper::ErrorMessageBox(m_hWnd, Msg_ScLogin, nRet);
		return nRet;
	}

	if(m_nLoginType != m_nLastLoginType)
	{
		nRet = UpdateLoginInfo(dstrAccount, dstrPassword);
	}

	return 0;
}

int     CZiLogin::OnRegister(TNotifyUI & /*notifyUi*/)
{
	OpenIE(g_tsRegisterUrl);
	return 0;
}

int     CZiLogin::OnFindPwd(TNotifyUI & /*notifyUi*/)
{
	OpenIE(g_tsFindPwdUrl);
	return 0;
}

int     CZiLogin::OnAutoLogin(TNotifyUI & /*notifyUi*/)
{
	CCheckBoxUI * pLoginCheck = DuiControl(CCheckBoxUI, _T("AutoLoginBtn"));
	if(pLoginCheck->IsSelected())
	{
		m_nLoginType |= Type_LoginAutoLogin;
	}
	else
	{
		m_nLoginType &= ~Type_LoginAutoLogin;
	}

	return 0;
}

int     CZiLogin::OnSavePwd(TNotifyUI & /*notifyUi*/)
{
	CCheckBoxUI * pPwdCheck = DuiControl(CCheckBoxUI, _T("SavePwdBtn"));
	if(pPwdCheck->IsSelected())
	{
		m_nLoginType |= Type_LoginAutoPassword;
	}
	else
	{
		m_nLoginType &= ~Type_LoginAutoPassword;
	}

	return 0;
}

int     CZiLogin::OpenIE(LPCTSTR lpcsUrl)
{
	SHELLEXECUTEINFO execInfo = {0};

	execInfo.cbSize = sizeof(execInfo);
	execInfo.fMask  = SEE_MASK_NOCLOSEPROCESS;
	execInfo.lpFile = lpcsUrl;
	execInfo.lpVerb = _T("open");

	::ShellExecuteEx(&execInfo);
	return 0;
}

LPCTSTR CZiLogin::AccountControlName(int nIndex)
{
	static LPCTSTR s_tsAccountControlName[] = 
	{
		_T("AccountEdit1"), 
		_T("AccountEdit2"), 
	};

	return nIndex == -1 ? 
		s_tsAccountControlName[m_nWindowIndex] : 
		s_tsAccountControlName[nIndex];
}

LPCTSTR CZiLogin::PasswordControlName(int nIndex)
{
	static LPCTSTR s_tsPasswordControlName[] = 
	{
		_T("PwdEdit1"), 
		_T("PwdEdit2"), 
	};

	return nIndex == -1 ? 
		s_tsPasswordControlName[m_nWindowIndex] : 
		s_tsPasswordControlName[nIndex];
}

int     CZiLogin::ChangeWindow(int nOld, int nNew)
{
	Assert(m_nWindowIndex == nOld);

	CEditUI* pAccountEdit0  = DuiControl(CEditUI, AccountControlName(nOld));
	CEditUI* pPasswordEdit0 = DuiControl(CEditUI, PasswordControlName(nOld));
	CEditUI* pAccountEdit1  = DuiControl(CEditUI, AccountControlName(nNew));
	CEditUI* pPasswordEdit1 = DuiControl(CEditUI, PasswordControlName(nNew));
	Assert(pAccountEdit0 && pPasswordEdit0);
	Assert(pAccountEdit1 && pPasswordEdit1);

	if(0 != _tcscmp(pAccountEdit0->GetText(), pAccountEdit1->GetText()))
	{
		pAccountEdit1->SetText(pAccountEdit0->GetText());
	}

	if(0 != _tcscmp(pPasswordEdit0->GetText(), pPasswordEdit1->GetText()))
	{
		pPasswordEdit1->SetPasswordMode(true);
		pPasswordEdit1->SetText(pPasswordEdit0->GetText());
	}

	if(!pAccountEdit0->IsEnabled()) pAccountEdit1->SetEnabled(true);
	if(!pAccountEdit0->IsEnabled()) pAccountEdit1->SetEnabled(true);

	m_nWindowIndex = nNew;

	return 0;
}


// ------------------------------------------------------------------------
// 
int     CZiLogin::GetCurrentLoginInfo(CDuiString & dstrAccount, CDuiString & dstrPassword)
{
	CEditUI* pAccountEdit  = DuiControl(CEditUI, AccountControlName());
	CEditUI* pPasswordEdit = DuiControl(CEditUI, PasswordControlName());

	if(!pAccountEdit || 
		!pPasswordEdit)
	{
		return Error_Fail;
	}

	dstrAccount = pAccountEdit->GetText();
	if(dstrAccount.IsEmpty())
	{
		return Error_LoginNoAccount;
	}

	Assert(pPasswordEdit);

	dstrPassword = pPasswordEdit->GetText();
	if(dstrPassword.IsEmpty())
	{
		return Error_LoginNoPassword;
	}

	if(dstrPassword.GetLength() < 6)
	{
		return Error_LoginPasswordError;
	}

	return 0;
}

//int     CZiLogin::SyncNormalLogin(LPCTSTR lpctsAccount, LPCTSTR lpctsPassword)
//{
//	// 隐藏登陆窗口
//	// 创建主窗口(等待状态), 并显示
//	// 等待结果 ... 如果成功, 则改变主窗口状态为上线状态. 销毁登陆窗口. 
//	//                  失败, 则销毁主窗口, 显示login 窗口. 
//	//                        
//
//	Assert(lpctsAccount && lpctsAccount[0] && 
//		lpctsPassword && lpctsPassword[0]);
//
//	ShowWindow(false, false);
//
//	CZiMainFrame * pMainFrame = new CZiMainFrame;
//	if(!pMainFrame) return Error_Fail;
//
//	pMainFrame->Init();
//	pMainFrame->Create(0, _T("主界面"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW);
//	pMainFrame->ShowWindow();
//
//	int nRet = 0;
//	nRet = NetworkLogin(lpctsAccount, lpctsPassword);
//
//	if(!nRet)
//	{
//		pMainFrame->SetState(State_MainRunning);
//		m_bIsExit = TRUE;
//	}
//	else
//	{
//		pMainFrame->Close();
//		delete pMainFrame;
//
//		ShowWindow(true, true);
//	}
//
//	return 0;
//}

int     CZiLogin::SyncNormalLogin(LPCTSTR lpctsAccount, LPCTSTR lpctsPassword)
{
	Assert(0);

	//Assert(lpctsAccount && lpctsAccount[0] && 
	//	lpctsPassword && lpctsPassword[0]);

	//// 设置登陆界面为等待状态. 
	//// ???

	//int        nRet = 0;
	//tstring    tstrLoginRet;
	//nRet = NetworkLogin(lpctsAccount, lpctsPassword, tstrLoginRet);

	//if(nRet)
	//{
	//	// 设置登陆界面为设置状态. 
	//	// ???
	//	return 1;
	//}

	//// 销毁登陆界面
	//Close();

	//// 显示主界面
	//CZiMainFrame * pMainFrame = new CZiMainFrame;
	//if(!pMainFrame) return Error_Fail;

	//pMainFrame->Create(0, _T("主界面"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW);
	//pMainFrame->SetWindowPosition();
	//pMainFrame->SetMainInfo(...);
	//pMainFrame->SetState(State_MainRunning);
	//pMainFrame->ShowWindow(true);

	return 0;
}

int     CZiLogin::AutoLogin(LPCTSTR lpctsAccount, LPCTSTR lpctsPassword)
{
	Assert(0);
	return 0;

	// 在初始化中调用. 
	// 此时窗口没有被显示. 
	//Assert(lpctsAccount && lpctsAccount[0] && 
	//	lpctsPassword && lpctsPassword[0]);

	//CEditUI     * pAccountEdit = DuiControl(CEditUI,     AccountControlName());
	//CEditUI     * pPwdEdit     = DuiControl(CEditUI,     PasswordControlName());
	//CCheckBoxUI * pPwdCheck    = DuiControl(CCheckBoxUI, _T("SavePwdBtn"));
	//CCheckBoxUI * pLoginCheck  = DuiControl(CCheckBoxUI, _T("AutoLoginBtn"));

	//if(pAccountEdit || pPwdEdit || 
	//	pPwdCheck || pLoginCheck)
	//{
	//	return Error_Fail;
	//}

	//pAccountEdit->SetText(lpctsAccount);
	//pPwdEdit->SetText(lpctsPassword);
	////pPwdCheck->SetCheck(true);
	////pLoginCheck->SetCheck(true);

	//int nRet = 0;
	//nRet = SyncNormalLogin(lpctsAccount, lpctsPassword);

	//return nRet;
}

int     CZiLogin::GetAutoLogin(int & nLoadType, tstring & tstrAccount, tstring & tstrPassword)
{
	int          nRet = 0;
	bool         bRet = false;
	std::string  strLoginInfo;
	Json::Reader jsReader;
	Json::Value  jsRoot;
	Json::Value  jsAutoLogin;

	nRet = ReadLoginInfo(strLoginInfo);
	if(strLoginInfo.empty()) return nRet;

	bRet = jsReader.parse(strLoginInfo, jsRoot);
	if(!bRet) return 1;

	if(jsRoot.isMember("logintype"))
	{
		nLoadType = jsRoot["logintype"].asInt();
	}

	if(jsRoot.isMember("autologin"))
	{
		jsAutoLogin  = jsRoot["autologin"];
		tstrAccount  = (LPTSTR)CA2T(jsAutoLogin["account"].asString().c_str());
		tstrPassword = (LPTSTR)CA2T(jsAutoLogin["pwd"].asString().c_str());
	}

	return nLoadType != Type_LoginDefault ? 
		0 : 2;
}

int     CZiLogin::GetAutoPassword(tstring & tstrAccount, tstring & tstrPassword)
{
	int          nRet = 0;
	bool         bRet = false;
	std::string  strLoginInfo;
	Json::Reader jsReader;
	Json::Value  jsRoot;
	Json::Value  jsAutoPwd;

	nRet = ReadLoginInfo(strLoginInfo);
	if(strLoginInfo.empty()) return nRet;

	bRet = jsReader.parse(strLoginInfo, jsRoot);
	if(!bRet) return 1;

	if(jsRoot.isMember("autopwd"))
	{
		jsAutoPwd             = jsRoot["autopwd"];

		if(tstrAccount.empty() && 
			jsAutoPwd.size() > 0)
		{
			Json::Value jsAccount0 = jsAutoPwd[Json::Value::UInt(0)];
			tstrAccount  = LPTSTR(CA2T(jsAccount0["account"].asString().c_str()));
			tstrPassword = LPTSTR(CA2T(jsAccount0["pwd"].asString().c_str()));
			return 0;
		}

		std::string strAccout = LPSTR(CT2A(tstrAccount.c_str()));
		for(Json::Value::iterator it = jsAutoPwd.begin(); 
			it != jsAutoPwd.end(); it++)
		{
			if(::strcmp((*it)["account"].asString().c_str(), 
				strAccout.c_str()) == 0)
			{
				tstrPassword = LPTSTR(CA2T((*it)["pwd"].asString().c_str()));
				return 0;
			}
		}
	}

	return 1;
}

int     CZiLogin::UpdateLoginInfo(CDuiString & dstrAccount, CDuiString & dstrPassword)
{
	Assert(m_nLoginType != m_nLastLoginType);

	int  nRet  = 0;
	int  nType = 0;

	// add. 
	if((m_nLoginType & Type_LoginAutoPassword) && !( 
		m_nLastLoginType & Type_LoginAutoPassword))
	{
		nType |= Type_LoginAutoPassword;
	}

	if((m_nLoginType & Type_LoginAutoLogin) && !(
		m_nLastLoginType & Type_LoginAutoLogin))
	{
		nType |= Type_LoginAutoLogin;
	}

	if(nType)
	{
		nRet  = UpdateLoginInfoImp(Type_LoginAddInfo, nType, dstrAccount, dstrPassword);
		nType = 0;
	}

	// sub
	if(!(m_nLoginType & Type_LoginAutoPassword) && (
		m_nLastLoginType & Type_LoginAutoPassword))
	{
		nType |= Type_LoginAutoPassword;
	}

	if(!(m_nLoginType & Type_LoginAutoLogin)   && (
		m_nLastLoginType & Type_LoginAutoLogin))
	{
		nType |= Type_LoginAutoLogin;
	}

	if(nType)
	{
		nRet = UpdateLoginInfoImp(Type_LoginSubInfo, nType, dstrAccount, dstrPassword);
	}

	return nRet;
}

int     CZiLogin::UpdateLoginInfoImp(int nActionType, int nLoginType, CDuiString & dstrAccount, CDuiString & dstrPassword)
{
	Assert(nLoginType & Type_LoginAutoLogin || 
		nLoginType & Type_LoginAutoPassword);
	Assert(!dstrAccount.IsEmpty() && !dstrPassword.IsEmpty());

	int         nRet = 0;
	std::string strLoginInfo;
	Json::Value jsRoot;
	Json::Value jsAutoPwd;
	Json::Value jsAutoLogin;
	Json::Value jsLoginInfo;

	jsLoginInfo["account"] = (LPSTR)CT2A(dstrAccount.GetData());
	jsLoginInfo["pwd"]     = (LPSTR)CT2A(dstrPassword.GetData());

	nRet = ReadLoginInfo(strLoginInfo);
	if(!strLoginInfo.empty())
	{
		bool         bRet = false;
		Json::Reader jsReader;

		bRet = jsReader.parse(strLoginInfo, jsRoot);
		if(!bRet) return Error_Fail;

		if(jsRoot.isMember("autopwd"))
		{
			jsAutoPwd = jsRoot["autopwd"];
		}

		if(jsRoot.isMember("autologin"))
		{
			jsAutoLogin = jsRoot["autologin"];
		}
	}

	if(nLoginType & Type_LoginAutoLogin)
	{
		if(nActionType == Type_LoginAddInfo)
		{
			jsRoot["autologin"] = jsLoginInfo;
		}

		if(nActionType == Type_LoginSubInfo)
		{
			if(jsAutoLogin == jsLoginInfo)
			{
				jsRoot.removeMember("autologin");
			}
		}
	}

	if(nLoginType & Type_LoginAutoPassword)
	{
		BOOL bFind  = FALSE;
		int  nIndex = 0;

		for(Json::Value::iterator it = jsAutoPwd.begin(); 
			it != jsAutoPwd.end(); it++, nIndex++)
		{
			if((*it) == (jsLoginInfo))
			{
				bFind = TRUE;
				break;
			}
		}

		if(nActionType == Type_LoginAddInfo)
		{
			if(!bFind) jsAutoPwd.append(jsLoginInfo);
			else       return 0;
		}

		if(nActionType == Type_LoginSubInfo)
		{
			if(bFind)  JsonArrayErea(jsAutoPwd, nIndex)
			else       return 0;
		}

		jsRoot["autopwd"] = jsAutoPwd;
	}

	// 测试时判断已被加入. ???
	jsRoot["logintype"] = m_nLoginType;
	nRet = WriteLoginInfo(jsRoot.toStyledString());
	return nRet;
}

int     CZiLogin::ReadLoginInfo(std::string & strLoginInfo)
{
	std::string strTmp;
	std::string strLoginFile  = GetLoginDataFolder();
	strLoginFile             += g_szLoginFile;

	FILE * pf = ::fopen(strLoginFile.c_str(), "rb");
	if(!pf) return 1;

	::fseek(pf, 0, SEEK_END);
	int nFileLen = ::ftell(pf);
	::fseek(pf, 0, SEEK_SET);
	if(nFileLen > 0) 
	{
		strTmp.resize(nFileLen);
		int nReaded = ::fread(&strTmp[0], 1, nFileLen, pf);
		if(nReaded != nFileLen) strTmp.clear();
	}

	::fclose(pf);

	if(!strTmp.empty())
	{
		CZiDataCoder::Decrypt(strTmp.c_str(), strLoginInfo);
	}

	return !strLoginInfo.empty() ? 0 : 2;
}

int     CZiLogin::WriteLoginInfo(std::string & strLoginInfo)
{
	Assert(!strLoginInfo.empty());

	std::string  strTmp;
	CZiDataCoder::Encrypt(strLoginInfo.c_str(), strTmp);
	if(strTmp.empty()) return 2;

	std::string  strLoginFile  = GetLoginDataFolder();
	strLoginFile              += g_szLoginFile;

	FILE * pf = ::fopen(strLoginFile.c_str(), "wb");
	if(!pf) return 1;

	::fwrite(strTmp.c_str(), 1, strTmp.length(), pf);
	::fflush(pf);
	::fclose(pf);
	return 0;
}

LPCSTR  CZiLogin::GetLoginDataFolder()
{
	static std::string s_strLoginPath;

	if(s_strLoginPath.empty())
	{
		char szModule[MAX_PATH + 1] = { 0 };
		::GetModuleFileNameA(CPaintManagerUI::GetInstance(), szModule, MAX_PATH);
		char * pEnd = ::strrchr(szModule, '\\');
		if(pEnd) { pEnd[1] = 0; s_strLoginPath = szModule; }
	}

	return s_strLoginPath.c_str();
}

int     CZiLogin::NetworkLogin(LPCTSTR lpctsAccount, LPCTSTR lpctsPassword, tstring & tstrLoginRet)
{
	Assert(0);

	// packet 然后 send. 
	// ...

	//Sleep(10000);
	return 0;
}


// ------------------------------------------------------------------------
// 
int     CZiLogin::AsyncLogin(LPCTSTR lpctsAccount, LPCTSTR lpctsPassword)
{
	int nRet = 0;
	LoginCsData_t loginQuery;
	CT2A pAData1(lpctsAccount);
	CT2A pAData2(lpctsPassword);
	loginQuery.szAccount  = pAData1;
	loginQuery.szPassword = pAData2;

	Assert(m_pLoginMsger);
	nRet = m_pLoginMsger->SendImMessage(Msg_CsLogin, (LPARAM)&loginQuery, sizeof(loginQuery));

	if(nRet) AsyncLoginReturn(nRet, 0);
	return nRet;
}

int     CZiLogin::AsyncLoginReturn(int nLoginResult, LoginScData_t * pLoginResult)
{
	int nRet = 0;
	CEditUI * pAccountEdit  = DuiControl(CEditUI, AccountControlName());
	CEditUI * pPasswordEdit = DuiControl(CEditUI, PasswordControlName());

	// 1 等待 Login 初始化完成
	// 暂时不需要. 退出不需要等待界面 ******

	// 2 处理失败消息. 
	if(nLoginResult)
	{
		int nError = nLoginResult > Error_Base ? nLoginResult : (
			nLoginResult == 1 ? Error_LoginDuplication : Error_LoginAccountOrPasswordError);
		CZimcHelper::ErrorMessageBox(m_hWnd, Msg_ScLogin, nError);

		// 设置为激活状态. 
		pAccountEdit ->SetEnabled(true);
		pPasswordEdit->SetEnabled(true);
		return nError;
	}

	Assert(pLoginResult);

	// 3 处理成功消息. 
	// 3.1 更新 Login 数据
	if(m_nLoginType != m_nLastLoginType)
	{
		nRet = UpdateLoginInfo(pAccountEdit->GetText(), pPasswordEdit->GetText());
	}

	//// 3.2 销毁登陆界面
	//Close();

	// 3.3 显示主界面
	CZiMainFrame * pMainFrame = new CZiMainFrame;
	if(!pMainFrame) return Error_Fail;

	pMainFrame->Create(0, _T("主界面"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW);
	pMainFrame->SetWindowPosition();
	pMainFrame->SetMainInfo(pLoginResult);
	pMainFrame->SetState(State_MainRunning);
	pMainFrame->ShowWindow(true);

	return nRet;
}

LRESULT CZiLogin::HandleCustomMessage(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int nRet = 0;

	switch(nMsg)
	{
	case Msg_ScLogin: 
		Assert(lParam);
		nRet = AsyncLoginReturn(((LoginScData_t*)lParam)->nLoginStatus, (LoginScData_t*)lParam);

		Assert(m_pLoginMsger);
		m_pLoginMsger->FreeDataEx(Msg_ScLogin, (void*)lParam);

		if(!nRet) Close();
		return 0;
	}

	return S_OK;
}


// zimc.dat
// logintype = 0x0, 正常登陆. 
//             0x1, 记住密码. 
//             0x2, 自动登陆. 
//             备注: 自动登陆会记住密码. 
// account   = string.  加密字符串. 
// pwd       = string,  加密字符串. 
// zimc      = version, 客户端信息
// 
// json 格式. 
// { 
//    "logintype" = int. 
//    "autopwd" = 
//    [
//      { "account" = "", "pwd" = "" }, 
//      ... ...
//    ]
//    "autologin" = 
//    { 
//      "account" = "", "pwd" = ""
//    }
//    "zimcinfo"  = 
//    {
//      "version" = "", 
//    }
// }
