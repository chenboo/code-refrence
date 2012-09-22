#ifndef __LIBTDPROCHOOK64_H__
#define __LIBTDPROCHOOK64_H__

#ifdef LIBTDPROCHOOK64_EXPORTS
#define LIBTDPROCHOOK64_API __declspec(dllexport)
#else
#define LIBTDPROCHOOK64_API __declspec(dllimport)
#endif

#include <windows.h>

extern "C"
{
	LIBTDPROCHOOK64_API BOOL TDProcHookInit(HANDLE hProcess);
	LIBTDPROCHOOK64_API void TDProcHookDone();
	LIBTDPROCHOOK64_API BOOL TDProcHookEnableTerminate(BOOL bEnable, DWORD dwProcID);
	LIBTDPROCHOOK64_API BOOL TDProcHookSetDenyReportExecPath(LPCSTR lpszExecPath);
	LIBTDPROCHOOK64_API BOOL TDProcHookSetExecuteRule(PTDPROCHOOK_EXECUTE_RULE pExecRule);
	LIBTDPROCHOOK64_API BOOL TDProcHookGetExeCheckSum(LPCTSTR lpszExecPath,	DWORD *pdwCheckSum);
	LIBTDPROCHOOK64_API BOOL TDProcHookSetUsbNoExcuteRule(LPCSTR lpszDriveList);
	LIBTDPROCHOOK64_API BOOL TDProcHookSetDeviceName(PTDPROCHOOK_DEVICE_DRIVE pDriversInfo);
}

#endif//__LIBTDPROCHOOK64_H__