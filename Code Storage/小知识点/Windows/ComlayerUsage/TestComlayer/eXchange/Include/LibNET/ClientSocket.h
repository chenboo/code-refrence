// ClientSocket.h: interface for the CClientSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTSOCKET_H__74CD611D_4066_42B2_ACDD_5EAE93DB789E__INCLUDED_)
#define AFX_CLIENTSOCKET_H__74CD611D_4066_42B2_ACDD_5EAE93DB789E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "Utils.h"
#include "TimeoutHeap.h"

class LIBNET_API CClientSocket  
{
// Construction & Destruction
public:
	CClientSocket();
	virtual ~CClientSocket();

// Attributes
public:
	SOCKET GetHandle() {
		return m_hSocket;
	}

// Overrides
public:
	virtual int  Create();
	virtual void Destroy();

	virtual int Connect(DWORD dwIPAddr, WORD wPort, DWORD dwTimeout = 1000);

	virtual int Send(LPVOID lpvBuffer, DWORD cbToSend, DWORD dwTimeout = 5000);
	virtual int Receive(LPVOID lpvBuffer, DWORD cbToRecv, DWORD dwTimeout = 5000);

// Operations
public:
	int	 Bind(DWORD dwIPAddr, WORD wPort);
	int  SetReuseAddr(BOOL bReuse);
	int  SetSendBufferSize(int nSize);
	int  SetRecvBufferSize(int nSize);
	int  SetNoDelay(BOOL bDelay);
	int  SetKeepAlive(BOOL bKeepAlive);
	int  Shutdown();

	int  Send(LPVOID lpvBuffer, DWORD cbToSend, DWORD *pcbSend, DWORD dwTimeout = 5000);
	int  Receive(LPVOID lpvBuffer, DWORD cbToRecv, DWORD *pcbRecv, DWORD dwTimeout = 5000);

// Implementation
protected:
	int	 GetErrorNo() {
		int nError = WSAGetLastError();
		return (nError == 0) ? WSABASEERR : nError;
	}

	SOCKET m_hSocket;
};

class LIBNET_API CAsyncClientSocket : public CClientSocket
{
// Construction & Destruction
public:
	CAsyncClientSocket();
	virtual ~CAsyncClientSocket();

// Attributes
public:
	enum STATE {
		STATE_UNINITIALIZED,
		STATE_INITIAL,
		STATE_CONNECTING,
		STATE_CONNECTED
	};

	enum IOSTATE {
		IOSTATE_SENDING = 1,
		IOSTATE_RECEIVING = 2
	};

	STATE GetSocketState();
	int   GetSocketIOState();

// Operations
public:
	class ICallback
	{
		friend class CAsyncClientSocket;

	protected:
		virtual void OnConnectSuccessful() {}
		virtual void OnConnectFailed(int nError) {}

		virtual void OnSendSuccessful() {}
		virtual void OnSendFailed(int nError) {}

		virtual void OnReceiveSuccessful() {}
		virtual void OnReceiveFailed(int nError) {}

		virtual void OnShutdownSuccessful() {}
		virtual void OnShutdownFailed(int nError) {}

		virtual void OnDataArrived(LPVOID lpvData, int cbData) {}
		virtual void OnDataSended(LPVOID lpvData, int cbData) {}

		virtual void OnTimeout(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3) {}

		virtual DWORD OnRequest(DWORD dwRequest, DWORD dwParam1, DWORD dwParam2) { return 0; }

		virtual void OnSendSuccessfulEx(LPVOID lpvData, int cbData) {}
		virtual void OnSendAborted(LPVOID lpvData, int cbData) {}
	};

	void SetCallback(ICallback *pCallback) {
		CAutoSectionLock lock(&m_csCallback);
		m_pCallback = pCallback;
	}

	BOOL Initialize();
	void Deinitialize();

	virtual int  Create();
	virtual void Destroy();

	virtual int Connect(DWORD dwIPAddr, WORD wPort);
	virtual int Send(LPVOID lpvBuffer, DWORD cbToSend);
	virtual int Receive(LPVOID lpvBuffer, DWORD cbToRecv);

	virtual void RegisterTimeout(DWORD dwTimeout, DWORD dwParam1 = 0, DWORD dwParam2 = 0, DWORD dwParam3 = 0);
	virtual void UnregisterTimeout(DWORD dwParam1);
	virtual void UnregisterTimeout(DWORD dwParam1, DWORD dwParam2);

	virtual BOOL SyncRequest(DWORD dwRequest, DWORD dwParam1 = 0, DWORD dwParam2 = 0, DWORD *pdwRet = NULL);
	virtual BOOL AsyncRequest(DWORD dwRequest, DWORD dwParam1 = 0, DWORD dwParam2 = 0, DWORD *pdwRet = NULL, HANDLE hEvNotify = NULL);

	virtual int  GetSendQueueCount();

	BOOL InProcessThread() {
		return (GetCurrentThreadId() == m_dwThreadId);
	}

// Implementation
protected:
	static DWORD WINAPI ThreadProc(LPVOID lpvParam);
	virtual DWORD ThreadProc();

