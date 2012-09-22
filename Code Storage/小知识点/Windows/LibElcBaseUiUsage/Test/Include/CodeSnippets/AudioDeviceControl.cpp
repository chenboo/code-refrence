#include "stdafx.h"
#include "AudioDeviceControl.h"
#include "../ElcGuidUtils.h"

#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys.h>
#include "PolicyConfig.h"

#define AUDIO_RENDER_REGKEY_WINXP	_T("SOFTWARE\\Microsoft\\Multimedia\\Sound Mapper")
#define AUDIO_RENDER_REGKEY_WIN7	_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\MMDevices\\Audio\\Render")

#define SAFE_RELEASE(punk) if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }


/////////////////////////////////////////
// CAudioMixerControl
CAudioMixerControl::CAudioMixerControl()
{
	m_pMixer = NULL;
	m_nInput = -1;
	m_bWinVista = FALSE;
	m_pCallback = NULL;
	m_pvParam = 0;
	m_nId = 0;

	m_bRealMixer = FALSE;
	m_bVirtualMuted = FALSE;
	m_nVirtualVolume = 100;

	memset(&m_guidDevice, 0, sizeof(m_guidDevice));
}

CAudioMixerControl::~CAudioMixerControl()
{
	Destroy();
}

BOOL CAudioMixerControl::CreateInstance(UINT nId, IAudioMixerControlCallback *pCallback, PVOID pvParam)
{
	if (!Create(NULL, 0, _T("AudioMixerControl")) != NULL)
		return FALSE;

	m_nId = nId;
	m_pCallback = pCallback;
	m_pvParam = pvParam;

	OSVERSIONINFO osVerInfo = {0};
	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	GetVersionEx(&osVerInfo);

	if (osVerInfo.dwMajorVersion >= 6)
		m_bWinVista = TRUE;
	else
		m_bWinVista = FALSE;

	return TRUE;
}

void CAudioMixerControl::Destroy()
{
	VOIP_DESTROY_MODULE(m_pMixer);

	if (IsWindow())
		DestroyWindow();

	m_nInput = -1;
	m_bWinVista = FALSE;
	m_pCallback = NULL;
	m_pvParam = NULL;
	m_nId = 0;

	m_bRealMixer = FALSE;
	m_bVirtualMuted = FALSE;
	m_nVirtualVolume = 100;
}

BOOL CAudioMixerControl::OpenDevice(const GUID* pGuid, BOOL bCaptureDevice, int nInput, BOOL bRealMixer)
{
	m_bRealMixer = bRealMixer;

	// virtual mixer
	if (!bRealMixer) {
		VOIP_DESTROY_MODULE(m_pMixer);
		memset(&m_guidDevice, 0, sizeof(m_guidDevice));
		m_nInput = -1;
		m_nVirtualVolume = 100;
		m_bVirtualMuted = FALSE;

		if (m_pCallback)
			m_pCallback->OnVolumeChanged(m_nId, TRUE, GetVolume(), m_bRealMixer, m_pvParam);

		return TRUE;
	}

	// real mixer
	if (m_pMixer && m_guidDevice == *pGuid) {
		if (m_nInput != nInput) {
			m_nInput = nInput;
			if (m_pCallback)
				m_pCallback->OnVolumeChanged(m_nId, TRUE, GetVolume(), m_bRealMixer, m_pvParam);
		}
		return TRUE;
	}

	do {
		VOIP_DESTROY_MODULE(m_pMixer);
		m_guidDevice = *pGuid;
		m_nInput = nInput;
		m_bCaptureDevice = bCaptureDevice;

		if (VOIP_FAILED(IAudioMixerModule::Create(m_guidDevice, bCaptureDevice, m_pMixer)))
			break;
		if (!m_bWinVista 
			&& bCaptureDevice
			&& m_pMixer->GetLineCount() <= nInput)
			break;

		m_pMixer->SetCallback(this, 0);

		if (m_pCallback)
			m_pCallback->OnVolumeChanged(m_nId, TRUE, GetVolume(), m_bRealMixer, m_pvParam);

		return TRUE;

	} while (0);

	VOIP_DESTROY_MODULE(m_pMixer);
	memset(&m_guidDevice, 0, sizeof(m_guidDevice));
	m_nInput = -1;
	if (m_pCallback)
		m_pCallback->OnVolumeChanged(m_nId, FALSE, 0, m_bRealMixer, m_pvParam);
	return FALSE;
}

void CAudioMixerControl::SetVolume(int nVolume)
{
	if (!m_bRealMixer) {
		m_nVirtualVolume = nVolume;
		if (m_pCallback)
			m_pCallback->OnVolumeChanged(m_nId, TRUE, nVolume, m_bRealMixer, m_pvParam);
		return;
	}

	if (!m_pMixer) {
		ASSERT(0);
		return;
	}

	VOLUME_100_TO_65535(nVolume);

	if (m_bWinVista || !m_bCaptureDevice) {
		m_pMixer->SetMasterVolume(nVolume);
	}
	else {
		if (m_pMixer->IsLineHasVolumeControl(m_nInput))
			m_pMixer->SetLineVolume(m_nInput, nVolume);

		if (m_pMixer->IsMasterHasVolumeControl())
			m_pMixer->SetMasterVolume(nVolume);
	}
}

