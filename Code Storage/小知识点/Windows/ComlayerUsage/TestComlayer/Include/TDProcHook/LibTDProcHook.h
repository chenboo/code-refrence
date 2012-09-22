#ifndef __LIB_TDPROC_HOOK_H__
#define __LIB_TDPROC_HOOK_H__

#ifdef LIBTDPROCHOOK_EXPORTS
#define LIBPROCHOOK_API __declspec(dllexport)
#else
#define LIBPROCHOOK_API __declspec(dllimport)
#endif

#include <winioctl.h>
#include "TDProcHookIOC.h"

extern "C" 
{
	LIBPROCHOOK_API BOOL TDProcHookInit();
	LIBPROCHOOK_API void TDProcHookDone();
	LIBPROCHOOK_API BOOL TDProcHookEnableTerminate(BOOL bEnable);
	LIBPROCHOOK_API BOOL TDProcHookSetDenyReportExecPath(LPCSTR lpszExecPath);
	LIBPROCHOOK_API BOOL TDProcHookSetExecuteRule(PTDPROCHOOK_EXECUTE_RULE pRule);
	LIBPROCHOOK_API BOOL TDProcHookGetExeCheckSum(LPCTSTR lpszExecPath,	DWORD *pdwCheckSum);
	LIBPROCHOOK_API BOOL TDProcHookSetUsbNoExcuteRule(LPCSTR lpszDriveList);
	LIBPROCHOOK_API BOOL TDProcHookSetDeviceName(PTDPROCHOOK_DEVICE_DRIVE pDeviceDrive);
}

#endif //__LIB_TDPROC_HOOK_H__