#ifndef __BaseItemListUI_H__
#define __BaseItemListUI_H__


#include "UiData.h"
#include "NodeList.h"


class CBaseItemListUI : public CListUI
{
public:
	CBaseItemListUI(CPaintManagerUI & pm);
	virtual ~CBaseItemListUI();

	bool Add(CControlUI * pControl);
	bool AddAt(CControlUI * pControl, int nIndex);
	bool Remove(CControlUI * pControl);
	bool RemoveAt(int nIndex);
	void RemoveAll();

	CNodeList * GetRoot();
	CNodeList * AddNode(const ItemNodeInfo_t & item, CNodeList * parent = NULL);
	bool RemoveNode(CNodeList * node);

	virtual bool CanExpand(CNodeList * node) const;
	virtual void SetChildVisible(CNodeList * node, bool visible);
	virtual bool SelectItem(int iIndex, bool bTakeFocus = false);
	virtual void DoEvent(TEventUI & ev);


protected:
	virtual LPCTSTR GetSkinFile();

	virtual bool SetItemText(CNodeList * pNode);
	virtual bool SetItemNickNameText(CNodeList * pNode);
	virtual bool SetItemDescriptText(CNodeList * pNode);


protected:
	enum { SCROLL_TIMERID = 10, };

	CNodeList       * root_node_;
	LONG              delay_deltaY_;
	DWORD             delay_number_;
	DWORD             delay_left_;

	tstring           level_expand_image_;
	tstring           level_collapse_image_;

	CPaintManagerUI & paint_manager_;
    CDialogBuilder    m_dlgBuilder;

	CDuiRect          m_rcTextPadding;
	int               m_nItemLevelStartPosition;
	int               m_nItemNormalHeight;
};


#endif // __UiFriends_H__
