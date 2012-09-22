#pragma once

#include "TestFloatBar.h"

class CTestDlg : public CElcSkinDialog
{
public:
	CTestDlg(CWnd* pParent = NULL);
	enum { IDD = IDD_TEST_DIALOG };

protected:
	CElcFloatBar	m_wndFloatBar;
	//CTestFloatBar	m_TestBar;

protected:
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);
};
