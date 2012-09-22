#pragma once

#include "RadioListBox.h"

class LIBELCBASEUI_API CBlendSelectWnd : public CWnd
{
	DECLARE_DYNAMIC(CBlendSelectWnd)

public:
	CBlendSelectWnd();
	virtual ~CBlendSelectWnd();

public:
	BOOL Create(CRect rc, CWnd* pParentWnd, UINT nID, DWORD dwStyle);
	void SetWindowCenter();

	void SetIcon(HICON hIcon);
	void SetCloseIcon(HICON hNormal, HICON hDown);
	void SetRadioIcon(HICON hCheck, HICON hUncheck);

	void SetTitle(CString strTitle);

	int AddItem(CString strItem);
	BOOL DelItem(int nItem);

	void SetTitleFont(CFont* pFont);
	void SetOptionFont(CFont* pFont);

	void SetColors(COLORREF crText, COLORREF crBkgnd);

	void SetAnimateDelay(DWORD dwTime);
	void SetShowDelay(DWORD dwTime);

protected:
	int CalculateTitleHeight();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	HICON		m_hIcon;

	CString		m_strTitle;

	COLORREF	m_crText;
	COLORREF	m_crBkgnd;

	DWORD		m_dwAnimateDelay;
	DWORD		m_dwShowDelay;

	CFont		m_ftTitle;
	CFont		m_ftOption;

	WORD		m_wAlpha;

	BOOL		m_bAnimateInTimer;
	BOOL		m_bAnimateOutTimer;
	BOOL		m_bShowTimer;

	DWORD		m_dwWidth;
	DWORD		m_dwHeight;
	DWORD		m_dwMargin;

	DWORD		m_dwTitleHeight;

	CRadioListBox m_wndListBox;

	HICON		m_hIconClose;
	HICON		m_hIconCloseDown;

	HICON		m_hIconCheck;
	HICON		m_hIconUnCheck;

	CRect		m_rcBtnClose;
	BOOL		m_bCloseDown;

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


