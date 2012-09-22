#pragma once

/////////////////////////////////////////////////////////////////////////////
// CBarButton window

BOOL GetRegionfromBitmap(RECT r, CRgn *pRgn, CBitmap *pBitmap, COLORREF keyColor, COLORREF cTolerance = 0x101010);

class LIBELCBASEUI_API CBarButton : public CButton
{
	DECLARE_DYNAMIC(CBarButton)

public:
	CBarButton();
	virtual ~CBarButton();

	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	void SetNotifyWindow(HWND hWnd, UINT uMsg);
	void SetNotifyMouseIn(BOOL bNotify, HWND hWnd, UINT uMsg);	


	void SetBitmap(CBitmap& bmpNormal, CBitmap& bmpOver, CBitmap& bmpDown, CBitmap& bmpDisabled);
	void SetBitmap(UINT nBimapID);
	void SetStretchBitmap(BOOL bOn);

	void SetCaption(LPCTSTR lpszCaption, BOOL bShowCaption = TRUE);
	void SetFont(CFont* pFont);
	void SetFontColor(COLORREF color);

	CSize GetBitmapSize();

	void SetCheck(BOOL m_bNewValue);
	int GetCheck();

	void SetTransparent(BOOL bTransparent, COLORREF crTransparent = RGB(255,0,255));
	BOOL IsTransparent();

	void SetBkColor(COLORREF crBk);
	COLORREF GetBkColor();

	CFont* GetFont() {
		return &m_ftCaption;
	}

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void Repaint();

protected:
	HWND			m_hNotify;
	UINT			m_uMsg;

	BOOL			m_bNotifyMouseIn;
	HWND			m_hNotifyMouse;
	UINT			m_uMouseMsg;

	BOOL			m_bMouseOnButton;
	BOOL			m_bChecked;
	BOOL			m_bLButtonDown;

	CBitmap			m_bmBtn[4];
	CSize			m_sizeBmp;
	
	CString			m_strCaption;
	BOOL			m_bShowCaption;
	CFont			m_ftCaption;
	COLORREF		m_crCaption;	
	
	BOOL			m_bStretchBitmap;
	BOOL			m_bTransparent;
	COLORREF		m_crTransparent;
	COLORREF		m_crBkColor;

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};