#ifndef __ELCFUNCTION_H__
#define __ELCFUNCTION_H__

DEFINE_GUID(IID_IActiveFunction, 0x56588044,0x6C35,0x479D,0x83,0x18,0xF9,0x98,0x86,0x44,0x86,0x5E);
DEFINE_GUID(IID_IPassiveFunction, 0x391E507E,0xB99E,0x4540,0xBA,0x8A,0xCA,0x81,0xBC,0xBD,0x44,0xFD);

DECLARE_INTERFACE_(IActiveFunction, IUnknown)
{
	STDMETHOD(GetInfo)(THIS_ DWORD *pdwCaps, LPTSTR pszName, DWORD cbName, LPTSTR pszDesc, DWORD cbDesc) PURE;

	STDMETHOD(Initialize)(THIS_ void *pFunctionManager) PURE;
	STDMETHOD(Uninitialize)(THIS) PURE;

	STDMETHOD(Run)(THIS) PURE;
	STDMETHOD(RunEx)(THIS_ DWORD dwFocused, DWORD *pdwSelected, DWORD dwCount) PURE;

	STDMETHOD(RunGroup)(THIS_ DWORD dwGroupId, DWORD dwActive, DWORD *pdwPassive, DWORD dwCount) PURE;
	STDMETHOD(StopGroup)(THIS_ DWORD dwGroupId) PURE;

	STDMETHOD(Stop)(THIS) PURE;

	STDMETHOD(IsRunning)(THIS) PURE;

	STDMETHOD(CalcState)(THIS_ BYTE *lpbParameters, DWORD cbSize, DWORD *pdwState) PURE;
	STDMETHOD(GetStateIcon)(THIS_ DWORD dwState, HICON *phiconState) PURE;
	STDMETHOD(GetParameters)(THIS_ DWORD dwIpAddress, DWORD dwState, BYTE *lpbParameters, DWORD *pcbParameters) PURE;

	STDMETHOD(SetPanelBkgnd)(THIS_ HBITMAP hBmpBkgnd) PURE;
	STDMETHOD(GetPanelHandle)(THIS_ HWND *phwndPanel) PURE;
	STDMETHOD(GetPropPageHandle)(THIS_ HWND *phwndPropPage, DWORD *pdwCount) PURE;

	STDMETHOD(PumpPreTranslateMessage)(THIS_ MSG *pMsg) PURE;
	STDMETHOD(PumpIdle)(THIS_ LONG lCount) PURE;
};

DECLARE_INTERFACE_(IPassiveFunction, IUnknown)
{
	STDMETHOD(GetInfo)(THIS_ DWORD *pdwCaps, LPTSTR pszName, DWORD cbName, LPTSTR pszDesc, DWORD cbDesc) PURE;

	STDMETHOD(Initialize)(THIS_ void *pFunctionManager) PURE;
	STDMETHOD(Uninitialize)(THIS) PURE;

	STDMETHOD(Run)(THIS_ BYTE *lpbParameters, DWORD cbSize) PURE;
	STDMETHOD(SetParameters)(THIS_ BYTE *lpbParameters, DWORD cbSize) PURE;
	STDMETHOD(Stop)(THIS) PURE;

	STDMETHOD(IsRunning)(THIS) PURE;
	STDMETHOD(CalcState)(THIS_ BYTE *lpbParameters, DWORD cbSize, DWORD *pdwState) PURE;
	STDMETHOD(GetCurrentState)(THIS_ DWORD *pdwState) PURE;

	STDMETHOD(PumpPreTranslateMessage)(THIS_ MSG *pMsg) PURE;
	STDMETHOD(PumpIdle)(THIS_ LONG lCount) PURE;
};

#endif //__ELCFUNCTION_H__