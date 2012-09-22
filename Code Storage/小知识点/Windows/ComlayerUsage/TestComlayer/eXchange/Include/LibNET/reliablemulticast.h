#pragma once

enum {
	MAX_PAYLOAD_SIZE = 1400,
	MAX_NUM_DATA_PACKETS_IN_GROUP = 128,
	DEF_NUM_DATA_PACKETS_IN_GROUP = 32
};

class IReliableMulticastSender
{
public:
	virtual void Delete() = NULL;

public:
	virtual BOOL JoinSession(LPCTSTR lpszLocalIp, 
							 LPCTSTR lpszDestIp, 
							 WORD wPort, 
							 float fBitrateKbps, 
							 float fWindowDurationSeconds, 
							 BOOL bAutoFlush,
							 BOOL bHasSyncPoint,
							 float fRedundancyRate,
							 WORD wMaxPayloadSize, 
							 WORD wNumDataPacketsInGroup) = NULL;
	virtual void LeaveSession() = NULL;

	virtual BOOL SendPayload(LPCVOID lpvPayload, WORD cbPayload, DWORD dwTimeout = INFINITE, BOOL bFrameBegin = TRUE, BOOL bFrameEnd = TRUE, BOOL bSyncPoint = TRUE) = NULL;
	virtual BOOL SendFrame(LPCVOID lpvFrame, DWORD cbFrame, DWORD dwTimeout = INFINITE, BOOL bSyncPoint = TRUE) = NULL;

	virtual int  GetTotalPacketsSent() = NULL;
	virtual double GetTotalBytesSent() = NULL;
};

class IReliableMulticastReceiver
{
public:
	virtual void Delete() = NULL;

public:
	class ICallback {
	public:
		virtual void OnNewSession(DWORD dwSessionID, DWORD dwParam) = NULL;
		virtual void OnPayload(LPCVOID lpvPayload, WORD cbPayload, DWORD dwParam, BOOL bFrameBegin, BOOL bFrameEnd) = NULL;
		virtual void OnPayloadLost(WORD wGroupId, BYTE byIndex, DWORD dwParam) = NULL;
	};

public:
	virtual BOOL JoinSession(LPCTSTR lpszLocalIp, 
							 LPCTSTR lpszDestIp, 
							 WORD wPort, 
							 float fBitrateKbps, 
							 float fWindowDurationSeconds,
							 WORD wMaxPayloadSize, 
							 WORD wNumDataPacketsInGroup) = NULL;
	virtual void LeaveSession() = NULL;

	virtual int GetNumGroups() = NULL;
	virtual int GetMaxGroups() = NULL;
	virtual void SetSourceFilter(DWORD dwSourceIp, WORD wSourcePort) = NULL;

	virtual void SetCallback(ICallback *lpCallback, DWORD dwParam) = NULL;
};

class IReliableMulticastGroupReceiver
{
public:
	virtual void Delete() = NULL;

public:
	class ICallback {
	public:
		virtual void OnNewReceiver(LPVOID pvParam, DWORD dwSourceIp, WORD wSourcePort, IReliableMulticastReceiver * pReceiver) = NULL;
		virtual void OnDeleteReceiver(LPVOID pvParam, DWORD dwSourceIp, WORD wSourcePort, IReliableMulticastReceiver * pReceiver) = NULL;
	};

public:
	virtual BOOL JoinGroup(LPCTSTR lpszLocalIp, 
						   LPCTSTR lpszDestIp, 
						   WORD wPort, 
						   float fBitrateKbps, 
						   float fWindowDurationSeconds,
						   WORD wMaxPayloadSize, 
						   WORD wNumDataPacketsInGroup
						   ) = NULL;
	virtual void LeaveGroup() = NULL;
};

class IFrameAssembler
{
public:
	virtual void Delete() = NULL;

public:
	virtual IReliableMulticastReceiver::ICallback * GetReceiverCallback() = NULL;

public:
	class ICallback {
	public:
		virtual void OnNewSession(DWORD dwSessionID, DWORD dwParam) = NULL;
		virtual void OnFrame(LPCVOID pvFrame, int cbFrame, DWORD dwParam) = NULL;
		virtual void OnIncompleteFrame(LPCVOID pvFrame, int cbFrame, DWORD dwParam) = NULL;
	};
};

LIBNET_API IReliableMulticastSender * RMCreateSender();
LIBNET_API IReliableMulticastReceiver * RMCreateReceiver(IReliableMulticastReceiver::ICallback * pCallback, DWORD dwParam);
LIBNET_API IFrameAssembler * RMCreateFrameAssembler(DWORD cbMaxFrame, IFrameAssembler::ICallback * pCallback, DWORD dwParam);
LIBNET_API IReliableMulticastGroupReceiver * RMCreateGroupReceiver(IReliableMulticastGroupReceiver::ICallback * pCallback, LPVOID pvParam);