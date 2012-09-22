#pragma once

class LIBELCBASEUI_API CBlendMsgWnd : public CWnd
{
	DECLARE_DYNAMIC(CBlendMsgWnd)

public:
	CBlendMsgWnd();
	~CBlendMsgWnd(void);

public:
	BOOL Create(CWnd* pParentWnd);

	void SetIcon(HICON hIcon);

	void SetTitle(CString strTitle);
	void SetDecs(CString strDecs);

	void SetTitleFont(CFont* pFont);
	void SetDescFont(CFont* pFont);

	void SetColors(COLORREF crText, COLORREF crBkgnd);

	void SetAnimateDelay(DWORD dwTime);
	void SetShowDelay(DWORD dwTime);

protected:
	DECLARE_MESSAGE_MAP()

protected:
	HICON		m_hIcon;

	CString		m_strTitle;
	CString		m_strDesc;

	COLORREF	m_crText;
	COLORREF	m_crBkgnd;

	DWORD		m_dwAnimateDelay;
	DWORD		m_dwShowDelay;

	CFont		m_ftTitle;
	CFont		m_ftDesc;

	WORD		m_wAlpha;

	BOOL		m_bAnimateInTimer;
	BOOL		m_bAnimateOutTimer;
	BOOL		m_bShowTimer;

	DWORD		m_dwWidth;
	DWORD		m_dwHeight;
	DWORD		m_dwMargin;

	int CalculateTitleHeight();
	int CalculateDesHeight();

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
