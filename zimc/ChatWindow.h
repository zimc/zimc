#ifndef __ChatWindow_H__
#define __ChatWindow_H__


#include "NodeList.h"
#include "DuiWindowBase.h"

class CZiMainFrame;

class CChatDialog
	: public CDuiWindowBase
{
public:
	CChatDialog(
		int                    nChatType, 
		const ItemNodeInfo_t & myselfInfo, 
		CNodeList            * pFriendsInfo, 
		ChatFont_t           * pChatFont, 
		CZiMainFrame         * pMainWindow
		);
	~CChatDialog();

	CZiMainFrame   * MainWindow()  { return m_pMainWindow; }
	ItemNodeInfo_t * GetSelfInfo();
	ItemNodeInfo_t * GetFriendInfo();

	int     OnTextMsgShow(ChatCcTextData_t * pTextData);
	void    recordMsg(ChatCcTextData_t *pChatData);
	void    ActiveWindow();

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


private:
	LPCTSTR GetWindowClassName() const;	
	tstring GetSkinFile();

	CControlUI * CreateControl(LPCTSTR pstrClass);

	void    OnFinalMessage(HWND hWnd);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);


protected:
	void    Notify             (TNotifyUI & msg);
	void    OnPrepare          (TNotifyUI & msg);
	void    OnCreateGroup      (TNotifyUI &msg);
	void    OnFontStyleChanged1(TNotifyUI & msg);
	void    OnFontStyleChanged2(TNotifyUI & msg);
	void    OnTimer            (TNotifyUI & msg);
	void    OnGroupItemClick   (TNotifyUI & msg);
	int     OnExit             (TNotifyUI & msg);
	int     OnMinBtn           (TNotifyUI & msg);
	int     OnMaxBtn           (TNotifyUI & msg);
	int     OnRestoreBtn       (TNotifyUI & msg);
	int     OnFontSetBtn       (TNotifyUI & msg);
    int     OnSendMsg          (TNotifyUI & msg);
    int     OnReportEvil       (TNotifyUI & msg);
    int     OnMsgRecord        (TNotifyUI & msg);

	//added by tian
	void	OnFontBold			(TNotifyUI & msg);
	void	OnFoutItalic		(TNotifyUI & msg);
	void	OnFontUnderLine		(TNotifyUI & msg);
	void	OnFontType			(TNotifyUI & msg);
	void	OnFontSize			(TNotifyUI & msg);
	void	OnFontColor			(TNotifyUI & msg);


	int     UpdateRightChatWindow();
	int     UpdateTextChatWindow(CContainerUI * pChatUi);
	int     UpdateAvChatWindow(CContainerUI * pChatUi);
	int     UpdateGroupChatWindow(CContainerUI * pChatUi);

	void    AddGroupFriendsList();
	void    FontStyleChanged();

	int     NetSendMsg(ChatCcTextData_t & textData, const char * pchData, int nDataLen);
	int     OnTextMsgShow_Test(TCHAR * tszText);
	int     SaveMsgRecord(ChatCcTextData_t & textData);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);


private: 
	UINT             m_nTimer;
	int              m_nChatType;
	ChatFont_t     * m_pChatFont;
	CNodeList      * m_pGroupInfo;

	ItemNodeInfo_t   m_myselfInfo;
	ItemNodeInfo_t   m_friendInfo;

	friend class CMsgRecordWindow;
	CMsgRecordWindow * m_pMsgRecordWindow;

	BOOL             m_nChatFlag;
	CZiMainFrame   * m_pMainWindow;
	//added by tian
	CRichEditUI* m_pSendEdit, * m_pRecvEdit;

	string       m_strPicFile;
	bool         m_inputEditKillFocus;
};


#endif // __ChatDialog_H__
