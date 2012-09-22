#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define CONNECT_NAME_WALLPAPER				_T("DESKTOPPICTURE")
#define CONNECT_NAME_APPPOLICY				_T("GETAPPPOLICY")
#define CONNECT_NAME_WEBPOLICY				_T("GETWEBPOLICY")
#define CONNECT_NAME_GROUPCHAT				_T("GETCHATATTACHMENTS")
#define CONNECT_NAME_FILECOLLECT			_T("FILECOLLECT")
#define CONNECT_NAME_COURSEWARE_LIST		_T("GETCWLIST")
#define CONNECT_NAME_COURSEWARE				_T("GETCOURSEWARE")

#pragma pack(push, 1)

#define FLG_NODESKTOP						0x00000001
#define ELC_MAX_MEMBER_COUNT				0x00000400 //1024
#define ELC_MAX_COMMAND_COUNT				32

typedef struct tagCLIENTID
{
	BYTE abMac[6];
} CLIENTID, *PCLIENTID;

typedef struct tagMEMBERINFO
{
	BOOL  bLoggedIn;
	BOOL  bSignIn;
	BOOL  bGroupLeader;
	UINT64 dwState;
	UINT64 dwCommand;
	DWORD dwReserved;
	DWORD dwIpAddress;
	TCHAR  szLoginName[32];
	TCHAR  szComputerName[32];
	TCHAR  szUserName[32];
	TCHAR  szDefaultName[32];
	//BYTE  abMacAddress[6];
	CLIENTID		id;
	TCHAR  szGroupName[32];

	BYTE* pbThumbnail;
	int   nImage;

	DWORD dwBattery;
	POINT ptThumb;

	// CMPC 2.2 
	POINT			ptGroupThumb;

	DWORD			dwGroupId : 24;
	DWORD			bChecked : 1;
	DWORD			bInDestop : 1;	// student report
	DWORD			bReserved1 : 1;
	DWORD			bReserved2 : 1;
	DWORD			bReserved3 : 1;
	DWORD			bReserved4 : 1;
	DWORD			bReserved5 : 1;
	DWORD			bReserved6 : 1;

	DWORD			cbSize;			// student report
	DWORD			dwVersion;		// student report

	DWORD			dwPlatformId : 8;
	DWORD			dwMajorVersion : 8;
	DWORD			dwMinorVersion : 8;
	DWORD			dwReserved7 : 8;

} MEMBERINFO, *PMEMBERINFO;

enum PLATFORM_TYPE {
	PLATFORM_UNKNOWN = 0,
	PLATFORM_WINDOWS,
	PLATFORM_UBUNTU,
	PLATFORM_ANDROID
};

// Teacher's view type
enum BASEVIEW_TYPE {
	BVT_MONITOR = 1,
	BVT_REPORT,
	BVT_FILESUBMIT,
};

typedef struct tagMEMBERANNOUNCE
{
	BOOL  bActive;
	UINT64 dwState;
	DWORD dwMemberIp;
	DWORD dwReserved;
	BOOL  bBlockMsg;
	DWORD dwBattery;
	DWORD dwActivView; // teacher's active view
} MEMBERANNOUNCE, *PMEMBERANNOUNCE;

// Active Command ID Mask
#define CMD_ACTIVE_MASK						0x00000000FFFFFFFF

// Teacher Command IDs
#define CMD_STOP_ALL						0x0000000000000000
#define CMD_BROADCAST						0x0000000000000001
#define CMD_VOICE_TEACH						0x0000000000000002
#define CMD_SPEAK							0x0000000000000004
#define CMD_MONITOR							0x0000000000000008
#define CMD_CONTROL							0x0000000000000010
#define CMD_BLACK_SCREEN					0x0000000000000020
#define CMD_DEMO							0x0000000000000040
#define CMD_VOICE_TALK						0x0000000000000080
#define CMD_MOVIE							0x0000000000000100
#define CMD_REMOTE_COMMAND					0x0000000000000200
#define CMD_EXTENSION						0x0000000000000400
#define CMD_MESSAGE							0x0000000000000800
#define CMD_RELOGIN							0x0000000000001000
#define CMD_FILE_CAST						0x0000000000002000
#define CMD_REMOTE_CONFIG					0x0000000000004000
#define CMD_LOGGEDIN						0x0000000000008000
#define CMD_REFRESH_SCREEN					0x0000000000010000
#define CMD_GROUP_EDUCATION					0x0000000000020000
#define CMD_GROUP_CHAT						0x0000000000040000
#define CMD_GET_NAMES						0x0000000000080000
#define CMD_REMOTE_INFO						0x0000000000100000
#define CMD_VRECORD							0x0000000000200000
#define CMD_QUIZ							0x0000000000400000
#define CMD_APPFORCE						0x0000000000800000
#define CMD_WEBFORCE						0x0000000001000000
#define CMD_VWEBSITE						0x0000000002000000
#define CMD_QUIZINSTANT						0x0000000004000000
#define CMD_FILE_COLLECT					0x0000000008000000
#define CMD_SHAREBOARD						0x0000000010000000

