#pragma once

class LIBVOICE_API CVoiceCapture : public IVUMeterModule::IMeterCallback
{
public:
	CVoiceCapture(void);
	~CVoiceCapture(void);

public:
	BOOL Create(GUID& guidDevice, 
				BOOL bEnableProcessor,
				int nFormat,
				HWND hWndMeter,
				DWORD dwMsgMeter);

	void Destroy();

	BOOL Run();
	void Stop();

	void AddOutputSink(IVoIPSink * pSink);
	void RemoveOutputSink(IVoIPSink * pSink);

	// value : db
	void SetGain(int nGain) {
		if (m_pVolumeControl)
			m_pVolumeControl->SetGain(nGain);
	}
	int GetGain() {
		if (m_pVolumeControl)
			return m_pVolumeControl->GetGain();
		return 0;
	}

	void SetGlobalGain(int nGain) {
		if (m_pVolumeControl)
			m_pVolumeControl->SetGlobalGain(nGain);
	}
	int	 GetGlobalGain() {
		if (m_pVolumeControl)
			return m_pVolumeControl->GetGlobalGain();
		return 0;
	}

	void SetMute(BOOL bMute) {
		if (m_pVolumeControl)
			m_pVolumeControl->SetMute(bMute);
	}
	int GetMute() {
		if (m_pVolumeControl)
			return m_pVolumeControl->IsMuted();
		return FALSE;
	}
	void SetMeterType(WORD wType) {
		m_wMeterType = wType;
	}

	const GUID* GetDeviceId() {
		return &m_guidDevice;
	}

	BOOL IsValid() {
		return m_bCreate;
	}

	BOOL IsRunning() {
		return m_nReference > 0;
	}

	LONG GetMeterLevel(); 

private:
	virtual void OnTotalVU(short sVU, LPVOID pvParam);
	virtual void OnChannelVU(const short * psVU, LPVOID pvParam) {}

private:
	IDSoundCaptureModule*	m_pDSoundCapture;
	IVoiceProcessorModule*	m_pVoiceProcessor;
	ISinkConnectorModule*	m_pSinkConnector;
	IVUMeterModule*			m_pMeterModule;
	IVolumeControlModule*	m_pVolumeControl;

	VOICE_FORMAT_INFO	m_vfi;

	GUID	m_guidDevice;
	BOOL	m_bEnableProcessor;
	BOOL	m_bCreate;
	HWND	m_hWndMeter;
	DWORD	m_dwMsgMeter;
	WORD	m_wMeterType;
	int		m_nReference;

	LONG	m_lLevel;
};
