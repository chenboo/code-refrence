// SkinDlgTest.cpp : implementation file
//

#include "stdafx.h"
#include "TestBaseUI.h"
#include "SkinDlgTest.h"


// CSkinDlgTest dialog

IMPLEMENT_DYNAMIC(CSkinDlgTest, CDialog)

CSkinDlgTest::CSkinDlgTest(CWnd* pParent /*=NULL*/)
	: CDialog(CSkinDlgTest::IDD, pParent)
{

}

CSkinDlgTest::~CSkinDlgTest()
{
}

void CSkinDlgTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST1, m_wndList);
}


BEGIN_MESSAGE_MAP(CSkinDlgTest, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON1, &CSkinDlgTest::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSkinDlgTest::OnBnClickedButton2)

	ON_MESSAGE(WM_ELC_UI_DRAW_NOTIFY, OnElcUIPredrawNotify)
END_MESSAGE_MAP()


// CSkinDlgTest message handlers

void CSkinDlgTest::PreInitDialog()
{

	CDialog::PreInitDialog();
}

BOOL CSkinDlgTest::OnInitDialog()
{
	CDialog::OnInitDialog();

	//ModifyStyleEx(0, WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 123, LWA_ALPHA);

	//AdjustImagePanePos(IDC_IMAGE_PANE);

		DWORD dwStyle = WS_CHILD
		| WS_VISIBLE
		| LVS_REPORT 
		| LVS_SHAREIMAGELISTS
		| LVS_SHOWSELALWAYS
		| LVS_EDITLABELS;
			  
	DWORD dwExStyle = LVS_EX_INFOTIP | 
				LVS_EX_FULLROWSELECT |  
				LVS_EX_DOUBLEBUFFER | 
				LVS_EX_HEADERDRAGDROP |
				LVS_EX_LABELTIP;

	m_wndList.Create(
		dwStyle,
		CRect(0,0,200,200),
		this,
		0);

	m_wndList.SetExtendedStyle(dwExStyle);


	m_wndList.InsertColumn(0, _T("colum 1"), 0, 100);

	GetDlgItem(IDC_EDIT1)->SetWindowText(_T("edit"));




	//GetNCSkinBinder()->BindWindow(m_wndList.GetSafeHwnd(), ELC_SKIN_STYLE_PANE_CONTROL_INNER);
	GetNCSkinBinder()->BindWindow(m_wndList.GetSafeHwnd(), ELC_SKIN_STYLE_PANE_CHILD);

	for (int i=0; i < 15; i ++) {
		CString strItem;
		strItem.Format(_T("Item %d"), i + 1);
		m_wndList.InsertItem(m_wndList.GetItemCount(), strItem);
	}

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// Set big icon
	SetIcon(hIcon, FALSE);		// Set small icon

	SetWindowText(_T("Test Caption Long"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSkinDlgTest::OnEraseBkgnd(CDC* pDC)
{
	CRect rc1, rc2;
	m_wndList.GetClientRect(rc1);
	m_wndList.GetWindowRect(rc2);
	m_wndList.ScreenToClient(&rc2);

	return CDialog::OnEraseBkgnd(pDC);
}

void CSkinDlgTest::OnBnClickedButton1()
{
	CString strItem;
	strItem.Format(_T("Item %d"), m_wndList.GetItemCount() + 1);
	m_wndList.InsertItem(m_wndList.GetItemCount(), strItem);
}

void CSkinDlgTest::OnBnClickedButton2()
{
	if (::IsWindow(m_SkinPopWnd.GetSafeHwnd())) {
		m_SkinPopWnd.BringWindowToTop();
		m_SkinPopWnd.SetForegroundWindow();
		return ;
	}

	LPCTSTR pClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS);
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	DWORD dwExStyle = 0;//WS_EX_TOPMOST;
	CRect rc(0,0,600,400);

	m_SkinPopWnd.CreateEx(dwExStyle, pClassName, _T(""), dwStyle, rc, this,0);

	m_SkinPopWnd.BringWindowToTop();
	m_SkinPopWnd.CenterWindow();
	m_SkinPopWnd.ShowWindow(SW_SHOW);
}

LRESULT CSkinDlgTest::OnElcUIPredrawNotify(WPARAM wParam, LPARAM lParam)
{
	PELC_UI_DRAW pPreDraw = (PELC_UI_DRAW)lParam;

	if (wParam == IDC_BUTTON1) {
		CBrush br(RGB(255,255,255));
		::FillRect(pPreDraw->hdc, &pPreDraw->rect, (HBRUSH)br.GetSafeHandle());
		return 1;
	}
	else if (wParam == IDC_BUTTON2) {
		CBrush br(RGB(255,0,0));
		::FillRect(pPreDraw->hdc, &pPreDraw->rect, (HBRUSH)br.GetSafeHandle());
		return 1;
	}
	

	return 0;
}
