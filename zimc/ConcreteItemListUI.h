#ifndef __ConcreteItemListUI_H__
#define __ConcreteItemListUI_H__


#include "BaseItemListUI.h"


class CFirendItemUI : public CBaseItemListUI
{
public:
	CFirendItemUI(CPaintManagerUI & pm)
		: CBaseItemListUI(pm)
	{}

	~CFirendItemUI()
	{}
};


class CGroupItemUI : public CBaseItemListUI
{
public:
	CGroupItemUI(CPaintManagerUI & pm)
		: CBaseItemListUI(pm)
	{}

	~CGroupItemUI()
	{}
};


class CNewUserItemUI : public CBaseItemListUI
{
public:
	CNewUserItemUI(CPaintManagerUI & pm)
		: CBaseItemListUI(pm)
	{}

	~CNewUserItemUI()
	{}

	LPCTSTR GetSkinFile()
	{
		return _T("zinewuser_list_item.xml");
	}

	bool SetItemDescriptText(CNodeList * pNode)
	{
		Assert(pNode);
		NodeData_t & itemNode = pNode->GetNodeData();

		//Assert(itemNode.chType == Type_ImcFriend);
		//Assert(!itemNode.bIsFolder && !itemNode.bIsHasChild);
		Assert(itemNode.nId > 0);

		TCHAR stBuf[MAX_PATH] = {0};
		::_stprintf_s(stBuf, MAX_PATH - 1, _T("( %u )"), itemNode.nId);

		CLabelUI * pAccountUi = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(
			itemNode.pListElement, _T("Account")));
		Assert(pAccountUi);

		//pAccountUi->SetShowHtml(true);
		//pAccountUi->SetText(stBuf);
		return true;
	}
};


class CGroupFriendItemUI : public CBaseItemListUI
{
public:
	CGroupFriendItemUI(CPaintManagerUI & pm)
		: CBaseItemListUI(pm)
	{
		m_nItemNormalHeight       = 20;
		m_nItemLevelStartPosition = 5;
		m_rcTextPadding.left      = 5;
	}

	~CGroupFriendItemUI()
	{}

	LPCTSTR GetSkinFile()
	{
		return _T("zigroup_friend_list_item.xml");
	}
}; 


class CSearchFriendItemUI : public CBaseItemListUI
{
public:
	CSearchFriendItemUI(CPaintManagerUI & pm)
		: CBaseItemListUI(pm)
	{}

	~CSearchFriendItemUI()
	{}

	LPCTSTR GetSkinFile()
	{
		return _T("zisearch_friend_list_item.xml");
	}
};


#endif