#define CMD_PARAMETER_MASK					0x00000FFF00000000

#define BROADCAST_PARAMMASK_AUDIO			0x0000000100000000
#define BROADCAST_PARAMMASK_QUALITY			0x0000000200000000
#define BROADCAST_PARAMMASK_FULLSCREEN		0x0000000400000000
#define BROADCAST_PARAMMASK_DISPMODE		0x0000000800000000

#define POLICY_MASK_APP			0x00000001
#define POLICY_MASK_WEB			0x00000002
#define POLICY_MASK_USB			0x00000004
#define POLICY_MASK_PRINT		0x00000008

enum APPPOLICY
{
	APP_UNRESTRICTED = 0,
	APP_BLOCK_BLACKLIST,
	APP_ENABLE_WHITELIST,
	APP_BLOCK_ALL
};

enum WEBPOLICY
{
	WEB_UNRESTRICTED = 0,
	WEB_BLOCK_BLACKLIST,
	WEB_ENABLE_WHITELIST,
	WEB_BLOCK_ALL
};

enum USBPOLICY
{
	USB_UNRESTRICTED = 0,
	USB_READONLY,
	USB_NOEXCUTE,
	USB_BLOCKACCESS
};

enum PRINTPOLICY
{
	PRINT_UNRESTRICTED = 0,
	PRINT_BLOCK_PAGES,
	PRINT_BLOCK_ALL
};

enum STATEICON {
	STATEICON_LOGOUT,
	STATEICON_LOGIN,
	STATEICON_HANDUP,
	STATEICON_BROADCAST,
	STATEICON_VOICE_TEACH,
	STATEICON_MONITOR,
	STATEICON_BLACK_SCREEN,
	STATEICON_CHAT,
	STATEICON_MOVIE,
	STATEICON_EXTEND,
	STATEICON_SPEAK,
	STATEICON_FILE_CAST,
	STATEICON_GROUP_EDUCATION,
	STATEICON_GROUP_CHAT,
	STATEICON_VRECORD,
    STATEICON_QUIZ, 
	STATEICON_APP_FORCE,
	STATEICON_WEB_FORCE,
	STATEICON_VWEBSITE,
	STATEICON_LOWRESOURCE,
	STATEICON_FILE_COLLECT,
	STATEICON_LOGOUT_LEADER,
	STATEICON_LOGIN_LEADER
};

#define CMD_GROUP_MASK			0x000FF00000000000
#define CMD_OPERATION_MASK		0x0007F00000000000
#define CMD_GROUP_ACTIVE		0x0008000000000000

// Passive Command ID Mask
#define CMD_PASSIVE_MASK		0x0FF0000000000000

// Student Command IDs
#define CMD_HANDUP				0x0010000000000000
#define CMD_STOPCONTROL			0x0020000000000000
#define CMD_REFRESHREQUEST		0x0040000000000000
#define CMD_REPORTINFO			0x0080000000000000
#define CMD_REPORTCURTASK		0x0100000000000000
#define CMD_REQUESTHELP			0x0200000000000000
#define CMD_REPORTMISCDATA		0x0300000000000000
#define CMD_LOWRESOURCE			0x0400000000000000
#define CMD_VERSIONDIFF			0x0500000000000000

// Command Action Mask
#define CMD_ACTION_MASK			0x7000000000000000

// Command Actions
#define CMD_START_FUNCTION		0x0000000000000000
#define CMD_STOP_FUNCTION		0x1000000000000000
#define CMD_MODIFY_PARAMS		0x2000000000000000

// Command Type
#define CMD_MODIFY_STATUS		0x8000000000000000

enum MISC_STUDENT_DATA_TYPE
{
	MSDT_CURRENT_APPLICATION = 0,
	MSDT_CURRENT_WEBSITE,
	MSDT_CURRENT_USBDISK,
	MSDT_REPORT_DATA
};

typedef struct tagMISC_DATA
{
	DWORD dwDataType;
	DWORD cbSize;
	BYTE abParams[1];
}MISC_DATA, *PMISC_DATA;

