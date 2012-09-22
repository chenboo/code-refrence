#pragma once

class LIBVOICE_API CRTPStream : public IVUMeterModule::IMeterCallback
{
public:
	CRTPStream(DWORD dwSSRC, DWORD dwIpAddress, WORD wPort, HWND hWndMeter, DWORD dwMsgMeter) :
		m_pMixerItem(NULL),
		m_pAssembler(NULL),
		m_pDecoder(NULL),
		m_pBuffer(NULL),
		m_pSourceConnector(NULL),
		m_pVolumeMeter(NULL),
		m_dwSSRC(dwSSRC),
		m_dwIpAddress(dwIpAddress),
		m_wPort(wPort),
		m_hWndMeter(hWndMeter),
		m_dwMsgMeter(dwMsgMeter)
	{
	}

	virtual ~CRTPStream() {
		Destroy();
	}

public:
	BOOL Create(DWORD dwEncodeType, 
				int cSamplesPerSec, 
				int cChannels, 
				int cSamplesPerFrame, 
				int cbMaxEncodedFrame, 
				int nJitterBufferFrames)
	{
		do
		{
			// Create
			if (VOIP_FAILED(IRTPAssemblerModule::Create(PT_VOIP, m_dwSSRC, cbMaxEncodedFrame, FALSE, m_pAssembler)))
				break;

			if (VOIP_FAILED(IAudioJitterBufferModule::Create(cChannels, cSamplesPerFrame, nJitterBufferFrames * 2, m_pBuffer)))
				break;

			m_pBuffer->SetDuration(nJitterBufferFrames);

			if (VOIP_FAILED(ISourceConnectorModule::Create(m_pSourceConnector)))
				break;

			int cPeriod = cSamplesPerSec / 20;
			cPeriod = (cPeriod + cSamplesPerFrame - 1) / cSamplesPerFrame * cSamplesPerFrame;
			if (VOIP_FAILED(IVUMeterModule::Create(cChannels, 
												  cSamplesPerFrame, 
												  cPeriod,
												  cPeriod,
												  IVUMeterModule::UNIT_DB,
												  this,
												  this,
												  m_pVolumeMeter)))
			{
				break;
			}

			if (IVoiceFrameInfo::VCT_NONE == dwEncodeType) {
			} else if (IVoiceFrameInfo::VCT_ADPCM == dwEncodeType) {
				if (VOIP_FAILED(IADPCMDecoderModule::Create(cSamplesPerSec, cChannels, cSamplesPerFrame, m_pDecoder)))
					break;
			} else if (IVoiceFrameInfo::VCT_AAC == dwEncodeType) { 
				if (VOIP_FAILED(IAACDecoderModule::Create(cSamplesPerSec, cChannels, m_pDecoder)))
					break;
			} else if (IVoiceFrameInfo::VCT_MP3 == dwEncodeType) { 
				if (VOIP_FAILED(IMP3DecoderModule::Create(cSamplesPerSec, cChannels, m_pDecoder)))
					break;
			} else if (IVoiceFrameInfo::VCT_SPEEX == dwEncodeType) { 
				if (VOIP_FAILED(ISpeexDecoderModule::Create(cSamplesPerSec, cChannels, m_pDecoder)))
					break;
			} else {
				break;
			}

			// connect
			if (m_pDecoder) {
				m_pAssembler->SetOutputSink(m_pDecoder->GetInputSink());
				m_pDecoder->SetOutputSink(m_pBuffer->GetInputSink());

			} else
			{
				m_pAssembler->SetOutputSink(m_pBuffer->GetInputSink());
			}

			m_pSourceConnector->SetInputSource(m_pBuffer->GetOutputSource());
			m_pSourceConnector->AddOutputSink(m_pVolumeMeter->GetInputSink());

			return TRUE;

		} while (TRUE);

		VOIP_DESTROY_MODULE(m_pAssembler);
		VOIP_DESTROY_MODULE(m_pDecoder);
		VOIP_DESTROY_MODULE(m_pBuffer);
		VOIP_DESTROY_MODULE(m_pSourceConnector);
		VOIP_DESTROY_MODULE(m_pVolumeMeter);

		return FALSE;
	}

	void Destroy() {
		VOIP_DESTROY_MODULE(m_pAssembler);
		VOIP_DESTROY_MODULE(m_pDecoder);
		VOIP_DESTROY_MODULE(m_pBuffer);
		VOIP_DESTROY_MODULE(m_pSourceConnector);
		VOIP_DESTROY_MODULE(m_pVolumeMeter);
	}

public:
	IVoIPSink * GetInputSink() {
		return m_pAssembler->GetInputSink();
	}

	IVoIPSource * GetOutputSource() {
		return m_pSourceConnector->GetOutputSource();
	}

	IMixerItem * GetMixerItem() {
		return m_pMixerItem;
	}

