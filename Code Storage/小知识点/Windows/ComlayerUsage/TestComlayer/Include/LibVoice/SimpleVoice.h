#pragma once

#include <LibVoice/LibVoice.h>

class LIBVOICE_API CSimpleVoice
{
public:
	CSimpleVoice(void);
	~CSimpleVoice(void);

public:
	BOOL InitVoice(
		GUID& guidIn, 
		GUID& guidOut, 
		int nFormat, 
		BOOL bEnableProcessor,
		DWORD dwSSRC,
		HWND hWndMeter = NULL,
		DWORD dwMsgMeter = 0);

	void DoneVoice();
	BOOL IsInited();

	// send side
	virtual BOOL BeginSendVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);
	virtual void StopSendVoice();
	virtual BOOL IsSendingVoice();
	virtual BOOL ResetSendVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);

	// Receive side
	virtual BOOL BeginRecvVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);
	virtual void StopRecvVoice();
	virtual BOOL IsReceivingVoice();
	virtual BOOL ResetRecvVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);

	virtual int GetSamplesPerSec();
	virtual int GetChannels();
	virtual int GetSamplesPerFrame();
	virtual int GetMaxEncodedFrame();

	virtual void SetEnableStreamCallback(IEnableStreamCallback* pCallback) {
		m_pEnableStreamCallback = pCallback;
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

	virtual void SetMicGlobalVolume(int nVolume);
	virtual int GetMicGlobalVolume();

	virtual void SetRenderGlobalVolume(int nVolume);
	virtual int GetRenderGlobalVolume();

	// get the renderer's voice
	virtual void AddRendererOutputSink(IVoIPSink* pSink);
	virtual void RemoveRendererOutputSink(IVoIPSink* pSink);

public:
	CVoiceCapture* GetVoiceCapture() {return m_pVoiceCapture;}
	CRTPUDPVoiceSender* GetVoiceSender() {return m_pVoiceSendVoice;}
	CRTPUDPVoiceRender*	GetVoiceRender() {return m_pVoiceRender;}


private:
	CVoiceCapture*					m_pVoiceCapture;
	CRTPUDPVoiceSender*				m_pVoiceSendVoice;
	CRTPUDPVoiceRender*				m_pVoiceRender;
	IEnableStreamCallback*			m_pEnableStreamCallback;

	BOOL	m_bCreate;
	BOOL	m_bSendVoice;
	BOOL	m_bRecvVoice;

	GUID	m_guidIn;
	GUID	m_guidOut;

	VOICE_FORMAT_INFO m_vfi;

	int		m_nFormat;
	BOOL	m_bEnableProcessor;
	DWORD	m_dwSSRC;
	HWND	m_hWndMeter;
	DWORD	m_dwMsgMeter;
};



//////////////////////////////////////////////
// CVoiceSender 
class LIBVOICE_API CVoiceSender
{
public:
	CVoiceSender();
	virtual ~CVoiceSender();

	BOOL InitVoice(
		GUID& guidIn, 
		int nFormat, 
		BOOL bEnableProcessor,
		DWORD dwSSRC,
		HWND hWndMeter = NULL,
		DWORD dwMsgMeter = 0);

	void DoneVoice();
	BOOL IsInited();

	// send side
	BOOL BeginSendVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);
	void StopSendVoice();
	BOOL IsSendingVoice();
	BOOL ResetSendVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);

	int GetSamplesPerSec();
	int GetChannels();
	int GetSamplesPerFrame();
	int GetMaxEncodedFrame();

	// 0 -- 100
	void SetVolume(int nVolume);
	int GetVolume();

	void SetGlobalVolume(int nVolume);
	int GetGlobalVolume();

public:
	CVoiceCapture* GetVoiceCapture() {
		return m_pVoiceCapture;
	}
	CRTPUDPVoiceSender* GetVoiceSender() {
		return m_pVoiceSendVoice;
	}

private:
	CVoiceCapture*					m_pVoiceCapture;
	CRTPUDPVoiceSender*				m_pVoiceSendVoice;

	BOOL							m_bCreate;
	BOOL							m_bSendVoice;

	GUID							m_guidIn;

	VOICE_FORMAT_INFO				m_vfi;

	int								m_nFormat;
	BOOL							m_bEnableProcessor;
	DWORD							m_dwSSRC;
	HWND							m_hWndMeter;
	DWORD							m_dwMsgMeter;
};


//////////////////////////////////////////////
// CVoiceReceiver
class LIBVOICE_API CVoiceReceiver
{
public:
	CVoiceReceiver();
	virtual ~CVoiceReceiver();

	BOOL InitVoice(
		GUID& guidOut, 
		int nFormat, 
		DWORD dwSSRC,
		HWND hWndMeter = NULL,
		DWORD dwMsgMeter = 0);

	void DoneVoice();
	BOOL IsInited();

	// Receive side
	BOOL BeginRecvVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);
	void StopRecvVoice();
	BOOL IsReceivingVoice();
	BOOL ResetRecvVoice(DWORD dwDestIpAddress, WORD wPort, DWORD dwLocalIpAddress, WORD wLocalPort);

	int GetSamplesPerSec();
	int GetChannels();
	int GetSamplesPerFrame();
	int GetMaxEncodedFrame();

	void SetEnableStreamCallback(IEnableStreamCallback* pCallback) {
		m_pEnableStreamCallback = pCallback;
		if (m_pVoiceRender) {
			m_pVoiceRender->SetEnableStreamCallback(pCallback);
		}
	}
	void EnableStream(DWORD dwSSRC, BOOL bEnable) {
		if (m_pVoiceRender) {
			m_pVoiceRender->EnableStream(dwSSRC, bEnable);
		}
	}
	void EnableAllStreams(BOOL bEnable) {
		if (m_pVoiceRender) {
			m_pVoiceRender->EnableAllStreams(bEnable);
		}
	}

	// 0 -- 100
	void SetVolume(int nVolume);
	int GetVolume();

	void SetGlobalVolume(int nVolume);
	int GetGlobalVolume();

	// get the renderer's voice
	void AddRendererOutputSink(IVoIPSink* pSink);
	void RemoveRendererOutputSink(IVoIPSink* pSink);

public:
	CRTPUDPVoiceRender*	GetVoiceRender() {
		return m_pVoiceRender;
	}

private:
	CRTPUDPVoiceRender*				m_pVoiceRender;
	IEnableStreamCallback*			m_pEnableStreamCallback;

	BOOL							m_bCreate;
	BOOL							m_bRecvVoice;

	GUID							m_guidOut;

	VOICE_FORMAT_INFO				m_vfi;

	int								m_nFormat;
	DWORD							m_dwSSRC;
	HWND							m_hWndMeter;
	DWORD							m_dwMsgMeter;
};