typedef struct tagREPORT_DATA
{
	ULONGLONG llDiskFreeSpace;
	DWORD dwProcessCount;
	int nCPURate;
	ULONGLONG llUsedMemory;
	DWORD dwNetworkActualSpeed;
	DWORD dwNetworkTheorySpeed;
//	TCHAR szWaveOutName[32];
//	TCHAR szWaveInName[32];
}REPORT_DATA, *PREPORT_DATA;

typedef struct tagCOMMANDPACKET
{
	DWORD cbCommand;
	UINT64 dwCommand;
	BYTE  abParams[1];
} COMMANDPACKET, *PCOMMANDPACKET;

typedef struct tagLOGINPARAMS
{
	BOOL bTcpComMode;
	WORD wTcpComPort;
	DWORD dwReserved;
	DWORD dwVersion; // Add by Alone at 7.25.2010 
}LOGINPARAMS, *PLOGINPARAMS;

typedef struct tagBROADCASTPARAMS
{
	BOOL  bFullScreen;
	BOOL  bWithAudio;
	BOOL  bLockInputDevice;
	DWORD dwIpAddressScreen;
	WORD  wPortScreen;
	DWORD dwIpAddressAudio;
	WORD  wPortAudio;
	WORD  wChannels;
	DWORD dwSamplesPerSec;
	WORD  wBitsPerSample;
	BOOL  bSilenceCheck;
	DWORD dwSilence;
	DWORD dwRecvBufferSize;
	DWORD dwIpPassive;
	DWORD dwCompressWay;
	int	  nSendSpeedKbps;
	int	  nPacketSize;
	int  nRepairMode;
	int	 nFrameLimite;
	int	 nCaptureQuality;
	BOOL bTcpComMode;
	DWORD dwServerIp;

	// Performance added paprams
	int  nWindowDurationSeconds;
	int  nRedundancyRate;
	int  nGroupSize;

	// Bounding Window
	int nDisMode;

	DWORD dwResolution;

	TCHAR szName[32];	// The demoed student's name when start "student demo" feature.
						// Ignore when screenbroadcast
} BROADCASTPARAMS, *PBROADCASTPARAMS;

typedef struct tagVOICETEACHPARAMS
{
	BOOL  bShowBlackScreen;
	BOOL  bLockInputDevice;
	DWORD dwIpAddressAudio;
	WORD  wPortAudio;
	WORD  wChannels;
	DWORD dwSamplesPerSec;
	WORD  wBitsPerSample;
	BOOL  bSilenceCheck;
	DWORD dwSilence;
	DWORD dwRecvBufferSize;
	DWORD dwCompressWay;
	int	  nSendSpeedKbps;
	int	  nPacketSize;
} VOICETEACHPARAMS, *PVOICETEACHPARAMS;

typedef struct tagSPEAKPARAMS
{
	DWORD dwIpAddressAudio;
	WORD  wPortAudio;
} SPEAKPARAMS, *PSPEAKPARAMS;

typedef struct tagMONITORPARAMS
{
	BOOL  bWithAudio;
	DWORD dwIpAddressScreen;
	WORD  wPortScreen;
	DWORD dwIpAddressAudio;
	WORD  wPortAudio;
	WORD  wChannels;
	DWORD dwSamplesPerSec;
	WORD  wBitsPerSample;
	BOOL  bSilenceCheck;
	DWORD dwSilence;
	DWORD dwRecvBufferSize;
	DWORD dwCompressWay;
	BOOL bShowMessage;
	int  nMaxSendSpeedKbps;
	int  nMaxPacketSize;
	int  nRepairMode;
	int	 nFrameLimite;
	int	 nCaptureQuality;
	BOOL bTcpComMode;
	DWORD dwServerIp;

	// Performance added params
	int  nWindowDurationSeconds;
	int  nRedundancyRate;
	int  nGroupSize;

	// Show Message params
	int   nKeepSeconds;
	TCHAR szMessage[256];

	BOOL bStudentDemo;
} MONITORPARAMS, *PMONITORPARAMS;

typedef struct tagCONTROLPARAMS
{
	DWORD dwIpAddress;
	WORD  wPort;
	BOOL bShowMessage;
	int   nKeepSeconds;
	TCHAR szMessage[256];
} CONTROLPARAMS, *PCONTROLPARAMS;

typedef struct tagDEMOPARAMS
{
	BOOL  bWithAudio;
	DWORD dwIpAddressScreen;
	WORD  wPortScreen;
	DWORD dwIpAddressAudio;
	WORD  wPortAudio;
} DEMOPARAMS, *PDEMOPARAMS;

