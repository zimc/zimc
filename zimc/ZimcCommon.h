#ifndef __ZimcCommon_H__
#define __ZimcCommon_H__


typedef unsigned char               Byte_t;
typedef std::basic_string<TCHAR>    tstring;


#ifndef Assert
	#define Assert                  _ASSERT
#endif

#define USE(FEATURE)                (defined USE_##FEATURE  && USE_##FEATURE)
#define ENABLE(FEATURE)             (defined ENABLE_##FEATURE  && ENABLE_##FEATURE)

#define DuiControl(_cls, _name)	            \
	static_cast<_cls*>(m_pmUi.FindControl(_name))

#define DuiGenericMessageMap(_name, _func)  \
	if(msg.pSender->GetName() == _name) { _func(msg); return; }

#define DuiClickButtonMap(_name, _func)     \
	DuiGenericMessageMap(_name, _func)

#define JsonArrayErea(_jsarr, _index)	    \
	{ \
		Assert(_index >= 0 && _index < (int)_jsarr.size()); \
		_jsarr[_index].swap(_jsarr[_jsarr.size() - 1]);	\
		_jsarr.resize(_jsarr.size() - 1); \
	}

#define IsValidOfJsonValue(_jsv, _name)     \
	(_jsv.isMember(_name) && !_jsv[_name].isNull())


struct NetItemInfo_t;
struct ItemNodeInfo_t;
int ParserFriend(void * jsFriend,  NetItemInfo_t  * pNetFriend,   int nType);
int PacketFriend(void * jsFriend,  ItemNodeInfo_t * pLocalFriend, int nType);


#define  USE_ZIP_SKIN 1
#define  _RSZ_WIN_LOG
#include "ZimcError.h"
#include "ZimcLogger.h"


#endif
