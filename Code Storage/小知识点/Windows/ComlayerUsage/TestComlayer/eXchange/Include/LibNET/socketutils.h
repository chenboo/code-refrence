// SocketUtils.h: interface for the CSocketUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETUTILS_H__E6D896D6_BA5D_42BE_8080_95DF252C221A__INCLUDED_)
#define AFX_SOCKETUTILS_H__E6D896D6_BA5D_42BE_8080_95DF252C221A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mswsock.h>

#define MAX_IP_ADDR_STRING_LENGTH		16

class LIBNET_API CSocketUtils
{
public:
	static BOOL  Initialize();
	static BOOL  RefreshLocalInterfaceInfo();
	static void  Uninitialize();

	static BOOL  IsMulticastIPAddr(DWORD dwIPAddr) {
		return IN_CLASSD(ntohl(dwIPAddr));
	}

	static BOOL  IsLocalIPAddr(DWORD dwIPAddr);

	static BOOL  IPAddrToString(DWORD dwIPAddr, LPSTR lpszIPAddr);
	static BOOL  IPAddrToString(DWORD dwIPAddr, LPWSTR lpwszIPAddr);

	static DWORD StringToIPAddr(LPCSTR lpszIPAddr) {
		return (DWORD)inet_addr(lpszIPAddr);
	}

	static DWORD StringToIPAddr(LPCWSTR lpwszIPAddr);

	static int	 GetNumLocalIPAddrs() {
		CAutoSectionLock lock(&m_csThreadLock);
		if (NULL == m_pdwLocalIPAddrs || 0 == m_nNumLocalIPAddrs)
			RefreshLocalInterfaceInfo();

		return m_nNumLocalIPAddrs;
	}

	static DWORD GetLocalIPAddr(int nIndex) {
		CAutoSectionLock lock(&m_csThreadLock);
		if (NULL == m_pdwLocalIPAddrs || 0 == m_nNumLocalIPAddrs)
			RefreshLocalInterfaceInfo();

		if ((nIndex < 0) || (nIndex >= m_nNumLocalIPAddrs) ||
			(NULL == m_pdwLocalIPAddrs))
			return INADDR_NONE;

		return m_pdwLocalIPAddrs[nIndex];
	}

	static DWORD GetBroadcastIPAddr(DWORD dwIPAddr);

public:
	static BOOL InitializeExtensionFunctions();

	static LPFN_CONNECTEX GetConnectEx() {
		InitializeExtensionFunctions();
		return m_lpfnConnectEx;
	}

	static LPFN_DISCONNECTEX GetDisconnectEx() {
		InitializeExtensionFunctions();
		return m_lpfnDisconnectEx;
	}

protected:
	static BOOL	 GetLocalInterfaceInfo();
	static void  EmptyLocalInterfaceInfo();

protected:
	static int    m_nNumLocalIPAddrs;
	static DWORD *m_pdwLocalIPAddrs;
	static DWORD *m_pdwBroadcastIPAddrs;
	static int	  m_nInitializeCount;

	static BOOL	  m_bExtFunctionInitialized;
	static LPFN_CONNECTEX		m_lpfnConnectEx;
	static LPFN_DISCONNECTEX	m_lpfnDisconnectEx;

	static CRITICAL_SECTION m_csThreadLock;
};

#endif // !defined(AFX_SOCKETUTILS_H__E6D896D6_BA5D_42BE_8080_95DF252C221A__INCLUDED_)
