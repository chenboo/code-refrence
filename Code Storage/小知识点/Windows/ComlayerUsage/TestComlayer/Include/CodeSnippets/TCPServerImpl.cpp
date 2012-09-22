#include "stdafx.h"
#include "TCPServerImpl.h"

BOOL CTCPServerImpl::StartupTcpServer(WORD wPort)
{
	if (m_pMediaServer)
		return FALSE;

	m_pMediaServer = CreateMiniMediaServer();
	if (!m_pMediaServer)
		return FALSE;

	if (!m_pMediaServer->Startup(m_pCallback, wPort)) {
		m_pMediaServer->Release();
		m_pMediaServer = NULL;
		return FALSE;
	}

	return TRUE;
}

void CTCPServerImpl::Shutdown()
{
	if (!m_pMediaServer)
		return;

	m_pMediaServer->Shutdown();
	m_pMediaServer->Release();
	m_pMediaServer = NULL;
}

STDMETHODIMP CTCPServerImpl::SendPacket(DWORD dwChannelID, LPVOID lpvPacket, DWORD cbPacket)
{
	if (!m_pMediaServer)
		return E_FAIL;

	m_pMediaServer->PutChannelPacket(dwChannelID, lpvPacket, cbPacket);
	return S_OK;
}

STDMETHODIMP CTCPServerImpl::QueryInterface(REFIID iid, void ** ppvObject)
{
	return S_OK;
}

STDMETHODIMP_(ULONG) CTCPServerImpl::AddRef(void)
{
	return 0;
}

STDMETHODIMP_(ULONG) CTCPServerImpl::Release(void)
{
	return 0;
}

STDMETHODIMP_(float) CTCPServerImpl::GetMaxBufferUsage(void)
{
	if (!m_pMediaServer)
		return 0.0f;

	return m_pMediaServer->GetMaxBufferUsage();
}
