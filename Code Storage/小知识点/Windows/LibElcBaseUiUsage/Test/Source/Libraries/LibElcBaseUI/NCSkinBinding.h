/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////



#pragma once

#include <map>

typedef struct ELC_NC_SKIN_PARAMS {
	WNDPROC				pOldWndProc;
	PELC_SKIN_PNAE_NC	pSkin;
	BOOL				bFillBackground;
	HBRUSH				hBrBackgnd;
}*PELC_NC_SKIN_PARAMS;

typedef std::map<HWND, PELC_NC_SKIN_PARAMS>		CSkinBindingWndMap;

class LIBELCBASEUI_API CNCSkinBinding
{
public:
	CNCSkinBinding(void);
	~CNCSkinBinding(void);

	// lpStyle : see class CElcSkinDoc for more detail
	BOOL BindWindow(HWND hWnd, LPCTSTR lpStyle, BOOL bFillBackground=FALSE);
	BOOL BindWindow(CWnd* pWnd, LPCTSTR lpStyle, BOOL bFillBackground=FALSE);
	void UnBindWindow(HWND hWnd);

	PELC_NC_SKIN_PARAMS GetSkinParams(HWND hBindingWnd);

protected:
	static LRESULT CALLBACK HookWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	CSkinBindingWndMap		m_mapBindingWnd;
};
