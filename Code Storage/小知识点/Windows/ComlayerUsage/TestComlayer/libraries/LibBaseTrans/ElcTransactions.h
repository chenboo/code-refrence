#ifndef __LANGLABTRANSACTIONS_H__
#define __LANGLABTRANSACTIONS_H__

using namespace std;

// File Submit
typedef struct tagSUBMIT_CONTEXT {
	DWORD dwStudentIp;
	LONGLONG llFileSize;
	DWORD dwFileCount;
	TCHAR pszStudentName[32];
	BOOL bAccepted;
	DWORD dwTickCount;
}SUBMIT_CONTEXT, *PSUBMIT_CONTEXT;

class LIBBASETRANS_API CSubmitFileTransaction : public CTransactionHandler
{
public:
	CSubmitFileTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CSubmitFileTransaction();

public:
	void EnableSubmit(PMEMBERINFO pMemberInfo, BOOL bEnable);
	void EnableSubmitRemove(PMEMBERINFO pMemberInfo);
	void EnableSubmitRemoveAll();

	void SetSubmitQuota(LONGLONG llMaxSize, DWORD dwMaxCount);
	void SetAutoAccept(BOOL bAutoAccept);

	void AcceptSubmitting(GUID guidRequest);
	void DenySubmitting(GUID guidRequest);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void ProcessRequest(PSUBMIT_REQUEST pRequest);
	void ProcessCancel(PSUBMIT_CANCEL pCancel);
	void ProcessComplete(PSUBMIT_COMPLETED pComplete);

	void SendRequestRetPacket(DWORD dwStudentIp, BOOL bEnable, BOOL bQuotaPass, BOOL bAccept);
	void SendCancelRetPacket(DWORD dwStudentIp, BOOL bAccept);
	void SendCompleteRetPacket(DWORD dwStudentIp);

private:
//	BOOL	 m_bEnable;

	typedef std::vector<PMEMBERINFO> CDisableList;
	CDisableList m_disableList;

	LONGLONG m_llMaxSize;
	DWORD	 m_dwMaxCount;

	BOOL	 m_bAutoAccept;

	struct CGUIDCompare {
		bool operator()(GUID guid1, GUID guid2) const {
			return memcmp(&guid1, &guid2, sizeof(GUID)) < 0;
		}
	};

	typedef std::map <GUID, PSUBMIT_CONTEXT, CGUIDCompare> CSubmitMap;
	CSubmitMap	m_pendingMap;
	CSubmitMap	m_checkedMap;
	CSubmitMap	m_finishedMap;

	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CSubmitFileClientTransaction : public CTransactionHandler
{
public:
	CSubmitFileClientTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CSubmitFileClientTransaction();

public:
	BOOL SendSubmitRequest(GUID guidRequest, DWORD dwTeacherIp, DWORD dwStudentIp, TCHAR* pszStudentName, LONGLONG llSize, DWORD dwCount, TCHAR* pszFileNames);
	BOOL SendSubmitCancel(int nCompletedFileCount);
	BOOL SendSubmitCompleted(int nCompletedFileCount);
	void Destroy();

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void ProcessRequestRet(PSUBMIT_REQUEST_RET pRequestRet);
	void ProcessCancelRet(PSUBMIT_CANCEL_RET pCancelRet);
	void ProcessCompleteRet(PSUBMIT_COMPLETE_RET pCompleteRet);

	BOOL SendReuestPacket();
	BOOL SendCancelPacket();
	BOOL SendCompletedPacket(int nCompletedFileCount);

private:
	CRITICAL_SECTION	m_cs;

	GUID m_guidRequest;

	DWORD m_dwTeacherIp;
	DWORD m_dwStudentIp;
	TCHAR m_szStudentName[32];
	TCHAR m_szFileNames[MAX_PATH];

	LONGLONG m_llSize;
	DWORD m_dwCount;

	int		m_nLastCompletedFileCount;

	BOOL	m_bRequest;
	BOOL	m_bCancel;
	BOOL	m_bWaitComplete;

	int		m_nCanelTimes;
};

// Thumbnail
#define MAX_THUMBNAIL_BLOCKS (300)
#define MAX_THUMBNAIL_PAYLOAD (1024)
#define MAX_THUMBNAIL_BUFFER (MAX_THUMBNAIL_BLOCKS * MAX_THUMBNAIL_PAYLOAD)

typedef struct tagTNAIL_POLICY {
	BOOL  bEnable;
	DWORD dwInterval;
	int   nWidth;
	int   nHeight;
}TNAIL_POLICY, *PTNAIL_POLICY;

typedef struct tagTNAIL_CONTEXT {
	// Student Ip
	DWORD dwIpAddress;

	// Policy
	DWORD dwTeacherPolicyId;
	DWORD dwStudentPolicyId;
	TNAIL_POLICY policy;

	// Thumbnail context
	DWORD nThumbnailId;
	DWORD cbTotalSize;
	DWORD cbReceived;
	DWORD cbBuffer;
	BYTE  abMap[MAX_THUMBNAIL_BLOCKS];
	BYTE  abBuffer[1];
}TNAIL_CONTEXT, *PTNAIL_CONTEXT;

class LIBBASETRANS_API CThumbnailTransaction : public CTransactionHandler
{
public:
	CThumbnailTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CThumbnailTransaction();

public:
	// New interfaces
	BOOL AddStudent(DWORD dwMemberIp);
	BOOL DelStudent(DWORD dwMemberIp);
	void DeleteAllStudents();

	BOOL StartStudent(DWORD dwMemberIp);
	void StartAllStudents();

	BOOL StopStudent(DWORD dwMemberIp);
	void StopAllStudents();

	BOOL GetStudentPolicy(DWORD dwMemberIp, PTNAIL_POLICY pTnailPolicy);
	BOOL SetStudentPolicy(DWORD dwMemberIp, PTNAIL_POLICY pTnailPolicy);

public:
	// CTransactionHandler
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void UpdatePolicy();
	void SendPolicyToMember(DWORD dwMemberIp, TNAIL_CONTEXT *pContext);
	void ReportThumbnailEvent(DWORD dwMemberId, LPBYTE lpJPGBuffer, DWORD cbSize);

