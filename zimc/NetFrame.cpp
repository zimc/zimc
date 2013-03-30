#include "stdafx.h"
#include "NetFrame.h"
#include "conn.h"


class CNetClient
{
public:
	CNetClient()
		: m_conn(0)
		, m_bConnected(false)
	{}

	int  Init(std::string & strIp, std::string & strPort)
	{
		#define SEND_BUFFER_SIZE		1024 * 1024
		#define RECV_BUFFER_SIZE		1024 * 1024

		Assert(!m_conn);

		if(!strIp.empty() && !strPort.empty())
		{
			m_conn = new conn_t(
				SEND_BUFFER_SIZE, RECV_BUFFER_SIZE, strIp, strPort);
		}

		return m_conn ? 0 : -1;
	}

	int  Send(msg_t * msg)
	{
		Assert(msg);

		int nRet = 0;

		if(!m_bConnected)
		{
			if(!m_conn && (nRet = DomainImpl()))
			{
				return nRet;
			}

			if(!m_conn->connectToServer())
			{
				return -1;
			}

			m_bConnected = true;
		}

		nRet = m_conn->sendToServer(msg);
		return nRet > 0 ? 0 : -1;
	}

	void Dispatch()
	{
		if(!m_bConnected)
		{
			::Sleep(1000);
			return;
		}

		extern int  NetCallBackProxy(msg_t * msg);
		m_conn->dispatch(NetCallBackProxy, 1);
	}


private:
	int  DomainImpl()
	{
		Assert(!m_conn);

		#define IMC_SERV_DOMAIN		"chat.arebank.com"
		#define IMC_SERV_PORT		"30001"

		std::string strAddress, strPort;
		struct hostent * phe = ::gethostbyname(IMC_SERV_DOMAIN);
		if(phe)
		{
			unsigned long ulAddr  = *((unsigned long*)phe->h_addr_list[0]);
			strAddress = ::inet_ntoa(*(in_addr*)&ulAddr);
			strPort    = IMC_SERV_PORT;
		}

		if(!strAddress.empty())
		{
			unsigned long ulAddr = ::inet_addr(strAddress.c_str());
			phe = ::gethostbyaddr((const char *)&ulAddr, sizeof(ulAddr), AF_INET);
			if(!phe) strAddress.clear();
		}

		if(strAddress.empty())
		{
			return Error_NetworkFail; 
		}

		m_conn = new conn_t(
			SEND_BUFFER_SIZE, RECV_BUFFER_SIZE, strAddress, strPort);
		return m_conn ? 0 : -1;
	}


private:
	conn_t      * m_conn;
	bool          m_bConnected;
	std::string   m_strDomain;
};

int  NetCallBackProxy(msg_t * msg)
{
	CNetLayer * pNet = static_cast<CNetLayer*>(
		CNetLayer::GetInstance());

	Assert(pNet);
	Assert(msg);

	// ...
	int nNetMsg   = msg->cmd();
	int nMsgType  = (nNetMsg == 14 || nNetMsg == 16) ? msg->type() : 0;
	pNet->Dispatch(CmdNetToLocal(nNetMsg, nMsgType), (LPARAM)msg, -1);
	return 0;
}


int  CNetLayer::Init(void * pu)
{
	int nRet = 0;

	if(pu)   SetNetAddress(pu);

	if(!m_pNetClient)
	{
		m_pNetClient = new CNetClient();
		if(!m_pNetClient) return -1;

		m_pNetClient->Init(m_strAddress, m_strPort);
	}

	nRet = m_netThread.Start();
	if(nRet) return nRet;

	return nRet;
}

int  CNetLayer::Uninit()
{
	int nRet = 0;

	m_bExit = true;
	m_netThread.SafeStopX();

	if(m_pNetClient)
	{
		delete m_pNetClient;
		m_pNetClient = 0;
	}

	return nRet;
}

void CNetLayer::SetNetAddress(void * pu)
{
	LPWSTR * szArgList = (LPWSTR *)pu;
	m_strAddress = CW2A(szArgList[1]);
	m_strPort    = CW2A(szArgList[2]);
}

void CNetLayer::operator ()()
{
	while(!m_bExit)
	{
		Assert(m_pNetClient);
		m_pNetClient->Dispatch();
	}
}

int  CNetLayer::SendImMessage(int nMsg, LPARAM lp, WPARAM wp)
{
	Assert(m_pNetClient);

	int nRet = m_pNetClient->Send((msg_t *)lp);
	return nRet;
}

BOOL CNetLayer::IsValid(CBaseLayer * pLayer)
{
	for(CBaseLayerList::iterator it = m_upLayers.begin(); 
		it != m_upLayers.end(); it++)
	{
		if(pLayer == *it) return TRUE;
	}

	return FALSE;
}

