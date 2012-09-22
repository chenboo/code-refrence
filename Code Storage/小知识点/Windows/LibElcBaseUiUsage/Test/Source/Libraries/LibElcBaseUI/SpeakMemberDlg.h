#pragma once
#include "afxcmn.h"


// CSpeakMemberDlg dialog

class CSpeakMemberDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpeakMemberDlg)

public:
	CSpeakMemberDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpeakMemberDlg();


	enum { IDD = IDD_MEMBER_SPEAK };

	void SetNotifyWindow(HWND hWnd, UINT nMsg);

	
	CListCtrl	m_wndSpeakersList;
	CBrush		m_brush;
	CImageList	m_imgList;

	HWND	m_hNofityWnd;
	UINT	m_nNotifyMsg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);	
	afx_msg void OnMemberListItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMDblclkSpeakers(NMHDR *pNMHDR, LRESULT *pResult);
};
