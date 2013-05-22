#ifndef __UiData_H__
#define __UiData_H__

#include "common/dbstruct.h"
// ----------------------------------------------------------
// 好友信息冗余定义
#define Item_Name(_str, _name)  _str##_name
#define Obj_Init(_cls)			_cls() { ::memset(this, 0, sizeof(*this)); }
#define VN(_LoginDef)           _LoginDef(std::string, str,  LocalId_t)
#define VL(_LoginDef)           _LoginDef(tstring,     tstr, LocalId_t)
#define VCS(_MsgDef)            _MsgDef(NetId_t)
#define VSC(_MsgDef)            _MsgDef(LocalId_t)
#define VCC(_MsgDef)			_MsgDef(MagicId_t)


#define TMP_DATA_DIR ".data"

#define Public_Item_Info_Define(_string_type, _pre, _id_type)  struct                      \
{ \
	/* public */       \
	_id_type                    nId;                           /* ***                   */ \
	int                         nAdminId;                      /* 所属组ID              */ \
	_string_type                Item_Name(_pre, NickName);     /* 好友昵称, 群名称.     */ \
	_string_type                Item_Name(_pre, AdminName);    /* 所属组名, 服务器规则  */ \
	_string_type                Item_Name(_pre, Logo);         /* logo.                 */ \
    _string_type                Item_Name(_pre, Picture);      /* 好友图片, 群背景图片. */ \
	_string_type                Item_Name(_pre, Description);  \
	int                         nAddFriendType;                /* 设置被添加好友类型 0:允许任何人添加,1:验证添加,2:不允许添加*/\
}

#define Friend_Item_Info_Define(_string_type, _pre, _id_type)  struct                      \
{ \
	/* add friends. */ \
	int                         nAge;                          \
	_string_type                Item_Name(_pre, RealName);     \
	_string_type                Item_Name(_pre, Address);      \
	_string_type                Item_Name(_pre, Sex);          \
	_string_type                Item_Name(_pre, Height);       \
	_string_type                Item_Name(_pre, NativePlace);  \
	_string_type                Item_Name(_pre, Birthday);     \
	_string_type                Item_Name(_pre, Position);     \
	_string_type                Item_Name(_pre, Studies);      \
	_string_type                Item_Name(_pre, Experience);   \
	_string_type                Item_Name(_pre, Evaluation);   \
}

#define Group_Item_Info_Define(_string_type, _pre, _id_type)   struct                      \
{ \
	/* add group */   \
	_id_type                    nGroupAdminId;                 /* 群管理者ID, 可能多个  */ \
	_string_type                Item_Name(_pre, Notify);       /* 群公告内容,           */ \
}

#define Public_Net_Msg_Header_Define(_id_type)                 struct                      \
{ \
	_id_type                    nSenderId;    \
	_id_type                    nRecverId;    \
	char                      * szSenderName; \
	char                      * szRecverName; \
}

#define Public_Net_Msg_Header_Define_Ex(_id_type)              struct                      \
{ \
	_id_type                    nSenderId;    \
	_id_type                    nRecverId;    \
	int                         nRecvType;                     /* 1 好友 2 群 3 讨论组 */  \
	char                      * szSenderName; \
	char                      * szRecverName; \
}


// ----------------------------------------------------------
// 消息定义
enum enumZimcMsg
{
	Msg_LocalBase  = 0x10, 
	Msg_OpenChatWindow, 

	Msg_NetCSBase  = WM_USER, 

	// login
	Msg_CsLogin, 
	Msg_ScLogin, 

	// user
	Msg_CsSearchItem, 
	Msg_ScSearchItem, 
	Msg_CsAddItem, 
	Msg_ScAddItem, 
	Msg_CsDelItem, 
	Msg_ScDelItem, 
	Msg_CsMoveItem, 
	Msg_ScMoveItem, 
	Msg_CsModifyItem, 
	Msg_ScModifyItem, 

	// search. 
	Msg_CsQueryUsers, 
	Msg_ScResponseUsers, 
	Msg_CsQueryGroup, //reserved
	Msg_ScResponseGroup,

	// add friend. 
	Msg_CsQueryVerify, 
	Msg_ScQueryVerify, 
	Msg_CsResponseVerify, 
	Msg_ScResponseVerify, 

    //del friend
    Msg_CsDelFriend,
    Msg_ScDelFriend,

	//add group
	Msg_CsAddGroupVerify,
	Msg_ScAddGroupVerify,
	Msg_CsResponseAddGroup,
	Msg_ScResponseAddGroup,

