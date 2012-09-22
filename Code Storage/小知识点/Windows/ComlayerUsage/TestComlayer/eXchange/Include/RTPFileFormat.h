#ifndef __RTP_FILE_FORMAT_H__
#define __RTP_FILE_FORMAT_H__

#define RTP_FILE_MAGIC			'RTPF'
#define RTP_FILE_PACKET			'RTPP'
#define RTP_FILE_INDEX			'RTPI'
#define RTP_FILE_PRIVATE_DATA	'RTPD'

///////////////////////////////////////////////////////////////////////////////
// RTP_FILE_HEADER
// RTP_PACKET_HEADER
// Packet Data
// RTP_PACKET_HEADER
// Packet Data
// RTP_PACKET_HEADER
// Packet Data
// RTP_PACKET_HEADER
// Packet Data
// ...

#define CLF_LAYOUT_MASTER_SLAVE		0
#define CLF_LAYOUT_AVERAGE			1
#define CLF_LAYOUT_UP_DOWN			2

typedef struct _RTP_FILE_HEADER
{
	DWORD dwMagic;
	DWORD dwStartTime;
	DWORD dwStopTime;
	DWORD dwIndexPosHigh;
	DWORD dwIndexPosLow;
	/*
	DWORD dwLayout;
	WORD  wWidth;
	WORD  wHeight;
	*/
} RTP_FILE_HEADER, *PRTP_FILE_HEADER;

typedef struct _RTP_CHANNEL_HEADER
{
	DWORD dwMagic;
	DWORD dwLayoutFlags;
	DWORD dwSSRC;
	WORD  wOrgWidth;
	WORD  wOrgHeight;
	DWORD dwLayoutIndex;
} RTP_CHANNEL_HEADER, *PRTP_CHANNEL_HEADER;

typedef struct _RTP_PACKET_HEADER
{
	DWORD dwMagic;
	DWORD dwPacketTime;
	WORD  cbPacket;
	WORD  wReserved;
} RTP_PACKET_HEADER, *PRTP_PACKET_HEADER;

typedef struct _RTP_INDEX_NODE
{
	DWORD dwPacketTime;
	DWORD dwPacketPosHigh;
	DWORD dwPacketPosLow;
} RTP_INDEX_NODE, *PRTP_INDEX_NODE;

typedef struct _RTP_INDEX_HEADER
{
	DWORD dwMagic;
	DWORD dwCount;
	// RTP_INDEX_NODE aNodes[dwCount];
} RTP_INDEX_HEADER, *PRTP_INDEX_HEADER;

typedef struct _RTP_PRIVATE_DATA_HEADER
{
	DWORD dwMagic;
	DWORD dwLength;
} RTP_PRIVATE_DATA_HEADER, *PRTP_PRIVATE_DATA_HEADER;

#endif //__RTP_FILE_FORMAT_H__