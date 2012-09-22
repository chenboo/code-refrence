#include "stdafx.h"
#include "DLG.h"
#include "DLGDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CDLGDlg dialog

CDLGDlg::CDLGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGDlg::IDD, pParent)
	, m_EditMsg(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bServer = FALSE;
}

void CDLGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_ListMsg);
	DDX_Text(pDX, IDC_EDIT_MSG, m_EditMsg);
}

BEGIN_MESSAGE_MAP(CDLGDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_RADIO_SEVER, &CDLGDlg::OnBnClickedRadioSever)
	ON_BN_CLICKED(IDC_RADIO_CLIENT, &CDLGDlg::OnBnClickedRadioClient)
	ON_BN_CLICKED(IDC_BTN_RUN, &CDLGDlg::OnBnClickedBtnRun)
	ON_BN_CLICKED(IDC_BTN_SEND, &CDLGDlg::OnBnClickedBtnSend)

	ON_MESSAGE(WM_ACCEPT,OnAccept)
	ON_MESSAGE(WM_RECEIVE,OnReceive)
	
END_MESSAGE_MAP()

// CDLGDlg message handlers

BOOL CDLGDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CheckRadioButton(IDC_RADIO_SEVER,IDC_RADIO_CLIENT,IDC_RADIO_SEVER);
	OnBnClickedRadioSever();

	m_InterSock.SetNotifyWnd(m_hWnd);
	m_InterAccept.SetNotifyWnd(m_hWnd);

	return TRUE;
}

void CDLGDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CDLGDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CDLGDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDLGDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);
}

void CDLGDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CDialog::OnMouseMove(nFlags, point);
}

void CDLGDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonUp(nFlags, point);
}

void CDLGDlg::OnBnClickedRadioSever()
{
	m_bServer = TRUE;
	CString str;
	str.LoadString(IDS_RUN);
	GetDlgItem(IDC_BTN_RUN)->SetWindowText(str);
}

void CDLGDlg::OnBnClickedRadioClient()
{
	m_bServer = FALSE;
	CString str;
	str.LoadString(IDS_LINK);
	GetDlgItem(IDC_BTN_RUN)->SetWindowText(str);
}

void CDLGDlg::OnBnClickedBtnRun()
{
	if(m_bServer)
	{
		m_InterSock.Create(2000);
		m_InterSock.Listen();
	}
	else
	{
		m_InterSock.Create();
		CString strAddress(_T("127.0.0.1"));
		m_InterSock.Connect(strAddress,2000);
	}

	GetDlgItem(IDC_BTN_RUN)->EnableWindow(FALSE);
}

LRESULT CDLGDlg::OnAccept(WPARAM wParam,LPARAM lParam)
{
	m_InterSock.Accept(m_InterAccept);
	return 0;
}

LRESULT CDLGDlg::OnReceive(WPARAM wParam,LPARAM lParam)
{
	CString strReceive;
	char* pch = new char[1025];

	if(m_bServer)
	{
		m_InterAccept.Receive(pch,1024);

		int dwCount = MultiByteToWideChar( CP_UTF8, 0, pch, -1, 0, 0 );
		TCHAR* pTch = new TCHAR[dwCount];
		memset(pTch, 0, dwCount * sizeof(TCHAR));
		MultiByteToWideChar( CP_UTF8, 0, pch, -1, pTch, dwCount);

		m_ListMsg.AddString(_T("客户机:"));
		m_ListMsg.AddString(pTch);

		delete[] pTch;
	}
	else
	{
		m_InterSock.Receive(pch,1024);

		int dwCount = MultiByteToWideChar( CP_UTF8, 0, pch, -1, 0, 0 );
		TCHAR* pTch = new TCHAR[dwCount];
		memset(pTch, 0, dwCount * sizeof(TCHAR));
		MultiByteToWideChar( CP_UTF8, 0, pch, -1, pTch, dwCount);

		m_ListMsg.AddString(_T("服务器:"));
		m_ListMsg.AddString(pTch);

		delete[] pTch;
	}

	return 0;
}

void CDLGDlg::OnBnClickedBtnSend()
{
	UpdateData();
	m_EditMsg.TrimLeft();
	if(m_EditMsg.IsEmpty())
	{
		AfxMessageBox(IDS_NOEMPTY);
		return;
	}

	DWORD dwNum = WideCharToMultiByte(CP_UTF8,NULL,m_EditMsg,-1,NULL,0,NULL,NULL);
	char* pch = new char[dwNum];
	WideCharToMultiByte(CP_UTF8,NULL,m_EditMsg,-1,pch,dwNum,NULL,NULL);

	if(m_bServer)
	{
		if(SOCKET_ERROR == m_InterAccept.Send(pch,dwNum))
		{
			AfxMessageBox(IDS_SEND_FAILED);
			return;
		}
	}

	else
	{
		if(SOCKET_ERROR == m_InterSock.Send(pch,dwNum))
		{
			AfxMessageBox(IDS_SEND_FAILED);
			return;
		}
	}

	m_ListMsg.AddString(_T("自己:"));
	m_ListMsg.AddString(m_EditMsg);
	m_EditMsg.Empty();
	UpdateData(FALSE);

	delete[] pch;
}
