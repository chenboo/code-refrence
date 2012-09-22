// DLG.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDLGApp:
// See DLG.cpp for the implementation of this class
//

class CDLGApp : public CWinApp
{
public:
	CDLGApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDLGApp theApp;

inline CString InetDwordToString(DWORD dwIpAddress)
{
	CA2W pszW(inet_ntoa(*(in_addr *)&dwIpAddress));
	CString str = pszW;
	return str;
}