int CAudioMixerControl::GetVolume()
{
	if (!m_bRealMixer)
		return m_nVirtualVolume;
	else if (!m_pMixer) {
		ASSERT(0);
		return 0;
	}

	int nVolume = 0;

	if (m_bWinVista || !m_bCaptureDevice) {
		m_pMixer->GetMasterVolume(nVolume);
	}
	else {
		if (m_pMixer->IsLineHasVolumeControl(m_nInput))
			m_pMixer->GetLineVolume(m_nInput, nVolume);
		else if (m_pMixer->IsMasterHasVolumeControl())
			m_pMixer->GetMasterVolume(nVolume);
	}

	VOLUME_65535_TO_100(nVolume);

	return nVolume;
}

void CAudioMixerControl::SetMute(BOOL bMute)
{
	if (!m_bRealMixer) {
		if (m_pCallback)
			m_pCallback->OnMuteChanged(m_nId, m_bVirtualMuted, m_bRealMixer, m_pvParam);
		m_bVirtualMuted = bMute;
	}
	else if (m_pMixer) {
		if (m_bCaptureDevice) {
			if (m_pMixer->IsLineSingleSelect()) {
				// Not Support SetMute
				// do nothing
				return;
			}
			else {
				m_pMixer->SetLineSelectState(m_nInput, !bMute);
			}
		}
		else {
			m_pMixer->SetMasterMuteState(bMute);
		}
	}
}

BOOL CAudioMixerControl::IsMuted()
{
	if (!m_bRealMixer) {
		return m_bVirtualMuted;
	}
	else if (m_pMixer) {
		if (m_bCaptureDevice) {
			if (m_pMixer->IsLineSingleSelect()) {
				return FALSE;
			}
			else {
				return m_pMixer->GetLineSelectState(m_nInput) == S_OK;
			}
		}
		else {
			return m_pMixer->GetMasterMuteState() == S_OK;
		}
	}
	return FALSE;
}

LRESULT CAudioMixerControl::OnMasterMuteChangeNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pCallback)
		m_pCallback->OnMuteChanged(m_nId, m_pMixer->GetMasterMuteState() == S_OK, m_bRealMixer, m_pvParam);

	return 0;
}

LRESULT CAudioMixerControl::OnMasterVolumeChangeNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int nVolume = 0;

	if (m_bWinVista || !m_bCaptureDevice) {
		m_pMixer->GetMasterVolume(nVolume);
	}
	else {
		if (!m_pMixer->IsLineSingleSelect()) {
			m_pMixer->GetMasterVolume(nVolume);
			if (m_pMixer->IsLineHasVolumeControl(m_nInput))
				m_pMixer->SetLineVolume(m_nInput, nVolume);
		}
		else {
			return 0;
		}
	}

	VOLUME_65535_TO_100(nVolume);

	if (m_pCallback)
		m_pCallback->OnVolumeChanged(m_nId, TRUE, nVolume, m_bRealMixer, m_pvParam);

	return 0;
}

LRESULT CAudioMixerControl::OnLineSelectChangeNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bChanged = FALSE;

	if (m_pMixer->IsLineSingleSelect())
	{
		if (m_pMixer->GetLineSelectState(m_nInput) != S_OK)
			bChanged = TRUE;
	}
	else
	{
		for (int i=0; i < m_pMixer->GetLineCount(); i++)
		{
			VOIP_RESULT hr = m_pMixer->GetLineSelectState(i);
			if ((hr == S_OK && m_nInput != i) ||
				(hr != S_OK && m_nInput == i) )
			{
				bChanged = TRUE;
				break;
			}
		}
	}	

	if (bChanged && m_pCallback) {
		m_pCallback->OnLineSelectChanged(m_nId, (int)wParam, m_pvParam);
	}

	return 0;
}

LRESULT CAudioMixerControl::OnLineVolumeChangeNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_nInput == (int)wParam) {
		int nVolume;
		if (m_pMixer->IsLineHasVolumeControl(m_nInput))
			m_pMixer->GetLineVolume(m_nInput, nVolume);
		else if (m_pMixer->IsMasterHasVolumeControl())
			m_pMixer->GetMasterVolume(nVolume);
		else
			nVolume = 0;

		VOLUME_65535_TO_100(nVolume);

		if (m_pCallback)
			m_pCallback->OnVolumeChanged(m_nId, TRUE, nVolume, m_bRealMixer, m_pvParam);
	}

	return 0;
}

///////////////////////////////////////////////
// CAudioMixerManager

CAudioMixerManager::CAudioMixerManager()
{
	m_pCallback = NULL;
	m_pvParam = NULL;
}

CAudioMixerManager::~CAudioMixerManager()
{
	Destroy();
}

