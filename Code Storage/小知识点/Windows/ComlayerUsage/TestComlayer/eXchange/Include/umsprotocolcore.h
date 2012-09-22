#ifndef __UMS_PROTOCOL_CORE_H__
#define __UMS_PROTOCOL_CORE_H__

#pragma warning(push)
#pragma warning(disable: 4200)

#define UMS_VERSION				0x00010000
#define UMS_DATA_BLOCK_SIZE		65536

#define UMS_OPEN				'OPEN'
#define UMS_OPENEX				'OPEX'
#define UMS_CLOSE				'CLOS'
#define UMS_SET_POINTER			'SETP'
#define UMS_RETURN				'RETR'
#define UMS_DATA				'DATA'
#define UMS_TERMINATE_STREAM	'MTRS'
#define UMS_OPEN_CHANNEL		'OPCH'
#define UMS_CLOSE_CHANNEL		'CLCH'
#define UMS_RTP_PACKET			'RTPP'
#define UMS_REQUEST_KEY_FRAME	'URKF'

#define UMS_MSG_PACKET			'MSGP'

typedef struct _UMS_PACKET
{
	DWORD	dwVersion;
	DWORD	dwMagic;
	DWORD	dwLength;
} UMS_PACKET, *PUMS_PACKET;

#define UMS_ERROR_OK				0
#define UMS_ERROR_OPEN				1
#define UMS_ERROR_AUTH				2
#define UMS_ERROR_CHAPTER			3

#define UMS_LEN_PROGRAM_NAME		64
#define UMS_LEN_PROGRAM_DIRECTOR	64
#define UMS_LEN_PROGRAM_PUBLISH		64
#define UMS_LEN_PROGRAM_ACTOR		128
#define UMS_LEN_PROGRAM_INFO		1024
#define UMS_LEN_CHAPTER_NAME		64
#define UMS_LEN_USER_NAME			64

#define UMS_MAX_CHAPTERS			32

typedef struct _UMS_OPENEX_REQ
{
	LONG		lSessionID;
	LONG		lClientID;
	LONG		lProgramID;
	LONG		lChapterIndex;
	LONGLONG	llPos;
} UMS_OPENEX_REQ, *PUMS_OPENEX_REQ;

typedef struct tagUMS_CHAPTER_ITEM
{
	LONG	lID;
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
	DWORD	dwMediaType;
	TCHAR	szName[UMS_LEN_CHAPTER_NAME + 1];
} UMS_CHAPTER_ITEM, *PUMS_CHAPTER_ITEM;

typedef struct _UMS_PROGRAM_INFO
{
	TCHAR	szName[UMS_LEN_PROGRAM_NAME + 1];
	TCHAR	szDirector[UMS_LEN_PROGRAM_DIRECTOR + 1];
	TCHAR	szPublish[UMS_LEN_PROGRAM_PUBLISH + 1];
	TCHAR	szActor[UMS_LEN_PROGRAM_ACTOR + 1];
	TCHAR	szInfo[UMS_LEN_PROGRAM_INFO + 1];
	TCHAR	szAddUser[UMS_LEN_USER_NAME + 1];
	
	DWORD	dwDuration;
	DWORD	dwChapterCount;
	// UMS_CHAPTER_ITEM achapterItems[dwChapterCount];
} UMS_PROGRAM_INFO, *PUMS_PROGRAM_INFO;

typedef struct _UMS_OPEN_REQ
{
	LONGLONG	llPos;
	TCHAR		szFileName[0];
} UMS_OPEN_REQ, *PUMS_OPEN_REQ;

typedef struct _UMS_OPEN_RET
{
	DWORD	dwErrorCode;
	DWORD	dwLengthLow;
	DWORD	dwLengthHigh;
} UMS_OPEN_RET, *PUMS_OPEN_RET;

typedef struct _UMS_OPENEX_RET : public UMS_OPEN_RET
{
	UMS_PROGRAM_INFO programInfo;
} UMS_OPENEX_RET, *PUMS_OPENEX_RET;

typedef struct _UMS_SET_POINTER_REQ
{
	DWORD	dwPosLow;
	DWORD	dwPosHigh;
	DWORD	dwEndLow;
	DWORD	dwEndHigh;
} UMS_SET_POINTER_REQ, *PUMS_SET_POINTER_REQ;

typedef struct _UMS_DATA_RET
{
	DWORD	dwPosLow;
	DWORD	dwPosHigh;
} UMS_DATA_RET, *PUMS_DATA_RET;

#define MAX_OPEN_CHANNELS	10

typedef struct _UMS_OPEN_CHANNEL_REQ	
{
	DWORD	cSSRCs;
	// SSRC List
	// DWORD adwSSRCs[cSSRCs];
} UMS_OPEN_CHANNEL_REQ, *PUMS_OPEN_CHANNEL_REQ;

#pragma warning(pop)

#endif //__UMS_PROTOCOL_CORE_H__