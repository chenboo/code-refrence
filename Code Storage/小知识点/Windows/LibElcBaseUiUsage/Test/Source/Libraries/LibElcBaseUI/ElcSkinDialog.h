/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////


#pragma once

class LIBELCBASEUI_API CElcSkinDialog : 
	public CDialog,
	public CElcSkinFrameBase
{
	DECLARE_DYNAMIC(CElcSkinDialog)

public:
	CElcSkinDialog(CWnd* pParent = NULL);
	CElcSkinDialog(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CElcSkinDialog();

	enum { IDD = 0x7FFF };

	BOOL LoadSkin(LPCTSTR lpStyle);
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);

	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);

	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
