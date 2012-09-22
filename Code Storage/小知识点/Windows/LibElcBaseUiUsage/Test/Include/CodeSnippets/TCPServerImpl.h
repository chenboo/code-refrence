#ifndef __TCP_SERVER_IMPL_H__
#define __TCP_SERVER_IMPL_H__

#include "TCPServer.h"
#include "LibMiniMedia/MiniMediaServer.h"

class CTCPServerImpl : public IELCTCPServer
{
public:
	CTCPServerImpl(IMiniMediaServerCallback* pCallback) {
		m_pMediaServer = NULL;
		m_pCallback = pCallback;
	}

	virtual ~CTCPServerImpl() {
		Shutdown();
	}

public:
	IMiniMediaServer *GetMiniMediaServer() {
		return m_pMediaServer;
	}

public:
	BOOL StartupTcpServer(WORD wPort);
	void Shutdown();

public:
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	STDMETHOD(SendPacket)(DWORD dwChannelID, LPVOID lpvPacket, DWORD cbPacket);
	STDMETHOD_(float, GetMaxBufferUsage)(void);

protected:
	IMiniMediaServer* m_pMediaServer;
	IMiniMediaServerCallback* m_pCallback;
};

#endif //__TCP_SERVER_IMPL_H__