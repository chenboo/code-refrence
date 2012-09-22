#ifndef __UDP_FRAMEWORK_H__
#define __UDP_FRAMEWORK_H__

#include "IpAddress.h"
#include "UDPSocket.h"
#include "Heap.h"
#include <map>

class CUDPEndpointTimer
{
public:
	static const DWORD INVALID_TIMEOUT_ID	= -1;
	static const DWORD MAX_TIMEOUT			= (1U << 31);

public:
	CUDPEndpointTimer();
	virtual ~CUDPEndpointTimer();

public:
	DWORD AddTimeout(DWORD dwTimeout, LPVOID pvParam);
	BOOL RemoveTimeout(DWORD dwTimeoutID);
	void RemoveAllTimeout();

public:
	DWORD GetMinTimeout(DWORD& dwTimeout, LPVOID& pvParam);
	DWORD PeekMinTimeout(DWORD& dwTimeout, LPVOID& pvParam);

protected:
	typedef struct _TIMEOUT_ITEM {
		DWORD	dwTimeoutID;
		LPVOID	pvParam;
	} TIMEOUT_ITEM, *PTIMEOUT_ITEM;

	typedef CHeap<DWORD, TIMEOUT_ITEM>		CTimeoutHeap;
	typedef CTimeoutHeap::PHEAP_ENTRY		PHEAP_ENTRY;
	typedef std::map<DWORD, PHEAP_ENTRY>	CTimeoutIDMap;

	CTimeoutHeap	m_heap;
	CTimeoutIDMap	m_mapTimeoutIDs;
	DWORD			m_dwCurrentTimeoutID;
};

class CUDPEndpoint
{
public:
	CUDPEndpoint();
	virtual ~CUDPEndpoint();

public:
	BOOL  Create(WORD cbMaxPacketSize, DWORD cbReceivePoolSize, WORD wPort);
	void  Destroy();

	BOOL  JoinMulticastGroup(const CIpAddress& ipAddrMulticast, const CIpAddress& ipAddrLocal);
	BOOL  LeaveMulticastGroup();

	BOOL  EnableBroadcast();
	BOOL  DisableBroadcast();

	BOOL  GetSendBufferSize(int &nSize) const;
	BOOL  SetSendBufferSize(int nSize);

	BOOL  GetRecvBufferSize(int &nSize) const;
	BOOL  SetRecvBufferSize(int nSize);

	BOOL  GetTTL(int &nTTL) const;
	BOOL  SetTTL(int nTTL);

	BOOL  GetLoopback(BOOL &bLoopback) const;
	BOOL  SetLoopback(BOOL bLoopback);

	BOOL  Send(const CIpAddress& ipAddrDest, LPCVOID pvPacket, int cbPacket, DWORD dwTimeout, LPVOID pvParam);

	DWORD AddTimeout(DWORD dwTimeoutMS, LPVOID pvParam);
	BOOL  RemoveTimeout(DWORD dwTimeoutID);
	void  RemoveAllTimeout();

protected:
	static DWORD WINAPI NetworkThreadProc(LPVOID pvParam);
	static DWORD WINAPI ProcessThreadProc(LPVOID pvParam);

	virtual DWORD NetworkThreadProc();
	virtual DWORD ProcessThreadProc();

	// Called in send thread context
	virtual BOOL OnPacketPreSend(const CIpAddress& ipAddrDest, LPCVOID pvPacket, int cbPacket, LPVOID pvParam) { return TRUE; }

	// Called in process thread context
	virtual void OnPacketReceived(LPCVOID pvPacket, int cbPacket, DWORD dwReceiveTime, const CIpAddress& ipAddrPeer) {}
	virtual void OnTimeout(DWORD dwTimeoutID, LPVOID pvParam) {}

protected:
	CUDPSocket			m_sock;
	HANDLE				m_hThread;
	HANDLE				m_hEvSendEnabled;
	CUDPEndpointTimer	m_timer;
};

inline BOOL  CUDPEndpoint::JoinMulticastGroup(const CIpAddress& ipAddrMulticast, const CIpAddress& ipAddrLocal)
{
	return m_sock.JoinMulticastGroup(ipAddrMulticast, ipAddrLocal);
}

inline BOOL  CUDPEndpoint::LeaveMulticastGroup()
{
	m_sock.LeaveMulticastGroup();
}

inline BOOL  CUDPEndpoint::EnableBroadcast()
{
	return m_sock.EnableBroadcast();
}

inline BOOL  CUDPEndpoint::DisableBroadcast()
{
	return m_sock.DisableBroadcast();
}

inline BOOL  CUDPEndpoint::GetSendBufferSize(int &nSize) const 
{
	return m_sock.GetSendBufferSize(nSize);
}

inline BOOL  CUDPEndpoint::SetSendBufferSize(int nSize)
{
	return m_sock.SetSendBufferSize(nSize);
}

inline BOOL  CUDPEndpoint::GetRecvBufferSize(int &nSize) const
{
	return m_sock.GetRecvBufferSize(nSize);
}

inline BOOL  CUDPEndpoint::SetRecvBufferSize(int nSize)
{
	return m_sock.SetRecvBufferSize(nSize);
}

inline BOOL  CUDPEndpoint::GetTTL(int &nTTL) const
{
	return m_sock.GetTTL(nTTL);
}

inline BOOL  CUDPEndpoint::SetTTL(int nTTL)
{
	return m_sock.SetTTL(nTTL);
}

inline BOOL  CUDPEndpoint::GetLoopback(BOOL &bLoopback) const 
{
	return m_sock.GetLoopback(bLoopback);
}

inline BOOL  CUDPEndpoint::SetLoopback(BOOL bLoopback)
{
	return m_sock.SetLoopback(bLoopback);
}

#endif //__UDP_FRAMEWORK_H__