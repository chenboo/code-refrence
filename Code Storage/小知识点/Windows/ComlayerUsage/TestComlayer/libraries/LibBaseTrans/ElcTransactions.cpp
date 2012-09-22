#include "stdafx.h"
#include "ElcTransactions.h"

#ifdef USE_PROXIMITY
#include <cmpc/proximity/proximity.h>
#endif

CSubmitFileTransaction::CSubmitFileTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);

	// 0 means no limite
	m_llMaxSize = 0;
	m_dwMaxCount = 0;

	m_bAutoAccept = FALSE;

	m_pComLayer->RegisterTimeout(m_pTransID, 10000, 0);
}

CSubmitFileTransaction::~CSubmitFileTransaction()
{
	DeleteCriticalSection(&m_cs);

	{
		CSubmitMap::iterator iterator = m_pendingMap.begin(); 
		while (iterator != m_pendingMap.end()) {
			PSUBMIT_CONTEXT pContext = (*iterator).second;
			free(pContext);
			m_pendingMap.erase(iterator);
			iterator = m_pendingMap.begin();
		}
	}

	{
		CSubmitMap::iterator iterator = m_checkedMap.begin(); 
		while (iterator != m_checkedMap.end()) {
			PSUBMIT_CONTEXT pContext = (*iterator).second;
			free(pContext);
			m_checkedMap.erase(iterator);
			iterator = m_checkedMap.begin();
		}
	}

	{
		CSubmitMap::iterator iterator = m_finishedMap.begin(); 
		while (iterator != m_finishedMap.end()) {
			PSUBMIT_CONTEXT pContext = (*iterator).second;
			free(pContext);
			m_finishedMap.erase(iterator);
			iterator = m_finishedMap.begin();
		}
	}
}

void CSubmitFileTransaction::EnableSubmit(PMEMBERINFO pMemberInfo, BOOL bEnable)
{
	CAutoSectionLock lock(&m_cs);

	CDisableList::iterator it = m_disableList.begin();
	while (it != m_disableList.end()) {
		if (pMemberInfo == *it) {
			m_disableList.erase(it);
			break;
		}

		it ++;
	}

	if (!bEnable) {
		m_disableList.push_back(pMemberInfo);
	}
}

void CSubmitFileTransaction::EnableSubmitRemove(PMEMBERINFO pMemberInfo)
{
	CAutoSectionLock lock(&m_cs);

	CDisableList::iterator it = m_disableList.begin();
	while (it != m_disableList.end()) {
		if (pMemberInfo == *it) {
			m_disableList.erase(it);
			break;
		}

		it ++;
	}
}

void CSubmitFileTransaction::EnableSubmitRemoveAll()
{
	CAutoSectionLock lock(&m_cs);

	CDisableList::iterator it = m_disableList.begin();
	while (it != m_disableList.end()) {
		m_disableList.erase(it);
		it = m_disableList.begin();
	}
}

void CSubmitFileTransaction::SetSubmitQuota(LONGLONG llMaxSize, DWORD dwMaxCount)
{
	CAutoSectionLock lock(&m_cs);

	m_llMaxSize = llMaxSize;
	m_dwMaxCount = dwMaxCount;
}

void CSubmitFileTransaction::SetAutoAccept(BOOL bAutoAccept)
{
	CAutoSectionLock lock(&m_cs);

	m_bAutoAccept = bAutoAccept;
}

void CSubmitFileTransaction::AcceptSubmitting(GUID guidRequest)
{
	CAutoSectionLock lock(&m_cs);

	CSubmitMap::iterator iterator = m_pendingMap.find(guidRequest);
	if (iterator == m_pendingMap.end())
		return;

	PSUBMIT_CONTEXT pContext = (*iterator).second;

	pContext->bAccepted = TRUE;
	pContext->dwTickCount = GetTickCount();

	SendRequestRetPacket(pContext->dwStudentIp, TRUE, TRUE, TRUE);

	m_pendingMap.erase(iterator);

	m_checkedMap.insert(CSubmitMap::value_type(guidRequest, pContext));
}

void CSubmitFileTransaction::DenySubmitting(GUID guidRequest)
{
	CAutoSectionLock lock(&m_cs);

	CSubmitMap::iterator iterator = m_pendingMap.find(guidRequest);
	if (iterator == m_pendingMap.end())
		return;

	PSUBMIT_CONTEXT pContext = (*iterator).second;

	pContext->bAccepted = FALSE;
	pContext->dwTickCount = GetTickCount();

	SendRequestRetPacket(pContext->dwStudentIp, TRUE, TRUE, FALSE);

	m_pendingMap.erase(iterator);

	m_checkedMap.insert(CSubmitMap::value_type(guidRequest, pContext));
}

void CSubmitFileTransaction::ProcessRequest(PSUBMIT_REQUEST pRequest)
{
	CAutoSectionLock lock(&m_cs);

	// File submit disabled by teacher
	CDisableList::iterator it = m_disableList.begin();
	while (it != m_disableList.end()) {
		PMEMBERINFO pMemberInfo = *it;
		if ((pMemberInfo->bLoggedIn) && (pRequest->dwStudentIp == pMemberInfo->dwIpAddress)) {
		SendRequestRetPacket(pRequest->dwStudentIp, FALSE, FALSE, FALSE);
		return;
	}

		it ++;
	}


	// check quota
	if ((m_llMaxSize != 0) && (pRequest->llSize > m_llMaxSize)) {
		SendRequestRetPacket(pRequest->dwStudentIp, TRUE, FALSE, FALSE);
		return;
	}

	if ((m_dwMaxCount != 0) && (pRequest->dwCount > m_dwMaxCount)) {
		SendRequestRetPacket(pRequest->dwStudentIp, TRUE, FALSE, FALSE);
		return;
	}

	if (m_bAutoAccept) {
		SendRequestRetPacket(pRequest->dwStudentIp, TRUE, TRUE, TRUE);
		return;
	}

	// look in checked map, and reply student if found
	CSubmitMap::iterator iterator = m_checkedMap.find(pRequest->guidRequest);
	if (iterator != m_checkedMap.end()) {
		PSUBMIT_CONTEXT pContext = (*iterator).second;
		SendRequestRetPacket(pContext->dwStudentIp, TRUE, TRUE, pContext->bAccepted);
		return;
	}

	// Insert and report 
	CSubmitMap::iterator iterator1 = m_pendingMap.find(pRequest->guidRequest);
	if (iterator1 == m_pendingMap.end()) {
		// insert item 
		PSUBMIT_CONTEXT pContext = (PSUBMIT_CONTEXT)malloc(sizeof(SUBMIT_CONTEXT));
		pContext->dwStudentIp = pRequest->dwStudentIp;
		pContext->llFileSize = pRequest->llSize;
		pContext->dwFileCount = pRequest->dwCount;
		_tcscpy(pContext->pszStudentName, pRequest->szStudentName);
		// pContext->bAccepted;
		// pContext->dwTickCount;

		m_pendingMap.insert(CSubmitMap::value_type(pRequest->guidRequest, pContext));

		// report event
		IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

		DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_SUBMIT_REQUEST);
		IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

		PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
		PEVENT_SUBMIT_REQUEST pEvent = (PEVENT_SUBMIT_REQUEST)(pHeader + 1);

		pHeader->dwEvent = EV_SUBMIT_REQUEST;
		pHeader->dwLength = sizeof(EVENT_SUBMIT_REQUEST);

		memset(pEvent, 0, sizeof(EVENT_SUBMIT_REQUEST));
		memcpy(&pEvent->guidRequest, &pRequest->guidRequest, sizeof(GUID));
		pEvent->dwStudentIp = pRequest->dwStudentIp;
		pEvent->llSize = pRequest->llSize;
		pEvent->dwCount = pRequest->dwCount;
		_tcscpy(pEvent->szStudentName, pRequest->szStudentName);
		_tcscpy(pEvent->szFileNames, pRequest->szFileNames);

		pEventMgr->PutPendingEvent(pItem);
		pEventMgr->Release();
	}
}

void CSubmitFileTransaction::ProcessCancel(PSUBMIT_CANCEL pCancel)
{
	CAutoSectionLock lock(&m_cs);

	// report event
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_SUBMIT_CANCEL);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_SUBMIT_CANCEL pEvent = (PEVENT_SUBMIT_CANCEL)(pHeader + 1);

	pHeader->dwEvent = EV_SUBMIT_CANCEL;
	pHeader->dwLength = sizeof(EVENT_SUBMIT_CANCEL);

	memcpy(&pEvent->guidRequest, &pCancel->guidRequest, sizeof(GUID));
	pEvent->dwStudentIp = pCancel->dwStudentIp;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();

	// look in checked map, and reply student if found
	CSubmitMap::iterator iterator = m_checkedMap.find(pCancel->guidRequest);
	if (iterator != m_checkedMap.end()) {
		PSUBMIT_CONTEXT pContext = (*iterator).second;
		SendCancelRetPacket(pContext->dwStudentIp, pContext->bAccepted);
		return;
	}

	// if not in pending map, just return
	CSubmitMap::iterator iterator1 = m_pendingMap.find(pCancel->guidRequest);
	if (iterator1 == m_pendingMap.end()) {
		return;
	}

	PSUBMIT_CONTEXT pContext = (*iterator1).second;
	pContext->bAccepted = FALSE;
	pContext->dwTickCount = GetTickCount();

	m_pendingMap.erase(iterator1);
	m_checkedMap.insert(CSubmitMap::value_type(pCancel->guidRequest, pContext));

	// reply student
	SendCancelRetPacket(pCancel->dwStudentIp, FALSE);
}

void CSubmitFileTransaction::ProcessComplete(PSUBMIT_COMPLETED pComplete)
{
	CAutoSectionLock lock(&m_cs);

	// Alreay report finished
	CSubmitMap::iterator iterator = m_finishedMap.find(pComplete->guidRequest);
	if (iterator != m_finishedMap.end()) {
		SendCompleteRetPacket(pComplete->dwStudentIp);
		return;
	}

	// Prepare node for completed student
	PSUBMIT_CONTEXT pContext = (PSUBMIT_CONTEXT)malloc(sizeof(SUBMIT_CONTEXT));
	if (!pContext)
		return;

	memset(pContext, 0, sizeof(SUBMIT_CONTEXT));
	pContext->dwStudentIp = pComplete->dwStudentIp;
	pContext->dwTickCount = GetTickCount();
	m_finishedMap.insert(CSubmitMap::value_type(pComplete->guidRequest, pContext));

	// report event
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_SUBMIT_COMPLETE);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_SUBMIT_COMPLETE pEvent = (PEVENT_SUBMIT_COMPLETE)(pHeader + 1);

	pHeader->dwEvent = EV_SUBMIT_COMPLETE;
	pHeader->dwLength = sizeof(EVENT_SUBMIT_COMPLETE);

	pEvent->dwStudentIp = pComplete->dwStudentIp;
	pEvent->nCompletedFileCount = pComplete->nCompletedFileCount;
	_tcscpy(pEvent->szStudentName, pComplete->szStudentName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();

	// reply student
	SendCompleteRetPacket(pComplete->dwStudentIp);
}

void CSubmitFileTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == 0)
	{
		{
			CSubmitMap::iterator iterator = m_checkedMap.begin();
			while (iterator != m_checkedMap.end()) {
				PSUBMIT_CONTEXT pContext = (*iterator).second;
				if (GetTickCount() - pContext->dwTickCount > 60000) {
					m_checkedMap.erase(iterator);
					free(pContext);

					iterator = m_checkedMap.begin();
				} else
					iterator ++;
			}
		}

		{
			CSubmitMap::iterator iterator = m_finishedMap.begin();
			while (iterator != m_finishedMap.end()) {
				PSUBMIT_CONTEXT pContext = (*iterator).second;
				if (GetTickCount() - pContext->dwTickCount > 60000) {
					m_finishedMap.erase(iterator);
					free(pContext);

					iterator = m_finishedMap.begin();
				} else
					iterator ++;
			}
		}
	}
}

void CSubmitFileTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_SUBMIT_REQUEST:
		{
			PSUBMIT_REQUEST pRequest = (PSUBMIT_REQUEST)(pPacket + 1);
			ProcessRequest(pRequest);
		}
		break;
	case MAGIC_SUBMIT_CANCEL:
		{
			PSUBMIT_CANCEL pCancel = (PSUBMIT_CANCEL)(pPacket + 1);
			ProcessCancel(pCancel);
		}
		break;
	case MAGIC_SUBMIT_COMPLETED:
		{
			PSUBMIT_COMPLETED pComplete = (PSUBMIT_COMPLETED)(pPacket + 1);
			ProcessComplete(pComplete);
		}
	default:
		break;
	}
}

void CSubmitFileTransaction::SendRequestRetPacket(DWORD dwStudentIp, BOOL bEnable, BOOL bQuotaPass, BOOL bAccept)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwStudentIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PSUBMIT_REQUEST_RET pRequestRetPacket = (PSUBMIT_REQUEST_RET)(pPacket + 1);

	pPacket->dwMagic = MAGIC_SUBMIT_REQUEST_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(SUBMIT_REQUEST_RET);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pRequestRetPacket->bEnable = bEnable;
	pRequestRetPacket->bQuotaPass = bQuotaPass;
	pRequestRetPacket->bAccept = bAccept;
	pRequestRetPacket->llQuotaSize = m_llMaxSize;
	pRequestRetPacket->dwQuotaCount = m_dwMaxCount;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CSubmitFileTransaction::SendCancelRetPacket(DWORD dwStudentIp, BOOL bAccept)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwStudentIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PSUBMIT_CANCEL_RET pCancelRetPacket = (PSUBMIT_CANCEL_RET)(pPacket + 1);

	pPacket->dwMagic = MAGIC_SUBMIT_CANCEL_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(SUBMIT_CANCEL_RET);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pCancelRetPacket->bAccept = bAccept;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CSubmitFileTransaction::SendCompleteRetPacket(DWORD dwStudentIp)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwStudentIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PSUBMIT_COMPLETE_RET pCompleteRetPacket = (PSUBMIT_COMPLETE_RET)(pPacket + 1);

	pPacket->dwMagic = MAGIC_SUBMIT_COMPLETE_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(SUBMIT_COMPLETE_RET);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pCompleteRetPacket->dwStudentIp = dwStudentIp;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CSubmitFileClientTransaction::CSubmitFileClientTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);

	m_dwTeacherIp = 0;
	m_dwStudentIp = 0;

	m_llSize = 0;
	m_dwCount = 0;

	m_bRequest = FALSE;
	m_bCancel = FALSE;
	m_bWaitComplete = FALSE;

	m_nLastCompletedFileCount = 0;

	m_nCanelTimes = 0;
}

CSubmitFileClientTransaction::~CSubmitFileClientTransaction()
{
	DeleteCriticalSection(&m_cs);
}

BOOL CSubmitFileClientTransaction::SendSubmitRequest(GUID guidRequest, DWORD dwTeacherIp, DWORD dwStudentIp, TCHAR* pszStudentName, LONGLONG llSize, DWORD dwCount, TCHAR* pszFileNames)
{
	CAutoSectionLock lock(&m_cs);

	if (m_bRequest || m_bCancel)
		return FALSE;

	if (m_bWaitComplete) {
		m_pComLayer->UnregisterTimeout(m_pTransID, 2);
		m_bWaitComplete = FALSE;
	}

	memcpy(&m_guidRequest, &guidRequest, sizeof(GUID));

	m_dwTeacherIp = dwTeacherIp;
	m_dwStudentIp = dwStudentIp;
	_tcscpy(m_szStudentName, pszStudentName);

	memset(m_szFileNames, 0, MAX_PATH * sizeof(TCHAR));

	if (pszFileNames && (_tcslen(pszFileNames) < MAX_PATH))
		_tcscpy(m_szFileNames, pszFileNames);

	m_llSize = llSize;
	m_dwCount = dwCount;

	if (!SendReuestPacket())
		return FALSE;
	
	m_pComLayer->RegisterTimeout(m_pTransID, 1000, 0);

	m_bRequest = TRUE;

	return TRUE;
}

BOOL CSubmitFileClientTransaction::SendSubmitCancel(int nCompletedFileCount)
{
	CAutoSectionLock lock(&m_cs);

	// if requesting, cancel it directly
	if (m_bRequest) {
		m_pComLayer->UnregisterTimeout(m_pTransID, 0);
		m_bRequest = FALSE;
	}

	if (m_bWaitComplete) {
		m_pComLayer->UnregisterTimeout(m_pTransID, 2);
		m_bWaitComplete = FALSE;
	}

	if (!SendCancelPacket())
		return FALSE;

	m_pComLayer->RegisterTimeout(m_pTransID, 1000, 1);

	m_bCancel = TRUE;

	return TRUE;
}

BOOL CSubmitFileClientTransaction::SendSubmitCompleted(int nCompletedFileCount)
{
	CAutoSectionLock lock(&m_cs);

	m_nLastCompletedFileCount = nCompletedFileCount;
	if (!SendCompletedPacket(m_nLastCompletedFileCount))
		return FALSE;

	m_pComLayer->RegisterTimeout(m_pTransID, 1000, 2);

	m_bWaitComplete = TRUE;

	return TRUE;
}

void CSubmitFileClientTransaction::Destroy()
{
	ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
	pTransactionMgr->DeleteTransaction(m_pTransID);
	pTransactionMgr->Release();
}

void CSubmitFileClientTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	// request timeout
	if (dwTimeID == 0)
	{
		if (!m_bRequest)
			return;

		// try again
		SendReuestPacket();
		m_pComLayer->RegisterTimeout(m_pTransID, 1000, 0);
	}

	// cancel timeout
	if (dwTimeID == 1)
	{
		if (!m_bCancel)
			return;

		if (m_nCanelTimes > 5) {
			// report timeout event
			IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

			DWORD cbEvent = sizeof(EVENT_HEADER);

			IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
			if (NULL != pItem) {
				PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
				pHeader->dwEvent = EV_SUBMIT_CANCEL_TIMEOUT;
				pHeader->dwLength = 0;

				pEventMgr->PutPendingEvent(pItem);
				pEventMgr->Release();
			} else
			{
				pEventMgr->Release();
			}

			m_bCancel = FALSE;
			m_nCanelTimes = 0;
		} else
		{
			SendCancelPacket();
			m_pComLayer->RegisterTimeout(m_pTransID, 1000, 1);

			m_nCanelTimes ++;
		}
	}

	// Complete timeout
	if (dwTimeID == 2)
	{
		if (!m_bWaitComplete)
			return;

		// try again
		SendCompletedPacket(m_nLastCompletedFileCount);
		m_pComLayer->RegisterTimeout(m_pTransID, 1000, 2);
	}
}

void CSubmitFileClientTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_SUBMIT_REQUEST_RET:
		{
			PSUBMIT_REQUEST_RET pRequestRet = (PSUBMIT_REQUEST_RET)(pPacket + 1);
			ProcessRequestRet(pRequestRet);
		}
		break;
	case MAGIC_SUBMIT_CANCEL_RET:
		{
			PSUBMIT_CANCEL_RET pCancelRet = (PSUBMIT_CANCEL_RET)(pPacket + 1);
			ProcessCancelRet(pCancelRet);
		}
		break;
	case MAGIC_SUBMIT_COMPLETE_RET:
		{
			PSUBMIT_COMPLETE_RET pCompleteRet = (PSUBMIT_COMPLETE_RET)(pPacket + 1);
			ProcessCompleteRet(pCompleteRet);
		}
		break;
	default:
		break;
	}
}

void CSubmitFileClientTransaction::ProcessRequestRet(PSUBMIT_REQUEST_RET pRequestRet)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bRequest)
		return;

	// report request ret event
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_SUBMIT_REQUEST_RET);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem) {
		pEventMgr->Release();
		return;
	}

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_SUBMIT_REQUEST_RET pEvent = (PEVENT_SUBMIT_REQUEST_RET)(pHeader + 1);

	pHeader->dwEvent = EV_SUBMIT_REQUEST_RET;
	pHeader->dwLength = sizeof(EVENT_SUBMIT_REQUEST_RET);

	pEvent->bEnable = pRequestRet->bEnable;
	pEvent->bQuotaPass = pRequestRet->bQuotaPass;
	pEvent->bAccept = pRequestRet->bAccept;
	pEvent->llQuotaSize = pRequestRet->llQuotaSize;
	pEvent->dwQuotaCount = pRequestRet->dwQuotaCount;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();

	// cancel timeout
	m_pComLayer->UnregisterTimeout(m_pTransID, 0);

	m_bRequest = FALSE;
}

void CSubmitFileClientTransaction::ProcessCancelRet(PSUBMIT_CANCEL_RET pCancelRet)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bCancel)
		return;

	// report cancel ret event
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_SUBMIT_CANCEL_RET);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem) {
		pEventMgr->Release();
		return;
	}

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_SUBMIT_CANCEL_RET pEvent = (PEVENT_SUBMIT_CANCEL_RET)(pHeader + 1);

	pHeader->dwEvent = EV_SUBMIT_CANCEL_RET;
	pHeader->dwLength = sizeof(EVENT_SUBMIT_CANCEL_RET);

	pEvent->bAccept = pCancelRet->bAccept;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();

	// cancel timeout
	m_pComLayer->UnregisterTimeout(m_pTransID, 1);

	m_bCancel = FALSE;
	m_nCanelTimes = 0;
}

void CSubmitFileClientTransaction::ProcessCompleteRet(PSUBMIT_COMPLETE_RET pCompleteRet)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bWaitComplete)
		return;

	// cancel timeout
	m_pComLayer->UnregisterTimeout(m_pTransID, 2);

	m_bWaitComplete = FALSE;
}

BOOL CSubmitFileClientTransaction::SendReuestPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return FALSE;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PSUBMIT_REQUEST pRequestPacket = (PSUBMIT_REQUEST)(pPacket + 1);

	pPacket->dwMagic = MAGIC_SUBMIT_REQUEST;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(SUBMIT_REQUEST);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	memcpy(&pRequestPacket->guidRequest, &m_guidRequest, sizeof(GUID));
	pRequestPacket->dwStudentIp = m_dwStudentIp;
	pRequestPacket->llSize = m_llSize;
	pRequestPacket->dwCount = m_dwCount;
	_tcscpy(pRequestPacket->szStudentName, m_szStudentName);
	_tcscpy(pRequestPacket->szFileNames, m_szFileNames);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);

	return TRUE;
}

BOOL CSubmitFileClientTransaction::SendCancelPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return FALSE;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PSUBMIT_CANCEL pCancelPacket = (PSUBMIT_CANCEL)(pPacket + 1);

	pPacket->dwMagic = MAGIC_SUBMIT_CANCEL;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(SUBMIT_CANCEL);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	memcpy(&pCancelPacket->guidRequest, &m_guidRequest, sizeof(GUID));
	pCancelPacket->dwStudentIp = m_dwStudentIp;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);

	return TRUE;
}

BOOL CSubmitFileClientTransaction::SendCompletedPacket(int nCompletedFileCount)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return FALSE;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PSUBMIT_COMPLETED pCompletePacket = (PSUBMIT_COMPLETED)(pPacket + 1);

	pPacket->dwMagic = MAGIC_SUBMIT_COMPLETED;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(SUBMIT_COMPLETED);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	memcpy(&pCompletePacket->guidRequest, &m_guidRequest, sizeof(GUID));
	pCompletePacket->dwStudentIp = m_dwStudentIp;
	pCompletePacket->nCompletedFileCount = nCompletedFileCount;
	_tcscpy(pCompletePacket->szStudentName, m_szStudentName);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CThumbnailTransaction::CThumbnailTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
/*
	m_tnPolicy.bEnable = TRUE;
	m_tnPolicy.nWidth = 32;
	m_tnPolicy.nHeight = 24;
	m_tnPolicy.dwInterval = 5;

	m_dwPolicyId = 1;

	m_nMaxBufferSize = 50 * 1024;
*/
	InitializeCriticalSection(&m_cs);

	m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);
}

CThumbnailTransaction::~CThumbnailTransaction()
{
	CTnailMap::iterator iterator = m_tnailMap.begin(); 
	while (iterator != m_tnailMap.end()) {
		TNAIL_CONTEXT *pContext = (*iterator).second;
		free(pContext);
		m_tnailMap.erase(iterator);
		iterator = m_tnailMap.begin();
	}

	DeleteCriticalSection(&m_cs);
}

BOOL CThumbnailTransaction::AddStudent(DWORD dwMemberIp)
{
	CAutoSectionLock lock(&m_cs);

	DWORD dwMemberID = dwMemberIp;

	CTnailMap::iterator iterator = m_tnailMap.find(dwMemberID);
	if (iterator != m_tnailMap.end())
		return TRUE;

	PTNAIL_CONTEXT pTnailContext = (PTNAIL_CONTEXT)malloc(sizeof(TNAIL_CONTEXT) + MAX_THUMBNAIL_BUFFER);
	if (!pTnailContext)
		return FALSE;

	memset(pTnailContext, 0, sizeof(TNAIL_CONTEXT));
	pTnailContext->dwIpAddress = dwMemberIp;

	pTnailContext->dwTeacherPolicyId = 1;
	pTnailContext->dwStudentPolicyId = 0;
	
	pTnailContext->policy.bEnable = FALSE;
	pTnailContext->policy.dwInterval = 10; // 10 seconds
	pTnailContext->policy.nWidth = 64;
	pTnailContext->policy.nHeight = 48;

	pTnailContext->cbBuffer = MAX_THUMBNAIL_BUFFER;

	m_tnailMap.insert(CTnailMap::value_type(dwMemberID, pTnailContext));

	SendPolicyToMember(dwMemberIp, pTnailContext);

	return TRUE;
}

BOOL CThumbnailTransaction::DelStudent(DWORD dwMemberIp)
{
	CAutoSectionLock lock(&m_cs);

	DWORD dwMemberID = dwMemberIp;

	CTnailMap::iterator iterator = m_tnailMap.find(dwMemberID);
	if (iterator == m_tnailMap.end())
		return FALSE;

	PTNAIL_CONTEXT pTnailContext = (*iterator).second;

	free(pTnailContext);
	m_tnailMap.erase(iterator);

	return TRUE;
}

void CThumbnailTransaction::DeleteAllStudents()
{
	CAutoSectionLock lock(&m_cs);

	CTnailMap::iterator iterator = m_tnailMap.begin();
	while (iterator != m_tnailMap.end()) {
		PTNAIL_CONTEXT pTnailContext = (*iterator).second;
		free(pTnailContext);
		m_tnailMap.erase(iterator);

		iterator = m_tnailMap.begin();
	}
}

BOOL CThumbnailTransaction::StartStudent(DWORD dwMemberIp)
{
	CAutoSectionLock lock(&m_cs);

	DWORD dwMemberID = dwMemberIp;

	CTnailMap::iterator iterator = m_tnailMap.find(dwMemberID);
	if (iterator == m_tnailMap.end())
		return FALSE;

	PTNAIL_CONTEXT pTnailContext = (*iterator).second;
	if (!pTnailContext->policy.bEnable) {
		pTnailContext->policy.bEnable = TRUE;
		pTnailContext->dwTeacherPolicyId ++;

		SendPolicyToMember(dwMemberIp, pTnailContext);
	}

	return TRUE;
}

void CThumbnailTransaction::StartAllStudents()
{
	CAutoSectionLock lock(&m_cs);

	CTnailMap::iterator iterator = m_tnailMap.begin();
	while (iterator != m_tnailMap.end()) {

		PTNAIL_CONTEXT pTnailContext = (*iterator).second;
		if (!pTnailContext->policy.bEnable) {
			pTnailContext->policy.bEnable = TRUE;
			pTnailContext->dwTeacherPolicyId ++;

			SendPolicyToMember(pTnailContext->dwIpAddress, pTnailContext);
		}

		iterator ++;
	}
}

BOOL CThumbnailTransaction::StopStudent(DWORD dwMemberIp)
{
	CAutoSectionLock lock(&m_cs);

	DWORD dwMemberID = dwMemberIp;

	CTnailMap::iterator iterator = m_tnailMap.find(dwMemberID);
	if (iterator == m_tnailMap.end())
		return FALSE;

	PTNAIL_CONTEXT pTnailContext = (*iterator).second;
	if (pTnailContext->policy.bEnable) {
		pTnailContext->policy.bEnable = FALSE;
		pTnailContext->dwTeacherPolicyId ++;

		SendPolicyToMember(dwMemberIp, pTnailContext);
	}

	return TRUE;
}

void CThumbnailTransaction::StopAllStudents()
{
	CAutoSectionLock lock(&m_cs);

	CTnailMap::iterator iterator = m_tnailMap.begin();
	while (iterator != m_tnailMap.end()) {

		PTNAIL_CONTEXT pTnailContext = (*iterator).second;
		if (pTnailContext->policy.bEnable) {
			pTnailContext->policy.bEnable = FALSE;
			pTnailContext->dwTeacherPolicyId ++;

			SendPolicyToMember(pTnailContext->dwIpAddress, pTnailContext);
		}

		iterator ++;
	}
}

BOOL CThumbnailTransaction::GetStudentPolicy(DWORD dwMemberIp, PTNAIL_POLICY pTnailPolicy)
{
	if (!pTnailPolicy)
		return FALSE;

	CAutoSectionLock lock(&m_cs);

	DWORD dwMemberID = dwMemberIp;

	CTnailMap::iterator iterator = m_tnailMap.find(dwMemberID);
	if (iterator == m_tnailMap.end())
		return FALSE;

	PTNAIL_CONTEXT pTnailContext = (*iterator).second;
	memcpy(pTnailPolicy, &pTnailContext->policy, sizeof(TNAIL_POLICY));

	return TRUE;
}

BOOL CThumbnailTransaction::SetStudentPolicy(DWORD dwMemberIp, PTNAIL_POLICY pTnailPolicy)
{
	if (!pTnailPolicy)
		return FALSE;

	CAutoSectionLock lock(&m_cs);

	DWORD dwMemberID = dwMemberIp;

	CTnailMap::iterator iterator = m_tnailMap.find(dwMemberID);
	if (iterator == m_tnailMap.end())
		return FALSE;

	PTNAIL_CONTEXT pTnailContext = (*iterator).second;

	if ((pTnailContext->policy.bEnable != pTnailPolicy->bEnable) ||
		(pTnailContext->policy.dwInterval != pTnailPolicy->dwInterval) ||
		(pTnailContext->policy.nWidth != pTnailPolicy->nWidth) ||
		(pTnailContext->policy.nHeight != pTnailPolicy->nHeight)) 
	{
		memcpy(&pTnailContext->policy, pTnailPolicy, sizeof(TNAIL_POLICY));

		pTnailContext->dwTeacherPolicyId ++;

		SendPolicyToMember(pTnailContext->dwIpAddress, pTnailContext);
	}

	return TRUE;
}

void CThumbnailTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	UpdatePolicy();

	m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);
}

void CThumbnailTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;

	switch (pPacket->dwMagic)
	{
	case MAGIC_TNAIL_POLICY_RET:
		{
			// Update policy Id

			if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_THUMBNAIL_POLICY_RET))
				return;

			PPACKET_THUMBNAIL_POLICY_RET pPacketPolicyRet = (PPACKET_THUMBNAIL_POLICY_RET)(pPacket + 1);

			CTnailMap::iterator iterator = m_tnailMap.find(pPacketPolicyRet->dwMemberId);
			if (iterator == m_tnailMap.end())
				return;

			PTNAIL_CONTEXT pTnailContext = (*iterator).second;
			pTnailContext->dwStudentPolicyId = pPacketPolicyRet->dwPolicyId;
		}
		break;
	case MAGIC_TNAIL_REPORT:
		{
			// Update thumbnail

			if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_THUMBNAIL))
				return;

			PPACKET_THUMBNAIL pPacketThumbnail = (PPACKET_THUMBNAIL)(pPacket + 1);

			CTnailMap::iterator iterator = m_tnailMap.find(pPacketThumbnail->dwMemberId);
			if (iterator == m_tnailMap.end())
				return;
			
			PTNAIL_CONTEXT pTnailContext = (*iterator).second;

			// Check policy id
			if (pTnailContext->dwTeacherPolicyId != pTnailContext->dwStudentPolicyId) {
				SendPolicyToMember(pTnailContext->dwIpAddress, pTnailContext);
				return;
			}

			int nPayloadSize = MAX_THUMBNAIL_PAYLOAD;

			if (pTnailContext->nThumbnailId == pPacketThumbnail->dwThumbnailId)
			{
				// Check buffer
				if (pTnailContext->cbReceived + pPacketThumbnail->cbSize > MAX_THUMBNAIL_BUFFER) {
					OutputDebugString(_T("\n OnReceiveComplete=> Thumbnail buffer overflow!"));
					return;
				}

				int nPayloadId = pPacketThumbnail->dwOffset / nPayloadSize;
				if (nPayloadId > MAX_THUMBNAIL_BLOCKS)
					return;

				if (pTnailContext->abMap[nPayloadId] == 0) {
					memcpy(pTnailContext->abBuffer + pPacketThumbnail->dwOffset, pPacketThumbnail->abBuffer, pPacketThumbnail->cbSize);
					pTnailContext->cbReceived += pPacketThumbnail->cbSize;

					// Mark received
					pTnailContext->abMap[nPayloadId] = 1;

					if (pTnailContext->cbReceived == pTnailContext->cbTotalSize) {
						// Report event
						ReportThumbnailEvent(pPacketThumbnail->dwMemberId, pTnailContext->abBuffer, pTnailContext->cbTotalSize);
					}
				}
			} else
			{
				pTnailContext->nThumbnailId = pPacketThumbnail->dwThumbnailId;
				pTnailContext->cbTotalSize = pPacketThumbnail->dwTotalSize;
				pTnailContext->cbReceived = pPacketThumbnail->cbSize;

				memset(pTnailContext->abMap, 0, MAX_THUMBNAIL_BLOCKS);

				memcpy(pTnailContext->abBuffer + pPacketThumbnail->dwOffset, pPacketThumbnail->abBuffer, pPacketThumbnail->cbSize);

				// Mark received
				pTnailContext->abMap[pPacketThumbnail->dwOffset / nPayloadSize] = 1;

				if (pTnailContext->cbReceived == pTnailContext->cbTotalSize) {
					// Report event
					ReportThumbnailEvent(pPacketThumbnail->dwMemberId, pTnailContext->abBuffer, pTnailContext->cbTotalSize);
				}
			}
		}
	case MAGIC_TNAIL_REPORT_END:
		{
			if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_THUMBNAIL_END))
				return;

			PPACKET_THUMBNAIL_END pPacketThumbnailEnd = (PPACKET_THUMBNAIL_END)(pPacket + 1);

			CTnailMap::iterator iterator = m_tnailMap.find(pPacketThumbnailEnd->dwMemberId);
			if (iterator == m_tnailMap.end())
				return;

			PTNAIL_CONTEXT pTnailContext = (*iterator).second;

			if (pTnailContext->nThumbnailId == pPacketThumbnailEnd->dwThumbnailId)
			{
				if (pTnailContext->cbReceived == pTnailContext->cbTotalSize)
				{
					// all received
					IBufferItem *pItem = m_pComLayer->AllocBufferItem();
					if (NULL == pItem)
						return;

					pItem->SetTransactionID(m_pTransID);
					pItem->SetDstIpAddress(CIpAddress(pPacketThumbnailEnd->dwMemberIp, m_pComLayer->GetComPort()));

					PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
					pPacket->dwMagic = MAGIC_TNAIL_REPORT_RESULT;
					pPacket->dwVersion = LANGLAB_VERSION;
					pPacket->dwLength = sizeof(PACKET_THUMBNAIL_RESULT);
					memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

					PPACKET_THUMBNAIL_RESULT pPacketThumbResult = (PPACKET_THUMBNAIL_RESULT)(pPacket + 1);
					pPacketThumbResult->dwThumbnailId = pTnailContext->nThumbnailId;
					pPacketThumbResult->bReceivedOk = TRUE;
					pPacketThumbResult->dwLostCount = 0;

					pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);

					m_pComLayer->AddToSendBuffer(pItem);
					
				} else
				{
					// lost some packet
					IBufferItem *pItem = m_pComLayer->AllocBufferItem();
					if (NULL == pItem)
						return;

					pItem->SetTransactionID(m_pTransID);
					pItem->SetDstIpAddress(CIpAddress(pPacketThumbnailEnd->dwMemberIp, m_pComLayer->GetComPort()));

					PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
					pPacket->dwMagic = MAGIC_TNAIL_REPORT_RESULT;
					pPacket->dwVersion = LANGLAB_VERSION;
//					pPacket->dwLength = sizeof(PACKET_THUMBNAIL_RESULT);
					memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

					PPACKET_THUMBNAIL_RESULT pPacketThumbResult = (PPACKET_THUMBNAIL_RESULT)(pPacket + 1);
					pPacketThumbResult->dwThumbnailId = pTnailContext->nThumbnailId;
					pPacketThumbResult->bReceivedOk = FALSE;
					pPacketThumbResult->dwLostCount = 0;

					int nPayloadSize = MAX_THUMBNAIL_PAYLOAD;
					int nCount = (pTnailContext->cbTotalSize + nPayloadSize - 1) / nPayloadSize;
					for (int i=0; i<nCount; i++) {
						if (!pTnailContext->abMap[i]) {
							pPacketThumbResult->abLostIds[pPacketThumbResult->dwLostCount] = i;
							pPacketThumbResult->dwLostCount ++;
						}
					}

					pPacket->dwLength = sizeof(PACKET_THUMBNAIL_RESULT) + pPacketThumbResult->dwLostCount * sizeof(WORD);

					pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);

					m_pComLayer->AddToSendBuffer(pItem);
				}
			} else
			{
				// all packet lost, should request all packets

				IBufferItem *pItem = m_pComLayer->AllocBufferItem();
				if (NULL == pItem)
					return;

				pItem->SetTransactionID(m_pTransID);
				pItem->SetDstIpAddress(CIpAddress(pPacketThumbnailEnd->dwMemberIp, m_pComLayer->GetComPort()));

				PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
				pPacket->dwMagic = MAGIC_TNAIL_REPORT_RESULT;
				pPacket->dwVersion = LANGLAB_VERSION;
				pPacket->dwLength = sizeof(PACKET_THUMBNAIL_RESULT);
				memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

				PPACKET_THUMBNAIL_RESULT pPacketThumbResult = (PPACKET_THUMBNAIL_RESULT)(pPacket + 1);
				pPacketThumbResult->dwThumbnailId = pPacketThumbnailEnd->dwThumbnailId;
				pPacketThumbResult->bReceivedOk = FALSE;
				pPacketThumbResult->dwLostCount = 0xFFFFFFFF;

				pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);

				m_pComLayer->AddToSendBuffer(pItem);
			}
		}
		break;
	default:
		break;
	}
}

