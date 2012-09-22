#pragma once


// CSceneChildWnd
#include "SimpleSplitter.h"
#include "AvailableMemberDlg.h"
#include "SceneMemberDlg.h"

class CSceneChildWnd : 
	public CWnd,
	public IAvailableMemberDlg
{
	DECLARE_DYNAMIC(CSceneChildWnd)

public:
	CSceneChildWnd();
	virtual ~CSceneChildWnd();

	BOOL Initialize(PSTUDV_PARAMS pParams);

	virtual void OnAddItem();
	virtual void OnDelItem();

	BOOL HasItem(PMEMBERINFO pMemberInfo);
	BOOL AddItem(PMEMBERINFO pMemberInfo);

	const CSceneMemberDlg* GetSceneMemberDlg();
	

protected:
	CSimpleSplitter			m_wndSpliter;
	CAvailableMemberDlg		m_dlgAvailableMember;
	CSceneMemberDlg			m_dlgSceneMember;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnUpdateButtonState(WPARAM wParam, LPARAM lParam);
};


