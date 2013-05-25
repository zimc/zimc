#ifndef __LoginWindow_H__
#define __LoginWindow_H__


#include "DuiWindowBase.h"


class CLoginMsger;
class CZiLogin
	: public CDuiWindowBase
{
public:
	CZiLogin();
	~CZiLogin();

	int  Init();
	int  Uninit();
	BOOL IsExit();


private:
	// INotifyUI
	void    Notify(TNotifyUI & msg);

	// CWindowWnd
	LPCTSTR GetWindowClassName() const;
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	tstring GetSkinFile();

	// self. 
	int     OnWindowPush   (TNotifyUI & notifyUi);
	int     OnWindowUp     (TNotifyUI & notifyUi);

	int     OnSelectAcount (TNotifyUI & notifyUi);
	int     OnFocusAccount (TNotifyUI & notifyUi);
	int     OnFocusPwd     (TNotifyUI & notifyUi);
	int     OnClickLogOn   (TNotifyUI & notifyUI);
	int     OnReturnAccount(TNotifyUI & notifyUi);
	int     OnReturnPwd    (TNotifyUI & notifyUi);
	int     OnClickAccount (TNotifyUI & notifyUi);
	int     OnClickPwd     (TNotifyUI & notifyUi);

	int     OnLogin        (TNotifyUI & notifyUi);
	int     OnRegister     (TNotifyUI & notifyUi);
	int     OnFindPwd      (TNotifyUI & notifyUi);
	int     OnSavePwd      (TNotifyUI & notifyUi);
	int     OnAutoLogin    (TNotifyUI & notifyUi);


private:
	// 数据交互. 
	int     GetCurrentLoginInfo(CDuiString & dstrAccount, CDuiString & dstrPassword);
	int     UpdateLoginInfo(CDuiString & dstrAccount, CDuiString & dstrPassword);
	int     SyncNormalLogin(LPCTSTR lpctsAccount, LPCTSTR lpctsPassword);
	int     AutoLogin(LPCTSTR lpctsAccount, LPCTSTR lpctsPassword);
	int     OpenIE(LPCTSTR lpctsUrl);
	int     GetAutoLogin(int & nLoadType, tstring & tstrAccount, tstring & tstrPassword);
	int     GetAutoPassword(tstring & tstrAccount, tstring & tstrPassword);

	LPCSTR  GetLoginDataFolder();
	int     ReadLoginInfo(std::string & strLoginInfo);
	int     WriteLoginInfo(std::string & strLoginInfo);
	int     UpdateLoginInfoImp(int nActionType, int nLoginType, CDuiString & dstrAccount, CDuiString & dstrPassword);


private:
	LPCTSTR AccountControlName(int nIndex = -1);
	LPCTSTR PasswordControlName(int nIndex = -1);
	int     ChangeWindow(int nOld, int nNew);

	
private:
	// net work. 
	// 异步消息. ( 登陆, 及其结果 )
	int     AsyncLogin(LPCTSTR lpctsAccount, LPCTSTR lpctsPassword);
	int     AsyncLoginReturn(int nLoginResult, LoginScData_t * stData);
	int     NetworkLogin(LPCTSTR lpctsAccount, LPCTSTR lpctsPassword, tstring & tstrLoginRet);


private:
	enum  { Type_LoginAddInfo = 0x0, Type_LoginSubInfo, };
	enum  { Type_LoginDefault = 0x0, Type_LoginAutoPassword = 0x01, Type_LoginAutoLogin = 0x2, };

	int     m_nLastLoginType;
	int     m_nLoginType;
	int     m_bIsExit;

	int     m_nWindowIndex;

	std::string   m_strAccount;
	CLoginMsger * m_pLoginMsger;
};


#endif
