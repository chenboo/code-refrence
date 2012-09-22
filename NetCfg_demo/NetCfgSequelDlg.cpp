// NetCfgSequelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetCfg - The Sequel.h"
#include "NetCfgSequelDlg.h"
#include "netadapter.h"
#include "DetailsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_TRAY_MESSAGE		WM_USER + 1

#define RANGE_ENSURE( nVal, nMin, nMax )	\
	if( nVal < nMin ) {	\
		nVal = nMin;	\
	}else{	\
		if( nVal > nMax ) {	\
			nVal = nMax;	\
		}	\
	}

BEGIN_MESSAGE_MAP(CNetCfgDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()	
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_HIDE, OnBnClickedBtnHide)	
	ON_MESSAGE( WM_TRAY_MESSAGE, OnTrayNotification )
	ON_COMMAND( IDM_TRAY_RESTORE, OnTrayMsgRestore )
	ON_WM_VSCROLL()		
	ON_BN_CLICKED(IDC_BTN_SHOWDETAILS, OnBnClickedBtnShowDetails)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ADPTRS, OnDeltaposSpinAdptrs)
	ON_BN_CLICKED(IDC_BTN_RELEASE, OnBnClickedBtnRelease)
	ON_BN_CLICKED(IDC_BTN_RENEW, OnBnClickedBtnRenew)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////
//	Desc:
//		Performs initialization and primary setup.
/////////////////////////////////////////////////////////
CNetCfgDlg::CNetCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetCfgDlg::IDD, pParent), m_tray( 1090 )
{
	m_hIcon		= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAdapters	= NULL;
	m_nDisplay	= 0;
	return;
}

/////////////////////////////////////////////////////////
//	Desc:
//		Cleans up any memory being used and detaches
//		our color control classes from their attached
//		windows the on the dialog.
/////////////////////////////////////////////////////////
BOOL CNetCfgDlg::DestroyWindow() {
	if( m_pAdapters ) {
		delete [] m_pAdapters;
		m_pAdapters = NULL;
	}

	return CDialog::DestroyWindow();
}

