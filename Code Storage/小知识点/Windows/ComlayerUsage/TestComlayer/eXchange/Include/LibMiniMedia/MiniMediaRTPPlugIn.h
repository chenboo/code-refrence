#ifndef __MINI_MEDIA_RTP_PLUGIN_H__
#define __MINI_MEDIA_RTP_PLUGIN_H__

#include "LibRTP/LibRTP.h"
#include "UMSPChannelClient.h"
#include "MiniMediaServer.h"

class CRTPMiniMediaInput : 
	public IRTPInput,
	public IUMSPChannelClientCallback
{
// Construction & destruction
public:
	CRTPMiniMediaInput() :
		m_outputPin(this)
	{
		m_pClient = NULL;
		m_dwChannelID = 0;
	}

	virtual ~CRTPMiniMediaInput() {
		Destroy();
	}

	BOOL Create(LPCSTR	lpszLocalIp, 
				LPCSTR	lpszServerIp, 
				WORD	wServerPort, 
				DWORD	dwChannelID, 
				DWORD	dwTimeout = 5000) {
		m_pClient = CreateUMSPChannelClient(this);
		if (NULL == m_pClient)
			return FALSE;

		m_dwChannelID = dwChannelID;
		if (!m_pClient->Connect(lpszLocalIp, lpszServerIp, wServerPort, dwTimeout, 1, &dwChannelID)) {
			Destroy();
			return FALSE;
		}

		return TRUE;
	}

	void Destroy() {
		if (NULL != m_pClient) {
			m_pClient->Release();
			m_pClient = NULL;
		}
	}

public:
	virtual void OnChannelPacket(DWORD dwChannelID, LPVOID lpvPacket, DWORD cbPacket) {
		if (dwChannelID == m_dwChannelID)
			m_outputPin.XferRTPPacket(lpvPacket, cbPacket);
	}

// IRTPInput implementation
public:
	STDMETHOD(RegisterRTPOutput)(IRTPOutput *pOutput) {
		return m_outputPin.RegisterRTPOutput(pOutput);
	}

	STDMETHOD(UnregisterRTPOutput)(IRTPOutput *pOutput) {
		return m_outputPin.UnregisterRTPOutput(pOutput);
	}

	STDMETHOD(Dispose)() { 
		delete this; 
		return S_OK; 
	}

protected:
	DWORD				m_dwChannelID;
	IUMSPChannelClient *m_pClient;
	CRTPOutputPin		m_outputPin;
};

class CRTPMiniMediaOutput : 
	public IRTPOutput
{
// Construction & destruction
public:
	CRTPMiniMediaOutput(IMiniMediaServer *pServer, DWORD dwChannelID) :
		m_inputPin(this)
	{
		m_dwChannelID = dwChannelID;
		m_pServer = pServer;
	}

	virtual ~CRTPMiniMediaOutput() {
	}

// IRTPOutput implementation
public:
	STDMETHOD(RegisterRTPInput)(IRTPInput *pInput) {
		return m_inputPin.RegisterRTPInput(pInput);
	}

	STDMETHOD(XferRTPPacket)(LPCVOID lpvPacket, DWORD cbPacket) {
		if (m_pServer) {
			m_pServer->PutChannelPacket(m_dwChannelID, (LPVOID)lpvPacket, cbPacket);
		}
		return S_OK;
	}

	STDMETHOD(UnregisterRTPInput)(IRTPInput *pInput) {
		return m_inputPin.UnregisterRTPInput(pInput);
	}

	STDMETHOD(Dispose)() { 
		delete this; 
		return S_OK; 
	}

protected:
	CRTPInputPin		m_inputPin;	

	IMiniMediaServer *	m_pServer;
	DWORD				m_dwChannelID;
};

#endif //__MINI_MEDIA_RTP_PLUGIN_H__