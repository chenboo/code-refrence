#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

#include "IpAddress.h"
#include "NetworkEvent.h"

class LIBNET_API CUDPSocket
{
// Constructions & Destructions
public:
	CUDPSocket();
	CUDPSocket(const CUDPSocket &rValue);

	virtual ~CUDPSocket();

// Attributes
public:
	BOOL JoinMulticastGroup(const CIpAddress &ipAddrMulticast);
	BOOL JoinMulticastGroup(const CIpAddress &ipAddrMulticast, const CIpAddress &ipAddrLocal);
	BOOL LeaveMulticastGroup();

	BOOL EnableBroadcast();
	BOOL DisableBroadcast();

	BOOL GetSendBufferSize(int &nSize) const;
	BOOL SetSendBufferSize(int nSize);

	BOOL GetRecvBufferSize(int &nSize) const;
	BOOL SetRecvBufferSize(int nSize);

	BOOL GetTTL(int &nTTL) const;
	BOOL SetTTL(int nTTL);
	BOOL GetLoopback(BOOL &bLoopback) const;
	BOOL SetLoopback(BOOL bLoopback);

	BOOL GetLocalInterface(CIpAddress &ipAddrLocal) const;

	const SOCKET GetHandle() const;
	operator const SOCKET() const;

// Operations
public:
	const CUDPSocket & operator =(const CUDPSocket &rValue);

	BOOL Create(const CIpAddress &ipAddrLocal);
	BOOL Destroy();

	BOOL Send(LPCVOID lpvBuffer, DWORD cbBuffer, DWORD &cbSend, const CIpAddress &ipAddrDest);
	BOOL Receive(LPCVOID lpvBuffer, DWORD cbBuffer, DWORD &cbReceived, CIpAddress &ipAddrSrc);

	BOOL SendEx(LPWSABUF lpBuffers, DWORD dwBufferCount, DWORD &cbSend, const CIpAddress &ipAddrDest);
	BOOL ReceiveEx(LPWSABUF lpBuffers, DWORD dwBufferCount, DWORD &cbReceived, CIpAddress &ipAddrSrc);

// Priority Send
public:
	typedef enum _PRIORITY {
		PRIORITY_MIN,
		PRIORITY_LOWEST = PRIORITY_MIN, 
		PRIORITY_LOW,
		PRIORITY_NORMAL,
		PRIORITY_HIGH,
		PRIORITY_HIGHEST,
		PRIORITY_MAX = PRIORITY_HIGHEST,
		PRIORITY_COUNT
	} PRIORITY, *PPRIORITY;

	static BOOL EnablePrioritySend();
	static void DisablePrioritySend();

	BOOL PrioritySend(LPCVOID lpvBuffer, DWORD cbBuffer, DWORD &cbSend, const CIpAddress &ipAddrDest, PRIORITY priority);

// Overrides
protected:

// Implementations
protected:
	BOOL SendInternal(LPCVOID lpvBuffer, DWORD cbBuffer, DWORD &cbSend, const CIpAddress &ipAddrDest);

	SOCKET		m_hSocket;
	CIpAddress	m_ipAddressLocal;
	CIpAddress  m_ipAddressMulticast;
	CIpAddress  m_ipAddressMulticastLocal;
};

inline CUDPSocket::CUDPSocket()
{
	m_hSocket = INVALID_SOCKET;
}

inline CUDPSocket::CUDPSocket(const CUDPSocket &rValue)
{
	m_hSocket = rValue.m_hSocket;
	m_ipAddressLocal = rValue.m_ipAddressLocal;
}

inline CUDPSocket::~CUDPSocket()
{
	Destroy();
}

inline BOOL  CUDPSocket::GetLocalInterface(CIpAddress &ipAddrLocal) const
{
	ipAddrLocal = m_ipAddressLocal;
	return TRUE;
}

inline const SOCKET CUDPSocket::GetHandle() const
{
	return m_hSocket;
}

inline CUDPSocket::operator const SOCKET() const
{
	return m_hSocket;
}

inline const CUDPSocket & CUDPSocket::operator =(const CUDPSocket &rValue)
{
	m_hSocket = rValue.m_hSocket;
	m_ipAddressLocal = rValue.m_ipAddressLocal;

	return *this;
}

#endif //__UDP_SOCKET_H__