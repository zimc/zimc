#include "stdafx.h"
#include "LoginMsger.h"


int CLoginMsger::LocalToNet(int nMsg, void * pLocalData, int nLocalDataLen, Byte_t ** ppbNetData,   int * pnNetDataLen)
{
	Assert(nMsg != 0 && pLocalData && DWORD(nLocalDataLen) > 0);
	int nRet = 0;

	switch(nMsg)
	{
	case Msg_CsLogin:
		{
			LoginCsData_t * pLoginQuery = (LoginCsData_t*)pLocalData;

			NetMsg_t * pNetData = new NetMsg_t;
			if(pNetData)
			{
				// ???
				// m_strAccount = pLoginQuery->szAccount;

				pNetData->set_cmd(1);
				pNetData->set_uid(pLoginQuery->szAccount);

				Json::Value jsTmp;
				jsTmp["pwd"]  = pLoginQuery->szPassword;
				pNetData->set_msg(jsTmp.toStyledString());

				*ppbNetData   = (Byte_t *)pNetData;
				*pnNetDataLen = -1;
			}
			else
			{
				nRet = Error_OutOfMemory;
			}
		}
		break;
    case Msg_LoadMessage:
        {
        }
        break;
	}

	return nRet;
}

int CLoginMsger::NetToLocal(int nMsg, Byte_t * pbNetData,  int nNetDataLen,   void   ** ppbLocalData, int * pnLocalDataLen)
{
	Assert(nMsg != 0 && pbNetData && DWORD(nNetDataLen) > 0);
	int nRet = 0;

	switch(nMsg)
	{
	case Msg_ScLogin:
		{
			LoginScData_t * pLoginResult = (LoginScData_t*)::calloc(sizeof(LoginScData_t), 1);
			if(!pLoginResult) return Error_OutOfMemory;
			
			NetMsg_t    * pNetResp = (NetMsg_t*)pbNetData;
			//Assert(::strcmp(pNetResp->uid().c_str(), m_strAccount.c_str()) == 0);
			int nLoginStatus       = pNetResp->succ();

			if(!nLoginStatus)
			{
				do
				{
					Json::Reader  jsReader;
					Json::Value   jsTmp;
					bool bRet = jsReader.parse(pNetResp->msg(), jsTmp);
					if(!bRet) { nLoginStatus = Error_InvalidNetData; break; }

					// ������Ϣ
					if(IsValidOfJsonValue(jsTmp, "uid") && 
						IsValidOfJsonValue(jsTmp, "id"))
					{
						ParserFriend(&jsTmp, &pLoginResult->selfInfo, Type_ImcFriend);
					}

					// �����б�
					if(IsValidOfJsonValue(jsTmp, "friends"))
					{
						ParserTeams(&jsTmp["friends"], &pLoginResult->pTeamInfo, &pLoginResult->nTeamSize);
					}

					// Ⱥ�б�
					if(IsValidOfJsonValue(jsTmp, "groups"))
					{
						ParserGroups(&jsTmp["groups"], &pLoginResult->pGroupInfo, &pLoginResult->nGroupSize);
					}

					// �������б�
					if(IsValidOfJsonValue(jsTmp, "talks"))
					{
						ParserTalks(&jsTmp["talks"], &jsTmp["friends"], &jsTmp, &pLoginResult->pTalkInfo, &pLoginResult->nTalkSize);
					}
				}while(0);
			}

			*ppbLocalData   = pLoginResult;
			*pnLocalDataLen = -1;
			pLoginResult->nLoginStatus = nLoginStatus;
		}
		break;
	}

	return nRet;
}

int CLoginMsger::FreeData  (int nMsg, void * pLocalData) 
{ 
	if(!pLocalData) return 0;

	switch(nMsg)
	{
	case Msg_CsLogin:
		{
			NetMsg_t * pNetData = (NetMsg_t*)pLocalData;
			delete pNetData;
		}
		break;
	}

	return 0;
}

