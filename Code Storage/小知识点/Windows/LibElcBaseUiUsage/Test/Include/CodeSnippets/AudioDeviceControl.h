#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include "ADCCommon.h"
#include <map>
#include <vector>

#define WM_MASTER_MUTE_CHANGE_NOTIFY								(WM_USER + 100)
#define WM_MASTER_VOLUME_CHANGE_NOTIFY								(WM_USER + 101)
#define WM_LINE_SELECT_CHANGE_NOTIFY								(WM_USER + 102)
#define WM_LINE_VOLUME_CHANGE_NOTIFY								(WM_USER + 103)

inline BOOL IsXICard(LPCTSTR lpszDeviceID) 
{ 
	if (_tcsstr(lpszDeviceID, XI_VGA_CARD_IDENTIFIER) != NULL
		|| _tcsstr(lpszDeviceID, XI_6CVBS_CARD_IDENTIFIER) != NULL)
		return TRUE;
	return FALSE;
}

inline BOOL IsVGADevice(LPCTSTR lpszDeviceID) 
{ 
	return _tcsstr(lpszDeviceID, _T("\\vga")) != NULL; 
}

// Get Hardware id from pnp id string
CString GetHardwareId(LPCTSTR lpszPnpId);
// Get subname from pnp id string. the subname string is include "\"
CString GetSubName(LPCTSTR lpszPnpId);

// Check Device by pnp id
BOOL CheckAudioDevice(CString strPnpId, CString strHardwareId, CString strName);

// Get Device id by hardware id & name
// Return GUID_NULL if not found.
GUID GetRendererDeviceId(LPCTSTR lpszHardwareId, LPCTSTR lpszName);
BOOL GetRendererDeviceId(OUT CString& strDeviceId, LPCTSTR lpszHardwareId, LPCTSTR lpszName);
GUID GetCaptureDeviceId(LPCTSTR lpszHardwareId, LPCTSTR lpszName);
BOOL GetCaptureDeviceId(OUT CString& strDeviceId, LPCTSTR lpszHardwareId, LPCTSTR lpszName);

CString GetRendererPnpId(LPCTSTR lpszHardwareId, LPCTSTR lpszName);
CString GetCapturePnpId(LPCTSTR lpszHardwareId, LPCTSTR lpszName);

/******************************************************
 XVA Device
*/
// Check XVA Device
BOOL IsXVAPlayback(LPCTSTR lpszPnpId);
BOOL IsXVALoopback(LPCTSTR lpszPnpId);
// Get XVA Device
GUID GetXVAPlayback();
GUID GetXVALoopback();

/******************************************************
 XA001AU Device
*/
// Check XA001AU Device
BOOL IsXA001AUPlayback(LPCTSTR lpszPnpId);
BOOL IsXA001AUVoice(LPCTSTR lpszPnpId);
BOOL IsXA001AURecord(LPCTSTR lpszPnpId);
BOOL IsXA001AULoopback(LPCTSTR lpszPnpId);
// Get XA001AU Device Id. Return GUID_NULL if not found.
GUID GetXA001Playback();
GUID GetXA001Voice();
GUID GetXA001Record();
GUID GetXA001Loopback();
// Get XA001AU Device Pnp Id
CString GetXA001PlaybackPnpId();
CString GetXA001VoicePnpId();
CString GetXA001RecordPnpId();
CString GetXA001LoopbackPnpId();

// XA001AU Properties
/*
BOOL IsXA001AUHeadphonePluged();
BOOL IsXA001AUMicrophonePluged();
DWORD GetXA001AUPlugState();
BOOL EnableXA001AUSideTone(BOOL bEnable);
*/

// check which audio device has been installed
int CheckXiAudioDevice();


// if device is not "Single select" device
// return the first select input;
// this function is used only for xp/2K
int GetAudioInputCurSel(const GUID guidDevice);

// get the default audio input device's guid
BOOL GetDefaultInputDevice(GUID& guid);

// check Available input device
BOOL HasAvailableInputDevice();

BOOL IsDeviceAvailable(const GUID& guid, BOOL bCapture);
BOOL IsDeviceAvailable(LPCTSTR lpszDeviceId, BOOL bCapture);