typedef struct tagVOICETALKPARAMS
{
	DWORD dwIpAddress;
	WORD  wPort;
	WORD  wChannels;
	DWORD dwSamplesPerSec;
	WORD  wBitsPerSample;
	BOOL  bSilenceCheck;
	DWORD dwSilence;
	DWORD dwRecvBufferSize;
	DWORD dwCompressWay;
} VOICETALKPARAMS, *PVOICETALKPARAMS;

typedef struct tagBLACKSCREENPARAM
{
	BOOL	bLockInputDevice;
	BOOL	bManualCancel;
	int		nCancelSeconds;
	BOOL    bShowUserText;
	COLORREF	crText;
	COLORREF	crBckgnd;
	TCHAR   szText[1];
} BLACKSCREENPARAM, *PBLACKSCREENPARAM;

typedef struct tagFILECASTPARAMS
{
	DWORD dwIpAddressControl;
	WORD  wPortControl;
	DWORD dwIpAddressCast;
	WORD  wPortCast;
	int   nSendSpeedKbps;
	int	  nPacketSize;
} FILECASTPARAMS, *PFILECASTPARAMS;

typedef struct tagVRECORDPARAMS
{
	DWORD dwIpAddressControl;
	DWORD dwPortControl;
	DWORD dwIpAddressCast;
	DWORD dwPortCast;
} VRECORDPARAMS, *PVRECORDPARAMS;

typedef struct tagQUIZPARAMS
{
	DWORD dwIpAddressControl;
	DWORD dwPortControl;
	DWORD dwIpAddressCast;
	DWORD dwPortCast;
	DWORD dwPortAnswer;
	int   nSendSpeedKbps;
	int	  nPacketSize;
}QUIZPARAMS, *PQUIZPARAMS;

typedef struct tagQUIZINSTANTPARAMS
{
	DWORD dwPortCast;
	DWORD dwIpAddressCast;
}QUIZINSTANTPARAMS, *PQUIZINSTANTPARAMS;

typedef struct tagAPPFORCEPARAMS
{
	DWORD dwComAddress;
	DWORD dwComPort;
}APPFORCEPARAMS, *PAPPFORCEPARAMS;

typedef struct tagWEBFORCEPARAMS
{
	DWORD dwComAddress;
	DWORD dwComPort;
}WEBFORCEPARAMS, *PWEBFORCEPARAMS;

typedef struct tagVWEBSITEPARAMS
{
	DWORD dwComAddress;
	DWORD dwComPort;
}VWEBSITEPARAMS, *PVWEBSITEPARAMS;

typedef struct tagPROJECTORPARAMS
{
	DWORD	dwResetType; // only used for Reset
	BOOL	bFullScreen;
	int		nBufferedMillisecond;

	DWORD	dwControlIp;
	WORD	wControlPort;

	// line in
	DWORD	dwAudioIp; 
	WORD	wAudioPort;
	int		nAudioCodeType; // index of AUDIO_CODE_TYPE in LibProjector
	int		nAudioFormat; // index of AUDIO_FORMAT in LibProjector
	int		nAudioCompression; // index of COMPRESSION_LEVEL in LibProjector

	DWORD	dwVideoIp;
	WORD	wVideoPort;

	int		nVideoCX;
	int		nVideoCY;
	float	fVideoFrameRate; 
	int		nVideoCompLevel; // COMPRESSION_LEVEL
	DWORD	dwVideoFOURCC;

	float	fBitrateKbps;
	float	fWindowDurationSeconds;
	float	fRedundancyRate;
	WORD	wMaxPayloadSize; 
	WORD	wNumDataPacketsInGroup;
}PROJECTORPARAMS, *PPROJECTORPARAMS;

#define GET_NAME_WITH_NAME     0x00000001
#define GET_NAME_WITH_CLASS    0x00000002
#define GET_NAME_WITH_NUMBER   0x00000004
#define GET_NAME_WITH_LOCK	   0x00000008

typedef struct tagGETNAMEPARAMS
{
	DWORD dwMask;
	TCHAR szInstructorName[32];
	TCHAR szClassTopicName[32];
}GETNAMEPARAMS, *PGETNAMEPARAMS;

