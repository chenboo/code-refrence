#include "stdafx.h"
#include "BufferPool.h"

CBufferItem::CBufferItem(DWORD cbAllocate) 
{
	m_lpvData = malloc(cbAllocate);
	m_cbData = 0;
	m_cbAllocated = cbAllocate;
	memset(&m_guidTransaction, 0, sizeof(m_guidTransaction));
}

CBufferItem::~CBufferItem() 
{
	free(m_lpvData);
}

LPVOID CBufferItem::GetData()
{
	return m_lpvData;
}

DWORD CBufferItem::GetLength()
{
	return m_cbData;
}

void CBufferItem::SetLength(DWORD cbData)
{
	m_cbData = cbData;
}

DWORD CBufferItem::GetSize()
{
	return m_cbAllocated;
}

GUID *CBufferItem::GetTransactionID()
{
	return &m_guidTransaction;
}

void CBufferItem::SetTransactionID(GUID *pTransID)
{
	memcpy(&m_guidTransaction, pTransID, sizeof(GUID));
}

CIpAddress CBufferItem::GetDstIpAddress()
{
	return m_ipAddrDst;
}

void CBufferItem::SetDstIpAddress(CIpAddress ipAddrDst)
{
	m_ipAddrDst = ipAddrDst;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CBufferItemManager::CBufferItemManager(DWORD cbBufferItem)
{
	m_cbBufferItem = cbBufferItem;

	InitializeCriticalSection(&m_csReady);
	InitializeCriticalSection(&m_csFree);

	m_nRef = 1;
}

CBufferItemManager::~CBufferItemManager()
{
	DeleteCriticalSection(&m_csReady);
	DeleteCriticalSection(&m_csFree);

	for (CBufferList::iterator i = m_bufferReady.begin(); 
		 i != m_bufferReady.end(); i++) {
		CBufferItem *pItem = *i;
		delete pItem;
	}
	m_bufferReady.clear();

	for (CBufferList::iterator i = m_bufferFree.begin(); 
		 i != m_bufferFree.end(); i++) {
		CBufferItem *pItem = *i;
		delete pItem;
	}
	m_bufferFree.clear();
}

LONG CBufferItemManager::AddRef()
{
	return InterlockedIncrement(&m_nRef);
}

LONG CBufferItemManager::Release()
{
	LONG nRet = InterlockedDecrement(&m_nRef);
	if (nRet <= 0)
		delete this;

	return nRet;
}

CBufferItem* CBufferItemManager::GetFreeBufferItem()
{
	CAutoSectionLock lock(&m_csFree);

	if (m_bufferFree.empty()) {
		CBufferItem *pItem = new CBufferItem(m_cbBufferItem);
		return pItem;
	}

	CBufferItem *pItem = *m_bufferFree.begin();
	m_bufferFree.pop_front();
	return pItem;
}

void CBufferItemManager::PutPendingBufferItem(CBufferItem* pBufferItem)
{
	CAutoSectionLock lock(&m_csReady);

	m_bufferReady.push_back(pBufferItem);
}

CBufferItem* CBufferItemManager::GetItem()
{
	CAutoSectionLock lock(&m_csReady);

	if (m_bufferReady.empty()) {
		return NULL;
	}

	CBufferItem *pItem = *m_bufferReady.begin();
	m_bufferReady.pop_front();
	return pItem;
}

void CBufferItemManager::RecycleBufferItem(CBufferItem* pBufferItem)
{
	CAutoSectionLock lock(&m_csFree);
	m_bufferFree.push_back(pBufferItem);
}