	//group
	Msg_CsCreateGroup,
	Msg_ScCreateGroup,
	Msg_CsModifyGroup,
	Msg_ScModifyGroup,

	// report 
	Msg_CsEvilReport, 
	Msg_ScEvilReport, 

	//set
	Msg_CsSetInfo,
	Msg_ScSetInfo,

	// chat
	Msg_CsTextChat, 
	Msg_ScTextChat, 

	// cs
	Msg_NetCSEnd, 
	Msg_NetInBase  = 0x0800, 

	// ???
	Msg_InTray, 
	Msg_InQueryVerify, 

	Msg_addGroupReserved,
	Msg_InAddGroupVerify,

    Msg_LoadMessage,
    Msg_KeepAlive,

	// unkown 
	Msg_NetUnknown = 0x1000, 

	// add msg. 
	//WM_IMC_LOGIN_OK, 
	//WM_IMC_TRAY, 
	//WM_IMC_VERIFY_OK, 
	//WM_IMC_VERIFY_CANCEL, 
	//WM_IMC_CHAT_TEXT, 
	//WM_IMC_SEARCH_FRIEND, 
};

#define Msg_Cs(_msg)			(_msg & 0x1)
#define Msg_Sc(_msg)            (!Msg_Cs(_msg))
#define AssertMsgCS(_msg)		{ Assert((_msg) > Msg_NetCSBase && (_msg) < Msg_NetCSEnd); }


typedef struct NetId_t
{
	int nId;

	NetId_t(int type, int id);
	NetId_t()                   { nId = 0;    }
	operator int()              { return nId; }
	operator const int() const  { return nId; }
}NetId_t;

typedef struct LocalId_t
{
	int nId;

	LocalId_t(int type, int id);
	LocalId_t()                 { nId = 0;    }
	operator int()              { return nId; }
	operator const int() const  { return nId; }
}LocalId_t;

typedef struct MagicId_t
{
	int nId;

	MagicId_t(int msg, int type, int id);
	MagicId_t()                 { nId = 0;    }
	MagicId_t(int nid)          { nId = nid; }
	operator int()              { return nId; }
	operator const int() const  { return nId; }
}MagicId_t;

#define NetId_F(_id)            NetId_t  (Type_ImcFriend,    int(_id))
#define NetId_R(_id)            NetId_t  (Type_ImcFriendX,   int(_id))
#define NetId_G(_id)            NetId_t  (Type_ImcGroup,     int(_id))
#define NetId_T(_id)            NetId_t  (Type_ImcTalk,      int(_id))
#define NetId_E(_id)            NetId_t  (Type_ImcTeam,      int(_id))
#define LocalId_F(_id)          LocalId_t(Type_ImcFriend,    int(_id))
#define LocalId_R(_id)          LocalId_t(Type_ImcFriendX,   int(_id))
#define LocalId_G(_id)          LocalId_t(Type_ImcGroup,     int(_id))
#define LocalId_T(_id)          LocalId_t(Type_ImcTalk,      int(_id))
#define LocalId_E(_id)          LocalId_t(Type_ImcTeam,      int(_id))
#define MagicId_F(_msg, _id)    MagicId_t(_msg, Type_ImcFriend,  int(_id))
#define MagicId_R(_msg, _id)    MagicId_t(_msg, Type_ImcFriendX, int(_id))
#define MagicId_G(_msg, _id)    MagicId_t(_msg, Type_ImcGroup,   int(_id))
#define MagicId_T(_msg, _id)    MagicId_t(_msg, Type_ImcTalk,    int(_id))
#define MagicId_E(_msg, _id)    MagicId_t(_msg, Type_ImcTeam,    int(_id))


// --------------------------------------------
// Login 消息结构
// 1 login 本地消息如下. 
// 2 缺少: login 的网络消息结构
// 
typedef struct LoginCsData_t
{
	char * szAccount;
	char * szPassword;

	Obj_Init(LoginCsData_t);
}LoginCsData_t;

typedef struct NetItemInfo_t
{
	int             nItemType;

	VN(Public_Item_Info_Define);
	VN(Friend_Item_Info_Define);
	VN(Group_Item_Info_Define);

	// 当 ItemType 不是好友时有效. 
	int             nFriendSize;
	NetItemInfo_t * pFriendInfo;

	Obj_Init(NetItemInfo_t);
}NetItemInfo_t;

