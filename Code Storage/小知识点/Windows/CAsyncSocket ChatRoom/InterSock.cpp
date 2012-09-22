#include "stdafx.h"
#include "DLG.h"
#include "InterSock.h"

CInterSock::CInterSock()
{
	m_hwndNotify = NULL;
}

CInterSock::~CInterSock()
{
}

void CInterSock::SetNotifyWnd(HWND hwnd)
{
	m_hwndNotify = hwnd;
}

void CInterSock::OnAccept(int nErrorCode)
{
	::PostMessage(m_hwndNotify, WM_ACCEPT, NULL, NULL);
	CAsyncSocket::OnAccept(nErrorCode);
}

void CInterSock::OnReceive(int nErrorCode)
{
	::PostMessage(m_hwndNotify, WM_RECEIVE, NULL, NULL);
	CAsyncSocket::OnReceive(nErrorCode);
}
