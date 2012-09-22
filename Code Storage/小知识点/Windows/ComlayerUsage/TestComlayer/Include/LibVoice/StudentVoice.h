#pragma once

#include <LibVoice/LibVoice.h>

class CStudentVoice : public IStudentVoice
{
public:
	CStudentVoice(void);
	~CStudentVoice(void);

public:
	BOOL Create(GUID& guidIn, 
				GUID& guidOut, 
				int nForamt,
				BOOL bEnableProcessor,
				DWORD dwSSRC,
				HWND hWndMeter = NULL,
				DWORD dwMsgMeter = 0);

	virtual void Destroy();

	// send side
	virtual BOOL BeginSendGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);
	virtual void StopSendGlobalVoice();
	virtual BOOL IsSendingGlobalVoice();
	virtual BOOL ResetSendGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);

	// Repeat
	virtual BOOL BeginSendRepeat(IVoIPSink * pSink);
	virtual void StopSendRepeat();
	virtual BOOL IsSendingRepeat();

	virtual void SetMainInputMute(BOOL bMute);
	virtual BOOL IsMainInputMuted();

	virtual BOOL AddMixerItem(IVoIPSource * pSource);
	virtual void RemoveMixerItem(IVoIPSource * pSource);

	// Receive side
	virtual BOOL BeginRecvGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);
	virtual void StopRecvGlobalVoice();
	virtual BOOL IsReceivingGlobalVoice();
	virtual BOOL ResetRecvGlobalVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);

	virtual int GetSamplesPerSec();
	virtual int GetChannels();
	virtual int GetSamplesPerFrame();
	virtual int GetMaxEncodedFrame();

	virtual void SetEnableStreamCallback(IEnableStreamCallback* pCallback) {
		if (m_pVoiceRender) {
			m_pVoiceRender->SetEnableStreamCallback(pCallback);
		}
	}
	virtual void EnableStream(DWORD dwSSRC, BOOL bEnable) {
		if (m_pVoiceRender) {
			m_pVoiceRender->EnableStream(dwSSRC, bEnable);
		}
	}
	virtual void EnableAllStreams(BOOL bEnable) {
		if (m_pVoiceRender) {
			m_pVoiceRender->EnableAllStreams(bEnable);
		}
	}

	// 0 -- 100
	virtual void SetMicVolume(int nVolume);
	virtual int GetMicVolume();
	virtual void SetRenderVolume(int nVolume);
	virtual int GetRenderVolume();

	virtual void SetRenderGlobalVolume(int nVolume);
	virtual int GetRenderGlobalVolume();

	// get the renderer's voice
	virtual void AddRendererOutputSink(IVoIPSink* pSink);
	virtual void RemoveRendererOutputSink(IVoIPSink* pSink);

private:
	CVoiceCapture*					m_pVoiceCapture;
	IVoIPSink*						m_pSinkRepeat;
	CRTPUDPVoiceSender*				m_pVoiceSendVoice;
	CRTPUDPVoiceRender*				m_pVoiceRender;
	IVolumeControlModule*			m_pVolumeControl;
	IPushedPassiveWaveMixerModule*	m_pPushedPassiveWaveMixer;

	CSourceMap m_mapSources;

	BOOL	m_bCreate;
	BOOL	m_bSendVoice;
	BOOL	m_bSendRepeat;
	BOOL	m_bRecvVoice;

	VOICE_FORMAT_INFO m_vfi;

	int		m_nFormat;
	GUID	m_guidIn;
	GUID	m_guidOut;
	BOOL	m_bEnableProcessor;
	DWORD	m_dwSSRC;
	HWND	m_hWndMeter;
	DWORD	m_dwMsgMeter;

	int		m_nMicVolume;
};

BOOL IStudentVoice::Create(GUID& guidIn, 
			GUID& guidOut, 
			int nFormat,
			BOOL bEnableProcessor,
			DWORD dwSSRC,
			HWND hWndMeter,
			DWORD dwMsgMeter,
			IStudentVoice *& pInstance)
{
	CStudentVoice* pStudentVoice = new CStudentVoice();
	if (NULL == pStudentVoice)
		return FALSE;

	if (!pStudentVoice->Create(guidIn, 
			guidOut, 
			nFormat,
			bEnableProcessor,
			dwSSRC,
			hWndMeter,
			dwMsgMeter))
	{
		pStudentVoice->Destroy();
		return FALSE;
	}

	pInstance = pStudentVoice;
	return TRUE;
}