void CAudioMixerManager::SetCallback(IAudioMixerControlCallback *pCallback, PVOID pvParam)
{
	m_pCallback = pCallback;
	m_pvParam = pvParam;

	for (CAudioMixerMap::iterator it = m_mapAudioMixer.begin();
		it != m_mapAudioMixer.end();
		it ++)
	{
		if (it->second) {
			((CAudioMixerControl*)it->second)->SetCallback(pCallback, pvParam);
		}
	}
}

void CAudioMixerManager::Destroy()
{
	for (CAudioMixerMap::iterator it = m_mapAudioMixer.begin();
		it != m_mapAudioMixer.end();
		it ++)
	{
		if (it->second) {
			delete (CAudioMixerControl*)it->second;
		}
	}

	m_mapAudioMixer.clear();

	m_pCallback = NULL;
	m_pvParam = NULL;
}

BOOL CAudioMixerManager::AddMixer(UINT nId, const GUID* pGuid, BOOL bCaptureDevice, int nInput, BOOL bRealMixer)
{
	CAudioMixerControl* pMixer = GetMixer(nId);
	if (pMixer != NULL) {
		return pMixer->OpenDevice(pGuid, bCaptureDevice, nInput, bRealMixer);
	}

	pMixer = new CAudioMixerControl;
	if (!pMixer)
		return FALSE;

	do {
		if (!pMixer->CreateInstance(nId, m_pCallback, m_pvParam))
			break;

		if (!pMixer->OpenDevice(pGuid, bCaptureDevice, nInput, bRealMixer))
			break;

		m_mapAudioMixer.insert(CAudioMixerMap::value_type(nId, pMixer));

		return TRUE;
	} while (0);
	
	delete pMixer;
	return FALSE;
}

void CAudioMixerManager::RemoveMixer(UINT nId)
{
	CAudioMixerMap::iterator it = m_mapAudioMixer.find(nId);
	if (it != m_mapAudioMixer.end()) {
		delete it->second;
		m_mapAudioMixer.erase(it);
	}
}

void CAudioMixerManager::SetVolume(UINT nId, int nVolume)
{
	CAudioMixerControl* pMixer = GetMixer(nId);
	if (pMixer)
		pMixer->SetVolume(nVolume);
}

int CAudioMixerManager::GetVolume(UINT nId)
{
	CAudioMixerControl* pMixer = GetMixer(nId);
	if (pMixer)
		return pMixer->GetVolume();
	return 0;
}

void CAudioMixerManager::SetMute(UINT nId, BOOL bMute)
{
	CAudioMixerControl* pMixer = GetMixer(nId);
	if (pMixer)
		pMixer->SetMute(bMute);
}

BOOL CAudioMixerManager::IsMuted(UINT nId)
{
	CAudioMixerControl* pMixer = GetMixer(nId);
	if (pMixer)
		return pMixer->IsMuted();
	return FALSE;
}

CAudioMixerControl* CAudioMixerManager::GetMixer(UINT nId)
{
	CAudioMixerMap::iterator it = m_mapAudioMixer.find(nId);
	if (it == m_mapAudioMixer.end())
		return NULL;
	return it->second;
}



//////////////////////////////////////////////////////////////////////////////
// CElcAudioDeviceEnumerator
int CElcAudioDeviceEnumerator::Enumerate(BOOL bCapture, BOOL bFilterPrivateDevice)
{
	Release();

	m_bCapture = bCapture;

	BOOL bSuccessful = FALSE;
	if (bCapture) {
		IDSoundCaptureEnumModule* pCaptureEnum = NULL;
		if (VOIP_FAILED(IDSoundCaptureEnumModule::Create(pCaptureEnum)))
			return FALSE;

		do {
			int nCount = pCaptureEnum->GetCount();

			GUID guidNull = GUID_NULL;
			for (int i=1; i < nCount; i++) {

				// filter private devices
				if (bFilterPrivateDevice) {
					LPCTSTR lpszDevicePnpId = pCaptureEnum->GetDevicePnpID(i);
					if (IsXVALoopback(lpszDevicePnpId)
						|| IsXA001AURecord(lpszDevicePnpId)
						|| IsXA001AULoopback(lpszDevicePnpId))
					{
						continue;
					}
				}

				IAudioMixerModule* pInputMixer = NULL;
				if (VOIP_FAILED(IAudioMixerModule::Create(*pCaptureEnum->GetDeviceID(i), TRUE, pInputMixer))) {
					//ASSERT(0);
					continue;
				}

				CElcAudioCaptureItem * pItem = new CElcAudioCaptureItem;

				pItem->m_nType = ELCDT_AUDIO_CAPTURE;
				ElcGuid2String(pCaptureEnum->GetDeviceID(i), pItem->m_strDeviceId);
				pItem->m_strDevicePnpId = pCaptureEnum->GetDevicePnpID(i);
				pItem->m_strDeviceName = pCaptureEnum->GetDeviceName(i);

				int nLineCount = pInputMixer->GetLineCount();
				for (int i=0; i < nLineCount; i++)
					pItem->m_arrInput.push_back(pInputMixer->GetLineName(i));

				VOIP_DESTROY_MODULE(pInputMixer);

				m_arrItem.push_back(pItem);
			}

			bSuccessful = TRUE;
		} while(0);

		VOIP_DESTROY_MODULE(pCaptureEnum);
	}
	else {
		IDSoundRendererEnumModule* pRendererEnum = NULL;
		if (VOIP_FAILED(IDSoundRendererEnumModule::Create(pRendererEnum)))
			return FALSE;

		do {
			int nCount = pRendererEnum->GetCount();
			if (nCount <= 1)
				break;

			GUID guidNull = GUID_NULL;
			for (int i=1; i < nCount; i++) {

				// filter private devices
				if (bFilterPrivateDevice) {
					LPCTSTR lpszDevicePnpId = pRendererEnum->GetDevicePnpID(i);
					if (IsXVAPlayback(lpszDevicePnpId)
						|| IsXA001AUPlayback(lpszDevicePnpId)
						|| IsXA001AUVoice(lpszDevicePnpId))
					{
						continue;
					}
				}

				CElcAudioRendererItem * pItem = new CElcAudioRendererItem;
				pItem->m_nType = ELCDT_AUDIO_RENDERER;
				ElcGuid2String(pRendererEnum->GetDeviceID(i), pItem->m_strDeviceId);
				pItem->m_strDevicePnpId = pRendererEnum->GetDevicePnpID(i);
				pItem->m_strDeviceName = pRendererEnum->GetDeviceName(i);
				m_arrItem.push_back(pItem);
			}
			bSuccessful = TRUE;
		} while(0);

		VOIP_DESTROY_MODULE(pRendererEnum);
	}

	return (int)m_arrItem.size();
}


