#ifndef __LIBVOICE_H__
#define __LIBVOICE_H__

#ifdef LIBVOICE_EXPORTS
#define LIBVOICE_API __declspec(dllexport)
#else
#define LIBVOICE_API __declspec(dllimport)
#endif

#include <limits.h>
#include <LibVoIP/Modules.h>
#include <CodeSnippets/ADCCommon.h>
using namespace VoIP;

#define KEEP_ALIVE_DUR					5000
#define SAMPLES_PER_FRAME_ADPCM			240
#define SAMPLES_PER_FRAME_ADPCM_LOW		60
#define PT_VOIP							10
#define VOICE_MTU						1024
#define VOICE_TTL						10

class IVoiceFrameInfo
{
public:
	enum VOICE_CODEC_TYPE
	{
		VCT_NONE = 0,
		VCT_ADPCM,
		VCT_AAC,
		VCT_MP3,
		VCT_SPEEX,
	};

public:
	static LIBVOICE_API BOOL GetFrameInfo(
		DWORD dwEncodeType, 
		int cSamplesPerSec, 
		int cChannels, 
		int* pcSamplesPerFrame, 
		int* pcMaxEncodedFrame);

	static LIBVOICE_API BOOL GetFrameInfo(
		DWORD dwEncodeType, 
		int cSamplesPerSec, 
		int cChannels, 
		int nBitRateBps, 
		int nSamplesPerFrame, 
		int* pcSamplesPerFrame, 
		int* pcMaxEncodedFrame);
};

enum VOICE_FORMAT_TYPE {
	VFT_NONE = 0,
	VFT_ADPCM,
	VFT_ADPCM_LOW,
	VFT_MP3,
	VFT_SPEEX,
};

typedef struct tagVOICE_FORMAT_INFO 
{
	DWORD dwEncodeType;
	DWORD dwSamplesPerSec;
	WORD  wChannels;
	int	  nCaptureBufferFrames;
	int	  nLocalJitterBufferFrames;
	int   nJitterBufferFrames;
	int   nDSoundBufferFrames;
	int   cSamplesPerFrame;
	int   cbMaxEncodedFrame;
	int   nBitRateBps;
}VOICE_FORMAT_INFO, *PVOICE_FORMAT_INFO;

class IVoiceFormatInfo
{
public:
	static LIBVOICE_API BOOL GetVoiceFormatInfo(int nFormat, PVOICE_FORMAT_INFO pVoiceFormatInfo);
};

class IEnableStreamCallback
{
public:
	virtual BOOL EnableStreamProc(DWORD dwSSRC) = NULL;
};

class ITeacherVoice
{
public:
	static LIBVOICE_API BOOL Create(
				GUID& guidIn, 
				GUID& guidOut, 
				int nFormat,
				BOOL bEnableProcessor,
				DWORD dwSSRC,
				HWND hWndMeter,
				DWORD dwMsgMeter,
				ITeacherVoice*& pInstance);
public:
	virtual void Destroy() = NULL;

	// Side Tone
	virtual BOOL BeginMonitor() = NULL; 
	virtual void StopMonitor() = NULL;
	virtual BOOL IsMonitoring() = NULL;

	virtual BOOL BeginSendGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort) = NULL;
	virtual void StopSendGlobalVoice() = NULL;
	virtual BOOL IsSendingGlobalVoice() = NULL;
	virtual BOOL ResetSendGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort) = NULL;

	virtual void SetMainInputMute(BOOL bMute) = NULL;
	virtual BOOL IsMainInputMuted() = NULL;

	virtual BOOL AddMixerItem(IVoIPSource * pSource) = NULL;
	virtual void RemoveMixerItem(IVoIPSource * pSource) = NULL;

	// Receive side
	virtual BOOL BeginRecvGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort) = NULL;
	virtual void StopRecvGlobalVoice() = NULL;
	virtual BOOL IsRecvingGlobalVoice() = NULL;
	virtual BOOL ResetRecvGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort) = NULL;

	virtual int GetSamplesPerSec() = NULL;
	virtual int GetChannels() = NULL;
	virtual int GetSamplesPerFrame() = NULL;
	virtual int GetMaxEncodedFrame() = NULL;

	virtual void SetEnableStreamCallback(IEnableStreamCallback* pCallback) = NULL;
	virtual void EnableStream(DWORD dwSSRC, BOOL bEnable) = NULL;
	virtual void EnableAllStreams(BOOL bEnable) = NULL;

	virtual int GetVoiceCaptureGain() = NULL;
	virtual void SetVoiceCaptureGain(int nGain) = NULL;

	virtual LONG GetVoiceMeter() = NULL;
};