int CLoginMsger::FreeDataEx(int nMsg, void   * pNetData)
{
	if(!pNetData) return 0;

	switch(nMsg)
	{
	case Msg_ScLogin:
		{
			LoginScData_t * pLocalData = (LoginScData_t*)pNetData;
			if(pLocalData->pTeamInfo)
			{
				if(pLocalData->pTeamInfo->pFriendInfo)
				{
					::free(pLocalData->pTeamInfo->pFriendInfo);
				}

				::free(pLocalData->pTeamInfo);
			}

			if(pLocalData->pGroupInfo)
			{
				if(pLocalData->pGroupInfo->pFriendInfo)
				{
					::free(pLocalData->pGroupInfo->pFriendInfo);
				}

				::free(pLocalData->pGroupInfo);
			}

			::free(pLocalData);
		}
		break;
	}

	return 0;
}

int CLoginMsger::ParserTeams(void * jsTeams, NetItemInfo_t ** ppNetTeamList, int * pnNetTeamSize)
{
	Assert(jsTeams && ppNetTeamList && pnNetTeamSize);
	Json::Value   & jsTeamsx   = *(Json::Value *)jsTeams;

	// ������û�б���team, team �������ں����б���. 
	// 1 ����ȷ��team ������. 
	// 2 ���� team �� friends �ڴ�ռ�. 
	// 3 Ȼ����ȷ���������Ϣ

	// int ��ʾ size. 
	typedef std::vector<std::pair<std::string, int>> TeamsTmpList_t;
	TeamsTmpList_t  vecTeams;

	int             nRet       = 0;
	int             i          = 0;
	NetItemInfo_t * pTeamList1 = 0, 
	              * pTeamList2 = 0;

	vecTeams.push_back(std::make_pair(Team_DefaultNameA, 0));

	for(Json::Value::iterator it1 = jsTeamsx.begin(); 
		it1 != jsTeamsx.end(); it1++)
	{
		Assert(IsValidOfJsonValue((*it1), "type"));

		bool bInsert = true;
		std::string & strTeam = (*it1)["type"].asString();
		for(TeamsTmpList_t::iterator it2 = vecTeams.begin(); 
			it2 != vecTeams.end(); it2++)
		{
			if(::strcmp(strTeam.c_str(), it2->first.c_str()) == 0)
			{
				(it2->second)++;
				bInsert = false;
				break;
			}
		}

		if(bInsert)	vecTeams.push_back(std::make_pair(strTeam, 1));
	}

	Assert(!vecTeams.empty());
	pTeamList1 = (NetItemInfo_t*)::calloc(sizeof(NetItemInfo_t), vecTeams.size());
	if(!pTeamList1) return Error_OutOfMemory;

	*ppNetTeamList  = pTeamList1;
	*pnNetTeamSize  = vecTeams.size();

	for(TeamsTmpList_t::iterator it1 = vecTeams.begin(); 
		it1 != vecTeams.end(); it1++, i++)
	{
		pTeamList2  = 0;
		if(it1->second > 0)
		{
			pTeamList2 = (NetItemInfo_t*)::calloc(sizeof(NetItemInfo_t), it1->second);
			if(!pTeamList2) return Error_OutOfMemory;
		}

		// ����Ϣ  ****** 
		// ����ͬ���� Item ����. 1 Ϊ����ֵ. 
		pTeamList1[i].nItemType   = Type_ImcTeam;
		pTeamList1[i].nId         = LocalId_t(Type_ImcTeam, i);
		pTeamList1[i].strNickName = it1->first;

		pTeamList1[i].pFriendInfo = pTeamList2;
		pTeamList1[i].nFriendSize = 0;
	}

	for(Json::Value::iterator it1 = jsTeamsx.begin(); 
		it1 != jsTeamsx.end(); it1++)
	{
		Assert(IsValidOfJsonValue((*it1), "type"));

		int           j       = 0;
		std::string & strTeam = (*it1)["type"].asString();
		for(TeamsTmpList_t::iterator it2 = vecTeams.begin(); 
			it2 != vecTeams.end(); it2++, j++)
		{
			if(::strcmp(strTeam.c_str(), it2->first.c_str()) == 0)
			{
				nRet = ParserFriend(
					&(*it1), pTeamList1[j].pFriendInfo + pTeamList1[j].nFriendSize, Type_ImcFriend);
				if(nRet) return nRet;

				pTeamList1[j].nFriendSize++;
				Assert(pTeamList1[j].nFriendSize <= it2->second);
				break;
			}
		}
	}

	return 0;
}

