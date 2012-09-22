
// LibElcBaseUI.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
//#include "studentvoicedlg.h"
#include "selectmemberdlg.h"
#include "StudentDemoListDlg.h"
#include "i18n.h"
#include "Objidl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


// CLibElcBaseUIApp

BEGIN_MESSAGE_MAP(CLibElcBaseUIApp, CWinApp)
END_MESSAGE_MAP()


// CLibElcBaseUIApp construction

CLibElcBaseUIApp::CLibElcBaseUIApp()
{
	gdiplusToken = NULL;
}


// The one and only CLibElcBaseUIApp object

CLibElcBaseUIApp	theApp;
CElcSkinDoc			theSkin;
CElcSkinPainter		thePainter;
CNCSkinBinding		theNCBinder;
CSkinWndBinder		theSkinWndBinder;

// CLibElcBaseUIApp initialization

BOOL CLibElcBaseUIApp::InitInstance()
{
	CWinApp::InitInstance();

	m_hResource = LoadResourceLibrary(_T("LibElcBaseUIRes.dll"));
	if (m_hResource) {
		AfxSetResourceHandle(m_hResource);
	}

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CString strSkinPath;
	GetL10NDirectory(strSkinPath);
	strSkinPath += _T("Skins\\SkinUI.xml");

	if (!theSkin.LoadFile(strSkinPath)) {
		//ElcMessageBox(IDS_ERROR_LOAD_SKIN, MB_OK|MB_ICONERROR);
		//return FALSE;
	}

	return TRUE;
}

int CLibElcBaseUIApp::ExitInstance()
{
	if (gdiplusToken)
		GdiplusShutdown(gdiplusToken);

	return CWinApp::ExitInstance();
}

/*
LIBELCBASEUI_API BOOL ElcBase_CreateStudentVoiceDlg(PSTUDV_PARAMS pParams)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!pParams)
		return FALSE;

	if (pParams->bModelless) 
	{
		CStudentVoiceDlg* pDlgStudentVoice = new CStudentVoiceDlg(pParams->pParent);
		if (!pDlgStudentVoice)
			return FALSE;

		pDlgStudentVoice->SetParams(pParams);
		if (!pDlgStudentVoice->Create(CStudentVoiceDlg::IDD, pParams->pParent)) 
		{
			delete pDlgStudentVoice;
			return FALSE;
		}

		pDlgStudentVoice->SetWindowPos(NULL, pParams->ptLeftTop.x, pParams->ptLeftTop.y, 
			0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
	else
	{
		CStudentVoiceDlg dlgStudentVoice(pParams->pParent);
		dlgStudentVoice.SetParams(pParams);
		dlgStudentVoice.DoModal();
	}

	return TRUE;
}

*/
LIBELCBASEUI_API BOOL ElcBase_CreateStudentCheckDlg(PSTUDC_PARAMS pParams, int nCount, PMEMBERINFO* ppMemberInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!pParams)
		return FALSE;

	if (pParams->bModelless)
	{
		CSelectMemberDlg* pDlgStudentSelect = new CSelectMemberDlg(pParams->pParent);
		if (!pDlgStudentSelect)
			return FALSE;

		pDlgStudentSelect->SetParams(pParams, nCount, ppMemberInfo);
		if (!pDlgStudentSelect->Create(CSelectMemberDlg::IDD, pParams->pParent)) 
		{
			delete pDlgStudentSelect;
			return FALSE;
		}

		pDlgStudentSelect->SetWindowPos(NULL, pParams->ptLeftTop.x, pParams->ptLeftTop.y, 
			0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

	} else
	{
		CSelectMemberDlg dlgStudentSelect(pParams->pParent);
		dlgStudentSelect.SetParams(pParams, nCount, ppMemberInfo);
		dlgStudentSelect.DoModal();
	}

	return TRUE;
}

LIBELCBASEUI_API BOOL ElcBase_CreateStudentDemoDlg(PSTUDD_PARAMS pParams)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!pParams)
		return FALSE;

	CStudentDemoListDlg dlgStudentDemo(pParams->pParent);
	dlgStudentDemo.SetParams(pParams);
	if (dlgStudentDemo.DoModal() != IDOK)
		return FALSE;

	return TRUE;
}

LIBELCBASEUI_API void ElcBase_ProcessDllIdle()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRY
	{
		long lCount = 0;
		while (AfxGetThread()->OnIdle(lCount))
			lCount++;
	}
	END_TRY
}

LIBELCBASEUI_API BOOL ElcBase_FilterDllMsg(MSG* pMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRY
	{
		return AfxGetThread()->PreTranslateMessage(pMsg);
	}
	END_TRY
	return FALSE;
}

