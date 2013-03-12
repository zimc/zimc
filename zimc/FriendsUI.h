#ifndef __FriendsUI_H__
#define __FriendsUI_H__


#include "BaseItemListUI.h"


class CFriendsUI : public CBaseItemListUI
{
public:
	CFriendsUI(CPaintManagerUI & pm)
		: CBaseItemListUI(pm)
	{}

	~CFriendsUI()
	{}
};


#endif
