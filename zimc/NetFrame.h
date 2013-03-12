#ifndef __NetFrame_H__
#define __NetFrame_H__


#include "UiData.h"
#include <list>
#include <string>
#include "SimplifyThread.h"
#include "msg.h"


// ------------------------------------------------------------------
// �ֲ��������. 
// --  �����:     ��������Ϣ����, ֻ�������ݽ��շ���. 
//     ��Ϣ�ַ���: ��������Ϣ����, ֻ��ȡ��Ϣ���ͽ��зַ�. Ҳ����˵����Ҫ֪����Ϣ���͵Ĺ���. 
//     ��Ϣ�����: ���������Ϣ, ������Ϣ���ݺ���
// ͳһ��Ϣ�ص��ӿڶ���. 
//
// 1 ��Ϣ����װ���� pbData ��, 
//   ����������������������·�װ. 
// 2 ��Ϣ�����ڸ�����Ϣ�ṹ���е�λ��ʱ�̶���. 
//   ����Ҫ��ÿһ����Ϣ���н��н���. 
// 3 ���㲻�ٷַ���Ϣ. 
// 4 �ײ㲻�ٷ�����Ϣ. 
// 
// �Ż���: 
// 1 ���Ϊ 2 ��: 
//   -- ����� -> ��Ϣ�����. 
// 


// ------------------------------------------------------------------
// ͨ�ò���
class CBaseLayer
{
public:
	CBaseLayer()
		: m_pUpLayer(0)
		, m_pDownLayer(0)
	{}

	virtual ~CBaseLayer()
	{}

	virtual int  Init(void *)
	{
		return 0;
	}

	virtual int  Uninit()
	{
		return 0;
	}

	virtual int  SendImMessage(int nMsg, LPARAM lp, WPARAM wp)
	{
		int nRet = 0;
		if(m_pDownLayer)
		{
			nRet = m_pDownLayer->SendImMessage(nMsg, lp, wp);
		}

		return nRet;
	}

	virtual int  Dispatch(int nMsg, LPARAM lp, WPARAM wp)
	{
		int nRet = 0;
		if(m_pUpLayer)
		{
			nRet = m_pUpLayer->Dispatch(nMsg, lp, wp);
		}

		return nRet;
	}

	virtual void SetUpLayer(CBaseLayer * pUpLayer)
	{
		m_pUpLayer = pUpLayer;
	}

	virtual void SetDownLayer(CBaseLayer * pDownLayer) 
	{
		m_pDownLayer = pDownLayer;
	}


protected:
	CBaseLayer * m_pUpLayer;
	CBaseLayer * m_pDownLayer;
};


// ------------------------------------------------------------------
// �����
// ��Ϣ��

typedef msg_t NetMsg_t;
class   CNetClient;

class CNetLayer
	: public CBaseLayer
{
public:
	CNetLayer()
		: CBaseLayer()
		, m_bExit(false)
		, m_netThread(this)
	{}

	virtual ~CNetLayer()
	{}

	void UnsetUpLayer(CBaseLayer * pUpLayer)
	{
		Assert(pUpLayer);

		CGenericLockHandler h(m_netLock);
		for(CBaseLayerList::iterator it = m_upLayers.begin(); 
			it != m_upLayers.end(); it++)
		{
			if(pUpLayer == *it)
			{
				m_upLayers.erase(it);
				break;
			}
		}
	}

	void SetUpLayer(CBaseLayer * pUpLayer)
	{
		Assert(pUpLayer);

		CGenericLockHandler h(m_netLock);
		m_upLayers.push_back(pUpLayer);
	}

	int  Dispatch(int nMsg, LPARAM lp, WPARAM wp)
	{
		int nRet = 0;

		{
			CBaseLayerList listTmp;
			{
				CGenericLockHandler h(m_netLock);
				listTmp = m_upLayers;
			}

			// �ж��߳�ͬ��������. 
			for(CBaseLayerList::iterator it = listTmp.begin(); 
				it != listTmp.end(); it++)
			{
				Assert(*it);

				// �˴��� login ʱ�ᷢ������. ???
				// �޸�login ����, ���������. 
				CGenericLockHandler h(m_netLock);
				if(IsValid(*it))
				{
					nRet = (*it)->Dispatch(nMsg, lp, wp);
					if(!nRet) break;
				}
			}
		}


		return nRet;
	}

	static CBaseLayer * GetInstance()
	{
		static CNetLayer g_netLayer;
		return &g_netLayer;
	}


public:
	int  Init(void *);
	int  Uninit();

	int  SendImMessage(int nMsg, LPARAM lp, WPARAM wp);
	void SetNetAddress(void *);
	void operator ()();

	BOOL IsValid(CBaseLayer * pLayer);


private:
	typedef std::list<CBaseLayer *> CBaseLayerList;
	CBaseLayerList    m_upLayers;

	std::string       m_strAddress;
	std::string       m_strPort;

	CNetClient      * m_pNetClient;

	bool              m_bExit;
	CSimplifyThread<CNetLayer> m_netThread;
	CCriticalSetionObject      m_netLock;
};


// ------------------------------------------------------------------
// ��Ϣ������
class CMsgLayer
	: public CBaseLayer
{
public:
	CMsgLayer()
		: CBaseLayer()
	{}

	virtual ~CMsgLayer()
	{}

	virtual int  Init(void * pu = 0)
	{
		int nRet = 0;

		// ע��: CNetLayer ��ʼ�������ٴ�֮ǰ. ******
		CNetLayer::GetInstance()->SetUpLayer(this);
		SetDownLayer(CNetLayer::GetInstance());

		return  nRet;
	}

	virtual int Uninit()
	{
		static_cast<CNetLayer*>(CNetLayer::GetInstance())->
			UnsetUpLayer(this);
		return 0;
	}

	int SendImMessage(int nMsg, LPARAM lp, WPARAM wp)
	{
		int nRet = 0;
		if(m_pDownLayer)
		{
			Byte_t * pbNetData   = 0;
			int      nNetDataLen = 0;

			Assert(nMsg != 0);
			nRet = LocalToNet(nMsg, (void*)lp, wp, &pbNetData, &nNetDataLen);
			if(nRet) return nRet;

			nRet = m_pDownLayer->SendImMessage(nMsg, (LPARAM)pbNetData, nNetDataLen);
			FreeData(nMsg, pbNetData);
		}

		return nRet;
	}

	int Dispatch(int nMsg, LPARAM lp, WPARAM wp)
	{
		// ����. 
		int    nRet          = 0;
		void * pLocalData    = 0;
		int    nLocalDataLen = 0;

		Assert(nMsg != 0);
		nRet = NetToLocal(nMsg, (Byte_t *)lp, wp, &pLocalData, &nLocalDataLen);
		if(nRet) return nRet;

		nRet = OnDispatch(nMsg, (LPARAM)pLocalData, nLocalDataLen);
		FreeData(nMsg, pLocalData);

		return nRet;
	}

	// ����ͨ��ʵ�ַ���: 
	// switch(nMsg)
	// {
	//     case Msg_XXX: ... break;
	// }
	virtual int OnDispatch(int nMsg, LPARAM lp, WPARAM wp) = 0;
	virtual int LocalToNet(int nMsg, void   * pLocalData, int nLocalDataLen, Byte_t ** ppbNetData,   int * pnNetDataLen)   = 0;
	virtual int NetToLocal(int nMsg, Byte_t * pbNetData,  int nNetDataLen,   void   ** ppbLocalData, int * pnLocalDataLen) = 0;
	virtual int FreeData  (int nMsg, void   * pbData) = 0;
};


#endif
