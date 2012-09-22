// NetCfgSequelDlg.h : header file
//

#pragma once

#include "trayicon.h"
#include "xpstylebuttonst.h"
#include "themehelperst.h"

class CNetworkAdapter;

// CNetCfgDlg dialog
class CNetCfgDlg : public CDialog
{
// Construction
public:
	CNetCfgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_NETCFGTHESEQUEL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL DestroyWindow();

protected:
	CXPStyleButtonST	m_exit;
	CXPStyleButtonST	m_hide;
	CXPStyleButtonST	m_action;
	CXPStyleButtonST	m_detailsbtn;
	CXPStyleButtonST	m_renew;
	CXPStyleButtonST	m_release;
	CThemeHelperST		m_ThemeHelper;
	CNetworkAdapter*	m_pAdapters;
	UINT				m_nCount;
	UINT				m_nDisplay;	
	CTrayIcon			m_tray;

// Implementation
protected:
	HICON m_hIcon;
	void SetToolTipForAdapter();
	BOOL BuildAdapterListing();

		// Generated message map functions
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnHide();	
	afx_msg void OnBnClickedBtnShowDetails();
	afx_msg void OnTrayMsgRestore();
	afx_msg void OnDeltaposSpinAdptrs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnTrayNotification( WPARAM wp, LPARAM lp );		
	DECLARE_MESSAGE_MAP()	
	
public:
	afx_msg void OnBnClickedBtnRelease();
	afx_msg void OnBnClickedBtnRenew();
};


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
protected:	
	CXPStyleButtonST	m_close;
	CThemeHelperST		m_ThemeHelper;
	DECLARE_MESSAGE_MAP()
};