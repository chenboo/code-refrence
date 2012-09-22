#pragma once

#include "SceneChildWnd.h"
#include "elcstatic.h"

// CSceneDlg dialog

class CSceneDlg : public CDialog
{
	DECLARE_DYNAMIC(CSceneDlg)

public:
	CSceneDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSceneDlg();

// Dialog Data
	enum { IDD = IDD_SCENE };

	BOOL Initialize(PSTUDV_PARAMS pParams);

protected:
	CSceneChildWnd	m_wndSence;
	CBrush			m_brBkgnd;

	int				m_nBtnHMargin;
	CElcStatic		m_wndIcon;

	IStudentVoiceDialogCallback* m_pCallback;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnOk();
	afx_msg LRESULT OnListChanged(WPARAM wParam, LPARAM lParam);
};