//////////////////////////////////////////////////////////////////////////////
// CElcVideoDeviceEnumerator
int CElcVideoDeviceEnumerator::Enumerate(BOOL bCapture)
{
	Release();

	BOOL bSuccessful = FALSE;
	if (bCapture) {
		IDShowCaptureEnumModule* pDShowCaptureEnum = NULL;
		if (VOIP_FAILED(IDShowCaptureEnumModule::Create(pDShowCaptureEnum)))
			return FALSE;

		for (int i=0; i<pDShowCaptureEnum->GetCount(); i ++) {
			// Filter Screen Capture
			if (_tcsicmp(pDShowCaptureEnum->GetDeviceName(i), TD_SCREEN_CAPTURE_IDENTIFIER) == 0)
				continue;

			CElcVideoCaptureItem * pItem = new CElcVideoCaptureItem;

			pItem->m_nType = ELCDT_VIDEO_CAPTURE;
			pItem->m_strDeviceId = pDShowCaptureEnum->GetDeviceID(i);
			pItem->m_strDeviceName = pDShowCaptureEnum->GetDeviceName(i);

			m_arrItem.push_back(pItem);
		}

		VOIP_DESTROY_MODULE(pDShowCaptureEnum);
		bSuccessful = TRUE;
	}
	else {
		// Not Implement
	}
	return (int)m_arrItem.size();
}


//////////////////////////////////////////////////////////////////////////////
// audio device handle functions 
CString GetHardwareId(LPCTSTR lpszPnpId)
{
	CString strPnpIds = (LPCTSTR)(lpszPnpId + 4); // skip _T("\\?\");
	int nFind = strPnpIds.Find(_T("#"));
	if (nFind != -1) {
		nFind = strPnpIds.Find(_T("#"), nFind + 1);
		if (nFind != -1)
			strPnpIds.Delete(nFind, strPnpIds.GetLength() - nFind);
		else
			strPnpIds.Empty();
	}
	else {
		strPnpIds.Empty();
	}

	return strPnpIds;
}

CString GetSubName(LPCTSTR lpszPnpId)
{
	LPCTSTR pszEnd = lpszPnpId + _tcslen(lpszPnpId) - 1;
	while (pszEnd > lpszPnpId && *pszEnd != '\\')
		pszEnd --;
	return (CString)pszEnd;
}

BOOL CheckAudioDevice(CString strPnpId, CString strHardwareId, CString strName)
{
	strPnpId.MakeLower();
	strHardwareId.MakeLower();
	strName.MakeLower();

	if (strPnpId.Find(strHardwareId) != -1
		&& strPnpId.Find(strName) != -1)
		return TRUE;
	return FALSE;
}

GUID GetRendererDeviceId(LPCTSTR lpszHardwareId, LPCTSTR lpszName)
{
	GUID guid = GUID_NULL;

	IDSoundRendererEnumModule* pRendererEnum = NULL;
	if (VOIP_FAILED(IDSoundRendererEnumModule::Create(pRendererEnum)))
		return guid;

	int nCount = pRendererEnum->GetCount();
	for (int i=1; i < nCount; i++) {
		if (CheckAudioDevice(pRendererEnum->GetDevicePnpID(i), lpszHardwareId, lpszName)) {
			guid = *pRendererEnum->GetDeviceID(i);
			break;
		}
	}

	VOIP_DESTROY_MODULE(pRendererEnum);
	return guid;
}

