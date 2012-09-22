#include "stdafx.h"
#include "CommunicationLayer.h"

CCommunicationLayer::CCommunicationLayer(IEventItemManager* pEventManager)
{
	m_hWorkThread = NULL;
	m_hEvQuit = NULL;
	m_pCurrentSend = NULL;

	InitializeCriticalSection(&m_csTimeout);
	InitializeCriticalSection(&m_csCurrentSend);

	m_pEventManager = pEventManager;
	m_pEventManager->AddRef();

	m_pBufferManager = new CBufferItemManager(MAX_PACKETS_SIZE);
	m_pTransanctionMgr = new CTransactionManager();
	m_pPacketHandlerMgr = new CPacketHandlerManager();

	m_nRef = 1;
}

CCommunicationLayer::~CCommunicationLayer() 
{
	Shutdown();

	DeleteCriticalSection(&m_csTimeout);
	DeleteCriticalSection(&m_csCurrentSend);

	m_pEventManager->Release();

	m_pBufferManager->Release();
	m_pTransanctionMgr->Release();
	m_pPacketHandlerMgr->Release();
}

IEventItemManager* CCommunicationLayer::GetEventManager()
{
	m_pEventManager->AddRef();
	return m_pEventManager;
}

IPacketHandlerManager* CCommunicationLayer::GetPacketHandlerManager()
{
	m_pPacketHandlerMgr->AddRef();
	return (IPacketHandlerManager *)m_pPacketHandlerMgr;
}

ITransactionManager* CCommunicationLayer::GetTransactionManager()
{
	m_pTransanctionMgr->AddRef();
	return (ITransactionManager *)m_pTransanctionMgr;
}

LONG CCommunicationLayer::AddRef()
{
	return InterlockedIncrement(&m_nRef);
}

LONG CCommunicationLayer::Release()
{
	LONG nRet = InterlockedDecrement(&m_nRef);
	if (nRet <= 0)
		delete this;

	return nRet;
}

DWORD CCommunicationLayer::GetLocalAddress()
{
	return m_dwLocalAddr;
}

DWORD CCommunicationLayer::GetComAddress()
{
	return m_dwComAddr;
}

WORD CCommunicationLayer::GetComPort()
{
	return m_wComPort;
}

BOOL CCommunicationLayer::Startup(DWORD dwLocalAddress, DWORD dwComAddress, WORD wComPort)
{
	if (m_hWorkThread)
		return TRUE;

	try
	{
		m_dwLocalAddr = dwLocalAddress;
		m_dwComAddr = dwComAddress;
		m_wComPort = wComPort;

		m_hEvQuit = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == m_hEvQuit)
			throw FALSE;

		// Create socket
		if (!m_socketSend.Create(CIpAddress(m_dwLocalAddr, 0)))
			throw FALSE;
		m_socketSend.SetSendBufferSize(65536);
		m_socketSend.SetLoopback(FALSE);
		m_socketSend.SetTTL(64);

		if ((!m_socketRecv.Create(CIpAddress(m_dwLocalAddr, m_wComPort))) ||
			(!m_socketRecv.JoinMulticastGroup(CIpAddress(m_dwComAddr, m_wComPort))))
			throw FALSE;

		m_socketRecv.SetRecvBufferSize(65536);

		// Create thread
		DWORD dwThreadID = 0;
		m_hWorkThread = CreateThread(NULL, 0, ComThreadProc, this, CREATE_SUSPENDED, &dwThreadID);
		if (NULL == m_hWorkThread) {
			throw FALSE;
		}

		SetThreadPriority(m_hWorkThread, THREAD_PRIORITY_HIGHEST);
		ResumeThread(m_hWorkThread);

	} catch (...)
	{
		if (m_hEvQuit) {
			CloseHandle(m_hEvQuit);
			m_hEvQuit = NULL;
		}

		m_socketSend.Destroy();
		m_socketRecv.Destroy();

		return FALSE;
	}

	return TRUE;
}

void CCommunicationLayer::Shutdown()
{
	if (!m_hWorkThread)
		return;

	// Destroy thread
	SetEvent(m_hEvQuit);
	WaitForSingleObject(m_hWorkThread, INFINITE);
	CloseHandle(m_hWorkThread);
	m_hWorkThread = NULL;

	// Destroy socket
	m_socketSend.Destroy();
	m_socketRecv.Destroy();

	CloseHandle(m_hEvQuit);
	m_hEvQuit = NULL;

	m_timeoutHeap.DeleteAll();
}

BOOL CCommunicationLayer::RegisterTimeout(GUID *pID, DWORD dwTimeout, DWORD dwTimeID)
{
	if (!m_hWorkThread)
		return FALSE;

	CAutoSectionLock lock(&m_csTimeout);
	return m_timeoutHeap.Insert(dwTimeout + GetTickCount(), 
		(DWORD)pID, dwTimeID, 0);
}

void CCommunicationLayer::UnregisterTimeout(GUID *pID, DWORD dwTimeID)
{
	CAutoSectionLock lock(&m_csTimeout);
	m_timeoutHeap.Delete((DWORD)pID, dwTimeID, 0, TIMEOUT_HEAP_PARAM1 | TIMEOUT_HEAP_PARAM2);
}

IBufferItem* CCommunicationLayer::AllocBufferItem()
{
	return m_pBufferManager->GetFreeBufferItem();
}

