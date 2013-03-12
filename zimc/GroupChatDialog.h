#ifndef __GroupChatDialog_H__
#define __GroupChatDialog_H__


class CGroupChatDialog : 
	public CChatDialog
{
public:
	CGroupChatDialog(
		const ItemNodeInfo_t & myselfInfo, 
		CNodeList            * pFriendsInfo, 
		ChatFont_t           * pChatFont
		)
		: CChatDialog(Type_ImcGroup, myselfInfo, pFriendsInfo->GetNodeData(), pChatFont)
		, m_pGroupInfo(pFriendsInfo)
	{}

	CControlUI * CreateControl(LPCTSTR pstrClass);


private:
	CNodeList      * m_pGroupInfo;
};


#endif
