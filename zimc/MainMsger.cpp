#include "stdafx.h"
#include "MainMsger.h"
#include "common/json_util.h"
#include "common/msginterface.h"


int CMainMsger::LocalToNet(int nMsg, void * pLocalData, int nLocalDataLen, Byte_t ** ppbNetData, int * pnNetDataLen)
{
	switch(nMsg)
	{
	case Msg_CsQueryUsers:
		{
			SearchCsQuery_t * pSearchQuery = (SearchCsQuery_t*)pLocalData;
			NetMsg_t * pNetData = new NetMsg_t;
			if(!pNetData) return Error_OutOfMemory;

			pNetData->set_cmd (13);
			pNetData->set_uid (pSearchQuery->szSelfName);
			pNetData->set_tuid(pSearchQuery->szNickName);
			pNetData->set_type(pSearchQuery->nMatchType);

			*ppbNetData   = (Byte_t *)pNetData;
			*pnNetDataLen = -1;
		}
		break;

	case Msg_CsQueryVerify:
		{
			VerifyCcQuery_t * pVerifyQuery = (VerifyCcQuery_t*)pLocalData;
			NetMsg_t        * pNetData     = new NetMsg_t;
			if(!pVerifyQuery || !pNetData) return Error_OutOfMemory;

			pNetData->set_cmd (14);
			pNetData->set_uid (pVerifyQuery->szSenderName);
			pNetData->set_tuid(pVerifyQuery->szRecverName);
			pNetData->set_type(0);

			Json::Value jsTmp;
			Json::Value jsFriends;
			jsTmp["id"]         = int(pVerifyQuery->nSenderId);
			jsTmp["tid"]        = int(pVerifyQuery->nRecverId);
			jsTmp["verifyinfo"] = pVerifyQuery->szVerifyData;
            jsTmp["friend_type"] = Team_DefaultNameA;

			Assert(pVerifyQuery->pSenderLocalQInfo);
			PacketFriend(&jsTmp["friend"], pVerifyQuery->pSenderLocalQInfo, Type_ImcFriend);
			pNetData->set_msg(jsTmp.toStyledString());

			*ppbNetData   = (Byte_t*)pNetData;
			*pnNetDataLen = -1;
		}
		break;

	case Msg_CsResponseVerify:
		{
			// VerifyResponse_t
			Assert(pLocalData);
			VerifyCcResponse_t * pVerifyResp = (VerifyCcResponse_t*)pLocalData;
			NetMsg_t           * pNetData    = new NetMsg_t;

			pNetData->set_cmd (14);
			pNetData->set_uid (pVerifyResp->szSenderName);
			pNetData->set_tuid(pVerifyResp->szRecverName);
			pNetData->set_type(1);
			pNetData->set_succ(!pVerifyResp->bIsAgree);

			Json::Value jsTmp;
			jsTmp["id"]         = int(pVerifyResp->nSenderId);
			jsTmp["tid"]        = int(pVerifyResp->nRecverId);
            jsTmp["friend_type"] = Team_DefaultNameA;

			Assert(pVerifyResp->pSenderLocalRInfo);
			PacketFriend(&jsTmp["friend"], pVerifyResp->pSenderLocalRInfo, Type_ImcFriend);
			pNetData->set_msg(jsTmp.toStyledString());

			*ppbNetData   = (Byte_t*)pNetData;
			*pnNetDataLen = -1;
		}
		break;

	case Msg_CsTextChat:
		{
			ChatCcTextData_t * pChatData = (ChatCcTextData_t*)pLocalData;
			NetMsg_t         * pNetData  = new NetMsg_t;

			if(!pNetData || !pChatData)
			{
				return Error_OutOfMemory;
			}

			pNetData->set_cmd (5);
			pNetData->set_uid (pChatData->szSenderName);
			pNetData->set_tuid(pChatData->szRecverName);
			pNetData->set_type(pChatData->nRecvType);
			pNetData->set_buf (pChatData->szData, pChatData->nDataLen);

            pNetData->set_user_id(pChatData->nSenderId);
            pNetData->set_tuser_id(pChatData->nRecverId);

			Json::Value jsTmp;
			jsTmp["id"]   = int(pChatData->nSenderId);
			jsTmp["tid"]  = int(pChatData->nRecverId);
			jsTmp["time"] = pChatData->szTime;
			pNetData->set_msg(jsTmp.toStyledString());

			*ppbNetData   = (Byte_t *)pNetData;
			*pnNetDataLen = -1;
		}
		break;
    case Msg_LoadMessage:
        {
            NetMsg_t        * pNetData     = new NetMsg_t;
            LoginCsData_t * pLoginQuery = (LoginCsData_t*)pLocalData;
			if(!pLoginQuery || !pNetData) return Error_OutOfMemory;
            pNetData->set_cmd(19);
            pNetData->set_uid(pLoginQuery->szAccount);
            *ppbNetData   = (Byte_t *)pNetData;
			*pnNetDataLen = -1;
        }
        break;
    case Msg_CsDelFriend:
        {
            NetMsg_t *pNetData = new NetMsg_t;
            DelFriendItem *pDelfriend = (DelFriendItem*)pLocalData;
            if (!pNetData || !pDelfriend) {
                return Error_OutOfMemory;
            }
            pNetData->set_cmd(15);
            pNetData->set_uid(pDelfriend->strSendName);
            pNetData->set_tuid(pDelfriend->strdelName);
            pNetData->set_user_id(pDelfriend->nSendId);
            pNetData->set_tuser_id(pDelfriend->nDelId);
			pNetData->set_type(pDelfriend->type);
            *ppbNetData = (Byte_t *)pNetData;
            *pnNetDataLen = -1;
        }
        break;
    case Msg_KeepAlive:
        {
            NetMsg_t *pNetData = new NetMsg_t;
            pNetData->set_cmd(18);
            *ppbNetData = (Byte_t *)pNetData;
            *pnNetDataLen = -1;
        }
        break;
	case Msg_CsEvilReport:
		{
			NetMsg_t *pNetData = new NetMsg_t;
			ReportCsEvilData_t *pReportData = (ReportCsEvilData_t*)pLocalData;
			if (!pNetData) {
				return Error_OutOfMemory;
			}
			pNetData->set_cmd(20);
			pNetData->set_uid(pReportData->szSenderName);
			pNetData->set_user_id(pReportData->nSenderId);
			pNetData->set_tuid(pReportData->szAccount);
			//TODO 不知中文是否有问题
			pNetData->set_buf(pReportData->szDescript, strlen(pReportData->szDescript));
			*ppbNetData = (Byte_t *)pNetData;
			*pnNetDataLen = -1;
		}
		break;
	case Msg_CsCreateGroup:
		{
			NetMsg_t *pNetData = new NetMsg_t;
			GroupInfoData_t *pGroupInfo = (GroupInfoData_t *)pLocalData;
			if (!pNetData) return Error_OutOfMemory;
			pNetData->set_cmd(16);
			pNetData->set_uid(pGroupInfo->strSender);
			pNetData->set_user_id(pGroupInfo->nSender);
			pNetData->set_type(GROUP_INFO_CREATE);
			pNetData->set_tuid(pGroupInfo->groupinfo.name);
			*ppbNetData = (Byte_t *)pNetData;
			*pnNetDataLen = -1;
		}
		break;
	case Msg_CsAddGroupVerify:
		{
			NetMsg_t *pNetData = new NetMsg_t;
			if (!pNetData) return Error_OutOfMemory;
			AddGroupInfo_t *pAddgroup = (AddGroupInfo_t *)pLocalData;
		    pNetData->set_cmd(16);
			pNetData->set_type(pAddgroup->type);
			pNetData->set_uid(pAddgroup->strSenderName);
			pNetData->set_user_id(pAddgroup->nSenderId);
			pNetData->set_succ(pAddgroup->succ);

			Value json(objectValue);
			json["verifyInfo"] = pAddgroup->strVerify;
			DBGroup dbgroup;
			dbgroup.group_id = pAddgroup->groupinfo.group_id;
			dbgroup.name  = pAddgroup->groupinfo.name;
			json["group"] = buildGroupJson(dbgroup);
			//Assert(pAddgroup->pSenderLocalQInfo);
			//PacketFriend(&json["friend"], pAddgroup->pSenderLocalQInfo, Type_ImcFriend);
			buildDBUserJson(json["friend"], pAddgroup->userinfo);
			if (pAddgroup->type == GROUP_INFO_VERIFY) {
				//TODO
			}
			else { //GROUP_INFO_REPLY
				pNetData->set_tuid(pAddgroup->strAdminName);
				pNetData->set_tuser_id(pAddgroup->nAdminId);
			}

			pNetData->set_msg(getJsonStr(json));
			*ppbNetData   = (Byte_t*)pNetData;
			*pnNetDataLen = -1;
		}
		break;
	case Msg_CsModifyGroup:
		{
			NetMsg_t *pNetData = new NetMsg_t;
			GroupInfoData_t *pGroupInfo = (GroupInfoData_t *)pLocalData;
			if (!pNetData) return Error_OutOfMemory;
			pNetData->set_cmd(16);
			pNetData->set_uid(pGroupInfo->strSender);
			pNetData->set_user_id(pGroupInfo->nSender);
			pNetData->set_type(GROUP_INFO_MODIFY);
			pNetData->set_tuid(pGroupInfo->groupinfo.name);
			pNetData->set_tuser_id(pGroupInfo->groupinfo.group_id);
			*ppbNetData = (Byte_t *)pNetData;
			*pnNetDataLen = -1;
		}
		break;
	default:
		Assert(0);
		break;
	}

	return 0;
}
//free NetData
int CMainMsger::FreeData  (int nMsg, void * pLocalData)
{
	if(!pLocalData) return 0;

	switch(nMsg)
	{
	case Msg_CsQueryUsers:
		{
			delete (NetMsg_t*)pLocalData;
		}
		break;

	case Msg_CsQueryVerify:
		{
			delete (NetMsg_t*)pLocalData;
		}
		break;

	case Msg_CsResponseVerify:
		{
			NetMsg_t * pNetMsg = (NetMsg_t*)pLocalData;
			delete pNetMsg;
		}
		break;

	case Msg_CsTextChat:
		{
			//ChatCcTextData_t * pChatData = (ChatCcTextData_t*)pLocalData;
			//::free(pChatData);
			delete (NetMsg_t*)pLocalData;
		}
		break;
	case Msg_LoadMessage:
		{
			delete (NetMsg_t*)pLocalData;
		}
		break;

	case Msg_CsDelFriend:
	case Msg_CsEvilReport:
	case Msg_CsCreateGroup:
	case Msg_CsAddGroupVerify:
	case Msg_CsModifyGroup:
		{
			delete (NetMsg_t*)pLocalData;
		}
		break;
	}

	return 0;
}

