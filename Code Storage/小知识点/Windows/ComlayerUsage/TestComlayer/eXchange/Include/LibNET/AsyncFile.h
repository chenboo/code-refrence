// AsyncFile.h: interface for the CAsyncFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASYNCFILE_H__331218E4_C4E8_42B5_9B80_0485E3CBA399__INCLUDED_)
#define AFX_ASYNCFILE_H__331218E4_C4E8_42B5_9B80_0485E3CBA399__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FOMOD_READ			1
#define FOMOD_WRITE			2
#define FOMOD_CREATE		4
#define FOMOD_SHARE_READ	8
#define FOMOD_SHARE_WRITE	16
#define FOMOD_NO_BUFFER		32

#include "TaskQueue.h"

class LIBNET_API CAsyncFile
{
public:
	CAsyncFile();
	virtual ~CAsyncFile();

public:
	HANDLE GetIOHandle() {
		return m_hFile;
	}

public:
	DWORD Open(LPCTSTR lpszFile, UINT uMode);
	void  Close();

	DWORD ReadFile(LPVOID lpvBuffer, DWORD dwOffset, DWORD cbToRead, DWORD *pcbRead, CIOOverlapped *pTaskOverlapped, DWORD dwOffsetHigh = 0);
	DWORD WriteFile(LPVOID lpvBuffer, DWORD dwOffset, DWORD cbToWrite, DWORD *pcbWritten, CIOOverlapped *pTaskOverlapped, DWORD dwOffsetHigh = 0);

protected:
	HANDLE m_hFile;
};

#endif // !defined(AFX_ASYNCFILE_H__331218E4_C4E8_42B5_9B80_0485E3CBA399__INCLUDED_)
