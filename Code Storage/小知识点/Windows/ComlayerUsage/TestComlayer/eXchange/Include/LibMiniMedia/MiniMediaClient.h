#ifndef __MINI_MEDIA_CLIENT_H__
#define __MINI_MEDIA_CLIENT_H__

#include "LibMiniMedia.h"

class IMiniMediaClientCallback
{
public:
	// Connection callbacks
	virtual void OnConnected() = NULL;
	virtual void OnConnectFailed(DWORD dwErrorCode) = NULL;
	virtual void OnDisconnected() = NULL;

	// Common request callbacks
	virtual void OnRequestFailed(MM_REQUEST request, DWORD dwErrorCode) = NULL;
	virtual void OnRequestComplete(MM_REQUEST request) = NULL;

	// Specific request callbacks
	virtual void OnEnumFileItem(LPCTSTR lpszFile, const FILE_ATTRIBUTE * pFileAttribute, BOOL bHasSubDir) = NULL;

	virtual BOOL OnUploadReadData(LPVOID lpvData, LONGLONG llOffset, int& cbData, BOOL& bEOF) = NULL;

	virtual void OnDownloadBegin(const FILE_ATTRIBUTE * pFileAttribute) = NULL;
	virtual void OnDownloadFileData(LPVOID lpvData, int cbData) = NULL;

	virtual void OnRequestCompleteEx(MM_REQUEST request, LPVOID lpvData, int cbData) = NULL;

	virtual void OnCustomData(LPBYTE lpvData, int cbData) = NULL;
};

class IMiniMediaClient
{
public:
	static const LONGLONG UPLOAD_OFFSET_END_OF_FILE = -1LL;

public:
	// Deletion
	virtual void Release() = NULL;

	// Connection
	virtual BOOL Connect(LPCTSTR lpszLocalIp, LPCTSTR lpszServerIp, WORD wServerPort, DWORD dwTimeout) = NULL;
	virtual void Disconnect() = NULL;

	virtual BOOL Cancel() = NULL;

	// Requests
	virtual BOOL NameConnection(LPCTSTR lpszName) = NULL;
	virtual BOOL EnumFile(LPCTSTR lpszPath, DWORD dwAttrMustHave, DWORD dwAttrDontHave) = NULL;
	virtual BOOL UploadFile(LPCTSTR lpszPath, const FILE_ATTRIBUTE * pFileAttribute, LONGLONG llOffset) = NULL;
	virtual BOOL DownloadFile(LPCTSTR lpszPath, LONGLONG llOffset) = NULL;
	virtual BOOL CreateDirectory(LPCTSTR lpszPath) = NULL;
	virtual BOOL DeleteTree(LPCTSTR lpszPath) = NULL;
	virtual BOOL RemoteProcessCall(const BYTE * pbRPCParams, int cbRPCParams) = NULL;
 	virtual BOOL RemoteProcessCall(const BYTE * pbRPCParams, int cbRPCParams, DWORD& dwRetCode, LPBYTE pbRPCRetData, int& cbRPCRetData) = NULL;

	virtual BOOL DisconnectEx(BOOL bAsyncNotify) = NULL;

	virtual BOOL CustomData(DWORD dwID, LPBYTE lpData, DWORD cbSize) = NULL;
};

LIBMINIMEDIA_API IMiniMediaClient * CreateMiniMediaClient(IMiniMediaClientCallback *pCallback);

#endif //__MINI_MEDIA_CLIENT_H__