typedef struct LoginScData_t
{
	int             nLoginStatus;

	// 自身信息
	// 已知: 帐号和密码. 
	// 未知: 
	// -- nick name
	//    descript. 
	//    ... ...
	// 
	NetItemInfo_t   selfInfo;

	// 1 好友必须存在于组(群)内. 
	// 2 组可以为空. 
	// 3 群不能为空. 
	// 4 原则上组之间不能有相同的好友
	// 5 好友, 组, 群通用结构. 
	//   -- id. 
	//      name
	//      type
	//      size

	// 好友信息. 
	// 1 首先组信息. 
	// 2 每个组的好友信息
	// 3 好友个数可能小于组个数, 因为组可能为空. 
	// 4 需要验证好友个数. 
	int             nTeamSize;
	NetItemInfo_t * pTeamInfo;

	// 群信息
	int             nGroupSize;
	NetItemInfo_t * pGroupInfo;

	// 讨论组信息
	int             nTalkSize;
	NetItemInfo_t * pTalkInfo;

	Obj_Init(LoginScData_t);
}LoginScData_t;


// --------------------------------------------
// report evils 消息结构
typedef struct ReportCsEvilData_t
{
	VCS(Public_Net_Msg_Header_Define);

	char *  szAccount;
	char *  szRecords;
	char *  szDescript;
	int     succ;

	Obj_Init(ReportCsEvilData_t);
}ReportCsEvilData_t;


// --------------------------------------------
// search 消息结构
typedef struct SearchCsQuery_t
{
	char * szSelfName;
	char * szNickName;
	int    nMatchType;

	Obj_Init(SearchCsQuery_t);
}SearchCsQuery_t;

typedef struct SearchScResponse_t
{
	// 仅搜寻的用户信息. 
	int              nMatchType;
	int              nItemSize;
	NetItemInfo_t  * pItemInfo;
	char            szSearchName[256];

	Obj_Init(SearchScResponse_t);
}SearchScResponse_t;

class SearchGroup_t {
public:
	string strSearchName;
	vector <DBGroup> groups;
};

// --------------------------------------------
// add friend 消息结构
typedef struct VerifyCcQuery_t
{
	VCC(Public_Net_Msg_Header_Define_Ex);

	ItemNodeInfo_t * pSenderLocalQInfo;  // c
	NetItemInfo_t  * pSenderNetQInfo;    // s
	char           * szVerifyData;
	int              nAddFriendType;

	Obj_Init(VerifyCcQuery_t);
}VerifyCcQuery_t;

typedef struct VerifyCcResponse_t
{
	VCC(Public_Net_Msg_Header_Define_Ex);

	// szReason 表示拒绝原因. 
	ItemNodeInfo_t * pSenderLocalRInfo;  // c
	NetItemInfo_t  * pSenderNetRInfo;    // s
	BOOL             bIsAgree;
	char           * szReason;

	char             szRecverName2[128];
	char             szSenderName2[128];
	int              nAddFriendType;

	Obj_Init(VerifyCcResponse_t);
}VerifyCcResponse_t;

typedef struct VerifyScQuery_t
	: public VerifyCcQuery_t
{
	char szSenderNamex[64];
	char szRecverNamex[64];
	char szVerifyDatax[1024];

	Obj_Init(VerifyScQuery_t);
}VerifyScQuery_t;

class AddGroupInfo_t {
public:
	int    nSenderId;
	string strSenderName;
	int    nAdminId;
	string strAdminName;
	int    type;  //0:发送验证， 1：回复验证， 2：通知
	int    succ;
	
	string strVerify;
	
	DBUser  userinfo;
    DBGroup groupinfo;

	ItemNodeInfo_t * pSenderLocalQInfo;

	AddGroupInfo_t()
		:nSenderId(0),
		nAdminId(0),
		type(-1),
		succ(-1),
		pSenderLocalQInfo(0)
	{}
};


//group
class GroupInfoData_t {
public:
	GroupInfoData_t()
	  :nSender(0)
	  ,type(-1)
	  ,succ(-1){}

	string strSender;
	int    nSender;
	int	   type;
	int    succ;

	DBGroup  groupinfo;
	DBFriend friendinfo;
};
enum GROUP_INFO_ {
	GROUP_INFO_VERIFY = 0,
	GROUP_INFO_REPLY = 1,
	GROUP_INFO_NOTIFY = 2,
	GROUP_INFO_CREATE = 3,
	GROUP_INFO_MODIFY = 4,
};

