#ifndef __NodeList_H__
#define __NodeList_H__


#include <math.h>
#include <vector>
#include "UiData.h"


inline double CalculateDelay(double dState)
{
	return ::pow(dState, 2);
}


class CNodeList
{
public:
	CNodeList() : m_pParentList(0) {}
	explicit CNodeList(NodeData_t & t) : m_pParentList(0), m_nodeData(t) {}
	CNodeList(NodeData_t & t, CNodeList * pParent) : m_pParentList(pParent), m_nodeData(t) {}
	~CNodeList()
	{
		for(size_t i = 0; i < m_vecChildren.size(); i++) delete m_vecChildren[i];
		m_vecChildren.clear();
	}

	NodeData_t & GetNodeData()           { return m_nodeData; }
	int          GetChildrenSize() const { return m_vecChildren.size(); }
	CNodeList  * GetChild(int i)         { return m_vecChildren[i]; }
	CNodeList  * GetParent()             { return m_pParentList; }
	bool         IsFolder()        const { return m_nodeData.bIsFolder; }
	bool         IsHasChildren()   const { return !m_vecChildren.empty(); }
	CNodeList  * GetLastChild()          { return m_vecChildren.empty() ? this : m_vecChildren[m_vecChildren.size() - 1]->GetLastChild(); }
	void         SetParent(CNodeList * pParentList) { m_pParentList = pParentList; }

	void         AddChild(CNodeList * pChild)
	{
		Assert(pChild);
		pChild->SetParent(this); m_vecChildren.push_back(pChild);
	}

	void         RemoveChild(CNodeList * pChild)
	{
		Assert(pChild);
		for(NodeListChildren_t::iterator it = m_vecChildren.begin(); 
			it != m_vecChildren.end(); it++)
		{
			if(*it == pChild) { m_vecChildren.erase(it); return; }
		}
	}


private:
	typedef std::vector<CNodeList *>   NodeListChildren_t;

	NodeListChildren_t	 m_vecChildren;
	CNodeList          * m_pParentList;
	NodeData_t           m_nodeData;
};


#endif   // __UiList_H__
