#include "stdafx.h"
#include "TaskList.h"

#include <psapi.h>
#include <tlhelp32.h>

#define UNKNOWN_TASK        _T("UNKNOWN")
#define SYSTEM_IDLE_PROCESS_NAME _T("Sytem Idle Process")

typedef struct tagWNDENUMDATA
{
	DWORD dwProcessId;
	HWND  hWnd;
} WNDENUMDATA, *PWNDENUMDATA;

typedef struct tagAPPENUMDATA
{
	PAPPLICATIONITEM pApplicationItems;
	DWORD dwCount;
	DWORD dwMaxCount;
} APPENUMDATA, *PAPPENUMDATA;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
BOOL CALLBACK EnumWindowsProc2(HWND hWnd, LPARAM lParam);

DWORD (*g_lpfnGetTaskList)(PTASKITEM, DWORD);
BOOL  (*g_lpfnEnableDebugPriv)();

DWORD GetTaskList95(PTASKITEM pTaskItems, DWORD dwNumTasks);
BOOL  EnableDebugPriv95();

DWORD GetTaskListNT(PTASKITEM pTaskItems, DWORD dwNumTasks);
BOOL  EnableDebugPrivNT();

// Win9X ToolHelp32 Lib
typedef BOOL (WINAPI *PROCESSWALK)(HANDLE, LPPROCESSENTRY32);
typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD, DWORD);

// WinNT PSAPI Lib
typedef BOOL (WINAPI *ENUMPROCESSES)(DWORD *, DWORD, DWORD *);
typedef BOOL (WINAPI *ENUMPROCESSMODULES)(HANDLE, HMODULE *, DWORD, LPDWORD);
typedef DWORD (WINAPI *GETMODULEFILENAMEEX)(HANDLE, HMODULE, LPTSTR, DWORD);

BOOL  InitTaskList()
{
	OSVERSIONINFO verInfo;

    verInfo.dwOSVersionInfoSize = sizeof (verInfo);
    GetVersionEx(&verInfo);

    switch (verInfo.dwPlatformId)
    {
    case VER_PLATFORM_WIN32_NT:
       g_lpfnGetTaskList     = GetTaskListNT;
       g_lpfnEnableDebugPriv = EnableDebugPrivNT;
       break;

    case VER_PLATFORM_WIN32_WINDOWS:
       g_lpfnGetTaskList     = GetTaskList95;
       g_lpfnEnableDebugPriv = EnableDebugPriv95;
       break;

    default:
		return FALSE;
    }

	return g_lpfnEnableDebugPriv();
}

DWORD GetTaskList(PTASKITEM pTaskItems, DWORD dwNumTasks)
{
	return g_lpfnGetTaskList(pTaskItems, dwNumTasks);
}

DWORD GetApplicationList(PAPPLICATIONITEM pApplicationItems, DWORD dwNumApplications)
{
	APPENUMDATA appEnumData;
	appEnumData.dwCount = 0;
	appEnumData.dwMaxCount = dwNumApplications;
	appEnumData.pApplicationItems = pApplicationItems;

    EnumWindows(EnumWindowsProc, (LPARAM)&appEnumData);

	return appEnumData.dwCount;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	PAPPENUMDATA pAppEnumData = (PAPPENUMDATA)lParam;

	DWORD dwProcessId = NULL;
	if (!GetWindowThreadProcessId(hWnd, &dwProcessId))
        return TRUE;

	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	if ((dwStyle & WS_CHILD) || !(dwStyle & WS_VISIBLE))
		return TRUE;

	HWND hWndParent = GetParent(hWnd);
	if (hWndParent)
		return TRUE;

	TCHAR szBufferTitle[TITLE_SIZE];
	int nLength = GetWindowText(hWnd, szBufferTitle, _countof(szBufferTitle));
	if (nLength <= 0)
		return TRUE;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
	if (!hProcess)
		return TRUE;

	TCHAR szBufferName[MAX_PATH] = {0};
	nLength = GetModuleFileNameEx(hProcess, NULL, szBufferName, MAX_PATH);
	if (nLength <= 0)
		return TRUE;

	PAPPLICATIONITEM pAppItem = 
		&(pAppEnumData->pApplicationItems[pAppEnumData->dwCount]);
	pAppItem->hWnd = hWnd;
	pAppItem->dwProcessId = dwProcessId;
	_tcscpy(pAppItem->szWindowTitle, szBufferTitle);
	_tcscpy(pAppItem->szProcessName, szBufferName);

	pAppEnumData->dwCount++;
	return (pAppEnumData->dwCount < pAppEnumData->dwMaxCount);
}

