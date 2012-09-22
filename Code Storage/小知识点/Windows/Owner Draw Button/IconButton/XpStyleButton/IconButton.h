#pragma once
// CIconButton

typedef HRESULT	(__stdcall *pFnDrawPBk)(HWND hWnd,HDC hdc, RECT *pRect);
typedef HRESULT	(__stdcall *pFnDrawBk)(HTHEME hTheme, HDC hdc,
								int iPartId, int iStateId,
								const RECT *pRect,  const RECT *pClipRect);
typedef HTHEME	(__stdcall *pFnOpenTheme)(HWND hwnd,
								LPCWSTR pszClassList);


class CIconButton : public CButton
{
	DECLARE_DYNAMIC(CIconButton)

public:
	CIconButton();
	virtual ~CIconButton();

public:
	void SetIcon(UINT nIcon);
	void SetBitmap(UINT nBmID);
	void SetToggle(BOOL bEnble);
	void SetTooltipText(int nTextID, BOOL bActivate = TRUE);
	void SetTooltipText(LPCTSTR lpszText, BOOL bActivate = TRUE);
	void ActivateTooltip(BOOL bEnable = TRUE);
	
	void InitXTheme();
	void SizeToContent();
	void DrawBackGround(CDC* pDC);
	void DrawIcon(CDC* pDC,CRect& rect);
	void DrawBitmap(CDC* pDC,CRect& rect);

public:
	HICON					m_hIcon;
	CBitmap					m_bitmap;
	int						m_nImageWidth;
	int						m_nImageHeight;

	BOOL					m_bTracking;//used for enable mousehover and mouseleave
	BOOL					m_bhover;
	BOOL					m_bEnableToggle;
	BOOL					m_bToggle;

	static HMODULE			m_hXTheme;
	static HTHEME			m_hTheme;
	static pFnOpenTheme		m_pFnOpenTheme;
	static pFnDrawPBk		m_pFnDrawPBk;
	static pFnDrawBk		m_pFnDrawBk;
	static BOOL				m_bThemeLoaded;

	CToolTipCtrl			m_CtlToolTip;
	CString					m_strTipText;

protected:
	DECLARE_MESSAGE_MAP()
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
};


