#pragma once


// CTranslateChildWnd
#include "SimpleSplitter.h"
#include "AvailableMemberDlg.h"
#include "TranslateMemberDlg.h"

class CTranslateChildWnd : 
	public CWnd,
	public IAvailableMemberDlg
{
	DECLARE_DYNAMIC(CTranslateChildWnd)

public:
	CTranslateChildWnd();
	virtual ~CTranslateChildWnd();

	
	BOOL Initialize(PSTUDV_PARAMS pParams);

	virtual void OnAddItem();
	virtual void OnDelItem();

	BOOL HasItem(PMEMBERINFO pMemberInfo);
	BOOL AddItem(PMEMBERINFO pMemberInfo);
	
	const CTranslateMemberDlg* GetTranslateMemberDlg();

protected:
	CSimpleSplitter			m_wndSpliter;
	CAvailableMemberDlg		m_dlgAvailableMember;
	CTranslateMemberDlg     m_dlgTranslateMember;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnUpdateButtonState(WPARAM wParam, LPARAM lParam);
};