typedef struct tagSHAREBOARDPARAMS
{
	DWORD		dwCmdIpAddress;
	WORD		wCmdPort;

	DWORD		dwDataIpAddress;
	WORD		wDataPort;
	float		fBitrateKbps;
	float		fWindowDurationSeconds;
	float		fRedundancyRate;
	WORD		wMaxPayloadSize; 
	WORD		wNumDataPacketsInGroup;
}SHAREBOARDPARAMS, *PSHAREBOARDPARAMS;

/////////////////////////////////////////
// Remote setting params
typedef struct tagRSCOMMANDPARAMS
{
	DWORD dwCommandType;
	DWORD cbSize;
	BYTE abParam[1];
}RSCOMMANDPARAMS, *PRSCOMMANDPARAMS;

enum RSCOMMANDTYPE
{
	RSCT_SET_WALLPAPER,
	RSCT_SET_DISPLAY,
	RSCT_SET_INTERNET,
	RSCT_SET_THEME,
	RSCT_SET_POWER,
	RSCT_SET_SCREENSAVER,
	RSCT_SET_ADVANCE
};

typedef struct tagRSWALLPAPERPARAMS
{
	DWORD dwWPStyle;
	TCHAR szFilePath[MAX_PATH];
}RSWALLPAPERPARAMS, *PRSWALLPAPERPARAMS;

typedef struct tagRSDISPLAYPARAMS
{
	BOOL bChangeBits;
	BOOL bChangeRes;
	int nBitsIndex;
	int nResIndex;

	int nNoDispCPL;
	int nNoDispAppearancePage;
	int nNoDispSettingsPage;
	int nNoDispScrSavPage;
	int nNodispBackgroundPage;
}RSDISPLAYPARAMS, *PRSDISPLAYPARAMS;

typedef struct tagRSINTERNETPARAMS
{
	BOOL bChangeInetSettings;
	BOOL bProxyEnable;
	BOOL bProxyOverride;
	BOOL bUseSameProxy;
	TCHAR szHttpServer[64];
	UINT uHttpPort;
	TCHAR szSecureServer[64];
	UINT uSecurePort;
	TCHAR szFtpServer[64];
	UINT uFtpPort;
	TCHAR szGopherServer[64];
	UINT uGopherPort;
	TCHAR szSocksServer[64];
	UINT uSocksPort;
}RSINTERNETPARAMS, *PRSINTERNETPARAMS;

typedef struct tagRSTHEMEPARAMS
{
	TCHAR szThemeName[256];
	//BOOL bClassic;
}RSTHEMEPARAMS, *PRSTHEMEPARAMS;

typedef struct tagRSPOWERPARAMS
{
	TCHAR	szSchemeName[32];
	ULONG	uVideoTimeoutAc;
	ULONG	uVideoTimeoutDc;
	ULONG	uIdleTimeoutAc;
	ULONG	uIdleTimeoutDc;
	ULONG	uSpindownTimeoutAc;
	ULONG	uSpindownTimeoutDc;
}RSPOWERPARAMS, *PRSPOWERPARAMS;

typedef struct tagSCREENSAVERPARAMS
{
	BOOL	bProtect;
	DWORD	dwWaitTime;
	TCHAR	szScreensaver[20];
}SCREENSAVERPARAMS, *PSCREENSAVERPARAMS;

typedef struct tagADVANCEPARAMS
{
	BOOL	bChangeNetwork;
	int		nRepairMode;
	int		nOfflineDuration;

	BOOL	bChangeAudioDevice;
	BOOL	bWaveMute;
	BOOL	bMicMute;
	int		nMicVolume;
	int		nWaveVolume;

	BOOL	bChangePassword;
	TCHAR	szPassword[33];

	int		nProtectProcess;
}ADVANCEPARAMS, *PADVANCEPARAMS;

// End Remote setting params
///////////////////////////////////

enum REMOTECOMMANDTYPE
{
	RCT_STOP_FILESUBMIT,
	RCT_SET_NAME_RESULT,
	RCT_CLOSE_APPS,
	RCT_SET_MONITOR_AUDIO,
	RCT_SET_VIDEO_COMMAND,
	RCT_SET_DEFAULTNAME,
	RCT_SEND_BLKSCREEN,
	RCT_REQHELP_ACKNOW,
	RCT_SET_THUMBNAIL_POLICY,
	RCT_SYNC_FILESUBMIT_POLICY,
	RCT_ENABLEAUTOLOCKSTUDENT,
	RCT_SETUNLOKPASSWORD,
	RCT_WAIT,
	RCT_KEYSTROKES,
	RCT_CLOSETOPWINDOW,
	RCT_COMMAND,
	RCT_CAD,
	RCT_LOGIN,
	RCT_CLOSELAST,
	RCT_REBOOT,
	RCT_HALT,
	RCT_EXIT,
	RCT_FORCE = 0x10000000
};

