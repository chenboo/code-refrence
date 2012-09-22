/////////////////////////////////////////////////////////////////////////////
// While modified somewhat, the majority of this code comes from 
// Paul DiLascia's traytest application published in the mircosoft
// systems journal, march 1996. Props to the man!!!
/////////////////////////////////////////////////////////////////////////////
#ifndef _TRAYICON_H____
#define _TRAYICON_H____
#include <afxwin.h>

////////////////////////////////////////////////////////////
//	Desc:
//		Defines a class wrapper around functionality to 
//		place an application icon in the system tray.
//		this icon will display a menu by default
//		when right clicked and execute the first
//		command on that menu by default when double clicked.
////////////////////////////////////////////////////////////
class CTrayIcon : public CCmdTarget {
	public:
		CTrayIcon( UINT nID = 1 );
		~CTrayIcon();

		BOOL SetNotifyWnd( CWnd* pWnd, UINT nMsg );		
		virtual LRESULT OnNotification( WPARAM uID, LPARAM lEvent );

		BOOL SetMenu( UINT nMenu );
		BOOL SetIcon( HICON hIcon );
		BOOL SetIcon( UINT nResID, LPCTSTR lpszModule = NULL );
		BOOL SetBallonTip( LPCTSTR lpszTipTitle = NULL, LPCTSTR lpszTip = NULL, DWORD dwIconType = NIIF_NONE );
		BOOL SetBallonTip( UINT nTitle, UINT nTip, DWORD dwIconType = NIIF_NONE );		

		BOOL SetToolTip( LPCTSTR lpszTip = NULL );
		BOOL SetToolTip( UINT nTip );		

	protected:
		CMenu			m_menu;		
		NOTIFYICONDATA	m_notifydata;
		DECLARE_DYNAMIC( CTrayIcon )
};

#endif //_TRAYICON_H____