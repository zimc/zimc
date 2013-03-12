#ifndef __ZimcHelper_H__
#define __ZimcHelper_H__


#include "ZimcCommon.h"


class CZimcHelper
{
public:
	static int ErrorMessageBox(HWND hWnd, int nMsg, int nErrorCode)
	{
		static CZiError g_ziError;

		char szErrorLog[256] = {0};
		_snprintf(szErrorLog, sizeof(szErrorLog) - 1, "程序错误, 错误信息 = %s(%d, %d)", 
			g_ziError.GetErrorMsgText(nMsg, nErrorCode), nMsg, nErrorCode);

		::MessageBoxA(hWnd, szErrorLog, "错误", 0);
		return 0;
	}

	static int GetScreenSize(int & nWidth, int & nHeigth)
	{
		DEVMODE devMode = {0};

		if(::EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &devMode))
		{
			nWidth  = devMode.dmPelsWidth;
			nHeigth = devMode.dmPelsHeight;
		}

		return 0;
	}

	static tstring GetCurrentTimeString()
	{
		SYSTEMTIME time             = {0};
		TCHAR      szTime[MAX_PATH] = {0};

		::GetLocalTime(&time);
		_stprintf_s(szTime, MAX_PATH, _T("%04d年%02d月%02d日 %02d:%02d:%02d"), 
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

		return tstring(szTime);
	}

	static void WideToMulti(const wchar_t* wide, std::string &multi)
	{
		int len = ::WideCharToMultiByte(CP_ACP, 0, wide, (int)::wcslen(wide), NULL, 0, NULL, NULL);
		if(len > 0)		// std::string
		{
			multi.resize(len);
			::WideCharToMultiByte(CP_ACP, 0, wide, -1, &multi[0], len, NULL, NULL);
		}
	}

	static void MultiToWide(const char* multi, std::wstring &wide)
	{
		int len = ::MultiByteToWideChar(CP_ACP, 0, multi, (int)::strlen(multi), NULL, 0);
		if(len > 0)		// std::wstring 
		{
			wide.resize(len);
			::MultiByteToWideChar(CP_ACP, 0, multi, -1, &wide[0], len);
		}
	}

	#ifdef _UNICODE
		#define ZI_T2A(_tstr, _multi)	CZimcHelper::WideToMulti(_tstr, _multi)
	#else
		#define ZI_T2A(_tstr, _multi)  (_multi = _tstr)
	#endif
};


#endif