typedef struct tagLOGINBLOCK
{
	TCHAR szUserName[128];
	TCHAR szPassword[128];
	TCHAR szDomain[128];
} LOGINBLOCK, *PLOGINBLOCK;

typedef struct tagCOMMANDBLOCK
{
	BOOL  bAutoSearch;
	TCHAR  szCommandLine[256];
	//char szParameters[256]; char --> wchar  256 --> 160 , By tomlie 2007.08.05
	TCHAR szParameters[160];
	int	  nDisplayStyle;
	BOOL  bActive;
	DWORD dwCommandId;
} COMMANDBLOCK, *PCOMMANDBLOCK;

typedef struct tagSHUTDOWNPARAM
{
	DWORD dwCommandType;
	DWORD dwWaitSeconds;
	BOOL  bActive;
	DWORD dwCommandId;
	TCHAR szShutdownMsg[256];
} SHUTDOWNPARAM, *PSHUTDOWNPARAM;

#define MAX_KEY_CONTENTS		512

typedef struct tagKEYSTROKESBLOCK
{
	DWORD dwCommandType;
	DWORD dwLength;
	TCHAR szContents[512];
} KEYSTROKESBLOCK, *PKEYSTROKESBLOCK;

typedef struct tagREMOTECOMMANDPARAMS
{
	DWORD dwCommandType;
	COMMANDBLOCK command;
} REMOTECOMMANDPARAMS, *PREMOTECOMMANDPARAMS;

typedef struct tagMONITORAUDIOPARAMS
{
	DWORD dwCommandType;
	BOOL bStart;
	DWORD dwIpAddressAudio;
	WORD  wPortAudio;
	WORD  wChannels;
	DWORD dwSamplesPerSec;
	WORD  wBitsPerSample;
	BOOL  bSilenceCheck;
	DWORD dwSilence;
	DWORD dwRecvBufferSize;
	DWORD dwCompressWay;
}MONITORAUDIOPARAMS, *PMONITORAUDIOPARAMS;

typedef struct tagMESSAGEPARAMS
{
	int nKeepSeconds;
	// modify by tomlie 2007.08.06
	// char szBuffer[1024];
	TCHAR  szBuffer[448];
	// end 
} MESSAGEPARAMS, *PMESSAGEPARAMS;

#define REQHELP_MESSAGE       0x00000000
#define REQHELP_CANCEL		  0x00000001
#define REQHELP_ACKNOW		  0x00000002	
typedef struct tagREQHELPPARAMS
{
	DWORD dwMagic;
	TCHAR  szName[32];
	TCHAR  abMessage[1];
}REQHELPPARAMS, *PREQHELPPARAMS;

typedef struct tagGROUPDESTPARAMS
{
	DWORD dwCount;
	DWORD adwDestions[1];
} GROUPDESTPARAMS, *PGROUPDESTPARAMS;

#define CHAT_TYPE_GROUP 	0x00000001
#define CHAT_TYPE_TOPIC		0x00000002
#define CHAT_TYPE_ICALL		0x00000004

typedef struct tagGROUPCHATPARAMS
{
	DWORD	dwChatType;
	DWORD	dwChatIpAddress;
	WORD	wChatPort;
	DWORD	dwShareFileIp;
	WORD	wShareFilePort;
	float	fBitrateKbps;
	float	fWindowDurationSeconds;
	float	fRedundancyRate;
	WORD	wMaxPayloadSize; 
	WORD	wNumDataPacketsInGroup;

	WORD	wYear;
	BYTE	btMonth;
	BYTE	btDay;
	BYTE	btHour;
	BYTE	btMinute;
	BYTE	btSecond;
} GROUPCHATPARAMS, *PGROUPCHATPARAMS;

typedef struct tagREMOTEINFOPARAMS
{
	DWORD dwCommand;
	HWND  hWndApp;
	DWORD dwProcessId;
	BOOL bForce;
} REMOTEINFOPARAMS, *PREMOTEINFOPARAMS;

#define GROUPTYPE_FIXED       0x00000000
#define GROUPTYPE_PART		  0x00000001
#define GROUPTYPE_PROXIMITY	  0x00000002
typedef struct tagGROUPTEACHPARAMS
{
	DWORD		dwComAddress;
	WORD		wComPort;
	DWORD		dwVoiceIp;

	WORD		wGroupType;//GROUPTYPE_FIXED,GROUPTYPE_PART,GROUPTYPE_PROXIMITY
}GROUPTEACHPARAMS, *PGROUPTEACHPARAMS;

