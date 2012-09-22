#ifndef __LANGLABEVENT_H__
#define __LANGLABEVENT_H__

class CEventItem : public IEventItem
{
public:
	CEventItem(DWORD cbRecycle, LPVOID lpvData, DWORD cbData) {
		m_cbRecycle = cbRecycle;
		m_lpvData = lpvData;
		m_cbData = cbData;
	}

	virtual ~CEventItem(void) {
	}

public:
	virtual LPVOID GetData();
	virtual DWORD GetLength();

	DWORD GetRecycleLength();

protected:
	DWORD				m_cbRecycle;
	LPVOID				m_lpvData;
	DWORD				m_cbData;
};

class CEventManager : public IEventItemManager
{
public:
	CEventManager(DWORD cbPoolSize);
	virtual ~CEventManager(void);

public:
	virtual void SetEventArrivedCallback(IEventManagerNotify* pCallback, LPVOID lpvUserValue);

	virtual LONG AddRef();
	virtual LONG Release();

	virtual IEventItem* GetFreeEvent(DWORD cbSize);
	virtual void PutPendingEvent(IEventItem* pEvent);

	virtual void PutPendingEvent(LPCVOID lpcEvData, DWORD cbEventSize);

	virtual IEventItem* GetPendingEvent();
	virtual void RecycleEvent(IEventItem* pEvent);

protected:
	IEventManagerNotify* m_pCallback;
	LPVOID m_lpvUserValue;

	CRITICAL_SECTION	m_csCreate;
	CRITICAL_SECTION	m_csEvent;

	CRITICAL_SECTION	m_csPending;

	HANDLE				m_hEvent;

	typedef std::list<CEventItem *> CEventList;
	CEventList			m_event;

	LONG				m_nRef;

	DWORD				m_cbPoolSize;
	DWORD				m_dwFreeSize;

	DWORD				m_dwGetPos;
	DWORD				m_dwPutPos;

	LPBYTE				m_pbPool;

	CEventItem* GetFreeItem(DWORD cbEventSize);
	void PutItem(CEventItem* pItem);
};

#endif //__LANGLABEVENT_H__