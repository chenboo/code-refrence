#ifndef __UMSP_CHANNEL_CLIENT_H__
#define __UMSP_CHANNEL_CLIENT_H__

#include "LibMiniMedia.h"

class IUMSPChannelClientCallback
{
public:
	virtual void OnConnected() {}
	virtual void OnConnectFailed() {}
	virtual void OnDisconnected() {}

	virtual void OnChannelPacket(DWORD dwChannelID, LPVOID lpvPacket, DWORD cbPacket) = NULL;
};

class IUMSPChannelClientCallbackEx
{
public:
	virtual void OnConnected(DWORD dwData) {}
	virtual void OnConnectFailed(DWORD dwData) {}
	virtual void OnDisconnected(DWORD dwData) {}

	virtual void OnChannelPacket(DWORD dwData, DWORD dwChannelID, LPVOID lpvPacket, DWORD cbPacket) = NULL;
};

class IUMSPChannelClient
{
public:
	virtual BOOL Connect(LPCSTR lpszLocalIp, 
						 LPCSTR lpszServerIp, 
						 WORD	wServerPort, 
						 DWORD	dwTimeout,
						 DWORD	dwNumChannels,
						 DWORD *pdwChannelIDs) = NULL;

	virtual void Release() = NULL;
};

LIBMINIMEDIA_API IUMSPChannelClient * CreateUMSPChannelClient(IUMSPChannelClientCallback *pCallback);
LIBMINIMEDIA_API IUMSPChannelClient * CreateUMSPChannelClientEx(IUMSPChannelClientCallbackEx *pCallback, DWORD dwData);

#endif //__UMSP_CHANNEL_CLIENT_H__
