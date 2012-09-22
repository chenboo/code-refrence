#pragma once


// CSpeakWnd
#include "SimpleSplitter.h"
#include "AvailableMemberDlg.h"
#include "SpeakMemberDlg.h"

class CSpeakWnd : 
	public CWnd,
	public IAvailableMemberDlg
{
	DECLARE_DYNAMIC(CSpeakWnd)

public:
	CSpeakWnd();
	virtual ~CSpeakWnd();

	BOOL Initialize(PSTUDV_PARAMS pParams);


	void OnAddItem();
	void OnDelItem();

	BOOL HasItem(PMEMBERINFO pMemberInfo);
	BOOL AddItem(PMEMBERINFO pMemberInfo);

	const CSpeakMemberDlg* GetSpeakMemberDlg();
	CAvailableMemberDlg* GetAvailableMemberDlg();

protected:
	CSimpleSplitter			m_wndSpliter;
	CAvailableMemberDlg		m_dlgAvailableMember;
	CSpeakMemberDlg			m_dlgSpeakMember;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnUpdateButtonState(WPARAM wParam, LPARAM lParam);
};