// --------------------------------------------
// text chat 消息结构
typedef struct ChatCcTextData_t
{
	VCC(Public_Net_Msg_Header_Define_Ex);

	char *  szData;
	int     nDataLen;
	char *  szTime;

	// add
	TCHAR * tsSenderName;
	TCHAR   tsTime[32];
	CHAR    szSenderNamex[64];

	Obj_Init(ChatCcTextData_t);
}ChatCcTextData_t;

#include <string>
using namespace std;

class DelFriendItem {
public:
	DelFriendItem():nSendId(0),nDelId(0),succ(-1),type(0){}
    string strSendName;
    string strdelName;
    int nSendId;
    int nDelId;
	int succ;
	int type; //0: 删除好友 1:删除群
};

enum _setFlag {
	SET_INFO_INVITE = 0,
};

class SetInfo_t {
public:
	SetInfo_t(): nId(0), nInvited(0), type(0){}
	int nId;
	int nInvited;
	int type;
};

// ----------------------------------------------------------
// 
#include <vector>


namespace DuiLib 
{
	class CListContainerElementUI;
}

enum enumChatFlag 
{ 
	Flag_TextChat  = 0x01, 
	Flag_AvChat    = 0x02, 
	Flag_GroupChat = 0x04, 
};

enum enumItemNodeType
{
	Type_ImcUnknown = 0, 

	// main. 
	Type_ImcTeam, 
	Type_ImcFriend, 
	Type_ImcFriendX,                            // 它表示群的成员, 注意群的成员不一定是好友. 
	Type_ImcGroup, 
	Type_ImcTalk, 

	Type_ImcMask    = 0x7F, 
	Type_ImcInvalid = 0x80, 
};

enum enumItemNodeStatus
{
	State_Read      = 0x01, 
	State_Write     = 0x02, 
	State_ReadWrite = 0x01 | 0x02, 
	State_Invalid   = 0x80, 
};

enum enumItemNodeId
{
	// 保留有 100 个ID 作为内部自定义ID, 对于不同的客户端来说可以重复.  
	// 内部保留一个默认组, 
	#define Team_DefaultNameA  "我的好友"
	#define Team_DefaultNameT  _T(Team_DefaultNameA)

	BaseId_Unknown    = 0, 
	BaseId_BaseTeam, 
	BaseId_MaxTeam    = 80, 
	BaseId_SearchWindow, 
	BaseId_Reserved   = 100, 

	BaseId_Friend, 
	BaseId_Group,
	BaseId_Talk, 
};

enum enumSearchType
{
	Type_MatchFriend = 1, 
	Type_MatchGroup,
};


typedef struct ItemNodeInfo_t
{
	bool                      bIsFolder;        // 是否是子项, 只有 Type_ImcFriend 类型为子项. 
	bool                      bIsHasChild;      // 子项是否为空. *** ( 无意义 )
	bool                      bIsChildVisible;  // 是否展开子项. 
	char                      chType;           // ***, 类型: enumItemNodeType

	char                      nLevel;           // 
	char                      chStatus;         // ***
	short                     sReserved;

	VL(Public_Item_Info_Define);
	VL(Friend_Item_Info_Define);
	VL(Group_Item_Info_Define);

	ItemNodeInfo_t() 
		: bIsFolder(false)
		, bIsChildVisible(false)
		, bIsHasChild(false)
		, chType(0)
		, nLevel(0)
		, nAdminId(0)
		, nId(0, 0)                             // ???
		, nGroupAdminId(0, 0)                   // ???
		, chStatus(State_Read)
		, nAge(0)
	{}

	int  Type()      { return chType & Type_ImcMask; }
	BOOL IsInvalid() { return chType & Type_ImcInvalid; }
}ItemNodeInfo_t;

typedef struct NodeData_t : public ItemNodeInfo_t
{
	CListContainerElementUI * pListElement;

	NodeData_t()
		: pListElement(0)
	{} 

	void Assign(const ItemNodeInfo_t & nodeData)
	{
		*(static_cast<ItemNodeInfo_t*>(this)) = nodeData;
	}
}NodeData_t;


typedef struct ChatFont_t
{
	bool     bBold;
	bool     bItalic;
	bool     bUnderline;
	DWORD    dwFontSize;
	DWORD    dwTextColor;
	TCHAR    tszFontName[64];

	ChatFont_t()
		: bBold(false)
		, bItalic(false)
		, bUnderline(false)
		, dwFontSize(12)
		, dwTextColor(0xFF000000)
	{
		_tcscpy_s(tszFontName, _T("微软雅黑"));
	}
}ChatFont_t;


