#include "stdafx.h"
#include "BaseItemListUI.h"


const int kFriendListItemNormalHeight = 32;
const int kFriendListItemSelectedHeight = 40;


CBaseItemListUI::CBaseItemListUI(CPaintManagerUI & pm)
	: root_node_(NULL)
	, delay_deltaY_(0)
	, delay_number_(0)
	, delay_left_(0)
	, level_expand_image_(_T("<i list_icon_b.png>"))
	, level_collapse_image_(_T("<i list_icon_a.png>"))
	, m_nItemLevelStartPosition(10)
	, m_rcTextPadding(10, 0, 0, 0)
	, paint_manager_(pm)
	, m_nItemNormalHeight(kFriendListItemNormalHeight)
{
	SetItemShowHtml(true);

	root_node_ = new CNodeList;
	root_node_->GetNodeData().nLevel          = -1;
	root_node_->GetNodeData().bIsChildVisible = true;
	root_node_->GetNodeData().bIsHasChild     = true;
	root_node_->GetNodeData().pListElement    = NULL;
}

CBaseItemListUI::~CBaseItemListUI()
{
	if (root_node_)
		delete root_node_;

	root_node_ = NULL;
}

bool CBaseItemListUI::Add(CControlUI * pControl)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::Add(pControl);
}

bool CBaseItemListUI::AddAt(CControlUI * pControl, int iIndex)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::AddAt(pControl, iIndex);
}

bool CBaseItemListUI::Remove(CControlUI* pControl)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	if (reinterpret_cast<CNodeList*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()) == NULL)
		return CListUI::Remove(pControl);
	else
		return RemoveNode(reinterpret_cast<CNodeList*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()));
}

bool CBaseItemListUI::RemoveAt(int iIndex)
{
	CControlUI* pControl = GetItemAt(iIndex);
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	if (reinterpret_cast<CNodeList*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()) == NULL)
		return CListUI::RemoveAt(iIndex);
	else
		return RemoveNode(reinterpret_cast<CNodeList*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()));
}

void CBaseItemListUI::RemoveAll()
{
	CListUI::RemoveAll();
	for (int i = 0; i < root_node_->GetChildrenSize(); ++i)
	{
		CNodeList* GetChild = root_node_->GetChild(i);
		RemoveNode(GetChild);
	}
	delete root_node_;

	root_node_ = new CNodeList;
	root_node_->GetNodeData().nLevel          = -1;
	root_node_->GetNodeData().bIsChildVisible = true;
	root_node_->GetNodeData().bIsHasChild     = true;
	root_node_->GetNodeData().pListElement    = NULL;
}

void CBaseItemListUI::DoEvent(TEventUI& event) 
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
	{
		if (m_pParent != NULL)
			m_pParent->DoEvent(event);
		else
			CVerticalLayoutUI::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_TIMER && event.wParam == SCROLL_TIMERID)
	{
		if (delay_left_ > 0)
		{
			--delay_left_;
			SIZE sz = GetScrollPos();
			LONG lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
			if ((lDeltaY > 0 && sz.cy != 0)  || (lDeltaY < 0 && sz.cy != GetScrollRange().cy ))
			{
				sz.cy -= lDeltaY;
				SetScrollPos(sz);
				return;
			}
		}
		delay_deltaY_ = 0;
		delay_number_ = 0;
		delay_left_ = 0;
		m_pManager->KillTimer(this, SCROLL_TIMERID);
		return;
	}
	if (event.Type == UIEVENT_SCROLLWHEEL)
	{
		LONG lDeltaY = 0;
		if (delay_number_ > 0)
			lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
		switch (LOWORD(event.wParam))
		{
		case SB_LINEUP:
			if (delay_deltaY_ >= 0)
				delay_deltaY_ = lDeltaY + 8;
			else
				delay_deltaY_ = lDeltaY + 12;
			break;
		case SB_LINEDOWN:
			if (delay_deltaY_ <= 0)
				delay_deltaY_ = lDeltaY - 8;
			else
				delay_deltaY_ = lDeltaY - 12;
			break;
		}
		if
			(delay_deltaY_ > 100) delay_deltaY_ = 100;
		else if
			(delay_deltaY_ < -100) delay_deltaY_ = -100;

		delay_number_ = (DWORD)sqrt((double)abs(delay_deltaY_)) * 5;
		delay_left_ = delay_number_;
		m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
		return;
	}

	CListUI::DoEvent(event);
}

CNodeList* CBaseItemListUI::GetRoot()
{
	return root_node_;
}

LPCTSTR    CBaseItemListUI::GetSkinFile()
{
	return _T("ziuser_list_item.xml");
}