void CThumbnailTransaction::ReportThumbnailEvent(DWORD dwMemberId, LPBYTE lpJPGBuffer, DWORD cbSize) 
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_THUMBNAIL_ARRIVED) + cbSize;
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_THUMBNAIL_ARRIVED pEvent = (PEVENT_THUMBNAIL_ARRIVED)(pHeader + 1);

	pHeader->dwEvent = EV_THUMBNAIL_ARRIVED;
	pHeader->dwLength = sizeof(EVENT_THUMBNAIL_ARRIVED) + cbSize;

	pEvent->dwMemberId = dwMemberId;
	pEvent->cbJPGSize = cbSize;
	memcpy(pEvent->abJPGBuffer, lpJPGBuffer, cbSize);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CThumbnailTransaction::UpdatePolicy()
{
	CTnailMap::iterator iterator = m_tnailMap.begin(); 
	while (iterator != m_tnailMap.end()) {
		TNAIL_CONTEXT *pContext = (*iterator).second;

		if (pContext->dwTeacherPolicyId != pContext->dwStudentPolicyId)
			SendPolicyToMember(pContext->dwIpAddress, pContext);

		iterator ++;
	}
}

void CThumbnailTransaction::SendPolicyToMember(DWORD dwMemberIp, TNAIL_CONTEXT *pContext)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwMemberIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_TNAIL_POLICY;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_THUMBNAIL_POLICY);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_THUMBNAIL_POLICY pPacketPolicy = (PPACKET_THUMBNAIL_POLICY)(pPacket + 1);
	pPacketPolicy->dwPolicyId = pContext->dwTeacherPolicyId;
	pPacketPolicy->bEnable = pContext->policy.bEnable;
	pPacketPolicy->nWidth = pContext->policy.nWidth;
	pPacketPolicy->nHeight = pContext->policy.nHeight;
	pPacketPolicy->dwInterval = pContext->policy.dwInterval;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);

	m_pComLayer->AddToSendBuffer(pItem);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CPassiveThumbnailTransaction::CPassiveThumbnailTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_nThumbnailId = 0;
	m_nThumbnailSuccessId = -1;

	m_dwMemberId = 0;
	m_dwMemberIp = 0;
	m_dwTeacherIp = 0;

	m_tnPolicy.bEnable = FALSE;
	m_tnPolicy.nWidth = 64;
	m_tnPolicy.nHeight = 48;
	m_tnPolicy.dwInterval = 10;

	m_dwPolicyId = 0;

	m_bStarted = FALSE;

	m_pJpgBuffer = (BYTE *)malloc(MAX_THUMBNAIL_BUFFER);
	m_dwJpgBufferSize = 0;

	InitializeCriticalSection(&m_cs);
}

CPassiveThumbnailTransaction::~CPassiveThumbnailTransaction()
{
	if (m_pJpgBuffer) {
		free(m_pJpgBuffer);
		m_pJpgBuffer = NULL;
	}

	DeleteCriticalSection(&m_cs);
}

void CPassiveThumbnailTransaction::Startup(DWORD dwLocalIp, DWORD dwTeacherIp)
{
	CAutoSectionLock lock(&m_cs);

	m_dwMemberId = dwLocalIp;
	m_dwMemberIp = dwLocalIp;
	m_dwTeacherIp = dwTeacherIp;
	m_dwPolicyId = 0;

	m_bStarted = TRUE;
}

void CPassiveThumbnailTransaction::Shutdown()
{
	CAutoSectionLock lock(&m_cs);

	m_bStarted = FALSE;
	m_dwPolicyId = 0;
}

void CPassiveThumbnailTransaction::SendThumbnail(BYTE* lpJpgBuffer, DWORD dwJpgBufferSize)
{
	if (!m_bStarted)
		return;

	if ((NULL == m_pJpgBuffer) || (0 == dwJpgBufferSize)) {
		m_pComLayer->UnregisterTimeout(m_pTransID, 1);
		m_pComLayer->RegisterTimeout(m_pTransID, m_tnPolicy.dwInterval * 1000, 1);
		return;
	} else
	{
		CAutoSectionLock lock(&m_cs);

		if (!m_tnPolicy.bEnable)
			return;

		if (dwJpgBufferSize > MAX_THUMBNAIL_BUFFER)
			return;

		// Save Send Buffer
		memcpy(m_pJpgBuffer, lpJpgBuffer, dwJpgBufferSize);
		m_dwJpgBufferSize = dwJpgBufferSize;

		m_nThumbnailId ++;

		int nPayloadSize = MAX_THUMBNAIL_PAYLOAD;
		int nCount = (dwJpgBufferSize + nPayloadSize - 1) / nPayloadSize;

		for (int i=0; i<nCount; i++) {
			IBufferItem *pItem = m_pComLayer->AllocBufferItem();
			if (NULL == pItem)
				return;

			pItem->SetTransactionID(m_pTransID);
			pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

			PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
			PPACKET_THUMBNAIL pPacketThumbnail = (PPACKET_THUMBNAIL)(pPacket + 1);

			pPacket->dwMagic = MAGIC_TNAIL_REPORT;
			pPacket->dwVersion = LANGLAB_VERSION;
	//		pPacket->dwLength = sizeof(PACKET_THUMBNAIL) + nPayloadSize;
			memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

			pPacketThumbnail->dwMemberId = m_dwMemberId;
			pPacketThumbnail->dwThumbnailId = m_nThumbnailId;
			pPacketThumbnail->dwTotalSize = dwJpgBufferSize;
			pPacketThumbnail->dwOffset = nPayloadSize * i;

			if (i == (nCount -1))
				pPacketThumbnail->cbSize = pPacketThumbnail->dwTotalSize - pPacketThumbnail->dwOffset;
			else
				pPacketThumbnail->cbSize = nPayloadSize;

			memcpy(pPacketThumbnail->abBuffer, lpJpgBuffer + pPacketThumbnail->dwOffset, pPacketThumbnail->cbSize);

			pPacket->dwLength = sizeof(PACKET_THUMBNAIL) + pPacketThumbnail->cbSize;

			pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
			m_pComLayer->AddToSendBuffer(pItem);
		}
	}

	m_pComLayer->RegisterTimeout(m_pTransID, 500, 0);
}

void CPassiveThumbnailTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (dwTimeID == 0) {
		SendThumbnailEndPacket();
		m_pComLayer->RegisterTimeout(m_pTransID, 500, 0);
	}

	if (dwTimeID == 1)
	{
		if (m_tnPolicy.bEnable)
			RefreshThumbnail();
	}
}

void CPassiveThumbnailTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;

	switch (pPacket->dwMagic)
	{
	case MAGIC_TNAIL_REPORT_RESULT:
		{
			if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_THUMBNAIL_RESULT))
				return;

			PPACKET_THUMBNAIL_RESULT pThumbNailResult = (PPACKET_THUMBNAIL_RESULT)(pPacket + 1);

			if (pThumbNailResult->dwThumbnailId != m_nThumbnailId)
				return;

			// Finished send end packet
			m_pComLayer->UnregisterTimeout(m_pTransID, 0);

			if (pThumbNailResult->bReceivedOk)
			{
				if (m_nThumbnailSuccessId < m_nThumbnailId) {

					// Request new thumbnail after several seconds
					m_pComLayer->UnregisterTimeout(m_pTransID, 1);
					m_pComLayer->RegisterTimeout(m_pTransID, m_tnPolicy.dwInterval * 1000, 1);

					m_nThumbnailSuccessId = m_nThumbnailId;
				} else
				{
					// ignore later success report
				}
			} else
			{
				int nPayloadSize = MAX_THUMBNAIL_PAYLOAD;
				int nCount = (m_dwJpgBufferSize + nPayloadSize - 1) / nPayloadSize;

				BOOL bAllLost = FALSE;
				DWORD dwLostCount = pThumbNailResult->dwLostCount;
				if (dwLostCount == 0xFFFFFFFF) {
					bAllLost = TRUE;
					dwLostCount = nCount;
				}

				for (DWORD i=0; i<dwLostCount; i++) {

					int nPayloadId = bAllLost ? i : pThumbNailResult->abLostIds[i];

					IBufferItem *pItem = m_pComLayer->AllocBufferItem();
					if (NULL == pItem)
						return;

					pItem->SetTransactionID(m_pTransID);
					pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

					PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
					PPACKET_THUMBNAIL pPacketThumbnail = (PPACKET_THUMBNAIL)(pPacket + 1);

					pPacket->dwMagic = MAGIC_TNAIL_REPORT;
					pPacket->dwVersion = LANGLAB_VERSION;
			//		pPacket->dwLength = sizeof(PACKET_THUMBNAIL) + nPayloadSize;
					memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

					pPacketThumbnail->dwMemberId = m_dwMemberId;
					pPacketThumbnail->dwThumbnailId = m_nThumbnailId;
					pPacketThumbnail->dwTotalSize = m_dwJpgBufferSize;
					pPacketThumbnail->dwOffset = nPayloadSize * nPayloadId;

					if (nPayloadId == (nCount -1))
						pPacketThumbnail->cbSize = pPacketThumbnail->dwTotalSize - pPacketThumbnail->dwOffset;
					else
						pPacketThumbnail->cbSize = nPayloadSize;

					memcpy(pPacketThumbnail->abBuffer, m_pJpgBuffer + pPacketThumbnail->dwOffset, pPacketThumbnail->cbSize);

					pPacket->dwLength = sizeof(PACKET_THUMBNAIL) + pPacketThumbnail->cbSize;

					pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
					m_pComLayer->AddToSendBuffer(pItem);
					
				}

				m_pComLayer->RegisterTimeout(m_pTransID, 500, 0);
			}
		}
		break;
	case MAGIC_TNAIL_POLICY:
		{
			if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_THUMBNAIL_POLICY))
				return;

			PPACKET_THUMBNAIL_POLICY pPacketPolicy = (PPACKET_THUMBNAIL_POLICY)(pPacket + 1);

			// Already got the policy
			if (m_dwPolicyId == pPacketPolicy->dwPolicyId) {
				SendPolicyReply();
				return;
			}

			if (m_dwPolicyId == 0) {
				// Get policy first Time
				m_tnPolicy.bEnable = pPacketPolicy->bEnable;
				m_tnPolicy.dwInterval = pPacketPolicy->dwInterval;
				m_tnPolicy.nWidth = pPacketPolicy->nWidth;
				m_tnPolicy.nHeight = pPacketPolicy->nHeight;

				m_dwPolicyId = pPacketPolicy->dwPolicyId;

				// Send Policy Ret
				SendPolicyReply();

				if (m_tnPolicy.bEnable) {
					RefreshThumbnail();
				}
			} else
			{
				// Teacher changed policy again
				m_dwPolicyId = pPacketPolicy->dwPolicyId;

				SendPolicyReply();

				// Enable changed
				if (m_tnPolicy.bEnable != pPacketPolicy->bEnable) {
					m_tnPolicy.bEnable = pPacketPolicy->bEnable;
					m_tnPolicy.dwInterval = pPacketPolicy->dwInterval;
					m_tnPolicy.nWidth = pPacketPolicy->nWidth;
					m_tnPolicy.nHeight = pPacketPolicy->nHeight;

					if (m_tnPolicy.bEnable) {
						RefreshThumbnail();
					} else
					{
						// Cancel thumbnail timer
						m_pComLayer->UnregisterTimeout(m_pTransID, 1);
					}

					return;
				}

				// Size changed
				if ((m_tnPolicy.nWidth != pPacketPolicy->nWidth) ||
					(m_tnPolicy.nHeight != pPacketPolicy->nHeight)) {

					m_tnPolicy.nWidth = pPacketPolicy->nWidth;
					m_tnPolicy.nHeight = pPacketPolicy->nHeight;
					m_tnPolicy.dwInterval = pPacketPolicy->dwInterval;

					if (m_tnPolicy.bEnable) {
						RefreshThumbnail();
					}

					return;
				}

				// Interval changed
				if (m_tnPolicy.dwInterval != pPacketPolicy->dwInterval) {
					m_tnPolicy.dwInterval = pPacketPolicy->dwInterval;
				}
			}
		}
		break;
	default:
		break;
	}
}

void CPassiveThumbnailTransaction::RefreshThumbnail()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_THUMBNAIL_UPDATE);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_UPDATE_THUMBNAIL;
	pHeader->dwLength = sizeof(EVENT_THUMBNAIL_UPDATE);

	PEVENT_THUMBNAIL_UPDATE pEvent = (PEVENT_THUMBNAIL_UPDATE)(pHeader + 1);
	pEvent->nWidth = m_tnPolicy.nWidth;
	pEvent->nHeight = m_tnPolicy.nHeight;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveThumbnailTransaction::SendPolicyReply()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_TNAIL_POLICY_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_THUMBNAIL_POLICY_RET);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_THUMBNAIL_POLICY_RET pPacketPolicyRet = (PPACKET_THUMBNAIL_POLICY_RET)(pPacket + 1);
	pPacketPolicyRet->dwMemberId = m_dwMemberId;
	pPacketPolicyRet->dwPolicyId = m_dwPolicyId;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);

	m_pComLayer->AddToSendBuffer(pItem);
}

void CPassiveThumbnailTransaction::SendThumbnailEndPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_TNAIL_REPORT_END;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_THUMBNAIL_END);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_THUMBNAIL_END pPacketThumbEnd = (PPACKET_THUMBNAIL_END)(pPacket + 1);
	pPacketThumbEnd->dwMemberId = m_dwMemberId;
	pPacketThumbEnd->dwMemberIp = m_dwMemberIp;
	pPacketThumbEnd->dwThumbnailId = m_nThumbnailId;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);

	m_pComLayer->AddToSendBuffer(pItem);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CFindTeacherTransaction::CFindTeacherTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwChannelId = 0xFFFFFFFF;
	m_bApplySuccess = FALSE;

	m_bStarted = FALSE;
	m_bLocked = FALSE;

	m_nNumClient = 0;

	m_hEvent = NULL;

	InitializeCriticalSection(&m_cs);
}

CFindTeacherTransaction::~CFindTeacherTransaction()
{
	DeleteCriticalSection(&m_cs);
}

BOOL CFindTeacherTransaction::Startup(TCHAR* pszTeacherName, DWORD dwLocalAddr, PCLIENTID pClientId, int nCount, DWORD* pdwChannelId)
{
	if (m_bStarted)
		return TRUE;

	_tcscpy(m_szTeacherName, pszTeacherName);
	m_dwLocalAddr = dwLocalAddr;

	m_nNumClient = nCount % MAX_CLIENTS_NUMBER;

	memcpy(m_abClientId, pClientId, sizeof(CLIENTID) * nCount);

	// Start apply channel
	m_dwChannelId = GetTickCount() % MAX_CHANNEL_NUMBER;
	m_dwStartApplyTime = GetTickCount();

	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	SendApplyChannel();
	m_pComLayer->RegisterTimeout(m_pTransID, APPLY_CHANNEL_INTERVAL, APPLY_CHANNEL);

	WaitForSingleObject(m_hEvent, INFINITE);
	CloseHandle(m_hEvent);
	m_hEvent = NULL;

	*pdwChannelId = m_dwChannelId;

	m_bStarted = TRUE;

	return TRUE;
}

void CFindTeacherTransaction::Shutdown()
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (m_bApplySuccess) {
		m_pComLayer->UnregisterTimeout(m_pTransID, ANNOUNCE_CHANNEL);

		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		SendQuitChannel(m_dwChannelId);

		WaitForSingleObject(m_hEvent, 2000);
		CloseHandle(m_hEvent);
		m_hEvent = NULL;

		m_bApplySuccess = FALSE;
	} else
		m_pComLayer->UnregisterTimeout(m_pTransID, APPLY_CHANNEL);

	m_bStarted = FALSE;
	m_bLocked = FALSE;
}

void CFindTeacherTransaction::SetLocked(BOOL bLock)
{
	CAutoSectionLock lock(&m_cs);

	m_bLocked = bLock;
}

void CFindTeacherTransaction::ResetUnconnectedMembers(PCLIENTID pClientId, int nCount)
{
	CAutoSectionLock lock(&m_cs);

	m_nNumClient = nCount;
	memcpy(m_abClientId, pClientId, sizeof(CLIENTID) * nCount);
}

void CFindTeacherTransaction::AddUnconnectedMember(PCLIENTID pClientId)
{
	CAutoSectionLock lock(&m_cs);

	if (m_nNumClient >= MAX_CLIENTS_NUMBER)
		return;

	for (int i=0; i<m_nNumClient; i++) {
		if (memcmp(&m_abClientId[i], pClientId, sizeof(CLIENTID)) == 0) {
			return;
		}
	}

	memcpy(&m_abClientId[m_nNumClient], pClientId, sizeof(CLIENTID));
	m_nNumClient ++;
}

void CFindTeacherTransaction::RemoveUnconnectedMember(PCLIENTID pClientId)
{
	CAutoSectionLock lock(&m_cs);

	if (m_nNumClient <= 0)
		return;

	int i;
	for (i=0; i<m_nNumClient; i++) {
		if (memcmp(&m_abClientId[i], pClientId, sizeof(CLIENTID)) == 0) {
			break;
		}
	}

	if (i >= m_nNumClient)
		return;

	if (i == (m_nNumClient - 1)) {
		m_nNumClient --;
		return;
	}

	int j;
	for (j=i; j<m_nNumClient-1; j++) {
		memcpy(&m_abClientId[j], &m_abClientId[j+1], sizeof(CLIENTID));
	}

	m_nNumClient --;
}

void CFindTeacherTransaction::SendRejectConnectToStudent(DWORD dwIpAddress, int nChannelId, DWORD dwRejectCode)
{
	CAutoSectionLock lock(&m_cs);

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PFINDTEACHER_REJECT_CONNECT pRejectConnect = (PFINDTEACHER_REJECT_CONNECT)(pPacket + 1);

	pPacket->dwMagic = MAGIC_REJECT_CONNECT;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(FINDTEACHER_REJECT_CONNECT);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pRejectConnect->dwTeacherIp = m_dwLocalAddr;
	pRejectConnect->nChannelId = nChannelId;
	pRejectConnect->dwRejectCode = dwRejectCode;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CFindTeacherTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == APPLY_CHANNEL)
	{
		if (GetTickCount() - m_dwStartApplyTime > MAX_APPLY_TIMEOUT)
		{
			m_bApplySuccess = TRUE;

			// Report apply channel success event
			SetEvent(m_hEvent);

			// Send announce channel packet
			SendAnnounceChannel();
			m_pComLayer->RegisterTimeout(m_pTransID, ANNOUNCE_CHANNEL_INTERVAL, ANNOUNCE_CHANNEL);
		} else
		{
			// apply channel
			SendApplyChannel();
			m_pComLayer->RegisterTimeout(m_pTransID, APPLY_CHANNEL_INTERVAL, APPLY_CHANNEL);
		}
	}

	if (dwTimeID == ANNOUNCE_CHANNEL) 
	{
		// Send announce channel packet
		SendAnnounceChannel();
		m_pComLayer->RegisterTimeout(m_pTransID, ANNOUNCE_CHANNEL_INTERVAL, ANNOUNCE_CHANNEL);
	}
}

void CFindTeacherTransaction::OnSendComplete(LPVOID lpvData, DWORD cbData)
{
	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	if (pPacket->dwMagic == MAGIC_QUIT_CHANNEL) {
		SetEvent(m_hEvent);
	}
}

void CFindTeacherTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;

	switch (pPacket->dwMagic)
	{
	case MAGIC_APPLY_CHANNEL: // teacher
		{
			PFINDTEACHER_APPLY_CHANNEL pApplyChannel = (PFINDTEACHER_APPLY_CHANNEL)(pPacket + 1);
			if (m_dwChannelId != pApplyChannel->nChannelId) 
				break;

			SendFightChannel(pApplyChannel->dwTeacherIp);
			
			// If I am applying the same channel, I will re-apply another channel
			if (!m_bApplySuccess) {

				m_pComLayer->UnregisterTimeout(m_pTransID, APPLY_CHANNEL);

				m_dwChannelId = (GetTickCount() + rand())% MAX_CHANNEL_NUMBER;
				m_dwStartApplyTime = GetTickCount();

				SendApplyChannel();
				m_pComLayer->RegisterTimeout(m_pTransID, APPLY_CHANNEL_INTERVAL, APPLY_CHANNEL);
			}
		}

		break;
	case MAGIC_FIGHT_CHANNEL: // teacher
		{
			// If I have applied a channel successful, I will ignore the packet
			if (m_bApplySuccess) {
				// This only happens on I have just applied a channel for very short time, and
				// another teacher have just get started and applied the same channel with me. I think 
				// He will learn from my period annouce packet and re-apply a new channel himeself.

				break;
			}

			// If I am apllying channel, and another teacher report collision to me, 
			// I will re-apply another channel...
			PFINDTEACHER_FIGHT_CHANNEL pFightChannel = (PFINDTEACHER_FIGHT_CHANNEL)(pPacket + 1);

			if (pFightChannel->dwFighterAddress == m_dwLocalAddr) 
				break;

			m_pComLayer->UnregisterTimeout(m_pTransID, APPLY_CHANNEL);

			m_dwChannelId = (GetTickCount() + rand())% MAX_CHANNEL_NUMBER;
			m_dwStartApplyTime = GetTickCount();

			SendApplyChannel();
			m_pComLayer->RegisterTimeout(m_pTransID, APPLY_CHANNEL_INTERVAL, APPLY_CHANNEL);
		}
		break;
	case MAGIC_CANCEL_CONNECT:	// teacher
		{
			PFINDTEACHER_CANCEL_CONNECT pCancelConnect = (PFINDTEACHER_CANCEL_CONNECT)(pPacket + 1);

			// Report connect cannceled event
			ReportConnectCanceledEvent(pCancelConnect->abMacAddress, pCancelConnect->dwClientIp);
		}
		break;
	case MAGIC_ASK_ACCESS: // teacher
		{
			if (!m_bApplySuccess)
				break;

			PFINDTEACHER_ASK_ACCESS pAskAccess = (PFINDTEACHER_ASK_ACCESS)(pPacket + 1);
			SendAnswerAccessible(pAskAccess->dwStudentIp);
		}
		break;
	case MAGIC_ANNOUNCE_CHANNEL: // teacher and student
		{
			PFINDTEACHER_ANNOUNCE_CHANNEL pAnnounceChannel = (PFINDTEACHER_ANNOUNCE_CHANNEL)(pPacket + 1);

			// If I am apllying channel, and another teacher announce that he have used the channel, 
			// I will re-apply another channel...
			if ((!m_bApplySuccess) && (pAnnounceChannel->nChannelId == m_dwChannelId)) {

				m_pComLayer->UnregisterTimeout(m_pTransID, APPLY_CHANNEL);

				m_dwChannelId = (GetTickCount() + rand())% MAX_CHANNEL_NUMBER;
				m_dwStartApplyTime = GetTickCount();

				SendApplyChannel();
				m_pComLayer->RegisterTimeout(m_pTransID, APPLY_CHANNEL_INTERVAL, APPLY_CHANNEL);
			}
		}
		break;
	}
}

void CFindTeacherTransaction::SendApplyChannel()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_pComLayer->GetComAddress(), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PFINDTEACHER_APPLY_CHANNEL pApplyChannel = (PFINDTEACHER_APPLY_CHANNEL)(pPacket + 1);

	pPacket->dwMagic = MAGIC_APPLY_CHANNEL;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(FINDTEACHER_APPLY_CHANNEL);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pApplyChannel->nChannelId = m_dwChannelId;
	pApplyChannel->dwTeacherIp = m_dwLocalAddr;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CFindTeacherTransaction::SendAnnounceChannel()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_pComLayer->GetComAddress(), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PFINDTEACHER_ANNOUNCE_CHANNEL pAnnounceChannel = (PFINDTEACHER_ANNOUNCE_CHANNEL)(pPacket + 1);

	pPacket->dwMagic = MAGIC_ANNOUNCE_CHANNEL;
	pPacket->dwVersion = LANGLAB_VERSION;
//	pPacket->dwLength = sizeof(FINDTEACHER_ANNOUNCE_CHANNEL);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pAnnounceChannel->dwIpAddress = m_dwLocalAddr;

	pAnnounceChannel->nChannelId = m_dwChannelId;
	pAnnounceChannel->bLock = m_bLocked;
	pAnnounceChannel->dwClientCount = 0;

	TCHAR* pszName = (TCHAR *)pAnnounceChannel->abParams;
	_tcscpy(pszName, m_szTeacherName);
	pAnnounceChannel->nNameLength = _tcslen(pszName);

	if (pAnnounceChannel->bLock) 
	{
		pAnnounceChannel->dwClientCount = m_nNumClient;
		if (pAnnounceChannel->dwClientCount > 0)
		{
			CLIENTID* abClientId = (CLIENTID *)(pAnnounceChannel->abParams + (pAnnounceChannel->nNameLength + 1) * sizeof(TCHAR));
			memcpy(abClientId, m_abClientId, sizeof(CLIENTID) * pAnnounceChannel->dwClientCount);
		}
	}

	pPacket->dwLength = sizeof(FINDTEACHER_ANNOUNCE_CHANNEL) + (pAnnounceChannel->nNameLength + 1) * sizeof(TCHAR);
	pPacket->dwLength += pAnnounceChannel->dwClientCount * sizeof(CLIENTID);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CFindTeacherTransaction::SendQuitChannel(DWORD nChannelId)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_pComLayer->GetComAddress(), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PFINDTEACHER_QUIT_CHANNEL pQuitClass = (PFINDTEACHER_QUIT_CHANNEL)(pPacket + 1);

	pPacket->dwMagic = MAGIC_QUIT_CHANNEL;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(FINDTEACHER_QUIT_CHANNEL);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pQuitClass->nChannelId = nChannelId;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CFindTeacherTransaction::SendFightChannel(DWORD dwTeacherIp)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PFINDTEACHER_FIGHT_CHANNEL pFightChannel = (PFINDTEACHER_FIGHT_CHANNEL)(pPacket + 1);

	pPacket->dwMagic = MAGIC_FIGHT_CHANNEL;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(FINDTEACHER_FIGHT_CHANNEL);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pFightChannel->dwFighterAddress = m_dwLocalAddr;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CFindTeacherTransaction::SendAnswerAccessible(DWORD dwClientIp)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwClientIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PFINDTEACHER_ANSWER_ACCESS pAnswerAccess = (PFINDTEACHER_ANSWER_ACCESS)(pPacket + 1);

	pPacket->dwMagic = MAGIC_ANSWER_ACCESS;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(FINDTEACHER_ANSWER_ACCESS);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pAnswerAccess->dwTeacherIp = m_dwLocalAddr;
	pAnswerAccess->nChannelId = m_dwChannelId;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CFindTeacherTransaction::ReportConnectCanceledEvent(BYTE* abMac, DWORD dwClientIp)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_CANCEL_CONNECT);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_CANCEL_CONNECT pEvent = (PEVENT_CANCEL_CONNECT)(pHeader + 1);

	pHeader->dwEvent = EV_CONNECT_CANCEL;
	pHeader->dwLength = sizeof(EVENT_CANCEL_CONNECT);

	pEvent->dwClientIp = dwClientIp;
	memcpy(pEvent->abMac, abMac, 6);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

CPassiveFindTeacherTransaction::CPassiveFindTeacherTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwLocalAddr = 0;
	m_bStarted = FALSE;
	m_bLogin = FALSE;
	m_dwTeacherIp = 0;

	InitializeCriticalSection(&m_cs);
}

CPassiveFindTeacherTransaction::~CPassiveFindTeacherTransaction()
{
	DeleteCriticalSection(&m_cs);
}

BOOL CPassiveFindTeacherTransaction::Startup(DWORD dwLocalAddr, PCLIENTID pClientId)
{
	if (m_bStarted)
		return TRUE;

	m_dwLocalAddr = dwLocalAddr;
	memcpy(&m_idClient, pClientId, sizeof(CLIENTID));

	CheckChannelLastAnnounceTime();
	m_pComLayer->RegisterTimeout(m_pTransID, CHECK_CHANNEL_INTERVAL, CHECK_CHANNEL);

	m_bStarted = TRUE;

	return TRUE;
}

void CPassiveFindTeacherTransaction::Shutdown()
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	m_pComLayer->UnregisterTimeout(m_pTransID, CHECK_CHANNEL);

	// clear all channnel
	CChannelMap::iterator iterator = m_channelMap.begin();
	while (iterator != m_channelMap.end()) {
		TEACHER_CHANNEL *pChannel = (*iterator).second;
		free(pChannel);
		m_channelMap.erase(iterator);
		iterator = m_channelMap.begin();
	}

	m_bStarted = FALSE;
}

void CPassiveFindTeacherTransaction::SetLogIn(BOOL bLogin, DWORD dwTeacherIp)
{
	CAutoSectionLock lock(&m_cs);

	m_bLogin = bLogin;
	m_dwTeacherIp = dwTeacherIp;
}

BOOL CPassiveFindTeacherTransaction::IsChannelLocked(DWORD nChannelId)
{
	CAutoSectionLock lock(&m_cs);

	CChannelMap::iterator iterator = m_channelMap.find(nChannelId);
	if (iterator == m_channelMap.end())
		return FALSE;

	TEACHER_CHANNEL *pChannel = (*iterator).second;
	return pChannel->bLock;
}

void CPassiveFindTeacherTransaction::SendCancelConnectToTeacher(DWORD nChannelId, BYTE* pabMac)
{
	CAutoSectionLock lock(&m_cs);

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(GetChannelTeacherIp(nChannelId), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PFINDTEACHER_CANCEL_CONNECT pCancelConnect = (PFINDTEACHER_CANCEL_CONNECT)(pPacket + 1);

	pPacket->dwMagic = MAGIC_CANCEL_CONNECT;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(FINDTEACHER_CANCEL_CONNECT);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	memcpy(pCancelConnect->abMacAddress, pabMac, 6);
	pCancelConnect->dwClientIp = m_dwLocalAddr;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPassiveFindTeacherTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == CHECK_CHANNEL) {
		CheckChannelLastAnnounceTime();
		m_pComLayer->RegisterTimeout(m_pTransID, CHECK_CHANNEL_INTERVAL, CHECK_CHANNEL);
	}
}

void CPassiveFindTeacherTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;

	switch (pPacket->dwMagic)
	{
	case MAGIC_ANNOUNCE_CHANNEL: // teacher and student
		{
			PFINDTEACHER_ANNOUNCE_CHANNEL pAnnounceChannel = (PFINDTEACHER_ANNOUNCE_CHANNEL)(pPacket + 1);

			if (HasChannel(pAnnounceChannel->nChannelId))
			{
				UpdateChannelLastAnnounce(pAnnounceChannel);
			} else
			{
				InsertChannel(pAnnounceChannel);
				SendAskAccessible(pAnnounceChannel->dwIpAddress);
			}
		}
		break;

	case MAGIC_ANSWER_ACCESS:	// student
		{
			PFINDTEACHER_ANSWER_ACCESS pAnswerAccess = (PFINDTEACHER_ANSWER_ACCESS)(pPacket + 1);
			CChannelMap::iterator iterator = m_channelMap.find(pAnswerAccess->nChannelId);
			if (iterator == m_channelMap.end())
				break;

			TEACHER_CHANNEL *pChannel = (*iterator).second;
			if (pChannel->bAccessble)
				break;

			pChannel->bAccessble = TRUE;

			ReportChannelArriveEvent(pChannel->szName, pChannel->dwIpAddress,
				pAnswerAccess->nChannelId);
		}

		break;
	case MAGIC_QUIT_CHANNEL:	// student
		{
			PFINDTEACHER_QUIT_CHANNEL pQuitChannel = (PFINDTEACHER_QUIT_CHANNEL)(pPacket + 1);
			if (HasChannel(pQuitChannel->nChannelId))
				DeleteChannel(pQuitChannel->nChannelId);
		}
		break;
	case MAGIC_REJECT_CONNECT:	// student
		{
			PFINDTEACHER_REJECT_CONNECT pRejectConnect = (PFINDTEACHER_REJECT_CONNECT)(pPacket + 1);
			ReportChannelRejectEvent(pRejectConnect->dwTeacherIp, pRejectConnect->nChannelId, pRejectConnect->dwRejectCode);
		}
		break;
	}
}

