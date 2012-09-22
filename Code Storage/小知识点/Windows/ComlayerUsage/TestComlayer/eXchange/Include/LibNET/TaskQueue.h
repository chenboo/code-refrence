// TaskQueue.h: interface for the CTaskQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TaskQueue_H__00DCC17C_7E74_434A_B6AF_3453D677F432__INCLUDED_)
#define AFX_TaskQueue_H__00DCC17C_7E74_434A_B6AF_3453D677F432__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)

#include <list>
#include <map>

#include "Utils.h"
#include "TimeoutHeap.h"
#include "Overlapped.h"

#define TIMEOUT_INFINITE	(0xFFFFFFFF)

typedef std::map<LONG, CTask *> CTaskMap;

class LIBNET_API CTaskQueue  
{
	friend class CWorkThread;

public:
	CTaskQueue();
	virtual ~CTaskQueue();

public:
	BOOL Create(int nWorkThreadCount);
	void Destroy();

public:
	static int GetProcessorCount();

	int  GetWorkThreadCount() {
		return m_nWorkThreadCount;
	}

public:
	BOOL AttachIOHandle(LONG lTaskId, HANDLE hHandle);
	BOOL Signal(LONG lTaskId, CTaskOverlapped *pTaskOverlapped);
	BOOL Signal(LONG lTaskId, DWORD dwSignalID, 
				DWORD dwParam1, DWORD dwParam2);
	BOOL ScheduleTimeout(LONG lTaskId, DWORD dwTimeout, 
						 DWORD dwParam1, DWORD dwParam2);

	enum {
		CHECK_TASKID = TIMEOUT_HEAP_PARAM1,
		CHECK_PARAM1 = TIMEOUT_HEAP_PARAM2,
		CHECK_PARAM2 = TIMEOUT_HEAP_PARAM3
	};
	BOOL DeleteTimeout(LONG lTaskId, DWORD dwParam1, DWORD dwParam2, DWORD dwMask);

public:
	BOOL AddTask(CTask *pTask);
	BOOL RemoveTask(CTask *pTask);

public:
	LONG GetTaskCount();
	LONG GetTaskIds(LONG *plTaskIds, LONG lCount);

	CTask *GetTask(LONG dwTaskId);

	CTimeoutHeap *GetTimeoutHeap() {
		EnterCriticalSection(&m_csTimeoutHeap);
		return m_pTimeoutHeap;
	}

	void ReleaseTimeoutHeap() {
		LeaveCriticalSection(&m_csTimeoutHeap);
	}

protected:
	BOOL GetCompletionTaskId(
			LONG *				plTaskId, 
			DWORD *				pdwErrorCode, 
			DWORD *				pdwTransferred, 
			CTaskOverlapped **	ppTaskOverlapped, 
			DWORD				dwTimeout);

	CTaskMap	 *			m_pTaskMap;
	CRITICAL_SECTION		m_csMap;

	HANDLE					m_hCompletionPort;
	int						m_nWorkThreadCount;
	CWorkThread **			m_ppWorkThreads;

	CRITICAL_SECTION		m_csTimeoutHeap;
	CTimeoutHeap *			m_pTimeoutHeap;

	CTaskOverlappedPoolEx<CSignalOverlapped> * m_pSignalPool;
};

#endif // !defined(AFX_TaskQueue_H__00DCC17C_7E74_434A_B6AF_3453D677F432__INCLUDED_)
