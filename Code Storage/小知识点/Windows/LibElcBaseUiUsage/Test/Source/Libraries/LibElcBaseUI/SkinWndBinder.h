/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////



#pragma once

#include <map>

typedef struct ELC_SKINWND_PARAMS {
	WNDPROC				pOldWndProc;
	CElcSkinFrameBase*	pSkinWnd;
	PVOID				pvUser;
} *PELC_SKINWND_PARAMS;

typedef std::map<HWND, PELC_SKINWND_PARAMS>		CSkinWndParamMap;

class LIBELCBASEUI_API CSkinWndBinder 
{
public:
	CSkinWndBinder(void);
	~CSkinWndBinder(void);

	// lpStyle : use "Window Style"
	// See CElcSkinDoc for more detail.
	BOOL BindWindow(HWND hWnd, LPCTSTR lpStyle);
	BOOL BindWindow(CWnd* pWnd, LPCTSTR lpStyle);
	void UnBindWindow(HWND hWnd);

	PELC_SKINWND_PARAMS GetSkinParams(HWND hBindingWnd);
	CElcSkinFrameBase* GetSkinFrame(HWND hBindingWnd);

protected:
	static LRESULT CALLBACK HookWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	CSkinWndParamMap		m_mapBindingWnd;
};