int CMainMsger::NetToLocal(int nMsg, Byte_t * pbNetData,  int nNetDataLen,   void   ** ppbLocalData, int * pnLocalDataLen)
{
	// 以后版本不再往下分发消息. 
	// pnLocalDataLen 被改为 error code. 
	// ...???
	Json::Reader jsReader;
	Json::Value  jsRoot;
	int          nRet     = 0;
	int          nError   = 0;
	int          nMsgType = Type_ImcFriend;    // 下面没有考虑群和组的添加 ...???

	Assert(pbNetData);
	NetMsg_t * pNetResp = (NetMsg_t*)pbNetData;

	if(!pNetResp->msg().empty())
	{
		jsReader.parse(pNetResp->msg(), jsRoot);
	}

	#define BeginScMsgMap \
		switch(nMsg) { 
	#define ScMsgMap(_msg, _func) \
		case _msg: nError = _func(pNetResp, jsRoot, ppbLocalData, 0); break; 
	#define EndScMsgMap   \
		default: break; }

	BeginScMsgMap
		ScMsgMap(Msg_ScResponseUsers,  ParserSearchUser)
		ScMsgMap(Msg_ScResponseGroup,  ParserSearchGroup)
		ScMsgMap(Msg_ScQueryVerify,    ParserQueryVerify)
		ScMsgMap(Msg_ScResponseVerify, ParserResponseVerify)
		ScMsgMap(Msg_ScTextChat,       ParserTextChat)
        ScMsgMap(Msg_ScDelFriend,      ParserDelFriend)
		ScMsgMap(Msg_ScEvilReport,	   ParseReport)
		ScMsgMap(Msg_ScCreateGroup,    ParseCreateGroup)
		ScMsgMap(Msg_ScAddGroupVerify, ParseAddGroupVerify)
		ScMsgMap(Msg_ScModifyGroup,    ParseModifyGroup)
	EndScMsgMap

	*pnLocalDataLen = nError;
	return nRet;
}

