#if defined(_TRACE)
#	undef _TRACE
#else // _TRACE

#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>

#ifndef _MAX_TRACE_LOG_LEN
#define _MAX_TRACE_LOG_LEN	512
#endif // _MAX_TRACE_LOG_LEN

inline void __cdecl _eXchangeTraceLog(LPCTSTR pszFormat, ...)
{
	TCHAR szBuffer[_MAX_TRACE_LOG_LEN];

	va_list ptr;
	va_start(ptr, pszFormat);

	if (_vstprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), pszFormat, ptr) > 0)
		OutputDebugString(szBuffer);

	va_end(ptr);
}

#endif // _TRACE

#if defined(_DEBUG)
#	define _TRACE	_eXchangeTraceLog
#else
#	if defined(FORCE_TRACE)
#		define _TRACE	_eXchangeTraceLog
#		pragma message("warning: _TRACE in Release Build! ")
#	else
#		define _TRACE	__noop
#	endif
#endif