inline int  IdNetToLocal(int nType, int nId)
{
#ifdef _IMC_TEST
	switch(nType)
	{
	case Type_ImcFriend: 
	case Type_ImcFriendX:
		return nId + 100;

	case Type_ImcTeam:
		return nId;

	case Type_ImcGroup:
		return nId + 100000;

	case Type_ImcTalk:
		return nId + 1000000;
	}
#else
	switch(nType)
	{
	case Type_ImcFriend: 
	case Type_ImcGroup:
	case Type_ImcFriendX:
	case Type_ImcTalk:
		return nId + BaseId_Reserved;

	case Type_ImcTeam:
		return nId + BaseId_BaseTeam;
	}
#endif

	// Assert(0);		// default, unknown
	return 0;
}

inline int  IdLocalToNet(int nType, int nId)
{
#ifdef _IMC_TEST
	switch(nType)
	{
	case Type_ImcFriend: 
	case Type_ImcFriendX:
		return nId - 100;

	case Type_ImcTeam:
		return nId;

	case Type_ImcGroup:
		return nId - 100000;

	case Type_ImcTalk:
		return nId - 1000000;
	}
#else
	switch(nType)
	{
	case Type_ImcFriend: 
	case Type_ImcGroup:
	case Type_ImcFriendX:
	case Type_ImcTalk:
		return nId - BaseId_Reserved;

	case Type_ImcTeam:
		return nId - BaseId_BaseTeam;
	}
#endif

	// Assert(0);		// default, unknown
	return 0;
}

inline int  CmdNetToLocal(int nNetCmd, int nType)
{
	// ... ???

	switch(nNetCmd)
	{
	case  1: return Msg_ScLogin;
	case  5: return Msg_ScTextChat;
	case 13: return nType == 1 ? Msg_ScResponseUsers : Msg_ScResponseGroup;
	case 14: return nType == 0 ? Msg_ScQueryVerify : Msg_ScResponseVerify;
    case 15: return Msg_ScDelFriend;
	case 16:
		{
			if (nType == GROUP_INFO_VERIFY || nType == GROUP_INFO_REPLY) { return Msg_ScAddGroupVerify;}
			else if (nType == GROUP_INFO_CREATE) { return Msg_ScCreateGroup;}
			else if (nType == GROUP_INFO_MODIFY) { return Msg_ScModifyGroup;}
			else Assert(0);
		}
		break;
	case 20: return Msg_ScEvilReport;
	}

	return Msg_NetUnknown;
}

inline int  CmdLocalToNet(int nLocalCmd)
{
	// ... ???
	Assert(0);
	return Msg_NetUnknown;
}

inline int  TypeNetToLocal(int nNetType)
{
	switch(nNetType)
	{
	case 1: return Type_ImcFriend;
	case 2: return Type_ImcGroup;
	case 3: return Type_ImcTalk;
	}

	Assert(0);
	return 0;
}

inline int  TypeLocalToNet(int nLocalType)
{
	switch(nLocalType)
	{
	case Type_ImcFriend:  return 1;
	case Type_ImcFriendX: return 1;
	case Type_ImcGroup:   return 2;
	case Type_ImcTalk:    return 3;
	}

	Assert(0);
	return 0;
}

inline NetId_t::NetId_t(int type, int id)
{
	nId = IdLocalToNet(type, id);
}

inline LocalId_t::LocalId_t(int type, int id)
{
	nId = IdNetToLocal(type, id);
}

inline MagicId_t::MagicId_t(int msg, int type, int id)
{
	AssertMsgCS(msg);
	if(Msg_Cs(msg)) nId = IdLocalToNet(type, id);
	else            nId = IdNetToLocal(type, id);
}

