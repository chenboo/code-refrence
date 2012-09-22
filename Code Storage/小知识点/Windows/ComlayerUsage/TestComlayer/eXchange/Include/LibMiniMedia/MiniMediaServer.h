#ifndef __MINI_MEDIA_SERVER_H__
#define __MINI_MEDIA_SERVER_H__

#include "LibMiniMedia.h"

typedef struct _NAME_CONNECTION_CONTEXT {
	// In
	LPCTSTR			lpszName;
	LONG			lTaskId;
} NAME_CONNECTION_CONTEXT, *PNAME_CONNECTION_CONTEXT;

typedef struct _DEMAND_REQUEST_CONTEXT {
	// In
	LPCTSTR			lpszName;
	LPCTSTR			lpszPath;

	// Out [_MAX_PATH]
	LPTSTR			lpszTranslatedPath;
} DEMAND_REQUEST_CONTEXT, *PDEMAND_REQUEST_CONTEXT;

class ITransmitProgressCallback
{
public:
	virtual void OnDataTransmited(LONGLONG llOffset, LONGLONG llSize, LPVOID pvParam) = NULL;
};

class IOutputBuffer
{
public:
	virtual BOOL SetCount(int cbBuffer) = NULL;
	virtual LPBYTE GetData() = NULL;
};

typedef struct _UPLOAD_REQUEST_CONTEXT {
	// In
	LPCTSTR			lpszName;
	LPCTSTR			lpszPath;
	FILE_ATTRIBUTE	attr;
	LONGLONG		llOffset;

	// Out [_MAX_PATH]
	LPTSTR			lpszTranslatedPath;
	ITransmitProgressCallback * pTransmitCallback;
	LPVOID			pTransmitCallbackParam;
} UPLOAD_REQUEST_CONTEXT, *PUPLOAD_REQUEST_CONTEXT;

typedef struct _DOWNLOAD_REQUEST_CONTEXT {
	// In
	LPCTSTR			lpszName;
	LPCTSTR			lpszPath;

	// Out [_MAX_PATH]
	LPTSTR			lpszTranslatedPath;
	ITransmitProgressCallback * pTransmitCallback;
	LPVOID			pTransmitCallbackParam;
} DOWNLOAD_REQUEST_CONTEXT, *PDOWNLOAD_REQUEST_CONTEXT;

typedef struct _ENUM_REQUEST_CONTEXT {
	// In
	LPCTSTR			lpszName;
	LPCTSTR			lpszPath;

	// Out [_MAX_PATH]
	LPTSTR			lpszTranslatedPath;
} ENUM_REQUEST_CONTEXT, *PENUM_REQUEST_CONTEXT;

typedef struct _CREATE_DIR_REQUEST_CONTEXT {
	// In
	LPCTSTR			lpszName;
	LPCTSTR			lpszPath;

	// Out [_MAX_PATH]
	LPTSTR			lpszTranslatedPath;
} CREATE_DIR_REQUEST_CONTEXT, *PCREATE_DIR_REQUEST_CONTEXT;

typedef struct _DELETE_TREE_REQUEST_CONTEXT {
	// In
	LPCTSTR			lpszName;
	LPCTSTR			lpszPath;

	// Out [_MAX_PATH]
	LPTSTR			lpszTranslatedPath;
} DELETE_TREE_REQUEST_CONTEXT, *PDELETE_TREE_REQUEST_CONTEXT;

typedef struct _CUSTOM_DATA_REQUEST_CONTEXT {
	// In
	LPCTSTR			lpszName;
	DWORD			dwID;
	LPBYTE			lpData;
	DWORD			dwDataSize;

	// Out
	LPBYTE			lpOutData;
	// In/Out
	DWORD			*pdwOutDataSize;

} CUSTOM_DATA_REQUEST_CONTEXT, *PCUSTOM_DATA_REQUEST_CONTEXT;

typedef struct _REMOTE_PROCESS_CALL_CONTEXT {
	// In
	LPCTSTR			lpszName;
	const BYTE *	pbParams;
	int				cbParams;

	// Out
	IOutputBuffer *	pOutputBuffer;
} REMOTE_PROCESS_CALL_CONTEXT, *PREMOTE_PROCESS_CALL_CONTEXT;


class IMiniMediaServerCallback
{
public:
	virtual DWORD OnRequest(MM_REQUEST request, LPVOID pvRequestContext) = NULL;
	virtual VOID OnDisconnect(LPCTSTR lpszName) = NULL;
};

class IMiniMediaServer
{
public:
	virtual BOOL Startup(IMiniMediaServerCallback *pCallback = NULL, WORD wPort = 100) = NULL;
	virtual BOOL Shutdown() = NULL;

	virtual void PutChannelPacket(DWORD dwChannelID, LPVOID lpvPacket, DWORD cbPacket) = NULL;

	virtual void Release() = NULL;

	virtual float GetMaxBufferUsage() = NULL;

	// Pls DO NOT call this routine to do self-terminate
	virtual BOOL TerminateTask(LONG lTaskId) = NULL;
};

LIBMINIMEDIA_API IMiniMediaServer * CreateMiniMediaServer();

#endif //__MINI_MEDIA_SERVER_H__