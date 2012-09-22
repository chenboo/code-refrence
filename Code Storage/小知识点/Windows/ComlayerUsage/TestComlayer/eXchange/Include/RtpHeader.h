#ifndef __RTPHDR_H__
#define __RTPHDR_H__

#define MAX_RTP_PACKET_SIZE		2300
#define DEF_RTP_PACKET_SIZE		1440

#define PT_MPEG_AUDIO		14
#define PT_MPEG_VIDEO		32
#define PT_DVD_SPU			100
#define PT_DVD_AC3			101
#define PT_GENERIC_VIDEO	102
#define PT_GENERIC_AUDIO	103
#define PT_GENERIC_VIDEO2	104

typedef struct _RTP_HEADER {
    WORD	NumCSRC:4;                                      
    WORD	fExtHeader:1;                                   
    WORD	fPadding:1;                                     
    WORD	Version:2;                                      
    WORD	PayloadType:7;
    WORD	fMarker:1;
    WORD	wSequenceNum;
    DWORD	dwTimestamp;
    DWORD	dwSSRC;
} RTP_HEADER, *PRTP_HEADER;

inline void NetworkToHostOrder(PRTP_HEADER pRtpHeader)
{
	pRtpHeader->wSequenceNum = ntohs(pRtpHeader->wSequenceNum);
	pRtpHeader->dwTimestamp = ntohl(pRtpHeader->dwTimestamp);
	pRtpHeader->dwSSRC = ntohl(pRtpHeader->dwSSRC);
}

inline void HostToNetworkOrder(PRTP_HEADER pRtpHeader)
{
	pRtpHeader->wSequenceNum = htons(pRtpHeader->wSequenceNum);
	pRtpHeader->dwTimestamp = htonl(pRtpHeader->dwTimestamp);
	pRtpHeader->dwSSRC = htonl(pRtpHeader->dwSSRC);
}

inline REFERENCE_TIME RtpToDxmTime(DWORD dwTimestamp)
{
	return (REFERENCE_TIME)((double)dwTimestamp * 1000.0 / 9.0);
}

typedef struct _RTP_GENERIC_VIDEO_HEADER {
	BYTE			bFrameHeader;
	BYTE			bKeyFrame;
	WORD			wReserved;
	DWORD			dwReserved;
	DWORD			cbFormatInfo;
	BYTE			abFormatInfo[1];
} RTP_GENERIC_VIDEO_HEADER, *PRTP_GENERIC_VIDEO_HEADER;

typedef struct _RTP_GENERIC_AUDIO_HEADER {
	BYTE			bFrameHeader;
	BYTE			bReserved;
	WORD			wFormatTag;
	DWORD			dwFrameDuration;
	DWORD			cbFormatInfo;
	BYTE			abFormatInfo[1];
} RTP_GENERIC_AUDIO_HEADER, *PRTP_GENERIC_AUDIO_HEADER;

typedef struct _RTP_SUB_PICTURE_HEADER {
	BYTE			bFrameHeader;
	BYTE			bReserved;
} RTP_SUB_PICTURE_HEADER, *PRTP_SUB_PICTURE_HEADER;

#endif //__RTPHDR_H__