const TCHAR* const kLogoButtonControlName     = _T("logo");
const TCHAR* const kLogoContainerControlName  = _T("logo_container");
const TCHAR* const kNickNameControlName       = _T("nickname");
const TCHAR* const kDescriptionControlName    = _T("description");
const TCHAR* const kOperatorPannelControlName = _T("operation");


static bool OnLogoButtonEvent(void* event) {
    if( ((TEventUI*)event)->Type == UIEVENT_BUTTONDOWN ) {
        CControlUI* pButton = ((TEventUI*)event)->pSender;
        if( pButton != NULL ) {
            CListContainerElementUI* pListElement = (CListContainerElementUI*)(pButton->GetTag());
            if( pListElement != NULL ) pListElement->DoEvent(*(TEventUI*)event);
        }
    }
    return true;
}

CNodeList* CBaseItemListUI::AddNode(const ItemNodeInfo_t& item, CNodeList* parent)
{
	if (!parent)
		parent = root_node_;

    CListContainerElementUI* pListElement = NULL;
    if( !m_dlgBuilder.GetMarkup()->IsValid() ) {
        pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(GetSkinFile(), (UINT)0, NULL, &paint_manager_));
    }
    else {
        pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((UINT)0, &paint_manager_));
    }
    if (pListElement == NULL)
        return NULL;

	CNodeList* node = new CNodeList;

	// visible 由外部控制. 
	node->GetNodeData().Assign(item);
	node->GetNodeData().nLevel          = parent->GetNodeData().nLevel + 1;
	//node->GetNodeData().bIsChildVisible = node->GetNodeData().nLevel == 0;
	node->GetNodeData().bIsHasChild     = item.bIsFolder ? item.bIsHasChild : false;
	node->GetNodeData().pListElement    = pListElement;

	if (!parent->GetNodeData().bIsChildVisible)
		pListElement->SetVisible(false);

	if (parent != root_node_ && !parent->GetNodeData().pListElement->IsVisible())
		pListElement->SetVisible(false);

	// padding. 每一层不同. 
	CDuiRect rcPadding = m_rcTextPadding;
	for (int i = 0; i < node->GetNodeData().nLevel; ++i)
	{
		rcPadding.left += m_nItemLevelStartPosition;		
	}
	pListElement->SetPadding(rcPadding);

	// 设置 Item 的显示内容. Virual 化后子类可以修改. 
	SetItemText(node);

	pListElement->SetFixedHeight(m_nItemNormalHeight);
	pListElement->SetTag((UINT_PTR)node);
	int index = 0;
	if (parent->IsHasChildren())
	{
		CNodeList* prev = parent->GetLastChild();
		index = prev->GetNodeData().pListElement->GetIndex() + 1;
	}
	else 
	{
		if (parent == root_node_)
			index = 0;
		else
			index = parent->GetNodeData().pListElement->GetIndex() + 1;
	}

	if (!CListUI::AddAt(pListElement, index))
	{
		delete pListElement;
		delete node;
		node = NULL;
	}
	else {
		parent->AddChild(node);
	}
	return node;
}

bool CBaseItemListUI::RemoveNode(CNodeList* node)
{
	if (!node || node == root_node_) return false;

	// 递归时会改变 node, 而 i 值不变. 
	//for (int i = 0; i < node->GetChildrenSize();; ++i)
	//{
	//	CNodeList* GetChild = node->GetChild(i);
	//	RemoveNode(GetChild);
	//}

	while(node->GetChildrenSize() > 0)
	{
		CNodeList * Child = node->GetChild(0);
		RemoveNode(Child);
	}

	CListUI::Remove(node->GetNodeData().pListElement);
	node->GetParent()->RemoveChild(node);
	delete node;

	return true;
}