	void OnConnectSuccessful()
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnConnectSuccessful();
	}

	void OnConnectFailed(int nError)
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnConnectFailed(nError);
	}

	void OnSendSuccessful()
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnSendSuccessful();
	}

	void OnSendSuccessfulEx(LPVOID lpvData, int cbData)
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnSendSuccessfulEx(lpvData, cbData);
	}

	void OnSendAborted(LPVOID lpvData, int cbData)
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnSendAborted(lpvData, cbData);
	}

	void OnSendFailed(int nError)
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnSendFailed(nError);
	}

	void OnReceiveSuccessful()
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnReceiveSuccessful();
	}

	void OnReceiveFailed(int nError)
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnReceiveFailed(nError);
	}

	void OnShutdownSuccessful()
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnShutdownSuccessful();
	}

	void OnShutdownFailed(int nError)
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnShutdownFailed(nError);
	}

	void OnDataArrived(LPVOID lpvData, int cbData) 
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnDataArrived(lpvData, cbData);
	}

	void OnDataSended(LPVOID lpvData, int cbData) 
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnDataSended(lpvData, cbData);
	}

	void OnTimeout(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
	{
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			m_pCallback->OnTimeout(dwParam1, dwParam2, dwParam3);
	}

	DWORD OnRequest(DWORD dwRequest, DWORD dwParam1, DWORD dwParam2) 
	{ 
		CAutoSectionLock lock(&m_csCallback);
		if (m_pCallback)
			return m_pCallback->OnRequest(dwRequest, dwParam1, dwParam2);
		else
			return 0;
	}

	void TrySend();
	void TryReceive();

protected:
	typedef struct _QUEUE_ITEM
	{
		LPBYTE lpbSendData;
		int	   cbSendData;
	} QUEUE_ITEM, *PQUEUE_ITEM;

	typedef std::list<QUEUE_ITEM> CQueueList;

	typedef struct _REQUEST_ITEM
	{
		DWORD	dwRequest;
		DWORD	dwParam1;
		DWORD	dwParam2;
		DWORD  *pdwRet;
		HANDLE  hEvComplete;
		BOOL	bAutoDelete;
	} REQUEST_ITEM, *PREQUEST_ITEM;

	typedef std::list<PREQUEST_ITEM> CRequestList;

	LPBYTE				m_lpbSendBuffer;
	int					m_cbSendBuffer;
	int					m_cbSended;

	LPBYTE				m_lpbReceiveBuffer;
	int					m_cbReceiveBuffer;
	int					m_cbReceived;

	STATE				m_nState;
	int					m_nIOState;

	ICallback *			m_pCallback;
	CRITICAL_SECTION	m_csCallback;

	HANDLE				m_hThread;
	HANDLE				m_hEvSocket;
	HANDLE				m_hEvSignal;
	HANDLE				m_hEvComplete;

	BOOL				m_bRunning;
	CRITICAL_SECTION	m_csState;

	CTimeoutHeap *		m_pTimeoutHeap;
	CRITICAL_SECTION	m_csTimeout;

	CRequestList *		m_pRequestList;
	CQueueList	 *		m_pQueueList;

	DWORD				m_dwThreadId;
	CRITICAL_SECTION	m_csRequest;
};

class LIBNET_API CStreamRingBuffer
{
public:
	CStreamRingBuffer() {
		m_pbBuffer = NULL;
		m_cbBuffer = 0;
		m_nReadPos = m_nWritePos = 0;
		m_cbData = 0;

		InitializeCriticalSection(&m_csLock);
	}

	virtual ~CStreamRingBuffer() {
		Destroy();
		DeleteCriticalSection(&m_csLock);
	}

	BOOL Create(int nBufferSize);
	void Destroy();

public:
	int GetBufferSize() {
		CAutoSectionLock lock(&m_csLock);

		return m_cbBuffer;
	}

	int GetNextFreeRegion(LPVOID *ppv) {
		CAutoSectionLock lock(&m_csLock);

		if (GetFreeLength() <= 0)
			return 0;

		*ppv = (LPVOID)(m_pbBuffer + m_nWritePos);
		if (m_nWritePos >= m_nReadPos) {
			return m_cbBuffer - m_nWritePos;
		}
		else {
			return m_nReadPos - m_nWritePos;
		}
	}

	int GetFreeLength() {
		CAutoSectionLock lock(&m_csLock);
		
		return m_cbBuffer - m_cbData;
	}

	BOOL AppendData(int cbData) {
		CAutoSectionLock lock(&m_csLock);

		if (cbData > GetFreeLength())
			return FALSE;
		
		m_nWritePos = (m_nWritePos + cbData) % m_cbBuffer;
		m_cbData += cbData;

		return TRUE;
	}

	int GetDataLength() {
		CAutoSectionLock lock(&m_csLock);
		return m_cbData;
	}

	LPVOID PeekData(int cbData) {
		CAutoSectionLock lock(&m_csLock);

		if (cbData > m_cbData)
			return NULL;

		int cbData1 = 0, cbData2 = 0;
		LPBYTE pbData1 = m_pbBuffer + m_nReadPos;
		LPBYTE pbData2 = NULL;
		if (m_nWritePos > m_nReadPos) {
			cbData1 = m_nWritePos - m_nReadPos;
		}
		else {
			cbData1 = m_cbBuffer - m_nReadPos;
			pbData2 = m_pbBuffer;
			cbData2 = m_nWritePos;
		}

		if (cbData > cbData1 && pbData2 != NULL) {
			memcpy(m_pbBuffer + m_cbBuffer, pbData2, cbData - cbData1);
		}

		return pbData1;
	}

	BOOL AdvanceReadPos(int cbData) {
		CAutoSectionLock lock(&m_csLock);

		if (cbData > m_cbData)
			return FALSE;

		m_nReadPos = (m_nReadPos + cbData) % m_cbBuffer;
		m_cbData -= cbData;
		return TRUE;
	}

	BOOL Resize(int cbBuffer);

protected:
	LPBYTE				m_pbBuffer;
	int					m_cbBuffer;
	int					m_nReadPos;
	int					m_nWritePos;
	int					m_cbData;

	CRITICAL_SECTION	m_csLock;
};

#endif // !defined(AFX_CLIENTSOCKET_H__74CD611D_4066_42B2_ACDD_5EAE93DB789E__INCLUDED_)
