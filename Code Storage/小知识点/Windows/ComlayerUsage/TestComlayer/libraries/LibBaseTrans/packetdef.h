#ifndef __PACKETDEF_H__
#define __PACKETDEF_H__

#pragma pack(push, 1)

#define MAGIC_SUBMIT_REQUEST			'SMRQ'
#define MAGIC_SUBMIT_CANCEL				'SMCC'
#define MAGIC_SUBMIT_REQUEST_RET		'SMRR'
#define MAGIC_SUBMIT_CANCEL_RET			'SMCR'
#define MAGIC_SUBMIT_COMPLETED			'SMOK'
#define MAGIC_SUBMIT_COMPLETE_RET		'SMOR'

#define MAGIC_TNAIL_REPORT				'TNAL'
#define MAGIC_TNAIL_REPORT_END			'TNED'
#define MAGIC_TNAIL_REPORT_RESULT		'TNRS'
#define MAGIC_TNAIL_POLICY_REQ			'TNRQ'
#define MAGIC_TNAIL_POLICY				'TNPL'
#define MAGIC_TNAIL_POLICY_RET			'TNRT'

// Find Teacher
#define MAGIC_APPLY_CHANNEL				'CNAP'
#define MAGIC_FIGHT_CHANNEL				'CNFT'
#define MAGIC_ANNOUNCE_CHANNEL			'CNAN'
#define MAGIC_QUIT_CHANNEL				'CNQT'
#define MAGIC_REJECT_CONNECT			'CTRJ'
#define MAGIC_CANCEL_CONNECT			'CTCN'
#define MAGIC_ASK_ACCESS				'ACAK'
#define MAGIC_ANSWER_ACCESS				'ACAW'

// Change Name
#define MAGIC_NAME_CHANGE				'NMCG'
#define MAGIC_NAME_CHANGE_RET			'NMRT'
#define MAGIC_RENAME_REQUEST			'RNRQ'
#define MAGIC_RENAME_REQUEST_RET		'RNRT'
#define MAGIC_NAME_CONFLICT				'NFNT'
#define MAGIC_NAME_CONFLICT_RET			'NFRT'

// Teacher Exit
#define MAGIC_TEACHER_EXIT				'TXNF'
#define MAGIC_TEACHER_EXIT_RET			'TXRT'

// COMMAND
#define MAGIC_COMMAND					'COMD'
#define MAGIC_COMMAND_RET				'CMRT'

// LITE COMMAND
#define MAGIC_LITE_COMMAND				'LCMD'

// Quiz
#define MAGIC_QUIZ_COMMAND				'QCMD'
#define MAGIC_QUIZ_COMMAND_RET			'QMRT'
#define MAGIC_QUIZ_BLOCK				'QBLK'
#define MAGIC_QUIZ_BLOCK_RET			'QBRT'
#define MAGIC_QUIZ_BLOCK_CLOSE			'QBCS'

#define MAGIC_QUIZ_CONNECT				'QCNT'
#define MAGIC_QUIZ_CONNECT_RET			'QCRT'
#define MAGIC_QUIZ_PING					'QPIN'
#define MAGIC_QUIZ_PING_RET				'QPRT'

#define MAGIC_QUIZ_DATA					'QDAT'
#define MAGIC_QUIZ_DATA_RET				'QDRT'
#define MAGIC_QUIZ_DATA_CONFIRM			'QDCF'

// File Collect
#define MAGIC_FILECOLLECT_MISSION		'FCMN'
#define MAGIC_FILECOLLECT_STOP			'FCSP'
#define MAGIC_FILECOLLECT_STATE			'FCST'
#define MAGIC_FILECOLLECT_TRANSMIT		'FCTM'
#define MAGIC_FILECOLLECT_EVENT			'FCEV'

// Conference Call
#define MAGIC_CONFCALL_ANNOUNCE			'CCAN'
#define MAGIC_CONFCALL_RESPOND			'CCRD'
#define MAGIC_CONFCALL_EXIT				'CCET'

// Group Chat
#define MAGIC_ASKGROUPID				'AKGP'
#define MAGIC_ANSWERGROUPID				'ASGP'
#define MAGIC_JOINGROUPCHAT				'JIGP'
#define MAGIC_LEAVEGROUPCHAT			'LVGP'
#define MAGIC_ANNOUCEGROUPCHAT			'ANGP'
#define MAGIC_SETGROUPMEMBERNAME		'SGMN'
#define MAGIC_SETGROUPMEMBERSTATE		'SGMS'

