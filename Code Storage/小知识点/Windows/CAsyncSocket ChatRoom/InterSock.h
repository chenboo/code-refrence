#pragma once

// CInterSock command target
#define WM_ACCEPT	(WM_USER + 10)
#define WM_RECEIVE	(WM_USER + 13)

class CInterSock : public CAsyncSocket
{
public:
	CInterSock();
	virtual ~CInterSock();

public:
	void SetNotifyWnd(HWND hwnd);

public:
	HWND	m_hwndNotify;

public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};


