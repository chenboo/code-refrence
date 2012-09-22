// AsyncSocket.h: interface for the CAsyncSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASYNCSOCKET_H__6B93911D_0DEC_4869_821F_D9E17E0B10A5__INCLUDED_)
#define AFX_ASYNCSOCKET_H__6B93911D_0DEC_4869_821F_D9E17E0B10A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TaskArch.h"

class LIBNET_API CAsyncSocket
{
public:
	CAsyncSocket();
	virtual ~CAsyncSocket();

public:
	HANDLE GetIOHandle() {
		return (HANDLE)m_hSocket;
	}

public:
	int  Create(int nType);
	void Destroy();

	void Attach(SOCKET hSocket) {
		m_hSocket = hSocket;
	}

	SOCKET Detach() {
		SOCKET hSocket = m_hSocket;
		m_hSocket = NULL;
		return hSocket;
	}

	int	 Bind(DWORD dwIPAddr, WORD wPort);
	int  SetReuseAddr(BOOL bReuse);
	int  SetSendBufferSize(int nSize);
	int  SetRecvBufferSize(int nSize);
	
protected:
	int	 GetErrorNo() {
		int nError = WSAGetLastError();
		return (nError == 0) ? WSABASEERR : nError;
	}

	SOCKET m_hSocket;
};

class CConnectQueue;
class LIBNET_API CAsyncTCPSocket : public CAsyncSocket
{
public:
	CAsyncTCPSocket() {}
	virtual ~CAsyncTCPSocket() {}

public:
	int  Create() {
		return CAsyncSocket::Create(SOCK_STREAM);
	}

	int  Listen(int nConnectionBacklog);
	int  AcceptEx(CAsyncSocket *pConnectedSocket, LPVOID lpvBuffer, 
				  DWORD cbReceive, DWORD cbLocalAddrLen, DWORD cbRemoteAddrLen, 
				  CIOOverlapped *pTaskOverlapped);
	int  ConnectEx(DWORD dwIPAddr, WORD wPort, CIOOverlapped *pTaskOverlapped);

	int  SetNoDelay(BOOL bDelay);
	int  SetKeepAlive(BOOL bKeepAlive);

	int  Send(LPWSABUF lpBuffers, DWORD dwBufferCount, 
			  DWORD *pcbSend, CIOOverlapped *pTaskOverlapped);
	int  Receive(LPWSABUF lpBuffers, DWORD dwBufferCount, 
				 DWORD *pcbReceived, CIOOverlapped *pTaskOverlapped);

	int  Shutdown();

public:
	static BOOL InitConnectEx();
	static void DoneConnectEx();

protected:
	static CConnectQueue *	m_pConnectQueue;
};

class LIBNET_API CAsyncUDPSocket : public CAsyncSocket
{
public:
	CAsyncUDPSocket() {} 
	virtual ~CAsyncUDPSocket() {}

public:
	int  Create() {
		return CAsyncSocket::Create(SOCK_DGRAM);
	}

	int  JoinMulticastGroup(DWORD dwMulticastIPAddr, DWORD dwIpAddr);
	int  LeaveMulticastGroup();

	int  EnableBroadcast(BOOL bEnable);

	int  SetLoopback(BOOL bLoopback);
	int  SetTTL(int nTTL);

	int  SendTo(LPWSABUF lpBuffers, DWORD dwBufferCount, 
				DWORD *pcbSend, DWORD dwIPAddr, WORD wPort,
				CIOOverlapped *pTaskOverlapped);
	int  ReceiveFrom(LPWSABUF lpBuffers, DWORD dwBufferCount, 
					 DWORD *pcbReceived, struct sockaddr FAR *lpFrom, int *pnFromLen, 
					 CIOOverlapped *pTaskOverlapped);

protected:
	DWORD	m_dwMulticastIpAddr;
	DWORD	m_dwIpAddr;
};

#endif // !defined(AFX_ASYNCSOCKET_H__6B93911D_0DEC_4869_821F_D9E17E0B10A5__INCLUDED_)
