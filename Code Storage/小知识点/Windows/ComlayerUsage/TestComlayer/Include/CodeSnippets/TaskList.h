#ifndef __TASKLIST_H__
#define __TASKLIST_H__

#ifdef __cplusplus
extern "C" {
#endif 

#define TITLE_SIZE          256
#define PROCESS_SIZE        MAX_PATH
#define MAX_APPICON_SIZE	(16 * 16 * 4)

typedef struct tagTASKITEM {
    DWORD   dwProcessId;
    TCHAR    szProcessName[PROCESS_SIZE];
} TASKITEM, *PTASKITEM;

typedef struct tagAPPLICATIONITEM {
	DWORD	dwProcessId;
	HWND	hWnd;
	WORD	wbmBitsPixel;
	WORD	cbSize;	
	BYTE	pbBitmapBits[MAX_APPICON_SIZE];
	TCHAR	szWindowTitle[TITLE_SIZE];
	TCHAR   szProcessName[PROCESS_SIZE];
} APPLICATIONITEM, *PAPPLICATIONITEM;

BOOL  InitTaskList();
DWORD GetTaskList(PTASKITEM pTaskItems, DWORD dwNumTasks);
DWORD GetApplicationList(PAPPLICATIONITEM pApplicationItems, DWORD dwNumApplications);
HWND  GetProcessMainWnd(DWORD dwProcessId);
BOOL  KillProcess(DWORD dwProcessId, BOOL bForce);

#ifdef __cplusplus
}
#endif 

#endif //__TASKLIST_H__