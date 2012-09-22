// GraphLib.h : main header file for the GraphLib DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CGraphLibApp
// See GraphLib.cpp for the implementation of this class
//

class CGraphLibApp : public CWinApp
{
public:
	CGraphLibApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