BOOL GetRendererDeviceId(OUT CString& strDeviceId, LPCTSTR lpszHardwareId, LPCTSTR lpszName)
{
	GUID guid = GetRendererDeviceId(lpszHardwareId, lpszName);
	if (guid != GUID_NULL
		&& ElcGuid2String(&guid, strDeviceId)) 
		return TRUE;
	else
		return FALSE;
}

GUID GetCaptureDeviceId(LPCTSTR lpszHardwareId, LPCTSTR lpszName)
{
	GUID guid = GUID_NULL;
	IDSoundCaptureEnumModule* pCaptureEnum = NULL;
	if (VOIP_FAILED(IDSoundCaptureEnumModule::Create(pCaptureEnum)))
		return guid;

	int nCount = pCaptureEnum->GetCount();
	for (int i=0; i < nCount; i++) {
		if (CheckAudioDevice(pCaptureEnum->GetDevicePnpID(i), lpszHardwareId, lpszName)) {
			guid = *pCaptureEnum->GetDeviceID(i);
			break;
		}
	}

	VOIP_DESTROY_MODULE(pCaptureEnum);
	return guid;
}

BOOL GetCaptureDeviceId(OUT CString& strDeviceId, LPCTSTR lpszHardwareId, LPCTSTR lpszName)
{
	GUID guid = GetCaptureDeviceId(lpszHardwareId, lpszName);
	if (guid != GUID_NULL
		&& ElcGuid2String(&guid, strDeviceId)) 
		return TRUE;
	else
		return FALSE;
}

CString GetRendererPnpId(LPCTSTR lpszHardwareId, LPCTSTR lpszName)
{
	CString strPnpId;
	IDSoundRendererEnumModule* pRendererEnum = NULL;
	if (VOIP_FAILED(IDSoundRendererEnumModule::Create(pRendererEnum)))
		return strPnpId;

	int nCount = pRendererEnum->GetCount();
	for (int i=1; i < nCount; i++) {
		if (CheckAudioDevice(pRendererEnum->GetDevicePnpID(i), lpszHardwareId, lpszName)) {
			strPnpId = pRendererEnum->GetDevicePnpID(i);
			break;
		}
	}

	VOIP_DESTROY_MODULE(pRendererEnum);
	return strPnpId;
}

CString GetCapturePnpId(LPCTSTR lpszHardwareId, LPCTSTR lpszName)
{
	CString strPnpId;
	IDSoundCaptureEnumModule* pCaptureEnum = NULL;
	if (VOIP_FAILED(IDSoundCaptureEnumModule::Create(pCaptureEnum)))
		return strPnpId;

	int nCount = pCaptureEnum->GetCount();
	for (int i=0; i < nCount; i++) {
		if (CheckAudioDevice(pCaptureEnum->GetDevicePnpID(i), lpszHardwareId, lpszName)) {
			strPnpId = pCaptureEnum->GetDevicePnpID(i);
			break;
		}
	}

	VOIP_DESTROY_MODULE(pCaptureEnum);
	return strPnpId;
}

BOOL IsXVAPlayback(LPCTSTR lpszPnpId) 
{
	return CheckAudioDevice(lpszPnpId, ELC_XVA_ID, ELC_XVA_PLAYBACK);
}

BOOL IsXVALoopback(LPCTSTR lpszPnpId)
{
	return CheckAudioDevice(lpszPnpId, ELC_XVA_ID, ELC_XVA_LOOPBACK);
}

BOOL IsXA001AUPlayback(LPCTSTR lpszPnpId) 
{
	return CheckAudioDevice(lpszPnpId, ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_PLAYBACK);
}

BOOL IsXA001AUVoice(LPCTSTR lpszPnpId) 
{
	return CheckAudioDevice(lpszPnpId, ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_VOICE_PLAYBACK);
}

BOOL IsXA001AURecord(LPCTSTR lpszPnpId) 
{
	return CheckAudioDevice(lpszPnpId, ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_RECORD);
}

BOOL IsXA001AULoopback(LPCTSTR lpszPnpId) 
{
	return CheckAudioDevice(lpszPnpId, ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_LOOPBACK);
}

GUID GetXVAPlayback()
{
	return GetRendererDeviceId(ELC_XVA_ID, ELC_XVA_PLAYBACK);
}

GUID GetXVALoopback()
{
	return GetCaptureDeviceId(ELC_XVA_ID, ELC_XVA_LOOPBACK);
}

GUID GetXA001Playback()
{
	return GetRendererDeviceId(ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_PLAYBACK);
}

GUID GetXA001Voice()
{
	return GetRendererDeviceId(ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_VOICE_PLAYBACK);
}

GUID GetXA001Record()
{
	return GetCaptureDeviceId(ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_RECORD);
}

GUID GetXA001Loopback()
{
	return GetCaptureDeviceId(ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_LOOPBACK);
}