HWND  GetProcessMainWnd(DWORD dwProcessId)
{
	WNDENUMDATA wndEnumData;
	wndEnumData.dwProcessId = dwProcessId;
	wndEnumData.hWnd = NULL;
    EnumWindows(EnumWindowsProc2, (LPARAM)&wndEnumData);

	return wndEnumData.hWnd;
}

BOOL CALLBACK EnumWindowsProc2(HWND hWnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
	PWNDENUMDATA pWndEnumData = (PWNDENUMDATA)lParam;

    if (!GetWindowThreadProcessId(hWnd, &dwProcessId))
        return TRUE;

	if (pWndEnumData->dwProcessId != dwProcessId)
		return TRUE;

	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	if ((dwStyle & WS_CHILD) || !(dwStyle & WS_VISIBLE))
		return TRUE;

	TCHAR szBuffer[TITLE_SIZE];
	int nLength = GetWindowText(hWnd, szBuffer, _countof(szBuffer));
	if (nLength <= 0)
		return TRUE;

	pWndEnumData->hWnd = hWnd;
    return FALSE;
}

BOOL  KillProcess(DWORD dwProcessId, BOOL bForce)
{
    HANDLE hProcess;
	HWND hWnd = GetProcessMainWnd(dwProcessId);

    if (bForce || !hWnd) {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
        if (hProcess) {
            BOOL bSuccess = TerminateProcess(hProcess, -1);
            CloseHandle(hProcess);
            return bSuccess;
        }
    }

	if (IsWindow(hWnd))
		PostMessage(hWnd, WM_CLOSE, 0, 0);

    return TRUE;
}

DWORD GetTaskList95(PTASKITEM pTaskItems, DWORD dwNumTasks)
{
	CREATESNAPSHOT pCreateToolhelp32Snapshot = NULL;
	PROCESSWALK    pProcess32First           = NULL;
	PROCESSWALK    pProcess32Next            = NULL;

	HINSTANCE      hKernel        = NULL;
	HANDLE		   hProcessSnap   = NULL;
	PROCESSENTRY32 pe32           = { 0 };
	DWORD          dwTaskCount    = 0;


	if (dwNumTasks == 0)
		return 0;

    hKernel = (HINSTANCE)GetModuleHandle(_T("KERNEL32.DLL"));

    if (hKernel) {
        pCreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress(
			hKernel, "CreateToolhelp32Snapshot");
        pProcess32First = (PROCESSWALK)GetProcAddress(
			hKernel, "Process32First");
        pProcess32Next  = (PROCESSWALK)GetProcAddress(
			hKernel, "Process32Next");
    }
    
    if (!(pProcess32First && pProcess32Next && pCreateToolhelp32Snapshot))
		return 0;
  
    hProcessSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == (HANDLE)-1)
        return 0;

    dwTaskCount = 0;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (pProcess32First(hProcessSnap, &pe32)) {
        do {
            pTaskItems->dwProcessId = pe32.th32ProcessID;
            _tcscpy(pTaskItems->szProcessName, pe32.szExeFile);

            dwTaskCount++;
            pTaskItems++;
        } while ((dwTaskCount < dwNumTasks) && pProcess32Next(hProcessSnap, &pe32));
    } else
        dwTaskCount = 0;

    CloseHandle (hProcessSnap);
    return dwTaskCount;
}