void CPassiveFindTeacherTransaction::ReportChannelArriveEvent(LPCTSTR lpszName, DWORD dwIpAddress, DWORD nChannelId)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_TEACHER_CHANNEL);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_TEACHER_CHANNEL pEvent = (PEVENT_TEACHER_CHANNEL)(pHeader + 1);

	pHeader->dwEvent = EV_CHANNEL_ARRIVE;
	pHeader->dwLength = sizeof(EVENT_TEACHER_CHANNEL);

	pEvent->nChannelId = nChannelId;
	pEvent->dwIpAddress = dwIpAddress;
	_tcscpy(pEvent->szName, lpszName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveFindTeacherTransaction::ReportChannelLeaveEvent(DWORD nChannelId)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_TEACHER_CHANNEL);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_TEACHER_CHANNEL pEvent = (PEVENT_TEACHER_CHANNEL)(pHeader + 1);

	pHeader->dwEvent = EV_CHANNEL_LEAVE;
	pHeader->dwLength = sizeof(EVENT_TEACHER_CHANNEL);

	pEvent->nChannelId = nChannelId;
//	pEvent->dwIpAddress = dwIpAddress;
//	_tcscpy(pEvent->szName, lpszName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveFindTeacherTransaction::ReportChannelTimeoutEvent(DWORD nChannelId)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_TEACHER_CHANNEL);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_TEACHER_CHANNEL pEvent = (PEVENT_TEACHER_CHANNEL)(pHeader + 1);

	pHeader->dwEvent = EV_CHANNEL_TIMEOUT;
	pHeader->dwLength = sizeof(EVENT_TEACHER_CHANNEL);

	pEvent->nChannelId = nChannelId;
//	pEvent->dwIpAddress = dwIpAddress;
//	_tcscpy(pEvent->szName, lpszName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveFindTeacherTransaction::ReportChannelForceEvent(DWORD nChannelId)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_TEACHER_CHANNEL);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_TEACHER_CHANNEL pEvent = (PEVENT_TEACHER_CHANNEL)(pHeader + 1);

	pHeader->dwEvent = EV_CHANNEL_FORCE;
	pHeader->dwLength = sizeof(EVENT_TEACHER_CHANNEL);

	pEvent->nChannelId = nChannelId;
//	pEvent->dwIpAddress = dwIpAddress;
//	_tcscpy(pEvent->szName, lpszName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveFindTeacherTransaction::ReportChannelReloginEvent(DWORD nChannelId)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_TEACHER_CHANNEL);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_TEACHER_CHANNEL pEvent = (PEVENT_TEACHER_CHANNEL)(pHeader + 1);

	pHeader->dwEvent = EV_CHANNEL_RELOGIN;
	pHeader->dwLength = sizeof(EVENT_TEACHER_CHANNEL);

	pEvent->nChannelId = nChannelId;
//	pEvent->dwIpAddress = dwIpAddress;
//	_tcscpy(pEvent->szName, lpszName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveFindTeacherTransaction::ReportChannelLockChanngedEvent(DWORD nChannelId, BOOL bLock)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_TEACHER_CHANNEL);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_TEACHER_CHANNEL pEvent = (PEVENT_TEACHER_CHANNEL)(pHeader + 1);

	pHeader->dwEvent = EV_CHANNEL_LOCKCHANGED;
	pHeader->dwLength = sizeof(EVENT_TEACHER_CHANNEL);

	pEvent->nChannelId = nChannelId;
//	pEvent->dwIpAddress = dwIpAddress;
//	_tcscpy(pEvent->szName, lpszName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveFindTeacherTransaction::ReportChannelRejectEvent(DWORD dwTeacherIp, DWORD nChannelId, DWORD dwRejectCode)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_TEACHER_CHANNEL);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_TEACHER_CHANNEL pEvent = (PEVENT_TEACHER_CHANNEL)(pHeader + 1);

	pHeader->dwEvent = EV_CHANNEL_REJECT;
	pHeader->dwLength = sizeof(EVENT_TEACHER_CHANNEL);

	pEvent->nChannelId = nChannelId;
	pEvent->dwRejectCode = dwRejectCode;
//	pEvent->dwIpAddress = dwIpAddress;
//	_tcscpy(pEvent->szName, lpszName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveFindTeacherTransaction::CheckChannelLastAnnounceTime()
{
	DWORD dwDelNums = 0;
	CChannelMap::iterator pitDelete[256];

	CChannelMap::iterator iterator = m_channelMap.begin();
	while (iterator != m_channelMap.end()) {
		TEACHER_CHANNEL *pChannel = (*iterator).second;
		if (GetTickCount() - pChannel->dwLastAnnounceTime > MAX_ANNOUNCE_CHANNEL_TIMEOUT)
		{
			pitDelete[dwDelNums ++] = iterator;
			iterator ++;
		} else
			iterator++;
	}

	for (DWORD i=0; i<dwDelNums; i++) {
		iterator = pitDelete[i];

		DWORD nChannelId = (*iterator).first;
		TEACHER_CHANNEL *pChannel = (*iterator).second;
		free(pChannel);
		m_channelMap.erase(iterator);

		ReportChannelTimeoutEvent(nChannelId);
	}
}

void CPassiveFindTeacherTransaction::SendAskAccessible(DWORD dwTeacherIp)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PFINDTEACHER_ASK_ACCESS pAskAccess = (PFINDTEACHER_ASK_ACCESS)(pPacket + 1);

	pPacket->dwMagic = MAGIC_ASK_ACCESS;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(FINDTEACHER_ASK_ACCESS);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pAskAccess->dwStudentIp = m_dwLocalAddr;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPassiveFindTeacherTransaction::UpdateChannelLastAnnounce(PFINDTEACHER_ANNOUNCE_CHANNEL pAnnounceChannel)
{
	CChannelMap::iterator iterator = m_channelMap.find(pAnnounceChannel->nChannelId);
	if (iterator == m_channelMap.end())
		return;

	TEACHER_CHANNEL *pChannel = (*iterator).second;
	pChannel->dwLastAnnounceTime = GetTickCount();

	BOOL bLock = pAnnounceChannel->bLock;

	if (pChannel->bLock != bLock) {
		pChannel->bLock = bLock;
		ReportChannelLockChanngedEvent(pAnnounceChannel->nChannelId, bLock);
	}

	if (!pChannel->bAccessble) {
		SendAskAccessible(pChannel->dwIpAddress);
		return;
	}

	// If the channel is locked, and I am logged out now, 
	// I will check whether the annouce packet include me or not 
	if (pChannel->bLock) 
	{
		if (m_bLogin) 
		{
			if (pAnnounceChannel->dwIpAddress != m_dwTeacherIp)
				return;

			CLIENTID clientId;
			memcpy(&clientId, &m_idClient, sizeof(CLIENTID));

			CLIENTID* abClientId = (CLIENTID *)(pAnnounceChannel->abParams + (pAnnounceChannel->nNameLength + 1) * sizeof(TCHAR));

			for (DWORD i=0; i<pAnnounceChannel->dwClientCount; i++) 
			{
				if (CompareClientId(clientId, abClientId[i]) == 0)
				{
					// Include me, I will relogin to the channel
					ReportChannelReloginEvent(pAnnounceChannel->nChannelId);
					break;
				}
			}
		} else
		{
			CLIENTID clientId;
			memcpy(&clientId, &m_idClient, sizeof(CLIENTID));

			CLIENTID* abClientId = (CLIENTID *)(pAnnounceChannel->abParams + (pAnnounceChannel->nNameLength + 1) * sizeof(TCHAR));

			for (DWORD i=0; i<pAnnounceChannel->dwClientCount; i++) 
			{
				if (CompareClientId(clientId, abClientId[i]) == 0)
				{
					// Include me, I will join the channel 
					ReportChannelForceEvent(pAnnounceChannel->nChannelId);
					break;
				}
			}
		}
	}
}

BOOL CPassiveFindTeacherTransaction::HasChannel(DWORD nChannelId)
{
	CChannelMap::iterator iterator = m_channelMap.find(nChannelId);
	return (iterator != m_channelMap.end());
}

void CPassiveFindTeacherTransaction::InsertChannel(PFINDTEACHER_ANNOUNCE_CHANNEL pAnnounceChannel)
{
	CChannelMap::iterator iterator = m_channelMap.find(pAnnounceChannel->nChannelId);
	if (iterator == m_channelMap.end())
	{
		PTEACHER_CHANNEL pChannel = (PTEACHER_CHANNEL)malloc(sizeof(TEACHER_CHANNEL));

		pChannel->dwIpAddress = pAnnounceChannel->dwIpAddress;
		pChannel->bLock = pAnnounceChannel->bLock;
		pChannel->dwLastAnnounceTime = GetTickCount();
		pChannel->bAccessble = FALSE;
		_tcscpy(pChannel->szName, (TCHAR*)pAnnounceChannel->abParams);

		m_channelMap.insert(CChannelMap::value_type(pAnnounceChannel->nChannelId, pChannel));
	}
}

void CPassiveFindTeacherTransaction::DeleteChannel(DWORD nChannelId)
{
	CChannelMap::iterator iterator = m_channelMap.find(nChannelId);
	if (iterator != m_channelMap.end())
	{
		TEACHER_CHANNEL *pChannel = (*iterator).second;
		free(pChannel);
		m_channelMap.erase(iterator);

		ReportChannelLeaveEvent(nChannelId);
	}
}

DWORD CPassiveFindTeacherTransaction::GetChannelTeacherIp(DWORD nChannelId)
{
	CChannelMap::iterator iterator = m_channelMap.find(nChannelId);
	if (iterator == m_channelMap.end())
		return 0;

	TEACHER_CHANNEL *pChannel = (*iterator).second;
	return pChannel->dwIpAddress;
}

int CPassiveFindTeacherTransaction::CompareClientId(CLIENTID srcClientId, CLIENTID dstClientId)
{
	return memcmp(srcClientId.abMac, dstClientId.abMac, 6);
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

CNameTransaction::CNameTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_bStarted = FALSE;

	InitializeCriticalSection(&m_cs);
}

CNameTransaction::~CNameTransaction()
{
	DeleteCriticalSection(&m_cs);
}

BOOL CNameTransaction::Startup()
{
	if (m_bStarted)
		return TRUE;

	m_pComLayer->RegisterTimeout(m_pTransID, 1000, 0);

	m_bStarted = TRUE;

	return TRUE;
}

void CNameTransaction::Shutdown()
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	m_pComLayer->UnregisterTimeout(m_pTransID, 0);

	CNameMap::iterator iterator = m_nameMap.begin();
	while (iterator != m_nameMap.end()) {
		PNAME_CONTEXT pTemp = (*iterator).second;
		free(pTemp);
		m_nameMap.erase(iterator);
		
		iterator = m_nameMap.begin();
	}

	m_bStarted = FALSE;
}

void CNameTransaction::AddClient(CLIENTID clientId, TCHAR* pszName)
{
	CAutoSectionLock lock(&m_cs);

	CNameMap::iterator iterator = m_nameMap.find(clientId);
	if (iterator != m_nameMap.end())
		return;

	PNAME_CONTEXT pTemp = (PNAME_CONTEXT)malloc(sizeof(NAME_CONTEXT));
	memset(pTemp, 0, sizeof(NAME_CONTEXT));

	_tcscpy(pTemp->szName, pszName);
	m_nameMap.insert(CNameMap::value_type(clientId, pTemp));
}

void CNameTransaction::RemoveClient(CLIENTID clientId)
{
	CAutoSectionLock lock(&m_cs);

	CNameMap::iterator iterator = m_nameMap.find(clientId);
	if (iterator == m_nameMap.end())
		return;

	PNAME_CONTEXT pTemp = (*iterator).second;
	free(pTemp);
	m_nameMap.erase(iterator);
}

void CNameTransaction::SetClientLogin(CLIENTID clientId, DWORD dwClientIp)
{
	CAutoSectionLock lock(&m_cs);

	CNameMap::iterator iterator = m_nameMap.find(clientId);
	if (iterator == m_nameMap.end())
		return;

	PNAME_CONTEXT pTemp = (*iterator).second;
	pTemp->dwClientIp = dwClientIp;
}

void CNameTransaction::SetClientLogout(CLIENTID clientId)
{
	CAutoSectionLock lock(&m_cs);

	CNameMap::iterator iterator = m_nameMap.find(clientId);
	if (iterator == m_nameMap.end())
		return;

	PNAME_CONTEXT pTemp = (*iterator).second;

	pTemp->dwClientIp = 0;

	pTemp->m_nReturnCount = 0;
	pTemp->m_nChangeCount = 0;
	pTemp->m_nReplyCount = 0;
}

void CNameTransaction::SetClientName(CLIENTID clientId, TCHAR* pszName)
{
	CAutoSectionLock lock(&m_cs);

	CNameMap::iterator iterator = m_nameMap.find(clientId);
	if (iterator == m_nameMap.end())
		return;

	PNAME_CONTEXT pTemp = (*iterator).second;
	_tcscpy(pTemp->szName, pszName);
}

void CNameTransaction::NotifyMemberNameChanged(CLIENTID clientId, TCHAR* pszNewName)
{
	CAutoSectionLock lock(&m_cs);

	CNameMap::iterator iterator = m_nameMap.find(clientId);
	if (iterator == m_nameMap.end())
		return;

	PNAME_CONTEXT pContext = (*iterator).second;
	if (_tcscmp(pContext->szName, pszNewName) == 0)
		return;

	_tcscpy(pContext->szName, pszNewName);
	pContext->m_nChangeCount ++;

	SendNameChangePacket(pContext->dwClientIp, pContext->m_nChangeCount, pszNewName);
}

void CNameTransaction::NotifyMemberNameConflicted(CLIENTID clientId, TCHAR* pszOldName)
{
	CAutoSectionLock lock(&m_cs);

	CNameMap::iterator iterator = m_nameMap.find(clientId);
	if (iterator == m_nameMap.end())
		return;

	PNAME_CONTEXT pContext = (*iterator).second;
	_tcscpy(pContext->szName, pszOldName);

	pContext->m_nChangeCount ++;
	SendNameChangePacket(pContext->dwClientIp, pContext->m_nChangeCount, pContext->szName);
}

void CNameTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == 0)
	{
		CNameMap::iterator iterator = m_nameMap.begin();
		while (iterator != m_nameMap.end()) {
			PNAME_CONTEXT pContext = (*iterator).second;

			if (pContext->m_nChangeCount > pContext->m_nReplyCount) {
				SendNameChangePacket(pContext->dwClientIp, pContext->m_nChangeCount, pContext->szName);
			}
			
			iterator ++;
		}

		m_pComLayer->RegisterTimeout(m_pTransID, 1000, 0);
	}
}

void CNameTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_NAME_CHANGE_RET:
		{
			PPACKET_NAME_CHANGE_RET pNameChangeRet = (PPACKET_NAME_CHANGE_RET)(pPacket + 1);

			CNameMap::iterator iterator = m_nameMap.find(pNameChangeRet->id);
			if (iterator != m_nameMap.end())
			{
				PNAME_CONTEXT pContext = (*iterator).second;
				pContext->m_nReplyCount = pNameChangeRet->nRefCound;
			}
		}
		break;
	case MAGIC_RENAME_REQUEST:
		{
			PPACKET_RENAME_REQUEST pRenameRequest = (PPACKET_RENAME_REQUEST)(pPacket + 1);

			CNameMap::iterator iterator = m_nameMap.find(pRenameRequest->id);
			if (iterator != m_nameMap.end())
			{
				PNAME_CONTEXT pContext = (*iterator).second;

				if (pContext->m_nReturnCount < pRenameRequest->nRefCount) 
				{
					// send request ret first time
					pContext->m_nReturnCount = pRenameRequest->nRefCount;

					if (_tcscmp(pContext->szName, pRenameRequest->szNewName) == 0)
					{
						SendRenameRequestRetPacket(pContext->dwClientIp, TRUE, pContext->m_nReturnCount, pContext->szName);
					} else
					{
						// loop check if there is duplicated name
						// if not, use use new name, report rename event and send appcept packet
						// if have, use old name, send deny packet
						if (HasSameName(pRenameRequest->szNewName))
						{
							SendRenameRequestRetPacket(pContext->dwClientIp, FALSE, pContext->m_nReturnCount, pContext->szName);
						} else
						{
							_tcscpy(pContext->szName, pRenameRequest->szNewName);

							SendRenameRequestRetPacket(pContext->dwClientIp, TRUE, pContext->m_nReturnCount, pContext->szName);

							ReportNameChangedEvent((*iterator).first, pContext->szName);
						}
					}

				} else
				{
					// send request ret again if client request again
					// It happens the client missed previous ret packet
					if (_tcscmp(pContext->szName, pRenameRequest->szNewName) == 0)
					{
						SendRenameRequestRetPacket(pContext->dwClientIp, TRUE, pContext->m_nReturnCount, pContext->szName);
					} else
					{
						SendRenameRequestRetPacket(pContext->dwClientIp, FALSE, pContext->m_nReturnCount, pContext->szName);
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

void CNameTransaction::SendNameChangePacket(DWORD dwClientIp, int nRefCount, TCHAR* pszNewName)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwClientIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_NAME_CHANGE pNameChange = (PPACKET_NAME_CHANGE)(pPacket + 1);

	pPacket->dwMagic = MAGIC_NAME_CHANGE;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_NAME_CHANGE);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pNameChange->nRefCount = nRefCount;
	_tcscpy(pNameChange->szNewName, pszNewName);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CNameTransaction::SendRenameRequestRetPacket(DWORD dwClientIp, BOOL bAccpted, int nRefCount, TCHAR* pszOldName)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwClientIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_RENAME_REQUEST_RET pRenameRequestRet = (PPACKET_RENAME_REQUEST_RET)(pPacket + 1);

	pPacket->dwMagic = MAGIC_RENAME_REQUEST_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_RENAME_REQUEST_RET);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pRenameRequestRet->bAccpted = bAccpted;
	pRenameRequestRet->nRefCount = nRefCount;
	_tcscpy(pRenameRequestRet->szOldName, pszOldName);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CNameTransaction::ReportNameChangedEvent(CLIENTID id, TCHAR* pszNewName)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_NAME_CHANGED_BY_STUDENT);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem) {
		pEventMgr->Release();
		return;
	}

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_NAME_CHANGED_BY_STUDENT pEvent = (PEVENT_NAME_CHANGED_BY_STUDENT)(pHeader + 1);

	pHeader->dwEvent = EV_NAME_CHANGED_BY_STUDENT;
	pHeader->dwLength = sizeof(EVENT_NAME_CHANGED_BY_STUDENT);

	memcpy(pEvent->abMacAddress, id.abMac, 6);
	_tcscpy(pEvent->szNewName, pszNewName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

BOOL CNameTransaction::HasSameName(TCHAR* pszName)
{
	CNameMap::iterator iterator = m_nameMap.begin();
	while (iterator != m_nameMap.end()) {

		PNAME_CONTEXT pContext = (*iterator).second;
		if (_tcsicmp(pContext->szName, pszName) == 0)
			return TRUE;

		iterator ++;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

CPassiveNameTransaction::CPassiveNameTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);

	m_bStarted = FALSE;

	m_nReturnCount = 0;
	m_nChangeCount = 0;
	m_nReplyCount = 0;
	memset(m_szName, 0, 32);

	m_dwTeacherIp = 0;
}

CPassiveNameTransaction::~CPassiveNameTransaction()
{
	DeleteCriticalSection(&m_cs);
}

BOOL CPassiveNameTransaction::Startup(DWORD dwTeacherIp, PCLIENTID pClientId, TCHAR* pszName)
{
	if (m_bStarted)
		return TRUE;

	m_dwTeacherIp = dwTeacherIp;
	memcpy(&m_id, pClientId, sizeof(CLIENTID));

	m_nReturnCount = 0;
	m_nChangeCount = 0;
	m_nReplyCount = 0;
	_tcscpy(m_szName, pszName);

	m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);

	m_bStarted = TRUE;

	return TRUE;
}

void CPassiveNameTransaction::Shutdown()
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	m_nReturnCount = 0;
	m_nChangeCount = 0;
	m_nReplyCount = 0;

	m_pComLayer->UnregisterTimeout(m_pTransID, 0);

	m_bStarted = FALSE;
}

void CPassiveNameTransaction::SetName(TCHAR* pszName)
{
	CAutoSectionLock lock(&m_cs);

	_tcscpy(m_szName, pszName);
}

void CPassiveNameTransaction::NotifyTeacherNameChanged(TCHAR* pszNewName)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (_tcscmp(m_szName, pszNewName) == 0)
		return;

	m_nChangeCount ++;
	_tcscpy(m_szName, pszNewName);

	SendNameChangePacket(m_dwTeacherIp, m_nChangeCount, pszNewName);
}

void CPassiveNameTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (dwTimeID == 0)
	{
		if (m_nChangeCount > m_nReplyCount)
		{
			SendNameChangePacket(m_dwTeacherIp, m_nChangeCount, m_szName);
		}

		m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);
	}
}

void CPassiveNameTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_NAME_CHANGE:
		{
			PPACKET_NAME_CHANGE pNameChange = (PPACKET_NAME_CHANGE)(pPacket + 1);

			if (pNameChange->nRefCount > m_nReturnCount) {
				m_nReturnCount = pNameChange->nRefCount;
				_tcscpy(m_szName, pNameChange->szNewName);

				ReportNameChangedEvent(m_szName);
			}

			SendNameChangeRetPacket(m_nReturnCount);
		}
		break;
	case MAGIC_RENAME_REQUEST_RET:
		{
			PPACKET_RENAME_REQUEST_RET pRenameRequestRet = (PPACKET_RENAME_REQUEST_RET)(pPacket + 1);

			if (m_nReplyCount >= pRenameRequestRet->nRefCount)
				break;

			m_nReplyCount = pRenameRequestRet->nRefCount;

			ReportRenameRequestRetEvent(pRenameRequestRet->bAccpted, pRenameRequestRet->szOldName);
		}
		break;
	default:
		break;
	}
}

void CPassiveNameTransaction::SendNameChangePacket(DWORD dwTeacherIp, int nRefCount, TCHAR* pszNewName)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_RENAME_REQUEST pRenameRequest = (PPACKET_RENAME_REQUEST)(pPacket + 1);

	pPacket->dwMagic = MAGIC_RENAME_REQUEST;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_RENAME_REQUEST);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	memcpy(&pRenameRequest->id, &m_id, sizeof(CLIENTID));
	pRenameRequest->nRefCount = nRefCount;
	_tcscpy(pRenameRequest->szNewName, pszNewName);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPassiveNameTransaction::SendNameChangeRetPacket(int nRefCount)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_NAME_CHANGE_RET pNameChangeRet = (PPACKET_NAME_CHANGE_RET)(pPacket + 1);

	pPacket->dwMagic = MAGIC_NAME_CHANGE_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_NAME_CHANGE_RET);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	memcpy(&pNameChangeRet->id, &m_id, sizeof(CLIENTID));
	pNameChangeRet->nRefCound = nRefCount;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPassiveNameTransaction::ReportNameChangedEvent(TCHAR* pszNewName)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_NAME_CHANGED_BY_TEACHER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem) {
		pEventMgr->Release();
		return;
	}

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_NAME_CHANGED_BY_TEACHER pEvent = (PEVENT_NAME_CHANGED_BY_TEACHER)(pHeader + 1);

	pHeader->dwEvent = EV_NAME_CHANGED_BY_TEACHER;
	pHeader->dwLength = sizeof(EVENT_NAME_CHANGED_BY_TEACHER);

	_tcscpy(pEvent->szNewName, pszNewName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveNameTransaction::ReportRenameRequestRetEvent(BOOL bAccepted, TCHAR* pszOldName)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_NAME_REQUEST_RET);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem) {
		pEventMgr->Release();
		return;
	}

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_NAME_REQUEST_RET pEvent = (PEVENT_NAME_REQUEST_RET)(pHeader + 1);

	pHeader->dwEvent = EV_NAME_REQUEST_RET;
	pHeader->dwLength = sizeof(EVENT_NAME_REQUEST_RET);

	pEvent->bAccepted = bAccepted;
	_tcscpy(pEvent->szResetName, pszOldName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CTeacherExitTransacation::CTeacherExitTransacation(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_bStarted = FALSE;
	m_bFinish = FALSE;

	InitializeCriticalSection(&m_cs);
}

CTeacherExitTransacation::~CTeacherExitTransacation()
{

	DeleteCriticalSection(&m_cs);
}

BOOL CTeacherExitTransacation::Startup(DWORD dwTeacherIp, PCLIENTID pClientId, int nCount, DWORD dwTimeout)
{
	if (m_bStarted)
		return TRUE;

	m_dwTeacherIp = dwTeacherIp;
	m_listClientId.clear();

	for (int i=0; i<nCount; i++) {
		m_listClientId.push_back(pClientId[i]);
	}

	SendExitPacket();

	m_pComLayer->RegisterTimeout(m_pTransID, 200, 0);
	m_pComLayer->RegisterTimeout(m_pTransID, dwTimeout, 1);

	m_bStarted = TRUE;

	return TRUE;
}

void CTeacherExitTransacation::Shutdown()
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (!m_bFinish) {
		m_pComLayer->UnregisterTimeout(m_pTransID, 0);
		m_pComLayer->UnregisterTimeout(m_pTransID, 1);
	}

	m_bStarted = FALSE;
}

void CTeacherExitTransacation::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (m_bFinish)
		return;

	if (dwTimeID == 0) {
		SendExitPacket();
		m_pComLayer->RegisterTimeout(m_pTransID, 200, 0);
	}

	if (dwTimeID == 1) {
		ReportExitTimoutEvent();

		m_pComLayer->UnregisterTimeout(m_pTransID, 0);

		m_bFinish = TRUE;
	}
}

void CTeacherExitTransacation::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	if (pPacket->dwMagic != MAGIC_TEACHER_EXIT_RET)
		return;

	PPACKET_TEACHER_EXIT_RET pExitReturn = (PPACKET_TEACHER_EXIT_RET)(pPacket + 1);
	ProcessExitReturn(pExitReturn);
}

void CTeacherExitTransacation::SendExitPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (!pItem)
		return;

	CLIENTID id[256];

	int nCount = 0;
	CClientIdList::iterator it = m_listClientId.begin();
	while (it != m_listClientId.end()) {
		memcpy(&id[nCount], &(*it), sizeof(CLIENTID));

		it ++;
		nCount ++;
	}

	if (nCount <= 0)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_pComLayer->GetComAddress(), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_TEACHER_EXIT pPacketExit = (PPACKET_TEACHER_EXIT)(pPacket + 1);

	pPacket->dwMagic = MAGIC_TEACHER_EXIT;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_TEACHER_EXIT) + nCount * sizeof(CLIENTID);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pPacketExit->dwTeacherIp = m_pComLayer->GetLocalAddress();
	pPacketExit->nCount = nCount;
	memcpy(pPacketExit->id, id, nCount * sizeof(CLIENTID));

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);

	m_pComLayer->AddToSendBuffer(pItem);
}

void CTeacherExitTransacation::ProcessExitReturn(PPACKET_TEACHER_EXIT_RET pPacketRet)
{
	CClientIdList::iterator it = m_listClientId.begin();
	while (it != m_listClientId.end()) {
		if (memcmp(&pPacketRet->id, &(*it), sizeof(CLIENTID)) == 0) {
			m_listClientId.erase(it);
			break;
		}

		it ++;
	}

	if (m_listClientId.size() == 0) {

		ReportExitFinishedEvent();

		m_pComLayer->UnregisterTimeout(m_pTransID, 0);
		m_pComLayer->UnregisterTimeout(m_pTransID, 1);

		m_bFinish = TRUE;
	}
}

void CTeacherExitTransacation::ReportExitTimoutEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_TEACHER_EXIT_TIMEOUT;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CTeacherExitTransacation::ReportExitFinishedEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_TEACHER_EXIT_FINISHED;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CCommandTransaction::CCommandTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);

	memset(m_abCommand, 0, sizeof(m_abCommand));
	m_cbCommand = 0;
}

CCommandTransaction::~CCommandTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CCommandTransaction::StartSendCommand(DWORD dwTeacherIp, DWORD* pdwIpAddress, int nCount, LPBYTE lpCmdBuffer, DWORD cbSize)
{
	CAutoSectionLock lock(&m_cs);

	m_dwTeacherIp = dwTeacherIp;

	for (int i=0; i<nCount; i++) {
		m_listReceivers.push_back(pdwIpAddress[i]);
	}

	m_cbCommand = cbSize;
	memcpy(m_abCommand, lpCmdBuffer, m_cbCommand);

	CReceiveList::iterator it = m_listReceivers.begin();
	while (it != m_listReceivers.end()) {
		SendCommand(*it);
		it ++;
	}

	m_pComLayer->RegisterTimeout(m_pTransID, 1000, 0);
	m_pComLayer->RegisterTimeout(m_pTransID, 15000, 1);
}

void CCommandTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	// Send command again
	if (dwTimeID == 0)
	{
		CReceiveList::iterator it = m_listReceivers.begin();
		while (it != m_listReceivers.end()) {
			SendCommand(*it);
			it ++;
		}

		m_pComLayer->RegisterTimeout(m_pTransID, 1000, 0);
	}

	// Command timeout
	if (dwTimeID == 1)
	{
		m_pComLayer->UnregisterTimeout(m_pTransID, 0);
		Destroy();
	}
}

void CCommandTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	if (pPacket->dwMagic != MAGIC_COMMAND_RET)
		return;

	if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_COMMAND_RET))
		return;

	PPACKET_COMMAND_RET pPacketRet = (PPACKET_COMMAND_RET)(pPacket + 1);

	CReceiveList::iterator it = m_listReceivers.begin();
	while (it != m_listReceivers.end()) {
		if (*it == pPacketRet->dwIpAddress) {
			m_listReceivers.erase(it);
			break;
		}

		it ++;
	}

	// All members have received the command, 
	// destroy the trasaction right now.
	if (m_listReceivers.size() == 0) {
		m_pComLayer->UnregisterTimeout(m_pTransID, 0);
		m_pComLayer->UnregisterTimeout(m_pTransID, 1);

		Destroy();
	}
}

void CCommandTransaction::SendCommand(DWORD dwClientIp)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwClientIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_COMMAND pPacketCommand = (PPACKET_COMMAND)(pPacket + 1);

	pPacket->dwMagic = MAGIC_COMMAND;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_COMMAND) + m_cbCommand;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pPacketCommand->dwTimeout = 20000;
	pPacketCommand->dwIpAddress = m_dwTeacherIp;
	pPacketCommand->cbCmdSize = m_cbCommand;
	memcpy(pPacketCommand->abCmdBuffer, m_abCommand, m_cbCommand);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CCommandTransaction::Destroy()
{
	ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
	pTransactionMgr->DeleteTransaction(m_pTransID);
	pTransactionMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CPassiveCommandTransaction::CPassiveCommandTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwTeacherIp = 0;
	InitializeCriticalSection(&m_cs);
}

CPassiveCommandTransaction::~CPassiveCommandTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CPassiveCommandTransaction::ProcesssCommand(PPACKET_COMMAND pPacketCommand)
{
	CAutoSectionLock lock(&m_cs);

	m_dwTeacherIp = pPacketCommand->dwIpAddress;

	// send command ret
	SendCommandRet(m_dwTeacherIp);

	// report command arrive event
	ReportCommandArrivedEvent(pPacketCommand->abCmdBuffer, pPacketCommand->cbCmdSize);

	// Set timeout
	m_pComLayer->RegisterTimeout(m_pTransID, pPacketCommand->dwTimeout, 0);
}

void CPassiveCommandTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == 0)
	{
		// Destroy transaction
		ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
		pTransactionMgr->DeleteTransaction(m_pTransID);
		pTransactionMgr->Release();
	}
}

void CPassiveCommandTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;

	if (pPacket->dwMagic != MAGIC_COMMAND)
		return;

	SendCommandRet(m_dwTeacherIp);
}

void CPassiveCommandTransaction::SendCommandRet(DWORD dwIpAddress)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_COMMAND_RET pPacketRet = (PPACKET_COMMAND_RET)(pPacket + 1);

	pPacket->dwMagic = MAGIC_COMMAND_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_COMMAND_RET);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pPacketRet->dwIpAddress = m_pComLayer->GetLocalAddress();

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPassiveCommandTransaction::ReportCommandArrivedEvent(BYTE* pabCommand, DWORD cbCommand)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_COMMAND_ARRIVED) + cbCommand;
	cbEvent += cbCommand;

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_COMMAND_ARRIVED;
	pHeader->dwLength = sizeof(EVENT_COMMAND_ARRIVED) + cbCommand;

	PEVENT_COMMAND_ARRIVED pEvent = (PEVENT_COMMAND_ARRIVED)(pHeader + 1);
	pEvent->cbCmdSize = cbCommand;
	memcpy(pEvent->abCmdBuffer, pabCommand, cbCommand);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

#define COMMAND_INTERVAL	500
#define COMMAND_TIMEOUT		60000

CQuizCommandTransaction::CQuizCommandTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	memset(m_abCommand, 0, sizeof(m_abCommand));
	m_cbCommand = 0;
	
	memset(m_abMac, 0, 6);

	InitializeCriticalSection(&m_cs);
}

CQuizCommandTransaction::~CQuizCommandTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CQuizCommandTransaction::StartSendCommand(DWORD dwSenderIp, BYTE* pbMac, DWORD* pdwIpAddress, int nCount, LPBYTE lpCmdBuffer, DWORD cbSize)
{
	CAutoSectionLock lock(&m_cs);
	
	m_dwSenderIp = dwSenderIp;
	
	if (pbMac) {
		memcpy(m_abMac, pbMac, 6);
	}

	for (int i=0; i<nCount; i++) {
		m_listReceivers.push_back(pdwIpAddress[i]);
	}

	m_cbCommand = cbSize;
	memcpy(m_abCommand, lpCmdBuffer, m_cbCommand);

	CReceiveList::iterator it = m_listReceivers.begin();
	while (it != m_listReceivers.end()) {
		SendCommand(*it);
		it ++;
	}

	m_pComLayer->RegisterTimeout(m_pTransID, COMMAND_INTERVAL, 0);
	m_pComLayer->RegisterTimeout(m_pTransID, COMMAND_TIMEOUT, 1);
}

void CQuizCommandTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	// Send command again
	if (dwTimeID == 0)
	{
		CReceiveList::iterator it = m_listReceivers.begin();
		while (it != m_listReceivers.end()) {
			SendCommand(*it);
			it ++;
		}

		m_pComLayer->RegisterTimeout(m_pTransID, COMMAND_INTERVAL, 0);
	}

	// Command timeout
	if (dwTimeID == 1)
	{
		CReceiveList::iterator it = m_listReceivers.begin();
		while (it != m_listReceivers.end()) {
			ReportFailedEvent(*it);
			it ++;
		}
		
		m_pComLayer->UnregisterTimeout(m_pTransID, 0);
		Destroy();
	}
}

void CQuizCommandTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	if (pPacket->dwMagic != MAGIC_QUIZ_COMMAND_RET)
		return;

	if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_QUIZ_COMMAND_RET))
		return;

	PPACKET_QUIZ_COMMAND_RET pPacketRet = (PPACKET_QUIZ_COMMAND_RET)(pPacket + 1);

	CReceiveList::iterator it = m_listReceivers.begin();
	while (it != m_listReceivers.end()) {
		if (*it == pPacketRet->dwIpAddress) {
			ReportSuccessEvent(*it);
			m_listReceivers.erase(it);
			break;
		}

		it ++;
	}

	// All members have received the command, 
	// destroy the trasaction right now.
	if (m_listReceivers.size() == 0) {
		m_pComLayer->UnregisterTimeout(m_pTransID, 0);
		m_pComLayer->UnregisterTimeout(m_pTransID, 1);

		Destroy();
	}
}

void CQuizCommandTransaction::SendCommand(DWORD dwClientIp)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwClientIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_QUIZ_COMMAND pPacketCommand = (PPACKET_QUIZ_COMMAND)(pPacket + 1);

	pPacket->dwMagic = MAGIC_QUIZ_COMMAND;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_COMMAND) + m_cbCommand;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pPacketCommand->dwTimeout = COMMAND_TIMEOUT + 20000;
	pPacketCommand->dwSenderIp = m_dwSenderIp;
	pPacketCommand->dwReceiverIp = dwClientIp;
	memcpy(pPacketCommand->abMac, m_abMac, 6);
	pPacketCommand->cbCmdSize = m_cbCommand;
	memcpy(pPacketCommand->abCmdBuffer, m_abCommand, m_cbCommand);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CQuizCommandTransaction::ReportSuccessEvent(DWORD dwClientIp)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();
	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_QUIZ_COMMAND_SUCCESS);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZ_COMMAND_SUCCESS;
	pHeader->dwLength = sizeof(EVENT_QUIZ_COMMAND_SUCCESS);

	PEVENT_QUIZ_COMMAND_SUCCESS pEvent = (PEVENT_QUIZ_COMMAND_SUCCESS)(pHeader + 1);
	pEvent->dwIpAddress = dwClientIp;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CQuizCommandTransaction::ReportFailedEvent(DWORD dwClientIp)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();
	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_QUIZ_COMMAND_FAILED);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZ_COMMAND_FAILED;
	pHeader->dwLength = sizeof(EVENT_QUIZ_COMMAND_FAILED);

	PEVENT_QUIZ_COMMAND_FAILED pEvent = (PEVENT_QUIZ_COMMAND_FAILED)(pHeader + 1);
	pEvent->dwIpAddress = dwClientIp;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CQuizCommandTransaction::Destroy()
{
	ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
	pTransactionMgr->DeleteTransaction(m_pTransID);
	pTransactionMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CQuizPassiveCommandTransaction::CQuizPassiveCommandTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwSenderIp = 0;
	InitializeCriticalSection(&m_cs);
}

CQuizPassiveCommandTransaction::~CQuizPassiveCommandTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CQuizPassiveCommandTransaction::ProcessCommand(PPACKET_QUIZ_COMMAND pPacketCommand)
{
	CAutoSectionLock lock(&m_cs);
	
	m_dwSenderIp = pPacketCommand->dwSenderIp;

	// send command ret
	SendCommandRet(m_dwSenderIp);

	// report command arrive event
	ReportCommandArrivedEvent(pPacketCommand);

	// Set timeout
	m_pComLayer->RegisterTimeout(m_pTransID, pPacketCommand->dwTimeout, 0);
}

void CQuizPassiveCommandTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == 0)
	{
		// Destroy transaction
		ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
		pTransactionMgr->DeleteTransaction(m_pTransID);
		pTransactionMgr->Release();
	}
}

void CQuizPassiveCommandTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;

	if (pPacket->dwMagic != MAGIC_QUIZ_COMMAND)
		return;

	SendCommandRet(m_dwSenderIp);
}

void CQuizPassiveCommandTransaction::SendCommandRet(DWORD dwIpAddress)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_QUIZ_COMMAND_RET pPacketRet = (PPACKET_QUIZ_COMMAND_RET)(pPacket + 1);

	pPacket->dwMagic = MAGIC_QUIZ_COMMAND_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_COMMAND_RET);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pPacketRet->dwIpAddress = m_pComLayer->GetLocalAddress();

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CQuizPassiveCommandTransaction::ReportCommandArrivedEvent(PPACKET_QUIZ_COMMAND pPacketCommand)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbCommand = pPacketCommand->cbCmdSize;
	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_QUIZ_COMMAND_ARRIVED) + cbCommand;

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZ_COMMAND_ARRIVED;
	pHeader->dwLength = sizeof(EVENT_QUIZ_COMMAND_ARRIVED) + cbCommand;

	PEVENT_QUIZ_COMMAND_ARRIVED pEvent = (PEVENT_QUIZ_COMMAND_ARRIVED)(pHeader + 1);
	pEvent->dwIpAddress = pPacketCommand->dwSenderIp;
	memcpy(pEvent->abMac, pPacketCommand->abMac, 6);
	pEvent->cbCmdSize = cbCommand;
	memcpy(pEvent->abCmdBuffer, pPacketCommand->abCmdBuffer, cbCommand);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CLiteCommandTransaction::CLiteCommandTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
}

CLiteCommandTransaction::~CLiteCommandTransaction()
{
}

void CLiteCommandTransaction::SendLiteCommand(DWORD* pdwIpAddress, int nCount, LPBYTE lpCmdBuffer, DWORD cbSize)
{
	for (int i=0; i<nCount; i++) 
	{
		IBufferItem *pItem = m_pComLayer->AllocBufferItem();
		if (NULL == pItem)
			return;

		pItem->SetTransactionID(m_pTransID);
		pItem->SetDstIpAddress(CIpAddress(pdwIpAddress[i], m_pComLayer->GetComPort()));

		PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
		PPACKET_LITE_COMMAND pPacketLiteCommand = (PPACKET_LITE_COMMAND)(pPacket + 1);

		pPacket->dwMagic = MAGIC_LITE_COMMAND;
		pPacket->dwVersion = LANGLAB_VERSION;
		pPacket->dwLength = sizeof(PACKET_LITE_COMMAND) + cbSize;
		memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

		pPacketLiteCommand->cbCmdSize = cbSize;
		memcpy(pPacketLiteCommand->abCmdBuffer, lpCmdBuffer, cbSize);

		pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
		m_pComLayer->AddToSendBuffer(pItem);
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CFileCollectTransaction::CFileCollectTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);
	m_bCollecting = FALSE;
	m_dwLocalInterface = 0;
	m_dwTaskId = 0;

	m_bCollectAll = FALSE;
}

CFileCollectTransaction::~CFileCollectTransaction()
{
	DeleteAllMembers();

	DeleteCriticalSection(&m_cs);
}

void CFileCollectTransaction::SetCollectMission(PCOLLECTMISSION pCollectMission)
{
	CAutoSectionLock lock(&m_cs);

	memcpy(&m_CollectMission, pCollectMission, sizeof(COLLECTMISSION));
}

void CFileCollectTransaction::SetCollectMembers(int nCount, PMEMBERINFO* ppMemberInfo)
{
	CAutoSectionLock lock(&m_cs);

	DeleteAllMembers();

	for (int i=0; i<nCount; i++) {
		PCOLLECTCONTEXT pContext = (PCOLLECTCONTEXT)malloc(sizeof(COLLECTCONTEXT));
		if (pContext) {
			ResetContext(pContext);
			m_collectMap.insert(CCollectMap::value_type(ppMemberInfo[i], pContext));
		}
	}
}

void CFileCollectTransaction::Start(DWORD dwLocalInterface, BOOL bCollectAll)
{
	CAutoSectionLock lock(&m_cs);

	if (m_bCollecting)
		return;

	m_dwLocalInterface = dwLocalInterface;
	m_bCollectAll = bCollectAll;
	m_dwTaskId = GetTickCount();

	// Send Collect Command to All Members
	CCollectMap::iterator it = m_collectMap.begin();
	while (it != m_collectMap.end()) {
		PMEMBERINFO pMemberInfo = (*it).first;
		PCOLLECTCONTEXT pContext = (*it).second;

		if (pMemberInfo && pMemberInfo->bLoggedIn) {
			SendCollectCommand(pMemberInfo);
		}

		it ++;
	}

	// Set timeout
	m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);

	m_bCollecting = TRUE;
}

void CFileCollectTransaction::Stop()
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bCollecting)
		return;

	// Send Stop Command to Member
	CCollectMap::iterator it = m_collectMap.begin();
	while (it != m_collectMap.end()) {
		PMEMBERINFO pMemberInfo = (*it).first;
		PCOLLECTCONTEXT pContext = (*it).second;

		if (pMemberInfo && pMemberInfo->bLoggedIn) {
			if (pContext->dwState != COLLECT_FINISHED) {
				SendStopCommand(pMemberInfo);

				if (pContext->dwState != COLLECT_NONE) {

					PACKET_COLLECT_STATE state;
					state.pMemberInfo = pMemberInfo;
					state.dwTaskId = m_dwTaskId;
					state.dwState = COLLECT_FINISHED;
					state.dwFileCount = pContext->dwFileCount;
					state.llFileSize = pContext->llFileSize;
					state.dwErrorCode = CE_NONE;

					ReportStateChangedEvent(&state);
				}
			}
		}

		ResetContext(pContext);

		it ++;
	}

	m_pComLayer->UnregisterTimeout(m_pTransID, 0);

	m_bCollecting = FALSE;
}

BOOL CFileCollectTransaction::OnMemberConnected(PMEMBERINFO pMemberInfo)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bCollecting)
		return FALSE;

	if (!m_bCollectAll) {
		return FALSE;
	}

	CCollectMap::iterator it = m_collectMap.find(pMemberInfo);
	if (it != m_collectMap.end()) {
		return FALSE;
	}

	PCOLLECTCONTEXT pContext = (PCOLLECTCONTEXT)malloc(sizeof(COLLECTCONTEXT));
	if (pContext) {
		ResetContext(pContext);
		m_collectMap.insert(CCollectMap::value_type(pMemberInfo, pContext));
	}

	return TRUE;
}

BOOL CFileCollectTransaction::OnMemberDisconnected(PMEMBERINFO pMemberInfo)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bCollecting)
		return FALSE;

	CCollectMap::iterator it = m_collectMap.find(pMemberInfo);
	if (it == m_collectMap.end())
		return FALSE;

	PCOLLECTCONTEXT pContext = (*it).second;
	free(pContext);

	m_collectMap.erase(it);
	
	if (IsCollectFinished()) {
		ReportFinishedEvent();
	}

	return TRUE;
}

void CFileCollectTransaction::StopMember(PMEMBERINFO pMemberInfo)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bCollecting)
		return;

	CCollectMap::iterator it = m_collectMap.find(pMemberInfo);
	if (it == m_collectMap.end())
		return;

	PCOLLECTCONTEXT pContext = (*it).second;
	if ((pContext->dwState == COLLECT_NONE) || (pContext->dwState == COLLECT_FINISHED))
		return;

	SendStopCommand(pMemberInfo);

	pContext->dwState = COLLECT_FINISHED;
	pContext->dwErrorCode = CE_NONE;

	// Report state change event
	PACKET_COLLECT_STATE state;
	state.pMemberInfo = pMemberInfo;
	state.dwTaskId = m_dwTaskId;
	state.dwState = COLLECT_FINISHED;
	state.dwFileCount = pContext->dwFileCount;
	state.llFileSize = pContext->llFileSize;
	state.dwErrorCode = CE_NONE;

	ReportStateChangedEvent(&state);

	if (IsCollectFinished()) {
		ReportFinishedEvent();
	}
}

void CFileCollectTransaction::StartMember(int nCount, PMEMBERINFO* ppMemberInfo)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bCollecting)
		return;

	PCOLLECTCONTEXT pContext = NULL;

	for (int i=0; i<nCount; i++) {
		CCollectMap::iterator it = m_collectMap.find(ppMemberInfo[i]);
		if (it == m_collectMap.end()) {
			pContext = (PCOLLECTCONTEXT)malloc(sizeof(COLLECTCONTEXT));
			if (!pContext)
				continue;

			ResetContext(pContext);
			m_collectMap.insert(CCollectMap::value_type(ppMemberInfo[i], pContext));
		} else
		{
			pContext = (*it).second;
			if ((COLLECT_NONE != pContext->dwState) && (COLLECT_FINISHED != pContext->dwState))
				continue;
		
			ResetContext(pContext);
		}
		
		SendCollectCommand(ppMemberInfo[i]);
	}
}

DWORD CFileCollectTransaction::GetMemberState(PMEMBERINFO pMemberInfo)
{
	CAutoSectionLock lock(&m_cs);

	CCollectMap::iterator it = m_collectMap.find(pMemberInfo);
	if (it == m_collectMap.end())
		return COLLECT_NONE;

	PCOLLECTCONTEXT pContext = (*it).second;
	return pContext->dwState;
}

void CFileCollectTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == 0) 
	{
		// Check all students have got the collect command
		CCollectMap::iterator it = m_collectMap.begin();
		while (it != m_collectMap.end()) {
			PMEMBERINFO pMemberInfo = (*it).first;
			if (pMemberInfo && pMemberInfo->bLoggedIn) {
				PCOLLECTCONTEXT pContext = (*it).second;
				if (COLLECT_NONE == pContext->dwState) {
					SendCollectCommand(pMemberInfo);
				}
			}

			it ++;
		}

		m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);
	}
}

void CFileCollectTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_FILECOLLECT_STATE:
		{
			PPACKET_COLLECT_STATE pState = (PPACKET_COLLECT_STATE)(pPacket + 1);
			ProcessStatePacket(pState);
		}
		break;
	case MAGIC_FILECOLLECT_EVENT:
		{
			PPACKET_COLLECT_EVENT pEvent = (PPACKET_COLLECT_EVENT)(pPacket + 1);
			ReportClientErrorEvent(pEvent);
		}
		break;
	default:
		break;
	}
	
}

void CFileCollectTransaction::ProcessStatePacket(PPACKET_COLLECT_STATE pState)
{
	CCollectMap::iterator it = m_collectMap.find(pState->pMemberInfo);
	if (it == m_collectMap.end())
		return;

	if (!m_bCollecting) {
		SendStopCommand(pState->pMemberInfo);
		return;
	}

	if (m_dwTaskId != pState->dwTaskId) {
		return;
	}

	PCOLLECTCONTEXT pContext = (*it).second;

	if (COLLECT_FINISHED == pContext->dwState) {
		SendStopCommand(pState->pMemberInfo);
		return;
	}

	if (pContext->dwState != pState->dwState)
	{
		pContext->dwState = pState->dwState;
		pContext->dwFileCount = pState->dwFileCount;
		pContext->llFileSize = pState->llFileSize;
		pContext->dwErrorCode = pState->dwErrorCode;

		// Report state changed event
		ReportStateChangedEvent(pState);

		if (COLLECT_FINISHED == pContext->dwState) 
		{
			SendStopCommand(pState->pMemberInfo);

			if (IsCollectFinished()) {
				ReportFinishedEvent();
			}
		}
	}

	if (COLLECT_PREPARED == pContext->dwState) {
		// Send Transmit Command
		SendTransmitCommand(pState->pMemberInfo);
	}
}

BOOL CFileCollectTransaction::IsCollectFinished()
{
	BOOL bFinished = TRUE;

	CCollectMap::iterator it = m_collectMap.begin();
	while (it != m_collectMap.end()) {
		PMEMBERINFO pMemberInfo = (*it).first;
		if (pMemberInfo && pMemberInfo->bLoggedIn) {
			PCOLLECTCONTEXT pContext = (*it).second;
			if (COLLECT_FINISHED != pContext->dwState) {
				bFinished = FALSE;
				break;
			}
		}

		it ++;
	}

	return bFinished;
}

void CFileCollectTransaction::SendCollectCommand(PMEMBERINFO pMemberInfo)
{
	DWORD dwIpAddress = pMemberInfo->dwIpAddress;

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_COLLECT_MISSION pCollectMission = (PPACKET_COLLECT_MISSION)(pPacket + 1);

	pPacket->dwMagic = MAGIC_FILECOLLECT_MISSION;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_COLLECT_MISSION);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pCollectMission->dwTaskId = m_dwTaskId;
	pCollectMission->dwTeacherIp = m_dwLocalInterface;
	pCollectMission->pMemberInfo = pMemberInfo;
	pCollectMission->dwFlags = m_CollectMission.dwFlags;
	_tcscpy(pCollectMission->szFileType, m_CollectMission.szFileType);
	_tcscpy(pCollectMission->szFilePath, m_CollectMission.szFilePath);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CFileCollectTransaction::SendStopCommand(PMEMBERINFO pMemberInfo)
{
	DWORD dwIpAddress = pMemberInfo->dwIpAddress;

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_FILECOLLECT_STOP;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = 0;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CFileCollectTransaction::ReportStateChangedEvent(PPACKET_COLLECT_STATE pState)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_FILECOLLECT_STATE_CHANGED);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_FILECOLLECT_STATE_CHANGED pEvent = (PEVENT_FILECOLLECT_STATE_CHANGED)(pHeader + 1);

	pHeader->dwEvent = EV_FILECOLLECT_STATE_CHANGED;
	pHeader->dwLength = sizeof(EVENT_FILECOLLECT_STATE_CHANGED);

	pEvent->pMemberInfo = pState->pMemberInfo;
	pEvent->dwState = pState->dwState;
	pEvent->dwCount = pState->dwFileCount;
	pEvent->llTotalSize = pState->llFileSize;
	pEvent->dwErrorCode = pState->dwErrorCode;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CFileCollectTransaction::ReportFinishedEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_FILECOLLECT_FINISHED;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CFileCollectTransaction::ReportClientErrorEvent(PPACKET_COLLECT_EVENT pPacket)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_FILECOLLECT_CLIENTERROR) + pPacket->cbSize;
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_FILECOLLECT_CLIENTERROR pEvent = (PEVENT_FILECOLLECT_CLIENTERROR)(pHeader + 1);

	pHeader->dwEvent = EV_FILECOLLECT_CLIENTERROR;
	pHeader->dwLength = sizeof(EVENT_FILECOLLECT_CLIENTERROR) + pPacket->cbSize;

	pEvent->pMemberInfo = pPacket->pMemberInfo;
	pEvent->dwErrorCode = pPacket->dwErrorCode;
	pEvent->cbSize = pPacket->cbSize;
	memcpy(pEvent->abParams, pPacket->abParams, pPacket->cbSize);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CFileCollectTransaction::SendTransmitCommand(PMEMBERINFO pMemberInfo)
{
	DWORD dwIpAddress = pMemberInfo->dwIpAddress;

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_FILECOLLECT_TRANSMIT;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = 0;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CFileCollectTransaction::ResetContext(PCOLLECTCONTEXT pContext)
{
	if (!pContext)
		return;

	pContext->dwState = COLLECT_NONE;
	pContext->dwFileCount = 0;
	pContext->llFileSize = 0;
}

void CFileCollectTransaction::DeleteAllMembers()
{
	CCollectMap::iterator it = m_collectMap.begin();
	while (it != m_collectMap.end()) {
		PCOLLECTCONTEXT pContext = (*it).second;
		free(pContext);

		m_collectMap.erase(it);

		it = m_collectMap.begin();
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CPassiveFileCollectTransaction::CPassiveFileCollectTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID, IMacroPathTranslator* pTranslator) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);
	m_dwState = COLLECT_NONE;

	m_pMemberInfo = NULL;
	m_llTotalSize = 0;
	m_dwTeacherIp = 0;

	m_dwTaskId = 0;
	m_dwErrorCode = CE_NONE;
	m_dwFlags = 0;

	m_pTranslator = pTranslator;
}

CPassiveFileCollectTransaction::~CPassiveFileCollectTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CPassiveFileCollectTransaction::Destroy()
{
	ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
	pTransactionMgr->DeleteTransaction(m_pTransID);
	pTransactionMgr->Release();
}

void CPassiveFileCollectTransaction::LogEvent(DWORD dwErrorCode, DWORD cbSize, LPVOID lpvBuffer)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_COLLECT_EVENT pEvent = (PPACKET_COLLECT_EVENT)(pPacket + 1);

	pPacket->dwMagic = MAGIC_FILECOLLECT_EVENT;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_COLLECT_EVENT) + cbSize;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pEvent->pMemberInfo = m_pMemberInfo;
	pEvent->dwErrorCode = dwErrorCode;
	pEvent->cbSize = cbSize;
	memcpy(pEvent->abParams, lpvBuffer, cbSize);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPassiveFileCollectTransaction::SetTransmitEvent(DWORD dwEvent, DWORD dwRequest, DWORD dwErrorCode)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_TRANSMIT);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_TRANSMIT pEvent = (PEVENT_TRANSMIT)(pHeader + 1);

	pHeader->dwEvent = EV_FILECOLLECT_TRANSMIT;
	pHeader->dwLength = sizeof(EVENT_TRANSMIT);

	pEvent->dwEvent = dwEvent;
	pEvent->dwRequest = dwRequest;
	pEvent->dwErrorCode = dwErrorCode;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveFileCollectTransaction::OnTransmitFinished(DWORD dwErrorCode)
{
	CAutoSectionLock lock(&m_cs);

//	if (m_dwState == COLLECT_TRANSMITTING) {

		ReportEndCollectEvent();

		// Update State
		m_dwErrorCode = dwErrorCode;
		m_dwState = COLLECT_FINISHED;
		UpdateCollectState();
//	}
}

int CPassiveFileCollectTransaction::GetFileCount()
{
	CAutoSectionLock lock(&m_cs);

	return m_FileList.size();
}

BOOL CPassiveFileCollectTransaction::GetFileName(TCHAR* pszFileName)
{
	CAutoSectionLock lock(&m_cs);

	if (!pszFileName)
		return FALSE;

	CCollectFileList::iterator it = m_FileList.begin();
	if (it == m_FileList.end())
		return FALSE;

	_tcscpy(pszFileName, (*it).szPathName);
	m_FileList.erase(it);

	return TRUE;
}

void CPassiveFileCollectTransaction::PutFileName(TCHAR* pszFileName)
{
	CAutoSectionLock lock(&m_cs);

	if (!pszFileName)
		return;

	COLLECTFILE ccFile = {0};
	_tcscpy(ccFile.szPathName, pszFileName);
	m_FileList.insert(m_FileList.begin(), ccFile);
}

void CPassiveFileCollectTransaction::DeleteAllFiles()
{
	CAutoSectionLock lock(&m_cs);

	m_FileList.clear();
}

void CPassiveFileCollectTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == 0)
	{
		UpdateCollectState();

		// Set timeout to report state
		m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);
	}
}

void CPassiveFileCollectTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_FILECOLLECT_MISSION:
		{
			// Start Collect Command
			PPACKET_COLLECT_MISSION pCollectMission = (PPACKET_COLLECT_MISSION)(pPacket + 1);

			_TRACE(_T("MAGIC_FILECOLLECT_MISSION \n"));

			if ((m_dwState == COLLECT_NONE) || (m_dwState == COLLECT_FINISHED))
			{
				_TRACE(_T("MAGIC_FILECOLLECT_MISSION 1\n"));

				m_pComLayer->UnregisterTimeout(m_pTransID, 0);

				m_dwFlags = pCollectMission->dwFlags;
				m_dwTaskId = pCollectMission->dwTaskId;
				m_pMemberInfo = pCollectMission->pMemberInfo;
				m_dwTeacherIp = pCollectMission->dwTeacherIp;

				// calculat file count and size
				m_FileList.clear();
				m_llTotalSize = 0;
				CalculateCollectFileList(pCollectMission);

				// Update State
				m_dwState = COLLECT_PREPARED;
				UpdateCollectState();

				// Set timeout to report state
				m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);
			} else
			{
				_TRACE(_T("MAGIC_FILECOLLECT_MISSION 2\n"));

				if (m_dwTaskId != pCollectMission->dwTaskId) {
					ReportEndCollectEvent();
					m_dwState = COLLECT_NONE;
					m_pComLayer->UnregisterTimeout(m_pTransID, 0);
					m_FileList.clear();
					m_llTotalSize = 0;
					m_dwErrorCode = CE_NONE;
				} else
				{
					_TRACE(_T("On Collecting %08X, Ignore this command. \n"), m_dwTaskId);
				}
			}
		}
		break;
	case MAGIC_FILECOLLECT_TRANSMIT:
		{
			_TRACE(_T("MAGIC_FILECOLLECT_TRANSMIT\n"));
			// Begin Send data
			if (COLLECT_PREPARED == m_dwState) {
				_TRACE(_T("MAGIC_FILECOLLECT_TRANSMIT 1\n"));

				m_dwState = COLLECT_TRANSMITTING;

				UpdateCollectState();

				// Report event to start send file data
				ReportBeginCollectEvent();
			}
		}
		break;
	case MAGIC_FILECOLLECT_STOP:
		{
			_TRACE(_T("MAGIC_FILECOLLECT_STOP\n"));

			// Stop collect
			ReportEndCollectEvent();
			m_dwState = COLLECT_NONE;
			m_pComLayer->UnregisterTimeout(m_pTransID, 0);
			m_FileList.clear();
			m_llTotalSize = 0;
			m_dwErrorCode = CE_NONE;
		}
		break;
	default:
		break;
	}
}

void CPassiveFileCollectTransaction::CalculateCollectFileList(PPACKET_COLLECT_MISSION pCollectMission)
{
	TCHAR* pStart = pCollectMission->szFileType;
	TCHAR* pEnd = _tcsstr(pStart, _T(";"));

	TCHAR szFilePath[MAX_PATH] = {0};
	_tcscpy(szFilePath, pCollectMission->szFilePath);

	if (m_pTranslator) {
		m_pTranslator->TranslatePath(szFilePath);
	}

	do 
	{
		if (NULL == pEnd) {
			SeachFilesInPath(szFilePath, pStart);
			break;
		} else
		{
			TCHAR szFileType[MAX_PATH] = {0};
			_tcsncpy(szFileType, pStart, pEnd - pStart);
			SeachFilesInPath(szFilePath, szFileType);

			pStart = pEnd + 1;
			if (pStart >= (pCollectMission->szFileType + _tcslen(pCollectMission->szFileType)))
				break;

			pEnd = _tcsstr(pStart, _T(";"));
		}
	} while (TRUE);

	_TRACE(_T("CalculateCollectFileList count %d\n"), m_FileList.size());
}

void CPassiveFileCollectTransaction::SeachFilesInPath(TCHAR* pszFilePath, TCHAR* pszFileType)
{
	if ((_tcslen(pszFilePath) == 0) || (_tcslen(pszFileType) == 0))
		return;

	TCHAR szPathName[MAX_PATH] = {0};
	_tcscpy(szPathName, pszFilePath);
	if (szPathName[_tcslen(szPathName) - 1] != '\\')
		_tcscat(szPathName, _T("\\"));

	TCHAR szFileName[MAX_PATH] = {0};
	_tcscpy(szFileName, szPathName);
	_tcscat(szFileName, pszFileType);

	WIN32_FIND_DATA findData;
	HANDLE hFindFile = FindFirstFile(szFileName, &findData);
	if (hFindFile == INVALID_HANDLE_VALUE)
		return;

	while (TRUE) {
		if ((!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) &&
			(findData.cFileName[0] != '.'))
		{
			COLLECTFILE ccFile = {0};
			_tcscpy(ccFile.szPathName, szPathName);
			_tcscat(ccFile.szPathName, findData.cFileName);

			if (!IsFileExist(&ccFile)) {
				m_llTotalSize += ((LONGLONG)findData.nFileSizeHigh << 32) | findData.nFileSizeLow;
				m_FileList.push_back(ccFile);
			}
		}

		if (!FindNextFile(hFindFile, &findData))
			break;
	}

	FindClose(hFindFile);
}

BOOL CPassiveFileCollectTransaction::IsFileExist(COLLECTFILE* pFile)
{
	CCollectFileList::iterator it = m_FileList.begin();
	while (it != m_FileList.end()) {
		if (_tcscmp((*it).szPathName, pFile->szPathName) == 0)
			return TRUE;

		it ++;
	}

	return FALSE;
}

void CPassiveFileCollectTransaction::UpdateCollectState()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_COLLECT_STATE pState = (PPACKET_COLLECT_STATE)(pPacket + 1);

	pPacket->dwMagic = MAGIC_FILECOLLECT_STATE;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_COLLECT_STATE);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pState->pMemberInfo = m_pMemberInfo;
	pState->dwTaskId = m_dwTaskId;
	pState->dwState = m_dwState;
	pState->dwFileCount = m_FileList.size();
	pState->llFileSize = m_llTotalSize;
	pState->dwErrorCode = m_dwErrorCode;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPassiveFileCollectTransaction::ReportBeginCollectEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_FILECOLLECT_BEGIN;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveFileCollectTransaction::ReportEndCollectEvent()
{
	// report event
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_FILECOLLECT_END;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CConferenceCallTransaction::CConferenceCallTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);

	m_dwChannelMask = 1;
	m_dwState = CC_IDLE;
	m_bTeacher = FALSE;
	m_bStarted = FALSE;

	m_dwCallingIp = 0;
	m_dwTalkingIp = 0;
}

CConferenceCallTransaction::~CConferenceCallTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CConferenceCallTransaction::Start(DWORD dwChannelMask, TCHAR* pszName, BOOL bTeacher)
{
	CAutoSectionLock lock(&m_cs);

	if (m_bStarted)
		return;

	m_dwChannelMask = dwChannelMask;
	m_bTeacher = bTeacher;
	_tcsncpy(m_szName, pszName, 32);

	// Start Announce
	SendAnnouncePacket();
	m_pComLayer->RegisterTimeout(m_pTransID, 500, TIMERID_ANNOUNCE);
	m_pComLayer->RegisterTimeout(m_pTransID, 5000, TIMERID_LOOPCHECK);

	m_bStarted = TRUE;
}

void CConferenceCallTransaction::Stop()
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	SendExitPacket();

	Sleep(100);

//	m_pComLayer->UnregisterTimeout(m_pTransID, TIMERID_ANNOUNCE);
//	m_pComLayer->UnregisterTimeout(m_pTransID, TIMERID_LOOPCHECK);

	m_bStarted = FALSE;

	// Clear Member List
	CConfCallMap::iterator it = m_ConfCallMap.begin();
	while (it != m_ConfCallMap.end()) {
		free((*it).second);
		m_ConfCallMap.erase(it);
		it = m_ConfCallMap.begin();
	}
}

void CConferenceCallTransaction::CallMember(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	// Check state
	if (m_dwState != CConferenceCallTransaction::CC_IDLE)
		return;

	m_dwCallingIp = dwIpAddress;
	m_dwState = CConferenceCallTransaction::CC_CALLING;

	SendAnnouncePacket();
}

void CConferenceCallTransaction::CancelCall()
{
	CAutoSectionLock lock(&m_cs);

	if (m_dwState != CConferenceCallTransaction::CC_CALLING)
		return;

	m_dwCallingIp = 0;
	m_dwState = CConferenceCallTransaction::CC_IDLE;

	SendAnnouncePacket();
}

void CConferenceCallTransaction::AcceptMember(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	_ASSERT (m_dwState != CConferenceCallTransaction::CC_TALKING);

	// Check if there is the member or not
	if (m_ConfCallMap.find(dwIpAddress) == m_ConfCallMap.end())
		return;

	// Check if last respond have been received ot not 
	if (m_RespondMap.find(dwIpAddress) != m_RespondMap.end())
		return;

	m_RespondMap.insert(CRespondMap::value_type(dwIpAddress, TRUE));

	SendRespondPacket(dwIpAddress, TRUE);
}

void CConferenceCallTransaction::DenyMember(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

//	_ASSERT (m_dwState != CConferenceCallTransaction::CC_TALKING);

	// Check if there is the member or not
	if (m_ConfCallMap.find(dwIpAddress) == m_ConfCallMap.end())
		return;

	// Check if last respond have been received ot not 
	if (m_RespondMap.find(dwIpAddress) != m_RespondMap.end())
		return;

	m_RespondMap.insert(CRespondMap::value_type(dwIpAddress, FALSE));

	SendRespondPacket(dwIpAddress, FALSE);
}

void CConferenceCallTransaction::EnterTalking(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	if (m_dwState == CConferenceCallTransaction::CC_TALKING)
		return;

	m_dwTalkingIp = dwIpAddress;
	m_dwState = CConferenceCallTransaction::CC_TALKING;

	SendAnnouncePacket();
}

void CConferenceCallTransaction::LeaveTalking()
{
	CAutoSectionLock lock(&m_cs);

	if (m_dwState != CConferenceCallTransaction::CC_TALKING)
		return;

	m_dwTalkingIp = 0;
	m_dwState = CConferenceCallTransaction::CC_IDLE;

	SendAnnouncePacket();
}

void CConferenceCallTransaction::SetName(TCHAR* pszName)
{
	CAutoSectionLock lock(&m_cs);

	_tcsncpy(m_szName, pszName, 32);

	SendAnnouncePacket();
}

DWORD CConferenceCallTransaction::GetTalkingIp(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	DWORD dwTalkingIp = 0;
	CConfCallMap::iterator it = m_ConfCallMap.find(dwIpAddress);
	if (it != m_ConfCallMap.end()) {
		PCONFCALL_CONTEXT pContext = (*it).second;
		if (pContext->dwState == CConferenceCallTransaction::CC_TALKING)
			dwTalkingIp = pContext->dwTalkingIp;
	}

	return dwTalkingIp;
}

void CConferenceCallTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	switch (dwTimeID)
	{
	case TIMERID_ANNOUNCE:
		{
			SendAnnouncePacket();
			m_pComLayer->RegisterTimeout(m_pTransID, 500, TIMERID_ANNOUNCE);
		}
		break;
	case TIMERID_LOOPCHECK:
		{
			CConfCallMap::iterator it = m_ConfCallMap.begin();
			while (it != m_ConfCallMap.end())
			{
				DWORD dwIpAddress = (*it).first;
				PCONFCALL_CONTEXT pContext = (*it).second;
				if (GetTickCount() - pContext->dwTickCount > 30000)
				{
					ReportDelMemberEvent(dwIpAddress, pContext);
					DeleteRespondMember(dwIpAddress);

					m_ConfCallMap.erase(it);
					free(pContext);
					it = m_ConfCallMap.begin();
				} else
					it ++;
			}

			m_pComLayer->RegisterTimeout(m_pTransID, 5000, TIMERID_LOOPCHECK);
		}
		break;
	default:
		break;
	}
}

void CConferenceCallTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_CONFCALL_ANNOUNCE:
		{
			PPACKET_CONFCALL_ANNOUNCE pAnncounce = (PPACKET_CONFCALL_ANNOUNCE)(pPacket + 1);
			if ((pAnncounce->dwChannelMask & m_dwChannelMask) &&
				(pAnncounce->dwIpAddress != m_pComLayer->GetLocalAddress())) 
			{
				ProcessAnnouncePacket(pAnncounce);
			}
		}
		break;
	case MAGIC_CONFCALL_RESPOND:
		{
			PPACKET_CONFCALL_RESPOND pRespond = (PPACKET_CONFCALL_RESPOND)(pPacket + 1);
			
			if ((m_dwState == CConferenceCallTransaction::CC_CALLING) && 
				(pRespond->dwIpAddress == m_dwCallingIp))
			{
				if (pRespond->bAccept)
				{
					m_dwCallingIp = 0;
					m_dwTalkingIp = pRespond->dwIpAddress;
					m_dwState = CConferenceCallTransaction::CC_TALKING;

					ReportRespondEvent(pRespond->dwIpAddress, TRUE);
				} else
				{
					m_dwCallingIp = 0;
					m_dwState = CConferenceCallTransaction::CC_IDLE;

					ReportRespondEvent(pRespond->dwIpAddress, FALSE);
				}

				SendAnnouncePacket();
			}
		}
		break;
	case MAGIC_CONFCALL_EXIT:
		{
			PPACKET_CONFCALL_EXIT pExit = (PPACKET_CONFCALL_EXIT)(pPacket + 1);
			if (pExit->dwIpAddress != m_pComLayer->GetLocalAddress()) {
				ProcessExitPacket(pExit);
			}
		}
		break;
	default:
		break;
	}

}

void CConferenceCallTransaction::SendAnnouncePacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_pComLayer->GetComAddress(), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_CONFCALL_ANNOUNCE pAnnounce = (PPACKET_CONFCALL_ANNOUNCE)(pPacket + 1);

	pPacket->dwMagic = MAGIC_CONFCALL_ANNOUNCE;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_CONFCALL_ANNOUNCE);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pAnnounce->bTeacher = m_bTeacher;
	pAnnounce->dwChannelMask = m_dwChannelMask;
	pAnnounce->dwIpAddress = m_pComLayer->GetLocalAddress();
	pAnnounce->dwState = m_dwState;
	pAnnounce->dwCallingIp = m_dwCallingIp;
	pAnnounce->dwTalkingIp = m_dwTalkingIp;
	_tcsncpy(pAnnounce->szName, m_szName, 32);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CConferenceCallTransaction::SendRespondPacket(BOOL dwIpAddress, BOOL bAccept)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_CONFCALL_RESPOND pRespond = (PPACKET_CONFCALL_RESPOND)(pPacket + 1);

	pPacket->dwMagic = MAGIC_CONFCALL_RESPOND;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_CONFCALL_RESPOND);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pRespond->dwIpAddress = m_pComLayer->GetLocalAddress();
	pRespond->bAccept = bAccept;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CConferenceCallTransaction::SendExitPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_pComLayer->GetComAddress(), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPACKET_CONFCALL_EXIT pExit = (PPACKET_CONFCALL_EXIT)(pPacket + 1);

	pPacket->dwMagic = MAGIC_CONFCALL_EXIT;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_CONFCALL_EXIT);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pExit->dwIpAddress = m_pComLayer->GetLocalAddress();

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CConferenceCallTransaction::ProcessAnnouncePacket(PPACKET_CONFCALL_ANNOUNCE pAnncounce)
{
	CConfCallMap::iterator it = m_ConfCallMap.find(pAnncounce->dwIpAddress);
	
	if (it == m_ConfCallMap.end())
	{
		PCONFCALL_CONTEXT pContext = (PCONFCALL_CONTEXT)malloc(sizeof(CONFCALL_CONTEXT));
		if (!pContext)
			return;

		pContext->bTeacher = pAnncounce->bTeacher;
		pContext->dwState = pAnncounce->dwState;
		pContext->dwTickCount = GetTickCount();
		_tcsncpy(pContext->szName, pAnncounce->szName, 32);

		m_ConfCallMap.insert(CConfCallMap::value_type(pAnncounce->dwIpAddress, pContext));

		ReportNewMemberEvent(pAnncounce->dwIpAddress, pContext);
	} else
	{
		DWORD dwIpAddress = (*it).first;
		PCONFCALL_CONTEXT pContext = (*it).second;
		pContext->dwTickCount = GetTickCount();

		// Process State Change
		if (pContext->dwState != pAnncounce->dwState) {
			pContext->dwState = pAnncounce->dwState;
			pContext->dwCallingIp = pAnncounce->dwCallingIp;
			pContext->dwTalkingIp = pAnncounce->dwTalkingIp;

			ReportStateChangedEvent(dwIpAddress, pContext);
		}

		// Process Respond Map
		CRespondMap::iterator it = m_RespondMap.find(pAnncounce->dwIpAddress);
		if (it != m_RespondMap.end()) {
			if (pContext->dwState == CConferenceCallTransaction::CC_CALLING)
				SendRespondPacket(pAnncounce->dwIpAddress, (*it).second);
			else
				m_RespondMap.erase(it);
		}

		// Process Name Change
		if (_tcscmp(pContext->szName, pAnncounce->szName) != 0) {
			_tcsncpy(pContext->szName, pAnncounce->szName, 32);
			ReportNameChangedEvent(dwIpAddress, pContext);
		}

		// Process Calling each other
/*
		if ((m_dwState == CConferenceCallTransaction::CC_CALLING) &&
			(m_dwCallingIp == pAnncounce->dwIpAddress) &&
			(pAnncounce->dwState == CConferenceCallTransaction::CC_CALLING) &&
			(pAnncounce->dwCallingIp == m_pComLayer->GetLocalAddress()))
		{
			m_dwCallingIp = 0;
			m_dwTalkingIp = pAnncounce->dwIpAddress;
			m_dwState = CConferenceCallTransaction::CC_TALKING;

			ReportRespondEvent(pAnncounce->dwIpAddress, TRUE);
		}
*/
	}
}

void CConferenceCallTransaction::ProcessExitPacket(PPACKET_CONFCALL_EXIT pExit)
{
	CConfCallMap::iterator it = m_ConfCallMap.find(pExit->dwIpAddress);
	if (it != m_ConfCallMap.end()) {
		DWORD dwIpAddress = (*it).first;
		PCONFCALL_CONTEXT pContext = (*it).second;

		ReportDelMemberEvent(dwIpAddress, pContext);
		DeleteRespondMember(dwIpAddress);

		m_ConfCallMap.erase(it);
		free(pContext);
	}
}

void CConferenceCallTransaction::ReportNewMemberEvent(DWORD dwIpAddress, PCONFCALL_CONTEXT pContext)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_CONFCALL_ADDMEMBER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_CONFCALL_ADDMEMBER pEvent = (PEVENT_CONFCALL_ADDMEMBER)(pHeader + 1);

	pHeader->dwEvent = EV_CONFCALL_ADDMEMBER;
	pHeader->dwLength = sizeof(EVENT_CONFCALL_ADDMEMBER);

	pEvent->bTeacher = pContext->bTeacher;
	pEvent->dwIpAddress = dwIpAddress;
	pEvent->dwState = pContext->dwState;
	_tcsncpy(pEvent->szName, pContext->szName, 32);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CConferenceCallTransaction::ReportDelMemberEvent(DWORD dwIpAddress, PCONFCALL_CONTEXT pContext)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_CONFCALL_DELMEMBER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_CONFCALL_DELMEMBER pEvent = (PEVENT_CONFCALL_DELMEMBER)(pHeader + 1);

	pHeader->dwEvent = EV_CONFCALL_DELMEMBER;
	pHeader->dwLength = sizeof(EVENT_CONFCALL_DELMEMBER);

	pEvent->dwIpAddress = dwIpAddress;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CConferenceCallTransaction::ReportStateChangedEvent(DWORD dwIpAddress, PCONFCALL_CONTEXT pContext)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_CONFCALL_STATECHANGED);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_CONFCALL_STATECHANGED pEvent = (PEVENT_CONFCALL_STATECHANGED)(pHeader + 1);

	pHeader->dwEvent = EV_CONFCALL_STATECHANGED;
	pHeader->dwLength = sizeof(EVENT_CONFCALL_STATECHANGED);

	pEvent->dwIpAddress = dwIpAddress;
	pEvent->dwNewState = pContext->dwState;
	pEvent->dwCallingIp = pContext->dwCallingIp;
	pEvent->dwTalkingIp = pContext->dwTalkingIp;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CConferenceCallTransaction::ReportNameChangedEvent(DWORD dwIpAddress, PCONFCALL_CONTEXT pContext)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_CONFCALL_NAMECHANGED);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_CONFCALL_NAMECHANGED pEvent = (PEVENT_CONFCALL_NAMECHANGED)(pHeader + 1);

	pHeader->dwEvent = EV_CONFCALL_NAMECHANGED;
	pHeader->dwLength = sizeof(EVENT_CONFCALL_NAMECHANGED);

	pEvent->dwIpAddress = dwIpAddress;
	_tcsncpy(pEvent->szNewName, pContext->szName, 32);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CConferenceCallTransaction::ReportRespondEvent(DWORD dwIpAddress, BOOL bAccept)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_CONFCALL_RESPOND);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_CONFCALL_RESPOND pEvent = (PEVENT_CONFCALL_RESPOND)(pHeader + 1);

	pHeader->dwEvent = EV_CONFCALL_RESPOND;
	pHeader->dwLength = sizeof(EVENT_CONFCALL_RESPOND);

	pEvent->dwIpAddress = dwIpAddress;
	pEvent->bAccept = bAccept;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CConferenceCallTransaction::DeleteRespondMember(DWORD dwIpAddress)
{
	CRespondMap::iterator it = m_RespondMap.find(dwIpAddress);
	if (it != m_RespondMap.end()) {
		m_RespondMap.erase(it);
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

#define BASE_BLOCK_SIZE		1024

#define BASE_BLOCK_SIZE		1024

CBlockSenderTransaction::CBlockSenderTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwLocalInterface = 0;
	m_dwReceiverIp = 0;
	m_dwBlockType = 0;
	m_pBlockBuffer = NULL;
	m_dwBlockSize = 0;

	m_nBlocks = 0;
	m_pRepairMap = NULL;

	m_nSyncId = 0;

	m_bSendFinished = FALSE;

	InitializeCriticalSection(&m_cs);
}

CBlockSenderTransaction::~CBlockSenderTransaction()
{
	if (m_pBlockBuffer) {
		free(m_pBlockBuffer);
		m_pBlockBuffer = NULL;
	}

	if (m_pRepairMap) {
		free(m_pRepairMap);
		m_pRepairMap = NULL;
	}

	DeleteCriticalSection(&m_cs);
}

BOOL CBlockSenderTransaction::StartSendBlock(DWORD dwLocalInterface, DWORD dwReceiverIp, DWORD dwBlockType, BYTE* pbBuffer, DWORD cbSize)
{
	CAutoSectionLock lock(&m_cs);

	m_dwLocalInterface = dwLocalInterface;
	m_dwReceiverIp = dwReceiverIp;
	m_dwBlockType = dwBlockType;

	m_dwBlockSize = cbSize;
	m_pBlockBuffer = (BYTE *)malloc(cbSize);
	if (!m_pBlockBuffer)
		return FALSE;

	memcpy(m_pBlockBuffer, pbBuffer, m_dwBlockSize);

	m_nBlocks = (m_dwBlockSize + BASE_BLOCK_SIZE - 1) / BASE_BLOCK_SIZE;
	m_pRepairMap = (BYTE *)malloc(m_nBlocks);
	if (!m_pRepairMap)
		return FALSE;

	memset(m_pRepairMap, 1, m_nBlocks);
	CheckAndSendBlock();
	m_pComLayer->RegisterTimeout(m_pTransID, GetRandomSyncTime(), CBlockSenderTransaction::ID_TIMER_SYNC);

	return TRUE;
}

void CBlockSenderTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == CBlockSenderTransaction::ID_TIMER_SYNC)
	{
		SendSyncPacket();
		m_pComLayer->RegisterTimeout(m_pTransID, GetRandomSyncTime(), CBlockSenderTransaction::ID_TIMER_SYNC);
	}

	if (dwTimeID == CBlockSenderTransaction::ID_TIMER_STOP)
	{
		SendStopPacket();
		m_pComLayer->RegisterTimeout(m_pTransID, GetRandomStopTime(), CBlockSenderTransaction::ID_TIMER_STOP);
	}

	if (dwTimeID == CBlockSenderTransaction::ID_TIMER_DESTROY)
	{
		_TRACE(_T("CBlockSenderTransaction::ID_TIMER_DESTROY, Lost Data\n"));
		m_pComLayer->UnregisterTimeout(m_pTransID, CBlockSenderTransaction::ID_TIMER_STOP);
		Destroy();
	}
}

void CBlockSenderTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_BLOCK_RET:
		{
			PPACKET_BLOCK_RET pBlockRet = (PPACKET_BLOCK_RET)(pPacket + 1);
			if (pBlockRet->nSyncId < m_nSyncId)
				return;

			m_pComLayer->UnregisterTimeout(m_pTransID, CBlockSenderTransaction::ID_TIMER_SYNC);

			if (pBlockRet->nRepairCount == 0)
			{
				SendStopPacket();

				if (!m_bSendFinished) {

					ReportSendOutEvent();

					m_pComLayer->RegisterTimeout(m_pTransID, GetRandomStopTime(), CBlockSenderTransaction::ID_TIMER_STOP);
					m_pComLayer->RegisterTimeout(m_pTransID, 60000, CBlockSenderTransaction::ID_TIMER_DESTROY);
					m_bSendFinished = TRUE;
				}
			} else if (pBlockRet->nRepairCount == -1)
			{
				memset(m_pRepairMap, 1, m_nBlocks);
				CheckAndSendBlock();
				m_pComLayer->RegisterTimeout(m_pTransID, GetRandomSyncTime(), CBlockSenderTransaction::ID_TIMER_SYNC);
			} else
			{
				for (int i=0; i<pBlockRet->nRepairCount; i++) {
					int nBlockId = pBlockRet->abBlockId[i];
					m_pRepairMap[nBlockId] = 1;
				}
				CheckAndSendBlock();
				m_pComLayer->RegisterTimeout(m_pTransID, GetRandomSyncTime(), CBlockSenderTransaction::ID_TIMER_SYNC);
			}
		}
		break;
	case MAGIC_BLOCK_CLOSE:
		{
			m_pComLayer->UnregisterTimeout(m_pTransID, CBlockSenderTransaction::ID_TIMER_STOP);
			m_pComLayer->UnregisterTimeout(m_pTransID, CBlockSenderTransaction::ID_TIMER_DESTROY);
			Destroy();
		}
		break;
	default:
		break;
	}	
}

void CBlockSenderTransaction::CheckAndSendBlock()
{
	for (int i=0; i<m_nBlocks; i++) {
		if (m_pRepairMap[i] == 1)
		{
			SendDataPacket(i);
		}
	}

	memset(m_pRepairMap, 0, m_nBlocks);

	m_nSyncId ++;
}

void CBlockSenderTransaction::SendDataPacket(int nBlockId)
{
	BYTE* pData = m_pBlockBuffer + nBlockId * BASE_BLOCK_SIZE;
	DWORD cbSize = BASE_BLOCK_SIZE;
	if (nBlockId == m_nBlocks - 1) {
		cbSize = m_dwBlockSize - nBlockId * BASE_BLOCK_SIZE;
	}
	
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwReceiverIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_BLOCK;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_BLOCK) + cbSize;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_BLOCK pPacketBlock = (PPACKET_BLOCK)(pPacket + 1);
	pPacketBlock->dwFlags = BLOCK_DATA_PACKET;
	pPacketBlock->dwSenderIp = m_dwLocalInterface;
	pPacketBlock->dwReceiverIp = m_dwReceiverIp;
	pPacketBlock->dwBlockType = m_dwBlockType;
	pPacketBlock->dwBlockSize = m_dwBlockSize;
	pPacketBlock->nBlockId = nBlockId;
	pPacketBlock->nSyncId = m_nSyncId;
	pPacketBlock->cbSize = cbSize;
	memcpy(pPacketBlock->abBuffer, pData, cbSize);

	_TRACE(_T("nBlockId: %d, cbSize: %d \n"), nBlockId, cbSize);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CBlockSenderTransaction::SendSyncPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwReceiverIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_BLOCK;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_BLOCK);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_BLOCK pPacketBlock = (PPACKET_BLOCK)(pPacket + 1);
	pPacketBlock->dwFlags = BLOCK_SYNC_PACKET;
	pPacketBlock->dwSenderIp = m_dwLocalInterface;
	pPacketBlock->dwReceiverIp = m_dwReceiverIp;
	pPacketBlock->dwBlockType = m_dwBlockType;
	pPacketBlock->dwBlockSize = m_dwBlockSize;
//	pPacketBlock->nBlockId;
	pPacketBlock->nSyncId = m_nSyncId;
	pPacketBlock->cbSize = 0;
//	pPacketBlock->abBuffer;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CBlockSenderTransaction::SendStopPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwReceiverIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_BLOCK;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_BLOCK);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_BLOCK pPacketBlock = (PPACKET_BLOCK)(pPacket + 1);
	pPacketBlock->dwFlags = BLOCK_STOP_PACKET;
	pPacketBlock->dwSenderIp = m_dwLocalInterface;
	pPacketBlock->dwReceiverIp = m_dwReceiverIp;
	pPacketBlock->dwBlockType = m_dwBlockType;
	pPacketBlock->dwBlockSize = m_dwBlockSize;
//	pPacketBlock->nBlockId;
//	pPacketBlock->nSyncId;
	pPacketBlock->cbSize = 0;
//	pPacketBlock->abBuffer;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CBlockSenderTransaction::ReportSendOutEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_BLOCK_SENDOUT);

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_BLOCK_SENDOUT;
	pHeader->dwLength = sizeof(EVENT_BLOCK_SENDOUT);

	PEVENT_BLOCK_SENDOUT pEvent = (PEVENT_BLOCK_SENDOUT)(pHeader + 1);
	pEvent->dwReceiverIp = m_dwReceiverIp;
	pEvent->dwBlockType = m_dwBlockType;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CBlockSenderTransaction::Destroy()
{
	ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
	pTransactionMgr->DeleteTransaction(m_pTransID);
	pTransactionMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CBlockRecverTransaction::CBlockRecverTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwSenderIp = 0;
	m_dwLocalInterface = 0;
	m_dwBlockType = 0;
	m_dwBlockSize = 0;
	m_nBlocks = 0;
	m_dwReceivedSize = 0;

	m_pBlockBuffer = NULL;
	m_pRepairMap = NULL;

	m_bGotBlock = FALSE;

	InitializeCriticalSection(&m_cs);
}

CBlockRecverTransaction::~CBlockRecverTransaction()
{
	if (m_pBlockBuffer) {
		free(m_pBlockBuffer);
		m_pBlockBuffer = NULL;
	}

	if (m_pRepairMap) {
		free(m_pRepairMap);
		m_pRepairMap = NULL;
	}

	DeleteCriticalSection(&m_cs);
}

BOOL CBlockRecverTransaction::ProcessBlock(PPACKET_BLOCK pPacketBlock)
{
	CAutoSectionLock lock(&m_cs);

	m_dwSenderIp = pPacketBlock->dwSenderIp;
	m_dwLocalInterface = pPacketBlock->dwReceiverIp;
	m_dwBlockType = pPacketBlock->dwBlockType;
	m_dwBlockSize = pPacketBlock->dwBlockSize;
	m_nBlocks = (m_dwBlockSize + BASE_BLOCK_SIZE - 1) / BASE_BLOCK_SIZE;

	m_pBlockBuffer = (BYTE *)malloc(m_dwBlockSize);
	if (!m_pBlockBuffer)
		return FALSE;

	m_pRepairMap = (BYTE *)malloc(m_nBlocks);
	if (!m_pRepairMap)
		return FALSE;

	memset(m_pRepairMap, 0, m_nBlocks);

	if (pPacketBlock->dwFlags == BLOCK_DATA_PACKET)
	{
		if (pPacketBlock->nBlockId >= m_nBlocks)
			return FALSE;

		memcpy(m_pBlockBuffer + pPacketBlock->nBlockId * BASE_BLOCK_SIZE, 
			pPacketBlock->abBuffer, pPacketBlock->cbSize);
		m_pRepairMap[pPacketBlock->nBlockId] = 1;
		m_dwReceivedSize += pPacketBlock->cbSize;

		_TRACE(_T("ProcessBlock pPacketBlock->nBlockId = %d \n"), pPacketBlock->nBlockId);

	} else if (pPacketBlock->dwFlags == BLOCK_SYNC_PACKET)
	{
		SendBlockRet(pPacketBlock->nSyncId);
	} else
	{
		_TRACE(_T("Unknown Packet \n"));
	}

	return TRUE;
}

void CBlockRecverTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == 0)
	{
		ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
		pTransactionMgr->DeleteTransaction(m_pTransID);
		pTransactionMgr->Release();
	}
}

void CBlockRecverTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_pBlockBuffer || !m_pRepairMap)
		return;

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	if (pPacket->dwMagic != MAGIC_BLOCK)
		return;

	if (m_bGotBlock) {
		SendBlockClose();
		return;
	}

	PPACKET_BLOCK pPacketBlock = (PPACKET_BLOCK)(pPacket + 1);
	switch (pPacketBlock->dwFlags)
	{
	case BLOCK_DATA_PACKET:
		{
			if (pPacketBlock->nBlockId >= m_nBlocks)
				return;

			if (m_pRepairMap[pPacketBlock->nBlockId])
				return;

			memcpy(m_pBlockBuffer + pPacketBlock->nBlockId * BASE_BLOCK_SIZE, 
				pPacketBlock->abBuffer, pPacketBlock->cbSize);
			m_pRepairMap[pPacketBlock->nBlockId] = 1;
			m_dwReceivedSize += pPacketBlock->cbSize;

			_TRACE(_T("pPacketBlock->nBlockId = %d \n"), pPacketBlock->nBlockId);

			if (m_dwReceivedSize >= m_dwBlockSize) {
				SendBlockRet(pPacketBlock->nSyncId + 1);
			}
		}
		break;
	case BLOCK_SYNC_PACKET:
		{
			SendBlockRet(pPacketBlock->nSyncId);
		}
		break;
	case BLOCK_STOP_PACKET:
		{
			SendBlockClose();

			if (!m_bGotBlock) {
				ReportBlockArrivedEvent();
				m_pComLayer->RegisterTimeout(m_pTransID, 60000, 0);
				m_bGotBlock = TRUE;
			}
		}
		break;
	default:
		break;
	}
}

void CBlockRecverTransaction::SendBlockRet(int nSyncId)
{
	int nCount = 0;
	int abBlock[256] = {0};

	if ((m_dwReceivedSize == 0) || (m_dwReceivedSize >= m_dwBlockSize)) {
	} else
	{
		for (int i=0; i<m_nBlocks; i++) {
			if (!m_pRepairMap[i]) {
				abBlock[nCount ++] = i;
				if (nCount >= 256)
					break;
			}
		}
	}

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwSenderIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_BLOCK_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_BLOCK_RET) + nCount * sizeof(int);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_BLOCK_RET pPacketBlockRet = (PPACKET_BLOCK_RET)(pPacket + 1);
	pPacketBlockRet->nSyncId = nSyncId;
	pPacketBlockRet->dwIpAddress = m_dwLocalInterface;
	pPacketBlockRet->dwReceivedSize = m_dwReceivedSize;
	pPacketBlockRet->nRepairCount = nCount;
	memcpy(pPacketBlockRet->abBlockId, abBlock, nCount * sizeof(int));

	if (m_dwReceivedSize == 0) {
		// do NOT get any data of quiz file
		pPacketBlockRet->nRepairCount = -1;
	}

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CBlockRecverTransaction::SendBlockClose()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwSenderIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_BLOCK_CLOSE;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_BLOCK_CLOSE);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_BLOCK_CLOSE pPacketBlockClose = (PPACKET_BLOCK_CLOSE)(pPacket + 1);
	pPacketBlockClose->dwIpAddress = m_dwLocalInterface;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CBlockRecverTransaction::ReportBlockArrivedEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_BLOCK_ARRIVED);

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_BLCOK_ARRIVED;
	pHeader->dwLength = sizeof(EVENT_BLOCK_ARRIVED);

	PEVENT_BLOCK_ARRIVED pEvent = (PEVENT_BLOCK_ARRIVED)(pHeader + 1);
	pEvent->dwSenderIp = m_dwSenderIp;
	pEvent->dwBlockType = m_dwBlockType;
	pEvent->dwBlockSize = m_dwBlockSize;
	pEvent->pBlockBuffer = (BYTE *)malloc(m_dwBlockSize);
	if (!pEvent->pBlockBuffer)
		return;

	memcpy(pEvent->pBlockBuffer, m_pBlockBuffer, m_dwBlockSize);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CQuizBlockSenderTransaction::CQuizBlockSenderTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwLocalInterface = 0;
	m_dwReceiverIp = 0;
	m_dwBlockType = 0;
	m_pBlockBuffer = NULL;
	m_dwBlockSize = 0;

	m_nBlocks = 0;
	m_pRepairMap = NULL;

	m_nSyncId = 0;

	m_bSendFinished = FALSE;

	InitializeCriticalSection(&m_cs);
}

CQuizBlockSenderTransaction::~CQuizBlockSenderTransaction()
{
	if (m_pBlockBuffer) {
		free(m_pBlockBuffer);
		m_pBlockBuffer = NULL;
	}

	if (m_pRepairMap) {
		free(m_pRepairMap);
		m_pRepairMap = NULL;
	}

	DeleteCriticalSection(&m_cs);
}

BOOL CQuizBlockSenderTransaction::StartSendBlock(DWORD dwLocalInterface, DWORD dwReceiverIp, DWORD dwBlockType, BYTE* pbBuffer, DWORD cbSize)
{
	CAutoSectionLock lock(&m_cs);

	m_dwLocalInterface = dwLocalInterface;
	m_dwReceiverIp = dwReceiverIp;
	m_dwBlockType = dwBlockType;

	m_dwBlockSize = cbSize;
	m_pBlockBuffer = (BYTE *)malloc(cbSize);
	if (!m_pBlockBuffer)
		return FALSE;

	memcpy(m_pBlockBuffer, pbBuffer, m_dwBlockSize);

	m_nBlocks = (m_dwBlockSize + BASE_BLOCK_SIZE - 1) / BASE_BLOCK_SIZE;
	m_pRepairMap = (BYTE *)malloc(m_nBlocks);
	if (!m_pRepairMap)
		return FALSE;

	memset(m_pRepairMap, 1, m_nBlocks);

	CheckAndSendBlock();
	m_pComLayer->RegisterTimeout(m_pTransID, GetRandomSyncTime(), CQuizBlockSenderTransaction::ID_TIMER_SYNC);

	return TRUE;
}

void CQuizBlockSenderTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == CQuizBlockSenderTransaction::ID_TIMER_SYNC)
	{
		SendSyncPacket();
		m_pComLayer->RegisterTimeout(m_pTransID, GetRandomSyncTime(), CQuizBlockSenderTransaction::ID_TIMER_SYNC);
	}

	if (dwTimeID == CQuizBlockSenderTransaction::ID_TIMER_STOP)
	{
		SendStopPacket();
		m_pComLayer->RegisterTimeout(m_pTransID, GetRandomStopTime(), CQuizBlockSenderTransaction::ID_TIMER_STOP);
	}

	if (dwTimeID == CQuizBlockSenderTransaction::ID_TIMER_DESTROY)
	{
		m_pComLayer->UnregisterTimeout(m_pTransID, CQuizBlockSenderTransaction::ID_TIMER_STOP);
		Destroy();
	}
}

void CQuizBlockSenderTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_QUIZ_BLOCK_RET:
		{
			PPACKET_QUIZ_BLOCK_RET pBlockRet = (PPACKET_QUIZ_BLOCK_RET)(pPacket + 1);
			if (pBlockRet->nSyncId < m_nSyncId)
				return;

			m_pComLayer->UnregisterTimeout(m_pTransID, CQuizBlockSenderTransaction::ID_TIMER_SYNC);

			if (pBlockRet->nRepairCount == 0)
			{
				SendStopPacket();

				if (!m_bSendFinished) {

					ReportSendOutEvent();

					m_pComLayer->RegisterTimeout(m_pTransID, GetRandomStopTime(), CQuizBlockSenderTransaction::ID_TIMER_STOP);
					m_pComLayer->RegisterTimeout(m_pTransID, 60000, CQuizBlockSenderTransaction::ID_TIMER_DESTROY);
					m_bSendFinished = TRUE;
				}
			} else if (pBlockRet->nRepairCount == -1)
			{
				memset(m_pRepairMap, 1, m_nBlocks);
				CheckAndSendBlock();
				m_pComLayer->RegisterTimeout(m_pTransID, GetRandomSyncTime(), CQuizBlockSenderTransaction::ID_TIMER_SYNC);
			} else
			{
				for (int i=0; i<pBlockRet->nRepairCount; i++) {
					int nBlockId = pBlockRet->abBlockId[i];
					m_pRepairMap[nBlockId] = 1;
				}
				CheckAndSendBlock();
				m_pComLayer->RegisterTimeout(m_pTransID, GetRandomSyncTime(), CQuizBlockSenderTransaction::ID_TIMER_SYNC);
			}
		}
		break;
	case MAGIC_QUIZ_BLOCK_CLOSE:
		{
			m_pComLayer->UnregisterTimeout(m_pTransID, CQuizBlockSenderTransaction::ID_TIMER_STOP);
			m_pComLayer->UnregisterTimeout(m_pTransID, CQuizBlockSenderTransaction::ID_TIMER_DESTROY);
			Destroy();
		}
		break;
	default:
		break;
	}	
}

void CQuizBlockSenderTransaction::CheckAndSendBlock()
{
	for (int i=0; i<m_nBlocks; i++) {
		if (m_pRepairMap[i] == 1)
		{
			SendDataPacket(i);
			//Sleep(10);
		}
	}

	memset(m_pRepairMap, 0, m_nBlocks);

	m_nSyncId ++;
}

void CQuizBlockSenderTransaction::SendDataPacket(int nBlockId)
{
	BYTE* pData = m_pBlockBuffer + nBlockId * BASE_BLOCK_SIZE;
	DWORD cbSize = BASE_BLOCK_SIZE;
	if (nBlockId == m_nBlocks - 1) {
		cbSize = m_dwBlockSize - nBlockId * BASE_BLOCK_SIZE;
	}
	
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwReceiverIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_BLOCK;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_BLOCK) + cbSize;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZ_BLOCK pPacketBlock = (PPACKET_QUIZ_BLOCK)(pPacket + 1);
	pPacketBlock->dwFlags = BLOCK_DATA_PACKET;
	pPacketBlock->dwSenderIp = m_dwLocalInterface;
	pPacketBlock->dwReceiverIp = m_dwReceiverIp;
	pPacketBlock->dwBlockType = m_dwBlockType;
	pPacketBlock->dwBlockSize = m_dwBlockSize;
	pPacketBlock->nBlockId = nBlockId;
	pPacketBlock->nSyncId = m_nSyncId;
	pPacketBlock->cbSize = cbSize;
	memcpy(pPacketBlock->abBuffer, pData, cbSize);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CQuizBlockSenderTransaction::SendSyncPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwReceiverIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_BLOCK;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_BLOCK);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZ_BLOCK pPacketBlock = (PPACKET_QUIZ_BLOCK)(pPacket + 1);
	pPacketBlock->dwFlags = BLOCK_SYNC_PACKET;
	pPacketBlock->dwSenderIp = m_dwLocalInterface;
	pPacketBlock->dwReceiverIp = m_dwReceiverIp;
	pPacketBlock->dwBlockType = m_dwBlockType;
	pPacketBlock->dwBlockSize = m_dwBlockSize;
//	pPacketBlock->nBlockId;
	pPacketBlock->nSyncId = m_nSyncId;
	pPacketBlock->cbSize = 0;
//	pPacketBlock->abBuffer;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CQuizBlockSenderTransaction::SendStopPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwReceiverIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_BLOCK;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_BLOCK);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZ_BLOCK pPacketBlock = (PPACKET_QUIZ_BLOCK)(pPacket + 1);
	pPacketBlock->dwFlags = BLOCK_STOP_PACKET;
	pPacketBlock->dwSenderIp = m_dwLocalInterface;
	pPacketBlock->dwReceiverIp = m_dwReceiverIp;
	pPacketBlock->dwBlockType = m_dwBlockType;
	pPacketBlock->dwBlockSize = m_dwBlockSize;
//	pPacketBlock->nBlockId;
//	pPacketBlock->nSyncId;
	pPacketBlock->cbSize = 0;
//	pPacketBlock->abBuffer;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CQuizBlockSenderTransaction::ReportSendOutEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_QUIZ_BLOCK_SENDOUT);

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZ_BLOCK_SENDOUT;
	pHeader->dwLength = sizeof(EVENT_QUIZ_BLOCK_SENDOUT);

	PEVENT_QUIZ_BLOCK_SENDOUT pEvent = (PEVENT_QUIZ_BLOCK_SENDOUT)(pHeader + 1);
	pEvent->dwReceiverIp = m_dwReceiverIp;
	pEvent->dwBlockType = m_dwBlockType;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CQuizBlockSenderTransaction::Destroy()
{
	ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
	pTransactionMgr->DeleteTransaction(m_pTransID);
	pTransactionMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CQuizBlockRecverTransaction::CQuizBlockRecverTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwSenderIp = 0;
	m_dwLocalInterface = 0;
	m_dwBlockType = 0;
	m_dwBlockSize = 0;
	m_nBlocks = 0;
	m_dwReceivedSize = 0;

	m_pBlockBuffer = NULL;
	m_pRepairMap = NULL;

	m_bGotBlock = FALSE;

	InitializeCriticalSection(&m_cs);
}

CQuizBlockRecverTransaction::~CQuizBlockRecverTransaction()
{
	if (m_pBlockBuffer) {
		free(m_pBlockBuffer);
		m_pBlockBuffer = NULL;
	}

	if (m_pRepairMap) {
		free(m_pRepairMap);
		m_pRepairMap = NULL;
	}

	DeleteCriticalSection(&m_cs);
}

