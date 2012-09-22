//#pragma once
//
//class CTestFloatBar : public CWnd
//{
//public:
//	CTestFloatBar();
//	virtual ~CTestFloatBar();
//
//
//public:
//	BOOL CreateBar(CWnd* pParent);
//	void ShowBar(BOOL bShow=TRUE);
//
//	void SetNotifyWindow(HWND hWnd)
//	{
//		m_hNotifyWnd = hWnd;
//	}
//
//
//protected:
//	HWND	m_hNotifyWnd;
//
//protected:
//	DECLARE_MESSAGE_MAP()
//	DECLARE_DYNAMIC(CTestFloatBar)
//
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg LRESULT OnNcHitTest(CPoint point);
//	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
//	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
//	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
//	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnClose();
//	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
//	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
//};
//
//
