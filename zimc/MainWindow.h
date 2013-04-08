#ifndef __MainWindow_H__
#define __MainWindow_H__


#include <map>
#include <list>
#include "lock.h"
#include "UiData.h"
#include "DuiWindowBase.h"
#include "NodeList.h"


enum enumMainFrameState
{ 
	State_MainVerifying = 0, 
	State_MainRunning, 
	State_MainExit, 
};


class CTrayWindow;
class CMainMsger;
class CChatDialog;
class CBaseItemListUI;
class CZiSearchWindow;
class CReportWindow;
class CCreateGroupWindow;

class CZiMainFrame 
	: public CDuiWindowBase
{
public:
	CZiMainFrame();
	~CZiMainFrame();

	// ... 
	int     Init();
	int     Uninit();

	int     State()              { return m_nStatus; }
	void    SetState(int nState) { m_nStatus = nState; UpdateState(); }
	CMainMsger * MainMsger()     { return m_pMainMsger; }

	int     SetWindowPosition();
	int     ReadSelfInfo();
	int     SetMainInfo(LoginScData_t * pLoginData);
	ItemNodeInfo_t * GetSelfInfo();

	// action
	int     AddItem (NetItemInfo_t  * pItemInfo, NetItemInfo_t * pItemInfoParent);
	int     AddItem (NetItemInfo_t  * pItemInfo, CNodeList * pNodeInfoParent);
	int     AddItem (ItemNodeInfo_t * pItemInfo, CNodeList * pNodeInfoParent);
	int     DelItem (CNodeList * pNodeInfo, CNodeList * pNodeInfoParent);
	//删除好友 或群
	int     DelItem(CNodeList *pNode);

	int     MoveItem(CNodeList * pNodeInfoSrc, CNodeList * pNodeInfoSrcParent, CNodeList * pNodeInfoDesParent);
	int     ModifyItem(CNodeList * pNodeInfo);

	// other 
	CNodeList  * GetFriendTeamNode();
	int     CreateChatDailog(CNodeList * pNodeList, CChatDialog ** ppChatDialog = 0);
	int     CreateChatDailog(int nId, CChatDialog ** ppChatDialog = 0);

	void    CancelWatch(int nChatId);
	int     SendImMessageX(int nMsg, LPARAM lp, WPARAM wp);


private:
	LPCTSTR GetWindowClassName() const;
	tstring GetSkinFile();

	CControlUI * CreateControl(LPCTSTR pstrClass);

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnNcHitTest (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);

	void    Notify(TNotifyUI & msg);

	const   TCHAR * GetCurListName(int nIndex);
	void    ExchangeEditFocus(CRichEditUI * pEditUi1, CRichEditUI * pEditUi2);


private:
	int     UpdateState();

    void    OnKeepAlive();
	void    OnTimer();
	void    OnPrepare(TNotifyUI & msg);
	void    OnKillFocus(TNotifyUI & msg);
	void    OnSelectTab(TNotifyUI & msg);
	void    OnSelectItemList(TNotifyUI & msg);
	void    OnItemChat(TNotifyUI & msg);

	int     OnClickRightButton(TNotifyUI & msg);
	int     OnClickRightMenu(TNotifyUI & msg);
	int		OnCreateGroup(TNotifyUI &msg);
	int     OnModifyGroupName(CNodeList *pNode);
	int     OnReturnSearch(TNotifyUI & msg);
	int     OnSelectSearch(TNotifyUI & msg);
	int     OnClickMatchButton(TNotifyUI & msg);
	int     OnClickSettingButton(TNotifyUI & msg);
	int     OnClickSearchButton(TNotifyUI & msg);
	int     OnClickVerifyResponse(WPARAM wp, LPARAM lp);
	int     OnClickAddGroupResponse(WPARAM wp, LPARAM lp);

	int     AddComboItem(LPCTSTR lpctsName);
	int     ClearComboItem();

	void    UpdateBackground();
	void    UpdateSelfInfo(LoginScData_t * pLoginInfo);
	void    UpdateFriendsList(LoginScData_t * pFriendsInfo);
	void    UpdateGroupsList(LoginScData_t  * pGroupsInfo);
	void    UpdateTalksList(LoginScData_t  * pTalksInfo);

	int     ParserSelfInfo(tstring & tstrSelfInfo);

	void    FlashTray(BOOL bFlashOrNot);
	BOOL    HandleNetCmd();
	void    DelayDispatchNetCmd(int nMsg, void * pNetData);
	int     HandleNetMessage(int nMsg, void * pNetData);


private:
	int                   m_nStatus;
	ChatFont_t            m_chatFont;
	ItemNodeInfo_t        m_itemSelfInfo;

	// **********
	// 将群成员与好友混一起了, 暂时没有修改, 不知道有什么隐患. 
	// 群的成员添加和删除只有管理员有权限. 
	// 将群和讨论组混在一起了, 这样不允许它们重名. 
	// 
	// 所有的 Item 信息, 包括: 组, 群, 好友. 
	// Item 唯一.
	typedef std::vector<CNodeList*>   ImcNodeList_t;
	typedef std::map<int, CNodeList*> ImcNodeTable_t;
	ImcNodeTable_t        m_mapNodeTable;
	CCriticalSetionObject m_mainLock;

	// 组最大ID. 
	int                   m_nMaxTeamId;

	// 群信息. 
	// Item 不唯一. 
	typedef CNodeList                 ImcGroupItem_t;
	typedef std::map<ImcGroupItem_t*, ImcNodeTable_t> ImcGroupTable_t;
	ImcGroupTable_t       m_mapGroupTable;

	// 搜索, 临时. 
	ImcNodeList_t         m_searchNodes;

	// 子对话框
	friend class CZiSearchWindow;
	friend class CReportWindow;
	friend class CMsgRecordWindow;
	friend class CCreateGroupWindow;

	CZiSearchWindow     * m_pSearchWindow;
	CReportWindow       * m_pReportWindow;
	CMsgRecordWindow    * m_pMsgRecordWindow;
	CCreateGroupWindow  * m_pCreateGroupWindow;
	CCreateGroupWindow  * m_pModifyGroupWindow;

	// tray
	CTrayWindow         * m_pTrayWindow;
	BOOL                  m_bFlash;
	UINT                  m_nTimer;
    time_t                m_nlastKeepAlive_time_;

	// 待处理表
	typedef std::pair<int, void *>    ImcNetData_t;
	typedef std::list<ImcNetData_t>   ImcNetDataList_t;
	ImcNetDataList_t      m_listNetData;

	// 子窗口. 
	typedef std::map<int,  CDuiWindowBase*> ImcSubWindowTable_t;
	ImcSubWindowTable_t  m_mapSubWindows;

	// NetData
	CMainMsger         * m_pMainMsger;


public:
	CBaseItemListUI * GetNodeListUi(int nNodeType);
	CNodeList       * GetNodeInfo(int nId);
	CNodeList       * GetTeamInfo(LPCTSTR tsTeamName);
	ItemNodeInfo_t  * GetNodeData(int nId);

	int     FindTeamId(LPCTSTR tsTeamName);
	int     SearchFriendsNode(LPCTSTR tsMask,  ImcNodeList_t & nodeList);
	bool    IsUnknownFriend(int nId);
	bool    IsUnknownGroup(int nid);

	void    DeleteGroupNode(int nId);
	void    DeleteTeamNode(int nId);
	void    DeleteGroupFriendNode(CNodeList * pParent, int nId);
	void    DeleteTeamFriendNode(int nId);
	void    InvalidGroupFriendNode(int nId);

	void    handleGreateGroup(GroupInfoData_t *pGroupInfoData);
	void    handlerAddGroupVerify(AddGroupInfo_t *pAddgroup);
	//report window
	void    reportEvil();
	void    MsgRecord();
};


#endif
