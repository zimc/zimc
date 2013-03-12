#ifndef __RszTLogger_H__
#define __RszTLogger_H__


#define RSZLOG_TEST          "[L0] "
#define RSZLOG_NOTICE        "[L1] "
#define RSZLOG_WARNING       "[L2] "
#define RSZLOG_ERROR         "[L8] "


#ifdef  _RSZ_WIN_LOG
	#include <stdio.h>
	#include <windows.h>
	#include <string>


	#define RszPrintf(_file_handler, _format, ...)	\
		{ \
			if(_file_handler) ::fprintf(_file_handler, _format, __VA_ARGS__); \
			else              ::fprintf(stdout, _format, __VA_ARGS__); \
		}

	#define RszLogger(_file_handler, _level, _format, ...)	\
		{ \
			SYSTEMTIME stNow; \
			::GetLocalTime(&stNow); \
			RszPrintf(_file_handler, "%u-%02u-%02u %02u:%02u:%02u ", stNow.wYear , stNow.wMonth , stNow.wDay , stNow.wHour ,stNow.wMinute, stNow.wSecond); \
			RszPrintf(_file_handler, _level##_format, __VA_ARGS__); \
			RszPrintf(_file_handler, "\n"); \
		}

	#define RszFlogger(_file_handler, _file_name, _level, _format, _mode, ...) \
		{ \
			if(!_file_handler) ::fopen_s(&_file_handler, _file_name, _mode); \
			RszLogger(_file_handler, _level, _format, __VA_ARGS__); \
			::fflush(_file_handler); \
		}

	#define RszDlogger(_level, _format, ...) \
		{ \
			RszLogger(0, _level, _format, __VA_ARGS__); \
		}

	#define RszFlogger1(_file_name, _level, _format, ...)	\
		{ \
			RszFlogger(g_fxLogger.pf, _file_name, _level, _format, "a", __VA_ARGS__); \
			g_fxLogger.Update(_file_name); \
			g_fxLogger.Close(); \
		}

	#define RszFlogger2(_file_name, _level, _format, ...)	\
		{ \
			RszFlogger(g_fxLogger.pf, _file_name, _level, _format, "w", __VA_ARGS__); \
			g_fxLogger.Update(_file_name); \
		}

	#define RszTLogger(_file_name, _level, _format, ...)	\
		{ \
			RszFlogger1(_file_name, _level, _format, __VA_ARGS__); \
		}


	typedef struct LogFileEx_t
	{
		#define LOG_FILE_MAX_SIZE	        1024 * 1024		// 1M

		FILE      * pf;

		static LogFileEx_t & GetInstance()  { static LogFileEx_t obj; return obj; }
		void   Close()                      { if(pf) { ::fclose(pf); pf = 0; } }
		void   Update(const char * szName)  { if(pf && ::ftell(pf) > LOG_FILE_MAX_SIZE) { 
			static std::string g_strName, g_strBakName; 
			if(g_strName.empty()) { g_strName = szName; g_strBakName = g_strName + ".log"; }
			::remove(g_strBakName.c_str()); ::rename(g_strName.c_str(), g_strBakName.c_str()); }
		}
		
		const char * LogName()              { static const char * g_szLogName = 0; 
			if(g_szLogName)                 { return g_szLogName; }
			static char g_szLogPath[MAX_PATH] = {0}; if(!::GetModuleFileNameA(0, g_szLogPath, sizeof(g_szLogPath))) return 0;
			::strcat_s(g_szLogPath, sizeof(g_szLogPath), ".log"); return g_szLogName = g_szLogPath;
		}


	protected:
		LogFileEx_t() : pf(0)               { }
		~LogFileEx_t()                      { if(pf) ::fclose(pf); }
	}LogFileEx_t;


	#define g_fxLogger                      LogFileEx_t::GetInstance()
	#define g_fxLoggerName                  g_fxLogger.LogName()
#endif


#ifdef _RSZ_WIN_LOG
	#define ZiLogger(_level, _format, ...)  \
		{ \
			RszTLogger(g_fxLoggerName, _level, _format, __VA_ARGS__); \
		}
#else 
	#define ImcLogger(_level, _format, ...) 
#endif


#endif