int CMainMsger::FreeDataEx(int nMsg, void * pNetData)
{
	if(!pNetData) return 0;

	switch(nMsg)
	{
	case Msg_ScResponseUsers:
		{
			SearchScResponse_t * pSearchResult = (SearchScResponse_t*)pNetData;
			if(pSearchResult->pItemInfo && 
				pSearchResult->nItemSize > 0)
			{
				::free(pSearchResult->pItemInfo);
			}

			::free(pSearchResult);
		}
		break;

	case Msg_ScQueryVerify:
		{
			VerifyCcQuery_t * pVerifyQuery = (VerifyCcQuery_t*)pNetData;
			if(pVerifyQuery->pSenderNetQInfo) ::free(pVerifyQuery->pSenderNetQInfo);
			if(pVerifyQuery->szVerifyData)    ::free(pVerifyQuery->szVerifyData);
			::free(pVerifyQuery);
		}
		break;

	case Msg_ScResponseVerify:
		{
			VerifyCcResponse_t * pVerifyResp = (VerifyCcResponse_t*)pNetData;
			if(pVerifyResp->pSenderNetRInfo)  ::free(pVerifyResp->pSenderNetRInfo);
			::free(pVerifyResp);
		}
		break;

	case Msg_ScTextChat:
		{
			ChatCcTextData_t * pTextChat = (ChatCcTextData_t*)pNetData;
			if(pTextChat->szTime) ::free(pTextChat->szTime);
			if(pTextChat->szData) ::free(pTextChat->szData);
			::free(pTextChat);
		}
		break;
    case Msg_ScDelFriend:
        {
            delete (DelFriendItem*)pNetData;
        }
		break;
	case Msg_ScEvilReport:
		{
			delete (ReportCsEvilData_t *)pNetData;
		}
		break;
	case Msg_CsCreateGroup:
	case Msg_CsModifyGroup:
		delete (GroupInfoData_t *)pNetData;
		break;
	case Msg_ScAddGroupVerify:
		//TODO 在别处释放了
		break;
	}

	return 0;
}

