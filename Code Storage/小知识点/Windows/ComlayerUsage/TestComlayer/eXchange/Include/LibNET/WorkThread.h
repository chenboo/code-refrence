// WorkThread.h: interface for the CWorkThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKTHREAD_H__0151D861_63D6_4B19_B2F2_6860EE68C843__INCLUDED_)
#define AFX_WORKTHREAD_H__0151D861_63D6_4B19_B2F2_6860EE68C843__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TimeoutHeap.h"

class CTask;
class CTaskQueue;
class CWorkThread  
{
public:
	CWorkThread(CTaskQueue *pTaskQueue);
	virtual ~CWorkThread();

public:
	BOOL Create();
	void Destroy();

public:
	DWORD GetThreadId() {
		return m_dwThreadId;
	}

	HANDLE GetThreadHandle() {
		return m_hHandle;
	}

protected:
	virtual DWORD ThreadProc();
	static DWORD WINAPI ThreadProc(LPVOID lpvParam);

protected:
	CTaskQueue		*m_pTaskQueue;
	HANDLE			 m_hHandle;
	DWORD			 m_dwThreadId;
	BOOL			 m_bRunning;
};

#endif // !defined(AFX_WORKTHREAD_H__0151D861_63D6_4B19_B2F2_6860EE68C843__INCLUDED_)
