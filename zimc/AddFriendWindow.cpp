#include "stdafx.h"
#include "AddFriendWindow.h"
#include "MainWindow.h"


CAddFriendWindow::CAddFriendWindow(
	CZiMainFrame * pMainWnd, ItemNodeInfo_t * pUserInfo, CNodeList * pParentNode)
	: m_pMainWindow(pMainWnd)
	, m_pParentNode(pParentNode)
	, m_pUserInfo(pUserInfo)
{}

CAddFriendWindow::~CAddFriendWindow()
{}


LPCTSTR CAddFriendWindow::GetWindowClassName() const
{
	return _T("AddFriendWnd");
}

tstring CAddFriendWindow::GetSkinFile() 
{
	return tstring(_T("ziaddfriend.xml"));
}

void    CAddFriendWindow::Notify(TNotifyUI & msg)
{
	if(_tcsicmp(msg.sType, _T("click")) == 0)
	{
		DuiClickButtonMap(_T("CloseBtn"),     OnExit);
		DuiClickButtonMap(_T("CloseBtn2"),    OnExit);
		DuiClickButtonMap(_T("AddFriendBtn"), OnAddFriend);
	}
}

void    CAddFriendWindow::OnFinalMessage(HWND hWnd)
{
	CDuiWindowBase::OnFinalMessage(hWnd);	
	delete this;
}

LRESULT CAddFriendWindow::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CAddFriendWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	CDuiWindowBase::OnCreate(uMsg, wParam, lParam, bHandled);

	// 修改"用户信息": 
	// NickNameLabel, AccountLabel, SexLabel, AgeLabel, AddressLabel. 
	CLabelUI * pNickNameUi = DuiControl(CLabelUI, _T("NickNameLabel"));
	CLabelUI * pAccountUi  = DuiControl(CLabelUI, _T("AccountLabel"));
	CLabelUI * pSexUi      = DuiControl(CLabelUI, _T("SexLabel"));
	CLabelUI * pAgeUi      = DuiControl(CLabelUI, _T("AgeLabel"));
	CLabelUI * pAddressUi  = DuiControl(CLabelUI, _T("AddressLabel"));

	Assert(m_pUserInfo);
	Assert(pNickNameUi);
	Assert(pAccountUi);
	Assert(pSexUi);
	Assert(pAgeUi);
	Assert(pAddressUi);

	pNickNameUi->SetText(m_pUserInfo->tstrNickName.c_str());

	char szTmp[256] = {0};
	//sprintf_s(szTmp, sizeof(szTmp) - 1, "%d", m_pUserInfo->nId);	// ???
	//pAccountUi->SetText(CA2T(szTmp));

	sprintf_s(szTmp, sizeof(szTmp) - 1, "性别: %s", CT2A(m_pUserInfo->tstrSex.c_str()));
	pSexUi->SetText(CA2T(szTmp));

	sprintf_s(szTmp, sizeof(szTmp) - 1, "年龄: %d", m_pUserInfo->nAge);
	pAgeUi->SetText(CA2T(szTmp));

	sprintf_s(szTmp, sizeof(szTmp) - 1, "地址: %s", CT2A(m_pUserInfo->tstrAddress.c_str()));
	pAddressUi->SetText(CA2T(szTmp));

	return S_OK;
}

int     CAddFriendWindow::OnExit(TNotifyUI & msg)
{
	Close();
	return 0;
}