int CLoginMsger::ParserGroups(void * jsGroups,  NetItemInfo_t ** ppNetGroups, int * pnNetGroupsSize)
{
	Assert(jsGroups && ppNetGroups && pnNetGroupsSize);
	Json::Value & jsGroupsx = *(Json::Value*)jsGroups;

	// Ⱥ�Ľ�����ͬ����, ���ڷ��������ѱ�����. 
	// Ⱥ��Ա��������Ⱥ��Ϣ��. 

	if(jsGroupsx.size() == 0) return 0;
	Assert(jsGroupsx.size() > 0);

	int             i            = 0;
	NetItemInfo_t * pGroupRoot   = 0, 
	              * pGroupMember = 0;

	pGroupRoot = (NetItemInfo_t*)::calloc(sizeof(NetItemInfo_t), jsGroupsx.size());
	if(!pGroupRoot) return Error_OutOfMemory;

	*ppNetGroups     = pGroupRoot;
	*pnNetGroupsSize = jsGroupsx.size();

	for(Json::Value::iterator it1 = jsGroupsx.begin(); 
		it1 != jsGroupsx.end(); it1++, i++)
	{
		// Ⱥ������Ϣ, ���ܱ仯 ???
		ParserFriend(&(*it1), &pGroupRoot[i], Type_ImcGroup);

		// Ⱥ�г�Ա��Ϣ
		if(IsValidOfJsonValue((*it1), "members"))
		{
			Json::Value & jsMember = (*it1)["members"];
			if(jsMember.empty()) continue;

			pGroupMember = (NetItemInfo_t*)::calloc(sizeof(NetItemInfo_t), jsMember.size());
			if(!pGroupMember) return Error_OutOfMemory;

			pGroupRoot[i].pFriendInfo = pGroupMember;
			pGroupRoot[i].nFriendSize = jsMember.size();

			int j = 0, nRet = 0;
			for(Json::Value::iterator it2 = jsMember.begin(); 
				it2 != jsMember.end(); it2++, j++)
			{
				nRet = ParserFriend(&(*it2), &pGroupMember[j], Type_ImcFriendX);
				if(nRet) return nRet;
			}
		}
	}

	return 0;
}

int CLoginMsger::ParserTalks(void * jsTalks, void * jsFriends, void * jsSelf, NetItemInfo_t ** ppNetTalks, int * pnNetTalksSize)
{
	Assert(jsTalks && jsFriends && ppNetTalks && pnNetTalksSize);
	Json::Value & jsTalksx   = *(Json::Value *)jsTalks;
	Json::Value & jsFriendsx = *(Json::Value *)jsFriends;
	Json::Value & jsSelfx    = *(Json::Value *)jsSelf;

	if(jsTalksx.size() == 0 || jsFriendsx.size() == 0) return 0;
	Assert(jsTalksx.size() > 0 && jsFriendsx.size() > 0);

	// ������Ľ�����Ⱥ��ͬ
	// ���Ա�����ں����б���. 

	int             i           = 0;
	NetItemInfo_t * pTalkRoot   = 0, 
	              * pTalkMember = 0;

	pTalkRoot = (NetItemInfo_t*)::calloc(sizeof(NetItemInfo_t), jsTalksx.size());
	if(!pTalkRoot) return Error_OutOfMemory;

	*ppNetTalks     = pTalkRoot;
	*pnNetTalksSize = jsTalksx.size();

	for(Json::Value::iterator it1 = jsTalksx.begin(); 
		it1 != jsTalksx.end(); it1++, i++)
	{
		// �޳�Ա����Ч. 
		if(!IsValidOfJsonValue((*it1), "members") && 
			(*it1)["members"].size() <= 0)
		{
			continue;
		}

		// ��������Ϣ, ???
		ParserFriend(&(*it1), &pTalkRoot[i], Type_ImcTalk);

		// �������Ա��Ϣ
		Json::Value & jsMember   = (*it1)["members"];
		pTalkMember              = (NetItemInfo_t*)::calloc(sizeof(NetItemInfo_t), jsMember.size());
		if(!pTalkMember) return Error_OutOfMemory;

		int j = 0, nRet = 0;
		for(Json::Value::iterator it2 = jsMember.begin(); 
			it2 != jsMember.end(); it2++, j++)
		{
			// ���������ص����� it2 �а����� member ����ϸ��Ϣ, ��ʱ�����˼·��. (ע�ʹ� Friends �л�ȡ������)
			// ��Ϊ���ܺ�����Ч member, ���类ɾ����. 
			// uid, id
			int nId = (*it2)["id"].asInt();
			//for(Json::Value::iterator it3 = jsFriendsx.begin(); 
			//	it3 != jsFriendsx.end(); it3++)
			{
				//if(nId == (*it3)["id"].asInt())
				{
					nRet = ParserFriend(&(*it2), &pTalkMember[j], Type_ImcFriend);
					if(nRet) return nRet;
					//break;
				}
			}
		}

		pTalkRoot[i].pFriendInfo = pTalkMember;
		pTalkRoot[i].nFriendSize = jsMember.size();
	}

	return 0;
}