int CMainMsger::ParserSearchUser(NetMsg_t * pNetMsg, Json::Value & jsRoot, void ** ppbLocalData, void * pUserData)
{
	SearchScResponse_t * pSearchResult = (SearchScResponse_t*)::calloc(sizeof(SearchScResponse_t), 1);
	if(!pSearchResult)  return Error_OutOfMemory;

	pSearchResult->nMatchType = pNetMsg->type() == Type_MatchGroup ? Type_ImcGroup : Type_ImcFriend;

	if(pNetMsg->succ() != 0 || pNetMsg->msg().empty()) return Error_SearchNoValidUser;
	if(jsRoot.empty())  return Error_InvalidNetData;

	NetItemInfo_t * pFriendList = (NetItemInfo_t *)::calloc(sizeof(NetItemInfo_t), jsRoot.size());
	if(!pFriendList)    return Error_OutOfMemory;

	pSearchResult->pItemInfo  = pFriendList;
	pSearchResult->nItemSize  = jsRoot.size();
	memcpy(pSearchResult->szSearchName, pNetMsg->tuid().c_str(), pNetMsg->tuid().size() > 255 ? 255 : pNetMsg->tuid().size());

	int i     = 0;
	for(Json::Value::iterator it =  jsRoot.begin(); 
		it != jsRoot.end(); it++, i++)
	{
		ParserFriend(&(*it), pFriendList + i, pSearchResult->nMatchType);
	}

	*ppbLocalData = pSearchResult;
	return 0;
}

