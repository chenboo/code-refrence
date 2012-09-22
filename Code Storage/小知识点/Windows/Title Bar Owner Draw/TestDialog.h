#pragma once

// CTestDialog dialog

class CTestDialog : public CDialog
{
	DECLARE_DYNAMIC(CTestDialog)

public:
	CTestDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestDialog();
	enum { IDD = IDD_DIALOG1 };

public:
	void DrawTitleBar();
	void DrawSysMenu();

public:
	BOOL		m_bFocus;
	CRect		m_rcTilte;
	CRect		m_rcExitButton;

	CBitmap		m_bmpExitNormal;
	CBitmap		m_bmpExitFocus;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
};