	void SetMixerItem(IMixerItem * pItem) {
		m_pMixerItem = pItem;
	}

private:
	virtual void OnTotalVU(short sVU, LPVOID pvParam) {
		//_TRACE(_T("OnTotalVU of Render, sVU = %d, SSRC = %08X \n"), sVU, m_dwSSRC);
/*
		if (m_hWndMeter) {
			WPARAM wParam = MAKELONG((WORD)sVU, ELCAMT_VOICE);
			::PostMessage(m_hWndMeter, m_dwMsgMeter, wParam, m_dwSSRC);
		}
*/
	}
	virtual void OnChannelVU(const short * psVU, LPVOID pvParam) {}

protected:
	IMixerItem				 *m_pMixerItem;			
	IRTPAssemblerModule		 *m_pAssembler;
	IAudioDecoderModule		 *m_pDecoder;
	IAudioJitterBufferModule *m_pBuffer;
	ISourceConnectorModule   *m_pSourceConnector;
	IVUMeterModule			 *m_pVolumeMeter;

	DWORD					  m_dwSSRC;
	DWORD					  m_dwIpAddress;
	WORD					  m_wPort;

	HWND					  m_hWndMeter;
	DWORD					  m_dwMsgMeter;
};

class LIBVOICE_API CRTPStreamManager : public IRTPDemultiplexerModule::IStatusCallback
{
public:
	CRTPStreamManager() :
		m_pMixer(NULL),
		m_pCallback(NULL)
	{
		InitializeCriticalSection(&m_cs);
	}

	virtual ~CRTPStreamManager() {
		Destroy();
		DeleteCriticalSection(&m_cs);
	}

public:
	BOOL Create(DWORD dwSelfSSRC, 
				DWORD dwEncodeType, 
				int cSamplesPerSec, 
				int cChannels, 
				int cSamplesPerFrame, 
				int cbMaxEncodedFrame, 
				int nJitterBufferFrames,
				HWND hWndMeter,
				DWORD dwMsgMeter) {
		m_dwSelfSSRC = dwSelfSSRC;

		m_dwEncodeType = dwEncodeType;
		m_cSamplesPerSec = cSamplesPerSec;
		m_cChannels = cChannels;
		m_cSamplesPerFrame = cSamplesPerFrame;
		m_cbMaxEncodedFrame = cbMaxEncodedFrame;
		m_nJitterBufferFrames = nJitterBufferFrames;

		m_hWndMeter = hWndMeter;
		m_dwMsgMeter = dwMsgMeter;

		if (VOIP_FAILED(IPassiveWaveMixerModule::Create(cChannels, cSamplesPerFrame, m_pMixer)))
			return FALSE;
		
		return TRUE;
	}

	void Destroy() {
		if (m_pMixer) {
			m_pMixer->Destroy();
			m_pMixer = NULL;
		}

		for (std::map<DWORD, CRTPStream *>::iterator i = m_mapStreams.begin();
			i != m_mapStreams.end(); i++) {
			delete i->second;
		}
		m_mapStreams.clear();
	}

public:
	IVoIPSource * GetMixedOutputSource() {
		return m_pMixer->GetOutputSource();
	}

	void SetEnableStreamCallback(IEnableStreamCallback* pCallback) {
		m_pCallback = pCallback;
	}

	void EnableStream(DWORD dwSSRC, BOOL bEnable) {
		CAutoSectionLock lock(&m_cs);

		std::map<DWORD, CRTPStream *>::iterator i = m_mapStreams.find(dwSSRC);
		if (i == m_mapStreams.end())
			return;

		CRTPStream *pStream = i->second;

		if (bEnable) {
			if (NULL == pStream->GetMixerItem()) {
				IMixerItem *pMixerItem;
				m_pMixer->AddMixerItem(pStream->GetOutputSource(), pMixerItem);
				pStream->SetMixerItem(pMixerItem);
			}
		} else
		{
			if (pStream->GetMixerItem()) {
				m_pMixer->RemoveMixerItem(pStream->GetMixerItem());
				pStream->SetMixerItem(NULL);
			}
		}
	}

