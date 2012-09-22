// DetailsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetCfg - The Sequel.h"
#include "DetailsDlg.h"
#include "netadapter.h"

IMPLEMENT_DYNAMIC(CDetailsDlg, CDialog)

// CDetailsDlg dialog
BEGIN_MESSAGE_MAP(CDetailsDlg, CDialog)
END_MESSAGE_MAP()


CDetailsDlg::CDetailsDlg( CNetworkAdapter* pAdapter, CWnd* pParent ) : CDialog(CDetailsDlg::IDD, pParent) {
	m_pAdapter = pAdapter;
	return;
}

CDetailsDlg::~CDetailsDlg() {
	return;
}

BOOL CDetailsDlg::OnInitDialog() {
	BOOL		bRet	= CDialog::OnInitDialog();
	CListCtrl*	pList	= ( CListCtrl* ) GetDlgItem( IDC_MYLIST );	
	int			nCnt	= 0;
	time_t		tTmTmp	= 0;
	TCHAR		sTimeBuf[ 200 ];
	CRect rcListWidth;
    
	pList->ModifyStyle( 0, LVS_REPORT );
	pList->InsertColumn( 0, _T("Description") );
	pList->InsertColumn( 1, _T("Address") );

	if( m_pAdapter ) {
	
		pList->GetWindowRect( &rcListWidth );
		pList->SetColumnWidth( 0, ( rcListWidth.Width() / 3 ) - 5 );
		pList->SetColumnWidth( 1, ( rcListWidth.Width() / 3 ) * 2 );

		pList->InsertItem( nCnt, _T("DHCP Enabled") );
		pList->SetItemText( nCnt, 1, ( m_pAdapter->IsDhcpUsed() ? _T("Yes") : _T("No") ) );
		nCnt++;

		if( m_pAdapter->IsDhcpUsed() ) {
			pList->InsertItem( nCnt, _T("DHCP Server") );
			pList->SetItemText( nCnt, 1, m_pAdapter->GetDchpAddr().c_str() );
			nCnt++;

			ZeroMemory( sTimeBuf, 200 * sizeof( TCHAR ) );
			tTmTmp = m_pAdapter->GetLeaseObtained();
			_tcsftime(  sTimeBuf, 200, _T("%A, %B %d, %Y"), localtime( &tTmTmp ) );

			pList->InsertItem( nCnt, _T("Lease Obtained") );
			pList->SetItemText( nCnt, 1, sTimeBuf );
			nCnt++;

			ZeroMemory( sTimeBuf, 200 * sizeof( TCHAR ) );
			tTmTmp = m_pAdapter->GetLeaseExpired();
			_tcsftime(  sTimeBuf, 200, _T("%A, %B %d, %Y"), localtime( &tTmTmp ) );

			pList->InsertItem( nCnt, _T("Lease Expires") );
			pList->SetItemText( nCnt, 1, sTimeBuf );
			nCnt++;
		}

		pList->InsertItem( nCnt, _T("WINS Enabled") );
		pList->SetItemText( nCnt, 1, ( m_pAdapter->IsWinsUsed() ) ? _T("Yes") : _T("No") );
		nCnt++;

		if( m_pAdapter->IsWinsUsed() ) {
			pList->InsertItem( nCnt, _T("Primary WINS Server") );
			pList->SetItemText( nCnt, 1, m_pAdapter->GetPrimaryWinsServer().c_str() );
			nCnt++;

			pList->InsertItem( nCnt, _T("Secondary WINS Server") );
			pList->SetItemText( nCnt, 1, m_pAdapter->GetSecondaryWinsServer().c_str() );
			nCnt++;
		}	
	}

	m_exit.SubclassDlgItem( IDOK, this );
	m_exit.SetIcon( IDI_IC_OKACTIVE, IDI_IC_OKINACTIVE );
	m_exit.SetThemeHelper( &m_ThemeHelper );

	return bRet;
}

void CDetailsDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);

	return;	
}


// CDetailsDlg message handlers