BOOL  EnableDebugPriv95()
{
	return TRUE;
}

DWORD GetTaskListNT(PTASKITEM pTaskItems, DWORD dwNumTasks)
{
	PROCESSENTRY32 procEntry;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		return 0 ;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnap, &procEntry))
	{
		return 0 ;
	}

	DWORD dwTaskCount = 0;

	do
	{
		if (GetCurrentProcessId() == procEntry.th32ProcessID) 
			continue;

		pTaskItems->dwProcessId = procEntry.th32ProcessID;

		if (procEntry.th32ProcessID == 0)
			_tcscpy(pTaskItems->szProcessName, SYSTEM_IDLE_PROCESS_NAME);
		else
			_tcscpy(pTaskItems->szProcessName, procEntry.szExeFile);

        pTaskItems++;
		dwTaskCount++;

	} while (Process32Next(hSnap, &procEntry));	

	return dwTaskCount;

	/*
	HINSTANCE hLib = LoadLibrary(_T("PSAPI.DLL"));
	if (!hLib) 
		return 0;

	HINSTANCE hVdmLib = LoadLibrary(_T("VDMDBG.DLL"));
	if (!hVdmLib) {
		FreeLibrary(hLib);
		return 0;
	}

	ENUMPROCESSES lpfnEnumProcesses = (ENUMPROCESSES)GetProcAddress(hLib, "EnumProcesses");
	ENUMPROCESSMODULES lpfnEnumProcessModules = (ENUMPROCESSMODULES)GetProcAddress(hLib, "EnumProcessModules");
	GETMODULEFILENAMEEX lpfnGetModuleFileNameEx = (GETMODULEFILENAMEEX)GetProcAddress(hLib, "GetModuleFileNameExW");
	if (!lpfnEnumProcesses || !lpfnEnumProcessModules || !lpfnGetModuleFileNameEx) {
		FreeLibrary(hLib);
		FreeLibrary(hVdmLib);
		return 0;
	}

	DWORD adwProcesses[256];
	DWORD dwSize;

	if (!lpfnEnumProcesses(adwProcesses, sizeof(adwProcesses), &dwSize)) {
		FreeLibrary(hLib);
		FreeLibrary(hVdmLib);
		return 0;
	}

	dwSize = dwSize / sizeof(DWORD);
	if (dwSize > dwNumTasks)
		dwSize = dwNumTasks;

	DWORD dwTaskCount = 0;
	for (DWORD i = 0; i < dwSize; i++) {
		if (GetCurrentProcessId() == adwProcesses[i]) {
			continue;
		}

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
			FALSE, adwProcesses[i]);
		if (!hProcess)
			continue;

		HMODULE hModule;
		TCHAR szFileName[MAX_PATH];
		DWORD dwSize2;
		if (lpfnEnumProcessModules(hProcess, &hModule, sizeof(hModule), &dwSize2)) {
			if (!lpfnGetModuleFileNameEx(hProcess, hModule, szFileName, _countof(szFileName)))
				_tcscpy(szFileName, UNKNOWN_TASK);
		}
		else
			_tcscpy(szFileName, UNKNOWN_TASK);

		pTaskItems->dwProcessId = adwProcesses[i];
		_tcscpy(pTaskItems->szProcessName, szFileName);

        pTaskItems++;
		dwTaskCount++;
		CloseHandle(hProcess);
	}

	return dwTaskCount;
	*/
}

BOOL  EnableDebugPrivNT()
{
    HANDLE			 hToken;
    LUID			 luidDebugValue;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return FALSE;

    if (!LookupPrivilegeValue((LPTSTR)NULL, SE_DEBUG_NAME, &luidDebugValue))
        return FALSE;

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luidDebugValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), 
		(PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL);

    if (GetLastError() != ERROR_SUCCESS)
        return FALSE;

    return TRUE;
}
