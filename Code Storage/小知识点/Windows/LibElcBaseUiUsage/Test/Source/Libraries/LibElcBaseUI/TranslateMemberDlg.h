#pragma once
#include "afxcmn.h"

#define SV_MAX_TRANSLATE_ROLE	6

// CTranslateMemberDlg dialog

class CTranslateMemberDlg : public CDialog
{
	DECLARE_DYNAMIC(CTranslateMemberDlg)

public:
	CTranslateMemberDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTranslateMemberDlg();

// Dialog Data
	enum { IDD = IDD_MEMBER_TRANSLATE };

	void SetNotifyWindow(HWND hWnd, UINT nMsg);

public:
	CElcListCtrl	m_wndTranslateList;
	
	CBrush			m_brBkgnd;

	HWND			m_hNofityWnd;
	UINT			m_nNotifyMsg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTranslateMemberListBeginLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTranslateMemberListEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMemberListItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
};
