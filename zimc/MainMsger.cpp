#include "stdafx.h"
#include "MainMsger.h"


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
            *ppbNetData = (Byte_t *)pNetData;
            *pnNetDataLen = -1;
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
		ScMsgMap(Msg_ScQueryVerify,    ParserQueryVerify)
		ScMsgMap(Msg_ScResponseVerify, ParserResponseVerify)
		ScMsgMap(Msg_ScTextChat,       ParserTextChat)
        ScMsgMap(Msg_ScDelFriend,      ParserDelFriend)
	EndScMsgMap

	*pnLocalDataLen = nError;
	return nRet;
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
			ChatCcTextData_t * pChatData = (ChatCcTextData_t*)pLocalData;
			::free(pChatData);
            //delete (NetMsg_t*)pLocalData;
		}
    case Msg_LoadMessage:
        {
            delete (NetMsg_t*)pLocalData;
        }
		break;
    case Msg_CsDelFriend:
        {
            delete (NetMsg_t*)pLocalData;
        }
        break;
	}

	return 0;
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

	int i     = 0;
	for(Json::Value::iterator it =  jsRoot.begin(); 
		it != jsRoot.end(); it++, i++)
	{
		ParserFriend(&(*it), pFriendList + i, pSearchResult->nMatchType);
	}

	*ppbLocalData = pSearchResult;
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
	else                                         return Error_InvalidNetData;

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

	pChatText->nRecvType = pNetMsg->type();

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
    *ppbLocalData = pDelFriend;
    return 0;
}
