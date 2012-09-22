#pragma once


// CTestPopupWnd

class CTestPopupWnd : public CWnd
{
	DECLARE_DYNAMIC(CTestPopupWnd)

public:
	CTestPopupWnd();
	virtual ~CTestPopupWnd();

	CElcReportListCtrl		m_wndList;
	CElcImagePaintMgr		m_ImagePaintMgr;

	CImageList				m_imgList;
	CBitmap					m_bitmap;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCustomButton1();
	afx_msg LRESULT OnElcUIDrawNotify(WPARAM wParam, LPARAM lParam);
};