#define MAGIC_CHATCOMMAND				'CCMD'
#define MAGIC_CHATCOMMAND_RET			'CCMR'

// Block
#define MAGIC_BLOCK						'BLCK'
#define MAGIC_BLOCK_RET					'BKRT'
#define MAGIC_BLOCK_CLOSE				'BKCS'

// Policy
#define MAGIC_POLICY_COMMAND			'PCMD'
#define MAGIC_POLICY_STATE				'PSTA'

// Application
#define MAGIC_PROCESSID_LIST			'PIDL'
#define MAGIC_PROCESSINFO_REQ			'PIRQ'
#define MAGIC_PROCESSINFO_RET			'PIRT'

// Web Site
#define MAGIC_WEBID_LIST				'WIDL'
#define MAGIC_WEBINFO_REQ				'WIRQ'
#define MAGIC_WEBINFO_RET				'WIRT'

// Proximity
#define MAGIC_PROXIMITY_INVITE			'PXIN'

// Submit
typedef struct tagSUBMIT_REQUEST {
	GUID guidRequest;
	DWORD dwStudentIp;
	LONGLONG llSize;
	DWORD dwCount;
	TCHAR szStudentName[32];
	TCHAR szFileNames[MAX_PATH];
} SUBMIT_REQUEST, *PSUBMIT_REQUEST;

typedef struct tagSUBMIT_REQUEST_RET {
	BOOL bEnable;
	BOOL bQuotaPass;
	BOOL bAccept;
	LONGLONG llQuotaSize;
	DWORD dwQuotaCount;
} SUBMIT_REQUEST_RET, *PSUBMIT_REQUEST_RET;

typedef struct tagSUBMIT_CANCEL {
	GUID guidRequest;
	DWORD dwStudentIp;
} SUBMIT_CANCEL, *PSUBMIT_CANCEL;

typedef struct tagSUBMIT_CANCEL_RET {
	BOOL bAccept;
} SUBMIT_CANCEL_RET, *PSUBMIT_CANCEL_RET;

typedef struct tagSUBMIT_COMPLETED {
	GUID guidRequest;
	DWORD dwStudentIp;
	int nCompletedFileCount;
	TCHAR szStudentName[32];
} SUBMIT_COMPLETED, *PSUBMIT_COMPLETED;

typedef struct tagSUBMIT_COMPLETE_RET {
	DWORD dwStudentIp;
} SUBMIT_COMPLETE_RET, *PSUBMIT_COMPLETE_RET;

// Thumbnail
typedef struct tagPACKET_THUMBNAIL {
	DWORD dwMemberId;
	DWORD dwThumbnailId;
	DWORD dwTotalSize;
	DWORD dwOffset;
	DWORD cbSize;
	BYTE  abBuffer[1];
}PACKET_THUMBNAIL, *PPACKET_THUMBNAIL;

typedef struct tagPACKET_THUMBNAIL_END {
	DWORD dwMemberId;
	DWORD dwMemberIp;
	DWORD dwThumbnailId;
}PACKET_THUMBNAIL_END, *PPACKET_THUMBNAIL_END;

typedef struct tagPACKET_THUMBNAIL_RESULT {
	DWORD dwThumbnailId;
	BOOL  bReceivedOk;
	DWORD dwLostCount;
	WORD  abLostIds[1];
}PACKET_THUMBNAIL_RESULT, *PPACKET_THUMBNAIL_RESULT;

typedef struct tagPACKET_THUMBNAIL_POLICY_REQ {
	DWORD dwMemberId;
	DWORD dwMemberIp;
}PACKET_THUMBNAIL_POLICY_REQ, *PPACKET_THUMBNAIL_POLICY_REQ;

typedef struct tagPACKET_THUMBNAIL_POLICY {
	DWORD dwPolicyId;
	BOOL  bEnable;
	int   nWidth;
	int   nHeight;
	DWORD dwInterval;
}PACKET_THUMBNAIL_POLICY, *PPACKET_THUMBNAIL_POLICY;

typedef struct tagPACKET_THUMBNAIL_POLICY_RET {
	DWORD dwMemberId;
	DWORD dwPolicyId;
}PACKET_THUMBNAIL_POLICY_RET, *PPACKET_THUMBNAIL_POLICY_RET;