// get the first real audio device id
BOOL GetRealAudioDeviceId(GUID& guid, BOOL bCapture);

// Set default audio device
// Notice : Make sure call CoInitialize() before when use this function at window vista or later
BOOL SetDefaultAudioDevice();
BOOL SetDefaultAudioDevice(ELC_XI_AUDIO_DEVICE_TYPE type); // Only support ELC_XADT_XA001AU and ELC_XADT_XVA
BOOL SetDefaultAudioDevice_XP(LPCTSTR lpszDeviceName);
BOOL SetDefaultAudioDevice_Vista(LPCTSTR lpszHardwareId, LPCTSTR lpszName);


/////////////////////////////////////////////////
// IAudioMixerControlCallback
class IAudioMixerControlCallback 
{
public:
	virtual void OnVolumeChanged(UINT nId, BOOL bValid, int nVolume, BOOL bRealMixer, PVOID pvParam) {}
	virtual void OnMuteChanged(UINT nId, BOOL bMuted, BOOL bRealMixer, PVOID pvParam) {}

	virtual void OnLineSelectChanged(UINT nId, int nLineIndex, PVOID pvParam) {};
};


/////////////////////////////////////////////////
// CAudioMixerControl
class CAudioMixerControl : 
	public CWindowImpl<CAudioMixerControl , CWindow, CFrameWinTraits>,
	public IAudioMixerModule::ICallback
{
public:
	DECLARE_WND_CLASS(NULL);

public:
	CAudioMixerControl();
	virtual ~CAudioMixerControl();

public:
	BOOL CreateInstance(UINT nId, IAudioMixerControlCallback *pCallback, PVOID pvParam);
	void Destroy();

	BOOL OpenDevice(const GUID* pGuid, BOOL bCaptureDevice, int nInput, BOOL bRealMixer=TRUE);

	// volume range : 0-100
	void SetVolume(int nVolume);
	int GetVolume();

	void SetMute(BOOL bMute);
	BOOL IsMuted();

	BOOL IsValid() {
		if (m_bRealMixer)
			return m_pMixer != NULL;
		return TRUE;
	}

	void SetCallback(IAudioMixerControlCallback *pCallback, PVOID pvParam) {
		m_pCallback = pCallback;
		m_pvParam = pvParam;
	}

	BOOL IsRealMixer() {
		return m_bRealMixer;
	}

	BOOL IsCaptureDevice() {
		return m_bCaptureDevice;
	}

	const GUID* GetDeviceId() {
		return &m_guidDevice;
	}

	int GetInput() {
		return m_nInput;
	}

protected:
	// IAudioMixerModule::ICallback Interface
	virtual void OnMasterMuteChange(LPVOID pvParam) {
		::PostMessage(m_hWnd, WM_MASTER_MUTE_CHANGE_NOTIFY, 0, (LPARAM)pvParam);
	}
	virtual void OnMasterVolumeChange(LPVOID pvParam) {
		::PostMessage(m_hWnd, WM_MASTER_VOLUME_CHANGE_NOTIFY, 0, (LPARAM)pvParam);
	}
	virtual void OnLineSelectChange(int nLineIndex, LPVOID pvParam) {
		::PostMessage(m_hWnd, WM_LINE_SELECT_CHANGE_NOTIFY, (WPARAM)nLineIndex, (LPARAM)pvParam);
	}
	virtual void OnLineVolumeChange(int nLineIndex, LPVOID pvParam) {
		::PostMessage(m_hWnd, WM_LINE_VOLUME_CHANGE_NOTIFY, (WPARAM)nLineIndex, (LPARAM)pvParam);
	}

protected:
	BEGIN_MSG_MAP(CAudioMixerControl)
		MESSAGE_HANDLER(WM_MASTER_MUTE_CHANGE_NOTIFY, OnMasterMuteChangeNotify)
		MESSAGE_HANDLER(WM_MASTER_VOLUME_CHANGE_NOTIFY, OnMasterVolumeChangeNotify)
		MESSAGE_HANDLER(WM_LINE_SELECT_CHANGE_NOTIFY, OnLineSelectChangeNotify)
		MESSAGE_HANDLER(WM_LINE_VOLUME_CHANGE_NOTIFY, OnLineVolumeChangeNotify)
	END_MSG_MAP()

	// message handler
	LRESULT OnMasterMuteChangeNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMasterVolumeChangeNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLineSelectChangeNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLineVolumeChangeNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	IAudioMixerModule*					m_pMixer;
	IAudioMixerControlCallback*			m_pCallback;
	PVOID								m_pvParam;

	UINT								m_nId;

	BOOL								m_bCaptureDevice;
	GUID								m_guidDevice;
	int									m_nInput;

	BOOL								m_bWinVista;

	BOOL								m_bRealMixer;
	BOOL								m_bVirtualMuted;
	int									m_nVirtualVolume;
};



