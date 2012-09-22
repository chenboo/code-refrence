// ChatRoomSrvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChatRoomSrv.h"
#include "ChatRoomSrvDlg.h"

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


CChatRoomSrvDlg::CChatRoomSrvDlg(CWnd* pParent /*=NULL*/)
: CDialog(CChatRoomSrvDlg::IDD, pParent)
, m_strInput(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hThread = NULL;
}

CChatRoomSrvDlg::~CChatRoomSrvDlg()
{
	closesocket(m_soketSrv);
	closesocket(m_soketCon);
	WSACleanup();
}

void CChatRoomSrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SHOW, m_ListMsg);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_strInput);
}

BEGIN_MESSAGE_MAP(CChatRoomSrvDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_START, &CChatRoomSrvDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_SEND, &CChatRoomSrvDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()

// CChatRoomSrvDlg message handlers

BOOL CChatRoomSrvDlg::OnInitDialog()
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
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if(!InitWinsock())
	{
		return FALSE;
	}

	GetDlgItem(IDC_EDIT_INPUT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);

	return TRUE;
}

BOOL CChatRoomSrvDlg::InitWinsock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		return FALSE;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		WSACleanup( );
		return FALSE; 
	}
}

void CChatRoomSrvDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CChatRoomSrvDlg::OnPaint()
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

HCURSOR CChatRoomSrvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChatRoomSrvDlg::OnBnClickedBtnStart()
{
	m_hThread = CreateThread(NULL, 0, SRVThreadProc, this, 0, NULL);
}

void CChatRoomSrvDlg::OnBnClickedBtnSend()
{
	UpdateData(TRUE);

	if(m_strInput.IsEmpty())
	{
		AfxMessageBox(IDS_EMPTY_INVALID);
		return;
	}

	DWORD dwNum = WideCharToMultiByte(CP_UTF8,NULL,m_strInput,-1,NULL,0,NULL,NULL);
	char* pch = new char[dwNum];
	WideCharToMultiByte(CP_UTF8,NULL,m_strInput,-1,pch,dwNum,NULL,NULL);

	send(m_soketCon,pch,100,0);

	CString str;
	str.Format(_T("SELF:%s"),m_strInput);
	m_ListMsg.AddString(str);

	m_strInput = _T("");
	UpdateData(FALSE);

	delete pch;
}

DWORD WINAPI SRVThreadProc(LPVOID pParam)
{
	int nPort = 0;
	sockaddr_in addrclt;
	CString strMsg;
	CChatRoomSrvDlg* pDlg = (CChatRoomSrvDlg*)pParam;
	ASSERT(pDlg != NULL);

	if(!CreateSrvSocket(pDlg))
	{
		return FALSE;
	}

	if(!GetPortNumber(pDlg,nPort))
	{
		return FALSE;
	}

	if(!BindAddress(pDlg->m_soketSrv,nPort))
	{
		return FALSE;
	}

	if(!ListenSocket(pDlg->m_soketSrv,pDlg->m_soketCon))
	{
		return FALSE;
	}

	CString str;
	str.LoadString(IDS_START_SUCCESS);
	pDlg->m_ListMsg.AddString(str);
	pDlg->GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
	(pDlg->GetDlgItem(IDC_EDIT_INPUT))->EnableWindow(TRUE);
	(pDlg->GetDlgItem(IDC_BTN_SEND))->EnableWindow(TRUE);

	if(!AcceptSocket(pDlg->m_soketSrv,pDlg->m_soketCon,addrclt))
	{
		return FALSE;
	}

	while(1)
	{
		strMsg = ReceiveMessage(pDlg->m_soketSrv,pDlg->m_soketCon,addrclt);
		if(strMsg.IsEmpty())
		{
			int nLen = sizeof(sockaddr_in);
			pDlg->m_soketCon = accept(pDlg->m_soketSrv, (struct sockaddr *)&addrclt , &nLen);
			if(pDlg->m_soketCon == INVALID_SOCKET) 
			{
				AfxMessageBox(IDS_ACCEPT_FAIL);
				closesocket(pDlg->m_soketSrv);
				closesocket(pDlg->m_soketCon);
				return FALSE;
			}
		}
		else
		{
			pDlg->m_ListMsg.AddString(strMsg);
		}
	}
	return TRUE;
}

BOOL CreateSrvSocket(CChatRoomSrvDlg* pDlg)
{
	ASSERT(pDlg != NULL);
	pDlg->m_soketSrv = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	if ( pDlg->m_soketSrv == INVALID_SOCKET ) 
	{
		AfxMessageBox(IDS_CREATE_FAIL);
		return FALSE;
	}
	return TRUE;
}

BOOL GetPortNumber(CChatRoomSrvDlg* pDlg,int& nPort)
{
	nPort = pDlg->GetDlgItemInt(IDC_EDIT_PORT);
	if ( nPort <= 0 || nPort > 65535 ) 
	{
		AfxMessageBox(IDS_INPUT_VALID_PORT);
		closesocket(pDlg->m_soketSrv);
		closesocket(pDlg->m_soketCon);
		return FALSE;
	}
	return TRUE;
}

BOOL BindAddress(SOCKET& socketSrv,int nPort)
{
	sockaddr_in addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = INADDR_ANY;
	addrSrv.sin_port = htons(nPort);
	if (bind(socketSrv, (sockaddr*)&addrSrv, sizeof(sockaddr_in)) == SOCKET_ERROR )
	{
		AfxMessageBox(IDS_BIND_FAIL);
		closesocket(socketSrv);
		return FALSE;
	}
	return TRUE;
}

BOOL ListenSocket(SOCKET& socketSrv,SOCKET& socketCon)
{
	if(listen(socketSrv, 5) == SOCKET_ERROR )
	{
		AfxMessageBox(IDS_LISTEN_FAIL);
		closesocket(socketSrv);
		closesocket(socketCon);
		return FALSE;
	}
	return TRUE;
}

BOOL AcceptSocket(SOCKET& socketSrv,SOCKET& socketCon,sockaddr_in& addrclt)
{
	int nLen = sizeof(sockaddr_in);
	socketCon = accept(socketSrv, (struct sockaddr *)&addrclt , &nLen);

	if(socketCon == INVALID_SOCKET) 
	{
		AfxMessageBox(IDS_ACCEPT_FAIL);
		closesocket(socketSrv);
		closesocket(socketCon);
		return FALSE;
	}
}

CString ReceiveMessage(SOCKET& socketSrv,SOCKET& socketCon,sockaddr_in& addrclt)
{
	char szBuf[100] = {0};
	if(SOCKET_ERROR == recv(socketCon,szBuf,100,0))
	{
		return NULL;
	}

	char* pchar = inet_ntoa(addrclt.sin_addr);
	int nCount = MultiByteToWideChar( CP_UTF8, 0, pchar, -1, 0, 0 );
	TCHAR* pTCHAR = new TCHAR[nCount];
	memset(pTCHAR, 0, nCount * sizeof(TCHAR));
	MultiByteToWideChar(CP_UTF8, 0, pchar, -1, pTCHAR, nCount);

	nCount = MultiByteToWideChar( CP_UTF8, 0, szBuf, -1, 0, 0 );
	TCHAR* pch = new TCHAR[nCount];
	memset( pch, 0, nCount * sizeof(TCHAR));
	MultiByteToWideChar(CP_UTF8, 0, szBuf, -1, pch, nCount);

	CString strmsg;
	strmsg.Format(_T("%s: %s"),pTCHAR,pch);

	delete []pTCHAR;
	delete []pch;

	return strmsg;
}