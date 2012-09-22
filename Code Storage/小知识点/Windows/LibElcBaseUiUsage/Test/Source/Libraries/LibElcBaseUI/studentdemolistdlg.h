#pragma once

#include "codesnippets/teacherframe.h"
#include "CheckListCtrlEx.h"

// CStudentDemoListDlg dialog

class CStudentDemoListDlg : 
	public CElcSkinDialog,
	public ITeacherFrameNotify
{
	DECLARE_DYNAMIC(CStudentDemoListDlg)

public:
	CStudentDemoListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStudentDemoListDlg();

	void SetParams(PSTUDD_PARAMS pParams);

// Dialog Data
	enum { IDD = IDD_DEMO_LIST };

	enum {DEMO_TO_ALL, DEMO_TO_PART, DEMO_TO_GROUP};

public:
	// ITeacherFrameNotify
	void OnMemberConnected(PMEMBERINFO pMemberInfo, DWORD dwUserData);
	void OnMemberDisconnected(PMEMBERINFO pMemberInfo, DWORD dwUserData);
	void OnMemberNameChanged(PMEMBERINFO pMemberInfo, DWORD dwUserData);
	void OnFrameExit(DWORD dwUserData);

protected:
	void InitAvailableList(int nType);
	void SetStudentSelected(int nType);

	void UpdateButtonState();

	int FindItem(PMEMBERINFO pMemberInfo);

protected:
	CCheckListCtrlEx m_wndAvailableList;
	CComboBox m_wndListType;

	int m_nDemoStudentItem;
	int m_nSelectType;

	ITeacherFrame* m_pTeacherFrame;
	IStudentDemoDialogCallback* m_pCallback;
	PMEMBERINFO m_pDemoMemberInfo;
	PSTUDD_PARAMS m_pParams;

	CWnd*	m_pWndParent;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedBtnHelp();
	afx_msg void OnCbnSelchangeListType();
	afx_msg void OnLvnItemchangedAvailableList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedChkSelectAll();
};