	typedef map<DWORD, TNAIL_CONTEXT*> CTnailMap;
	CTnailMap		m_tnailMap;

//	TNAIL_POLICY	m_tnPolicy;
//	DWORD			m_dwPolicyId;

	CRITICAL_SECTION	m_cs;

//	DWORD				m_nMaxBufferSize;
};

class LIBBASETRANS_API CPassiveThumbnailTransaction : public CTransactionHandler
{
public:
	CPassiveThumbnailTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CPassiveThumbnailTransaction();

public:
	void Startup(DWORD dwLocalIp, DWORD dwTeacherIp);
	void Shutdown();
	void SendThumbnail(BYTE* lpJpgBuffer, DWORD dwJpgBufferSize);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void RefreshThumbnail();
	void SendPolicyReply();
	void SendThumbnailEndPacket();

	int		m_nThumbnailId;
	int		m_nThumbnailSuccessId;
	DWORD   m_dwMemberId;
	DWORD   m_dwMemberIp;
	DWORD	m_dwTeacherIp;

	TNAIL_POLICY	m_tnPolicy;
	DWORD			m_dwPolicyId;

	CRITICAL_SECTION	m_cs;

	BOOL m_bStarted;

	BYTE*	m_pJpgBuffer;
	DWORD	m_dwJpgBufferSize;
};

#define MAX_CHANNEL_NUMBER					100
#define MAX_APPLY_TIMEOUT					2000
#define MAX_ANNOUNCE_CHANNEL_TIMEOUT		10000

#define CHECK_CHANNEL_INTERVAL				1000
#define APPLY_CHANNEL_INTERVAL				50
#define ANNOUNCE_CHANNEL_INTERVAL			1000

#define MAX_CLIENTS_NUMBER					256

typedef struct tagTEACHER_CHANNEL
{
	DWORD dwIpAddress;
	BOOL  bLock;
	DWORD dwLastAnnounceTime;
	BOOL  bAccessble;
	TCHAR  szName[32];
}TEACHER_CHANNEL, *PTEACHER_CHANNEL;

class LIBBASETRANS_API CFindTeacherTransaction : public CTransactionHandler
{
public:
	CFindTeacherTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CFindTeacherTransaction();

	enum TIME_ID
	{
		APPLY_CHANNEL = 0,
		ANNOUNCE_CHANNEL
	};

public:
	BOOL Startup(TCHAR* pszTeacherName, DWORD dwLocalAddr, PCLIENTID pClientId, int nCount, DWORD* pdwChannelId);
	void Shutdown();

	void SetLocked(BOOL bLock);

	void ResetUnconnectedMembers(PCLIENTID pClientId, int nCount);
	void AddUnconnectedMember(PCLIENTID pClientId);
	void RemoveUnconnectedMember(PCLIENTID pClientId);

	void SendRejectConnectToStudent(DWORD dwIpAddress, int nChannelId, DWORD dwRejectCode);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData);
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void SendApplyChannel();
	void SendAnnounceChannel();
	void SendQuitChannel(DWORD nChannelId);
	void SendFightChannel(DWORD dwTeacherIp);
	void SendAnswerAccessible(DWORD dwClientIp);

	void ReportConnectCanceledEvent(BYTE* abMac, DWORD dwClientIp);

	CRITICAL_SECTION	m_cs;

	DWORD				m_dwLocalAddr;

	int					m_dwChannelId;
	BOOL				m_bApplySuccess;
	DWORD				m_dwStartApplyTime;

	TCHAR				m_szTeacherName[32];

	BOOL				m_bStarted;
	BOOL				m_bLocked;

	int					m_nNumClient;
	CLIENTID			m_abClientId[MAX_CLIENTS_NUMBER];

	HANDLE				m_hEvent;
};

class LIBBASETRANS_API CPassiveFindTeacherTransaction : public CTransactionHandler
{
public:
	CPassiveFindTeacherTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CPassiveFindTeacherTransaction();

	enum TIME_ID
	{
		CHECK_CHANNEL = 0,
	};

public:
	BOOL Startup(DWORD dwLocalAddr, PCLIENTID pClientId);
	void Shutdown();

	void SetLogIn(BOOL bLogin, DWORD dwTeacherIp);

	BOOL IsChannelLocked(DWORD nChannelId);
	void SendCancelConnectToTeacher(DWORD nChannelId, BYTE* pabMac);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void ReportChannelArriveEvent(LPCTSTR lpszName, DWORD dwIpAddress, DWORD nChannelId);
	void ReportChannelLeaveEvent(DWORD nChannelId);
	void ReportChannelTimeoutEvent(DWORD nChannelId);
	void ReportChannelForceEvent(DWORD nChannelId);
	void ReportChannelReloginEvent(DWORD nChannelId);
	void ReportChannelLockChanngedEvent(DWORD nChannelId, BOOL bLock);
	void ReportChannelRejectEvent(DWORD dwTeacherIp, DWORD nChannelId, DWORD dwRejectCode);

	void CheckChannelLastAnnounceTime();
	void SendAskAccessible(DWORD dwTeacherIp);

	void UpdateChannelLastAnnounce(PFINDTEACHER_ANNOUNCE_CHANNEL pAnnounceChannel);
	BOOL HasChannel(DWORD nChannelId);
	void InsertChannel(PFINDTEACHER_ANNOUNCE_CHANNEL pAnnounceChannel);
	void DeleteChannel(DWORD nChannelId);
	
	DWORD GetChannelTeacherIp(DWORD nChannelId);
	int CompareClientId(CLIENTID srcClientId, CLIENTID dstClientId);


	CRITICAL_SECTION	m_cs;

	DWORD				m_dwLocalAddr;
	BOOL				m_bStarted;
	BOOL				m_bLogin;
	DWORD				m_dwTeacherIp;

	CLIENTID			m_idClient;

