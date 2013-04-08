#include "stdafx.h"
#include "CreateGroupWindow.h"


CCreateGroupWindow::CCreateGroupWindow(CZiMainFrame* pMainWindow,int type,char *group_name)
	: m_pMainWindow(pMainWindow),
	m_nType(type),
	m_chName(group_name)
{}

CCreateGroupWindow::~CCreateGroupWindow()
{}


LPCTSTR CCreateGroupWindow::GetWindowClassName() const
{
	return _T("GreateGroupWnd");
}

tstring CCreateGroupWindow::GetSkinFile()
{
	return _T("zicreategroup.xml");
}

LRESULT CCreateGroupWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT res = CDuiWindowBase::OnCreate(uMsg, wParam, lParam, bHandled);
	if (res == S_OK) {
		if (m_nType == 1) {
			CLabelUI *pLabelTitle = DuiControl(CLabelUI, _T("TitleLabel1"));
			CLabelUI *pLabelName  = DuiControl(CLabelUI, _T("TitleLabel2"));
			CLabelUI *pEdit = DuiControl(CEditUI, _T("GroupName"));
			Assert(pLabelTitle);
			Assert(pLabelName);
			pLabelTitle->SetText(_T("修改群名称"));
			pLabelName->SetText(_T("请在下面修改群名称"));
			if (m_chName) {
				pEdit->SetText(CA2T(m_chName));
				pEdit->SetFocus();
			}
		}
	}
	return res;
}

void    CCreateGroupWindow::Notify(TNotifyUI & msg)
{
	if(msg.sType == _T("click"))
	{
		DuiClickButtonMap(_T("CloseBtn"),   OnExit);
		DuiClickButtonMap(_T("BtnCancel"),  OnExit);
		DuiClickButtonMap(_T("BtnOk"),  OnCreateGroup);
	}
}

void    CCreateGroupWindow::OnFinalMessage(HWND hWnd)
{
	if (m_nType) {
		m_pMainWindow->m_pModifyGroupWindow = 0;
	}
	else {
		m_pMainWindow->m_pCreateGroupWindow = 0;
	}

	CDuiWindowBase::OnFinalMessage(hWnd);
	delete this;
}

int    CCreateGroupWindow::OnExit(TNotifyUI & msg)
{
	Close();
	return 0;
}

int    CCreateGroupWindow::OnCreateGroup(TNotifyUI & msg)
{
	// 获取 
	CEditUI     * pGroupNameEdit  = DuiControl(CEditUI, _T("GroupName"));

	Assert(pGroupNameEdit);

	DuiLib::CDuiString & groupname  = pGroupNameEdit->GetText();

	if(groupname.IsEmpty() || groupname.GetLength() == 0 )
	{
		::MessageBox(m_hWnd, _T("群的名称不能为空"), _T("错误"), 0);
		return -1;
	}

	CT2A pAData1(groupname);
	GroupInfoData_t groupinfo;
	groupinfo.strSender = CT2A(m_pMainWindow->GetSelfInfo()->tstrNickName.c_str());
	groupinfo.nSender = IdLocalToNet(Type_ImcFriendX, m_pMainWindow->GetSelfInfo()->nId);
	groupinfo.groupinfo.name = pAData1;
	m_pMainWindow->SendImMessageX(Msg_CsCreateGroup, (LPARAM)&groupinfo, sizeof(groupinfo));
	
	// 需要关闭吗. ?
	OnExit(msg);
	return 0;
}