int CMainMsger::ParserSearchGroup(NetMsg_t * pNetMsg, Json::Value & jsRoot, void ** ppbLocalData, void * pUserData) {
    SearchGroup_t *pSearchGroup = new SearchGroup_t;
	pSearchGroup->strSearchName = pNetMsg->tuid();
	if (pNetMsg->succ() == 0) {
		Value json = parseJsonStr(pNetMsg->msg());
		if (json.isObject() && check_arr_member(json, "groupinfos")) {
			Value groups = json["groupinfos"];
			for (size_t i = 0; i < groups.size(); i++) {
				pSearchGroup->groups.push_back(DBGroup());
				pSearchGroup->groups[i].group_id = groups[i]["group_id"].asInt();
				pSearchGroup->groups[i].name = groups[i]["name"].asString();
			}
		}
	}
	*ppbLocalData = pSearchGroup;
	return 0;
}

int CMainMsger::ParserQueryVerify(NetMsg_t * pNetMsg, Json::Value & jsRoot, void ** ppbLocalData, void * pUserData)
{
	// 对方发过来的加入请求. 
	VerifyScQuery_t * pVerifyQuery  = (VerifyScQuery_t*)::calloc(sizeof(VerifyScQuery_t), 1);
	if(!pVerifyQuery)                            return Error_OutOfMemory;
	if(pNetMsg->msg().empty() || jsRoot.empty()) return Error_InvalidNetData;

	int nMsgType = Type_ImcFriend;
	::strcpy_s(pVerifyQuery->szSenderNamex, sizeof(pVerifyQuery->szSenderNamex), pNetMsg->uid().c_str());
	::strcpy_s(pVerifyQuery->szRecverNamex, sizeof(pVerifyQuery->szRecverNamex), pNetMsg->tuid().c_str());

	if(IsValidOfJsonValue(jsRoot,  "id")   && 
		IsValidOfJsonValue(jsRoot, "tid")  && 
		IsValidOfJsonValue(jsRoot, "verifyinfo"))
	{
		pVerifyQuery->nRecvType   = nMsgType;
		pVerifyQuery->nSenderId   = MagicId_F(Msg_ScQueryVerify, jsRoot["id"].asInt());
		pVerifyQuery->nRecverId   = MagicId_t(Msg_ScQueryVerify, nMsgType, jsRoot["tid"].asInt());
		std::string strVerifyText = jsRoot["verifyinfo"].asString();
		if(!strVerifyText.empty()) ::strcpy(pVerifyQuery->szVerifyDatax, strVerifyText.c_str());
	}
	else                                         return Error_InvalidNetData;

	// 对于群组, 需要 new 群成员内存. ???
	int nCount = 1;
	pVerifyQuery->pSenderNetQInfo = (NetItemInfo_t*)::calloc(sizeof(NetItemInfo_t), 1);
	if(!pVerifyQuery->pSenderNetQInfo)           return Error_OutOfMemory;

	if(nMsgType == Type_ImcFriend && 
		IsValidOfJsonValue(jsRoot, "friend"))
	{
		ParserFriend(&jsRoot["friend"], pVerifyQuery->pSenderNetQInfo, nMsgType);
	}
	else                                         return Error_InvalidNetData;

	// 可能是群或者讨论组. 
	//Assert(pVerifyQuery->nRecverId == m_itemSelfInfo.nId);
	*ppbLocalData   = pVerifyQuery;
	return 0;
}