/////////////////////////////////////////////////
// CAudioMixerManager

typedef std::map<UINT, CAudioMixerControl*>		CAudioMixerMap;

class CAudioMixerManager
{
public:
	CAudioMixerManager();
	virtual ~CAudioMixerManager();

public:
	void SetCallback(IAudioMixerControlCallback *pCallback, PVOID pvParam);
	void Destroy();

	BOOL AddMixer(UINT nId, const GUID* pGuid, BOOL bCaptureDevice, int nInput, BOOL bRealMixer=TRUE);
	void RemoveMixer(UINT nId);

	// volume range : 0-100
	void SetVolume(UINT nId, int nVolume);
	int GetVolume(UINT nId);

	void SetMute(UINT nId, BOOL bMute);
	BOOL IsMuted(UINT nId);

	CAudioMixerControl* GetMixer(UINT nId);

protected:
	CAudioMixerMap					m_mapAudioMixer;
	IAudioMixerControlCallback *	m_pCallback;
	PVOID							m_pvParam;
};



///////////////////////////////////////////////////////
// Device Enumerator

enum ELC_DEVICE_TYPE {
	ELCDT_VIDEO_NONE = 0,
	ELCDT_VIDEO_CAPTURE,
	ELCDT_VIDEO_RENDERER, // not impl
	ELCDT_AUDIO_CAPTURE,
	ELCDT_AUDIO_RENDERER,
};

class CElcDeviceItem 
{
public:
	CElcDeviceItem() {
		m_nType = ELCDT_VIDEO_NONE;
		m_strDeviceId = _T("");
		m_strDevicePnpId = _T("");
		m_strDeviceName = _T("");
		m_dwData = 0;
	}
	~CElcDeviceItem() {
	}

	void operator=(const CElcDeviceItem& srcItem) {
		m_nType = srcItem.m_nType;
		m_strDeviceId = srcItem.m_strDeviceId;
		m_strDevicePnpId = srcItem.m_strDevicePnpId;
		m_strDeviceName = srcItem.m_strDeviceName;
		m_dwData = srcItem.m_dwData;
	}

public:
	UINT		m_nType; // ELC_DEVICE_TYPE
	CString		m_strDeviceId;
	CString		m_strDevicePnpId;
	CString		m_strDeviceName;
	DWORD_PTR	m_dwData; // user data
};

///////////////////////////////////////////////////////
// CElcVideoCaptureItem
class CElcVideoCaptureItem : public CElcDeviceItem
{
public:
	CElcVideoCaptureItem() {
		m_nType = ELCDT_VIDEO_CAPTURE;
	}
	~CElcVideoCaptureItem() {
	}

	void operator=(const CElcVideoCaptureItem& srcItem) {
		m_nType = srcItem.m_nType;
		m_strDeviceId = srcItem.m_strDeviceId;
		m_strDevicePnpId = srcItem.m_strDevicePnpId;
		m_strDeviceName = srcItem.m_strDeviceName;
		m_dwData = srcItem.m_dwData;
	}
};

class CElcVideoRendererItem : public CElcDeviceItem
{
public:
	CElcVideoRendererItem() {
		m_nType = ELCDT_VIDEO_RENDERER;
	}
	~CElcVideoRendererItem() {
	}

	void operator=(const CElcVideoRendererItem& srcItem) {
		m_nType = srcItem.m_nType;
		m_strDeviceId = srcItem.m_strDeviceId;
		m_strDevicePnpId = srcItem.m_strDevicePnpId;
		m_strDeviceName = srcItem.m_strDeviceName;
		m_dwData = srcItem.m_dwData;
	}
};

