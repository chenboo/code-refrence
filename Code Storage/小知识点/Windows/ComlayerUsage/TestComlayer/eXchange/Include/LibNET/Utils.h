#ifndef __UTILS_H__
#define __UTILS_H__

class CAutoSectionLock
{
public:
	CAutoSectionLock(CRITICAL_SECTION *pcsLock) {
		m_pcsLock = pcsLock;
		EnterCriticalSection(pcsLock);
	}

	~CAutoSectionLock() {
		LeaveCriticalSection(m_pcsLock);
	}

protected:
	CRITICAL_SECTION *m_pcsLock;
};

class CAutoSectionUnlock
{
public:
	CAutoSectionUnlock(CRITICAL_SECTION *pcsLock) {
		m_pcsLock = pcsLock;
		EnterCriticalSection(pcsLock);
	}

	~CAutoSectionUnlock() {
		LeaveCriticalSection(m_pcsLock);
	}

protected:
	CRITICAL_SECTION *m_pcsLock;
};

inline unsigned long long rdtsc()
{
	__asm _emit 0x0F
	__asm _emit 0x31
}

#endif //__UTILS_H__