int CMainMsger::ParserResponseVerify(NetMsg_t * pNetMsg, Json::Value & jsRoot, void ** ppbLocalData, void * pUserData)
{
	// 对方对自己请求的答复. 
	VerifyCcResponse_t * pVerifyResult = (VerifyCcResponse_t*)::calloc(sizeof(VerifyCcResponse_t), 1);
	if(!pVerifyResult)                           return Error_OutOfMemory;
	if(pNetMsg->msg().empty() || jsRoot.empty()) return Error_InvalidNetData;

	int nMsgType = Type_ImcFriend;
	pVerifyResult->bIsAgree = pNetMsg->succ() == 0;
	::strcpy_s(pVerifyResult->szSenderName2, sizeof(pVerifyResult->szSenderName2), pNetMsg->uid().c_str());
	::strcpy_s(pVerifyResult->szRecverName2, sizeof(pVerifyResult->szRecverName2), pNetMsg->tuid().c_str());
	
	if(IsValidOfJsonValue(jsRoot,  "id") && 
		IsValidOfJsonValue(jsRoot, "tid"))
	{
		pVerifyResult->nRecvType = nMsgType;
		pVerifyResult->nSenderId = MagicId_t(Msg_ScResponseVerify, nMsgType, jsRoot["id"].asInt());
		pVerifyResult->nRecverId = MagicId_F(Msg_ScResponseVerify, jsRoot["tid"].asInt());
	}
	else                                     return Error_InvalidNetData;

	// 对于群组, 需要 new 群成员内存. ???
	int nCount = 1;
	pVerifyResult->pSenderNetRInfo = (NetItemInfo_t*)::calloc(sizeof(NetItemInfo_t), 1);
	if(!pVerifyResult->pSenderNetRInfo)      return Error_OutOfMemory;

	if(nMsgType == Type_ImcFriend && 
		IsValidOfJsonValue(jsRoot, "friend"))
	{
		ParserFriend(&jsRoot["friend"], pVerifyResult->pSenderNetRInfo, nMsgType);
	}
	else                                     return Error_InvalidNetData;

	// Assert(pVerifyResult->nRecverId == m_itemSelfInfo.nId);
	*ppbLocalData   = pVerifyResult;
	return 0;
}

int CMainMsger::ParserTextChat(NetMsg_t * pNetMsg, Json::Value & jsRoot, void ** ppbLocalData, void * pUserData)
{
	// 主界面总是先处理到数据. 
	ChatCcTextData_t * pChatText = (ChatCcTextData_t*)::calloc(sizeof(ChatCcTextData_t), 1);
	if(!pChatText)                               return Error_OutOfMemory;
	if(pNetMsg->msg().empty() || jsRoot.empty()) return Error_InvalidNetData;

	int nMsgType = Type_ImcFriend;
	::strcpy_s(pChatText->szSenderNamex, sizeof(pChatText->szSenderNamex), pNetMsg->uid().c_str());
	if(IsValidOfJsonValue(jsRoot, "id") && 
		IsValidOfJsonValue(jsRoot, "tid"))
	{
		pChatText->nRecvType = nMsgType;
		pChatText->nSenderId = MagicId_F(Msg_ScTextChat, jsRoot["id"].asInt());
		pChatText->nRecverId = MagicId_t(Msg_ScTextChat, TypeNetToLocal(pNetMsg->type()), jsRoot["tid"].asInt());
	}
	else return Error_InvalidNetData;

	if(IsValidOfJsonValue(jsRoot, "time"))
	{
		char * szTime = (char *)::calloc(32, 1);
		if(szTime) ::strcpy(szTime, jsRoot["time"].asString().c_str());
		pChatText->szTime = szTime;
	}

	int nTextLen = pNetMsg->buflen();
	if(nTextLen > 0)
	{
		char * szBuf = (char *)::calloc(nTextLen + 1, 1);
		if(szBuf) ::memcpy(szBuf, pNetMsg->buf(), nTextLen);
		pChatText->szData = szBuf;
	}

	pChatText->nRecvType = TypeNetToLocal(pNetMsg->type());

	*ppbLocalData = pChatText;
	return 0;
}

int CMainMsger::ParserDelFriend(NetMsg_t * pNetMsg, Json::Value & jsRoot, void ** ppbLocalData, void * pUserData) {
    DelFriendItem * pDelFriend = new DelFriendItem;
    pDelFriend->nSendId = pNetMsg->user_id();
    pDelFriend->nDelId = pNetMsg->tuser_id();
    pDelFriend->strSendName = pNetMsg->uid();
    pDelFriend->strdelName = pNetMsg->tuid();
	pDelFriend->succ = pNetMsg->succ();
	pDelFriend->type = pNetMsg->type();
    *ppbLocalData = pDelFriend;
    return 0;
}

