#ifndef __BUFFERPOOL_H__
#define __BUFFERPOOL_H__

#pragma warning(disable: 4251)

class CBufferItem : public IBufferItem
{
public:
	CBufferItem(DWORD cbAllocate);
	virtual ~CBufferItem();

public:
	virtual LPVOID GetData();

	virtual DWORD GetLength();
	virtual void SetLength(DWORD cbData);

	virtual DWORD GetSize();

	virtual GUID *GetTransactionID();
	virtual void SetTransactionID(GUID *pTransID);

	virtual CIpAddress GetDstIpAddress();
	virtual void SetDstIpAddress(CIpAddress ipAddrDst);

protected:
	LPVOID				m_lpvData;
	DWORD				m_cbData;
	DWORD				m_cbAllocated;
	GUID				m_guidTransaction;
	CIpAddress			m_ipAddrDst;
};

class CBufferItemManager
{
public:
	CBufferItemManager(DWORD cbBufferItem);
	virtual ~CBufferItemManager();

public:
	LONG AddRef();
	LONG Release();

	CBufferItem* GetFreeBufferItem();
	void PutPendingBufferItem(CBufferItem* pBufferItem);

	CBufferItem* GetItem();
	void RecycleBufferItem(CBufferItem* pBufferItem);

protected:
	typedef std::list<CBufferItem *> CBufferList;

	CRITICAL_SECTION	m_csReady;
	CBufferList			m_bufferReady;

	CRITICAL_SECTION	m_csFree;
	CBufferList			m_bufferFree;

	DWORD				m_cbBufferItem;
	LONG				m_nRef;
};

#endif //__BUFFERPOOL_H__