	typedef map<DWORD, TEACHER_CHANNEL*> CChannelMap;
	CChannelMap		m_channelMap;
};

typedef struct tagNAME_CONTEXT
{
	DWORD dwClientIp;

	int m_nReturnCount;
	int m_nChangeCount;
	int m_nReplyCount;
	TCHAR szName[32];
}NAME_CONTEXT, *PNAME_CONTEXT;

class LIBBASETRANS_API CNameTransaction : public CTransactionHandler
{
public:
	CNameTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CNameTransaction();

public:
	BOOL Startup();
	void Shutdown();

	void AddClient(CLIENTID clientId, TCHAR* pszName);
	void RemoveClient(CLIENTID clientId);
	void SetClientLogin(CLIENTID clientId, DWORD dwClientIp);
	void SetClientLogout(CLIENTID clientId);
	void SetClientName(CLIENTID clientId, TCHAR* pszName);

	void NotifyMemberNameChanged(CLIENTID clientId, TCHAR* pszNewName);
	void NotifyMemberNameConflicted(CLIENTID clientId, TCHAR* pszOldName);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);
protected:

	void SendNameChangePacket(DWORD dwClientIp, int nRefCount, TCHAR* pszNewName);
	void SendRenameRequestRetPacket(DWORD dwClientIp, BOOL bAccpted, int nRefCount, TCHAR* pszOldName);
	void ReportNameChangedEvent(CLIENTID id, TCHAR* pszNewName);

	BOOL HasSameName(TCHAR* pszName);

	struct CClientIDCompare {
		bool operator()(CLIENTID id1, CLIENTID id2) const {
			return memcmp(&id1, &id2, sizeof(CLIENTID)) < 0;
		}
	};

	typedef std::map <CLIENTID, PNAME_CONTEXT, CClientIDCompare> CNameMap;
	CNameMap m_nameMap;

	CRITICAL_SECTION	m_cs;

	BOOL	m_bStarted;
};

class LIBBASETRANS_API CPassiveNameTransaction : public CTransactionHandler
{
public:
	CPassiveNameTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CPassiveNameTransaction();

public:
	BOOL Startup(DWORD dwTeacherIp, PCLIENTID pClientId, TCHAR* pszName);
	void Shutdown();

	void SetName(TCHAR* pszName);
	void NotifyTeacherNameChanged(TCHAR* pszNewName);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);
protected:

	void SendNameChangePacket(DWORD dwTeacherIp, int nRefCount, TCHAR* pszNewName);
	void SendNameChangeRetPacket(int nRefCount);

	void ReportNameChangedEvent(TCHAR* pszNewName);
	void ReportRenameRequestRetEvent(BOOL bAccepted, TCHAR* pszOldName);

	CRITICAL_SECTION	m_cs;
	BOOL	m_bStarted;

	int m_nReturnCount;
	int m_nChangeCount;
	int m_nReplyCount;

	TCHAR m_szName[32];

	DWORD m_dwTeacherIp;
	CLIENTID m_id;
};

class LIBBASETRANS_API CTeacherExitTransacation : public CTransactionHandler
{
public:
	CTeacherExitTransacation(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CTeacherExitTransacation();

public:
	BOOL Startup(DWORD dwTeacherIp, PCLIENTID pClientId, int nCount, DWORD dwTimeout);
	void Shutdown();

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void SendExitPacket();
	void ProcessExitReturn(PPACKET_TEACHER_EXIT_RET pPacketRet);
	void ReportExitTimoutEvent();
	void ReportExitFinishedEvent();

	CRITICAL_SECTION	m_cs;

	typedef std::vector<CLIENTID> CClientIdList;
	CClientIdList	m_listClientId;
	BOOL			m_bFinish;

	BOOL			m_bStarted;
	DWORD			m_dwTeacherIp;
};

class LIBBASETRANS_API CCommandTransaction : public CTransactionHandler
{
public:
	CCommandTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CCommandTransaction();

public:
	void StartSendCommand(DWORD dwTeacherIp, DWORD* pdwIpAddress, int nCount, LPBYTE lpCmdBuffer, DWORD cbSize);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void SendCommand(DWORD dwClientIp);
	void SendCommandConfirm(DWORD dwIpAddress);

	void Destroy();

	typedef std::vector<DWORD> CReceiveList;
	CReceiveList		m_listReceivers;
	BYTE				m_abCommand[2048];
	DWORD				m_cbCommand;

