/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////



#pragma once


// CBarButtonEx

class LIBELCBASEUI_API CBarButtonEx : public CButton
{
	DECLARE_DYNAMIC(CBarButtonEx)

public:
	CBarButtonEx();
	virtual ~CBarButtonEx();

public:
	// DO NOT call other create function of CWnd. Just This.
	BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// load a predefine skin from CElcSkinDoc
	BOOL LoadSkin(LPCTSTR lpStyle, BOOL bRedraw=TRUE);

	// set icon and text layout
	// see ELC_BUTTON_LAYOUT 
	void SetLayout(int layout) {
		m_layout = layout;
	}
	int GetLayout() {
		return m_layout;
	}

	// set image, default type is RT_PNG when lpszType=NULL
	// bitmap resource : RT_BITMAP
	// icon resource : RT_ICON
	// set ur custom image if u want
	// notice : the button will load a default backgnd image when be created,
	// u can change it if want.
	// Recommend to using PNG image
	BOOL SetBackgndImage(UINT nResId, LPCTSTR lpszType=RT_PNG, int nLeftSplit=-1, int nRightSplit=-1, int nTopSplit=-1, int nBottomSplit=-1);

	// set a icon image, image format : png, bitmap, icon 
	BOOL SetIconImage(UINT nResId, LPCTSTR lpszType=RT_PNG, ELC_BUTTON_ICON_TYPE mode=ELCBIM_SINGLE);
	// this function is used for loading a 24-bit bitmap with transparent color
	BOOL SetIconImage(UINT nBitmapResId, COLORREF crTransparent, ELC_BUTTON_ICON_TYPE mode=ELCBIM_SINGLE);


	// see ELC_BUTTON_DROP_TYPE
	// the button default is normal, has not drop image
	void SetButtonDropType(int nType);
	// set your custom image if u want
	BOOL SetDropImage(UINT nResId, LPCTSTR lpszType=RT_PNG, ELC_BUTTON_ICON_TYPE mode=ELCBIM_SINGLE);

	// when u change text, the button's size is changed also.
	// so u can call "GetIdealSize()" to get the ideal size of button.
	void SetText(LPCTSTR lpszText);
	void SetText(UINT nResId);
	CString GetText() {
		return m_strText;
	}

	void SetTextColor(COLORREF color);
	COLORREF GetTextColor() {
		return m_btnskin.crTextNormal;
	}

	// when u change the font, the button's size is changed also.
	// so u can call "GetIdealSize()" to get the ideal size of button.
	void SetFont(CFont* pFont);
	CFont * GetFont() {
		return &m_ftText;
	}

	void SetBackgndColor(COLORREF color);
	COLORREF GetBackgndColor() {
		return m_btnskin.crBackgnd;
	}

	// get the button's size after set image and text
	CSize GetIdealSize();
	CSize GetBackgndImageSize();

	void SetCheck(BOOL bChecked);
	BOOL GetCheck() {
		return m_bChecked;
	}

	// set notify window if you want
	void SetNotifyWindow(HWND hWnd, UINT uMsg) {
		m_hNotifyWnd = hWnd;
		m_nNotifyMsg = uMsg;
	}

	// when u change margin, the button's size is changed also.
	// so u can call "GetIdealSize()" to get the ideal size of button.
	void SetHMargin(int nMargin) {
		m_nHMargin = nMargin;
	}
	int GetHMargin() {
		return m_nHMargin;
	}
	void SetVMargin(int nMargin) {
		m_nVMargin = nMargin;
	}
	int GetVMargin() {
		return m_nVMargin;
	}

	void SetAnimateDown(BOOL bEnable) {
		m_bAnimateDown = bEnable;
		Repaint();
	}
	BOOL IsAnimateDown() {
		return m_bAnimateDown;
	}

	// Call this function to create a "text-link" button, and it'll clear background & icon image.
	// This style button has NOT background & icon image, also has NOT borders.
	// So remember DON NOT set background & icon image, borders if u want to use this style in button.
	// underline style : see ELC_BUTTON_TEXT_STYLE
	// the function return the ideal size of the button if successful.
	CSize SetTextLinkerButton(LPCTSTR lpszText, int nUnderlineStyle=ELCBTS_UNDERLINE_WHEN_HOT, COLORREF crNormal=RGB(14,87,163), COLORREF crHot=RGB(204,0,0));

	void SetAdjust(int x, int y) {
		m_ptAdjust.x = x;
		m_ptAdjust.y = y;
	}

	enum OVERLAY_ICON_ALIGN {
		ALIGN_LEFT		= 0x00000001,
		ALIGN_RIGHT		= 0x00000002,
		ALIGN_H_CENTER	= 0x00000003,
		ALIGN_H_MASK	= 0x0000000F,

		ALIGN_TOP		= 0x00010000,
		ALIGN_BOTTOM	= 0x00020000,
		ALIGN_V_CENTER	= 0x00030000,

		ALIGN_V_MASK	= 0x000F0000,
	};
	enum {
		ID_TIMER_OVERLAY_FLICKER = 1,
	};

	// nFlickerTime : overlay icon flicker time (ms)
	void SetOverlayIcon(LPCTSTR lpszStyle, UINT nAlign, int nFlickerTime = 0, int nFlickerCount = 0);

protected:
	int 			m_layout;
	DWORD 			m_dwBorderType;
	int				m_droptype;
	int				m_nHMargin;
	int				m_nVMargin;
	int				m_nIconImageMode;

	POINT			m_ptAdjust;

	ELC_SKIN_BUTTON	m_btnskin;

	LPVOID			m_pIconImage;
	CSize			m_sizeIcon;
	CSize			m_sizeDrop;
	UINT			m_nIconResId;

	ELC_SKIN_IMAGE	m_skinOverlay;
	CSize			m_sizeOverlay;
	UINT			m_nAlign;
	int				m_nFlickerTime;
	int				m_nFlickerCount;
	int				m_nFlickerCounter;
	BOOL			m_bShowOverlay;

	CString			m_strText;

	CFont			m_ftText;

	BOOL			m_bChecked;
	BOOL			m_bMouseOnButton;
	BOOL			m_bLButtonDown;
	BOOL			m_bAnimateDown;

	HWND			m_hNotifyWnd;
	UINT			m_nNotifyMsg;

	BOOL			m_bTextlinkerBtn;
	int				m_nUnderlineStyle;

protected:
	void Repaint();
	void ReleaseImages();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawImageButton(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawTextlinkerButton(LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


