#pragma once


#include "TranslateChildWnd.h"


// CTranslateDlg dialog

class CTranslateDlg : public CDialog
{
	DECLARE_DYNAMIC(CTranslateDlg)

public:
	CTranslateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTranslateDlg();

// Dialog Data
	enum { IDD = IDD_TRANSLATE };

	BOOL Initialize(PSTUDV_PARAMS pParams);

protected:
	CTranslateChildWnd	m_wndTranslate;
	CElcStatic			m_wndIcon;

	CBrush				m_brBkgnd;
	int					m_nBtnHMargin;

	IStudentVoiceDialogCallback* m_pCallback;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnOk();
	afx_msg LRESULT OnListChanged(WPARAM wParam, LPARAM lParam);
};
