#include "stdafx.h"
#include <objbase.h>
#include "Transactions.h"
#include "communicationLayer.h"

#define MIN_TIMEOUT_VALUE	(20)

////////////////////////////////////////////////////////////////////////////
///
// CTransactionHandler
CTransactionHandler::CTransactionHandler(ICommunicationLayer* pComLayer, GUID *pTransID)
{
	m_pComLayer = pComLayer;
	m_pTransID = (GUID *)malloc(sizeof(GUID));

	if (NULL == pTransID)
		CoCreateGuid(m_pTransID);
	else
		memcpy(m_pTransID, pTransID, sizeof(GUID));

	m_nRef = 1;

	ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
	pTransactionMgr->AddTransaction(m_pTransID, this);
	pTransactionMgr->Release();
}

CTransactionHandler::~CTransactionHandler()
{
	if (NULL != m_pTransID) {
		free(m_pTransID);
		m_pTransID = NULL;
	}
}

LONG CTransactionHandler::AddRef() 
{
	return InterlockedIncrement(&m_nRef);
}

LONG CTransactionHandler::Release()
{
	LONG nRet = InterlockedDecrement(&m_nRef);
	if (nRet <= 0)
		delete this;

	return nRet;
}

GUID *CTransactionHandler::GetTransactionID()
{
	return m_pTransID;
}
///////////////////////////////////////////////////////////////////////////////
// CTransactionManager
CTransactionManager::CTransactionManager() 
{
	InitializeCriticalSection(&m_csTransactions);

	m_nRef = 1;
}

CTransactionManager::~CTransactionManager() 
{
	DeleteAllTransactions();
	DeleteCriticalSection(&m_csTransactions);
}


LONG CTransactionManager::AddRef()
{
	return InterlockedIncrement(&m_nRef);
}

LONG CTransactionManager::Release()
{
	LONG nRet = InterlockedDecrement(&m_nRef);
	if (nRet <= 0)
		delete this;

	return nRet;
}

BOOL CTransactionManager::AddTransaction(GUID *pID, CTransactionHandler *pHandler) 
{
	CAutoSectionLock lock(&m_csTransactions);

	CTransactionHandler *pTempHandler = FindTransaction(pID);
	if (pTempHandler) {
		pTempHandler->Release();
		return FALSE;
	}

	pHandler->AddRef();
	m_mapTransactions[pID] = pHandler;
	return TRUE;
}

BOOL CTransactionManager::DeleteTransaction(GUID *pID) 
{
	CAutoSectionLock lock(&m_csTransactions);

	CTransactionMap::iterator i = m_mapTransactions.find(pID);
	if (m_mapTransactions.end() == i)
		return FALSE;

	m_delTransactions.push_back((*i).second);
	m_mapTransactions.erase(i);

	return TRUE;
}

CTransactionHandler *CTransactionManager::FindTransaction(GUID *pID)
{
	CAutoSectionLock lock(&m_csTransactions);

	CTransactionMap::iterator i = m_mapTransactions.find(pID);
	if (m_mapTransactions.end() == i)
		return NULL;

	(*i).second->AddRef();
	return (*i).second;
}

void CTransactionManager::DoPendingDeletion()
{
	CAutoSectionLock lock(&m_csTransactions);

	CTransactionList::iterator iterator;

	for (iterator = m_delTransactions.begin();
		 iterator != m_delTransactions.end(); 
		 iterator++) {
		(*iterator)->Release();
	}
	m_delTransactions.clear();
}

void  CTransactionManager::DeleteAllTransactions()
{
	CAutoSectionLock lock(&m_csTransactions);

	{
		CTransactionMap::iterator iterator;

		iterator = m_mapTransactions.begin();
		while (iterator != m_mapTransactions.end()) {
			(*iterator).second->Release();
			m_mapTransactions.erase(iterator);
			iterator = m_mapTransactions.begin();
		}
	}

	{
		CTransactionList::iterator iterator;

		for (iterator = m_delTransactions.begin();
			 iterator != m_delTransactions.end(); 
			 iterator++) {
			(*iterator)->Release();
		}

		m_delTransactions.clear();
	}
}