// Find Teacher
typedef struct tagFINDTEACHER_APPLY_CHANNEL
{
	DWORD nChannelId;
	DWORD dwTeacherIp;
}FINDTEACHER_APPLY_CHANNEL, *PFINDTEACHER_APPLY_CHANNEL;

typedef struct tagFINDTEACHER_FIGHT_CHANNEL
{
	DWORD dwFighterAddress;
}FINDTEACHER_FIGHT_CHANNEL, *PFINDTEACHER_FIGHT_CHANNEL;

typedef struct tagFINDTEACHER_ANNOUNCE_CHANNEL
{
	DWORD nChannelId : 8;
	DWORD dwClientCount : 8;
	DWORD bLock : 1;
	DWORD nNameLength : 15;
	DWORD dwIpAddress;
	BYTE  abParams[1];
//	char  szName[32];
//	CLIENTID abClientId[1];
}FINDTEACHER_ANNOUNCE_CHANNEL, *PFINDTEACHER_ANNOUNCE_CHANNEL;

typedef struct tagFINDTEACHER_QUIT_CHANNEL
{
	DWORD nChannelId;
}FINDTEACHER_QUIT_CHANNEL, *PFINDTEACHER_QUIT_CHANNEL;

typedef struct tagFINDTEACHER_QUERY_CLASS
{
	DWORD dwClientIp;
	BYTE  abMacAddress[6];
	TCHAR  szStudentName[32];
	TCHAR  szComputerName[32];
}FINDTEACHER_QUERY_CLASS, *PFINDTEACHER_QUERY_CLASS;

typedef struct tagFINDTEACHER_ANSWER_CLASS
{
	BOOL bInClass;
	BOOL bLock;
	DWORD dwLockCount;
}FINDTEACHER_ANSWER_CLASS, *PFINDTEACHER_ANSWER_CLASS;

typedef struct tagFINDTEACHER_JOIN_CLASS
{
	DWORD dwTeacherIp;
	DWORD nChannelId;
	TCHAR  szName[32];
}FINDTEACHER_JOIN_CLASS, *PFINDTEACHER_JOIN_CLASS;

typedef struct tagFINDTEACHER_REJECT_CONNECT
{
	DWORD dwTeacherIp;
	DWORD nChannelId;
	DWORD dwRejectCode;
}FINDTEACHER_REJECT_CONNECT, *PFINDTEACHER_REJECT_CONNECT;

typedef struct tagFINDTEACHER_CANCEL_CONNECT
{
	BYTE  abMacAddress[6];
	DWORD dwClientIp;
}FINDTEACHER_CANCEL_CONNECT, *PFINDTEACHER_CANCEL_CONNECT;

typedef struct tagFINDTEACHER_ASK_ACCESS
{
	DWORD dwStudentIp;
}FINDTEACHER_ASK_ACCESS, *PFINDTEACHER_ASK_ACCESS;

typedef struct tagFINDTEACHER_ANSWER_ACCESS
{
	DWORD dwTeacherIp;
	DWORD nChannelId;
}FINDTEACHER_ANSWER_ACCESS, *PFINDTEACHER_ANSWER_ACCESS;

typedef struct tagPACKET_NAME_CHANGE
{
	int nRefCount;
	TCHAR szNewName[32];
}PACKET_NAME_CHANGE, *PPACKET_NAME_CHANGE;

typedef struct tagPACKET_NAME_CHANGE_RET
{
	CLIENTID id;
	int nRefCound;
}PACKET_NAME_CHANGE_RET, *PPACKET_NAME_CHANGE_RET;

typedef struct tagPACKET_RENAME_REQUEST
{
	CLIENTID id;
	int nRefCount;
	TCHAR szNewName[32];
}PACKET_RENAME_REQUEST, *PPACKET_RENAME_REQUEST;

typedef struct tagPACKET_RENAME_REQUEST_RET
{
	BOOL bAccpted;
	int nRefCount;
	TCHAR szOldName[32];
}PACKET_RENAME_REQUEST_RET, *PPACKET_RENAME_REQUEST_RET;

typedef struct tagPACKET_TEACHER_EXIT
{
	DWORD dwTeacherIp;
	int nCount;
	CLIENTID id[1];
}PACKET_TEACHER_EXIT, *PPACKET_TEACHER_EXIT;

typedef struct tagPACKET_TEACHER_EXIT_RET
{
	DWORD dwClientIp;
	CLIENTID id;
}PACKET_TEACHER_EXIT_RET, *PPACKET_TEACHER_EXIT_RET;

