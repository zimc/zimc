#ifndef __GroupsUI_H__
#define __GroupsUI_H__


#include "BaseItemListUI.h"


class CGroupsUI : public CBaseItemListUI
{
public:
	CGroupsUI(CPaintManagerUI & pm)
		: CBaseItemListUI(pm)
	{}

	~CGroupsUI()
	{}
};


#endif
