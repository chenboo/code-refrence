#ifndef __IP_ADDRESS_H__
#define __IP_ADDRESS_H__

class LIBNET_API CIpAddress
{
// Constructions & Destructions
public:
	CIpAddress();
	CIpAddress(const CIpAddress &rValue);
	CIpAddress(const SOCKADDR_IN &sockAddr);
	CIpAddress(DWORD dwIpAddress, WORD wPort);
	CIpAddress(LPCSTR lpszIpAddress, WORD wPort);
	CIpAddress(LPCWSTR lpwszIpAddress, WORD wPort);

	virtual ~CIpAddress();

// Attributes
public:
	BOOL GetSockAddr(SOCKADDR_IN &sockAddr) const;
	BOOL GetAddressString(LPSTR lpszIpAddress, DWORD &cbBuffer) const;
	BOOL GetAddressString(LPWSTR lpwszIpAddress, DWORD &cbBuffer) const;
	DWORD GetIpAddress() const;
	WORD GetPort() const;

	BOOL SetSockAddr(const SOCKADDR_IN &sockAddr);
	BOOL SetAddressString(const LPCSTR lpszIpAddress);
	BOOL SetAddressString(const LPCWSTR lpwszIpAddress);
	BOOL SetIpAddress(DWORD dwIpAddress);
	BOOL SetPort(WORD wPort);

	BOOL IsClassA() const;
	BOOL IsClassB() const;
	BOOL IsCalssC() const;
	BOOL IsClassD() const;
	BOOL IsMulticastAddress() const;
	BOOL IsBroadcastAddress() const;

	operator const SOCKADDR_IN &() const;

	static int  GetNumInterface();
	static BOOL GetInterface(DWORD &dwIpAddress, int nPos);

// Operations
public:
	const CIpAddress & operator =(const CIpAddress &rValue);

// Overrides
protected:

// Implementations
protected:
	SOCKADDR_IN m_sockAddr;
};

inline CIpAddress::CIpAddress()
{
	ZeroMemory(&m_sockAddr, sizeof(m_sockAddr));
	m_sockAddr.sin_family = AF_INET;
	m_sockAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	m_sockAddr.sin_port = htons(0);
}

inline CIpAddress::CIpAddress(const CIpAddress &rValue)
{
	CopyMemory(&m_sockAddr, &rValue.m_sockAddr, sizeof(m_sockAddr));
}

inline CIpAddress::CIpAddress(const SOCKADDR_IN &sockAddr)
{
	CopyMemory(&m_sockAddr, &sockAddr, sizeof(m_sockAddr));
}

inline CIpAddress::CIpAddress(LPCSTR lpszIpAddress, WORD wPort)
{
	ZeroMemory(&m_sockAddr, sizeof(m_sockAddr));
	m_sockAddr.sin_family = AF_INET;

	SetAddressString(lpszIpAddress);
	SetPort(wPort);
}

inline CIpAddress::CIpAddress(LPCWSTR lpwszIpAddress, WORD wPort)
{
	ZeroMemory(&m_sockAddr, sizeof(m_sockAddr));
	m_sockAddr.sin_family = AF_INET;

	SetAddressString(lpwszIpAddress);
	SetPort(wPort);
}

inline CIpAddress::CIpAddress(DWORD dwIpAddress, WORD wPort)
{
	ZeroMemory(&m_sockAddr, sizeof(m_sockAddr));
	m_sockAddr.sin_family = AF_INET;
	m_sockAddr.sin_addr.S_un.S_addr = dwIpAddress;
	m_sockAddr.sin_port = htons(wPort);
}

inline CIpAddress::~CIpAddress()
{
}

inline BOOL CIpAddress::GetSockAddr(SOCKADDR_IN &sockAddr) const
{
	CopyMemory(&sockAddr, &m_sockAddr, sizeof(m_sockAddr));
	return TRUE;
}

inline DWORD CIpAddress::GetIpAddress() const
{
	return (DWORD)m_sockAddr.sin_addr.S_un.S_addr;
}

