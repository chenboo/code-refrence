// ChatRoomClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChatRoomClient.h"
#include "ChatRoomClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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

// CChatRoomClientDlg 对话框

CChatRoomClientDlg::CChatRoomClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatRoomClientDlg::IDD, pParent)
	, m_strMsg(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hThread = NULL;
}

CChatRoomClientDlg::~CChatRoomClientDlg()
{
	closesocket(m_socket);
	WSACleanup();
}

void CChatRoomClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SHOW, m_ListMsg);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_strMsg);
}

BEGIN_MESSAGE_MAP(CChatRoomClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_LINK, &CChatRoomClientDlg::OnBnClickedBtnLink)
	ON_BN_CLICKED(IDC_BTN_SEND, &CChatRoomClientDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CChatRoomClientDlg 消息处理程序

BOOL CChatRoomClientDlg::OnInitDialog()
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

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	if(!InitWinsock())
	{
		return FALSE;
	}

	GetDlgItem(IDC_EDIT_INPUT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BOOL CChatRoomClientDlg::InitWinsock()
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
	
	return TRUE;
}

void CChatRoomClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CChatRoomClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CChatRoomClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChatRoomClientDlg::OnBnClickedBtnLink()
{
	m_hThread = CreateThread(NULL, 0, CLIThreadProc, this, 0, NULL);
}

DWORD WINAPI CLIThreadProc(LPVOID pParam)
{
	CChatRoomClientDlg* pDlg = (CChatRoomClientDlg*)pParam;
	ASSERT(pDlg != NULL);

	pDlg->m_socket = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	if ( pDlg->m_socket == INVALID_SOCKET ) 
	{
		AfxMessageBox(IDS_ACCEPT_FAIL);
		return FALSE;
	}

	//get the port number
	int nPort = pDlg->GetDlgItemInt(IDC_EDIT_PORT);
	if ( nPort <= 0 || nPort > 65535 ) 
	{
		AfxMessageBox(IDS_INPUT_VALID_PORT);
		closesocket(pDlg->m_socket);
		return FALSE;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(nPort);

	if(SOCKET_ERROR == connect(pDlg->m_socket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR)))
	{
		AfxMessageBox(IDS_LINK_FAIL);
		closesocket(pDlg->m_socket);
		return FALSE;
	}

	CString str;
	str.LoadString(IDS_LINK_SUCCESS);
	pDlg->m_ListMsg.AddString(str);
	pDlg->GetDlgItem(IDC_BTN_LINK)->EnableWindow(FALSE);

	(pDlg->GetDlgItem(IDC_EDIT_INPUT))->EnableWindow(TRUE);
	(pDlg->GetDlgItem(IDC_BTN_SEND))->EnableWindow(TRUE);

	char szBuf[100] = {0};
	
	while(1)
	{
		if(SOCKET_ERROR == recv(pDlg->m_socket,szBuf,100,0))
		{
			if(SOCKET_ERROR == connect(pDlg->m_socket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR)))
			{
				AfxMessageBox(IDS_LINK_FAIL);
				closesocket(pDlg->m_socket);
				return FALSE;
			}
			continue;
		}

		int nCount = MultiByteToWideChar( CP_UTF8, 0, szBuf, -1, 0, 0 );
		TCHAR* pch = new TCHAR[nCount];
		memset( pch, 0, nCount * sizeof(TCHAR) );
		MultiByteToWideChar( CP_UTF8, 0, szBuf, -1, pch, nCount);

		CString strmsg;
		strmsg.Format(_T("SEVER : %s"),pch);
		pDlg->m_ListMsg.AddString(strmsg);

		delete[] pch;
	}

	return TRUE;
}

void CChatRoomClientDlg::OnBnClickedBtnSend()
{
	UpdateData(TRUE);

	if(m_strMsg.IsEmpty())
	{
		AfxMessageBox(IDS_EMPTY_INVALID);
		return;
	}

	DWORD dwNum = WideCharToMultiByte(CP_UTF8,NULL,m_strMsg,-1,NULL,0,NULL,NULL);
	char* pch = new char[dwNum];
	WideCharToMultiByte(CP_UTF8,NULL,m_strMsg,-1,pch,dwNum,NULL,NULL);

	send(m_socket,pch,100,0);

	CString str;
	str.Format(_T("SELF:%s"),m_strMsg);
	m_ListMsg.AddString(str);

	m_strMsg = _T("");
	UpdateData(FALSE);

	delete pch;
}
