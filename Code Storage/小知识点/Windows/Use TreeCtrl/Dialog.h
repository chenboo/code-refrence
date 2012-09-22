// Dialog.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDialogApp:
// See Dialog.cpp for the implementation of this class
//

class CDialogApp : public CWinApp
{
public:
	CDialogApp();

// Overrides
	public:
	virtual BOOL InitInstance();

	ULONG_PTR			m_gdiplusToken;

// Implementation

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int ExitInstance();
};

extern CDialogApp theApp;