inline BOOL CIpAddress::GetAddressString(LPSTR lpszIpAddress, DWORD &cbBuffer) const
{
	char szIpAddress[MAX_IP_ADDR_STRING_LENGTH];
	if (!CSocketUtils::IPAddrToString(m_sockAddr.sin_addr.S_un.S_addr, szIpAddress))
		return FALSE;

	DWORD cbLen = (DWORD)strlen(szIpAddress) + 1;
	if (NULL == lpszIpAddress) {
		cbBuffer = cbLen;
		return TRUE;
	}
	else if (cbBuffer < cbLen) {
		return FALSE;
	}

	strcpy(lpszIpAddress, szIpAddress);
	cbBuffer = cbLen;
	return TRUE;
}

inline BOOL CIpAddress::GetAddressString(LPWSTR lpwszIpAddress, DWORD &cchBuffer) const
{
	wchar_t wszIpAddress[MAX_IP_ADDR_STRING_LENGTH];
	if (!CSocketUtils::IPAddrToString(m_sockAddr.sin_addr.S_un.S_addr, wszIpAddress))
		return FALSE;

	DWORD cchLen = (DWORD)wcslen(wszIpAddress) + 1;
	if (NULL == lpwszIpAddress) {
		cchBuffer = cchLen;
		return TRUE;
	}
	else if (cchBuffer < cchLen) {
		return FALSE;
	}

	wcscpy(lpwszIpAddress, wszIpAddress);
	cchBuffer = cchLen;
	return TRUE;
}

inline WORD CIpAddress::GetPort() const
{
	return ntohs(m_sockAddr.sin_port);
}

inline BOOL CIpAddress::SetSockAddr(const SOCKADDR_IN &sockAddr)
{
	CopyMemory(&m_sockAddr, &sockAddr, sizeof(m_sockAddr));
	return TRUE;
}

inline BOOL CIpAddress::SetAddressString(const LPCSTR lpszIpAddress)
{
	m_sockAddr.sin_addr.S_un.S_addr = CSocketUtils::StringToIPAddr(lpszIpAddress);
	return TRUE;
}

inline BOOL CIpAddress::SetAddressString(const LPCWSTR lpwszIpAddress)
{
	m_sockAddr.sin_addr.S_un.S_addr = CSocketUtils::StringToIPAddr(lpwszIpAddress);
	return FALSE;
}

inline BOOL CIpAddress::SetIpAddress(DWORD dwIpAddress)
{
	m_sockAddr.sin_addr.S_un.S_addr = dwIpAddress;

	return TRUE;
}

inline BOOL CIpAddress::SetPort(WORD wPort)
{
	m_sockAddr.sin_port = htons(wPort);

	return TRUE;
}

inline BOOL CIpAddress::IsClassA() const
{
	return IN_CLASSA(ntohl(m_sockAddr.sin_addr.S_un.S_addr));
}

inline BOOL CIpAddress::IsClassB() const
{
	return IN_CLASSB(ntohl(m_sockAddr.sin_addr.S_un.S_addr));
}

inline BOOL CIpAddress::IsCalssC() const
{
	return IN_CLASSC(ntohl(m_sockAddr.sin_addr.S_un.S_addr));
}

inline BOOL CIpAddress::IsClassD() const
{
	return IN_CLASSD(ntohl(m_sockAddr.sin_addr.S_un.S_addr));
}

inline BOOL CIpAddress::IsMulticastAddress() const
{
	return IsClassD();
}

inline BOOL CIpAddress::IsBroadcastAddress() const
{
	return ((ntohl(m_sockAddr.sin_addr.S_un.S_addr) & 0xFF) == 0xFF);
}

inline CIpAddress::operator const SOCKADDR_IN &() const
{
	return m_sockAddr;
}

inline const CIpAddress & CIpAddress::operator =(const CIpAddress &rValue)
{
	CopyMemory(&m_sockAddr, &rValue.m_sockAddr, sizeof(m_sockAddr));
	return *this;
}

#endif //__IPADDRESS_H__