	DWORD				m_dwTeacherIp;

	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CPassiveCommandTransaction : public CTransactionHandler
{
public:
	CPassiveCommandTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CPassiveCommandTransaction();

public:
	void ProcesssCommand(PPACKET_COMMAND pPacketCommand);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void SendCommandRet(DWORD dwIpAddress);
	void ReportCommandArrivedEvent(BYTE* pabCommand, DWORD cbCommand);

	DWORD m_dwTeacherIp;
	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CLiteCommandTransaction : public CTransactionHandler
{
public:
	CLiteCommandTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CLiteCommandTransaction();

public:
	void SendLiteCommand(DWORD* pdwIpAddress, int nCount, LPBYTE lpCmdBuffer, DWORD cbSize);

public:
	virtual void OnTimeout(DWORD dwTimeID) {}
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData) {}
};

#define CLEAR_SAVEFOLDER_BEFORE_COLLECT		0x00000001
#define REMOVE_CLIENTFILE_AFTER_COLLECT		0x00000002
#define SAVE_SUBFOLDER_WITH_STUDENTNAME		0x00000004
#define SAVE_SUBFOLDER_WITH_MACHINENAME		0x00000008

typedef struct tagCOLLECTMISSION
{
	DWORD dwTaskId;
	DWORD dwFlags;
	TCHAR szFileType[MAX_PATH];
	TCHAR szFilePath[MAX_PATH];
	TCHAR szFileSaveFolder[MAX_PATH];
}COLLECTMISSION, *PCOLLECTMISSION;

typedef struct tagCOLLECTCONTEXT
{
	DWORD dwState;
	DWORD dwFileCount;
	LONGLONG llFileSize;
	DWORD dwErrorCode;
}COLLECTCONTEXT, *PCOLLECTCONTEXT;

enum COLLECT_STATE
{
	COLLECT_NONE = 0,
	COLLECT_PREPARED,
	COLLECT_TRANSMITTING,
	COLLECT_FINISHED
};

enum COLLECT_ERROR
{
	CE_NONE = 0,
	CE_NAMECONNECT,
	CE_UPLOADFILE,
	CE_OPENFILE,
	CE_READFILE,
	CE_DELFILE
};

class LIBBASETRANS_API CFileCollectTransaction : public CTransactionHandler
{
public:
	CFileCollectTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CFileCollectTransaction();

public:
	void SetCollectMission(PCOLLECTMISSION pCollectMission);
	void SetCollectMembers(int nCount, PMEMBERINFO* ppMemberInfo);
	void Start(DWORD dwLocalInterface, BOOL bCollectAll = TRUE);
	void Stop();
	BOOL IsCollectting() {return m_bCollecting;}
	BOOL CanStartLastMission() {return (m_dwTaskId != 0);}
	BOOL OnMemberConnected(PMEMBERINFO pMemberInfo);
	BOOL OnMemberDisconnected(PMEMBERINFO pMemberInfo);
	void StopMember(PMEMBERINFO pMemberInfo);
	void StartMember(int nCount, PMEMBERINFO* ppMemberInfo);
	DWORD GetMemberState(PMEMBERINFO pMemberInfo);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void ProcessStatePacket(PPACKET_COLLECT_STATE pState);
	void SendCollectCommand(PMEMBERINFO pMemberInfo);
	void SendStopCommand(PMEMBERINFO pMemberInfo);
	void ReportStateChangedEvent(PPACKET_COLLECT_STATE pState);
	void ReportFinishedEvent();
	void ReportClientErrorEvent(PPACKET_COLLECT_EVENT pPacket);
	void SendTransmitCommand(PMEMBERINFO pMemberInfo);
	void ResetContext(PCOLLECTCONTEXT pContext);
	void DeleteAllMembers();
	BOOL IsCollectFinished();

protected:
	typedef std::map <PMEMBERINFO, PCOLLECTCONTEXT> CCollectMap;

	CCollectMap	m_collectMap;

	CRITICAL_SECTION	m_cs;

	BOOL			m_bCollecting;

	DWORD m_dwTaskId;
	COLLECTMISSION m_CollectMission;
	DWORD m_dwLocalInterface;

	BOOL	m_bCollectAll;
};

typedef struct tagCOLLECTFILE
{
	TCHAR szPathName[MAX_PATH];
}COLLECTFILE, *PCOLLECTITEM;

class IMacroPathTranslator
{
public:
	virtual void TranslatePath(TCHAR* pszPath) = NULL;
};

class LIBBASETRANS_API CPassiveFileCollectTransaction : public CTransactionHandler
{
public:
	CPassiveFileCollectTransaction(ICommunicationLayer* pComLayer, GUID *pTransID, IMacroPathTranslator* pTranslator);
	virtual ~CPassiveFileCollectTransaction();

public:

	void SetTransmitEvent(DWORD dwEvent, DWORD dwRequest, DWORD dwErrorCode);
	void OnTransmitFinished(DWORD dwErrorCode);

	int GetFileCount();
	DWORD GetFlags() {return m_dwFlags;}
	BOOL GetFileName(TCHAR* pszFileName);
	void PutFileName(TCHAR* pszFileName);
	void DeleteAllFiles();
	void Destroy();
	void LogEvent(DWORD dwErrorCode, DWORD cbSize, LPVOID lpvBuffer);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void SeachFilesInPath(TCHAR* pszFilePath, TCHAR* pszFileType);
	void CalculateCollectFileList(PPACKET_COLLECT_MISSION pCollectMission);
	void UpdateCollectState();
	void ReportBeginCollectEvent();
	void ReportEndCollectEvent();
	BOOL IsFileExist(COLLECTFILE* pFile);

protected:
	CRITICAL_SECTION	m_cs;
	DWORD				m_dwState;

	typedef std::vector<COLLECTFILE> CCollectFileList;
	CCollectFileList m_FileList;

	DWORD		m_dwTeacherIp;
	PMEMBERINFO	m_pMemberInfo;
	LONGLONG	m_llTotalSize;
	DWORD		m_dwTaskId;
	DWORD		m_dwErrorCode;
	DWORD		m_dwFlags;

	IMacroPathTranslator* m_pTranslator;
};

typedef struct tagCONFCALL_CONTEXT {
	BOOL bTeacher;
	DWORD dwState;
	DWORD dwTickCount;
	DWORD dwCallingIp;
	DWORD dwTalkingIp;
	TCHAR szName[32];
} CONFCALL_CONTEXT, *PCONFCALL_CONTEXT;

class LIBBASETRANS_API CConferenceCallTransaction : public CTransactionHandler
{
public:
	CConferenceCallTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CConferenceCallTransaction();

	enum CC_STATE
	{
		CC_IDLE = 0,
		CC_CALLING,
		CC_TALKING
	};

	enum TIMER_ID
	{
		TIMERID_ANNOUNCE = 0,
		TIMERID_LOOPCHECK,
		TIMERID_CALLING
	};

public:
	void Start(DWORD dwChannelMask, TCHAR* pszName, BOOL bTeacher);
	void Stop();

	void CallMember(DWORD dwIpAddress);
	void CancelCall();
	void AcceptMember(DWORD dwIpAddress);
	void DenyMember(DWORD dwIpAddress);

	void EnterTalking(DWORD dwIpAddress);
	void LeaveTalking();

	void SetName(TCHAR* pszName);

	DWORD GetTalkingIp(DWORD dwIpAddress);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void SendAnnouncePacket();
	void SendRespondPacket(BOOL dwIpAddress, BOOL bAccept);
	void SendExitPacket();

	void ProcessAnnouncePacket(PPACKET_CONFCALL_ANNOUNCE pAnncounce);
	void ProcessExitPacket(PPACKET_CONFCALL_EXIT pExit);

