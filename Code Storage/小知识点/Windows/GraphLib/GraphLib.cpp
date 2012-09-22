// GraphLib.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "GraphLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGraphLibApp

BEGIN_MESSAGE_MAP(CGraphLibApp, CWinApp)
END_MESSAGE_MAP()


// CGraphLibApp construction

CGraphLibApp::CGraphLibApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CGraphLibApp object

CGraphLibApp theApp;


// CGraphLibApp initialization

BOOL CGraphLibApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

BOOL GRAPHLIBAPI GraphlibPreTranslateMessage(MSG *pMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.PreTranslateMessage(pMsg);
}