inline void ItemDataNetToLocal(NetItemInfo_t & netNode, ItemNodeInfo_t & localNode)
{
	localNode.chType           = netNode.nItemType;
	localNode.chStatus         = localNode.IsInvalid() ? State_Invalid : State_Read;
	localNode.bIsFolder        = localNode.Type() != Type_ImcFriend && localNode.Type() != Type_ImcFriendX;
	localNode.bIsHasChild      = localNode.Type() != Type_ImcFriend && localNode.Type() != Type_ImcFriendX;
	localNode.bIsChildVisible  = netNode.nItemType == Type_ImcTeam;

	// ... ???
	localNode.nId              = netNode.nId;
	localNode.nAdminId         = netNode.nAdminId;
	localNode.tstrNickName     = CA2T(netNode.strNickName.c_str());
	localNode.tstrAdminName    = CA2T(netNode.strAdminName.c_str());
	localNode.tstrDescription  = CA2T(netNode.strDescription.c_str());
	localNode.tstrRealName     = CA2T(netNode.strRealName.c_str());
	localNode.tstrSex		   = CA2T(netNode.strSex.c_str());
    localNode.tstrHeight       = CA2T(netNode.strHeight.c_str());
	localNode.tstrNativePlace  = CA2T(netNode.strNativePlace.c_str());
	localNode.tstrBirthday     = CA2T(netNode.strBirthday.c_str());
	localNode.tstrPosition     = CA2T(netNode.strPosition.c_str());
	localNode.tstrStudies      = CA2T(netNode.strStudies.c_str());
	localNode.tstrExperience   = CA2T(netNode.strExperience.c_str());
	localNode.tstrEvaluation   = CA2T(netNode.strEvaluation.c_str()); 
	localNode.nAddFriendType   = netNode.nAddFriendType;
}

inline void ItemDataNetToLocal(GroupInfoData_t &groupNode, ItemNodeInfo_t & localNode) {
	localNode.chType           = Type_ImcGroup;
	localNode.chStatus         = localNode.IsInvalid() ? State_Invalid : State_Read;
	localNode.bIsFolder        = localNode.Type() != Type_ImcFriend && localNode.Type() != Type_ImcFriendX;
	localNode.bIsHasChild      = localNode.Type() != Type_ImcFriend && localNode.Type() != Type_ImcFriendX;
	localNode.bIsChildVisible  = localNode.Type() == Type_ImcTeam;

	// ... ???
	localNode.nId              = LocalId_t(Type_ImcGroup, groupNode.groupinfo.group_id );
	localNode.nAdminId         = 0;
	localNode.tstrNickName     = CA2T(groupNode.groupinfo.name.c_str());
	//localNode.tstrAdminName    = CA2T(groupNode.groupinfo.strAdminName.c_str());
	//localNode.tstrDescription  = CA2T(groupNode.groupinfo.strDescription.c_str());
}

inline void ItemDataNetToLocal(DBGroup &groupNode, ItemNodeInfo_t & localNode) {
	localNode.chType           = Type_ImcGroup;
	localNode.chStatus         = localNode.IsInvalid() ? State_Invalid : State_Read;
	localNode.bIsFolder        = localNode.Type() != Type_ImcFriend && localNode.Type() != Type_ImcFriendX;
	localNode.bIsHasChild      = localNode.Type() != Type_ImcFriend && localNode.Type() != Type_ImcFriendX;
	localNode.bIsChildVisible  = localNode.Type() == Type_ImcTeam;

	// ... ???
	localNode.nId              = LocalId_t(Type_ImcGroup, groupNode.group_id );
	localNode.nAdminId         = 0;
	localNode.tstrNickName     = CA2T(groupNode.name.c_str());
	//localNode.tstrAdminName    = CA2T(groupNode.groupinfo.strAdminName.c_str());
	//localNode.tstrDescription  = CA2T(groupNode.groupinfo.strDescription.c_str());
}

inline void ItemDataNetToLocal(DBUser &dbuser, ItemNodeInfo_t & localNode, int type) {
	localNode.chType           = type;
	localNode.chStatus         = localNode.IsInvalid() ? State_Invalid : State_Read;
	localNode.bIsFolder        = localNode.Type() != Type_ImcFriend && localNode.Type() != Type_ImcFriendX;
	localNode.bIsHasChild      = localNode.Type() != Type_ImcFriend && localNode.Type() != Type_ImcFriendX;
	localNode.bIsChildVisible  = localNode.Type() == Type_ImcTeam;

	// ... ???
	localNode.nId              = LocalId_t(type, dbuser.user_id);
	localNode.nAdminId         = 0;
	localNode.tstrNickName     = CA2T(dbuser.user_name.c_str());
	localNode.tstrAdminName    = CA2T(dbuser.type.c_str());
	localNode.tstrDescription  = CA2T(dbuser.experience.c_str());
}

inline void ItemDataLocatToNet(ItemNodeInfo_t & localNode, DBUser &dbuser) {
	dbuser.user_id = IdLocalToNet(Type_ImcFriend, localNode.nId);
	dbuser.user_name = CT2A(localNode.tstrNickName.c_str());
	//TODO
}

#endif
