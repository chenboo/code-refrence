#pragma once
#include "afxcmn.h"


#define SV_MAX_SCENE_ROLE	6

// CSceneMemberDlg dialog

class CSceneMemberDlg : public CDialog
{
	DECLARE_DYNAMIC(CSceneMemberDlg)

public:
	CSceneMemberDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSceneMemberDlg();

// Dialog Data
	enum { IDD = IDD_MEMBER_SCENE };

	void SetNotifyWindow(HWND hWnd, UINT nMsg);

	CString GetAvailableName();

public:
	CElcListCtrl		m_wndSceneMemberList;

	CBrush				m_brBkgnd;

	HWND				m_hNofityWnd;
	UINT				m_nNotifyMsg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSceneMemberListChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSceneMemberListBeginLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSceneMemberListEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
};
