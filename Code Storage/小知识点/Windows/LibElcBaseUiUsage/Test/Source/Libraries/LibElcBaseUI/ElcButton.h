#pragma once


class LIBELCBASEUI_API CElcButton : public CButton
{
	DECLARE_DYNAMIC(CElcButton)

public:
	CElcButton();
	virtual ~CElcButton();

public:
	BOOL LoadSkin(LPCTSTR lpszStyle);

	void SetTextColor(COLORREF cr) {
		m_skin.crTextNormal = cr;
		Invalidate();
	}
	COLORREF GetTextColor() {
		return m_skin.crTextNormal;
	}
	void SetTextColor(COLORREF crNormal, COLORREF crHot, COLORREF crDown, COLORREF crDisabled) {
		m_skin.crTextNormal = crNormal;
		m_skin.crTextHot = crHot;
		m_skin.crTextDown = crDown;
		m_skin.crTextDisabled = crDisabled;
		Invalidate();
	}

	void SetBkColor(COLORREF cr) {
		m_skin.crBackgnd = cr;
		Invalidate();
	}
	COLORREF GetBkColor() {
		return m_skin.crBackgnd;
	}

	void EnableCtlColor(BOOL bEnable) {
		m_bEnableCtrlColor = bEnable;
	}
	BOOL IsEnableCtlColor() {
		return m_bEnableCtrlColor;
	}

public:
	// these function are for pushbutton

	void SetLayout(int nLayout=ELCBL_HORIZONTAL) {
		m_nLayout = nLayout;
	}
	int GetLayout() {
		return m_nLayout;
	}

	// set icon : png, bitmap, icon...
	void SetIconImage(UINT nResId, LPCTSTR lpszType=RT_PNG, ELC_BUTTON_ICON_TYPE type=ELCBIM_SINGLE);
	void SetIconImage(Image * pImage, ELC_BUTTON_ICON_TYPE type);

	// set text style
	// see ELC_BUTTON_TEXT_STYLE
	void SetTextStyle(int nTextStyle=ELCBTS_UNDERLINE_WHEN_HOT) {
		m_textstyle = nTextStyle;
	}
	int GetTextStyle() {
		return m_textstyle;
	}

	void SetDropType(int nType) {
		m_droptype = nType;
	}
	int GetDropType() {
		return m_droptype;
	}

	// When u change margin, the button's size will be changed also.
	// So u need to call "GetIdealSize()" to get the ideal size of button.
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
	}
	BOOL IsAnimateDown() {
		return m_bAnimateDown;
	}

	// get the button's size after set image and text
	CSize GetIdealSize();

	// set notify window if you want
	void SetNotifyWindow(HWND hWnd, UINT uMsg) {
		m_hNotifyWnd = hWnd;
		m_nNotifyMsg = uMsg;
	}

	void SetCheck(int nCheck) {
		m_nChecked = nCheck;
	}
	int GetCheck() {
		return m_nChecked;
	}
protected:
	int							m_nLayout;
	int							m_droptype;
	int							m_textstyle;

	ELC_SKIN_BUTTON				m_skin;
	Image *						m_pIcon;
	int							m_nIconType;
	BOOL						m_bIconExtern;

	CRect						m_rcClient;
	CSize						m_sizeBackgnd;

	BOOL						m_bMouseOnButton;
	BOOL						m_bLButtonDown;
	BOOL						m_bAnimateDown;
	BOOL						m_bEnableCtrlColor;
	int							m_nChecked;

	int							m_nHMargin;
	int							m_nVMargin;

	CFont						m_ftText;

	HWND						m_hNotifyWnd;
	UINT						m_nNotifyMsg;

	BOOL						m_bMirror;

protected:
	void DrawCheckBox(CDC* pDC);
	void DrawGroupBox(CDC* pDC);
	void DrawPushButton(CDC* pDC);

	int GetButtonState();

	void LoadDefSkin();

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


