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