/////////////////////////////////////////////////////////
//	Desc:
//		Handles the initial setup of all the dialog
//		controls present.  All builds the list
//		of network adapters to be used.
/////////////////////////////////////////////////////////
BOOL CNetCfgDlg::OnInitDialog() {
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	
	m_exit.SubclassDlgItem( IDOK, this );
	m_exit.SetIcon( IDI_IC_OKACTIVE, IDI_IC_OKINACTIVE );
	m_exit.SetThemeHelper( &m_ThemeHelper );
	
	m_hide.SubclassDlgItem( IDC_BTN_HIDE, this );
	m_hide.SetIcon( IDI_IC_HIDEACTIVE, IDI_IC_HIDENOTACTIVE );
	m_hide.SetThemeHelper( &m_ThemeHelper );

	m_detailsbtn.SubclassDlgItem( IDC_BTN_SHOWDETAILS, this );
	m_detailsbtn.SetIcon( IDI_IC_WORLD_CONNECT );
	m_detailsbtn.SetThemeHelper( &m_ThemeHelper );

	m_renew.SubclassDlgItem( IDC_BTN_RELEASE, this );
	m_renew.SetIcon( IDI_IC_RELEASE );
	m_renew.SetThemeHelper( &m_ThemeHelper );

	m_release.SubclassDlgItem( IDC_BTN_RENEW, this );
	m_release.SetIcon( IDI_IC_RENEW );
	m_release.SetThemeHelper( &m_ThemeHelper );

	BuildAdapterListing();

	m_tray.SetNotifyWnd( this, WM_TRAY_MESSAGE );
	m_tray.SetIcon( IDR_MAINFRAME );
	m_tray.SetMenu( IDR_TRAY_MENU );

	CDialog::OnInitDialog();	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////
//	Desc:
//		Used to build the list of adapters.  Must be 
//		called again after each call that modifies 
//		the adapters information.  Currently this includes
//		the renewing and releasing of adapter information.
/////////////////////////////////////////////////////////
BOOL CNetCfgDlg::BuildAdapterListing() {
	BOOL	bRet		= TRUE;
	DWORD	dwErr		= 0;
	ULONG	ulNeeded	= 0;

	dwErr = EnumNetworkAdapters( m_pAdapters, 0, &ulNeeded );
	if( dwErr == ERROR_INSUFFICIENT_BUFFER ) {		
		m_nCount	= ulNeeded / sizeof( CNetworkAdapter );
		m_pAdapters = new CNetworkAdapter[ ulNeeded / sizeof( CNetworkAdapter ) ];		
		dwErr		= EnumNetworkAdapters( m_pAdapters, ulNeeded, &ulNeeded );		
		if( ! m_pAdapters ) {
			AfxMessageBox( _T("No Network Adapters Found on System."), MB_OK | MB_ICONERROR );
			::SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
		}		
	}else{
		AfxMessageBox( _T("No Network Adapters Found on System."), MB_OK | MB_ICONERROR );
		::SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
		bRet = FALSE;
	}

	CSpinButtonCtrl* pSpin = ( CSpinButtonCtrl* )GetDlgItem( IDC_SPIN_ADPTRS );

	if( m_nCount > 1 ) {
		pSpin->SetRange32( 0, m_nCount - 1 );
		pSpin->SetPos32( m_nDisplay );
	}else{
		pSpin->EnableWindow( FALSE );
	}

	return bRet;
}

/////////////////////////////////////////////////////////
//	Desc:
//		Moves data to the dialog controls from the currently
//		selected adapter.
/////////////////////////////////////////////////////////
void CNetCfgDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	CString sType,
			sIP,
			sDesc,
			sSub,
			sGate,
			sDns1,
			sDns2;

	CNetworkAdapter* pAdapt = NULL;

	if( ! pDX->m_bSaveAndValidate ) {
		if( m_pAdapters ) {			
			pAdapt = &m_pAdapters[ m_nDisplay ];
			( pAdapt->IsDhcpUsed() ) ? sType = _T("Dynamic Address (DHCP)") : sType = _T("Static IP");
			
			sIP = pAdapt->GetIpAddr().c_str();
			sDesc = pAdapt->GetAdapterDescription().c_str();
			sSub = pAdapt->GetSubnetForIpAddr().c_str();
			sGate = pAdapt->GetGatewayAddr().c_str();
			sDns1 = pAdapt->GetDnsAddr( 0 ).c_str();
			sDns2 = pAdapt->GetDnsAddr( 1 ).c_str();

			DDX_Text( pDX, IDC_STATIC_IPADDR, sIP );
			DDX_Text( pDX, IDC_STATIC_ADNAME, sDesc );
			DDX_Text( pDX, IDC_STATIC_IPTYPE, sType );
			DDX_Text( pDX, IDC_STATIC_SUBNET, sSub );
			DDX_Text( pDX, IDC_STATIC_GATEWAY, sGate );
			DDX_Text( pDX, IDC_STATIC_DNSONE, sDns1 );
			DDX_Text( pDX, IDC_STATIC_DNSTWO, sDns2 );	

			SetToolTipForAdapter();
		}				
	}

	return;
}

// CNetCfgDlg message handlers

