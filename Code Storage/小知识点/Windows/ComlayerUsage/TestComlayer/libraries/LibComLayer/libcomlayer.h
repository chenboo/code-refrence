#ifndef __LIBCOMLAYER_H__
#define __LIBCOMLAYER_H__

#ifdef LIBCOMLAYER_EXPORTS
#define LIBCOMLAYER_API __declspec(dllexport)
#else
#define LIBCOMLAYER_API __declspec(dllimport)
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#define LANGLAB_VERSION						(0x00010000)
#define MAX_PACKETS_SIZE						(1440)

#pragma pack(push, 1)

typedef struct tagLANGLAB_PACKET {
	DWORD dwMagic;
	DWORD dwVersion;
	DWORD dwLength;
	GUID  guidTransaction;
}LANGLAB_PACKET, *PLANGLAB_PACKET;

#pragma pack(pop)

// export class
class ICommunicationLayer;

class LIBCOMLAYER_API CPacketHandler
{
public:
	CPacketHandler(ICommunicationLayer* pComLayer);
protected:
	virtual ~CPacketHandler();

public:
	LONG AddRef();
	LONG Release();
	
public:
	virtual DWORD GetPacketMagic() = 0;
	virtual void ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize) = 0;
protected:
	LONG  m_nRef;

	ICommunicationLayer* m_pComLayer;
};

class LIBCOMLAYER_API CTransactionHandler
{
public:
	CTransactionHandler(ICommunicationLayer* pComLayer, GUID *pTransID);
protected:
	virtual ~CTransactionHandler();

public:
	LONG AddRef();
	LONG Release();
	GUID *GetTransactionID();

public:
	virtual void OnTimeout(DWORD dwTimeID) = NULL;
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) = NULL;
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData) = NULL;

protected:
	LONG				 m_nRef;

	ICommunicationLayer	*m_pComLayer;
	GUID				*m_pTransID;
};

// export inter face
class IEventItem
{
public:
	virtual LPVOID GetData() = 0;
	virtual DWORD GetLength() = 0;
};

class LIBCOMLAYER_API IEventManagerNotify	
{
public:
	virtual void OnEventArrived(LPVOID lpvUserValue) = 0;
};

class IEventItemManager
{
public:
	virtual void SetEventArrivedCallback(IEventManagerNotify* pCallback, LPVOID lpvUserValue) = 0;

	virtual LONG AddRef() = 0;
	virtual LONG Release() = 0;

	virtual IEventItem* GetFreeEvent(DWORD cbSize) = 0;
	virtual void PutPendingEvent(IEventItem* pEvent) = 0;

	virtual void PutPendingEvent(LPCVOID lpcEvData, DWORD cbEventSize) = 0;

	virtual IEventItem* GetPendingEvent() = 0;
	virtual void RecycleEvent(IEventItem* pEvent) = 0;
};

class IBufferItem
{
public:
	virtual LPVOID GetData() = 0;

	virtual DWORD GetLength() = 0;
	virtual void SetLength(DWORD cbData) = 0;

	virtual DWORD GetSize() = 0;

	virtual GUID *GetTransactionID() = 0;
	virtual void SetTransactionID(GUID *pTransID) = 0;

	virtual CIpAddress GetDstIpAddress() = 0;
	virtual void SetDstIpAddress(CIpAddress ipAddrDst) = 0;
};

class IPacketHandlerManager
{
public:
	virtual LONG AddRef() = 0;
	virtual LONG Release() = 0;

	virtual BOOL AddPacketHandler(CPacketHandler* pPacketHandler) = 0;
	virtual void DeletePacketHandler(DWORD dwMagic) = 0;
};

class ITransactionManager
{
public:
	virtual LONG AddRef() = 0;
	virtual LONG Release() = 0;

	virtual BOOL AddTransaction(GUID *pID, CTransactionHandler *pTransactionHandler) = 0;
	virtual BOOL DeleteTransaction(GUID *pID) = 0;
};

class ICommunicationLayer
{
public:
	virtual IEventItemManager* GetEventManager() = 0;
	virtual IPacketHandlerManager* GetPacketHandlerManager() = 0;
	virtual ITransactionManager* GetTransactionManager() = 0;

	virtual LONG AddRef() = 0;
	virtual LONG Release() = 0;

	virtual DWORD GetLocalAddress() = 0;
	virtual DWORD GetComAddress() = 0;
	virtual WORD GetComPort() = 0;

	virtual BOOL Startup(DWORD dwLocalAddress, DWORD dwComAddress, WORD wComPort) = 0;
	virtual void Shutdown() = 0;

	virtual BOOL RegisterTimeout(GUID *pID, DWORD dwTimeout, DWORD dwTimeID) = 0;
	virtual void UnregisterTimeout(GUID *pID, DWORD dwTimeID) = 0;

	virtual IBufferItem* AllocBufferItem() = 0;
	virtual BOOL AddToSendBuffer(IBufferItem *pBufferItem) = 0;
};

// Class factory
LIBCOMLAYER_API IEventItemManager* CreateEventManager(DWORD cbPoolSize);
LIBCOMLAYER_API ICommunicationLayer* CreateCommunicationLayer(IEventItemManager* pEventManager);

#endif //__LIBCOMLAYER_H__