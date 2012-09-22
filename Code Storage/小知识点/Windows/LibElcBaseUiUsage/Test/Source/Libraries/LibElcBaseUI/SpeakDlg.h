#pragma once


#include "SpeakWnd.h"

// CSpeakDlg dialog

class CSpeakDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpeakDlg)

public:
	CSpeakDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpeakDlg();

	BOOL Initialize(PSTUDV_PARAMS pParams);
	

// Dialog Data
	enum { IDD = IDD_SPEAK };

protected:
	CSpeakWnd		m_wndSpeak;
	CElcStatic		m_wndIcon;

	CBrush			m_brBkgnd;
	int				m_nBtnHMargin;

	IStudentVoiceDialogCallback* m_pCallback;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnListChanged(WPARAM wParam, LPARAM lParam);
};