	void ReportNewMemberEvent(DWORD dwIpAddress, PCONFCALL_CONTEXT pContext);
	void ReportDelMemberEvent(DWORD dwIpAddress, PCONFCALL_CONTEXT pContext);
	void ReportStateChangedEvent(DWORD dwIpAddress, PCONFCALL_CONTEXT pContext);
	void ReportNameChangedEvent(DWORD dwIpAddress, PCONFCALL_CONTEXT pContext);
	void ReportRespondEvent(DWORD dwIpAddress, BOOL bAccept);

	void DeleteRespondMember(DWORD dwIpAddress);

protected:
	typedef std::map <DWORD, PCONFCALL_CONTEXT> CConfCallMap;
	CConfCallMap m_ConfCallMap;

	typedef std::map <DWORD, BOOL> CRespondMap;
	CRespondMap m_RespondMap;

	CRITICAL_SECTION	m_cs;

	DWORD m_dwChannelMask;
	BOOL m_bStarted;
	BOOL m_bTeacher;
	DWORD m_dwState;
	DWORD m_dwCallingIp;
	DWORD m_dwTalkingIp;

	TCHAR m_szName[32];
};

class LIBBASETRANS_API CAskGroupIdActiveTransaction : public CTransactionHandler
{
public:
	CAskGroupIdActiveTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	~CAskGroupIdActiveTransaction(void);

public:
	void SetGroupMap(DWORD dwIpAddress, DWORD dwGroupId, LPCTSTR pszGroupName, LPCTSTR pszName, WORD wVoicePort, DWORD dwState, UINT64 nCommand);
	void DelGroupMap(DWORD dwIpAddress);
	DWORD FindMemberGroupId(DWORD dwIpAddress);
	DWORD FindMemberState(DWORD dwIpAddress);
	UINT64 FindMemberCommand(DWORD dwIpAddress);

	void ModifyMemberState(DWORD dwIpAddress, DWORD dwState);
	void ModifyMemberState(DWORD dwIpAddress, DWORD dwMask, DWORD dwState);
	void ModifyGroupName(DWORD dwIpAddress, LPCTSTR lpszName);
	void ModifyMemberName(DWORD dwIpAddress, LPCTSTR lpszName);
	void ModifyMemberCommand(DWORD dwIpAddress, UINT64 nCommand);

public:
	virtual void OnTimeout(DWORD dwTimeID) {}
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}	
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void SendGroupIdReply(DWORD dwIpAddress, PANSWERGROUPID pGroupInfo);

private:
	typedef map<DWORD, PANSWERGROUPID> CAskGroupMap;
	CAskGroupMap		m_askMap;

	CRITICAL_SECTION	m_cs;
};


class LIBBASETRANS_API CAskGroupIdPassiveTransaction : public CTransactionHandler
{
public:
	CAskGroupIdPassiveTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	~CAskGroupIdPassiveTransaction(void);

public:
	void BeginAskGroupId(DWORD dwLocalInterface, DWORD dwTeacherIp);
	
public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void SendAskGroupIdPacket(DWORD dwLocalInterface, DWORD dwTeacherIp);
	void ReportGroupIdEvent(PANSWERGROUPID pAnswerGroupId);

protected:
	DWORD m_dwLocalInterface;
	DWORD m_dwTeacherIp;
};

typedef struct tagCHATMEMBERINFO
{
	DWORD dwGroupId;
	DWORD dwState;
	DWORD dwLastAnnounceTime;
	TCHAR szName[MAX_TEXT_LENGTH];
	UINT64 nCommand;
}CHATMEMBERINFO, *PCHATMEMBERINFO;

class LIBBASETRANS_API CGroupMemberTransaction : public CTransactionHandler
{
public:
	CGroupMemberTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	~CGroupMemberTransaction(void);

public:
	void JoinGroup(DWORD dwIpAddress, DWORD dwGroupId, TCHAR* pszName, BYTE* pabMac, DWORD dwState = 0, UINT64 nCommand = 0);
	void LeaveGroup(DWORD dwIpAddress, DWORD dwGroupId,TCHAR* pszName);

	void DeleteMemberFromMap(DWORD dwIpAddress);
	void ClearMemberMap();

	void SetName(TCHAR* pszName);
	void SetLeader(BOOL bLeader);
	void SetMute(BOOL bMute); 
	void SetSilent(BOOL bSilent);
	void SetCommand(UINT64 nCommand);
	
	DWORD FindMemberGroupId(DWORD dwIpAddress);

private:
	void SendJoinGroupPacket(DWORD dwIpAddress, DWORD dwGroupId, TCHAR* pszName, BYTE* pabMac, DWORD dwState = 0, UINT64 nCommand = 0);
	void SendAnnouncePacket();
	void SendLeaveGroupPacket(DWORD dwIpAddress, DWORD dwGroupId, TCHAR* pszName);

	void ResetAnnounce();

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	DWORD	m_dwGroupId;
	BOOL	m_bTeacher;
	BOOL	m_bLeader;
	BOOL	m_bMute;
	BOOL	m_bSilent;
	TCHAR	m_szName[32];

	BYTE	m_abMac[6];

	UINT64	m_nCommand;
	
	DWORD	m_dwLocalInterface;

	typedef map<DWORD, PCHATMEMBERINFO> CChatMemberMap;
	CChatMemberMap m_GroupMemberMap;

	CRITICAL_SECTION	m_cs;
	CRITICAL_SECTION	m_csAnnounce;