CString GetXA001PlaybackPnpId()
{
	return GetRendererPnpId(ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_PLAYBACK);
}

CString GetXA001VoicePnpId()
{
	return GetRendererPnpId(ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_VOICE_PLAYBACK);
}

CString GetXA001RecordPnpId()
{
	return GetCapturePnpId(ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_RECORD);
}

CString GetXA001LoopbackPnpId()
{
	return GetCapturePnpId(ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_LOOPBACK);
}

/*
BOOL IsXA001AUHeadphonePluged()
{
	BOOL bSuccessful = FALSE;
	IKsPropertyModule* pKsPropertyModule = NULL;

	do
	{
		CString strDevicePndID = GetXA001PlaybackPnpId();
		if (strDevicePndID.IsEmpty())
			break;

		if (VOIP_FAILED(IKsPropertyModule::Create(strDevicePndID, pKsPropertyModule)))
			break;

		DWORD dwStatus = 0;
		if (VOIP_FAILED(pKsPropertyModule->GetProperty(
			PROPSETID_XA001AU_CUSTOM,
			KSPROPERTY_XA001AU_GET_JACK_STATUS, 
			&dwStatus,
			sizeof(DWORD))))
			break;

		if (dwStatus & JACK_HEADPHONE_OUT)
			bSuccessful = TRUE;

	} while(0);

	VOIP_DESTROY_MODULE(pKsPropertyModule);
	return bSuccessful;
}

BOOL IsXA001AUMicrophonePluged()
{
	BOOL bSuccessful = FALSE;
	IKsPropertyModule* pKsPropertyModule = NULL;

	do
	{
		CString strDevicePndID = GetXA001RecordPnpId();
		if (strDevicePndID.IsEmpty())
			break;

		if (VOIP_FAILED(IKsPropertyModule::Create(strDevicePndID, pKsPropertyModule)))
			break;

		DWORD dwStatus = 0;
		if (VOIP_FAILED(pKsPropertyModule->GetProperty(
			PROPSETID_XA001AU_CUSTOM,
			KSPROPERTY_XA001AU_GET_JACK_STATUS, 
			&dwStatus,
			sizeof(DWORD))))
			break;

		if (dwStatus & JACK_MICROPHONE_IN)
			bSuccessful = TRUE;

	} while(0);

	VOIP_DESTROY_MODULE(pKsPropertyModule);
	return bSuccessful;
}

DWORD GetXA001AUPlugState()
{
	DWORD dwStatus = 0xFFFFFFFF;
	IKsPropertyModule* pKsPropertyModule = NULL;

	do
	{
		CString strDevicePndID = GetXA001PlaybackPnpId();
		if (strDevicePndID.IsEmpty())
			break;

		if (VOIP_FAILED(IKsPropertyModule::Create(strDevicePndID, pKsPropertyModule)))
			break;

		if (VOIP_FAILED(pKsPropertyModule->GetProperty(
			PROPSETID_XA001AU_CUSTOM,
			KSPROPERTY_XA001AU_GET_JACK_STATUS, 
			&dwStatus,
			sizeof(DWORD))))
			break;

	} while(0);

	VOIP_DESTROY_MODULE(pKsPropertyModule);
	return dwStatus;
}

BOOL EnableXA001AUSideTone(BOOL bEnable)
{
	BOOL bSuccessful = FALSE;
	IKsPropertyModule* pKsPropertyModule = NULL;

	do
	{
		CString strDevicePndID = GetXA001RecordPnpId();
		if (strDevicePndID.IsEmpty())
			break;

		if (VOIP_FAILED(IKsPropertyModule::Create(strDevicePndID, pKsPropertyModule)))
			break;

		if (VOIP_FAILED(pKsPropertyModule->SetProperty(
			PROPSETID_XA001AU_CUSTOM,
			KSPROPERTY_XA001AU_SET_SIDE_TALK, 
			&bEnable,
			sizeof(BOOL))))
			break;

		bSuccessful = TRUE;
	} while(0);

	VOIP_DESTROY_MODULE(pKsPropertyModule);
	return bSuccessful;
}
*/

int CheckXiAudioDevice()
{
	int nRet = ELC_XADT_NONE;
	CElcAudioDeviceEnumerator audioEnum;

	if (audioEnum.Enumerate(FALSE)) {
		CElcDeviceArray * pArrItem = audioEnum.GetItems();
		for (CElcDeviceArray::iterator it = pArrItem->begin();
			it != pArrItem->end();
			it ++)
		{
			if (IsXA001AUPlayback((*it)->m_strDevicePnpId))
				nRet |= ELC_XADT_XA001AU;
			else if (IsXVAPlayback((*it)->m_strDevicePnpId))
				nRet |= ELC_XADT_XVA;
		}
	}

	return nRet;
}