typedef struct tagSETNAMEPARAMS
{
	TCHAR  szUserName[32];
	TCHAR  szClass[32];
	TCHAR  szNumber[32];
} SETNAMEPARAMS, *PSETNAMEPARAMS;

typedef struct tagFILESUBMIT_POLICY
{
	DWORD dwCommandType;
	DWORD dwMaxFileSize;
	DWORD dwMaxFileNums;
}FILESUBMIT_POLICY, *PFILESUBMIT_POLICY;

typedef struct tagTHUMBNAIL_POLICY
{
	DWORD dwCommandType;
	DWORD dwRefreshSeconds;
}THUMBNAIL_POLICY, *PTHUMBNAIL_POLICY;

#define VOD_DISABLE		0
#define VOD_ENABLE		1
#define VOD_SETPARAMS	2

typedef struct tagVODPARAMS
{
	int nPolicy;
	DWORD dwAddress;
	int nPort;
}VODPARAMS, *PVODPARAMS;

#define MAX_WEB_KEYSIZE		512
#define MAX_WEB_URLSIZE		512
#define MAX_WEB_NAMESIZE	512

typedef struct tagWEBSITEITEM {
	DWORD	dwWebId;
	TCHAR	szURL[MAX_WEB_URLSIZE];
	TCHAR   szName[MAX_WEB_NAMESIZE];
} WEBSITEITEM, *PWEBSITEITEM;

#pragma pack(pop)

inline DWORD GetCommandIndex(UINT64 dwCommand)
{
	dwCommand &= CMD_ACTIVE_MASK;

	DWORD i = 0;
	UINT64 dwOne = 1;
	UINT64 dwMask = 1 << i;

	while (dwMask <= CMD_ACTIVE_MASK) {
		if (dwCommand & dwMask)
			return i + 1;

		i++, 
		dwMask = dwOne << i;
	}
	
	return 0;
}

inline int GetStateIconIndex(PMEMBERINFO pMemberInfo)
{
	if (!pMemberInfo->bLoggedIn)
		return STATEICON_LOGOUT;
	else if (pMemberInfo->dwState & CMD_HANDUP) 
		return STATEICON_HANDUP;
	else if (pMemberInfo->dwState & CMD_LOWRESOURCE)
		return STATEICON_LOWRESOURCE;
	else if (pMemberInfo->dwState & CMD_BLACK_SCREEN)
		return STATEICON_BLACK_SCREEN;
	else if (pMemberInfo->dwState & CMD_MONITOR)
		return STATEICON_MONITOR;
	else if (pMemberInfo->dwState & CMD_SPEAK) {
		if (pMemberInfo->dwState & CMD_VOICE_TEACH)
			return STATEICON_CHAT;
		else
			return STATEICON_SPEAK;
	}
	else if (pMemberInfo->dwState & CMD_BROADCAST)
		return STATEICON_BROADCAST;
	else if (pMemberInfo->dwState & CMD_VOICE_TALK)
		return STATEICON_CHAT;
	else if (pMemberInfo->dwState & CMD_VOICE_TEACH)
		return STATEICON_VOICE_TEACH;
	else if (pMemberInfo->dwState & CMD_MOVIE)
		return STATEICON_MOVIE;
	else if (pMemberInfo->dwState & CMD_EXTENSION)
		return STATEICON_EXTEND;
	else if (pMemberInfo->dwState & CMD_FILE_CAST)
		return STATEICON_FILE_CAST;
	else if (pMemberInfo->dwState & CMD_GROUP_EDUCATION)
		return STATEICON_GROUP_EDUCATION;
	else if (pMemberInfo->dwState & CMD_GROUP_CHAT)
		return STATEICON_GROUP_CHAT;
	else if (pMemberInfo->dwState & CMD_VRECORD)
		return STATEICON_VRECORD;
	else if (pMemberInfo->dwState & CMD_QUIZ)
		return STATEICON_QUIZ;
	else if (pMemberInfo->dwState & CMD_QUIZINSTANT)
		return STATEICON_QUIZ;
	else if (pMemberInfo->dwState & CMD_APPFORCE)
		return STATEICON_APP_FORCE;
	else if (pMemberInfo->dwState & CMD_WEBFORCE)
		return STATEICON_WEB_FORCE; 
	else if (pMemberInfo->dwState & CMD_VWEBSITE)
		return STATEICON_VWEBSITE; 
	else if (pMemberInfo->dwState & CMD_FILE_COLLECT)
		return STATEICON_FILE_COLLECT;
	else
		return STATEICON_LOGIN;
}