int     CAddFriendWindow::OnAddFriend(TNotifyUI & msg)
{
	// Assert(m_pParentNode);		// 默认为我的好友
	Assert(m_pMainWindow);
	Assert(m_pUserInfo);
	//Assert(m_pUserInfo->Type() == Type_ImcFriend);
	if (m_pUserInfo->Type() != Type_ImcFriend) {
		return addGroup(msg);
	}

	CEditUI   * pVerifyUi    = DuiControl(CEditUI,   _T("VerifyInfoEdit"));
	CButtonUI * pAddFriendUi = DuiControl(CButtonUI, _T("AddFriendBtn"));
	Assert(pVerifyUi && pAddFriendUi);

	// 首先判断是否已经是好友了
	if(!m_pMainWindow->IsUnknownFriend(m_pUserInfo->nId))
	{
		Close();
		return 0;
	}

	if(!pVerifyUi->GetText().GetData() || 
		pVerifyUi->GetText().GetLength() == 0)
	{
		MessageBox(m_hWnd, _T("验证信息为空"), _T("错误"), 0);
		return 0;
	}

	{
		Assert(m_pMainWindow && m_pMainWindow->GetSelfInfo());

		CT2A pszVerifyText(pVerifyUi->GetText().GetData());
		CT2A pszSenderName(m_pMainWindow->GetSelfInfo()->tstrNickName.c_str());
		CT2A pszRecverName(m_pUserInfo->tstrNickName.c_str());

		VerifyCcQuery_t verifyQuery;
		verifyQuery.nSenderId     = MagicId_F(Msg_CsQueryVerify, m_pMainWindow->GetSelfInfo()->nId);
		verifyQuery.nRecverId     = MagicId_t(Msg_CsQueryVerify, m_pUserInfo->chType, m_pUserInfo->nId);
		verifyQuery.szSenderName  = pszSenderName;
		verifyQuery.szRecverName  = pszRecverName;
		verifyQuery.szVerifyData  = pszVerifyText;
		verifyQuery.nRecvType     = m_pUserInfo->chType;
		verifyQuery.pSenderLocalQInfo = m_pMainWindow->GetNodeData(m_pMainWindow->GetSelfInfo()->nId);

		pVerifyUi->SetEnabled(false);
		pAddFriendUi->SetEnabled(false);
		m_pMainWindow->SendImMessageX(Msg_CsQueryVerify, (LPARAM)&verifyQuery, sizeof(verifyQuery));
	}

	Close();
	return 0;
}

int     CAddFriendWindow::addGroup(TNotifyUI & msg) {

	CEditUI   * pVerifyUi    = DuiControl(CEditUI,   _T("VerifyInfoEdit"));
	CButtonUI * pAddFriendUi = DuiControl(CButtonUI, _T("AddFriendBtn"));
	Assert(pVerifyUi && pAddFriendUi);

	// 首先判断是否已经是好友了
	if(!m_pMainWindow->IsUnknownGroup(m_pUserInfo->nId))
	{
		Close();
		return 0;
	}

	if(!pVerifyUi->GetText().GetData() || 
		pVerifyUi->GetText().GetLength() == 0)
	{
		MessageBox(m_hWnd, _T("验证信息为空"), _T("错误"), 0);
		return 0;
	}

	Assert(m_pMainWindow && m_pMainWindow->GetSelfInfo());
	AddGroupInfo_t addgroupinfo;
	CT2A pszVerifyText(pVerifyUi->GetText().GetData());
	CT2A pszSenderName(m_pMainWindow->GetSelfInfo()->tstrNickName.c_str());
	CT2A pszRecverName(m_pUserInfo->tstrNickName.c_str());

	addgroupinfo.groupinfo.group_id = IdLocalToNet(Type_ImcGroup, m_pUserInfo->nId);
	addgroupinfo.groupinfo.name = pszRecverName;
	addgroupinfo.nSenderId = IdLocalToNet(Type_ImcFriend, m_pMainWindow->GetSelfInfo()->nId);
	addgroupinfo.strSenderName = pszSenderName;
	addgroupinfo.strVerify = pszVerifyText;
	addgroupinfo.type = GROUP_INFO_VERIFY;
	//addgroupinfo.pSenderLocalQInfo = m_pMainWindow->GetNodeData(m_pMainWindow->GetSelfInfo()->nId);
	ItemDataLocatToNet(*(m_pMainWindow->GetNodeData(m_pMainWindow->GetSelfInfo()->nId)), addgroupinfo.userinfo);
	
	pVerifyUi->SetEnabled(false);
	pAddFriendUi->SetEnabled(false);

	m_pMainWindow->SendImMessageX(Msg_CsAddGroupVerify, (LPARAM)&addgroupinfo, sizeof(addgroupinfo));

	Close();
	return 0;
}
