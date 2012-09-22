#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>

#define DEFAULT_LOGNAME _T("ELCDebug.log")

#ifndef _MAX_DEBUG_LOG_LEN
#	define _MAX_DEBUG_LOG_LEN	512
#endif // _MAX_DEBUG_LOG_LEN

enum ELC_DEBUG_LOG_TYPE {
	ELC_DLT_NONE = 0,
	ELC_DLT_ONLY_TRACE,
	ELC_DLT_ONLY_LOGFILE,
	ELC_DLT_BOTH,

	ELC_DLT_DEFAULT = ELC_DLT_BOTH,
};

class CELCDebugLog
{
public:
	CELCDebugLog()
	{
		memset(m_szLogPath, 0, sizeof(m_szLogPath));
		memset(m_szPrefix, 0, sizeof(m_szPrefix));
		m_bAddTimestamp = TRUE;
		m_nTraceType = ELC_DLT_DEFAULT;
	};

	CELCDebugLog(LPCTSTR lpszFilePath, BOOL bAddTimestamp=TRUE, ELC_DEBUG_LOG_TYPE type=ELC_DLT_DEFAULT)
	{
		m_bAddTimestamp = bAddTimestamp;
		m_nTraceType = type;

		if (lpszFilePath)
			_tcscpy(m_szLogPath, lpszFilePath);
		else
			memset(m_szLogPath, 0, sizeof(m_szLogPath));

		memset(m_szPrefix, 0, sizeof(m_szPrefix));
	}

	virtual ~CELCDebugLog() {
	}

public:
	// Set logfile's path
	// u can set log path at construct function.
	// also u can change it here
	void SetLogPath(LPCTSTR lpszFilePath) 
	{
		if (lpszFilePath)
			_tcscpy(m_szLogPath, lpszFilePath);
		else 
			memset(m_szLogPath, 0, sizeof(m_szLogPath));
	}

	// enable/disable timestamp
	void EnableTimestamp(BOOL bEnable) {
		m_bAddTimestamp = bEnable;
	}

	// set debug info output type
	void DbgOutputType(ELC_DEBUG_LOG_TYPE type) {
		m_nTraceType = type;
	}

	// prefix
	void SetPrefixText(LPCTSTR lpszPrefix) {
		if (lpszPrefix)
			_tcscpy(m_szPrefix, lpszPrefix);
		else
			memset(m_szPrefix, 0, sizeof(m_szPrefix));
	}

	// write debug string into log or output trace
	BOOL __cdecl DbgPrint(LPCTSTR pszFormat, ...) 
	{
		if (m_nTraceType == ELC_DLT_NONE)
			return FALSE;

		TCHAR szBuffer[_MAX_DEBUG_LOG_LEN] = {0};

		va_list ptr;
		va_start(ptr, pszFormat);

		BOOL bRet = FALSE;
		if (_vstprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), pszFormat, ptr) > 0)
			bRet = DbgOutput(szBuffer);

		va_end(ptr);

		return bRet;
	}

private:
	BOOL DbgOutput(LPCTSTR lpszText)
	{
		if (m_nTraceType == ELC_DLT_ONLY_TRACE) {
			OutputDebugString(lpszText);
			return TRUE;
		}
		else if (m_nTraceType == ELC_DLT_ONLY_TRACE) {
			OutputDebugString(lpszText);
		}
		
		if (_tcslen(m_szLogPath) == 0) 
			return FALSE;

		DWORD cbWritten = 0;
		DWORD dwCreation = OPEN_EXISTING;

		if (_taccess(m_szLogPath, 0) != 0)
			dwCreation = CREATE_NEW;

		HANDLE  hFile = CreateFile(m_szLogPath, 
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ, 
			NULL, 
			dwCreation, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

		if (INVALID_HANDLE_VALUE == hFile) {
			DWORD dwError = GetLastError();
			return FALSE;
		}

#ifdef UNICODE
		if (CREATE_NEW == dwCreation) {
			// Create unicode file
			WORD wFlag = 0xfeff;
			WriteFile(hFile, &wFlag, sizeof(WORD), &cbWritten, NULL);
		}
#endif // UNICODE

		SetFilePointer(hFile, 0, NULL, FILE_END);

		if (m_bAddTimestamp) {
			SYSTEMTIME systime;
			GetLocalTime(&systime);
			TCHAR szTime[100] = { 0 };
			wsprintf(szTime, _T("%04d-%02d-%02d %02d:%02d:%02d.%03d -- "), 
				systime.wYear,
				systime.wMonth,
				systime.wDay,
				systime.wHour, 
				systime.wMinute, 
				systime.wSecond, 
				systime.wMilliseconds); 
			WriteFile(hFile, szTime, (_tcslen(szTime)) * sizeof(TCHAR), &cbWritten, NULL);
		}

		if (_tcslen(m_szPrefix) > 0) {
			WriteFile(hFile, m_szPrefix, (_tcslen(m_szPrefix)) * sizeof(TCHAR), &cbWritten, NULL);
		}

		WriteFile(hFile, lpszText, (_tcslen(lpszText)) * sizeof(TCHAR), &cbWritten, NULL);
		WriteFile(hFile, _T("\r\n"), (_tcslen(_T("\r\n"))) * sizeof(TCHAR), &cbWritten, NULL);

		CloseHandle(hFile);
		return TRUE;
	}

private:
	TCHAR	m_szLogPath[MAX_PATH];
	TCHAR	m_szPrefix[_MAX_DEBUG_LOG_LEN];
	BOOL	m_bAddTimestamp;
	int		m_nTraceType;
};