typedef struct tagPACKET_COMMAND {
	DWORD dwTimeout;
	DWORD dwIpAddress;
	DWORD cbCmdSize;
	BYTE  abCmdBuffer[1];
}PACKET_COMMAND, *PPACKET_COMMAND;

typedef struct tagPACKET_COMMAND_RET {
	DWORD dwIpAddress;
}PACKET_COMMAND_RET, *PPACKET_COMMAND_RET;

typedef struct tagPACKET_LITE_COMMAND {
	DWORD cbCmdSize;
	BYTE  abCmdBuffer[1];
}PACKET_LITE_COMMAND, *PPACKET_LITE_COMMAND;

// Quiz
typedef struct tagPACKET_QUIZ_COMMAND {
	DWORD dwTimeout;
	DWORD dwSenderIp;
	DWORD dwReceiverIp;
	BYTE abMac[6];
	DWORD cbCmdSize;
	BYTE  abCmdBuffer[1];
}PACKET_QUIZ_COMMAND, *PPACKET_QUIZ_COMMAND;

typedef struct tagPACKET_QUIZ_COMMAND_RET {
	DWORD dwIpAddress;
}PACKET_QUIZ_COMMAND_RET, *PPACKET_QUIZ_COMMAND_RET;

#define BLOCK_DATA_PACKET			0x00000001
#define BLOCK_SYNC_PACKET			0x00000002
#define BLOCK_STOP_PACKET			0x00000004

typedef struct tagPACKET_QUIZ_BLOCK {
	DWORD dwFlags;
	DWORD dwSenderIp;
	DWORD dwReceiverIp;
	DWORD dwBlockType;
	DWORD dwBlockSize;
	int   nBlockId;
	int   nSyncId;
	DWORD cbSize;
	BYTE  abBuffer[1];
}PACKET_QUIZ_BLOCK, *PPACKET_QUIZ_BLOCK;

typedef struct tagPACKET_QUIZ_BLOCK_RET
{
	int   nSyncId;
	DWORD dwIpAddress;
	DWORD dwReceivedSize;
	int nRepairCount;
	int abBlockId[1];
}PACKET_QUIZ_BLOCK_RET, *PPACKET_QUIZ_BLOCK_RET;

typedef struct tagPACKET_QUIZ_BLOCK_CLOSE
{
	DWORD dwIpAddress;
}PACKET_QUIZ_BLOCK_CLOSE, *PPACKET_QUIZ_BLOCK_CLOSE;

typedef struct tagPACKET_QUIZ_CONNECT {
	DWORD dwIpAddress;
	BYTE abMac[6];
}PACKET_QUIZ_CONNECT, *PPACKET_QUIZ_CONNECT;

typedef struct tagPACKET_QUIZ_CONNECT_RET {
	DWORD dwIpAddress;
}PACKET_QUIZ_CONNECT_RET, *PPACKET_QUIZ_CONNECT_RET;

typedef struct tagPACKET_QUIZ_PING
{
	DWORD dwIpAddress;
	BYTE abMac[6];
	DWORD cbSize;
	BYTE abBuffer[1];
}PACKET_QUIZ_PING, *PPACKET_QUIZ_PING;

typedef struct tagPACKET_QUIZ_PING_RET
{
	DWORD dwIpAddress;
	DWORD cbSize;
	BYTE abBuffer[1];
}PACKET_QUIZ_PING_RET, *PPACKET_QUIZ_PING_RET;

typedef struct tagPACKET_QUIZDATA {
	int nBlockId;
	DWORD cbSize;
	BYTE  abBuffer[1];
}PACKET_QUIZDATA, *PPACKET_QUIZDATA;

typedef struct tagPACKET_QUIZDATA_RET {
	DWORD dwIpAddress;
	BYTE abMac[6];
	DWORD dwReceivedSize;
	int nRepairCount;
	int abBlockId[1];
}PACKET_QUIZDATA_RET, *PPACKET_QUIZDATA_RET;

typedef struct tagPACKET_QUIZDATA_CONFIRM {
	DWORD dwIpAddress;
}PACKET_QUIZDATA_CONFIRM, *PPACKET_QUIZDATA_CONFIRM;

typedef struct tagPACKET_COLLECT_MISSION
{
	DWORD dwTaskId;
	DWORD dwTeacherIp;
	PMEMBERINFO pMemberInfo;
	DWORD dwFlags;
	TCHAR szFileType[MAX_PATH];
	TCHAR szFilePath[MAX_PATH];
}PACKET_COLLECT_MISSION, *PPACKET_COLLECT_MISSION;

