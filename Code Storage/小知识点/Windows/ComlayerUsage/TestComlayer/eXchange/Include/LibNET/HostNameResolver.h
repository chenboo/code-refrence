// HostNameResolver.h: interface for the CHostNameResolver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOSTNAMERESOLVER_H__46CF9412_DFCA_4F03_98BC_1F7A3760A291__INCLUDED_)
#define AFX_HOSTNAMERESOLVER_H__46CF9412_DFCA_4F03_98BC_1F7A3760A291__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LIBNET_API CHostNameResolver  
{
public:
	CHostNameResolver() {
		m_hThread = NULL;
		m_hEvInitOK = NULL;
		m_hGetHostByName = NULL;
		m_pCallback = NULL;
		m_dwTimeout = 0;
		m_hWnd = NULL;

		InitializeCriticalSection(&m_csLock);
	}

	virtual ~CHostNameResolver() {
		Shutdown();
		DeleteCriticalSection(&m_csLock);
	}

	BOOL Create() {
		m_hEvInitOK = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == m_hEvInitOK)
			goto ERROR_PROCESS;

		m_hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, &m_dwThreadId);
		if (NULL == m_hThread) 
			goto ERROR_PROCESS;

		WaitForSingleObject(m_hEvInitOK, INFINITE);
		if (!m_hWnd || !IsWindow(m_hWnd))
			goto ERROR_PROCESS;

		return TRUE;

	ERROR_PROCESS:
		Shutdown();
		return FALSE;
	}

	void Shutdown() {
		if (NULL != m_hThread) {
			Cancel();

			if (IsWindow(m_hWnd)) {
				PostMessage(m_hWnd, WM_QUIT, 0, 0);
			}

			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}

		if (NULL != m_hEvInitOK) {
			CloseHandle(m_hEvInitOK);
			m_hEvInitOK = NULL;
		}

		m_hWnd = NULL;
	}

public:
	static DWORD Resolve(LPCSTR lpszHostName, DWORD dwTimeout = INFINITE);
	static DWORD Resolve(LPCWSTR lpwszHostName, DWORD dwTimeout = INFINITE);

public:
	class ICallback
	{
		friend class CHostNameResolver;

	protected:
		virtual void OnResolved(DWORD dwHostAddr) {}
		virtual void OnResolveTimeout() {}
		virtual void OnResolveFailed(int nError) {}
	};

	BOOL Resolve(ICallback *pCallback, LPCSTR lpszHostName, DWORD dwTimeout = INFINITE);
	BOOL Resolve(ICallback *pCallback, LPCWSTR lpwszHostName, DWORD dwTimeout = INFINITE);
	void Cancel();

protected:
	static DWORD WINAPI ThreadProc(LPVOID lpvParam);
	virtual DWORD ThreadProc();

	char   m_szName[MAX_PATH];

	HANDLE m_hThread;
	HANDLE m_hEvInitOK;
	HANDLE m_hGetHostByName;
	DWORD  m_dwIpAddress;
	HWND   m_hWnd;

	DWORD  m_dwThreadId;

	ICallback *m_pCallback;
	DWORD  m_dwTimeout;

	CRITICAL_SECTION m_csLock;
};

class CHostNameResolverCallback : public CHostNameResolver::ICallback
{
public:
	CHostNameResolverCallback() {
		m_dwIpAddress = INADDR_NONE;
		m_hEvComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	virtual ~CHostNameResolverCallback() {
		if (NULL != m_hEvComplete) {
			CloseHandle(m_hEvComplete);
			m_hEvComplete = NULL;
		}
	}

public:
	BOOL Wait() {
		if (m_hEvComplete) {
			return (WAIT_OBJECT_0 == WaitForSingleObject(m_hEvComplete, INFINITE));
		}
		else {
			return FALSE;
		}
	}

	DWORD GetHostAddr() {
		return m_dwIpAddress;
	}

protected:
	virtual void OnResolved(DWORD dwHostAddr) {
		m_dwIpAddress = dwHostAddr;
		SetEvent(m_hEvComplete);
	}

	virtual void OnResolveTimeout() {
		SetEvent(m_hEvComplete);
	}

	virtual void OnResolveFailed(int nError) {
		SetEvent(m_hEvComplete);
	}

protected:
	HANDLE m_hEvComplete;
	DWORD m_dwIpAddress;
};

#endif // !defined(AFX_HOSTNAMERESOLVER_H__46CF9412_DFCA_4F03_98BC_1F7A3760A291__INCLUDED_)