LIBELCBASEUI_API BOOL GetStudentImage(CBitmap& bitmap)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	return bitmap.LoadBitmap(IDB_GROUP_LIST_STATE);
}

LIBELCBASEUI_API BOOL SetCtrlPos(UINT nPosId, CWnd* pWndCtrl, CWnd* pParent)
{
	CWnd* pWnd = pParent->GetDlgItem(nPosId);
	if (!pWnd)
		return FALSE;

	RECT rect;
	pWnd->GetWindowRect(&rect);
	pParent->ScreenToClient(&rect);
	pWndCtrl->MoveWindow(&rect);

	return TRUE;
}

LIBELCBASEUI_API BOOL LoadImageFile(LPCTSTR lpszFile, CBitmap* pBitmap, CSize& size)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!pBitmap || _taccess(lpszFile, 0) != 0)
		return FALSE;

	Bitmap bitmap(lpszFile);
	if (bitmap.GetLastStatus() != Ok)
		return FALSE;
	
	HBITMAP hBitmap;
	bitmap.GetHBITMAP(Color::White, &hBitmap);
	pBitmap->DeleteObject();
	if (!pBitmap->Attach(hBitmap))
		return FALSE;

	BITMAP bm;
	GetObject(pBitmap->m_hObject, sizeof(bm), &bm);

	size.cx = bitmap.GetWidth();
	size.cy = bitmap.GetHeight();

	return TRUE;
}

LIBELCBASEUI_API BOOL LoadImageFromBuffer(LPBYTE lpBuffer, int cbBuffer, CBitmap* pBitmap, CSize& size)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!lpBuffer || cbBuffer <= 0 || !pBitmap)
		return FALSE;

	IStream* pstm = NULL;
	Bitmap *pImage = NULL;
	BOOL bSuccessful = FALSE;
	PBYTE pMem = NULL;
	HGLOBAL hMem = NULL;
	
	do 
	{
		hMem = GlobalAlloc(GMEM_FIXED, cbBuffer);
		pMem = (BYTE*)GlobalLock(hMem);
		memcpy(pMem, lpBuffer, cbBuffer);
		CreateStreamOnHGlobal(hMem, FALSE, &pstm);

		pImage = Bitmap::FromStream(pstm);
		if (!pImage || pImage->GetLastStatus() != Ok)
			break;

		HBITMAP hBitmap = NULL;
		pImage->GetHBITMAP(Color::White, &hBitmap);
		pBitmap->DeleteObject();
		if (!pBitmap->Attach(hBitmap))
			break;

		BITMAP bm;
		GetObject(pBitmap->m_hObject, sizeof(bm), &bm);

		size.cx = pImage->GetWidth();
		size.cy = pImage->GetHeight();

		bSuccessful = TRUE;
	} while (0);

	if (hMem)
		GlobalUnlock(hMem);
	if (pstm)
		pstm->Release();
	if (pImage)
		delete pImage;

	return bSuccessful;
}

LIBELCBASEUI_API LPVOID LoadImageFromResource(UINT nRedId, LPCTSTR lpszType)
{
	return (LPVOID)ImageFromIDResource(nRedId, lpszType);
}

LIBELCBASEUI_API void ReleaseImage(LPVOID pImage)
{
	if (pImage)
		delete (Bitmap *)pImage;
}

LIBELCBASEUI_API CElcSkinDoc* GetSkinDoc() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return &theSkin;
}

LIBELCBASEUI_API CElcSkinPainter* GetPainter()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return &thePainter;
}

LIBELCBASEUI_API CNCSkinBinding* GetNCSkinBinder()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return &theNCBinder;
}

LIBELCBASEUI_API CSkinWndBinder* GetSkinWndBinder()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return &theSkinWndBinder;
}

LIBELCBASEUI_API BOOL CreateTransparentWnd(CWnd* pWnd, BYTE btAlpha, const RECT& rect, CWnd* pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!pWnd) {
		return FALSE;
	}
	else if (::IsWindow(pWnd->GetSafeHwnd())) {
		pWnd->ModifyStyle(WS_CHILD, WS_POPUP);
		pWnd->ModifyStyleEx(0, WS_EX_LAYERED);
		pWnd->SetLayeredWindowAttributes(0, btAlpha, LWA_ALPHA);
		return TRUE;
	}

	LPCTSTR pClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS);

	DWORD dwStyle = WS_POPUP | WS_VISIBLE;
	DWORD dwExStyle = WS_EX_LAYERED;
	if (!pWnd->CreateEx(dwExStyle, 
		pClassName, 
		_T(""), 
		dwStyle, 
		rect, 
		pParent, 
		0))
		return FALSE;

	pWnd->SetLayeredWindowAttributes(0, btAlpha, LWA_ALPHA);
	return TRUE;
}