typedef struct tagPACKET_COLLECT_STATE
{
	PMEMBERINFO pMemberInfo;
	DWORD dwTaskId;
	DWORD dwState;
	DWORD dwFileCount;
	LONGLONG llFileSize;
	DWORD dwErrorCode;
}PACKET_COLLECT_STATE, *PPACKET_COLLECT_STATE;

typedef struct tagPACKET_COLLECT_EVENT
{
	PMEMBERINFO pMemberInfo;
	DWORD dwErrorCode;
	DWORD cbSize;
	BYTE  abParams[1];
}PACKET_COLLECT_EVENT, *PPACKET_COLLECT_EVENT;

typedef struct tagPACKET_CONFCALL_ANNOUNCE
{
	BOOL  bTeacher;
	DWORD dwChannelMask;
	DWORD dwIpAddress;
	DWORD dwState;
	DWORD dwCallingIp;
	DWORD dwTalkingIp;
	TCHAR szName[32];
}PACKET_CONFCALL_ANNOUNCE, *PPACKET_CONFCALL_ANNOUNCE;

typedef struct tagPACKET_CONFCALL_RESPOND
{
	DWORD dwIpAddress;
	BOOL bAccept;
}PACKET_CONFCALL_RESPOND, *PPACKET_CONFCALL_RESPOND;

typedef struct tagPACKET_CONFCALL_EXIT
{
	DWORD dwIpAddress;
}PACKET_CONFCALL_EXIT, *PPACKET_CONFCALL_EXIT;

// Block
#define BLOCK_DATA_PACKET			0x00000001
#define BLOCK_SYNC_PACKET			0x00000002
#define BLOCK_STOP_PACKET			0x00000004

typedef struct tagPACKET_BLOCK {
	DWORD dwFlags;
	DWORD dwSenderIp;
	DWORD dwReceiverIp;
	DWORD dwBlockType;
	DWORD dwBlockSize;
	int   nBlockId;
	int   nSyncId;
	DWORD cbSize;
	BYTE  abBuffer[1];
}PACKET_BLOCK, *PPACKET_BLOCK;

typedef struct tagPACKET_BLOCK_RET
{
	int   nSyncId;
	DWORD dwIpAddress;
	DWORD dwReceivedSize;
	int nRepairCount;
	int abBlockId[1];
}PACKET_BLOCK_RET, *PPACKET_BLOCK_RET;

typedef struct tagPACKET_BLOCK_CLOSE
{
	DWORD dwIpAddress;
}PACKET_BLOCK_CLOSE, *PPACKET_BLOCK_CLOSE;

// Group Chat
#define MAX_TEXT_LENGTH					32

typedef struct tagASKGROUPID {
	DWORD dwIpAddress;
}ASKGROUPID, *PASKGROUPID;

typedef struct tagANSWERGROUPID {
	DWORD dwGroupId;
	TCHAR szGroupName[MAX_TEXT_LENGTH + 1];
	TCHAR szName[MAX_TEXT_LENGTH + 1];
	DWORD dwState;
	WORD wVoicePort;
	UINT64 nCommand;//Record command
}ANSWERGROUPID, *PANSWERGROUPID;

typedef tagANSWERGROUPID	EVENT_ASKGROUPID_RET, *PEVENT_ASKGROUPID_RET;

typedef struct tagJOINGROUPCHAT {
	DWORD dwIpAddress;
	BYTE  abMacAddress[6];
	DWORD dwGroupId;
	DWORD dwState;
	TCHAR szName[MAX_TEXT_LENGTH + 1];
	UINT64 nCommand;//Report command
}JOINGROUPCHAT,*PJOINGROUPCHAT;

typedef tagJOINGROUPCHAT EVENT_JOINGROUPCHAT, *PEVENT_JOINGROUPCHAT;

typedef struct tagANNOUNCEGROUPCHAT {
	DWORD dwIpAddress;
	BYTE  abMacAddress[6];
	DWORD dwGroupId;
	DWORD dwState;
	TCHAR szName[MAX_TEXT_LENGTH + 1];
	UINT64 nCommand;//Report command
}ANNOUNCEGROUPCHAT, *PANNOUNCEGROUPCHAT;

typedef tagANNOUNCEGROUPCHAT EVENT_ANNOUNCEGROUPCHAT, *PEVENT_ANNOUNCEGROUPCHAT;

