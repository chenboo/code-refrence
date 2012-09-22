
#include "stdafx.h"
#include "Drawer.h"
#include "DrawerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDrawerApp

BEGIN_MESSAGE_MAP(CDrawerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

HHOOK CDrawerApp::m_hMhook = NULL;
BOOL CDrawerApp::m_bNcLButtonDown = NULL;

// CDrawerApp construction

CDrawerApp::CDrawerApp()
{

}

CDrawerApp theApp;

// CDrawerApp initialization

BOOL CDrawerApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();
	SetRegistryKey(_T("PENG"));

	HKEY hKey;
	LONG lRet = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		_T("Software\\PENG\\Drawer\\USEAGE"),
		0,
		KEY_QUERY_VALUE,
		&hKey);

	if(lRet != ERROR_SUCCESS)
	{
		CreateSoftWareInfomation();
	}
	else if(!IsOutOfDate())
	{
		return FALSE;
	}

	CDrawerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{

	}

	return FALSE;
}

int CDrawerApp::ExitInstance()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	if(m_hMhook != NULL)
	{
		UnhookWindowsHookEx(m_hMhook);
	}

	return CWinApp::ExitInstance();
}

LRESULT CALLBACK CDrawerApp::MouseHookProc( int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode == HC_ACTION)
	{
		PMOUSEHOOKSTRUCT mhs = (PMOUSEHOOKSTRUCT) lParam;
		switch(wParam)
		{
		case WM_NCLBUTTONDOWN:
			if(mhs->wHitTestCode == HTCAPTION)
			{
				m_bNcLButtonDown = TRUE;
			}
			break;
		case WM_NCLBUTTONUP:
			m_bNcLButtonDown = FALSE;
			break;
		case WM_LBUTTONUP:
			if(m_bNcLButtonDown == TRUE)
			{
				::PostMessage(mhs->hwnd,WM_NCLBUTTONUP,
							HTCAPTION, 
							MAKELONG(mhs->pt.x,mhs->pt.y));
				m_bNcLButtonDown = FALSE;
			}
			break;
		default:
			break;
		}
	}
	
	return CallNextHookEx(m_hMhook, nCode, wParam, lParam); 
}

BOOL CDrawerApp::PreTranslateMessage(MSG* pMsg)
{
	if(GraphlibPreTranslateMessage(pMsg))
	{
		return TRUE;
	}

	int iResult;
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		iResult = TranslateAccelerator(m_pMainWnd->m_hWnd,m_hAcc,pMsg);
		if(iResult)
		{
			return TRUE;
		}
		break;
	default :
		break;
	}

	return CWinApp::PreTranslateMessage(pMsg);
}

void CDrawerApp::CreateSoftWareInfomation()
{
	CTime t = CTime::GetCurrentTime();

	int nYear = t.GetYear();
	int nMonth = t.GetMonth();
	int nDay = t.GetDay();

	AfxGetApp()->WriteProfileInt(_T("USEAGE"), _T("SYear"), nYear);
	AfxGetApp()->WriteProfileInt(_T("USEAGE"), _T("SMonth"), nMonth);
	AfxGetApp()->WriteProfileInt(_T("USEAGE"), _T("SDay"), nDay);

	//AfxGetApp()->WriteProfileInt(_T("USEAGE"), _T("LYear"), nYear);
	//AfxGetApp()->WriteProfileInt(_T("USEAGE"), _T("LMonth"), nMonth);
	//AfxGetApp()->WriteProfileInt(_T("USEAGE"), _T("LDay"), nDay);
}

BOOL CDrawerApp::IsOutOfDate()
{
	//int nLYear = AfxGetApp()->GetProfileInt(_T("USEAGE"), _T("LYear"), 2011);
	//int nLMonth = AfxGetApp()->GetProfileInt(_T("USEAGE"), _T("LMonth"), 1);
	//int nDay = AfxGetApp()->GetProfileInt(_T("USEAGE"), _T("LDay"), 1);

	CTime deadlineTime( 2011, 3, 1, 0, 0, 0 ); // 10:15PM March 19, 1999
	CTime currentTime = CTime::GetCurrentTime();

	if(currentTime >= deadlineTime)
	{
		AfxMessageBox(_T("试用期结束，感谢您的使用"));
		return FALSE;
	}
	else
	{
		CTimeSpan tmSpan = deadlineTime - currentTime;
		CString str;
		str.Format(_T("剩余试用天数: %d 天"), tmSpan.GetDays());
		AfxMessageBox(str);
		return TRUE;
	}
}