/////////////////////////////////////////////////////////
//	Desc:
//		handler for messages from the system menu.
/////////////////////////////////////////////////////////
void CNetCfgDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}else{
		CDialog::OnSysCommand(nID, lParam);
	}

	return;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CNetCfgDlg::OnPaint()  {
	if (IsIconic()) {
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
	}else{
		CDialog::OnPaint();
	}

	return;
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNetCfgDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////
//	Desc:
//		Exits.  Function stubbed here for possible
//		'on exit' processing.
/////////////////////////////////////////////////////////
void CNetCfgDlg::OnBnClickedOk() {
	OnOK();
	return;
}

/////////////////////////////////////////////////////////
//	Desc:
//		Minimizes this windows.  May be restored through
//		the system tray icon.
/////////////////////////////////////////////////////////
void CNetCfgDlg::OnBnClickedBtnHide() {
	ShowWindow( SW_HIDE );
	return;
}

////////////////////////////////////////////////////////
//	Desc:
//		Displays all additional information including addresses
//		and lease details.
/////////////////////////////////////////////////////////
void CNetCfgDlg::OnBnClickedBtnShowDetails() {
	if( ( m_nCount > 0 ) && ( m_pAdapters ) ) {
		CDetailsDlg dlg( &m_pAdapters[ m_nDisplay ] );
		dlg.DoModal();		
	}
	return;
}


/////////////////////////////////////////////////////////
//	Desc:
//		Handler for the notification for the  spin control
//		that the user has selected to go up or down with their
//		adapters.
/////////////////////////////////////////////////////////
void CNetCfgDlg::OnDeltaposSpinAdptrs(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

		// we do the minus sign because of the way the ctrl is actually placed
		// on the dialog. To make it += instead would be counter intuitive to the
		// user.
	m_nDisplay -= ( pNMUpDown->iDelta );
	RANGE_ENSURE( m_nDisplay, 0, ( m_nCount - 1 ) );
	UpdateData( FALSE );

	return;
}

/////////////////////////////////////////////////////////
//	Desc:
//		Sets the trayicon tooltip for the current adapter.
//		the tooltip lists the ip address for the currently
//		selected adapter in the main dialog window.
/////////////////////////////////////////////////////////
void CNetCfgDlg::SetToolTipForAdapter() {
	CString sTip;
	CNetworkAdapter* pAd = & m_pAdapters[ m_nDisplay ];
	sTip.Format( _T("IP:  %s\nGateway:  %s\nPri DNS: %s"),				 
				 pAd->GetIpAddr().c_str(),
				 pAd->GetGatewayAddr().c_str(),
				 pAd->GetDnsAddr().c_str() );
	m_tray.SetToolTip( sTip );

	return;
}

/////////////////////////////////////////////////////////
//	Desc:
//		Releases all address information for the 
//		network adapter currently selected.
/////////////////////////////////////////////////////////
void CNetCfgDlg::OnBnClickedBtnRelease() {
	CNetworkAdapter* pAd = & m_pAdapters[ m_nDisplay ];
	CString sMsg;
	if( pAd->ReleaseAddress() ) {
		sMsg.Format( _T("All Address Information for %s has been released."), pAd->GetAdapterDescription() );
		AfxMessageBox( sMsg, MB_OK | MB_ICONINFORMATION );
		UpdateData( FALSE );
	}else{
		AfxMessageBox( _T("Failed To Release Adapater Addresses."), MB_OK | MB_ICONERROR );
	}
	return;
}

/////////////////////////////////////////////////////////
//	Desc:
//		Reobtains all dhcp addresses for the network adapter
//		currently selected. This includes information on the dns servers.
/////////////////////////////////////////////////////////
void CNetCfgDlg::OnBnClickedBtnRenew() {
	CNetworkAdapter* pAd = & m_pAdapters[ m_nDisplay ];
	CString sMsg;
	if( pAd->RenewAddress() ) {
		sMsg.Format( _T("All Address Information for %s has been renewed."), pAd->GetAdapterDescription() );
		AfxMessageBox( sMsg, MB_OK | MB_ICONINFORMATION );
		UpdateData( FALSE );
	}else{
		AfxMessageBox( _T("Failed To Renew Adapater Addresses."), MB_OK | MB_ICONERROR );
	}
	return;
}


/////////////////////////////////////////////////////////
//	Desc:
//		When a message comes in from the tray icon it
//		is routed here and then passed into the tray icon
//		class for first crack at the message.
/////////////////////////////////////////////////////////
LRESULT CNetCfgDlg::OnTrayNotification( WPARAM wp, LPARAM lp ) {	
	return m_tray.OnNotification( wp, lp );
}

/////////////////////////////////////////////////////////
//	Desc:
//		Handles the restore message from the tray icon.
//		we just need to make sure the window is visible.
/////////////////////////////////////////////////////////
void CNetCfgDlg::OnTrayMsgRestore() {
	ShowWindow( SW_NORMAL );
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////
//	Desc:
//		construction. ( nothing )
/////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
	return;
}

/////////////////////////////////////////////////////////
//	Desc:
//		Sets up our ok button.
/////////////////////////////////////////////////////////
BOOL CAboutDlg::OnInitDialog() {
	m_close.SubclassDlgItem( IDOK, this );
	m_close.SetIcon( IDI_IC_OKACTIVE, IDI_IC_OKINACTIVE );
	m_close.SetThemeHelper( &m_ThemeHelper );
	return CDialog::OnInitDialog();
}

/////////////////////////////////////////////////////////
//	Desc:
//		Data exchange. ( not used )
/////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	return;
}