typedef struct tagLEAVEGROUPCHAT {
	DWORD dwGroupId;
	DWORD dwIpAddress;
	TCHAR szName[MAX_TEXT_LENGTH + 1];
}LEAVEGROUPCHAT, *PLEAVEGROUPCHAT;

typedef tagLEAVEGROUPCHAT EVENT_LEAVEGROUPCHAT, *PEVENT_LEAVEGROUPCHAT;


typedef struct tagGROUPMEMBERNAME {
	DWORD dwIpAddress;
	DWORD dwGroupId;
	TCHAR szName[MAX_TEXT_LENGTH + 1];
}GROUPMEMBERNAME, *PGROUPMEMBERNAME;

typedef tagGROUPMEMBERNAME EVENT_MEMBERNAMECHANGED, *PEVENT_MEMBERNAMECHANGED;

typedef struct tagGROUPMEMBERSTATE {
	DWORD dwIpAddress;
	DWORD dwGroupId;
	DWORD dwState;
}GROUPMEMBERSTATE, *PGROUPMEMBERSTATE;

typedef tagGROUPMEMBERSTATE EVENT_MEMBERSTATECHANGED, *PEVENT_MEMBERSTATECHANGED;

typedef struct tagGROUPMEMBERCOMMAND {
	DWORD dwIpAddress;
	DWORD dwGroupId;
	UINT64 nCommand;
}GROUPMEMBERCOMMAND, *PGROUPMEMBERCOMMAND;

typedef tagGROUPMEMBERCOMMAND EVENT_MEMBERCOMMANDCHANGED, *PEVENT_MEMBERCOMMANDCHANGED;

typedef struct tagMESSAGE_HEADER
{
	DWORD dwMagic;
	DWORD dwLength;
}MESSAGE_HEADER, *PMESSAGE_HEADER;

typedef struct tagPACKET_MESSAGE
{
	DWORD dwGroupId;
	DWORD dwSenderIp;
	
	DWORD cbMessage;
	BYTE  abMessage[1];
}PACKET_MESSAGE, *PPACKET_MESSAGE;

typedef struct tagSEND_MESSAGE {
	DWORD	cbMessage;
	BYTE	abMessage[1];
}SEND_MESSAGE, *PSEND_MESSAGE;

typedef struct tagPOLICY_COMMAND
{
	DWORD dwTeacherIp;
	DWORD dwAppPolicy;
	DWORD dwWebPolicy;
	DWORD dwUsbPolicy;
	DWORD dwPrintPolicy;
}POLICY_COMMAND, *PPOLICY_COMMAND;

typedef struct tagPOLICY_STATE
{
	DWORD dwStudentIp;
	DWORD dwAppPolicy;
	DWORD dwWebPolicy;
	DWORD dwUsbPolicy;
	DWORD dwPrintPolicy;
}POLICY_STATE, *PPOLICY_STATE;

// Application
typedef struct tagPROCESSID_LIST
{
	DWORD dwStudentIp;
	int nCount;
	DWORD pdwProcessId[1];
}PROCESSID_LIST, *PPROCESSID_LIST;

typedef struct tagPROCESSINFO_REQ
{
	DWORD dwProcessId;
}PROCESSINFO_REQ, *PPROCESSINFO_REQ;

typedef struct tagPROCESSINFO_RET
{
	DWORD dwStudentIp;
	DWORD dwProcessId;
	DWORD cbInfoSize;
	BYTE  abInfo[1];
}PROCESSINFO_RET, *PPROCESSINFO_RET;

// Web Site
typedef struct tagWEBID_LIST
{
	DWORD dwStudentIp;
	int nCount;
	DWORD pdwWebId[1];
}WEBID_LIST, *PWEBID_LIST;

typedef struct tagWEBINFO_REQ
{
	DWORD dwWebId;
}WEBINFO_REQ, *PWEBINFO_REQ;

typedef struct tagWEBINFO_RET
{
	DWORD dwStudentIp;
	DWORD dwWebId;
	DWORD cbInfoSize;
	BYTE  abInfo[1];
}WEBINFO_RET, *PWEBINFO_RET;

// Proximity
typedef struct tagPROXIMITY_INVITE
{
	DWORD dwInviterIp;
	DWORD dwGroupLeaderIp;
	DWORD dwGroupId;
}PROXIMITY_INVITE, *PPROXIMITY_INVITE;

#pragma pack(pop)

#endif//__PACKETDEF_H__