	BOOL    m_bInGroup;
	int		m_nAnnounceCount;

protected:
	void ReportMemberJoinEvent(DWORD dwIpAddress, DWORD dwGroupId, TCHAR* szName, BYTE* pabMac, DWORD dwState = 0, UINT64 nCommand = 0);
	void ReportMemberLeaveEvent(DWORD dwIpAddrss, DWORD dwGroupId, TCHAR* pszName);
	void ReportMemberNameChanged(DWORD dwIpAddress, DWORD dwGroupId, TCHAR* pszNewName);
	void ReportMemberStateChanged(DWORD dwIpAddress, DWORD dwGroupId, DWORD dwNewState);
	void ReportMemberCommandChanged(DWORD dwIpAddress, DWORD dwGroupId, UINT64 nCommand);
};

#define MAX_NEAREST_COUNT		10


#ifdef USE_PROXIMITY

class LIBBASETRANS_API CProximityTransaction : public  CTransactionHandler
{
public:
	CProximityTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CProximityTransaction();

public:
	BOOL StartProximity(DWORD dwGroupLeaderIp = 0, DWORD dwGroupId = 0xFFFFFFFF);
	void StopProximity();

	void AcceptInvite(DWORD dwGroupLeaderIp, DWORD dwGroupId);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void SendInviteCommand(DWORD dwNearIp);
	void ReportProximityInviteEvent(PPROXIMITY_INVITE pProximityInvite);

protected:
	DWORD m_dwGroupLeaderIp;
	DWORD m_dwGroupId;

	DWORD m_dwNearCount;
	DWORD m_pdwNearIp[MAX_NEAREST_COUNT];

	BOOL  m_bProcessInvite;

	BOOL m_bInited;

	HMODULE m_hDcsProximity;

	CRITICAL_SECTION	m_cs;
};

#endif // USE_PROXIMITY

class LIBBASETRANS_API CQuizCommandTransaction : public CTransactionHandler
{
public:
	CQuizCommandTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CQuizCommandTransaction();

public:
	void StartSendCommand(DWORD dwSenderIp, BYTE* pbMac, DWORD* pdwIpAddress, int nCount, LPBYTE lpCmdBuffer, DWORD cbSize);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void SendCommand(DWORD dwClientIp);
	void ReportSuccessEvent(DWORD dwClientIp);
	void ReportFailedEvent(DWORD dwClientIp);

	void Destroy();

	typedef std::vector<DWORD> CReceiveList;
	CReceiveList		m_listReceivers;
	BYTE				m_abCommand[2048];
	DWORD				m_cbCommand;

