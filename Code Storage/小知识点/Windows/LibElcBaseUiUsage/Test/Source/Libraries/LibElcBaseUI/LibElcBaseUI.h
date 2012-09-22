// LibElcBaseUI.h : main header file for the LibElcBaseUI DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CLibElcBaseUIApp
// See LibElcBaseUI.cpp for the implementation of this class
//

class CLibElcBaseUIApp : public CWinApp
{
public:
	CLibElcBaseUIApp();

	HINSTANCE				m_hResource;
	GdiplusStartupInput		gdiplusStartupInput;
	ULONG_PTR				gdiplusToken;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};


extern CLibElcBaseUIApp theApp;
extern CElcSkinDoc		theSkin;
extern CElcSkinPainter	thePainter;
extern CNCSkinBinding	theNCBinder;
extern CSkinWndBinder	theSkinWndBinder;

