#pragma once

//call back msg send to the notify window
//to let the notify wnd deal
#define WM_TRAYCALLMSG	(WM_USER + 1000)


class CTrayIcon : public CObject
{
public:
	CTrayIcon();
	virtual ~CTrayIcon();
	
public:
	CWnd*	GetNotifyWnd() const;
	BOOL	SetNotifyWnd(CWnd* pNtfWnd);
	BOOL	CreateIcon(CWnd* pNtfWnd, UINT uID, HICON hIcon,
			LPCTSTR lpszTip);
	BOOL	DeleteIcon();
	virtual LRESULT OnNotify(WPARAM WParam, LPARAM LParam);
	BOOL	SetTipText(UINT nID);
	BOOL	SetTipText(LPCTSTR lpszTip);
	BOOL	ChangeIcon(HICON hIcon);
	BOOL	ChangeIcon(UINT nID);
	BOOL	ChangeStandardIcon(LPCTSTR lpszIconName);
	
public:
	NOTIFYICONDATA	m_ntfData;
	BOOL			m_bIconExit;
	CWnd*			m_pNtfWnd;
};