inline int GetStateIconIndexPro(PMEMBERINFO pMemberInfo)
{
	int nIconIndex = GetStateIconIndex(pMemberInfo);

	switch (nIconIndex)
	{
	case STATEICON_LOGOUT:
		if (pMemberInfo->bGroupLeader)
			nIconIndex = STATEICON_LOGOUT_LEADER;
		break;
	case STATEICON_LOGIN:
		if (pMemberInfo->bGroupLeader)
			nIconIndex = STATEICON_LOGIN_LEADER;
	default:
		break;
	}

	return nIconIndex;
}

#define STR_TEACHER_NAME								_T("TEACHER_NAME")
#define INT_NETWORK_TYPE								_T("NETWORK_TYPE")
#define INT_LOCAL_INTERFACE								_T("LOCAL_INTERFACE")
#define INT_CHANNEL_ID									_T("CHANNEL_ID")
#define INT_TCP_COMMODE									_T("TCP_COM_MODE")
#define INT_TCP_COMPORT									_T("TCP_COM_PORT")
#define STR_RECORD_PATH									_T("RECORD_SAVE_PATH")

#define STR_VOICE_MIC_DEVICE_NAME						_T("VOICE_MIC_DEVICE_NAME")
#define INT_VOICE_MIC_INPUT								_T("VOICE_MIC_INPUT")	
#define STR_VOICE_MIXER_DEVICE_NAME						_T("VOICE_MIXER_DEVICE_NAME")
#define INT_VOICE_MIXER_INPUT							_T("VOICE_MIXER_INPUT")	
#define STR_VOICE_LINEIN_DEVICE_NAME1					_T("VOICE_MIC_DEVICE_NAME1")
#define INT_VOICE_LINEIN_INPUT1							_T("VOICE_MIC_INPUT1")
#define STR_VOICE_LINEIN_DEVICE_NAME2					_T("VOICE_MIC_DEVICE_NAME2")
#define INT_VOICE_LINEIN_INPUT2							_T("VOICE_MIC_INPUT2")
#define INT_VOICE_COMPRESS_WAY							_T("VOICE_COMPRESS_WAY")

#define INT_MAXSENDSPEEDKBPS							_T("MAX_SEND_SPEAD_KBPS")
#define INT_MAXPACKETSIZE								_T("MAX_PACKET_SIZE")
#define INT_REPAIR_MODE									_T("NETWORK_REPAIR_MODE")

#define INT_BROADCAST_WINDOWDURATIONSECONDS				_T("BROADCAST_WINDOW_DURATION_SECONDS")
#define INT_BROADCAST_REDUNDANCYRATE					_T("BROADCAST_REDUND_ANCYRATE")
#define INT_BROADCAST_GROUPSIZE							_T("BROADCAST_GROUP_SIZE")
#define INT_BROADCAST_CAPTUREPRIORITY					_T("BROADCAST_CAPTURE_PRIORITY")
#define INT_BROADCAST_PICTUREQUALITY					_T("BROADCAST_PICTURE_QUALITY")
#define INT_BROADCAST_FRAMERATE							_T("BROADCAST_FRAME_RATE")
#define INT_BROADCAST_REPAIRMODE						_T("BROADCAST_REPAIR_MODE")
#define INT_BROADCAST_CHECKSUM							_T("BROADCAST_CHECK_SUM")


#define INT_MOVIE_WINDOWDURATIONSECONDS					_T("MOVIE_WINDOW_DURATION_SECONDS")
#define INT_MOVIE_REDUNDANCYRATE						_T("MOVIE_REDUNDANCY_RATE")
#define INT_MOVIE_GROUPSIZE								_T("MOVIE_GROUP_SIZE")
#define INT_MOVIE_BUFFERSIZE							_T("MOVIE_BUFFER_SIZE")

#define INT_OFFLINE_DURATION							_T("NETWORK_OFFLINE_DURATION")

#define INT_BITS_PER_SAMPLE								_T("BITS_PER_SAMPLE")
#define INT_NUM_CHANNELS								_T("NUM_CHANNELS")
#define INT_SAMPLES_PER_SEC								_T("SAMPLES_PER_SEC")
#define INT_SILENCE_CHECK								_T("SILENCE_CHECK")
#define INT_SILENCE										_T("SILENCE")
#define INT_AUDIO_BUFFER_SIZE							_T("AUDIO_BUFFER_SIZE")



#endif //__PROTOCOL_H__