int GetAudioInputCurSel(const GUID guidDevice)
{
	IAudioMixerModule* pInputMixer = NULL;
	if (VOIP_FAILED(IAudioMixerModule::Create(guidDevice, TRUE, pInputMixer)))
		return -1;

	int nCurSel = -1;

	for (int i=0; i < pInputMixer->GetLineCount(); i ++) {
		if (pInputMixer->IsLineHasSelectControl(i)) {
			nCurSel = i;
			break;
		}
	}

	VOIP_DESTROY_MODULE(pInputMixer);

	return nCurSel;
}

BOOL GetDefaultInputDevice(GUID& guid)
{
	IDSoundCaptureEnumModule* pCaptureEnum = NULL;
	guid = GUID_NULL;

	BOOL bSuccessful = FALSE;
	do 
	{
		if (VOIP_FAILED(IDSoundCaptureEnumModule::Create(pCaptureEnum)))
			break;

		int nCount = pCaptureEnum->GetCount();
		for (int i=1; i < nCount; i ++) {
			if (!IsXVALoopback(pCaptureEnum->GetDevicePnpID(i))) {
				guid = *pCaptureEnum->GetDeviceID(i);
				bSuccessful = TRUE;
				break;
			}
		}

	} while (0);

	VOIP_DESTROY_MODULE(pCaptureEnum);

	return bSuccessful;
}

BOOL HasAvailableInputDevice()
{
	IDSoundCaptureEnumModule* pCaptureEnum = NULL;
	BOOL bSuccessful = FALSE;
	do 
	{
		if (VOIP_FAILED(IDSoundCaptureEnumModule::Create(pCaptureEnum)))
			break;

		if (pCaptureEnum->GetCount() <= 1)
			break;

		bSuccessful = TRUE;

	} while (0);

	VOIP_DESTROY_MODULE(pCaptureEnum);

	return bSuccessful;
}

BOOL IsDeviceAvailable(const GUID& guid, BOOL bCapture)
{
	IAudioMixerModule* pMixer = NULL;
	if (VOIP_FAILED(IAudioMixerModule::Create(guid, bCapture, pMixer))) {
		if (bCapture)
			return FALSE;
		else {
			IDSoundRendererModule * pRenderer = NULL;
			if (VOIP_FAILED(IDSoundRendererModule::Create(
				guid,
				44100,
				2,
				1000,
				2,
				pRenderer)))
				return FALSE;

			VOIP_DESTROY_MODULE(pRenderer);
			return TRUE;
		}
	}

	VOIP_DESTROY_MODULE(pMixer);
	return TRUE;
}

BOOL IsDeviceAvailable(LPCTSTR lpszDeviceId, BOOL bCapture)
{
	GUID guid;
	if (!ElcString2Guid(guid, lpszDeviceId))
		return FALSE;

	return IsDeviceAvailable(guid, bCapture);
}

BOOL GetRealAudioDeviceId(GUID& guid, BOOL bCapture)
{
	BOOL bFound = FALSE;

	if (bCapture) {
		IDSoundCaptureEnumModule* pDSCaptureEnum = NULL;
		if (VOIP_FAILED(IDSoundCaptureEnumModule::Create(pDSCaptureEnum)))
			return bFound;

		for (int i=1; i < pDSCaptureEnum->GetCount(); i ++) {
			if (!IsXVALoopback(pDSCaptureEnum->GetDevicePnpID(i))
				&& !IsXA001AURecord(pDSCaptureEnum->GetDevicePnpID(i))
				&& !IsXA001AULoopback(pDSCaptureEnum->GetDevicePnpID(i))) 
			{
				guid = *pDSCaptureEnum->GetDeviceID(i);
				bFound = TRUE;
				break;
			}
		}

		VOIP_DESTROY_MODULE(pDSCaptureEnum);
	}
	else {
		IDSoundRendererEnumModule* pDSRendererEnum = NULL;
		if (VOIP_FAILED(IDSoundRendererEnumModule::Create(pDSRendererEnum)))
			return bFound;

		for (int i=1; i < pDSRendererEnum->GetCount(); i ++) {
			if (!IsXVAPlayback(pDSRendererEnum->GetDevicePnpID(i))
				&& !IsXA001AUPlayback(pDSRendererEnum->GetDevicePnpID(i))
				&& !IsXA001AUVoice(pDSRendererEnum->GetDevicePnpID(i))) 
			{
				guid = *pDSRendererEnum->GetDeviceID(i);
				bFound = TRUE;
				break;
			}
		}

		VOIP_DESTROY_MODULE(pDSRendererEnum);
	}

	return bFound;
}