// --------------------------------------
int ParserFriend(void * jsFriend,  NetItemInfo_t * pNetFriend, int nType)
{
	Assert(jsFriend && pNetFriend);
	Json::Value & jsFriendx = *(Json::Value *)jsFriend;

	pNetFriend->nItemType    = nType;
	pNetFriend->nId          = LocalId_t(nType, jsFriendx["id"].asInt());
	pNetFriend->strNickName  = jsFriendx["uid"].asString();

#ifdef _DEBUG
	{
		char   szTmp[64]    = {0};
		sprintf_s(szTmp, sizeof(szTmp), "%s_%d_%d", 
			pNetFriend->strNickName.c_str(), nType, pNetFriend->nId);
		pNetFriend->strDescription    = szTmp;
	}
#endif

	// nAge;          \
	// RealName);     \
	// Address);      \
	// Sex);          \
	// Height);       \
	// NativePlace);  \
	// Birthday);     \
	// Position);     \
	// Studies);      \
	// Experience);   \
	// Evaluation);   \

	switch(nType)
	{
	case Type_ImcFriend:
	case Type_ImcFriendX:
		{
			pNetFriend->nAdminId      = 0;		// ���ڷ��������ַ�����Ϊ����ж�����, ����������. 
			pNetFriend->strAdminName  = jsFriendx["type"].asString();
			pNetFriend->strPicture    = jsFriendx["headurl"].asString();
			pNetFriend->strRealName	  = jsFriendx["name"].asString();
			pNetFriend->strSex		  = jsFriendx["sex"].asString();
			pNetFriend->strHeight     = jsFriendx["height"].asString();
			pNetFriend->strNativePlace= jsFriendx["place"].asString();
			pNetFriend->strBirthday   = jsFriendx["birthday"].asString();
			pNetFriend->strPosition   = jsFriendx["job"].asString();
			//pNetFriend->strStudies    = jsFriendx[""]
			pNetFriend->strExperience  = jsFriendx["experience"].asString();
			pNetFriend->strEvaluation = jsFriendx["selfdescription"].asString();
			pNetFriend->nAddFriendType = jsFriendx["invited"].asInt();
		}
		break;

	case Type_ImcGroup:
		{
			pNetFriend->nGroupAdminId = LocalId_t(Type_ImcFriend, jsFriendx["admin"].asInt());
			pNetFriend->strNotify     = jsFriendx["notify"].asString();
			pNetFriend->strPicture    = jsFriendx["headurl"].asString();
		}
		break;
	}

	// ...

	return 0;
}

int PacketFriend(void * jsFriend,  ItemNodeInfo_t * pLocalFriend, int nType)
{
	Assert(jsFriend && pLocalFriend);
	Assert(pLocalFriend->chType == nType);

	Json::Value & jsFriendx = *(Json::Value*)jsFriend;
	jsFriendx["id"]   = (int)NetId_t(nType, pLocalFriend->nId);
	jsFriendx["uid"]  = (LPCSTR)CT2A(pLocalFriend->tstrNickName.c_str());

	switch(nType)
	{
	case Type_ImcFriend:
		jsFriendx["type"] = (LPCSTR)CT2A(pLocalFriend->tstrAdminName.c_str());
		jsFriendx["name"] = (LPCSTR)CT2A(pLocalFriend->tstrRealName.c_str());
		jsFriendx["sex"]  = (LPCSTR)CT2A(pLocalFriend->tstrSex.c_str());
		break;

	case Type_ImcGroup:
		break;

	case Type_ImcTalk:
		break;

	default:
		Assert(0);
		break;
	}

	return 0;
}