///////////////////////////////////////////////////////
// CElcAudioCaptureItem
class CElcAudioCaptureItem : public CElcDeviceItem
{
public:
	typedef std::vector<CString>	CInputItem;
	CInputItem						m_arrInput;

public:
	CElcAudioCaptureItem() {
		m_nType = ELCDT_AUDIO_CAPTURE;
	}
	~CElcAudioCaptureItem() {
	}

	void operator=(const CElcAudioCaptureItem& srcItem) {
		m_nType = srcItem.m_nType;
		m_strDeviceId = srcItem.m_strDeviceId;
		m_strDevicePnpId = srcItem.m_strDevicePnpId;
		m_strDeviceName = srcItem.m_strDeviceName;
		m_dwData = srcItem.m_dwData;
		m_arrInput = srcItem.m_arrInput;
	}
};

///////////////////////////////////////////////////////
// CElcAudioRendererItem
class CElcAudioRendererItem : public CElcDeviceItem
{
public:
	CElcAudioRendererItem() {
		m_nType = ELCDT_AUDIO_RENDERER;
	}
	~CElcAudioRendererItem() {
	}

	void operator=(const CElcAudioRendererItem& srcItem) {
		m_nType = srcItem.m_nType;
		m_strDeviceId = srcItem.m_strDeviceId;
		m_strDevicePnpId = srcItem.m_strDevicePnpId;
		m_strDeviceName = srcItem.m_strDeviceName;
		m_dwData = srcItem.m_dwData;
	}
};


typedef std::vector<CElcDeviceItem *>	CElcDeviceArray;

///////////////////////////////////////////////////////
// CElcDeviceEnumerator
class CElcDeviceEnumerator
{
public:
	CElcDeviceEnumerator() {}
	~CElcDeviceEnumerator() {
		Release();
	}

public:
	CElcDeviceArray * GetItems() {
		return &m_arrItem;
	}

	void Release() {
		for (CElcDeviceArray::iterator it = m_arrItem.begin();
			it != m_arrItem.end();
			it ++)
		{
			if ((*it)->m_nType == ELCDT_VIDEO_CAPTURE) 
				delete (CElcVideoCaptureItem *)(*it);
			else if ((*it)->m_nType == ELCDT_VIDEO_RENDERER)
				delete (CElcVideoRendererItem *)(*it);
			else if ((*it)->m_nType == ELCDT_AUDIO_CAPTURE)
				delete (CElcAudioCaptureItem *)(*it);
			else if ((*it)->m_nType == ELCDT_AUDIO_RENDERER)
				delete (CElcAudioRendererItem *)(*it);
		}

		m_arrItem.clear();
	}

	CElcDeviceItem * InsertItem(const CElcDeviceItem* pItem, CElcDeviceArray::iterator _Where) {
		CElcDeviceItem * pNewItem = NULL;
		if (pItem->m_nType == ELCDT_VIDEO_NONE) {
			return NULL;
		}
		else if (pItem->m_nType == ELCDT_VIDEO_CAPTURE) {
			pNewItem = new CElcVideoCaptureItem;
			*(CElcVideoCaptureItem *)pNewItem = *(CElcVideoCaptureItem *)pItem;
		}
		else if (pItem->m_nType == ELCDT_VIDEO_RENDERER) {
			pNewItem = new CElcVideoRendererItem;
			*(CElcVideoRendererItem *)pNewItem = *(CElcVideoRendererItem *)pItem;
		}
		else if (pItem->m_nType == ELCDT_AUDIO_CAPTURE) {
			pNewItem = new CElcAudioCaptureItem;
			*(CElcAudioCaptureItem *)pNewItem = *(CElcAudioCaptureItem *)pItem;
		}
		else if (pItem->m_nType == ELCDT_AUDIO_RENDERER) {
			pNewItem = new CElcAudioRendererItem;
			*(CElcAudioRendererItem *)pNewItem = *(CElcAudioRendererItem *)pItem;
		}

		m_arrItem.insert(_Where, pNewItem);
		return pNewItem;
	}