class IStudentVoice
{
public:
	static LIBVOICE_API BOOL Create(
				GUID& guidIn, 
				GUID& guidOut, 
				int nFormat,
				BOOL bEnableProcessor,
				DWORD dwSSRC,
				HWND hWndMeter,
				DWORD dwMsgMeter,
				IStudentVoice *& pInstance);

public:
	virtual void Destroy() = NULL;

	virtual BOOL BeginSendGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort) = NULL;
	virtual void StopSendGlobalVoice() = NULL;
	virtual BOOL IsSendingGlobalVoice() = NULL;
	virtual BOOL ResetSendGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort) = NULL;

	virtual BOOL BeginSendRepeat(IVoIPSink * pSink) = NULL;
	virtual void StopSendRepeat() = NULL;
	virtual BOOL IsSendingRepeat() = NULL;

	virtual void SetMainInputMute(BOOL bMute) = NULL;
	virtual BOOL IsMainInputMuted() = NULL;

	virtual BOOL AddMixerItem(IVoIPSource * pSource) = NULL;
	virtual void RemoveMixerItem(IVoIPSource * pSource) = NULL;

	// Receive side
	virtual BOOL BeginRecvGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort) = NULL;
	virtual void StopRecvGlobalVoice() = NULL;
	virtual BOOL IsReceivingGlobalVoice() = NULL;
	virtual BOOL ResetRecvGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort) = NULL;

	virtual int GetSamplesPerSec() = NULL;
	virtual int GetChannels() = NULL;
	virtual int GetSamplesPerFrame() = NULL;
	virtual int GetMaxEncodedFrame() = NULL;

	virtual void SetEnableStreamCallback(IEnableStreamCallback* pCallback) = NULL;
	virtual void EnableStream(DWORD dwSSRC, BOOL bEnable) = NULL;
	virtual void EnableAllStreams(BOOL bEnable) = NULL;

	virtual void SetMicVolume(int nVolume) = NULL;
	virtual int GetMicVolume() = NULL;
	virtual void SetRenderVolume(int nVolume) = NULL;
	virtual int GetRenderVolume() = NULL;

	virtual void SetRenderGlobalVolume(int nVolume) = NULL;
	virtual int GetRenderGlobalVolume() = NULL;

	// get the renderer's voice
	virtual void AddRendererOutputSink(IVoIPSink* pSink) = NULL;
	virtual void RemoveRendererOutputSink(IVoIPSink* pSink) = NULL;
};

class IExternalVoice
{
public:
	static LIBVOICE_API BOOL Create(
			GUID& guidIn, 
			GUID& guidOut,
			int nFormat,
			BOOL bEnableProcessor,
			HWND hWndMeter,
			DWORD dwMsgMeter,
			WORD wMeterType,
			IExternalVoice *& pInstance);

public:
	virtual void Destroy() = NULL;

	virtual BOOL BeginMonitor(GUID& guidOut) = NULL;
	virtual void StopMonitor() = NULL;
	virtual BOOL IsMonitoring() = NULL;
	virtual LONG GetVoiceMeter() = NULL;

	virtual IVoIPSource * GetOutputSource() = NULL;
};

#include "VoiceCapture.h"
#include "RTPUDPVoiceSender.h"
#include "RTPUDPVoiceRender.h"
#include "SimpleVoice.h"

#endif //__LIBVOICE_H__