#pragma once

class LIBELCBASEUI_API CButtonEx : public CButton
{
public:
	CButtonEx();
	virtual ~CButtonEx();

public:
    enum 
	{
		ALIGN_HORIZ, 
		ALIGN_VERT, 
		ALIGN_HORIZ_RIGHT
	};

	enum
	{
		COLOR_BK_IN = 0,
		COLOR_FG_IN,
		COLOR_BK_OUT,
		COLOR_FG_OUT,
	};

public:
	void  SetAlign(int nAlign);
	int   GetAlign();

	BOOL  CButtonEx::GetColor(BYTE bColorIndex, COLORREF& clrColor);
	BOOL  SetColor(BYTE bColorIndex, COLORREF clrColor, BOOL bRepaint = TRUE);
	BOOL  SetDefaultColors(BOOL bRepaint = TRUE);

	BOOL  SetButtonCursor(int nCursorId = NULL);

	void  SetFlat(BOOL bState = TRUE);
	BOOL  GetFlat();
	void  SetFlatFocus(BOOL bDrawFlatFocus, BOOL bRepaint = FALSE);

	BOOL  SetAutoRepeat(BOOL bSet, DWORD dwMilliseconds = 100);

	void  SetCheck(int nCheck, BOOL bRepaint = TRUE);
	int   GetCheck();

	void  DrawBorder(BOOL bEnable = TRUE);

	BOOL  SetIcon(int nIconInId, int nIconOutId = NULL);
	BOOL  SetIcon(HICON hIconIn, HICON hIconOut = NULL);

	BOOL  SetBitmaps(int nBitmapIn, COLORREF crTransColorIn, int nBitmapOut = NULL, COLORREF crTransColorOut = 0);
	BOOL  SetBitmaps(HBITMAP hBitmapIn, COLORREF crTransColorIn, HBITMAP hBitmapOut = NULL, COLORREF crTransColorOut = 0);
	
public:
	BOOL IsDefault();

	void CancelHover();

	void PrepareImageRect(BOOL bHasTitle, RECT* rpItem, CRect* rpTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, CRect* rpImage);
	void DrawTheBitmap(CDC* pDC, BOOL bHasTitle, RECT *rItem, CRect *rCaption, BOOL bIsPressed, BOOL bIsDisabled);
	void DrawTheIcon(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpTitle, BOOL bIsPressed, BOOL bIsDisabled);

	virtual BOOL OnDrawBackground(CDC* pDC, LPCRECT pRect, BOOL bIsPressed);
	virtual BOOL OnDrawBorder(CDC* pDC, LPCRECT pRect, BOOL bIsPressed);

	HBITMAP CreateBitmapMask(HBITMAP hSourceBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTransColor);
	void FreeResources(BOOL bCheckForNULL = TRUE);


protected:

	typedef struct _tagBUTTONICON
	{
		HICON		hIcon;			// Handle to icon
		DWORD		dwWidth;		// Width of icon
		DWORD		dwHeight;		// Height of icon
	} BUTTONICON;

	typedef struct _tagBUTTONBITMAP
	{
		HBITMAP		hBitmap;		// Handle to bitmap
		DWORD		dwWidth;		// Width of bitmap
		DWORD		dwHeight;		// Height of bitmap
		HBITMAP		hMask;			// Handle to mask bitmap
		COLORREF	crTransparent;	// Transparent color
	} BUTTONBITMAP;

	BUTTONICON		m_abiIcons[2];
	BUTTONBITMAP	m_abmBitmaps[2];

	HCURSOR			m_hCursor;
	BOOL			m_bIsFlat;
	int				m_nAlign;
	BOOL			m_bDrawBorder;
	BOOL			m_bDrawFlatFocus;

	BOOL			m_bAutoRepeat;
	DWORD			m_dwPeriodAutoRepeat;

	BOOL			m_bIsCheckBox;
	int				m_nCheck;

	BOOL			m_bMouseOnButton;

	UINT			m_uIdRepeatTimer;
	COLORREF		m_clrColors[COLOR_FG_OUT + 1];

protected:

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnCancelMode();
	afx_msg void OnSysColorChange();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
