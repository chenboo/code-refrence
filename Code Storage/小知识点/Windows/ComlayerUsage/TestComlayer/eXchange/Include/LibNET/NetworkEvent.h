#ifndef __NETWORK_EVENT_H__
#define __NETWORK_EVENT_H__

class LIBNET_API CNetworkEvent
{
// Constructions & Destructions
public:
	CNetworkEvent();
	CNetworkEvent(WSAEVENT hWSAEvent);
	CNetworkEvent(const CNetworkEvent &rValue);

	virtual ~CNetworkEvent();

// Attributes
public:
	const HANDLE GetHandle() const;
	operator const HANDLE() const;

// Operations
public:
	BOOL Create();
	BOOL Destroy();

	BOOL Set();
	BOOL Reset();

	BOOL Select(SOCKET hSocket, LONG lNetworkEvents);
	BOOL EnumNetworkEvents(WSANETWORKEVENTS &wsaNetworkEvents);

	const CNetworkEvent & operator =(const CNetworkEvent &rValue);

// Overrides
protected:


// Implementations
protected:
	WSAEVENT m_hEvent;
	SOCKET	 m_hSocket;
};

inline CNetworkEvent::CNetworkEvent()
{
	m_hEvent = NULL;
	m_hSocket = NULL;
}

inline CNetworkEvent::CNetworkEvent(WSAEVENT hWSAEvent)
{
	m_hEvent = (HANDLE)hWSAEvent;
}

inline CNetworkEvent::CNetworkEvent(const CNetworkEvent &rValue)
{
	m_hEvent = rValue.m_hEvent;
	m_hSocket = rValue.m_hSocket;
}

inline CNetworkEvent::~CNetworkEvent()
{
	Destroy();
}

inline const HANDLE CNetworkEvent::GetHandle() const
{
	return m_hEvent;
}

inline CNetworkEvent::operator const HANDLE() const
{
	return m_hEvent;
}

inline BOOL CNetworkEvent::Create()
{
	m_hEvent = WSACreateEvent();
	return (WSA_INVALID_EVENT != m_hEvent);
}

inline BOOL CNetworkEvent::Destroy()
{
	if (NULL != m_hEvent) {
		BOOL bRet = WSACloseEvent(m_hEvent);
		m_hEvent = NULL;

		return bRet;
	}
	else
		return FALSE;
}

inline BOOL CNetworkEvent::Set()
{
	if (NULL != m_hEvent)
		return WSASetEvent(m_hEvent);
	else
		return FALSE;
}

inline BOOL CNetworkEvent::Reset()
{
	if (NULL != m_hEvent)
		return WSAResetEvent(m_hEvent);
	else
		return FALSE;
}

inline BOOL CNetworkEvent::Select(SOCKET hSocket, LONG lNetworkEvents)
{
	if ((NULL == hSocket) || (NULL == m_hEvent))
		return FALSE;

	m_hSocket = hSocket;
	return (WSAEventSelect(m_hSocket, m_hEvent, lNetworkEvents) != SOCKET_ERROR);
}

inline BOOL CNetworkEvent::EnumNetworkEvents(WSANETWORKEVENTS &wsaNetworkEvents)
{
	if ((NULL == m_hSocket) || (NULL == m_hEvent))
		return FALSE;

	return (WSAEnumNetworkEvents(m_hSocket, m_hEvent, &wsaNetworkEvents) != SOCKET_ERROR);
}

inline const CNetworkEvent & CNetworkEvent::operator =(const CNetworkEvent &rValue)
{
	m_hEvent = rValue.m_hEvent;
	m_hSocket = rValue.m_hSocket;
	
	return *this;
}

#endif //__NETWORK_EVENT_H__