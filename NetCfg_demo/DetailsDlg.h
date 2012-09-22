#pragma once

#include "xpstylebuttonst.h"
#include "themehelperst.h"
#include <afxcmn.h>


////////////////////////////////
// prototype
class CNetworkAdapter;

// CDetailsDlg dialog

class CDetailsDlg : public CDialog
{
	DECLARE_DYNAMIC(CDetailsDlg)

public:
	CDetailsDlg( CNetworkAdapter* pAdapter, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDetailsDlg();

// Dialog Data
	enum { IDD = IDD_DLG_DETAILS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

private:
	CXPStyleButtonST	m_exit;
	CThemeHelperST		m_ThemeHelper;
	CNetworkAdapter*	m_pAdapter;

	
	DECLARE_MESSAGE_MAP()
	CListCtrl m_listctrl;
};
