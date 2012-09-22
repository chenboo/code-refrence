#ifndef __COMMUNICATIONLAYER_H__
#define __COMMUNICATIONLAYER_H__

#include "Bufferpool.h"
#include "LangLabEvent.h"
#include "transactions.h"
#include "PacketHandler.h"

#define BUFFER_ITEM_SIZE	(2048)
#define EVENT_POOL_SIZE		(1024 * 1024)

class CCommunicationLayer : public ICommunicationLayer
{
public:
	CCommunicationLayer(IEventItemManager* pEventManager);
private:
	virtual ~CCommunicationLayer();

public:
	virtual IEventItemManager* GetEventManager();
	virtual IPacketHandlerManager* GetPacketHandlerManager();
	virtual ITransactionManager* GetTransactionManager();

	virtual LONG AddRef();
	virtual LONG Release();

	virtual DWORD GetLocalAddress();
	virtual DWORD GetComAddress();
	virtual WORD GetComPort();

	virtual BOOL Startup(DWORD dwLocalAddress, DWORD dwComAddress, WORD wComPort);
	virtual void Shutdown();

	virtual BOOL RegisterTimeout(GUID *pID, DWORD dwTimeout, DWORD dwTimeID);
	virtual void UnregisterTimeout(GUID *pID, DWORD dwTimeID);

	virtual IBufferItem* AllocBufferItem();
	virtual BOOL AddToSendBuffer(IBufferItem *pBufferItem);

protected:
	BOOL ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize);
	BOOL Send(LPCVOID lpBuffer, DWORD cbBuffer, CIpAddress ipAddrDst, GUID *pID);
	BOOL AddToSendBufferInternal(CBufferItem *pItem);

	static DWORD WINAPI ComThreadProc(LPVOID lpvParam);
	virtual DWORD ThreadProc();

	DWORD				m_dwLocalAddr;
	DWORD				m_dwComAddr;
	WORD				m_wComPort;

	HANDLE				m_hWorkThread;
	HANDLE				m_hEvQuit;

	CRITICAL_SECTION	m_csTimeout;
	CTimeoutHeap		m_timeoutHeap;

	IEventItemManager*  m_pEventManager;

	CBufferItemManager*	m_pBufferManager;
	CRITICAL_SECTION	m_csCurrentSend;
	CBufferItem			*m_pCurrentSend;

	CUDPSocket			m_socketSend;
	CUDPSocket			m_socketRecv;

	CTransactionManager *m_pTransanctionMgr;
	CPacketHandlerManager *m_pPacketHandlerMgr;

	LONG				m_nRef;
};

#endif //__COMMUNICATIONLAYER_H__