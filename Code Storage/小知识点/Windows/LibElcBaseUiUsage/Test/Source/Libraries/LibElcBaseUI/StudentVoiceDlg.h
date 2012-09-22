#pragma once
#include "afxcmn.h"

#include "SpeakDlg.h"
#include "SceneDlg.h"
#include "TranslateDlg.h"

// CStudentVoiceDlg dialog

class CStudentVoiceDlg : public CElcSkinDialog
{
	DECLARE_DYNAMIC(CStudentVoiceDlg)

public:
	CStudentVoiceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStudentVoiceDlg();

// Dialog Data
	enum { IDD = IDD_STUDENT_VOICE };

	void SetParams(PSTUDV_PARAMS pParams);


protected:
	CElcTabCtrl			m_wndTab;

	CSpeakDlg			m_dlgSpeak;
	CSceneDlg			m_dlgScene;
	CTranslateDlg		m_dlgTranslate;

	STUDV_PARAMS		m_studParams;

protected:

	void Relayout();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTabPage(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnTabCtrlNotify(WPARAM wParam, LPARAM lParam);
};
