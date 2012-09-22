#pragma once

class LIBVOICE_API CRTPUDPVoiceSender
{
public:
	CRTPUDPVoiceSender(void);
	~CRTPUDPVoiceSender(void);

public:
	BOOL Create(
		int nFormat,
		DWORD dwSSRC,
		DWORD dwDestIpAddress, 
		WORD wPort, 
		DWORD dwLocalIpAddress, 
		WORD wLocalPort);

	void Destroy();

	BOOL ResetSender(DWORD dwDestIpAddress, 
				WORD wPort, 
				DWORD dwLocalIpAddress, 
				WORD wLocalPort);

	IVoIPSink * GetInputSink();

	int GetEncoderSamplesPerFrame();
	int GetEncoderMaxOutputFrameSize();

protected:
	IAudioEncoderModule *	m_pAudioEncoder;
	IRTPPacketizerModule *	m_pRTPPacketizer;
	IRTPUDPSenderModule *	m_pRTPUDPSender;

	VOICE_FORMAT_INFO	m_vfi;

	DWORD	m_dwSSRC;
	DWORD	m_dwDestIpAddress;
	WORD	m_wPort;
	DWORD	m_dwLocalIpAddress;
	WORD	m_wLocalPort;

	BOOL	m_bCreate;
};