	CElcDeviceItem * FindItemByName(LPCTSTR lpszDeviceName) {
		for (CElcDeviceArray::iterator it = m_arrItem.begin();
			it != m_arrItem.end();
			it ++)
		{
			if ((*it)->m_strDeviceName.CompareNoCase(lpszDeviceName) == 0
				|| (*it)->m_strDeviceName.Find(lpszDeviceName) != -1)
				return (*it);
		}
		return NULL;
	}
	CElcDeviceItem * FindItemById(LPCTSTR lpszDeviceId) {
		for (CElcDeviceArray::iterator it = m_arrItem.begin();
			it != m_arrItem.end();
			it ++)
		{
			if ((*it)->m_strDeviceId.CompareNoCase(lpszDeviceId) == 0)
				return (*it);
		}
		return NULL;
	}

	CElcDeviceItem * FindItemBySubString(LPCTSTR lpszSubString) {
		for (CElcDeviceArray::iterator it = m_arrItem.begin();
			it != m_arrItem.end();
			it ++)
		{
			if (_tcsstr((*it)->m_strDeviceId, lpszSubString) != NULL)
				return (*it);
		}
		return NULL;
	}

	void Clone(const CElcDeviceArray * pSrc) {
		if (!pSrc || &m_arrItem == pSrc)
			return;

		Release();

		for (CElcDeviceArray::const_iterator it = pSrc->begin();
			it != pSrc->end();
			it ++) 
		{
			CElcDeviceItem * pNewItem = NULL;
			if ((*it)->m_nType == ELCDT_VIDEO_NONE) {
				return;
			}
			else if ((*it)->m_nType == ELCDT_VIDEO_CAPTURE) {
				pNewItem = new CElcVideoCaptureItem;
				*(CElcVideoCaptureItem *)pNewItem = *(CElcVideoCaptureItem *)(*it);
			}
			else if ((*it)->m_nType == ELCDT_VIDEO_RENDERER) {
				pNewItem = new CElcVideoRendererItem;
				*(CElcVideoRendererItem *)pNewItem = *(CElcVideoRendererItem *)(*it);
			}
			else if ((*it)->m_nType == ELCDT_AUDIO_CAPTURE) {
				pNewItem = new CElcAudioCaptureItem;
				*(CElcAudioCaptureItem *)pNewItem = *(CElcAudioCaptureItem *)(*it);
			}
			else if ((*it)->m_nType == ELCDT_AUDIO_RENDERER) {
				pNewItem = new CElcAudioRendererItem;
				*(CElcAudioRendererItem *)pNewItem = *(CElcAudioRendererItem *)(*it);
			}

			m_arrItem.push_back(pNewItem);
		}
	}

protected:
	CElcDeviceArray		m_arrItem;
};

///////////////////////////////////////////////////////
// CElcAudioDeviceEnumerator
class CElcAudioDeviceEnumerator : public CElcDeviceEnumerator
{
public:
	CElcAudioDeviceEnumerator() {
		m_bCapture = TRUE;
	}
	~CElcAudioDeviceEnumerator() {}

	// bFilterPrivateDevice : filter voice box and xva device
	int Enumerate(BOOL bCapture, BOOL bFilterPrivateDevice = FALSE);

	BOOL IsCapture() {
		return m_bCapture;
	}

	// find device by pnp id
	CElcDeviceItem * FindItemByPnpId(LPCTSTR lpszHardwareId, LPCTSTR lpszName) {
		for (CElcDeviceArray::iterator it = m_arrItem.begin();
			it != m_arrItem.end();
			it ++)
		{
			if (CheckAudioDevice((*it)->m_strDevicePnpId, lpszHardwareId, lpszName))
				return (*it);
		}
		return NULL;
	}

protected:
	BOOL		m_bCapture;
};

///////////////////////////////////////////////////////
// CElcVideoDeviceEnumerator
class CElcVideoDeviceEnumerator : public CElcDeviceEnumerator
{
public:
	CElcVideoDeviceEnumerator() {}
	~CElcVideoDeviceEnumerator() {}

	int Enumerate(BOOL bCapture);
};
