#include "stdafx.h"
#include "LangLabEvent.h"

LPVOID CEventItem::GetData()
{
	return m_lpvData;
}

DWORD CEventItem::GetLength()
{
	return m_cbData;
}

DWORD CEventItem::GetRecycleLength()
{
	return m_cbRecycle;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

CEventManager::CEventManager(DWORD cbPoolSize)
{
	m_pCallback = NULL;
	m_lpvUserValue = NULL;

	InitializeCriticalSection(&m_csCreate);
	InitializeCriticalSection(&m_csEvent);
	InitializeCriticalSection(&m_csPending);

	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	m_nRef = 1;

	m_cbPoolSize = cbPoolSize;
	m_pbPool = (LPBYTE)malloc(m_cbPoolSize);

	m_dwGetPos = 0;
	m_dwPutPos = 0;

	m_dwFreeSize = m_cbPoolSize;
}

CEventManager::~CEventManager(void)
{
	DeleteCriticalSection(&m_csCreate);
	DeleteCriticalSection(&m_csEvent);
	DeleteCriticalSection(&m_csPending);
	
	CloseHandle(m_hEvent);

	for (CEventList::iterator i = m_event.begin(); 
		 i != m_event.end(); i++) {
		CEventItem *pItem = *i;
		delete pItem;
	}
	m_event.clear();

	if (m_pbPool) {
		free(m_pbPool);
		m_pbPool = NULL;
	}
}

void CEventManager::SetEventArrivedCallback(IEventManagerNotify* pCallback, LPVOID lpvUserValue)
{
	m_pCallback = pCallback;
	m_lpvUserValue = lpvUserValue;
}

LONG CEventManager::AddRef()
{
	return InterlockedIncrement(&m_nRef);
}

LONG CEventManager::Release()
{
	LONG nRet = InterlockedDecrement(&m_nRef);
	if (nRet <= 0)
		delete this;

	return nRet;
}

IEventItem* CEventManager::GetFreeEvent(DWORD cbSize)
{
	EnterCriticalSection(&m_csPending);
	return (IEventItem *)GetFreeItem(cbSize);
}

void CEventManager::PutPendingEvent(IEventItem* pEvent)
{
	PutItem((CEventItem *)pEvent);
	LeaveCriticalSection(&m_csPending);
}

void CEventManager::PutPendingEvent(LPCVOID lpcEvData, DWORD cbEventSize)
{
	CEventItem* pItem = GetFreeItem(cbEventSize);
	memcpy(pItem->GetData(), lpcEvData, cbEventSize);

	PutItem(pItem);
}

IEventItem* CEventManager::GetPendingEvent()
{
	CAutoSectionLock lockList(&m_csEvent);

	if (m_event.empty()) {
		return NULL;
	}

	CEventItem *pItem = *m_event.begin();
	m_event.pop_front();
	return pItem;
}
	
void CEventManager::RecycleEvent(IEventItem* pEvent)
{
	CAutoSectionLock lock(&m_csCreate);	

	CEventItem* pItem = (CEventItem *)pEvent;

	m_dwPutPos += pItem->GetRecycleLength();
	m_dwPutPos %= m_cbPoolSize;
	m_dwFreeSize += pItem->GetRecycleLength();
	delete pEvent;

	SetEvent(m_hEvent);
}

CEventItem* CEventManager::GetFreeItem(DWORD cbEventSize)
{
	CAutoSectionLock lock(&m_csCreate);

	// If less needed, wait....
	DWORD cbRecycle = cbEventSize;

	do {
		while (m_dwFreeSize < cbEventSize) {
			ResetEvent(m_hEvent);

//			lock.Unlock();
			LeaveCriticalSection(&m_csCreate);

			WaitForSingleObject(m_hEvent, INFINITE);
			ResetEvent(m_hEvent);

//			lock.Lock();
			EnterCriticalSection(&m_csCreate);
		}

		if ((m_dwGetPos + cbEventSize) <= m_cbPoolSize)
			break;

		// Auto return to buffer head
		DWORD cbSkipped = m_cbPoolSize - m_dwGetPos;
		m_dwFreeSize -= cbSkipped;
		cbRecycle += cbSkipped;
		m_dwGetPos = 0;
	} while (TRUE);

	CEventItem* pItem = new CEventItem(cbRecycle, m_pbPool + m_dwGetPos, cbEventSize);
	m_dwGetPos += cbEventSize;
	m_dwFreeSize -= cbEventSize;
/*
	char szInfo[100];
	sprintf(szInfo, "\n CEventManager: Free size: %d", m_dwFreeSize);
	OutputDebugString(szInfo);
*/
	return pItem;
}

void CEventManager::PutItem(CEventItem* pItem)
{
	CAutoSectionLock lock(&m_csEvent);

	m_event.push_back(pItem);
	if ((m_event.size() == 1) && m_pCallback)
		m_pCallback->OnEventArrived(m_lpvUserValue);
}