BOOL CQuizBlockRecverTransaction::ProcessBlock(PPACKET_QUIZ_BLOCK pPacketBlock)
{
	CAutoSectionLock lock(&m_cs);

	m_dwSenderIp = pPacketBlock->dwSenderIp;
	m_dwLocalInterface = pPacketBlock->dwReceiverIp;
	m_dwBlockType = pPacketBlock->dwBlockType;
	m_dwBlockSize = pPacketBlock->dwBlockSize;
	m_nBlocks = (m_dwBlockSize + BASE_BLOCK_SIZE - 1) / BASE_BLOCK_SIZE;

	m_pBlockBuffer = (BYTE *)malloc(m_dwBlockSize);
	if (!m_pBlockBuffer)
		return FALSE;

	m_pRepairMap = (BYTE *)malloc(m_nBlocks);
	if (!m_pRepairMap)
		return FALSE;

	memset(m_pRepairMap, 0, m_nBlocks);

	if (pPacketBlock->dwFlags == BLOCK_DATA_PACKET)
	{
		if (pPacketBlock->nBlockId >= m_nBlocks)
			return FALSE;

		memcpy(m_pBlockBuffer + pPacketBlock->nBlockId * BASE_BLOCK_SIZE, 
			pPacketBlock->abBuffer, pPacketBlock->cbSize);
		m_pRepairMap[pPacketBlock->nBlockId] = 1;
		m_dwReceivedSize += pPacketBlock->cbSize;

	} else if (pPacketBlock->dwFlags == BLOCK_SYNC_PACKET)
	{
		SendBlockRet(pPacketBlock->nSyncId);
	} else
	{
		OutputDebugString(_T("CQuizBlockRecverTransaction internal error. \n"));
	}

	return TRUE;
}

void CQuizBlockRecverTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == 0)
	{
		ITransactionManager* pTransactionMgr = m_pComLayer->GetTransactionManager();
		pTransactionMgr->DeleteTransaction(m_pTransID);
		pTransactionMgr->Release();
	}
}

void CQuizBlockRecverTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_pBlockBuffer || !m_pRepairMap)
		return;

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	if (pPacket->dwMagic != MAGIC_QUIZ_BLOCK)
		return;

	if (m_bGotBlock) {
		SendBlockClose();
		return;
	}

	PPACKET_QUIZ_BLOCK pPacketBlock = (PPACKET_QUIZ_BLOCK)(pPacket + 1);
	switch (pPacketBlock->dwFlags)
	{
	case BLOCK_DATA_PACKET:
		{
			if (pPacketBlock->nBlockId >= m_nBlocks)
				return;

			if (m_pRepairMap[pPacketBlock->nBlockId])
				return;

			memcpy(m_pBlockBuffer + pPacketBlock->nBlockId * BASE_BLOCK_SIZE, 
				pPacketBlock->abBuffer, pPacketBlock->cbSize);
			m_pRepairMap[pPacketBlock->nBlockId] = 1;
			m_dwReceivedSize += pPacketBlock->cbSize;

			if (m_dwReceivedSize >= m_dwBlockSize) {
				SendBlockRet(pPacketBlock->nSyncId + 1);
			}
		}
		break;
	case BLOCK_SYNC_PACKET:
		{
			SendBlockRet(pPacketBlock->nSyncId);
		}
		break;
	case BLOCK_STOP_PACKET:
		{
			SendBlockClose();

			if (!m_bGotBlock) {
				ReportBlockArrivedEvent();
				m_pComLayer->RegisterTimeout(m_pTransID, 60000, 0);
				m_bGotBlock = TRUE;
			}
		}
		break;
	default:
		break;
	}
}

void CQuizBlockRecverTransaction::SendBlockRet(int nSyncId)
{
	int nCount = 0;
	int abBlock[256] = {0};

	if ((m_dwReceivedSize == 0) || (m_dwReceivedSize >= m_dwBlockSize)) {
	} else
	{
		for (int i=0; i<m_nBlocks; i++) {
			if (!m_pRepairMap[i]) {
				abBlock[nCount ++] = i;
				if (nCount >= 256)
					break;
			}
		}
	}

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwSenderIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_BLOCK_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_BLOCK_RET) + nCount * sizeof(int);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZ_BLOCK_RET pPacketBlockRet = (PPACKET_QUIZ_BLOCK_RET)(pPacket + 1);
	pPacketBlockRet->nSyncId = nSyncId;
	pPacketBlockRet->dwIpAddress = m_dwLocalInterface;
	pPacketBlockRet->dwReceivedSize = m_dwReceivedSize;
	pPacketBlockRet->nRepairCount = nCount;
	memcpy(pPacketBlockRet->abBlockId, abBlock, nCount * sizeof(int));

	if (m_dwReceivedSize == 0) {
		// do NOT get any data of quiz file
		pPacketBlockRet->nRepairCount = -1;
	}

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CQuizBlockRecverTransaction::SendBlockClose()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwSenderIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_BLOCK_CLOSE;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_BLOCK_CLOSE);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZ_BLOCK_CLOSE pPacketBlockClose = (PPACKET_QUIZ_BLOCK_CLOSE)(pPacket + 1);
	pPacketBlockClose->dwIpAddress = m_dwLocalInterface;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CQuizBlockRecverTransaction::ReportBlockArrivedEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_QUIZ_BLOCK_ARRIVED);

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZ_BLOCK_ARRIVED;
	pHeader->dwLength = sizeof(EVENT_QUIZ_BLOCK_ARRIVED);

	PEVENT_QUIZ_BLOCK_ARRIVED pEvent = (PEVENT_QUIZ_BLOCK_ARRIVED)(pHeader + 1);
	pEvent->dwSenderIp = m_dwSenderIp;
	pEvent->dwBlockType = m_dwBlockType;
	pEvent->dwBlockSize = m_dwBlockSize;
	pEvent->pBlockBuffer = (BYTE *)malloc(m_dwBlockSize);
	if (!pEvent->pBlockBuffer)
		return;

	memcpy(pEvent->pBlockBuffer, m_pBlockBuffer, m_dwBlockSize);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CTestServerTransaction::CTestServerTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);
}

CTestServerTransaction::~CTestServerTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CTestServerTransaction::Start(DWORD dwLocalInterface)
{
	CAutoSectionLock lock(&m_cs);
}

void CTestServerTransaction::Stop()
{
	CAutoSectionLock lock(&m_cs);
	m_connectMap.clear();
}

void CTestServerTransaction::SendPingRet(const LPBYTE lpData, DWORD cbSize, DWORD dwClientIP)
{
	CAutoSectionLock lock(&m_cs);

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwClientIP, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_PING_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_PING_RET) + cbSize;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZ_PING_RET pPacketPingRet = (PPACKET_QUIZ_PING_RET)(pPacket + 1);
	pPacketPingRet->dwIpAddress = dwClientIP;
	pPacketPingRet->cbSize = cbSize;
	memcpy(pPacketPingRet->abBuffer, lpData, cbSize);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CTestServerTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_QUIZ_CONNECT:
		{
			if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_QUIZ_CONNECT))
				return;

			PPACKET_QUIZ_CONNECT pPacketConnect = (PPACKET_QUIZ_CONNECT)(pPacket + 1);
			CConnectMap::iterator it = m_connectMap.find(pPacketConnect->dwIpAddress);
			if (it == m_connectMap.end()) {
				DWORD dwLastPingTime = GetTickCount();
				m_connectMap.insert(CConnectMap::value_type(pPacketConnect->dwIpAddress, dwLastPingTime));
			}

			SendConnectRetPacket(pPacketConnect);
		}
		break;
	case MAGIC_QUIZ_PING:
		{
			if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_QUIZ_PING))
				return;
				
			PPACKET_QUIZ_PING pPacketPing = (PPACKET_QUIZ_PING)(pPacket + 1);
			CConnectMap::iterator it = m_connectMap.find(pPacketPing->dwIpAddress);
			if (it != m_connectMap.end()) {
				(*it).second = GetTickCount();
			}
			
			ReportPingEvent(pPacketPing);
		}
		break;
	default:
		break;
	}
}

void CTestServerTransaction::SendConnectRetPacket(PPACKET_QUIZ_CONNECT pPacketConnect)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(pPacketConnect->dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_CONNECT_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_CONNECT_RET);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZ_CONNECT_RET pPacketConnectRet = (PPACKET_QUIZ_CONNECT_RET)(pPacket + 1);
	pPacketConnectRet->dwIpAddress = pPacketConnect->dwIpAddress;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CTestServerTransaction::ReportPingEvent(PPACKET_QUIZ_PING pPacketPing)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_QUIZ_PING);
	cbEvent += pPacketPing->cbSize;

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZ_PING;
	pHeader->dwLength = sizeof(EVENT_QUIZ_PING) + pPacketPing->cbSize;

	PEVENT_QUIZ_PING pEvent = (PEVENT_QUIZ_PING)(pHeader + 1);
	pEvent->dwIpAddress = pPacketPing->dwIpAddress;
	memcpy(pEvent->abMac, pPacketPing->abMac, 6);
	pEvent->cbSize = pPacketPing->cbSize;
	memcpy(pEvent->abBuffer, pPacketPing->abBuffer, pPacketPing->cbSize);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
#define MAX_CONNECT_COUNT		120

CTestClientTransaction::CTestClientTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_bConnected = FALSE;
	m_bGrade = FALSE;
	m_dwRetryCount = 0;
	m_dwTeacherIp = 0;
	m_dwLocalInterface = 0;
	memset(m_abMac, 0, 6);
	m_cbPing = 0;

	InitializeCriticalSection(&m_cs);
}

CTestClientTransaction::~CTestClientTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CTestClientTransaction::Start(DWORD dwTeacherIp, DWORD dwLocalInterface, BYTE* pbMac, BOOL bGrade)
{
	CAutoSectionLock lock(&m_cs);

	m_bGrade = bGrade;
	m_dwTeacherIp = dwTeacherIp;
	m_dwLocalInterface = dwLocalInterface;
	memcpy(m_abMac, pbMac, 6);
	
	if (m_bGrade) {
		SendPingPacket();
		m_pComLayer->RegisterTimeout(m_pTransID, GetRandomPingTime(), 1);
	} else
	{
		SendConnectPacket();
		m_pComLayer->RegisterTimeout(m_pTransID, GetRandomConnectTime(), 0);
	}
}

void CTestClientTransaction::Stop()
{
	CAutoSectionLock lock(&m_cs);

	m_bConnected = FALSE;
	m_pComLayer->UnregisterTimeout(m_pTransID, 0);
	m_pComLayer->UnregisterTimeout(m_pTransID, 1);
}

void CTestClientTransaction::SetPingBuffer(const LPBYTE lpBuffer, DWORD cbSize)
{
	CAutoSectionLock lock(&m_cs);
	
	m_cbPing = (cbSize <= 1024) ? cbSize : 1024;
	memcpy(m_abPing, lpBuffer, m_cbPing);
}

void CTestClientTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if ((dwTimeID == 0) && (!m_bConnected))
	{
		if (m_dwRetryCount > MAX_CONNECT_COUNT) 
		{
			ReportConnectFailedEvent();
		} else
		{
			m_dwRetryCount ++;
			SendConnectPacket();
			m_pComLayer->RegisterTimeout(m_pTransID, GetRandomConnectTime(), 0);
		}
	}

	if ((dwTimeID == 1) && (m_bConnected || m_bGrade))
	{
		SendPingPacket();
		m_pComLayer->RegisterTimeout(m_pTransID, GetRandomPingTime(), 1);
	}
}

void CTestClientTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);
	
	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_QUIZ_PING_RET:
		{
			if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_QUIZ_PING_RET))
				return;

			PPACKET_QUIZ_PING_RET pPingRet = (PPACKET_QUIZ_PING_RET)(pPacket + 1);
			if (pPingRet->dwIpAddress != m_dwLocalInterface)
				return;

			ReportPingRetEvent(pPingRet);
		}
		break;		
	case MAGIC_QUIZ_CONNECT_RET:
		{
			if (m_bConnected)
				return;

			if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_QUIZ_CONNECT_RET))
				return;

			PPACKET_QUIZ_CONNECT_RET pPacketRet = (PPACKET_QUIZ_CONNECT_RET)(pPacket + 1);
			if (pPacketRet->dwIpAddress == m_dwLocalInterface) {
				m_bConnected = TRUE;
				m_pComLayer->UnregisterTimeout(m_pTransID, 0);

				ReportConnectSuccessEvent();
				
				SendPingPacket();
				m_pComLayer->RegisterTimeout(m_pTransID, GetRandomPingTime(), 1);
			}
		}
		break;
	default:
		break;
	}
}

void CTestClientTransaction::SendConnectPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_CONNECT;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_CONNECT);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZ_CONNECT pPacketConnect = (PPACKET_QUIZ_CONNECT)(pPacket + 1);
	pPacketConnect->dwIpAddress = m_dwLocalInterface;
	memcpy(pPacketConnect->abMac, m_abMac, 6);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CTestClientTransaction::SendPingPacket()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_PING;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZ_PING) + m_cbPing;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZ_PING pPacketPing = (PPACKET_QUIZ_PING)(pPacket + 1);
	pPacketPing->dwIpAddress = m_dwLocalInterface;
	memcpy(pPacketPing->abMac, m_abMac, 6);
	pPacketPing->cbSize = m_cbPing;
	memcpy(pPacketPing->abBuffer, m_abPing, m_cbPing);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CTestClientTransaction::ReportPingRetEvent(PPACKET_QUIZ_PING_RET pPingRet)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_QUIZ_PING_RET);
	cbEvent += pPingRet->cbSize;

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZ_PING_RET;
	pHeader->dwLength = sizeof(EVENT_QUIZ_PING_RET) + pPingRet->cbSize;

	PEVENT_QUIZ_PING_RET pEvent = (PEVENT_QUIZ_PING_RET)(pHeader + 1);
	pEvent->cbSize = pPingRet->cbSize;
	memcpy(pEvent->abBuffer, pPingRet->abBuffer, pPingRet->cbSize);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CTestClientTransaction::ReportConnectSuccessEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZ_CONNECT_SUCCESS;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CTestClientTransaction::ReportConnectFailedEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZ_CONNECT_FAILED;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

#define BASE_BLOCK_SIZE		1024

CQuizFileSenderTransaction::CQuizFileSenderTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwLocalInterface = 0;

	m_dwFileSize = 0;
	m_pFileBuffer = NULL;
	m_nBlocks = 0;
	m_pRepairMap = NULL;
	m_bStarted = FALSE;

	InitializeCriticalSection(&m_cs);
}

CQuizFileSenderTransaction::~CQuizFileSenderTransaction()
{
	DeleteCriticalSection(&m_cs);
}

BOOL CQuizFileSenderTransaction::Start(DWORD dwLocalInterface, DWORD* pdwIpAddress, int nCount, LPCTSTR lpFileName)
{
	{
		CAutoSectionLock lock(&m_cs);
		
		m_dwLocalInterface = dwLocalInterface;

		m_listReceivers.clear();
		for (int i=0; i<nCount; i++) {
			m_listReceivers.insert(CReceiveList::value_type(pdwIpAddress[i], 0));
		}

		HANDLE hFile = NULL;

		do
		{
			hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
				OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
	
			if (hFile == INVALID_HANDLE_VALUE) 
			{
				hFile = NULL;
				break;
			}

			m_dwFileSize = GetFileSize(hFile, NULL);
			m_nBlocks = (m_dwFileSize + BASE_BLOCK_SIZE - 1) / BASE_BLOCK_SIZE;
			m_pRepairMap = (BYTE *)malloc(m_nBlocks);
			if (!m_pRepairMap)
				break;

			m_pFileBuffer = (BYTE *)malloc(m_dwFileSize);
			if (!m_pFileBuffer)
				break;

			memset(m_pRepairMap, 1, m_nBlocks);

			DWORD dwBytesReaded;
			if (!ReadFile(hFile, m_pFileBuffer, m_dwFileSize, &dwBytesReaded, NULL)) {
				OutputDebugString(_T("CQuizFileSenderTransaction::Start ReadFile failed!\n"));
			}

			if (dwBytesReaded != m_dwFileSize)
				break;

			CloseHandle(hFile);
			hFile = NULL;

			// Start send after 100ms
			m_pComLayer->RegisterTimeout(m_pTransID, 100, 0);

			m_bStarted = TRUE;

			return TRUE;
		} while (false);

		if (hFile) {
			CloseHandle(hFile);
			hFile = NULL;
		}
	}

	Stop();

	return FALSE;
}

void CQuizFileSenderTransaction::Stop()
{
	CAutoSectionLock lock(&m_cs);

	m_pComLayer->UnregisterTimeout(m_pTransID, 0);

	if (m_pFileBuffer) {
		free(m_pFileBuffer);
		m_pFileBuffer = NULL;
	}

	if (m_pRepairMap) {
		free(m_pRepairMap);
		m_pRepairMap = NULL;
	}

	m_bStarted = FALSE;
}

void CQuizFileSenderTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (dwTimeID == 0)
	{
		CheckAndSendQuizFile();

		// send again after 2 seconds
		m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);
	}
}

void CQuizFileSenderTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_QUIZ_DATA_RET:
		{
			if (cbData - sizeof(LANGLAB_PACKET) < sizeof(PACKET_QUIZDATA_RET))
				return;

			PPACKET_QUIZDATA_RET pQuizDataRet = (PPACKET_QUIZDATA_RET)(pPacket + 1);

			CReceiveList::iterator it = m_listReceivers.find(pQuizDataRet->dwIpAddress);
			if (it != m_listReceivers.end()) {
				if ((*it).second != pQuizDataRet->dwReceivedSize) {
					(*it).second = pQuizDataRet->dwReceivedSize;
					ReportQuizFileProgressEvent(pQuizDataRet);
				}
			}

			if (pQuizDataRet->nRepairCount == 0)
			{
				if (it != m_listReceivers.end()) {
					m_listReceivers.erase(it);
				}
				SendConfirm(pQuizDataRet->dwIpAddress);
			} else if (pQuizDataRet->nRepairCount == -1)
			{
				memset(m_pRepairMap, 1, m_nBlocks);
			} else
			{
				for (int i=0; i<pQuizDataRet->nRepairCount; i++) {
					int nBlockId = pQuizDataRet->abBlockId[i];
					if (nBlockId < m_nBlocks) {
						m_pRepairMap[nBlockId] = 1;
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

void CQuizFileSenderTransaction::CheckAndSendQuizFile()
{
	int nCount = 0;
	for (int i=0; i<m_nBlocks; i++) 
	{
		if (m_pRepairMap[i] == 1)
		{
			SendData(i);
			m_pRepairMap[i] = 0;

			nCount ++;
			if (nCount >= 100)
				break;
		}
	}

//	memset(m_pRepairMap, 0, m_nBlocks);
}

void CQuizFileSenderTransaction::SendData(int nBlockId)
{
	BYTE* pData = m_pFileBuffer + nBlockId * BASE_BLOCK_SIZE;
	DWORD cbSize = BASE_BLOCK_SIZE;
	if (nBlockId == m_nBlocks - 1) {
		cbSize = m_dwFileSize - nBlockId * BASE_BLOCK_SIZE;
	}
	
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_pComLayer->GetComAddress(), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_DATA;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZDATA) + cbSize;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZDATA pPacketQuizData = (PPACKET_QUIZDATA)(pPacket + 1);
	pPacketQuizData->nBlockId = nBlockId;
	pPacketQuizData->cbSize = cbSize;
	memcpy(pPacketQuizData->abBuffer, pData, cbSize);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CQuizFileSenderTransaction::SendConfirm(DWORD dwIpAddress)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_DATA_CONFIRM;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PACKET_QUIZDATA_CONFIRM);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZDATA_CONFIRM pPacketQuizDataConfirm = (PPACKET_QUIZDATA_CONFIRM)(pPacket + 1);
	pPacketQuizDataConfirm->dwIpAddress = dwIpAddress;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CQuizFileSenderTransaction::ReportQuizFileProgressEvent(PPACKET_QUIZDATA_RET pQuizDataRet)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_QUIZFILE_PROGRESS);

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZFILE_PROGRESS;
	pHeader->dwLength = sizeof(EVENT_QUIZFILE_PROGRESS);

	PEVENT_QUIZFILE_PROGRESS pEvent = (PEVENT_QUIZFILE_PROGRESS)(pHeader + 1);
	pEvent->dwIpAddress = pQuizDataRet->dwIpAddress;
	memcpy(pEvent->abMac, pQuizDataRet->abMac, 6);
	pEvent->dwFileSize = m_dwFileSize;
	pEvent->dwReceivedSize = pQuizDataRet->dwReceivedSize;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CQuizFileRecverTransaction::CQuizFileRecverTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwTeacherIp = 0;
	m_dwLocalInterface = 0;

	m_hFile = NULL;
	m_pFileBuffer = NULL;
	m_nBlocks = 0;
	m_pRepairMap = NULL;
	m_dwFileSize = 0;
	m_dwReceivedSize = 0;

	m_bDownload = FALSE;

	InitializeCriticalSection(&m_cs);
}

CQuizFileRecverTransaction::~CQuizFileRecverTransaction()
{
	DeleteCriticalSection(&m_cs);
}

BOOL CQuizFileRecverTransaction::Start(DWORD dwTeacherIp, DWORD dwLocalInterface, LPCTSTR lpFileName, DWORD dwFileSize)
{
	{
		CAutoSectionLock lock(&m_cs);

		m_dwTeacherIp = dwTeacherIp;
		m_dwLocalInterface = dwLocalInterface;
		m_dwFileSize = dwFileSize;

		do
		{
			m_hFile = CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);

			if (m_hFile == INVALID_HANDLE_VALUE) 
			{
				SetFileAttributes(lpFileName, FILE_ATTRIBUTE_ARCHIVE);
				DeleteFile(lpFileName);

				m_hFile = CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
					CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);

				if (m_hFile == INVALID_HANDLE_VALUE) 
				{
					m_hFile = NULL;
					break;
				}
			}

			m_nBlocks = (dwFileSize + BASE_BLOCK_SIZE - 1) / BASE_BLOCK_SIZE;
			m_pRepairMap = (BYTE *)malloc(m_nBlocks);
			if (!m_pRepairMap)
				break;

			memset(m_pRepairMap, 0, m_nBlocks);

			m_pFileBuffer = (BYTE *)malloc(dwFileSize);
			if (!m_pFileBuffer)
				break;

			m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);

			m_bDownload = TRUE;

			return TRUE;
		} while (false);
	}

	Stop();

	return FALSE;
}

void CQuizFileRecverTransaction::Stop()
{
	CAutoSectionLock lock(&m_cs);

	m_pComLayer->UnregisterTimeout(m_pTransID, 0);

	if (m_hFile) {
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	if (m_pFileBuffer) {
		free(m_pFileBuffer);
		m_pFileBuffer = NULL;
	}

	if (m_pRepairMap) {
		free(m_pRepairMap);
		m_pRepairMap = NULL;
	}

	m_bDownload = FALSE;

	m_nBlocks = 0;
	m_dwFileSize = 0;
	m_dwReceivedSize = 0;
}

void CQuizFileRecverTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bDownload)
		return;

	ReportQuizFileProgressEvent();
	SendQuizDataRetPacket();
	m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);
}

void CQuizFileRecverTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bDownload)
		return;

	if (cbData < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_QUIZ_DATA:
		{
			PPACKET_QUIZDATA pQuizData = (PPACKET_QUIZDATA)(pPacket + 1);
			if (pQuizData->nBlockId >= m_nBlocks)
				return;

			if (m_pRepairMap[pQuizData->nBlockId] == 1)
				return;

			m_pRepairMap[pQuizData->nBlockId] = 1;
			memcpy(m_pFileBuffer + pQuizData->nBlockId * BASE_BLOCK_SIZE, 
				pQuizData->abBuffer, pQuizData->cbSize);
			m_dwReceivedSize += pQuizData->cbSize;

			if (m_dwReceivedSize == m_dwFileSize) {
				ReportQuizFileProgressEvent();
				SendQuizDataRetPacket();
			}
		}
		break;
	case MAGIC_QUIZ_DATA_CONFIRM:
		{
			PPACKET_QUIZDATA_CONFIRM pQuizDataConfirm = (PPACKET_QUIZDATA_CONFIRM)(pPacket + 1);
			if (pQuizDataConfirm->dwIpAddress != m_dwLocalInterface)
				return;

			m_pComLayer->UnregisterTimeout(m_pTransID, 0);

			DWORD dwBytesWriten = 0;
			if (!WriteFile(m_hFile, m_pFileBuffer, m_dwFileSize, &dwBytesWriten, NULL)) 
			{
				ReportQuizFileFailedEvent();
			}

			if (dwBytesWriten != m_dwFileSize)
			{
				ReportQuizFileFailedEvent();
			} else
			{
				ReportQuizFileSuccessEvent();
			}
				
			CloseHandle(m_hFile);
			m_hFile = NULL;

			m_dwReceivedSize = 0;//ywm add

			m_bDownload = FALSE;
		}
		break;
	default:
		break;
	}
}

void CQuizFileRecverTransaction::SendQuizDataRetPacket()
{
	CRepairList repairList;

	if ((m_dwReceivedSize == 0) || (m_dwReceivedSize >= m_dwFileSize)) {

	} else
	{
		for (int i=0; i<m_nBlocks; i++) {
			if (!m_pRepairMap[i]) 
			{
				repairList.push_back(i);
				if (repairList.size() >= 256)
					break;
			}
		}
	}

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;
		
	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_QUIZ_DATA_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = (DWORD)(sizeof(PACKET_QUIZDATA_RET) + repairList.size() * sizeof(int));
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PPACKET_QUIZDATA_RET pPacketQuizDataRet = (PPACKET_QUIZDATA_RET)(pPacket + 1);
	pPacketQuizDataRet->dwIpAddress = m_dwLocalInterface;
	//memcpy(pPacketQuizDataRet->abMac, m_abMac, 6);
	pPacketQuizDataRet->dwReceivedSize = m_dwReceivedSize;
	pPacketQuizDataRet->nRepairCount = (int)repairList.size();

	int i=0;
	CRepairList::iterator it = repairList.begin();
	while (it != repairList.end()) {
		pPacketQuizDataRet->abBlockId[i++] = *it;
		it ++;
	}

	if (m_dwReceivedSize == 0) {
		// do NOT get any data of quiz file
		pPacketQuizDataRet->nRepairCount = -1;
	}

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CQuizFileRecverTransaction::ReportQuizFileFailedEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZFILE_FAILED;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CQuizFileRecverTransaction::ReportQuizFileSuccessEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZFILE_SUCCESS;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CQuizFileRecverTransaction::ReportQuizFileProgressEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_QUIZFILE_PROGRESS);

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_QUIZFILE_PROGRESS;
	pHeader->dwLength = sizeof(EVENT_QUIZFILE_PROGRESS);

	PEVENT_QUIZFILE_PROGRESS pEvent = (PEVENT_QUIZFILE_PROGRESS)(pHeader + 1);
	pEvent->dwIpAddress = m_dwLocalInterface;
//	memcpy(pEvent->abMac, m_abMac, 6);
	pEvent->dwFileSize = m_dwFileSize;
	pEvent->dwReceivedSize = m_dwReceivedSize;
	
	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

#define TIMEOUT_ASK_GROUPID		2000

#define TIMEOUT_ANNOUNCE		3000
#define TIMEOUT_CHECK_STATE		20000

#define TIMER_ANNOUNCE			1
#define TIMER_CHECK_STATE		2

//////////////////////////////////////////////////
// CAskGroupIdActiveTransaction
////////////////////////////////////////////////

CAskGroupIdActiveTransaction::CAskGroupIdActiveTransaction(ICommunicationLayer* pComLayer, GUID *pTransID)
	: CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);
}

CAskGroupIdActiveTransaction::~CAskGroupIdActiveTransaction(void)
{
	DeleteCriticalSection(&m_cs);

	// Clear all group info list
	CAskGroupMap::iterator iterator = m_askMap.begin();
	while (iterator != m_askMap.end()) {
		PANSWERGROUPID pGroupInfo = (*iterator).second;
		m_askMap.erase(iterator);
		free(pGroupInfo);
		iterator = m_askMap.begin();
	}
}

void CAskGroupIdActiveTransaction::SetGroupMap(DWORD dwIpAddress, DWORD dwGroupId, LPCTSTR pszGroupName, LPCTSTR pszName, WORD wVoicePort, DWORD dwState, UINT64 nCommand)
{
	CAutoSectionLock lock(&m_cs);
	
	CAskGroupMap::iterator iterator = m_askMap.find(dwIpAddress);
	if (iterator == m_askMap.end())
	{
		PANSWERGROUPID pGroupInfo = (PANSWERGROUPID)malloc(sizeof(ANSWERGROUPID));
		if (!pGroupInfo)
			return;

		pGroupInfo->dwGroupId = dwGroupId;
		pGroupInfo->dwState = dwState;
		pGroupInfo->wVoicePort = wVoicePort;

		_tcscpy(pGroupInfo->szGroupName, pszGroupName);
		_tcscpy(pGroupInfo->szName, pszName);

		pGroupInfo->nCommand = nCommand;

		m_askMap.insert(CAskGroupMap::value_type(dwIpAddress, pGroupInfo));
	}
	else
	{
		PANSWERGROUPID pGroupInfo = (*iterator).second;

		if (pszGroupName)
			_tcscpy(pGroupInfo->szGroupName, pszGroupName);
		if (pszName)
			_tcscpy(pGroupInfo->szName, pszName);

		pGroupInfo->dwGroupId = dwGroupId;
		pGroupInfo->dwState = dwState;
		pGroupInfo->wVoicePort = wVoicePort;

		pGroupInfo->nCommand = nCommand;
	}
}

void CAskGroupIdActiveTransaction::DelGroupMap(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	CAskGroupMap::iterator iterator = m_askMap.find(dwIpAddress);
	if (iterator != m_askMap.end()) {
		free((*iterator).second);
		m_askMap.erase(iterator);
	}
}

DWORD CAskGroupIdActiveTransaction::FindMemberGroupId(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	CAskGroupMap::iterator iterator = m_askMap.find(dwIpAddress);
	if (iterator != m_askMap.end()) {
		return (*iterator).second->dwGroupId;
	}

	return 0;
}

DWORD CAskGroupIdActiveTransaction::FindMemberState(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	CAskGroupMap::iterator iterator = m_askMap.find(dwIpAddress);
	if (iterator != m_askMap.end()) {
		return (*iterator).second->dwState;
	}

	return 0;
}

UINT64 CAskGroupIdActiveTransaction::FindMemberCommand(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	CAskGroupMap::iterator iterator = m_askMap.find(dwIpAddress);
	if (iterator != m_askMap.end()) {
		return (*iterator).second->nCommand;
	}

	return 0;
}

void CAskGroupIdActiveTransaction::ModifyMemberState(DWORD dwIpAddress, DWORD dwState)
{
	CAutoSectionLock lock(&m_cs);

	CAskGroupMap::iterator iterator = m_askMap.find(dwIpAddress);
	if (iterator != m_askMap.end()) {
		(*iterator).second->dwState = dwState;
	}
}

void CAskGroupIdActiveTransaction::ModifyMemberState(DWORD dwIpAddress, DWORD dwMask, DWORD dwState)
{
	CAutoSectionLock lock(&m_cs);

	CAskGroupMap::iterator iterator = m_askMap.find(dwIpAddress);
	if (iterator != m_askMap.end()) {
		(*iterator).second->dwState &= ~dwMask;
		(*iterator).second->dwState |= dwState;
	}
}

void CAskGroupIdActiveTransaction::ModifyGroupName(DWORD dwIpAddress, LPCTSTR lpszName)
{
	CAutoSectionLock lock(&m_cs);

	CAskGroupMap::iterator iterator = m_askMap.find(dwIpAddress);
	if (iterator != m_askMap.end()) {
		_tcscpy((*iterator).second->szGroupName, lpszName);
	}
}

void CAskGroupIdActiveTransaction::ModifyMemberName(DWORD dwIpAddress, LPCTSTR lpszName)
{
	CAutoSectionLock lock(&m_cs);

	CAskGroupMap::iterator iterator = m_askMap.find(dwIpAddress);
	if (iterator != m_askMap.end()) {
		_tcscpy((*iterator).second->szName, lpszName);
	}
}

void CAskGroupIdActiveTransaction::ModifyMemberCommand(DWORD dwIpAddress, UINT64 nCommand)
{
	CAutoSectionLock lock(&m_cs);

	CAskGroupMap::iterator iterator = m_askMap.find(dwIpAddress);
	if (iterator != m_askMap.end()) {
		(*iterator).second->nCommand = nCommand;
	}
}

void CAskGroupIdActiveTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	if (pPacket->dwMagic != MAGIC_ASKGROUPID)
		return;

	PASKGROUPID pAskPacket = (PASKGROUPID)(pPacket + 1);
	{
		CAutoSectionLock lock(&m_cs);

		CAskGroupMap::iterator iterator = m_askMap.find(pAskPacket->dwIpAddress);
		if (iterator != m_askMap.end())
		{
			SendGroupIdReply(pAskPacket->dwIpAddress, (*iterator).second);
		}
	}
}

void CAskGroupIdActiveTransaction::SendGroupIdReply(DWORD dwIpAddress, PANSWERGROUPID pGroupInfo)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	pPacket->dwMagic = MAGIC_ANSWERGROUPID;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(ANSWERGROUPID);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	PANSWERGROUPID pAnswerkGroupPacket = (PANSWERGROUPID)(pPacket + 1);
	memcpy(pAnswerkGroupPacket, pGroupInfo, sizeof(ANSWERGROUPID));

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

//////////////////////////////////////////////////
// CAskGroupIdPassiveTransaction
////////////////////////////////////////////////

CAskGroupIdPassiveTransaction::CAskGroupIdPassiveTransaction(ICommunicationLayer* pComLayer, GUID *pTransID)
	: CTransactionHandler(pComLayer, pTransID)
{
	m_dwLocalInterface = 0;
	m_dwTeacherIp = 0;
}

CAskGroupIdPassiveTransaction::~CAskGroupIdPassiveTransaction(void)
{

}

void CAskGroupIdPassiveTransaction::BeginAskGroupId(DWORD dwLocalInterface, DWORD dwTeacherIp)
{
	m_dwLocalInterface = dwLocalInterface;
	m_dwTeacherIp = dwTeacherIp;
	SendAskGroupIdPacket(dwLocalInterface, dwTeacherIp);
	m_pComLayer->RegisterTimeout(m_pTransID, TIMEOUT_ASK_GROUPID, 0);
}

void CAskGroupIdPassiveTransaction::OnTimeout(DWORD dwTimeID)
{
	SendAskGroupIdPacket(m_dwLocalInterface, m_dwTeacherIp);
	m_pComLayer->RegisterTimeout(m_pTransID, TIMEOUT_ASK_GROUPID, 0);
}

void CAskGroupIdPassiveTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	if (pPacket->dwMagic != MAGIC_ANSWERGROUPID)
		return;

	// Unregister timeout
	m_pComLayer->UnregisterTimeout(m_pTransID, 0);

	// Report groupid event
	PANSWERGROUPID pAnswerPacket = (PANSWERGROUPID)(pPacket + 1);
	ReportGroupIdEvent(pAnswerPacket);
}

