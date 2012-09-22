#include "trayicon.h"

IMPLEMENT_DYNAMIC( CTrayIcon, CCmdTarget )

////////////////////////////////////////////////////////////
//	Desc:
//		Construction.  Initializes the variables.
////////////////////////////////////////////////////////////
CTrayIcon::CTrayIcon( UINT nID ) {
	::memset( &m_notifydata, 0, sizeof( NOTIFYICONDATA ) );
	m_notifydata.cbSize = sizeof( NOTIFYICONDATA );
	m_notifydata.uID	= nID;	
	return;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Shuts down this object.  The icon must be removed
//		from the system tray.
////////////////////////////////////////////////////////////
CTrayIcon::~CTrayIcon() {
	SetIcon( (HICON)NULL );
	
	if( m_menu.GetSafeHmenu() ) {
		m_menu.DestroyMenu();
	}
	return;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Sets the notification data for all messages that
//		are routed through the shell icon.
////////////////////////////////////////////////////////////
BOOL CTrayIcon::SetNotifyWnd( CWnd* pWnd, UINT nMsg ) {
	BOOL bSet = FALSE; 
	if( pWnd && ::IsWindow( pWnd->GetSafeHwnd() ) ) {
		m_notifydata.hWnd = pWnd->GetSafeHwnd();
		if( nMsg != 0 ) {
			m_notifydata.uCallbackMessage = nMsg;
			bSet = TRUE;
		}
	}
	return bSet;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Sets the current menu to be used either while 
//		right clicking or double clicking from a resource.
////////////////////////////////////////////////////////////
BOOL CTrayIcon::SetMenu( UINT nMenu ) {
	BOOL bSet = FALSE;
		
	if( m_menu.GetSafeHmenu() ) {
		m_menu.DestroyMenu();
	}
	
	bSet = m_menu.LoadMenu( nMenu );

	return bSet;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Sets the icon that is to be displayed in the 
//		system tray.  Icon should already by loaded.  If
//		the HICON is null the icon is removed from the
//		system tray.
////////////////////////////////////////////////////////////
BOOL CTrayIcon::SetIcon( HICON hIcon ) {
	BOOL bSet = FALSE;
	UINT nMsg = 0;
	m_notifydata.uFlags = 0;

	if( hIcon ) {
		nMsg = ( m_notifydata.hIcon ) ? NIM_MODIFY : NIM_ADD;
		m_notifydata.hIcon = hIcon;
		m_notifydata.uFlags |= NIF_ICON;
	}else{
		( m_notifydata.hIcon ) ? nMsg = NIM_DELETE : bSet = TRUE;		
	}

	if( m_notifydata.hWnd && m_notifydata.uCallbackMessage ) {
		m_notifydata.uFlags |= NIF_MESSAGE;
	}

	bSet = Shell_NotifyIcon( nMsg, &m_notifydata );
	if( !bSet || ( nMsg == NIM_DELETE ) ) {
		m_notifydata.hIcon = NULL;
	}	

	return bSet;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Sets the icon that is to be displayed in the 
//		system tray.  Icon is loaded using a resource
//		id from the module specified by lpszModule. If
//		lpszModule is NULL icon is loaded from current
//		module.
////////////////////////////////////////////////////////////
BOOL CTrayIcon::SetIcon( UINT nResID, LPCTSTR lpszModule ) {
	BOOL bSet = FALSE;
	HICON hIcon = ( HICON ) ::LoadImage(	::GetModuleHandle( lpszModule ), 
											MAKEINTRESOURCE( nResID ), 
											IMAGE_ICON, 
											0, 
											0, 
											LR_DEFAULTCOLOR | LR_LOADMAP3DCOLORS | LR_DEFAULTSIZE );
	bSet = SetIcon( hIcon );
	return bSet;
}

/////////////////////////////////////////////////////////////////
//	Desc:
//		Causes a a ballon to popup over the system tray icon.
//		All strings are set then the tray icon is informed of
//		the notification.
/////////////////////////////////////////////////////////////////
BOOL CTrayIcon::SetBallonTip( LPCTSTR lpszTipTitle, LPCTSTR lpszTip, DWORD dwIconType ) {
	BOOL bSet = FALSE;	
	UINT nMsg = 0;
	m_notifydata.uFlags = 0;

	if( m_notifydata.hIcon ) {
		if( lpszTip && lpszTipTitle ) {
			::_tcscpy( m_notifydata.szInfo,			lpszTip );
			::_tcscpy( m_notifydata.szInfoTitle,	lpszTipTitle );			
		}else{
			m_notifydata.szInfo[ 0 ]		= _T('\0');
			m_notifydata.szInfoTitle[ 0 ]	= _T('\0');
		}	

		if( m_notifydata.hWnd && m_notifydata.uCallbackMessage ) {
			m_notifydata.uFlags |= NIF_MESSAGE;
		}
		
		m_notifydata.uFlags			= NIF_INFO;
		m_notifydata.dwInfoFlags	= dwIconType;
		m_notifydata.uTimeout		= 10;

		bSet = Shell_NotifyIcon( NIM_MODIFY, &m_notifydata );
		if( !bSet || ( nMsg == NIM_DELETE ) ) {
			m_notifydata.hIcon = NULL;
		}	
	}

	return bSet;
}

/////////////////////////////////////////////////////////////////
//	Desc:
//		Causes a a ballon to popup over the system tray icon.
//		the various strings are loaded from resources and the 
//		passed to the main ballon set function.
/////////////////////////////////////////////////////////////////
BOOL CTrayIcon::SetBallonTip( UINT nTitle, UINT nTip, DWORD dwIconType ) {
	BOOL bGotTitle	= FALSE,
		 bGotTip	= FALSE,
		 bSet		= FALSE;
	CString sTitle,
			sTip;
	bGotTitle	= sTitle.LoadString( nTitle );
	bGotTip		= sTip.LoadString( nTip );
	
	if( bGotTitle && bGotTip ) {
		bSet = SetBallonTip( sTitle, sTip, dwIconType );
	}

	return bSet;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Sets the tooltip that will be displayed with the
//		system tray icon when the mouse cursor hovers
//		over it.
////////////////////////////////////////////////////////////
BOOL CTrayIcon::SetToolTip( LPCTSTR lpszTip ) {
	BOOL bSet = FALSE;	
	UINT nMsg = 0;
	m_notifydata.uFlags = 0;

	if( m_notifydata.hIcon ) {
		if( lpszTip ) {
			::_tcscpy( m_notifydata.szTip,	lpszTip );			
		}else{
			m_notifydata.szTip[ 0 ]	= _T('\0');			
		}	

		if( m_notifydata.hWnd && m_notifydata.uCallbackMessage ) {
			m_notifydata.uFlags |= NIF_MESSAGE;
		}
		
		m_notifydata.uFlags	= NIF_TIP;		

		bSet = Shell_NotifyIcon( NIM_MODIFY, &m_notifydata );
		if( !bSet || ( nMsg == NIM_DELETE ) ) {
			m_notifydata.hIcon = NULL;
		}	
	}

	return bSet;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Sets the tooltip that will be displayed with the
//		system tray icon when the mouse cursor hovers
//		over it.
////////////////////////////////////////////////////////////
BOOL CTrayIcon::SetToolTip( UINT nTip ) {
	BOOL bSet = FALSE;
	CString sTip;
	bSet = sTip.LoadString( nTip );
	if( bSet ) {
		bSet = SetToolTip( sTip );
	}

	return bSet;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Handles all the messages that are delivered to the
//		shell icon in the system tray.
////////////////////////////////////////////////////////////
LRESULT CTrayIcon::OnNotification( WPARAM uID, LPARAM lEvent ) {	
	if( ( uID != m_notifydata.uID ) ||  
		( ( lEvent != WM_RBUTTONUP ) && ( lEvent != WM_LBUTTONDBLCLK ) ) ||
		( ! m_menu.GetSafeHmenu() ) )  {
			return 0;
	}

	CPoint pt;	
	CMenu* pPop = m_menu.GetSubMenu( 0 );
	::SetMenuDefaultItem( pPop->GetSafeHmenu(), 0, TRUE);

	if( lEvent == WM_RBUTTONUP ) {
			// if right click we need to display popup menu.		
		::GetCursorPos( &pt );
		::SetForegroundWindow( m_notifydata.hWnd );
		::TrackPopupMenuEx( pPop->GetSafeHmenu(), 0, pt.x, pt.y, m_notifydata.hWnd, NULL );
	}else{
		::SendMessage( m_notifydata.hWnd, WM_COMMAND, pPop->GetMenuItemID( 0 ), 0 );
	}
	
	return 1;
}






