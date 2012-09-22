#pragma once

class CExternalVoice : public IExternalVoice
{
public:
	CExternalVoice(void);
	~CExternalVoice(void);

public:
	BOOL Create(GUID& guidIn, 
				GUID& guidOut,
				int nFormat,
				BOOL bEnableProcessor,
				HWND hWndMeter,
				DWORD dwMsgMeter,
				WORD wMeterType);

	virtual void Destroy();

	virtual BOOL BeginMonitor(GUID& guidOut);
	virtual void StopMonitor();
	virtual BOOL IsMonitoring();
	virtual LONG GetVoiceMeter();

	virtual IVoIPSource * GetOutputSource();

private:
	CVoiceCapture*				m_pVoiceCapture;
	IAudioJitterBufferModule*	m_pJBufferMixer;
	IAudioJitterBufferModule*	m_pJBufferRender;
	IDSoundRendererModule*		m_pDSoundRenderer;

	VOICE_FORMAT_INFO	m_vfi;

	GUID	m_guidIn;
	GUID	m_guidOut;
	BOOL	m_bEnableProcessor;
	BOOL	m_bCreate;
	BOOL    m_bMonitoring;

	HWND	m_hWndMeter;
	DWORD	m_dwMsgMeter;
};

BOOL IExternalVoice::Create(
		GUID& guidIn, 
		GUID& guidOut,
		int nFormat,
		BOOL bEnableProcessor,
		HWND hWndMeter,
		DWORD dwMsgMeter,
		WORD wMeterType,
		IExternalVoice *& pInstance)
{
	CExternalVoice* pExternalVoice = new CExternalVoice();
	if (NULL == pExternalVoice)
		return FALSE;

	if (!pExternalVoice->Create(
		guidIn, 
		guidOut,
		nFormat,
		bEnableProcessor,
		hWndMeter,
		dwMsgMeter,
		wMeterType))
	{
		pExternalVoice->Destroy();
		return FALSE;
	}

	pInstance = (IExternalVoice *)pExternalVoice;
	return TRUE;
}