void CBaseItemListUI::SetChildVisible(CNodeList* node, bool visible)
{
	if (!node || node == root_node_)
		return;

	if (node->GetNodeData().bIsChildVisible == visible)
		return;

	node->GetNodeData().bIsChildVisible = visible;

	// 由于 DIR 类型的ITEM 在展开和隐藏时的 logo 要发生改变. 
	// 因此在此处增加
	SetItemNickNameText(node);

	if (!node->GetNodeData().pListElement->IsVisible())
		return;

	if (!node->IsHasChildren())
		return;

	CNodeList* begin = node->GetChild(0);
	CNodeList* end = node->GetLastChild();
	for (int i = begin->GetNodeData().pListElement->GetIndex(); i <= end->GetNodeData().pListElement->GetIndex(); ++i)
	{
		CControlUI* control = GetItemAt(i);
		if (_tcsicmp(control->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			if (visible) 
			{
				CNodeList* local_parent = ((CNodeList*)control->GetTag())->GetParent();
				if (local_parent->GetNodeData().bIsChildVisible && local_parent->GetNodeData().pListElement->IsVisible())
				{
					control->SetVisible(true);
				}
			}
			else
			{
				control->SetVisible(false);
			}
		}
	}
}

bool CBaseItemListUI::CanExpand(CNodeList* node) const
{
	if (!node || node == root_node_ || node->GetNodeData().Type() == Type_ImcGroup || node->GetNodeData().Type() == Type_ImcTalk)
		return false;

	return node->GetNodeData().bIsHasChild;
}

bool CBaseItemListUI::SelectItem(int iIndex, bool bTakeFocus)
{
	// 此函数当鼠标点击是, 显示和隐藏好友的下拉操作按钮 (多用于附加功能: 例如短信等). 
	// 可以暂时不用. 

    if( iIndex == m_iCurSel ) return true;

    // We should first unselect the currently selected item
    if( m_iCurSel >= 0 ) {
        CControlUI* pControl = GetItemAt(m_iCurSel);
        if( pControl != NULL) {
            IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
            if( pListItem != NULL )
			{
				//CListContainerElementUI* pFriendListItem = static_cast<CListContainerElementUI*>(pControl);
				//CNodeList* node = (CNodeList*)pControl->GetTag();
				//if ((pFriendListItem != NULL) && (node != NULL) && !node->IsFolder())
				//{
				//	pFriendListItem->SetFixedHeight(kFriendListItemNormalHeight);
				//	CContainerUI* pOperatorPannel = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pFriendListItem, kOperatorPannelControlName));
				//	if (pOperatorPannel != NULL)
				//	{
				//		pOperatorPannel->SetVisible(false);
				//	}
				//}
				pListItem->Select(false);
			}
        }

        m_iCurSel = -1;
    }

    if( iIndex < 0 )
		return false;

	if (!__super::SelectItem(iIndex, bTakeFocus))
		return false;


	//CControlUI* pControl = GetItemAt(m_iCurSel);
	//if( pControl != NULL)
	//{
	//	CListContainerElementUI* pFriendListItem = static_cast<CListContainerElementUI*>(pControl);
	//	CNodeList* node = (CNodeList*)pControl->GetTag();
	//	if((pFriendListItem != NULL) && (node != NULL) && !node->IsFolder())
	//	{
	//		pFriendListItem->SetFixedHeight(kFriendListItemSelectedHeight);
	//		CContainerUI* pOperatorPannel = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pFriendListItem, kOperatorPannelControlName));
	//		if (pOperatorPannel != NULL)
	//		{
	//			pOperatorPannel->SetVisible(true);
	//		}
	//	}
	//}
	return true;
}

bool CBaseItemListUI::SetItemText(CNodeList * pNode)
{
	SetItemNickNameText(pNode);
	SetItemDescriptText(pNode);

	return true;
}

bool CBaseItemListUI::SetItemNickNameText(CNodeList * pNode)
{
	Assert(pNode);
	tstring      strHtmlText;
	NodeData_t & itemNode = pNode->GetNodeData();

	// Team or Group' logo
	if(itemNode.bIsHasChild)
	{
		if(itemNode.bIsChildVisible)
			strHtmlText += level_expand_image_;
		else
			strHtmlText += level_collapse_image_;

		// 增加 logo 和 nick 之间的间隔距离
		TCHAR szBuf[MAX_PATH] = {0};
		_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), m_nItemLevelStartPosition);
		strHtmlText += szBuf;
	}

	// nick name
	strHtmlText += itemNode.tstrNickName;

	CLabelUI* tstrNickName = static_cast<CLabelUI*>(
		paint_manager_.FindSubControlByName(itemNode.pListElement, kNickNameControlName));
	if (tstrNickName != NULL)
	{
		if (itemNode.bIsFolder)
			tstrNickName->SetFixedWidth(0);

		tstrNickName->SetShowHtml(true);
		tstrNickName->SetText(strHtmlText.c_str());
	}

	return true;
}

bool CBaseItemListUI::SetItemDescriptText(CNodeList * pNode)
{
	Assert(pNode);
	NodeData_t & itemNode = pNode->GetNodeData();

	if (!itemNode.bIsFolder && 
		!itemNode.tstrDescription.empty())
	{
		CLabelUI* description = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(
			itemNode.pListElement, kDescriptionControlName));
		if (description != NULL)
		{
			// 颜色由 xml 控制. 
			//description->SetShowHtml(true);
			description->SetText(itemNode.tstrDescription.c_str());
		}
	}

	return true;
}
