#pragma once
#include "afxcmn.h"


#include "TestPopupWnd.h"
#include "afxwin.h"

// CSkinDlgTest dialog

class CSkinDlgTest : public CDialog
{
	DECLARE_DYNAMIC(CSkinDlgTest)

public:
	CSkinDlgTest(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSkinDlgTest();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//CElcReportListCtrl		m_wndList;
	//CListCtrl				m_wndList;
	CElcListCtrl			m_wndList;

	CTestPopupWnd			m_SkinPopWnd;

protected:
	virtual void PreInitDialog();

	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

	afx_msg LRESULT OnElcUIPredrawNotify(WPARAM wParam, LPARAM lParam);
};