void CAskGroupIdPassiveTransaction::SendAskGroupIdPacket(DWORD dwLocalInterface, DWORD dwTeacherIp)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PASKGROUPID pAskGroupPacket = (PASKGROUPID)(pPacket + 1);

	pPacket->dwMagic = MAGIC_ASKGROUPID;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(ASKGROUPID);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pAskGroupPacket->dwIpAddress = dwLocalInterface;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CAskGroupIdPassiveTransaction::ReportGroupIdEvent(PANSWERGROUPID pAnswerGroupId)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_ASKGROUPID_RET);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_ASKGROUPID_RET;
	pHeader->dwLength = sizeof(EVENT_ASKGROUPID_RET);

	PEVENT_ASKGROUPID_RET pEvent = (PEVENT_ASKGROUPID_RET)(pHeader + 1);
	memcpy(pEvent, pAnswerGroupId, sizeof(ANSWERGROUPID));

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

///////////////////////////////////
///CGroupMemberTransaction
////////////////////////////////////

#define MEMBER_STATE_NORMAL			0x00000000
#define MEMBER_STATE_TEACHER		0x00000001
#define MEMBER_STATE_MUTE			0x00000002
#define MEMBER_STATE_LEADER			0x00000004
#define MEMBER_STATE_BLACK_SCREEN	0x00000010

CGroupMemberTransaction::CGroupMemberTransaction(ICommunicationLayer* pComLayer, GUID *pTransID)
	: CTransactionHandler(pComLayer, pTransID)
{
	m_bTeacher = FALSE;
	m_dwGroupId = 0;
	m_bLeader = FALSE;
	m_bMute	= FALSE;
	m_bSilent = FALSE;

	m_nCommand = 0;

	m_dwLocalInterface = 0;

	memset(m_abMac, 0, 6);
	memset(m_szName, 0, sizeof(m_szName));

	m_bInGroup = FALSE;
	m_nAnnounceCount = 0;

	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_csAnnounce);
}

CGroupMemberTransaction::~CGroupMemberTransaction(void)
{
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_csAnnounce);

	// Clear all chat member list
	CChatMemberMap::iterator iterator = m_GroupMemberMap.begin();
	while (iterator != m_GroupMemberMap.end()) {
		PCHATMEMBERINFO pMemberInfo = (*iterator).second;
		m_GroupMemberMap.erase(iterator);
		free(pMemberInfo);
		iterator = m_GroupMemberMap.begin();
	}
}

void CGroupMemberTransaction::JoinGroup(DWORD dwIpAddress, DWORD dwGroupId,
	TCHAR* pszName, BYTE* pabMac, DWORD dwState/* = 0*/, UINT64 nCommand/* = 0*/)
{
	if (pabMac)
		memcpy(m_abMac, pabMac, 6);

	m_dwLocalInterface = dwIpAddress;
	m_bTeacher = (dwState & MEMBER_STATE_TEACHER) ? TRUE : FALSE;
	m_bLeader = (dwState & MEMBER_STATE_LEADER) ? TRUE : FALSE;
	m_dwGroupId = dwGroupId;
	//m_nAnnounceCount = 0;
	_TRACE(_T("JionGroup, GroupId: %d\n"), m_dwGroupId);

	_tcscpy(m_szName, pszName);

	m_nCommand = nCommand;

	SendJoinGroupPacket(dwIpAddress, dwGroupId, pszName, m_abMac, dwState, m_nCommand);

	// Set Announce Time
	//m_pComLayer->UnregisterTimeout(m_pTransID, TIMER_ANNOUNCE);
	//m_pComLayer->RegisterTimeout(m_pTransID, 100, TIMER_ANNOUNCE);
	ResetAnnounce();

	// Set Check Time
	m_pComLayer->RegisterTimeout(m_pTransID, TIMEOUT_CHECK_STATE, TIMER_CHECK_STATE);

	m_bInGroup = TRUE;
}

void CGroupMemberTransaction::LeaveGroup(DWORD dwIpAddress, DWORD dwGroupId, TCHAR* pszName)
{
	m_pComLayer->UnregisterTimeout(m_pTransID, TIMER_CHECK_STATE);

	SendLeaveGroupPacket(dwIpAddress, dwGroupId, pszName);

	_TRACE( L"<<XHL>>	Leave topic\n" );

	m_dwGroupId = 0;
	m_bInGroup = FALSE;
	m_nCommand = 0;
}

void CGroupMemberTransaction::SetName(TCHAR* pszName)
{
	CAutoSectionLock lock(&m_csAnnounce);

	if (_tcscmp(m_szName, pszName) == 0)
		return;

	_tcscpy(m_szName, pszName);
	SendAnnouncePacket();
}

void CGroupMemberTransaction::SetLeader(BOOL bLeader)
{
	CAutoSectionLock lock(&m_csAnnounce);

	m_bLeader = bLeader;
	SendAnnouncePacket();
}

void CGroupMemberTransaction::SetMute(BOOL bMute)
{
	CAutoSectionLock lock(&m_csAnnounce);

	m_bMute = bMute;
	SendAnnouncePacket();
}

void CGroupMemberTransaction::SetSilent(BOOL bSilent)
{
	CAutoSectionLock lock(&m_csAnnounce);

	m_bSilent = bSilent;
	SendAnnouncePacket();
}

void CGroupMemberTransaction::SetCommand(UINT64 nCommand)
{
	CAutoSectionLock lock(&m_csAnnounce);

	m_nCommand = nCommand;
	SendAnnouncePacket();
}

int rand_range(int nLow, int nHigh)
{
	return (int)((double)rand() * (double)(nHigh - nLow) / (double)RAND_MAX) + nLow;
}

#include <time.h>

void CGroupMemberTransaction::OnTimeout(DWORD dwTimeID)
{
	if (dwTimeID == TIMER_ANNOUNCE) 
	{
		{
			CAutoSectionLock lock(&m_csAnnounce);
			SendAnnouncePacket();
			m_nAnnounceCount ++;
		}

		if (m_nAnnounceCount < 30) {
			srand((unsigned)time(NULL) ^ (m_dwLocalInterface << 16));
			m_pComLayer->RegisterTimeout(m_pTransID, rand_range(100, 400), TIMER_ANNOUNCE);
		} else
		{
			m_pComLayer->RegisterTimeout(m_pTransID, TIMEOUT_ANNOUNCE, TIMER_ANNOUNCE);
		}
	}
	else if (dwTimeID == TIMER_CHECK_STATE)
	{
		CAutoSectionLock lock(&m_cs);

		CChatMemberMap::iterator iterator = m_GroupMemberMap.begin();
		while (iterator != m_GroupMemberMap.end()) {
			PCHATMEMBERINFO pMemberInfo = (*iterator).second;
			if (GetTickCount() - pMemberInfo->dwLastAnnounceTime > 30000) {

				ReportMemberLeaveEvent((*iterator).first, pMemberInfo->dwGroupId, pMemberInfo->szName);

				m_GroupMemberMap.erase(iterator);
				free(pMemberInfo);
				iterator = m_GroupMemberMap.begin();
			}
			else
			{
				iterator ++;
			}
		}
		m_pComLayer->RegisterTimeout(m_pTransID, TIMEOUT_CHECK_STATE, TIMER_CHECK_STATE);
	}
}

void CGroupMemberTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;

	switch (pPacket->dwMagic)
	{
	case MAGIC_JOINGROUPCHAT:
		{
			PJOINGROUPCHAT pJoinGroupPacket = (PJOINGROUPCHAT)(pPacket + 1);

			// if I am a student, and the sender doesn't join my group, just ignore it
			if ((!m_bTeacher) && (pJoinGroupPacket->dwGroupId != m_dwGroupId))
			{
				break;
			}

			if ( 0 == pJoinGroupPacket->dwGroupId )
			{
				break;
			}

			CChatMemberMap::iterator iterator = m_GroupMemberMap.find(pJoinGroupPacket->dwIpAddress);
			if (iterator == m_GroupMemberMap.end())
			{
				PCHATMEMBERINFO pMemberInfo = (PCHATMEMBERINFO)malloc(sizeof(CHATMEMBERINFO));
				pMemberInfo->dwGroupId = pJoinGroupPacket->dwGroupId;
				pMemberInfo->dwState = pJoinGroupPacket->dwState;
				pMemberInfo->dwLastAnnounceTime = GetTickCount();
				_tcscpy(pMemberInfo->szName, pJoinGroupPacket->szName);
				pMemberInfo->nCommand = pJoinGroupPacket->nCommand;

				m_GroupMemberMap.insert(CChatMemberMap::value_type(pJoinGroupPacket->dwIpAddress, pMemberInfo));

				ReportMemberJoinEvent(pJoinGroupPacket->dwIpAddress, pJoinGroupPacket->dwGroupId, pJoinGroupPacket->szName, 
					pJoinGroupPacket->abMacAddress, pJoinGroupPacket->dwState, pJoinGroupPacket->nCommand);

				ResetAnnounce();
			}
			else
			{
				PCHATMEMBERINFO pMemberInfo = (*iterator).second;
				pMemberInfo->dwLastAnnounceTime = GetTickCount();

				if (pMemberInfo->dwGroupId == pJoinGroupPacket->dwGroupId)
				{
					// Name changed
					if (_tcscmp(pMemberInfo->szName, pJoinGroupPacket->szName) != 0)
					{
						_tcscpy(pMemberInfo->szName, pJoinGroupPacket->szName);
						ReportMemberNameChanged(pJoinGroupPacket->dwIpAddress, pMemberInfo->dwGroupId, pMemberInfo->szName);
					}

					// State changed
					if (pMemberInfo->dwState != pJoinGroupPacket->dwState)
					{
						pMemberInfo->dwState = pJoinGroupPacket->dwState;
						ReportMemberStateChanged(pJoinGroupPacket->dwIpAddress, pMemberInfo->dwGroupId, pMemberInfo->dwState);
					}

					// Command changed
					if (pMemberInfo->nCommand != pJoinGroupPacket->nCommand)
					{
						pMemberInfo->nCommand = pJoinGroupPacket->nCommand;
						ReportMemberCommandChanged(pJoinGroupPacket->dwIpAddress, pMemberInfo->dwGroupId, pMemberInfo->nCommand);
					}
				}
				else
				{
					// leave event
					ReportMemberLeaveEvent(pJoinGroupPacket->dwIpAddress, pMemberInfo->dwGroupId, pMemberInfo->szName);

					// join event
					pMemberInfo->dwGroupId = pJoinGroupPacket->dwGroupId;
					pMemberInfo->dwState = pJoinGroupPacket->dwState;
					_tcscpy(pMemberInfo->szName, pJoinGroupPacket->szName);
					pMemberInfo->nCommand = pJoinGroupPacket->nCommand;

					ReportMemberJoinEvent(pJoinGroupPacket->dwIpAddress, pJoinGroupPacket->dwGroupId, pJoinGroupPacket->szName, 
						pJoinGroupPacket->abMacAddress, pJoinGroupPacket->dwState, pJoinGroupPacket->nCommand);

					ResetAnnounce();
				}
			}
		}
		break;
	case MAGIC_ANNOUCEGROUPCHAT:
		{
			PANNOUNCEGROUPCHAT pAnnouncePacket = (PANNOUNCEGROUPCHAT)(pPacket + 1);

			// if I am a student, and the sender doesn't join my group, just ignore it
			if ((!m_bTeacher) && (pAnnouncePacket->dwGroupId != m_dwGroupId))
				break;

			CChatMemberMap::iterator iterator = m_GroupMemberMap.find(pAnnouncePacket->dwIpAddress);
			if (iterator == m_GroupMemberMap.end())
			{
				if ( pAnnouncePacket->dwGroupId != 0 )
				{
				PCHATMEMBERINFO pMemberInfo = (PCHATMEMBERINFO)malloc(sizeof(CHATMEMBERINFO));
				pMemberInfo->dwGroupId = pAnnouncePacket->dwGroupId;
				pMemberInfo->dwState = pAnnouncePacket->dwState;
				pMemberInfo->dwLastAnnounceTime = GetTickCount();
				_tcscpy(pMemberInfo->szName, pAnnouncePacket->szName);
				pMemberInfo->nCommand = pAnnouncePacket->nCommand;

				m_GroupMemberMap.insert(CChatMemberMap::value_type(pAnnouncePacket->dwIpAddress, pMemberInfo));

				ReportMemberJoinEvent(pAnnouncePacket->dwIpAddress, pAnnouncePacket->dwGroupId, pAnnouncePacket->szName, 
					pAnnouncePacket->abMacAddress, pAnnouncePacket->dwState, pAnnouncePacket->nCommand);

					//SendAnnouncePacket();
				ResetAnnounce();
				}
			}
			else 
			{
				PCHATMEMBERINFO pMemberInfo = (*iterator).second;
				pMemberInfo->dwLastAnnounceTime = GetTickCount();

				if (pMemberInfo->dwGroupId == pAnnouncePacket->dwGroupId)
				{

					// Name changed
					if (_tcscmp(pMemberInfo->szName, pAnnouncePacket->szName) != 0)
					{
						_tcscpy(pMemberInfo->szName, pAnnouncePacket->szName);
						ReportMemberNameChanged(pAnnouncePacket->dwIpAddress, pMemberInfo->dwGroupId, pMemberInfo->szName);
					}

					// State changed
					if (pMemberInfo->dwState != pAnnouncePacket->dwState)
					{
						pMemberInfo->dwState = pAnnouncePacket->dwState;
						ReportMemberStateChanged(pAnnouncePacket->dwIpAddress, pMemberInfo->dwGroupId, pMemberInfo->dwState);
					}

					// Command changed
					if (pMemberInfo->nCommand != pAnnouncePacket->nCommand)
					{
						pMemberInfo->nCommand = pAnnouncePacket->nCommand;
						ReportMemberCommandChanged(pAnnouncePacket->dwIpAddress, pMemberInfo->dwGroupId, pMemberInfo->nCommand);
					}
				}
				else
				{
					_TRACE( L"<<XHL>>	Use annouce to adjust the number\n" );
					// leave event
					ReportMemberLeaveEvent(pAnnouncePacket->dwIpAddress, pMemberInfo->dwGroupId, pMemberInfo->szName);
					//DeleteMemberFromMap( pAnnouncePacket->dwIpAddress );

					if ( 0 != pAnnouncePacket->dwGroupId )
					{
						// join event
						pMemberInfo->dwGroupId = pAnnouncePacket->dwGroupId;
						pMemberInfo->dwState = pAnnouncePacket->dwState;
						_tcscpy_s(pMemberInfo->szName, pAnnouncePacket->szName);
						pMemberInfo->nCommand = pAnnouncePacket->nCommand;

						ReportMemberJoinEvent(pAnnouncePacket->dwIpAddress, pAnnouncePacket->dwGroupId, pAnnouncePacket->szName, 
							pAnnouncePacket->abMacAddress, pAnnouncePacket->dwState, pAnnouncePacket->nCommand);

						ResetAnnounce();
					}
				}
			}
		}
		break;
	case MAGIC_LEAVEGROUPCHAT:
		{
			PLEAVEGROUPCHAT pLeavePacket = (PLEAVEGROUPCHAT)(pPacket + 1);

			// if I am a student, and the sender doesn't join my group, just ignore it
			if ((!m_bTeacher) && (pLeavePacket->dwGroupId != m_dwGroupId))
				break;

			if ( 0 == pLeavePacket->dwGroupId )
			{
				break;
			}

			CChatMemberMap::iterator iterator = m_GroupMemberMap.find(pLeavePacket->dwIpAddress);
			if (iterator != m_GroupMemberMap.end()) {
				PCHATMEMBERINFO pMemberInfo = (*iterator).second;
				ReportMemberLeaveEvent((*iterator).first, pMemberInfo->dwGroupId, pMemberInfo->szName);

				//TRACE( TEXT( "Recv leve packet\n" ) );
				m_GroupMemberMap.erase(iterator);
				free(pMemberInfo);
			}
		}
		break;
	default:
		break;
	}
}

void CGroupMemberTransaction::DeleteMemberFromMap(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);
	
	CChatMemberMap::iterator iterator = m_GroupMemberMap.find(dwIpAddress);
	if (iterator != m_GroupMemberMap.end()) {
		PCHATMEMBERINFO pMemberInfo = (*iterator).second;
		
		m_GroupMemberMap.erase(iterator);
		free(pMemberInfo);
	}
}

void CGroupMemberTransaction::ClearMemberMap()
{
	CAutoSectionLock lock(&m_cs);
	
	CChatMemberMap::iterator iterator = m_GroupMemberMap.begin();
	while (iterator != m_GroupMemberMap.end())
	{
		PCHATMEMBERINFO pMemberInfo = (*iterator).second;
		m_GroupMemberMap.erase(iterator);
		free(pMemberInfo);
		iterator = m_GroupMemberMap.begin();
	}
}

void CGroupMemberTransaction::ReportMemberJoinEvent(DWORD dwIpAddress, DWORD dwGroupId,
	TCHAR* szName, BYTE* pabMac, DWORD dwState/* = 0*/, UINT64 nCommand/* = 0*/)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_JOINGROUPCHAT);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_JOINGROUPCHAT;
	pHeader->dwLength = sizeof(EVENT_JOINGROUPCHAT);

	PEVENT_JOINGROUPCHAT pEvent = (PEVENT_JOINGROUPCHAT)(pHeader + 1);
	pEvent->dwIpAddress = dwIpAddress;
	memcpy(pEvent->abMacAddress, pabMac, 6);
	pEvent->dwGroupId = dwGroupId;
	pEvent->dwState = dwState;
	_tcscpy(pEvent->szName, szName);
	pEvent->nCommand = nCommand;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CGroupMemberTransaction::ReportMemberLeaveEvent(DWORD dwIpAddress, DWORD dwGroupId, TCHAR* pszName)
{
	_TRACE( L"I report a leave event\n" );
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_LEAVEGROUPCHAT);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_LEAVEGROUPCHAT;
	pHeader->dwLength = sizeof(EVENT_LEAVEGROUPCHAT);

	PEVENT_LEAVEGROUPCHAT pEvent = (PEVENT_LEAVEGROUPCHAT)(pHeader + 1);
	pEvent->dwIpAddress = dwIpAddress;
	pEvent->dwGroupId = dwGroupId;
	_tcscpy(pEvent->szName, pszName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();		
}

void CGroupMemberTransaction::ReportMemberNameChanged(DWORD dwIpAddress, DWORD dwGroupId, TCHAR* pszNewName)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_MEMBERNAMECHANGED);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_MEMBERNAMECHANGED;
	pHeader->dwLength = sizeof(EVENT_MEMBERNAMECHANGED);

	PEVENT_MEMBERNAMECHANGED pEvent = (PEVENT_MEMBERNAMECHANGED)(pHeader + 1);
	pEvent->dwIpAddress = dwIpAddress;
	pEvent->dwGroupId = dwGroupId;
	_tcscpy(pEvent->szName, pszNewName);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CGroupMemberTransaction::ReportMemberStateChanged(DWORD dwIpAddress, DWORD dwGroupId, DWORD dwNewState)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_MEMBERSTATECHANGED);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_MEMBERSTATECHANGED;
	pHeader->dwLength = sizeof(EVENT_MEMBERSTATECHANGED);

	PEVENT_MEMBERSTATECHANGED pEvent = (PEVENT_MEMBERSTATECHANGED)(pHeader + 1);
	pEvent->dwIpAddress = dwIpAddress;
	pEvent->dwGroupId = dwGroupId;
	pEvent->dwState = dwNewState;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();	
}

void CGroupMemberTransaction::ReportMemberCommandChanged(DWORD dwIpAddress, DWORD dwGroupId, UINT64 nCommand)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_MEMBERCOMMANDCHANGED);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_MEMBERCOMMANDCHANGED;
	pHeader->dwLength = sizeof(EVENT_MEMBERCOMMANDCHANGED);

	PEVENT_MEMBERCOMMANDCHANGED pEvent = (PEVENT_MEMBERCOMMANDCHANGED)(pHeader + 1);
	pEvent->dwIpAddress = dwIpAddress;
	pEvent->dwGroupId = dwGroupId;
	pEvent->nCommand = nCommand;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CGroupMemberTransaction::SendJoinGroupPacket(DWORD dwIpAddress, DWORD dwGroupId, 
	TCHAR* pszName, BYTE* pabMac, DWORD dwState/* = 0*/, UINT64 nCommand/* = 0*/)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_pComLayer->GetComAddress(), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PJOINGROUPCHAT pJoinGroupPacket = (PJOINGROUPCHAT)(pPacket + 1);

	pPacket->dwMagic = MAGIC_JOINGROUPCHAT;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(JOINGROUPCHAT);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pJoinGroupPacket->dwIpAddress = dwIpAddress;
	memcpy(pJoinGroupPacket->abMacAddress, pabMac, 6);
	pJoinGroupPacket->dwState = dwState;
	pJoinGroupPacket->dwGroupId = dwGroupId;
	_tcscpy(pJoinGroupPacket->szName, pszName);
	pJoinGroupPacket->nCommand = nCommand;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);	
}

void CGroupMemberTransaction::SendLeaveGroupPacket(DWORD dwIpAddress, DWORD dwGroupId, TCHAR* pszName)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_pComLayer->GetComAddress(), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PLEAVEGROUPCHAT pLeaveGroupPacket = (PLEAVEGROUPCHAT)(pPacket + 1);

	pPacket->dwMagic = MAGIC_LEAVEGROUPCHAT;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(LEAVEGROUPCHAT);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pLeaveGroupPacket->dwIpAddress = dwIpAddress;
	pLeaveGroupPacket->dwGroupId = dwGroupId;
	_tcscpy(pLeaveGroupPacket->szName, pszName); 

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);	
}

void CGroupMemberTransaction::SendAnnouncePacket()
{
	//if (!m_dwGroupId)
	//	return;

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_pComLayer->GetComAddress(), m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PANNOUNCEGROUPCHAT pAnnouncePacket = (PANNOUNCEGROUPCHAT)(pPacket + 1);

	pPacket->dwMagic = MAGIC_ANNOUCEGROUPCHAT;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(ANNOUNCEGROUPCHAT);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	DWORD dwState = MEMBER_STATE_NORMAL;
	if (m_bTeacher)
		dwState |= MEMBER_STATE_TEACHER;
	if (m_bLeader)
		dwState |= MEMBER_STATE_LEADER;
	if (m_bMute)
		dwState |= MEMBER_STATE_MUTE;
	if (m_bSilent)
		dwState |= MEMBER_STATE_BLACK_SCREEN;

	pAnnouncePacket->dwIpAddress = m_dwLocalInterface;
	memcpy(pAnnouncePacket->abMacAddress, m_abMac, 6);
	pAnnouncePacket->dwGroupId = m_dwGroupId;
	pAnnouncePacket->dwState = dwState;
	_tcscpy(pAnnouncePacket->szName, m_szName); 
	pAnnouncePacket->nCommand = m_nCommand;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

DWORD CGroupMemberTransaction::FindMemberGroupId(DWORD dwIpAddress)
{
	CChatMemberMap::iterator it = m_GroupMemberMap.find(dwIpAddress);
	if (it == m_GroupMemberMap.end())
		return 0;

	PCHATMEMBERINFO pChatMemberInfo = it->second;

	return pChatMemberInfo->dwGroupId;
}

void CGroupMemberTransaction::ResetAnnounce()
{
	m_pComLayer->UnregisterTimeout(m_pTransID, TIMER_ANNOUNCE);
	m_nAnnounceCount = 0;
	srand((unsigned)time(NULL) ^ (m_dwLocalInterface << 16));
	m_pComLayer->RegisterTimeout(m_pTransID, rand_range(100, 400), TIMER_ANNOUNCE);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
#ifdef USE_PROXIMITY

typedef int (*PPDN_INITIALIZE)();
typedef int (*PPDN_UNINITIALIZE)();
typedef int (*PPDN_START)();
typedef int (*PPDN_STOP)();
typedef int (*PPDN_REGISTERNOTIFICATION)(PDN_Callback callback, void* context);
typedef int (*PPDN_UNREGISTERNOTIFICATION)(PDN_Callback callback, void* context);
typedef int (*PPDN_FINDNEARESTNODES)(uint32_t num_nodes, PDN_Uuid* uuids, uint32_t* num_returned);
typedef int (*PPDN_GETNODE)(const PDN_Uuid* uuid, PDN_Node** node_ptr);
typedef int (*PPDN_DUPLICATENODE)(const PDN_Node* node, PDN_Node** node_ptr);
typedef int (*PPDN_FREENODE)(PDN_Node** node_ptr);

CProximityTransaction::CProximityTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_dwGroupLeaderIp = 0;
	m_dwGroupId = 0XFFFFFFFF;
	m_bProcessInvite = FALSE;
	m_bInited = FALSE;

	m_dwNearCount = 0;
	memset(m_pdwNearIp, 0, MAX_NEAREST_COUNT * sizeof(DWORD));

	m_hDcsProximity = LoadLibrary(_T("DcsProximity.dll"));

	InitializeCriticalSection(&m_cs);
}

CProximityTransaction::~CProximityTransaction()
{
	DeleteCriticalSection(&m_cs);

	if (m_hDcsProximity) {
		FreeLibrary(m_hDcsProximity);
		m_hDcsProximity = NULL;
	}
}

void ProximityCallback(PDN_Notification code, const PDN_Node* node, void* context)
{
	CProximityTransaction *pThis = (CProximityTransaction *)context;
	_TRACE(_T("Proximity callback code: %d \n"), code);
}

BOOL CProximityTransaction::StartProximity(DWORD dwGroupLeaderIp, DWORD dwGroupId)
{
	CAutoSectionLock lock(&m_cs);

	if (m_bInited)
		return TRUE;

	if (!m_hDcsProximity)
		return FALSE;

	m_dwGroupLeaderIp = dwGroupLeaderIp;
	m_dwGroupId = dwGroupId;

	m_dwNearCount = 0;
	memset(m_pdwNearIp, 0, MAX_NEAREST_COUNT * sizeof(DWORD));

	//PPDN_INITIALIZE pPDN_Initialize = (PPDN_INITIALIZE)GetProcAddress(m_hDcsProximity, "PDN_Initialize");
	//PPDN_REGISTERNOTIFICATION pPDN_RegisterNotification = (PPDN_REGISTERNOTIFICATION)GetProcAddress(m_hDcsProximity, "PDN_RegisterNotification");
	PPDN_START pPDN_Start = (PPDN_START)GetProcAddress(m_hDcsProximity, "PDN_Start");
	if (/*!pPDN_Initialize || *//*!pPDN_RegisterNotification || */!pPDN_Start)
		return FALSE;

	int retcode;
 //   retcode = pPDN_Initialize();
	//if (retcode != PDN_SUCCESS) {
	//	_TRACE(_T("PDN_Initialize Failed: %d \n"), retcode);
	//	return FALSE;
	//}

 //   retcode = pPDN_RegisterNotification(ProximityCallback, this);
	//if (retcode != PDN_SUCCESS) {
	//	_TRACE(_T("PDN_RegisterNotification Failed: %d \n"), retcode);
	//	return FALSE;
	//}

    retcode = pPDN_Start();
	if (retcode != PDN_SUCCESS) {
		_TRACE(_T("PDN_Start Failed: %d \n"), retcode);
		return FALSE;
	}

	m_pComLayer->RegisterTimeout(m_pTransID, 100, 0);

	m_bInited = TRUE;

	return TRUE;
}

void CProximityTransaction::StopProximity()
{
	CAutoSectionLock lock(&m_cs);

	if (!m_hDcsProximity)
		return;

	if (!m_bInited)
		return;

	PPDN_STOP pPDN_Stop = (PPDN_STOP)GetProcAddress(m_hDcsProximity, "PDN_Stop");
	//PPDN_UNREGISTERNOTIFICATION pPDN_UnregisterNotification = (PPDN_UNREGISTERNOTIFICATION)GetProcAddress(m_hDcsProximity, "PDN_UnregisterNotification");
	//PPDN_UNINITIALIZE pPDN_Uninitialize = (PPDN_UNINITIALIZE)GetProcAddress(m_hDcsProximity, "PDN_Uninitialize");
	if (!pPDN_Stop/* || !pPDN_UnregisterNotification*//* || !pPDN_Uninitialize*/)
		return;

	pPDN_Stop();
	//pPDN_UnregisterNotification(ProximityCallback, NULL);
	//pPDN_Uninitialize();

	m_bInited = FALSE;
}

void CProximityTransaction::AcceptInvite(DWORD dwGroupLeaderIp, DWORD dwGroupId)
{
	CAutoSectionLock lock(&m_cs);

	m_dwGroupLeaderIp = dwGroupLeaderIp;
	m_dwGroupId = dwGroupId;

	m_bProcessInvite = FALSE;
}

void CProximityTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (dwTimeID == 0)
	{
		if (!m_hDcsProximity)
			return;

		if (!m_bInited)
			return;

		PPDN_FINDNEARESTNODES pPDN_FindNearestNodes = (PPDN_FINDNEARESTNODES)GetProcAddress(m_hDcsProximity, "PDN_FindNearestNodes");
		PPDN_GETNODE pPDN_GetNode = (PPDN_GETNODE)GetProcAddress(m_hDcsProximity, "PDN_GetNode");
		PPDN_FREENODE pPDN_FreeNode = (PPDN_FREENODE)GetProcAddress(m_hDcsProximity, "PDN_FreeNode");
		if (!pPDN_FindNearestNodes || !pPDN_GetNode || !pPDN_FreeNode)
			return;

        uint32_t num_nodes;
		PDN_Uuid uuid[MAX_NEAREST_COUNT] = {0};

		DWORD nCount = 0;
		DWORD pdwNearIp[MAX_NEAREST_COUNT] = {0};
		
        int retcode = pPDN_FindNearestNodes(MAX_NEAREST_COUNT, uuid, &num_nodes);
		_TRACE(_T("PDN_FindNearestNodes: %d, num_nodes: %d \n"), retcode, num_nodes);

		if ((retcode == PDN_SUCCESS) && (num_nodes > 0)) 
		{
			for (DWORD i=0; i<num_nodes; i++)
			{
				PDN_Node* node = NULL;
				retcode = pPDN_GetNode(&uuid[i], &node);

				_TRACE(_T("PDN_GetNode: %d, nearness: %d \n"), retcode, (retcode == PDN_SUCCESS && node) ? node->nearness : -1);

				if ((retcode == PDN_SUCCESS) && (node->nearness >= PDN_CLOSE)) {

					PBYTE pbSrcValue = (PBYTE)&node->ipv4_addr;
					PBYTE pbDstValue = (PBYTE)&pdwNearIp[nCount];

					*(pbDstValue + 0) = *(pbSrcValue + 3);
					*(pbDstValue + 1) = *(pbSrcValue + 2);
					*(pbDstValue + 2) = *(pbSrcValue + 1);
					*(pbDstValue + 3) = *(pbSrcValue + 0);

					nCount ++;
				}

				if (node) {
					pPDN_FreeNode(&node);
				}
			}
		}

		m_dwNearCount = nCount;
		memcpy(m_pdwNearIp, pdwNearIp, MAX_NEAREST_COUNT * sizeof(DWORD));

		if ((m_dwNearCount > 0) && (m_dwGroupId != 0xFFFFFFFF)) {

			for (DWORD i=0; i<m_dwNearCount; i++) {
				SendInviteCommand(m_pdwNearIp[i]);
			}
		}

		m_pComLayer->RegisterTimeout(m_pTransID, 100, 0);
	}
}

void CProximityTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_PROXIMITY_INVITE:
		{
			PPROXIMITY_INVITE pProximityInvite = (PPROXIMITY_INVITE)(pPacket + 1);

			_TRACE(_T("Received Proximity Invitation, InviterIP: %08X \n"), pProximityInvite->dwInviterIp);

			// Have been in group, will not accept inviattion
			if ((m_dwGroupLeaderIp != 0) || (m_dwGroupId != 0xFFFFFFFF)) {
				_TRACE(_T("Have been in group. \n"));
				break;
			}

			// Intviter is not near me currently
			DWORD i;
			for (i=0; i<m_dwNearCount; i++) {
				if (m_pdwNearIp[i] == pProximityInvite->dwInviterIp)
					break;
			}
 
			if (i >= m_dwNearCount) {
				_TRACE(_T("Intviter is NOT near me. \n"));
				break;
			}

			// Being process the invitation
			if (!m_bProcessInvite) {
				m_bProcessInvite = TRUE;
				_TRACE(_T("Report Proximity Intvitation Event. \n"));
				ReportProximityInviteEvent(pProximityInvite);
			}
		}
		break;
	default:
		break;
	}
}

void CProximityTransaction::SendInviteCommand(DWORD dwNearIp)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwNearIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPROXIMITY_INVITE pProximityInvite = (PPROXIMITY_INVITE)(pPacket + 1);

	pPacket->dwMagic = MAGIC_PROXIMITY_INVITE;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PROXIMITY_INVITE);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pProximityInvite->dwInviterIp = m_pComLayer->GetLocalAddress();
	pProximityInvite->dwGroupLeaderIp = m_dwGroupLeaderIp;
	pProximityInvite->dwGroupId = m_dwGroupId;
	
	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CProximityTransaction::ReportProximityInviteEvent(PPROXIMITY_INVITE pProximityInvite)
{
	// report event
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_PROXIMITY_INVITE);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_PROXIMITY_INVITE pEvent = (PEVENT_PROXIMITY_INVITE)(pHeader + 1);

	pHeader->dwEvent = EV_PROXIMITY_INVITE;
	pHeader->dwLength = sizeof(EVENT_PROXIMITY_INVITE);

	pEvent->dwInviterIp = pProximityInvite->dwInviterIp;
	pEvent->dwGroupLeaderIp = pProximityInvite->dwGroupLeaderIp;
	pEvent->dwGroupId = pProximityInvite->dwGroupId;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

#endif // USE_PROXIMITY

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CPolicyTransaction::CPolicyTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);

	m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);
}

CPolicyTransaction::~CPolicyTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CPolicyTransaction::AddMember(PMEMBERINFO pMemberInfo, DWORD dwAppPolicy, DWORD dwWebPolicy, DWORD dwUsbPolicy, DWORD dwPrintPolicy)
{
	CAutoSectionLock lock(&m_cs);

	CPolicyMap::iterator it = m_policyMap.find(pMemberInfo);
	if (it != m_policyMap.end())
		return;

	PPOLICY_CONTEXT pPolicyContext = (PPOLICY_CONTEXT)malloc(sizeof(POLICY_CONTEXT));
	if (!pPolicyContext)
		return;

	pPolicyContext->dwAppPolicyCommand = dwAppPolicy;
	pPolicyContext->dwAppPolicyState = dwAppPolicy;
	pPolicyContext->dwWebPolicyCommand = dwWebPolicy;
	pPolicyContext->dwWebPolicyState = dwWebPolicy;
	pPolicyContext->dwUsbPolicyCommand = dwUsbPolicy;
	pPolicyContext->dwUsbPolicyState = dwUsbPolicy;
	pPolicyContext->dwPrintPolicyCommand = dwPrintPolicy;
	pPolicyContext->dwPrintPolicyState = dwPrintPolicy;

	m_policyMap.insert(CPolicyMap::value_type(pMemberInfo, pPolicyContext));
}

