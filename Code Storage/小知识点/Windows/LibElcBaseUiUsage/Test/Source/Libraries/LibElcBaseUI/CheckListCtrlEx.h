#if !defined(AFX_CHECKLISTCTRLEX_H__064E5EAB_960B_4314_BF0F_8F468E6483FC__INCLUDED_)
#define AFX_CHECKLISTCTRL_H__064E5EAB_960B_4314_BF0F_8F468E6483FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCheckListCtrlEx window

class CCheckListCtrlEx : public CListCtrl
{
// Construction
public:
	CCheckListCtrlEx();

// Attributes
public:
	void EnableCheck(UINT uIdBitmap);

	BOOL GetCheck(int nItem);
	void SetCheck(int nItem, BOOL bChecked);

	void EnableSelect(BOOL bEnable);

	void SetDisable(int nItem, BOOL bDisable);
	BOOL IsDisable(int nItem);

// Operations
public:
	typedef enum {
		STATE_EMPTY = 0,
		STATE_UNCHECK,
		STATE_CHECK,
		STATE_DISABLECHECK,
		STATE_DISABLE
	};
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckListCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCheckListCtrlEx();

	// Generated message map functions
protected:
	CImageList	m_imgListState;
	BOOL		m_bEnabled;

	//{{AFX_MSG(CCheckListCtrlEx)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKLISTCTRL_H__064E5EAB_960B_4314_BF0F_8F468E6483FC__INCLUDED_)