BOOL CCommunicationLayer::AddToSendBuffer(IBufferItem *pBufferItem)
{
	CAutoSectionLock lock(&m_csCurrentSend);
	m_pBufferManager->PutPendingBufferItem((CBufferItem *)pBufferItem);

	return TRUE;
}

BOOL CCommunicationLayer::AddToSendBufferInternal(CBufferItem *pItem)
{
	CAutoSectionLock lock(&m_csCurrentSend);

	if (NULL == m_pCurrentSend) {
		m_pCurrentSend = pItem;

		return Send(pItem->GetData(), 
			pItem->GetLength(), pItem->GetDstIpAddress(), pItem->GetTransactionID());
	}
	else {
		m_pBufferManager->PutPendingBufferItem(pItem);
	}
	
	return TRUE;
}

BOOL CCommunicationLayer::ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize)
{
	if (cbSize < sizeof(LANGLAB_PACKET))
		return FALSE;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpcBuffer;
	if (pPacket->dwVersion != LANGLAB_VERSION)
		return FALSE;

	if (cbSize < (sizeof(LANGLAB_PACKET) + pPacket->dwLength))
		return FALSE;

	CTransactionHandler* pTransactionHandler = (CTransactionHandler *)
		m_pTransanctionMgr->FindTransaction(&pPacket->guidTransaction);

	if (pTransactionHandler) {
		pTransactionHandler->OnReceiveComplete((LPVOID)lpcBuffer, cbSize);
		pTransactionHandler->Release();
		return TRUE;
	}
	

	CPacketHandler* pPacketHandler = m_pPacketHandlerMgr->FindPacketHandler(pPacket->dwMagic);
	if (pPacketHandler) {
		pPacketHandler->ProcessPacket(lpcBuffer, cbSize);
		pPacketHandler->Release();
		return TRUE;
	}

	return FALSE;
}

BOOL CCommunicationLayer::Send(LPCVOID lpBuffer, DWORD cbBuffer, CIpAddress ipAddrDst, GUID *pID)
{
	DWORD cbSended;
	return m_socketSend.Send(lpBuffer, cbBuffer, cbSended, ipAddrDst);
}

DWORD WINAPI CCommunicationLayer::ComThreadProc(LPVOID lpvParam)
{
	CCommunicationLayer *pThis = (CCommunicationLayer *)lpvParam;
	return pThis->ThreadProc();
}	

DWORD CCommunicationLayer::ThreadProc()
{
	char szReceiveBuffer[MAX_PACKETS_SIZE];
	DWORD cbReceived;
	DWORD dwTimeout;

	CNetworkEvent networkEvent;
	networkEvent.Create();
	networkEvent.Select(m_socketRecv.GetHandle(), FD_READ);

	HANDLE ahEvents[2];
	ahEvents[0] = m_hEvQuit;
	ahEvents[1] = networkEvent.GetHandle();

	srand( (unsigned)time(NULL));
	rand();

	BOOL bRunning = TRUE;
	while (bRunning)
	{
		DWORD dwRet = WSAWaitForMultipleEvents(sizeof(ahEvents) / sizeof(HANDLE), ahEvents,
			FALSE, 20, FALSE);

		switch (dwRet) 
		{
		case WAIT_OBJECT_0:
			bRunning = FALSE;
			break;

		case WAIT_OBJECT_0 + 1:
			{
				networkEvent.Reset();

				CIpAddress ipAddrSrc;
				if (!m_socketRecv.Receive(szReceiveBuffer, sizeof(szReceiveBuffer), cbReceived, ipAddrSrc))
					break;

				if ((ipAddrSrc.GetIpAddress() == m_dwLocalAddr) || 
					(ipAddrSrc.GetIpAddress() == 0))
					break;

				ProcessPacket(szReceiveBuffer, cbReceived);
			}

			break;
		}

		// Send packet
		{
			CAutoSectionLock lock(&m_csCurrentSend);

			m_pCurrentSend = m_pBufferManager->GetItem();
			while (bRunning && m_pCurrentSend) {
				Send(m_pCurrentSend->GetData(), 
					m_pCurrentSend->GetLength(), m_pCurrentSend->GetDstIpAddress(), m_pCurrentSend->GetTransactionID());

				CTransactionHandler* pTransactionHandler = (CTransactionHandler *)
					m_pTransanctionMgr->FindTransaction(m_pCurrentSend->GetTransactionID());

				if (pTransactionHandler) {
					pTransactionHandler->OnSendComplete(m_pCurrentSend->GetData(), m_pCurrentSend->GetLength());
					pTransactionHandler->Release();
				}

				m_pBufferManager->RecycleBufferItem(m_pCurrentSend);

				m_pCurrentSend = m_pBufferManager->GetItem();
			}
		}

		// Cheak timeout
		{
			CAutoSectionLock lock(&m_csTimeout);

			while (bRunning && m_timeoutHeap.PeekMin(&dwTimeout, NULL, NULL, NULL)) {
				if (dwTimeout <= GetTickCount()) {
					DWORD dwParam1, dwParam2, dwParam3;
					m_timeoutHeap.ExtractMin(&dwTimeout, &dwParam1, &dwParam2, &dwParam3);

					GUID *pGUID = (GUID *)dwParam1;
					CTransactionHandler *pHandler = m_pTransanctionMgr->FindTransaction(pGUID);
					if (pHandler) {
						pHandler->OnTimeout(dwParam2);
						pHandler->Release();
					}
				} else
				{
					break;
				}
			}
		}

		m_pTransanctionMgr->DoPendingDeletion();
	}

	return 0;
}