void CPolicyTransaction::DelMember(PMEMBERINFO pMemberInfo)
{
	CAutoSectionLock lock(&m_cs);

	CPolicyMap::iterator it = m_policyMap.find(pMemberInfo);
	if (it != m_policyMap.end())
		return;

	free((*it).second);
	m_policyMap.erase(it);
}

void CPolicyTransaction::SetAppPolicy(PMEMBERINFO pMemberInfo, DWORD dwPolicy)
{
	CAutoSectionLock lock(&m_cs);

	CPolicyMap::iterator it = m_policyMap.find(pMemberInfo);
	if (it != m_policyMap.end())
		return;

	PPOLICY_CONTEXT pPolicyContext = (*it).second;
	pPolicyContext->dwAppPolicyCommand = dwPolicy;

	if (dwPolicy != pPolicyContext->dwAppPolicyState) {
		SendPolicyCommand(pMemberInfo, pPolicyContext);
	}
}

void CPolicyTransaction::SetWebPolicy(PMEMBERINFO pMemberInfo, DWORD dwPolicy)
{
	CAutoSectionLock lock(&m_cs);

	CPolicyMap::iterator it = m_policyMap.find(pMemberInfo);
	if (it != m_policyMap.end())
		return;

	PPOLICY_CONTEXT pPolicyContext = (*it).second;
	pPolicyContext->dwWebPolicyCommand = dwPolicy;

	if (dwPolicy != pPolicyContext->dwWebPolicyState) {
		SendPolicyCommand(pMemberInfo, pPolicyContext);
	}
}

void CPolicyTransaction::SetUsbPolicy(PMEMBERINFO pMemberInfo, DWORD dwPolicy)
{
	CAutoSectionLock lock(&m_cs);

	CPolicyMap::iterator it = m_policyMap.find(pMemberInfo);
	if (it != m_policyMap.end())
		return;

	PPOLICY_CONTEXT pPolicyContext = (*it).second;
	pPolicyContext->dwUsbPolicyCommand = dwPolicy;

	if (dwPolicy != pPolicyContext->dwUsbPolicyState) {
		SendPolicyCommand(pMemberInfo, pPolicyContext);
	}
}

void CPolicyTransaction::SetPrintPolicy(PMEMBERINFO pMemberInfo, DWORD dwPolicy)
{
	CAutoSectionLock lock(&m_cs);

	CPolicyMap::iterator it = m_policyMap.find(pMemberInfo);
	if (it != m_policyMap.end())
		return;

	PPOLICY_CONTEXT pPolicyContext = (*it).second;
	pPolicyContext->dwPrintPolicyCommand = dwPolicy;

	if (dwPolicy != pPolicyContext->dwPrintPolicyState) {
		SendPolicyCommand(pMemberInfo, pPolicyContext);
	}
}

void CPolicyTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (0 == dwTimeID) {
		CPolicyMap::iterator it = m_policyMap.begin();
		while (it != m_policyMap.end()) {
			PPOLICY_CONTEXT pPolicyContext = (*it).second;
			if ((pPolicyContext->dwAppPolicyCommand != pPolicyContext->dwAppPolicyState) ||
				(pPolicyContext->dwWebPolicyCommand != pPolicyContext->dwWebPolicyState) ||
				(pPolicyContext->dwUsbPolicyCommand != pPolicyContext->dwUsbPolicyState) ||
				(pPolicyContext->dwPrintPolicyCommand != pPolicyContext->dwPrintPolicyState))
			{
				SendPolicyCommand((*it).first, pPolicyContext);
			}

			it ++;
		}

		m_pComLayer->RegisterTimeout(m_pTransID, 2000, 0);		
	}
}

void CPolicyTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_POLICY_STATE:
		{
			PPOLICY_STATE pPolicyState = (PPOLICY_STATE)(pPacket + 1);
			ProcessPolicyState(pPolicyState);
		}
		break;
	default:
		break;
	}
}

void CPolicyTransaction::SendPolicyCommand(PMEMBERINFO pMemberInfo, PPOLICY_CONTEXT pPolicyContext)
{
	if (!pMemberInfo->bLoggedIn)
		return;

	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(pMemberInfo->dwIpAddress, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPOLICY_COMMAND pPolicyCommand = (PPOLICY_COMMAND)(pPacket + 1);

	pPacket->dwMagic = MAGIC_POLICY_COMMAND;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(POLICY_COMMAND);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pPolicyCommand->dwTeacherIp = m_pComLayer->GetLocalAddress();
	pPolicyCommand->dwAppPolicy = pPolicyContext->dwAppPolicyCommand;
	pPolicyCommand->dwWebPolicy = pPolicyContext->dwWebPolicyCommand;
	pPolicyCommand->dwUsbPolicy = pPolicyContext->dwUsbPolicyCommand;
	pPolicyCommand->dwPrintPolicy = pPolicyContext->dwPrintPolicyCommand;
	
	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPolicyTransaction::ProcessPolicyState(PPOLICY_STATE pPolicyState)
{
	CPolicyMap::iterator it = m_policyMap.begin();
	while (it != m_policyMap.end()) {
		PMEMBERINFO pMemberInfo = (*it).first;
		if ((pMemberInfo->bLoggedIn) &&
			(pMemberInfo->dwIpAddress == pPolicyState->dwStudentIp))
		{
			PPOLICY_CONTEXT pPolicyContext = (*it).second;

			pPolicyContext->dwAppPolicyState = pPolicyState->dwAppPolicy;
			pPolicyContext->dwWebPolicyState = pPolicyState->dwWebPolicy;
			pPolicyContext->dwUsbPolicyState = pPolicyState->dwUsbPolicy;
			pPolicyContext->dwPrintPolicyState = pPolicyState->dwPrintPolicy;

			return;
		}
		
		it ++;
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CPolicyClientTransaction::CPolicyClientTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);

	m_dwAppPolicy = APP_UNRESTRICTED;
	m_dwWebPolicy = WEB_UNRESTRICTED;
	m_dwUsbPolicy = USB_UNRESTRICTED;
	m_dwPrintPolicy = PRINT_UNRESTRICTED;
}

CPolicyClientTransaction::~CPolicyClientTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CPolicyClientTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_POLICY_COMMAND:
		{
			PPOLICY_COMMAND pPolicyCommand = (PPOLICY_COMMAND)(pPacket + 1);
			ProcessPolicyCommand(pPolicyCommand);
		}
		break;
	default:
		break;
	}
}

void CPolicyClientTransaction::ProcessPolicyCommand(PPOLICY_COMMAND pPolicyCommand)
{
	DWORD dwPolicyMask = 0;

	if (m_dwAppPolicy != pPolicyCommand->dwAppPolicy) {
		m_dwAppPolicy = pPolicyCommand->dwAppPolicy;
		dwPolicyMask |= POLICY_MASK_APP;
	}

	if (m_dwWebPolicy != pPolicyCommand->dwWebPolicy) {
		m_dwWebPolicy = pPolicyCommand->dwWebPolicy;
		dwPolicyMask |= POLICY_MASK_WEB;
	}

	if (m_dwUsbPolicy != pPolicyCommand->dwUsbPolicy) {
		m_dwUsbPolicy = pPolicyCommand->dwUsbPolicy;
		dwPolicyMask |= POLICY_MASK_USB;
	}

	if (m_dwPrintPolicy != pPolicyCommand->dwPrintPolicy) {
		m_dwPrintPolicy = pPolicyCommand->dwPrintPolicy;
		dwPolicyMask |= POLICY_MASK_PRINT;
	}

	if (dwPolicyMask) {
		ReportPolicyChangeEvent(dwPolicyMask);
	}

	SendPolicyState(pPolicyCommand->dwTeacherIp);
}

void CPolicyClientTransaction::SendPolicyState(DWORD dwTeacherIp)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPOLICY_STATE pPolicyState = (PPOLICY_STATE)(pPacket + 1);

	pPacket->dwMagic = MAGIC_POLICY_STATE;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(POLICY_STATE);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pPolicyState->dwStudentIp = m_pComLayer->GetLocalAddress();
	pPolicyState->dwAppPolicy = m_dwAppPolicy;
	pPolicyState->dwWebPolicy = m_dwWebPolicy;
	pPolicyState->dwUsbPolicy = m_dwUsbPolicy;
	pPolicyState->dwPrintPolicy = m_dwPrintPolicy;
	
	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPolicyClientTransaction::ReportPolicyChangeEvent(DWORD dwPolicyMask)
{
	// report event
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_POLICY_CHANGE);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_POLICY_CHANGE pEvent = (PEVENT_POLICY_CHANGE)(pHeader + 1);

	pHeader->dwEvent = EV_POLICY_CHANGE;
	pHeader->dwLength = sizeof(EVENT_POLICY_CHANGE);

	pEvent->dwPolicyMask = dwPolicyMask;
	pEvent->dwAppPolicy = m_dwAppPolicy;
	pEvent->dwWebPolicy = m_dwWebPolicy;
	pEvent->dwUsbPolicy = m_dwUsbPolicy;
	pEvent->dwPrintPolicy = m_dwPrintPolicy;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CApplicationTransaction::CApplicationTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);
}

CApplicationTransaction::~CApplicationTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CApplicationTransaction::AddStudent(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	CProcessMap::iterator it = m_procMap.find(dwIpAddress);
	if (it != m_procMap.end())
		return;

	CProcIDList* pProcList = new CProcIDList();
	if (!pProcList)
		return;

	m_procMap.insert(CProcessMap::value_type(dwIpAddress, pProcList));
}

void CApplicationTransaction::DelStudent(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	CProcessMap::iterator it = m_procMap.find(dwIpAddress);
	if (it == m_procMap.end())
		return;

	CProcIDList* pProcList = (*it).second;
	pProcList->clear();
	delete pProcList;

	m_procMap.erase(it);
}

void CApplicationTransaction::DeleteAllStudents()
{
	CAutoSectionLock lock(&m_cs);

	CProcessMap::iterator it = m_procMap.begin();
	while (it != m_procMap.end()) 
	{
		CProcIDList* pProcList = (*it).second;
		pProcList->clear();
		delete pProcList;
		m_procMap.erase(it);
		
		it = m_procMap.begin();
	}
}

void CApplicationTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_PROCESSID_LIST:
		{
			PPROCESSID_LIST pProcIdList = (PPROCESSID_LIST)(pPacket + 1);
			ProcessProcIdList(pProcIdList);
		}
		break;
	case MAGIC_PROCESSINFO_RET:
		{
			PPROCESSINFO_RET pProcInfoRet = (PPROCESSINFO_RET)(pPacket + 1);
			ProcessProcInfoRet(pProcInfoRet);
		}
		break;
	}
}

void CApplicationTransaction::ProcessProcIdList(PPROCESSID_LIST pProcIdList)
{
	CProcessMap::iterator it = m_procMap.find(pProcIdList->dwStudentIp);
	if (it == m_procMap.end())
		return;

	int i=0;
	int nExitCount = 0;
	DWORD pdwExitProcId[256] = {0};

	CProcIDList* pProcessIdList = (*it).second;
	CProcIDList::iterator iterator = pProcessIdList->begin();
	while (iterator != pProcessIdList->end()) {

		// Lock each PROCID in the new PROCID List
		DWORD dwProcessID = (*iterator);
		for (i=0; i<pProcIdList->nCount; i++) {
			if (dwProcessID == pProcIdList->pdwProcessId[i])
				break;
		}

		// Remember the deleted PROCID
		if (i >= pProcIdList->nCount) {
			pdwExitProcId[nExitCount] = dwProcessID;
			nExitCount ++;

			pProcessIdList->erase(iterator);
			iterator = pProcessIdList->begin();
		} else
		{
			iterator ++;
		}
	}

	if (nExitCount > 0) {
		ReportProcExitEvent(pProcIdList->dwStudentIp, nExitCount, pdwExitProcId);
	}

	for (i=0; i<pProcIdList->nCount; i++) {
		if (!ProcIDInList(pProcIdList->pdwProcessId[i], pProcessIdList)) {
			SendProcRequestPacket(pProcIdList->dwStudentIp, pProcIdList->pdwProcessId[i]);
		}
	}
}

BOOL CApplicationTransaction::ProcIDInList(DWORD dwProcID, CProcIDList* pProcessIdList)
{
	CProcIDList::iterator it = pProcessIdList->begin();
	while (it != pProcessIdList->end()) {
		if (dwProcID == (*it))
			return TRUE;

		it ++;
	}

	return FALSE;
}

void CApplicationTransaction::DelProcIDInList(DWORD dwProcID, CProcIDList* pProcessIdList)
{
	CProcIDList::iterator it = pProcessIdList->begin();
	while (it != pProcessIdList->end()) {
		if (dwProcID == (*it)) {
			pProcessIdList->erase(it);
			return;
		}

		it ++;
	}
}

void CApplicationTransaction::ProcessProcInfoRet(PPROCESSINFO_RET pProcInfoRet)
{
	// Check student
	CProcessMap::iterator it = m_procMap.find(pProcInfoRet->dwStudentIp);
	if (it == m_procMap.end())
		return;	

	// Check if the process has reported it's data
	CProcIDList* pProcessIdList = (*it).second;
	CProcIDList::iterator iterator = pProcessIdList->begin();
	while (iterator != pProcessIdList->end()) {
		if (pProcInfoRet->dwProcessId == (*iterator))
			return;

		iterator ++;
	}

	pProcessIdList->push_back(pProcInfoRet->dwProcessId);

	ReportProcCreateEvent(pProcInfoRet);
}

void CApplicationTransaction::SendProcRequestPacket(DWORD dwStudentIp, DWORD dwProcId)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwStudentIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPROCESSINFO_REQ pProcInfoReq = (PPROCESSINFO_REQ)(pPacket + 1);

	pPacket->dwMagic = MAGIC_PROCESSINFO_REQ;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PROCESSINFO_REQ);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pProcInfoReq->dwProcessId = dwProcId;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CApplicationTransaction::ReportProcCreateEvent(PPROCESSINFO_RET pProcInfoRet)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_PROC_CREATED);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_PROC_CREATED pEvent = (PEVENT_PROC_CREATED)(pHeader + 1);

	pHeader->dwEvent = EV_APPLICATION_CREATED;
	pHeader->dwLength = sizeof(EVENT_PROC_CREATED);

	pEvent->dwStudentIp = pProcInfoRet->dwStudentIp;
	pEvent->dwProcessId = pProcInfoRet->dwProcessId;
	pEvent->cbInfoSize = pProcInfoRet->cbInfoSize;
	pEvent->pbInfo = (BYTE *)malloc(pProcInfoRet->cbInfoSize);
	if (pEvent->pbInfo) {
		memcpy(pEvent->pbInfo, pProcInfoRet->abInfo, pProcInfoRet->cbInfoSize);
	}

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();	
}

void CApplicationTransaction::ReportProcExitEvent(DWORD dwStudentIp, int nExitCount, DWORD* pdwExitProcId)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_PROC_EXITED) + nExitCount * sizeof(DWORD);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_PROC_EXITED pEvent = (PEVENT_PROC_EXITED)(pHeader + 1);

	pHeader->dwEvent = EV_APPLICATION_EXITED;
	pHeader->dwLength = sizeof(EVENT_PROC_EXITED) + nExitCount * sizeof(DWORD);

	pEvent->dwStudentIp = dwStudentIp;
	pEvent->dwCount = (DWORD)nExitCount;
	memcpy(pEvent->pdwProcIdList, pdwExitProcId, nExitCount * sizeof(DWORD));

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();	
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CPassiveApplicationTransaction::CPassiveApplicationTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_bStarted = FALSE;
	m_dwTeacherIp = 0;
	m_nUpdateSeconds = 5;

	InitializeCriticalSection(&m_cs);
}

CPassiveApplicationTransaction::~CPassiveApplicationTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CPassiveApplicationTransaction::Startup(DWORD dwTeacherIp, int nUpdateSeconds)
{
	CAutoSectionLock lock(&m_cs);

	m_bStarted = TRUE;
	m_dwTeacherIp = dwTeacherIp;
	m_nUpdateSeconds = nUpdateSeconds;

	m_pComLayer->RegisterTimeout(m_pTransID, m_nUpdateSeconds * 1000, 0);	
}

void CPassiveApplicationTransaction::Shutdown()
{
	CAutoSectionLock lock(&m_cs);
	m_bStarted = FALSE;

	CApplicatonList::iterator it = m_appList.begin();
	while (it != m_appList.end()) {
		free((*it));
		m_appList.erase(it);
		it = m_appList.begin();
	}
}

void CPassiveApplicationTransaction::SendApplicationList(LPVOID pvAppItem, DWORD nCount)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	PAPPLICATIONITEM pAppItem = (PAPPLICATIONITEM)pvAppItem;

	// Clear Application List
	CApplicatonList::iterator it = m_appList.begin();
	while (it != m_appList.end()) {
		free((*it));
		m_appList.erase(it);
		it = m_appList.begin();
	}

	BYTE abBuffer[2048];

	for (DWORD i=0; i<nCount; i++) 
	{
		memset(abBuffer, 0, 2048);
		PAPPLICATIONDATA pAppDataTemp = (PAPPLICATIONDATA)abBuffer;

		if (pAppItem[i].dwProcessId == 0)
			continue;

		pAppDataTemp->cbSize = sizeof(APPLICATIONDATA);
		pAppDataTemp->dwProcessId = pAppItem[i].dwProcessId;

		TCHAR szName[_MAX_FNAME];
		TCHAR szExt[_MAX_EXT];
		_tsplitpath(pAppItem[i].szProcessName, NULL, NULL, szName, szExt);
		_tcscat(szName, szExt);

		pAppDataTemp->wNameSize = (_tcslen(szName) + 1) * sizeof(TCHAR);
		pAppDataTemp->cbSize += pAppDataTemp->wNameSize;
		pAppDataTemp->wNameOffset = 0;
		memcpy(pAppDataTemp->abParams + pAppDataTemp->wNameOffset, szName, pAppDataTemp->wNameSize);

		pAppDataTemp->wTitleSize = (_tcslen(pAppItem[i].szWindowTitle) + 1) * sizeof(TCHAR);
		pAppDataTemp->cbSize += pAppDataTemp->wTitleSize;
		pAppDataTemp->wTitleOffset = pAppDataTemp->wNameSize;
		memcpy(pAppDataTemp->abParams + pAppDataTemp->wTitleOffset, pAppItem[i].szWindowTitle, pAppDataTemp->wTitleSize);

		pAppDataTemp->wbmBitsPixel = pAppItem[i].wbmBitsPixel;
		pAppDataTemp->wIconSize = pAppItem[i].cbSize;
		pAppDataTemp->cbSize += pAppDataTemp->wIconSize;
		pAppDataTemp->wIconOffset = pAppDataTemp->wTitleOffset + pAppDataTemp->wTitleSize;
		memcpy(pAppDataTemp->abParams + pAppDataTemp->wIconOffset, pAppItem[i].pbBitmapBits, pAppDataTemp->wIconSize);
			
		PAPPLICATIONDATA pAppData = (PAPPLICATIONDATA)malloc(pAppDataTemp->cbSize);
		if (pAppData) {
			memcpy(pAppData, pAppDataTemp, pAppDataTemp->cbSize);
			m_appList.push_back(pAppData);
		}
	}

	SendProcessIdList();
}

void CPassiveApplicationTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (0 == dwTimeID)
	{
		ReportUpdateAppListEvent();
		m_pComLayer->RegisterTimeout(m_pTransID, m_nUpdateSeconds * 1000, 0);	
	}
}

void CPassiveApplicationTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_PROCESSINFO_REQ:
		{
			PPROCESSINFO_REQ pProcInfoReq = (PPROCESSINFO_REQ)(pPacket + 1);
			ProcessProcInfoRequest(pProcInfoReq);
		}
		break;
	}
}

void CPassiveApplicationTransaction::ProcessProcInfoRequest(PPROCESSINFO_REQ pProcInfoReq)
{
	CApplicatonList::iterator it = m_appList.begin();
	while (it != m_appList.end()) {
		PAPPLICATIONDATA pAppData = (*it);
		if (pAppData->dwProcessId == pProcInfoReq->dwProcessId) {
			SendProcessInfo(pAppData);
			break;
		}

		it ++;
	}
}

void CPassiveApplicationTransaction::ReportUpdateAppListEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_UPDATE_APPLIST;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveApplicationTransaction::SendProcessIdList()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPROCESSID_LIST pProcessIdList = (PPROCESSID_LIST)(pPacket + 1);

	pPacket->dwMagic = MAGIC_PROCESSID_LIST;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PROCESSID_LIST) + m_appList.size() * sizeof(DWORD);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	int nCount = 0;
	CApplicatonList::iterator it = m_appList.begin();
	while (it != m_appList.end()) {
		pProcessIdList->pdwProcessId[nCount] = (*it)->dwProcessId;
		nCount ++;
		it ++;
	}

	pProcessIdList->dwStudentIp = m_pComLayer->GetLocalAddress();
	pProcessIdList->nCount = nCount;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPassiveApplicationTransaction::SendProcessInfo(PAPPLICATIONDATA pAppData)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PPROCESSINFO_RET pProcessInfoRet = (PPROCESSINFO_RET)(pPacket + 1);

	pPacket->dwMagic = MAGIC_PROCESSINFO_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(PROCESSINFO_RET) + pAppData->cbSize;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pProcessInfoRet->dwStudentIp = m_pComLayer->GetLocalAddress();
	pProcessInfoRet->dwProcessId = pAppData->dwProcessId;
	pProcessInfoRet->cbInfoSize = pAppData->cbSize;
	memcpy(pProcessInfoRet->abInfo, pAppData, pAppData->cbSize);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CWebSiteTransaction::CWebSiteTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	InitializeCriticalSection(&m_cs);
}

CWebSiteTransaction::~CWebSiteTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CWebSiteTransaction::AddStudent(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	CWebSiteMap::iterator it = m_webMap.find(dwIpAddress);
	if (it != m_webMap.end())
		return;

	CWebIDList* pWebList = new CWebIDList();
	if (!pWebList)
		return;

	m_webMap.insert(CWebSiteMap::value_type(dwIpAddress, pWebList));
}

void CWebSiteTransaction::DelStudent(DWORD dwIpAddress)
{
	CAutoSectionLock lock(&m_cs);

	CWebSiteMap::iterator it = m_webMap.find(dwIpAddress);
	if (it == m_webMap.end())
		return;

	CWebIDList* pWebList = (*it).second;
	pWebList->clear();
	delete pWebList;

	m_webMap.erase(it);
}

void CWebSiteTransaction::DeleteAllStudents()
{
	CAutoSectionLock lock(&m_cs);

	CWebSiteMap::iterator it = m_webMap.begin();
	while (it != m_webMap.end()) 
	{
		CWebIDList* pWebList = (*it).second;
		pWebList->clear();
		delete pWebList;
		m_webMap.erase(it);
		
		it = m_webMap.begin();
	}
}

void CWebSiteTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_WEBID_LIST:
		{
			PWEBID_LIST pWebIdList = (PWEBID_LIST)(pPacket + 1);
			ProcessWebIdList(pWebIdList);
		}
		break;
	case MAGIC_WEBINFO_RET:
		{
			PWEBINFO_RET pWebInfoRet = (PWEBINFO_RET)(pPacket + 1);
			ProcessWebInfoRet(pWebInfoRet);
		}
		break;
	}
}

void CWebSiteTransaction::ProcessWebIdList(PWEBID_LIST pWebIdList)
{
	CWebSiteMap::iterator it = m_webMap.find(pWebIdList->dwStudentIp);
	if (it == m_webMap.end())
		return;

	int i=0;
	int nExitCount = 0;
	DWORD pdwExitWebId[256] = {0};

	CWebIDList* pWebsiteIdList = (*it).second;
	CWebIDList::iterator iterator = pWebsiteIdList->begin();
	while (iterator != pWebsiteIdList->end()) {

		// Lock each WEBID in the new WEBID List
		DWORD dwWebID = (*iterator);
		for (i=0; i<pWebIdList->nCount; i++) {
			if (dwWebID == pWebIdList->pdwWebId[i])
				break;
		}

		// Remember the deleted WEBID
		if (i >= pWebIdList->nCount) {
			pdwExitWebId[nExitCount] = dwWebID;
			nExitCount ++;

			pWebsiteIdList->erase(iterator);
			iterator = pWebsiteIdList->begin();
		} else
		{
			iterator ++;
		}
	}

	if (nExitCount > 0) {
		ReportWebExitEvent(pWebIdList->dwStudentIp, nExitCount, pdwExitWebId);
	}

	for (i=0; i<pWebIdList->nCount; i++) {
		if (!WebIDInList(pWebIdList->pdwWebId[i], pWebsiteIdList)) {
			SendWebRequestPacket(pWebIdList->dwStudentIp, pWebIdList->pdwWebId[i]);
		}
	}
}

BOOL CWebSiteTransaction::WebIDInList(DWORD dwWebID, CWebIDList* pWebIdList)
{
	CWebIDList::iterator it = pWebIdList->begin();
	while (it != pWebIdList->end()) {
		if (dwWebID == (*it))
			return TRUE;

		it ++;
	}

	return FALSE;
}

void CWebSiteTransaction::DelWebIDInList(DWORD dwWebID, CWebIDList* pWebIdList)
{
	CWebIDList::iterator it = pWebIdList->begin();
	while (it != pWebIdList->end()) {
		if (dwWebID == (*it)) {
			pWebIdList->erase(it);
			return;
		}

		it ++;
	}
}

void CWebSiteTransaction::ProcessWebInfoRet(PWEBINFO_RET pWebInfoRet)
{
	// Check student
	CWebSiteMap::iterator it = m_webMap.find(pWebInfoRet->dwStudentIp);
	if (it == m_webMap.end())
		return;	

	// Check if the web site has reported it's data
	CWebIDList* pWebIdList = (*it).second;
	CWebIDList::iterator iterator = pWebIdList->begin();
	while (iterator != pWebIdList->end()) {
		if (pWebInfoRet->dwWebId == (*iterator))
			return;

		iterator ++;
	}

	pWebIdList->push_back(pWebInfoRet->dwWebId);

	ReportWebCreateEvent(pWebInfoRet);
}

void CWebSiteTransaction::SendWebRequestPacket(DWORD dwStudentIp, DWORD dwWebId)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(dwStudentIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PWEBINFO_REQ pWebInfoReq = (PWEBINFO_REQ)(pPacket + 1);

	pPacket->dwMagic = MAGIC_WEBINFO_REQ;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(WEBINFO_REQ);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pWebInfoReq->dwWebId = dwWebId;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CWebSiteTransaction::ReportWebCreateEvent(PWEBINFO_RET pWebInfoRet)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_WEB_CREATED);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_WEB_CREATED pEvent = (PEVENT_WEB_CREATED)(pHeader + 1);

	pHeader->dwEvent = EV_WEBSITE_CREATED;
	pHeader->dwLength = sizeof(EVENT_WEB_CREATED);

	pEvent->dwStudentIp = pWebInfoRet->dwStudentIp;
	pEvent->dwWebId = pWebInfoRet->dwWebId;
	pEvent->cbInfoSize = pWebInfoRet->cbInfoSize;
	pEvent->pbInfo = (BYTE *)malloc(pWebInfoRet->cbInfoSize);
	if (pEvent->pbInfo) {
		memcpy(pEvent->pbInfo, pWebInfoRet->abInfo, pWebInfoRet->cbInfoSize);
	}

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();	
}

void CWebSiteTransaction::ReportWebExitEvent(DWORD dwStudentIp, int nExitCount, DWORD* pdwExitWebId)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_WEB_EXITED) + nExitCount * sizeof(DWORD);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	PEVENT_WEB_EXITED pEvent = (PEVENT_WEB_EXITED)(pHeader + 1);

	pHeader->dwEvent = EV_WEBSITE_EXITED;
	pHeader->dwLength = sizeof(EVENT_WEB_EXITED) + nExitCount * sizeof(DWORD);

	pEvent->dwStudentIp = dwStudentIp;
	pEvent->dwCount = (DWORD)nExitCount;
	memcpy(pEvent->pdwWebIdList, pdwExitWebId, nExitCount * sizeof(DWORD));

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();	
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CPassiveWebSiteTransaction::CPassiveWebSiteTransaction(
	ICommunicationLayer* pComLayer, GUID *pTransID) :
		CTransactionHandler(pComLayer, pTransID)
{
	m_bStarted = FALSE;
	m_dwTeacherIp = 0;
	m_nUpdateSeconds = 5;

	InitializeCriticalSection(&m_cs);
}

CPassiveWebSiteTransaction::~CPassiveWebSiteTransaction()
{
	DeleteCriticalSection(&m_cs);
}

void CPassiveWebSiteTransaction::Startup(DWORD dwTeacherIp, int nUpdateSeconds)
{
	CAutoSectionLock lock(&m_cs);

	m_bStarted = TRUE;
	m_dwTeacherIp = dwTeacherIp;
	m_nUpdateSeconds = nUpdateSeconds;

	m_pComLayer->RegisterTimeout(m_pTransID, m_nUpdateSeconds * 1000, 0);	
}

void CPassiveWebSiteTransaction::Shutdown()
{
	CAutoSectionLock lock(&m_cs);
	m_bStarted = FALSE;

	CWebSiteList::iterator it = m_webList.begin();
	while (it != m_webList.end()) {
		free((*it));
		m_webList.erase(it);
		it = m_webList.begin();
	}
}

void CPassiveWebSiteTransaction::SendWebSiteList(LPVOID pvWebItem, DWORD nCount)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	PWEBSITEITEM pWebItem = (PWEBSITEITEM)pvWebItem;

	// Clear Application List
	CWebSiteList::iterator it = m_webList.begin();
	while (it != m_webList.end()) {
		free((*it));
		m_webList.erase(it);
		it = m_webList.begin();
	}

	BYTE abBuffer[2048];

	for (DWORD i=0; i<nCount; i++) 
	{
		memset(abBuffer, 0, 2048);
		PWEBSITEDATA pWebDataTemp = (PWEBSITEDATA)abBuffer;

		pWebDataTemp->cbSize = sizeof(WEBSITEDATA);
		pWebDataTemp->dwWebId = pWebItem[i].dwWebId;

		pWebDataTemp->wURLSize = (_tcslen(pWebItem[i].szURL) + 1) * sizeof(TCHAR);
		pWebDataTemp->cbSize += pWebDataTemp->wURLSize;
		pWebDataTemp->wURLOffset = 0;
		memcpy(pWebDataTemp->abParams + pWebDataTemp->wURLOffset, pWebItem[i].szURL, pWebDataTemp->wURLSize);

		pWebDataTemp->wNameSize = (_tcslen(pWebItem[i].szName) + 1) * sizeof(TCHAR);
		pWebDataTemp->cbSize += pWebDataTemp->wNameSize;
		pWebDataTemp->wNameOffset = pWebDataTemp->wURLSize;
		memcpy(pWebDataTemp->abParams + pWebDataTemp->wNameOffset, pWebItem[i].szName, pWebDataTemp->wNameSize);

		pWebDataTemp->wIconSize = 0;
		pWebDataTemp->cbSize += pWebDataTemp->wIconSize;
		pWebDataTemp->wIconOffset = pWebDataTemp->wURLSize + pWebDataTemp->wNameSize;
			
		PWEBSITEDATA pWebData = (PWEBSITEDATA)malloc(pWebDataTemp->cbSize);
		if (pWebData) {
			memcpy(pWebData, pWebDataTemp, pWebDataTemp->cbSize);
			m_webList.push_back(pWebData);
		}
	}

	SendWebIdList();
}

void CPassiveWebSiteTransaction::OnTimeout(DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	if (0 == dwTimeID)
	{
		ReportUpdateWebListEvent();
		m_pComLayer->RegisterTimeout(m_pTransID, m_nUpdateSeconds * 1000, 0);	
	}
}

void CPassiveWebSiteTransaction::OnReceiveComplete(LPVOID lpvData, DWORD cbData)
{
	CAutoSectionLock lock(&m_cs);

	if (!m_bStarted)
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpvData;
	switch (pPacket->dwMagic)
	{
	case MAGIC_WEBINFO_REQ:
		{
			PWEBINFO_REQ pWebInfoReq = (PWEBINFO_REQ)(pPacket + 1);
			ProcessWebInfoRequest(pWebInfoReq);
		}
		break;
	}
}

void CPassiveWebSiteTransaction::ProcessWebInfoRequest(PWEBINFO_REQ pWebInfoReq)
{
	CWebSiteList::iterator it = m_webList.begin();
	while (it != m_webList.end()) {
		PWEBSITEDATA pWebData = (*it);
		if (pWebData->dwWebId == pWebInfoReq->dwWebId) {
			SendWebInfo(pWebData);
			break;
		}

		it ++;
	}
}

void CPassiveWebSiteTransaction::ReportUpdateWebListEvent()
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER);
	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_UPDATE_WEBLIST;
	pHeader->dwLength = 0;

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

void CPassiveWebSiteTransaction::SendWebIdList()
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PWEBID_LIST pWebIdList = (PWEBID_LIST)(pPacket + 1);

	pPacket->dwMagic = MAGIC_WEBID_LIST;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(WEBID_LIST) + m_webList.size() * sizeof(DWORD);
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	int nCount = 0;
	CWebSiteList::iterator it = m_webList.begin();
	while (it != m_webList.end()) {
		pWebIdList->pdwWebId[nCount] = (*it)->dwWebId;
		nCount ++;
		it ++;
	}

	pWebIdList->dwStudentIp = m_pComLayer->GetLocalAddress();
	pWebIdList->nCount = nCount;

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}

void CPassiveWebSiteTransaction::SendWebInfo(PWEBSITEDATA pWebData)
{
	IBufferItem *pItem = m_pComLayer->AllocBufferItem();
	if (NULL == pItem)
		return;

	pItem->SetTransactionID(m_pTransID);
	pItem->SetDstIpAddress(CIpAddress(m_dwTeacherIp, m_pComLayer->GetComPort()));

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)pItem->GetData();
	PWEBINFO_RET pWebInfoRet = (PWEBINFO_RET)(pPacket + 1);

	pPacket->dwMagic = MAGIC_WEBINFO_RET;
	pPacket->dwVersion = LANGLAB_VERSION;
	pPacket->dwLength = sizeof(WEBINFO_RET) + pWebData->cbSize;
	memcpy(&pPacket->guidTransaction, m_pTransID, sizeof(GUID));

	pWebInfoRet->dwStudentIp = m_pComLayer->GetLocalAddress();
	pWebInfoRet->dwWebId = pWebData->dwWebId;
	pWebInfoRet->cbInfoSize = pWebData->cbSize;
	memcpy(pWebInfoRet->abInfo, pWebData, pWebData->cbSize);

	pItem->SetLength(sizeof(LANGLAB_PACKET) + pPacket->dwLength);
	m_pComLayer->AddToSendBuffer(pItem);
}
