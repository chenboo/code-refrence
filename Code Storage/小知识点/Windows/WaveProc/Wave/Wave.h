// Wave.h : main header file for the WAVE application
//

#if !defined(AFX_WAVE_H__E06E0626_F904_4ABD_B37D_895AD8CDFA3A__INCLUDED_)
#define AFX_WAVE_H__E06E0626_F904_4ABD_B37D_895AD8CDFA3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWaveApp:
// See Wave.cpp for the implementation of this class
//

class CWaveApp : public CWinApp
{
public:
	CWaveApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaveApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWaveApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVE_H__E06E0626_F904_4ABD_B37D_895AD8CDFA3A__INCLUDED_)