	void EnableAllStreams(BOOL bEnable) {
		CAutoSectionLock lock(&m_cs);
		
		std::map<DWORD, CRTPStream *>::iterator i = m_mapStreams.begin();
		while (i != m_mapStreams.end()) {
			CRTPStream *pStream = i->second;

			if (bEnable) {
				if (NULL == pStream->GetMixerItem()) {
					IMixerItem *pMixerItem;
					m_pMixer->AddMixerItem(pStream->GetOutputSource(), pMixerItem);
					pStream->SetMixerItem(pMixerItem);
				}
			} else
			{
				if (pStream->GetMixerItem()) {
					m_pMixer->RemoveMixerItem(pStream->GetMixerItem());
					pStream->SetMixerItem(NULL);
				}
			}

			i ++;
		}
	}

protected:
	IVoIPSink * OnNewStream(DWORD dwSSRC, DWORD dwIpAddress, WORD wPort) {

		CAutoSectionLock lock(&m_cs);

		if (m_dwSelfSSRC == dwSSRC)
			return NULL;

		std::map<DWORD, CRTPStream *>::iterator i = m_mapStreams.find(dwSSRC);
		if (i != m_mapStreams.end())
			return NULL;

		CRTPStream *pStream = new CRTPStream(dwSSRC, dwIpAddress, wPort, m_hWndMeter, m_dwMsgMeter);
		if (NULL == pStream)
			return NULL;

		if (!pStream->Create(m_dwEncodeType, m_cSamplesPerSec, m_cChannels, m_cSamplesPerFrame, m_cbMaxEncodedFrame, m_nJitterBufferFrames) )
		{
			delete pStream;
			return NULL;
		}

		if (m_pCallback && (!m_pCallback->EnableStreamProc(dwSSRC))) {
			// do nothing
		} else
		{
			IMixerItem *pMixerItem;
			m_pMixer->AddMixerItem(pStream->GetOutputSource(), pMixerItem);
			pStream->SetMixerItem(pMixerItem);
		}

		m_mapStreams[dwSSRC] = pStream;
		return pStream->GetInputSink();
	}

	void OnDeleteStream(DWORD dwSSRC, IVoIPSink *pSink) {
		CAutoSectionLock lock(&m_cs);

		std::map<DWORD, CRTPStream *>::iterator i = m_mapStreams.find(dwSSRC);
		if (i == m_mapStreams.end())
			return;

		CRTPStream *pStream = i->second;
		m_pMixer->RemoveMixerItem(pStream->GetMixerItem());
		m_mapStreams.erase(i);
		delete pStream;
	}

protected:
	IPassiveWaveMixerModule *		m_pMixer;

	typedef std::map <DWORD, CRTPStream *> CStreamMap;
	CStreamMap m_mapStreams;

	IEnableStreamCallback*			m_pCallback;
	CRITICAL_SECTION				m_cs;

	DWORD							m_dwEncodeType;
	int								m_cChannels;
	int								m_cSamplesPerFrame;
	int								m_cSamplesPerSec;
	int								m_cbMaxEncodedFrame;
	int								m_nJitterBufferFrames;
	DWORD							m_dwSelfSSRC;

	HWND							m_hWndMeter;
	DWORD							m_dwMsgMeter;
};

class LIBVOICE_API CRTPUDPVoiceRender
{
public:
	CRTPUDPVoiceRender(void);
	~CRTPUDPVoiceRender(void);

public:
	BOOL Create(DWORD dwSSRC,
				DWORD dwDestIpAddress,
				WORD wDestPort, 
				DWORD dwLocalIpAddress,
				WORD wLocalPort,
				const GUID& guidDevice, 
				int nFormat,
				HWND hWndMeter,
				DWORD dwMsgMeter);

	void Destroy();

	BOOL Run();
	void Stop();

	BOOL ResetReceiver(DWORD dwDestIpAddress,
				WORD wDestPort, 
				DWORD dwLocalIpAddress,
				WORD wLocalPort);

	void AddOutputSink(IVoIPSink * pSink);
	void RemoveOutputSink(IVoIPSink * pSink);

	void SetEnableStreamCallback(IEnableStreamCallback* pCallback) {
		m_streamMgr.SetEnableStreamCallback(pCallback);
	}
	void EnableStream(DWORD dwSSRC, BOOL bEnable){
		m_streamMgr.EnableStream(dwSSRC, bEnable);
	}
	void EnableAllStreams(BOOL bEnable) {
		m_streamMgr.EnableAllStreams(bEnable);
	}

	// nVolume range: 0 -- 100
	void SetVolume(int nVolume);
	int GetVolume();

	// Set voice global volume when student uses XVA Adapter
	void SetGlobalVolume(int nVolume);
	int GetGlobalVolume();
	
private:
	IRTPUDPReceiverModule*		m_pRTPUDPReceiver;
	IRTPDemultiplexerModule*	m_pRTPDemultiplexer;
	CRTPStreamManager			m_streamMgr;
	ISourceConnectorModule*		m_pSourceConnector;
	IDSoundRendererModule*		m_pDSoundRenderer;

//	IWaveFileWriterModule*			m_WaveFileWriter;

	VOICE_FORMAT_INFO	m_vfi;

	DWORD	m_dwSSRC;
	DWORD	m_dwDestIpAddress;
	WORD	m_wDestPort;
	DWORD	m_dwLocalIpAddress;
	WORD	m_wLocalPort;
	GUID	m_guidDevice;

	BOOL	m_bCreate;
	BOOL	m_bRun;

	int		m_nRendererVolume;
	int		m_nGlobalVolume;
};