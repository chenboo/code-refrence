//comlayer中主要用到了单播和组播的技术
//涉及到的库有LibBaseTrans、LibComlayer以及
//封装好的libNet, LibNet30d


//这里以一个组播的过程为例来说明一下, 我建立了一个对话框工程
//首先自然要将使用的库添加到工程中去，添加库的依赖，以及头文件的包含

//主要的代码如下：
//DLGDlg.h文件中:
#pragma once

class CDLGDlg : public CDialog,
	public IEventManagerNotify,
	public IFrameAssembler::ICallback
{
public:
	CDLGDlg(CWnd* pParent = NULL);	// standard constructor
	enum { IDD = IDD_DLG_DIALOG };

protected:
	BOOL Startup();
	void Shutdown();

	//IFrameAssembler::ICallback Interface
	virtual void OnFrame(LPCVOID pvFrame, int cbFrame, DWORD dwParam);//用来接收通过组播发来的数据
	virtual void OnNewSession(DWORD dwSessionID, DWORD dwParam){};
	virtual void OnIncompleteFrame(LPCVOID pvFrame, int cbFrame, DWORD dwParam){};

	//IEventManagerNotify interface
	virtual void OnEventArrived(LPVOID lpvUserValue){};//用来接收通过单播发来的数据，这里暂时未使用

protected:
	HICON							m_hIcon;

	IEventItemManager*				m_pEventMgr;	//管理单播的manager
	ICommunicationLayer*			m_pComLayer;	//用来管理单播的comlayer
	IReliableMulticastSender *		m_pSBDSender;	//用来发送组播数据
	IReliableMulticastReceiver *	m_pSBDReceiver;	//用来接收组播数据，主要是将本机加入组播
	IFrameAssembler *				m_pSBDAssembler;

	
	//一些结构体，用来设置一些网络参数
	typedef struct tagSHAREBOARDPARAMS
	{
		DWORD		dwCmdIpAddress;
		WORD		wCmdPort;
		DWORD		dwDataIpAddress;
		WORD		wDataPort;
		float		fBitrateKbps;
		float		fWindowDurationSeconds;
		float		fRedundancyRate;
		WORD		wMaxPayloadSize; 
		WORD		wNumDataPacketsInGroup;
	}SHAREBOARDPARAMS, *PSHAREBOARDPARAMS;

	typedef struct SBSERVER_PARAM
	{
		HWND		hWndNotify;
		UINT		nNotifyMsg;
		TCHAR		szName[32];
		DWORD		dwLocalInterface;
		SHAREBOARDPARAMS	params;
	}*PSBSERVER_PARAM;

	SBSERVER_PARAM					m_param;//网络参数

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};

//DLGDlg.cpp文件主要是实现
	BOOL Startup();
	void Shutdown();
	virtual void OnFrame(LPCVOID pvFrame, int cbFrame, DWORD dwParam);
//这三个函数，源文件如下：
#include "stdafx.h"
#include "DLG.h"
#include "DLGDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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

CDLGDlg::CDLGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDLGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDLGDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SEND, &CDLGDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


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

	Startup();
	return TRUE;  // return TRUE  unless you set the focus to a control
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

BOOL CDLGDlg::Startup()
{
	do {
		CSocketUtils::Initialize();

		m_param.dwLocalInterface = 0xb800a8c0;
		m_param.params.dwCmdIpAddress = 224527073;
		m_param.params.wCmdPort = 54700;
		m_param.params.dwDataIpAddress = 207749857;
		m_param.params.wDataPort = 54697;
		m_param.params.fBitrateKbps = 4096;
		m_param.params.fWindowDurationSeconds = 10;
		m_param.params.fRedundancyRate = 1.2f;
		m_param.params.wMaxPayloadSize = 1440;
		m_param.params.wNumDataPacketsInGroup = 32;

		// start comlayer
		m_pEventMgr = CreateEventManager(5 * 1024 * 1024);
		if(!m_pEventMgr)
		{
			break;
		}

		m_pEventMgr->SetEventArrivedCallback(this, 0);
		m_pComLayer = CreateCommunicationLayer(m_pEventMgr);
		if(!m_pComLayer)
		{
			break;
		}

		if(!m_pComLayer->Startup(m_param.dwLocalInterface, m_param.params.dwCmdIpAddress, m_param.params.wCmdPort))
		{
			break;
		}

		//Multicast network
		m_pSBDSender = RMCreateSender();
		if (NULL == m_pSBDSender)
			break;

		if (!m_pSBDSender->JoinSession(
			InetDwordToString(m_param.dwLocalInterface),
			InetDwordToString(m_param.params.dwDataIpAddress),
			m_param.params.wDataPort,
			m_param.params.fBitrateKbps,
			m_param.params.fWindowDurationSeconds,
			TRUE,
			FALSE,
			m_param.params.fRedundancyRate,
			m_param.params.wMaxPayloadSize,
			m_param.params.wNumDataPacketsInGroup))
			break;

		//receiver
		m_pSBDAssembler = RMCreateFrameAssembler(2 * 1024 * 1024, this, 0);
		if (!m_pSBDAssembler)
			break;
		//create the receiver
		m_pSBDReceiver = RMCreateReceiver(m_pSBDAssembler->GetReceiverCallback(), 0);
		if (!m_pSBDReceiver)
			break;

		if (!m_pSBDReceiver->JoinSession(
			InetDwordToString(m_param.dwLocalInterface),
			InetDwordToString(m_param.params.dwDataIpAddress),
			m_param.params.wDataPort, 
			m_param.params.fBitrateKbps,
			m_param.params.fWindowDurationSeconds,
			m_param.params.wMaxPayloadSize,
			m_param.params.wNumDataPacketsInGroup))
			break;

		return TRUE;
	} while (0);

	Shutdown();
	return FALSE;
}

void CDLGDlg::Shutdown()
{
	//shutdown Multicast network
	if (m_pSBDReceiver)
	{
		m_pSBDReceiver->LeaveSession();
		m_pSBDReceiver->Delete();
	}

	if (m_pSBDAssembler)
	{
		m_pSBDAssembler->Delete();
	}

	if (m_pSBDSender)
	{
		m_pSBDSender->LeaveSession();
		m_pSBDSender->Delete();
		m_pSBDSender = NULL;
	}

	//shutdown unicast
	if (m_pEventMgr)
	{
		m_pEventMgr->Release();
		m_pEventMgr = NULL;
	}

	if (m_pComLayer)
	{
		m_pComLayer->Shutdown();
		m_pComLayer->Release();
		m_pComLayer = NULL;
	}

	CSocketUtils::Uninitialize();

}
void CDLGDlg::OnDestroy()
{
	Shutdown();
	CDialog::OnDestroy();
}

void CDLGDlg::OnBnClickedBtnSend()
{
	char sz[] = {"peng shui ming"};
	m_pSBDSender->SendFrame(sz, strlen(sz), INFINITE);
}

void CDLGDlg::OnFrame(LPCVOID pvFrame, int cbFrame, DWORD dwParam)
{
	char* pdata = new char[cbFrame + 1];
	memset(pdata, 0, cbFrame + 1);
	memcpy(pdata, pvFrame, cbFrame);

	CA2W str(pdata);
	MessageBox(str);
	delete[] pdata;
}

//完。