BOOL SetDefaultAudioDevice_XP(LPCTSTR lpszDeviceName)
{
	HKEY hKey = NULL;
	LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER, AUDIO_RENDER_REGKEY_WINXP, 0, KEY_ALL_ACCESS, &hKey);
	if (ERROR_SUCCESS != lRet) {
		lRet = RegCreateKeyEx(HKEY_CURRENT_USER, AUDIO_RENDER_REGKEY_WINXP, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL);
		if (ERROR_SUCCESS != lRet) {
			return FALSE;
		}
	}

	BOOL bRet = FALSE;
	lRet = RegSetValueEx(hKey, _T("Playback"), 0, REG_SZ, (LPBYTE)lpszDeviceName, (DWORD)((_tcslen(lpszDeviceName) + 1) * sizeof(TCHAR)));
	if (ERROR_SUCCESS == lRet) {
		DWORD dwType = 0;
		DWORD dwValue = 0;
		DWORD cbData = 0;
		lRet = RegQueryValueEx(hKey, _T("SetupPreferredAudioDevicesCount"), 0, &dwType, (LPBYTE)&dwValue, &cbData);
		if (ERROR_SUCCESS != lRet) {
			dwValue = 1;
			RegSetValueEx(hKey, _T("SetupPreferredAudioDevicesCount"), 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
		}

		bRet = TRUE;
	}

	RegCloseKey(hKey);
	return bRet;
}

BOOL SetDefaultAudioDevice_Vista(LPCTSTR lpszHardwareId, LPCTSTR lpszName)
{
	HRESULT hr = S_FALSE;
	BOOL bSuccessful = FALSE;

	IMMDeviceEnumerator *pDeviceEnumerator = NULL;
	IMMDeviceCollection* pCollection = NULL;
	IPolicyConfigVista *pPolicyConfigVista = NULL;

	do {
		CElcAudioDeviceEnumerator enumerator;
		if (!enumerator.Enumerate(FALSE))
			break;
		CElcDeviceItem * pAudioItem = enumerator.FindItemByPnpId(lpszHardwareId, lpszName);
		if (!pAudioItem)
			break;

		if (FAILED(CoCreateInstance(
			__uuidof(MMDeviceEnumerator), 
			NULL, 
			CLSCTX_INPROC_SERVER, 
			__uuidof(IMMDeviceEnumerator), 
			(LPVOID *)&pDeviceEnumerator)))
		{
			break;
		}

		if (FAILED(pDeviceEnumerator->EnumAudioEndpoints(
			eRender, 
			DEVICE_STATE_ACTIVE, 
			&pCollection)))
		{
			break;
		}

		LPWSTR lpszId = NULL;
		UINT nCount = 0;
		pCollection->GetCount(&nCount);

		for (UINT i=0; i<nCount; i++) {
			IMMDevice* pDevice = NULL;
			if (SUCCEEDED(pCollection->Item(i, &pDevice))) {

				pDevice->GetId(&lpszId);
				SAFE_RELEASE(pDevice);

				LPCWSTR lpszIdDup = lpszId + 17;
				if (pAudioItem->m_strDeviceId.CompareNoCase(lpszIdDup) == 0)
					break;

				CoTaskMemFree(lpszId);
				lpszId = NULL;
			}
		}

		if (lpszId && SUCCEEDED(CoCreateInstance(
			__uuidof(CPolicyConfigVistaClient), 
			NULL, 
			CLSCTX_ALL, 
			__uuidof(IPolicyConfigVista), 
			(LPVOID *)&pPolicyConfigVista)))
		{
			if (SUCCEEDED(pPolicyConfigVista->SetDefaultEndpoint(lpszId, eMultimedia))
				&& SUCCEEDED(pPolicyConfigVista->SetDefaultEndpoint(lpszId, eCommunications)))
				bSuccessful = TRUE;
		}

		if (lpszId)
			CoTaskMemFree(lpszId);

	} while (0);

	SAFE_RELEASE(pPolicyConfigVista);
	SAFE_RELEASE(pCollection);
	SAFE_RELEASE(pDeviceEnumerator);

	return bSuccessful;
}

BOOL SetDefaultAudioDevice(ELC_XI_AUDIO_DEVICE_TYPE type)
{
	OSVERSIONINFO osVerInfo = {0};
	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	GetVersionEx(&osVerInfo);

	BOOL bRet = FALSE;
	if (osVerInfo.dwMajorVersion >= 6) {
		if (type == ELC_XADT_XA001AU) {
			bRet = SetDefaultAudioDevice_Vista(ELC_XA001AU_HARDWARE_ID, ELC_XA001AU_PLAYBACK);
		}
		else if (type == ELC_XADT_XVA) {
			bRet = SetDefaultAudioDevice_Vista(ELC_XVA_ID, ELC_XVA_PLAYBACK);
		}
	}
	else {
		if (type == ELC_XADT_XA001AU) {
			bRet = SetDefaultAudioDevice_XP(ELC_XA001AU_PLAYBACK_NAME_XP);
		}
		else if (type == ELC_XADT_XVA) {
			bRet = SetDefaultAudioDevice_XP(ELC_XVA_PLAYBACK_NAME_XP);
		}
	}
	return bRet;
}

BOOL SetDefaultAudioDevice()
{
	int nRet = CheckXiAudioDevice();
	if (nRet & ELC_XADT_XA001AU)
		return SetDefaultAudioDevice(ELC_XADT_XA001AU);
	else if (nRet & ELC_XADT_XVA)
		return SetDefaultAudioDevice(ELC_XADT_XVA);
	else 
		return FALSE;
}


