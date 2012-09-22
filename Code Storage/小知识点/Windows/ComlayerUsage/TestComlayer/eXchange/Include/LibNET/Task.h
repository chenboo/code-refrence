// Task.h: interface for the CTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASK_H__77B9BC49_612C_4165_8575_75B8946C9492__INCLUDED_)
#define AFX_TASK_H__77B9BC49_612C_4165_8575_75B8946C9492__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TaskQueue.h"
#include "WorkThread.h"

class LIBNET_API CTask  
{
public:
	CTask(CTaskQueue *pTaskQueue) {
		m_pTaskQueue = pTaskQueue;
		m_lRefCount = 1;
		m_lTaskId = InterlockedIncrement(&m_lNextTaskId);
		m_bShutdown = FALSE;
		InitializeCriticalSection(&m_csTask);
		InterlockedIncrement(&m_lTaskCount);
		m_pTaskQueue->AddTask(this);
	}

	virtual ~CTask() {
		DeleteCriticalSection(&m_csTask);
		InterlockedDecrement(&m_lTaskCount);
	}

	virtual LONG AddRef() {
		return InterlockedIncrement(&m_lRefCount);
	}

	virtual LONG Release() {
		LONG lRefCount = InterlockedDecrement(&m_lRefCount);
		if (lRefCount <= 0) {
			delete this;
		}

		return lRefCount;
	}

public:
	BOOL AttachIOHandle(HANDLE hHandle) {
		return m_pTaskQueue->AttachIOHandle(m_lTaskId, hHandle);
	}

	BOOL Signal(CIOOverlapped *pTaskOverlapped) {
		return m_pTaskQueue->Signal(m_lTaskId, pTaskOverlapped);
	}

	BOOL Signal(DWORD dwSignalID, DWORD dwParam1, DWORD dwParam2) {
		return m_pTaskQueue->Signal(m_lTaskId, dwSignalID, dwParam1, dwParam2);
	}

	BOOL ScheduleTimeout(DWORD dwTimeout, DWORD dwParam1, DWORD dwParam2) {
		return m_pTaskQueue->ScheduleTimeout(
			m_lTaskId, dwTimeout, dwParam1, dwParam2);
	}

	BOOL DeleteTimeout(DWORD dwParam1, DWORD dwParam2, DWORD dwMask) {
		return m_pTaskQueue->DeleteTimeout(
			m_lTaskId, dwParam1, dwParam2, dwMask);
	}

public:
	LONG GetTaskId() {
		return m_lTaskId;
	}

	CRITICAL_SECTION * GetTaskLock() {
		return &m_csTask;
	}

	CTaskQueue * GetTaskQueue() {
		return m_pTaskQueue;
	}

	void Shutdown() {
		{
			CAutoSectionLock lock(&m_csTask);
			if (!m_bShutdown) {
				OnShutdown();
				m_bShutdown = TRUE;
			}
		}

		m_pTaskQueue->RemoveTask(this);
	}

	BOOL IsShutdown() {
		CAutoSectionLock lock(&m_csTask);
		return m_bShutdown;
	}

public:
	virtual void OnIOComplete(CIOOverlapped *pIOOverlapped) {}
	virtual void OnSignal(DWORD dwSignalID, DWORD dwParam1, DWORD dwParam2) {}
	virtual void OnTimeout(DWORD dwParam1, DWORD dwParam2) {}
	virtual void OnShutdown() {}

public:
	static LONG GetTotalCount() {
		return m_lTaskCount;
	}

protected:
	static LONG m_lTaskCount;
	static LONG m_lNextTaskId;

	LONG			 m_lTaskId;
	CTaskQueue		*m_pTaskQueue;

	LONG			 m_lRefCount;
	CRITICAL_SECTION m_csTask;

	BOOL			 m_bShutdown;
};

#endif // !defined(AFX_TASK_H__77B9BC49_612C_4165_8575_75B8946C9492__INCLUDED_)