	DWORD				m_dwSenderIp;
	BYTE				m_abMac[6];

	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CQuizPassiveCommandTransaction : public CTransactionHandler
{
public:
	CQuizPassiveCommandTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CQuizPassiveCommandTransaction();

public:
	void ProcessCommand(PPACKET_QUIZ_COMMAND pPacketCommand);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void SendCommandRet(DWORD dwIpAddress);
	void ReportCommandArrivedEvent(PPACKET_QUIZ_COMMAND pPacketCommand);

	DWORD m_dwSenderIp;
	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CQuizBlockSenderTransaction : public CTransactionHandler
{
public:
	CQuizBlockSenderTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CQuizBlockSenderTransaction();

public:
	enum TIMER
	{
		ID_TIMER_SYNC = 0,
		ID_TIMER_STOP,
		ID_TIMER_DESTROY
	};

public:
	BOOL StartSendBlock(DWORD dwLocalInterface, DWORD dwReceiverIp, DWORD dwBlockType, BYTE* pbBuffer, DWORD cbSize);
	void Destroy();

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void CheckAndSendBlock();
	void SendDataPacket(int nBlockId);
	void SendSyncPacket();
	void SendStopPacket();
	void ReportSendOutEvent();

	DWORD GetRandomSyncTime() {return (400 + (abs(rand()) % 200));}
	DWORD GetRandomStopTime() {return (400 + (abs(rand()) % 200));}

protected:
	DWORD m_dwLocalInterface;
	DWORD m_dwReceiverIp;
	DWORD m_dwBlockType;
	BYTE* m_pBlockBuffer;
	DWORD m_dwBlockSize;

	int   m_nBlocks;
	BYTE* m_pRepairMap;

	int	  m_nSyncId;

	BOOL  m_bSendFinished;

	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CQuizBlockRecverTransaction : public CTransactionHandler
{
public:
	CQuizBlockRecverTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CQuizBlockRecverTransaction();

public:
	BOOL ProcessBlock(PPACKET_QUIZ_BLOCK pPacketBlock);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void SendBlockRet(int nSyncId);
	void SendBlockClose();
	void ReportBlockArrivedEvent();

protected:
	DWORD m_dwSenderIp;
	DWORD m_dwLocalInterface;
	DWORD m_dwBlockType;
	DWORD m_dwBlockSize;
	int   m_nBlocks;
	DWORD m_dwReceivedSize;

	BYTE* m_pBlockBuffer;
	BYTE* m_pRepairMap;

	BOOL  m_bGotBlock;

	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CBlockSenderTransaction : public CTransactionHandler
{
public:
	CBlockSenderTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CBlockSenderTransaction();

public:
	enum TIMER
	{
		ID_TIMER_SYNC = 0,
		ID_TIMER_STOP,
		ID_TIMER_DESTROY
	};

public:
	BOOL StartSendBlock(DWORD dwLocalInterface, DWORD dwReceiverIp, DWORD dwBlockType, BYTE* pbBuffer, DWORD cbSize);
	void Destroy();

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void CheckAndSendBlock();
	void SendDataPacket(int nBlockId);
	void SendSyncPacket();
	void SendStopPacket();
	void ReportSendOutEvent();

	DWORD GetRandomSyncTime() {return (400 + (abs(rand()) % 200));}
	DWORD GetRandomStopTime() {return (400 + (abs(rand()) % 200));}

protected:
	DWORD m_dwLocalInterface;
	DWORD m_dwReceiverIp;
	DWORD m_dwBlockType;
	BYTE* m_pBlockBuffer;
	DWORD m_dwBlockSize;

	int   m_nBlocks;
	BYTE* m_pRepairMap;

	int	  m_nSyncId;

	BOOL  m_bSendFinished;

	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CBlockRecverTransaction : public CTransactionHandler
{
public:
	CBlockRecverTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CBlockRecverTransaction();

public:
	BOOL ProcessBlock(PPACKET_BLOCK pPacketBlock);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void SendBlockRet(int nSyncId);
	void SendBlockClose();
	void ReportBlockArrivedEvent();

protected:
	DWORD m_dwSenderIp;
	DWORD m_dwLocalInterface;
	DWORD m_dwBlockType;
	DWORD m_dwBlockSize;
	int   m_nBlocks;
	DWORD m_dwReceivedSize;

	BYTE* m_pBlockBuffer;
	BYTE* m_pRepairMap;

	BOOL  m_bGotBlock;

	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CTestServerTransaction : public CTransactionHandler
{
public:
	CTestServerTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CTestServerTransaction();

public:
	void Start(DWORD dwLocalInterface);
	void Stop();
	
	void SendPingRet(const LPBYTE lpData, DWORD cbSize, DWORD dwClientIP);

public:
	virtual void OnTimeout(DWORD dwTimeID) {}
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void SendConnectRetPacket(PPACKET_QUIZ_CONNECT pPacketConnect);
	void ReportPingEvent(PPACKET_QUIZ_PING pPacketPing);


protected:
	typedef std::map <DWORD, DWORD> CConnectMap;
	CConnectMap m_connectMap;
	
	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CTestClientTransaction : public CTransactionHandler
{
public:
	CTestClientTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CTestClientTransaction();

public:
	void Start(DWORD dwTeacherIp, DWORD dwLocalInterface, BYTE* pbMac, BOOL bGrade = FALSE);
	void Stop();

	void SetPingBuffer(const LPBYTE lpBuffer, DWORD cbSize);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void SendConnectPacket();
	void SendPingPacket();

	void ReportPingRetEvent(PPACKET_QUIZ_PING_RET pPingRet);
	void ReportConnectSuccessEvent();
	void ReportConnectFailedEvent();

	DWORD GetRandomConnectTime() {return (400 + (abs(rand()) % 200));}
	DWORD GetRandomPingTime() {return (800 + (abs(rand()) % 400));}

protected:
	BOOL m_bGrade;
	BOOL m_bConnected;

	DWORD m_dwRetryCount;
//	DWORD m_dwPingIntercval;
	
	DWORD m_dwTeacherIp;
	DWORD m_dwLocalInterface;

	BYTE  m_abMac[6];

	BYTE		 m_abPing[1024];
	DWORD		 m_cbPing;

	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CQuizFileSenderTransaction : public CTransactionHandler
{
public:
	CQuizFileSenderTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CQuizFileSenderTransaction();

public:
	BOOL Start(DWORD dwLocalInterface, DWORD* pdwIpAddress, int nCount, LPCTSTR lpFileName);
	void Cancel();
	void Stop();

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void CheckAndSendQuizFile();
	void SendData(int nBlockId);
	void SendConfirm(DWORD dwIpAddress);
	void ReportQuizFileProgressEvent(PPACKET_QUIZDATA_RET pQuizDataRet);

protected:
	DWORD m_dwLocalInterface;

	typedef std::map <DWORD, DWORD> CReceiveList;
	CReceiveList m_listReceivers;

	DWORD m_dwFileSize;
	BYTE* m_pFileBuffer;
	int   m_nBlocks;
	BYTE* m_pRepairMap;

	BOOL m_bStarted;

	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CQuizFileRecverTransaction : public CTransactionHandler
{
public:
	CQuizFileRecverTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CQuizFileRecverTransaction();

public:
	BOOL Start(DWORD dwTeacherIp, DWORD dwLocalInterface, LPCTSTR lpFileName, DWORD dwFileSize);
	void Stop();

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void SendQuizDataRetPacket();
	void ReportQuizFileFailedEvent();
	void ReportQuizFileSuccessEvent();
	void ReportQuizFileProgressEvent();

protected:
	typedef std::vector<int> CRepairList;

	DWORD m_dwTeacherIp;
	DWORD m_dwLocalInterface;

	HANDLE m_hFile;
	BYTE* m_pFileBuffer;
	int   m_nBlocks;
	BYTE* m_pRepairMap;
	DWORD m_dwFileSize;
	DWORD m_dwReceivedSize;

	CRITICAL_SECTION	m_cs;

	BOOL m_bDownload;
};

typedef struct tagPOLICY_CONTEXT
{
	DWORD dwAppPolicyCommand;
	DWORD dwAppPolicyState;

	DWORD dwWebPolicyCommand;
	DWORD dwWebPolicyState;

	DWORD dwUsbPolicyCommand;
	DWORD dwUsbPolicyState;

	DWORD dwPrintPolicyCommand;
	DWORD dwPrintPolicyState;
}POLICY_CONTEXT, *PPOLICY_CONTEXT;

class LIBBASETRANS_API CPolicyTransaction : public CTransactionHandler
{
public:
	CPolicyTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CPolicyTransaction();

public:
	void AddMember(PMEMBERINFO pMemberInfo, DWORD dwAppPolicy, DWORD dwWebPolicy, DWORD dwUsbPolicy, DWORD dwPrintPolicy);
	void DelMember(PMEMBERINFO pMemberInfo);

	void SetAppPolicy(PMEMBERINFO pMemberInfo, DWORD dwPolicy);
	void SetWebPolicy(PMEMBERINFO pMemberInfo, DWORD dwPolicy);
	void SetUsbPolicy(PMEMBERINFO pMemberInfo, DWORD dwPolicy);
	void SetPrintPolicy(PMEMBERINFO pMemberInfo, DWORD dwPolicy);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void SendPolicyCommand(PMEMBERINFO pMemberInfo, PPOLICY_CONTEXT pPolicyContext);
	void ProcessPolicyState(PPOLICY_STATE pPolicyState);
private:
	typedef std::map <PMEMBERINFO, PPOLICY_CONTEXT> CPolicyMap;
	CPolicyMap m_policyMap;

	BOOL	m_bStarted;
	CRITICAL_SECTION	m_cs;
};

class LIBBASETRANS_API CPolicyClientTransaction : public CTransactionHandler
{
public:
	CPolicyClientTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CPolicyClientTransaction();

public:

public:
	virtual void OnTimeout(DWORD dwTimeID) {}
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

private:
	void ProcessPolicyCommand(PPOLICY_COMMAND pPolicyCommand);
	void SendPolicyState(DWORD dwTeacherIp);
	void ReportPolicyChangeEvent(DWORD dwPolicyMask);

private:
	CRITICAL_SECTION	m_cs;

	DWORD m_dwAppPolicy;
	DWORD m_dwWebPolicy;
	DWORD m_dwUsbPolicy;
	DWORD m_dwPrintPolicy;
};

typedef struct tagAPPLICATIONDATA
{
	WORD  cbSize;
	DWORD dwProcessId;
	WORD  wNameSize;
	WORD  wNameOffset;
	WORD  wTitleSize;
	WORD  wTitleOffset;
	WORD  wbmBitsPixel;
	WORD  wIconSize;
	WORD  wIconOffset;
	BYTE  abParams[1];
}APPLICATIONDATA, *PAPPLICATIONDATA;

class LIBBASETRANS_API CApplicationTransaction : public CTransactionHandler
{
public:
	CApplicationTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CApplicationTransaction();

public:
	// New interfaces
	void AddStudent(DWORD dwIpAddress);
	void DelStudent(DWORD dwIpAddress);
	void DeleteAllStudents();

public:
	// CTransactionHandler
	virtual void OnTimeout(DWORD dwTimeID) {}
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	typedef std::vector<DWORD> CProcIDList;
	typedef map<DWORD, CProcIDList*> CProcessMap;
	CProcessMap	m_procMap;

	CRITICAL_SECTION	m_cs;

protected:
	void ProcessProcIdList(PPROCESSID_LIST pProcIdList);
	void ProcessProcInfoRet(PPROCESSINFO_RET pProcInfoRet);

	void SendProcRequestPacket(DWORD dwStudentIp, DWORD dwProcId);

	void ReportProcCreateEvent(PPROCESSINFO_RET pProcInfoRet);
	void ReportProcExitEvent(DWORD dwStudentIp, int nExitCount, DWORD* pdwExitProcId);

	BOOL ProcIDInList(DWORD dwProcID, CProcIDList* pProcessIdList);
	void DelProcIDInList(DWORD dwProcID, CProcIDList* pProcessIdList);
};

class LIBBASETRANS_API CPassiveApplicationTransaction : public CTransactionHandler
{
public:
	CPassiveApplicationTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CPassiveApplicationTransaction();

public:
	void Startup(DWORD dwTeacherIp, int nUpdateSeconds = 5);
	void Shutdown();

	void SendApplicationList(LPVOID pvAppItem, DWORD nCount);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void ProcessProcInfoRequest(PPROCESSINFO_REQ pProcInfoReq);
	void ReportUpdateAppListEvent();
	void SendProcessIdList();
	void SendProcessInfo(PAPPLICATIONDATA pAppData);

protected:
	typedef std::vector<PAPPLICATIONDATA> CApplicatonList;
	CApplicatonList m_appList;

	BOOL m_bStarted;
	DWORD m_dwTeacherIp;
	int m_nUpdateSeconds;

	CRITICAL_SECTION	m_cs;
};

typedef struct tagWEBSITEDATA
{
	WORD  cbSize;
	DWORD dwWebId;
	WORD  wURLSize;
	WORD  wURLOffset;
	WORD  wNameSize;
	WORD  wNameOffset;
	WORD  wIconSize;
	WORD  wIconOffset;
	BYTE  abParams[1];
}WEBSITEDATA, *PWEBSITEDATA;

class LIBBASETRANS_API CWebSiteTransaction : public CTransactionHandler
{
public:
	CWebSiteTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CWebSiteTransaction();

public:
	// New interfaces
	void AddStudent(DWORD dwIpAddress);
	void DelStudent(DWORD dwIpAddress);
	void DeleteAllStudents();

public:
	// CTransactionHandler
	virtual void OnTimeout(DWORD dwTimeID) {}
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	typedef std::vector<DWORD> CWebIDList;
	typedef map<DWORD, CWebIDList*> CWebSiteMap;
	CWebSiteMap	m_webMap;

	CRITICAL_SECTION	m_cs;

protected:
	void ProcessWebIdList(PWEBID_LIST pWebIdList);
	void ProcessWebInfoRet(PWEBINFO_RET pWebInfoRet);

	void SendWebRequestPacket(DWORD dwStudentIp, DWORD dwWebId);

	void ReportWebCreateEvent(PWEBINFO_RET pWebInfoRet);
	void ReportWebExitEvent(DWORD dwStudentIp, int nExitCount, DWORD* pdwExitWebId);

	BOOL WebIDInList(DWORD dwWebID, CWebIDList* pWebIdList);
	void DelWebIDInList(DWORD dwWebID, CWebIDList* pWebIdList);
};

class LIBBASETRANS_API CPassiveWebSiteTransaction : public CTransactionHandler
{
public:
	CPassiveWebSiteTransaction(ICommunicationLayer* pComLayer, GUID *pTransID);
	virtual ~CPassiveWebSiteTransaction();

public:
	void Startup(DWORD dwTeacherIp, int nUpdateSeconds = 5);
	void Shutdown();

	void SendWebSiteList(LPVOID pvWebItem, DWORD nCount);

public:
	virtual void OnTimeout(DWORD dwTimeID);
	virtual void OnSendComplete(LPVOID lpvData, DWORD cbData) {}
	virtual void OnReceiveComplete(LPVOID lpvData, DWORD cbData);

protected:
	void ProcessWebInfoRequest(PWEBINFO_REQ pWebInfoReq);
	void ReportUpdateWebListEvent();
	void SendWebIdList();
	void SendWebInfo(PWEBSITEDATA pWebData);

protected:
	typedef std::vector<PWEBSITEDATA> CWebSiteList;
	CWebSiteList m_webList;

	BOOL m_bStarted;
	DWORD m_dwTeacherIp;
	int m_nUpdateSeconds;

	CRITICAL_SECTION	m_cs;
};

#endif //__LANGLABTRANSACTIONS_H__