int CMainMsger::ParseReport(NetMsg_t *pNetMsg, Json::Value &jsRoot, void **ppbLocalData, void *pUserData) {
	ReportCsEvilData_t *pReportData = new ReportCsEvilData_t;
	pReportData->succ = pNetMsg->succ();
	*ppbLocalData = pReportData;
	return 0;
}

int CMainMsger::ParseCreateGroup(NetMsg_t *pNetMsg, Json::Value &jsRoot, void **ppbLocalData, void *pUserData) {
	GroupInfoData_t *pGroupInfo = new GroupInfoData_t();
	pGroupInfo->succ = pNetMsg->succ();
	pGroupInfo->nSender = pNetMsg->user_id();//LocalId_t(Type_ImcFriend, pNetMsg->user_id());
	pGroupInfo->strSender = pNetMsg->uid();
	pGroupInfo->type = pNetMsg->type();
	Value json = parseJsonStr(pNetMsg->msg());
	if (!check_obj_member(json, "group") || parse_group(json["group"], pGroupInfo->groupinfo) < 0) {
		pGroupInfo->succ = -1;
	}
	else {
		//pGroupInfo->groupinfo.group_id = LocalId_t(Type_ImcGroup, pGroupInfo->groupinfo.group_id );
	}
	*ppbLocalData = pGroupInfo;
	return 0;
}

int CMainMsger::ParseAddGroupVerify(NetMsg_t *pNetMsg, Json::Value &jsRoot, void **ppbLocalData, void *pUserData) {
	AddGroupInfo_t *pAddgroup = new AddGroupInfo_t;
	if (!pAddgroup) return Error_OutOfMemory;
	
	*ppbLocalData = pAddgroup;

	pAddgroup->type = pNetMsg->type();
	pAddgroup->strSenderName = pNetMsg->uid();
	pAddgroup->nSenderId = pNetMsg->user_id();
	pAddgroup->strAdminName = pNetMsg->tuid();
	pAddgroup->nAdminId = pNetMsg->tuser_id();
	pAddgroup->succ = pNetMsg->succ();
	Value json = parseJsonStr(pNetMsg->msg());
	if (json.isObject() && json.isMember("verifyInfo")) {
		/*&& json.isMember("group_id") && json.isMember("group_name")) {*/
			pAddgroup->strVerify = json["verifyInfo"].asString();
			//pAddgroup->groupinfo.group_id = json["group_id"].asInt();
			//pAddgroup->groupinfo.name = json["group_name"].asString();
	}
	else {
		return Error_InvalidNetData;
	}
	//pAddgroup->pSenderLocalQInfo = (ItemNodeInfo_t *)::calloc(sizeof(ItemNodeInfo_t), 1);
	//if (!pAddgroup->pSenderLocalQInfo) return Error_OutOfMemory;
	if (json.isMember("friend") && json["friend"].isObject()) {
		//ParserFriend(&json["friend"], pAddgroup->pSenderLocalQInfo, Type_ImcGroup);
		parse_user(json["friend"], pAddgroup->userinfo);
	}
	if (json.isMember("group") && json["group"].isObject()) {
		parse_group(json["group"], pAddgroup->groupinfo);
	}
	else  return Error_InvalidNetData;
	return 0;
}

int CMainMsger::ParseModifyGroup(NetMsg_t *pNetMsg, Json::Value &jsRoot, void **ppbLocalData, void *pUserData) {
	GroupInfoData_t *pGroupInfo = new GroupInfoData_t();
	pGroupInfo->succ = pNetMsg->succ();
	pGroupInfo->nSender = (int)LocalId_t(Type_ImcFriend, pNetMsg->user_id());
	pGroupInfo->strSender = pNetMsg->uid();
	pGroupInfo->type = pNetMsg->type();
	pGroupInfo->groupinfo.name = pNetMsg->tuid();
	pGroupInfo->groupinfo.group_id = (int)LocalId_t(Type_ImcGroup, pNetMsg->tuser_id());
	*ppbLocalData = pGroupInfo;
	return 0;
}