#ifndef __OVERLAPPED_H__
#define __OVERLAPPED_H__

class CTask;
class CTaskOverlappedPool;

class LIBNET_API CTaskOverlapped : public OVERLAPPED
{
public:
	CTaskOverlapped(CTaskOverlappedPool *pPool = NULL) :
		m_pPool(pPool),
		m_lRefCount(1)
	{
		Internal = 0;
		InternalHigh = 0;
		Pointer = NULL;
		hEvent = NULL;

		InterlockedIncrement(&m_lOverlappedCount);
	}

	virtual ~CTaskOverlapped() {
		InterlockedDecrement(&m_lOverlappedCount);
	}

public:
	virtual LONG AddRef() {
		return InterlockedIncrement(&m_lRefCount);
	}

	virtual LONG Release();

public:
	void SetPool(CTaskOverlappedPool *pPool) {
		m_pPool = pPool;
	}

	CTaskOverlappedPool * GetPool() {
		return m_pPool;
	}

public:
	static LONG GetTotalCount() {
		return m_lOverlappedCount;
	}

protected:
	virtual void FinalRelease() { }

	LONG m_lRefCount;
	CTaskOverlappedPool *m_pPool;

private:
	static LONG	m_lOverlappedCount;
};

#define SIGID_EXIT			0
#define SIGID_USER			2000

class LIBNET_API CSignalOverlapped : public CTaskOverlapped
{
public:
	CSignalOverlapped(CTaskOverlappedPool *pPool = NULL) :
		CTaskOverlapped(pPool),
		m_dwSignalID(0),
		m_dwParam1(0),
		m_dwParam2(0)
	{
	}

	CSignalOverlapped(DWORD dwSignalID, DWORD dwParam1, DWORD dwParam2) :
		m_dwSignalID(dwSignalID),
		m_dwParam1(dwParam1),
		m_dwParam2(dwParam2)
	{
	}

	virtual ~CSignalOverlapped() {
	}

public:
	DWORD	m_dwSignalID;
	DWORD	m_dwParam1;
	DWORD	m_dwParam2;
};

class LIBNET_API CIOOverlapped : public CTaskOverlapped
{
public:
	enum IO_TYPE {
		IOT_NONE,
		IOT_READ,
		IOT_WRITE,
		IOT_SEND,
		IOT_RECV,
		IOT_ACCEPT,
		IOT_CONNECT
	};

public:
	CIOOverlapped(CTask *pTask, CTaskOverlappedPool *pPool = NULL);
	virtual ~CIOOverlapped();

	void Initialize(IO_TYPE ioType, DWORD dwOffsetHigh = 0, DWORD dwOffset = 0) {
		m_ioType = ioType;
		m_dwErrorCode = 0;
		m_dwTransferred = 0;

		hEvent = NULL;
		OffsetHigh = dwOffsetHigh;
		Offset = dwOffset;
	}

protected:
	void FinalRelease();

public:
	IO_TYPE m_ioType;
	DWORD	m_dwErrorCode;
	DWORD	m_dwTransferred;
	CTask *	m_pTask;
};

class LIBNET_API CTaskOverlappedPool : public CRITICAL_SECTION
{
public:
	CTaskOverlappedPool() : m_pList(NULL) {
		InitializeCriticalSection(this);
	}

	virtual ~CTaskOverlappedPool() {
		Destroy();
		DeleteCriticalSection(this);
	}

public:
	BOOL Create();
	void Destroy();

	virtual LONG Put(CTaskOverlapped *pOverlapped);

protected:
	CTaskOverlapped * GetInternal();

	typedef std::list<CTaskOverlapped *> CTaskOverlappedList;
	CTaskOverlappedList * m_pList;
};

template <class OVERLAPPEDTYPE>
class CTaskOverlappedPoolEx : public CTaskOverlappedPool
{
public:
	LONG Put(CTaskOverlapped *pOverlapped) {
		_ASSERT(dynamic_cast<OVERLAPPEDTYPE *>(pOverlapped) != NULL);
		return CTaskOverlappedPool::Put(pOverlapped);
	}

	OVERLAPPEDTYPE * Get() {
		CTaskOverlapped *pOverlapped = CTaskOverlappedPool::GetInternal();
		if (NULL == pOverlapped)
			return NULL;

		OVERLAPPEDTYPE *pRet = dynamic_cast<OVERLAPPEDTYPE *>(pOverlapped);
		_ASSERT(pRet != NULL);
		return pRet;
	}
};

#endif //__OVERLAPPED_H__