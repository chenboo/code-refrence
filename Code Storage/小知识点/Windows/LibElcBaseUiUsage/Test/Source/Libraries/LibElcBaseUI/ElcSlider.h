#pragma once


// CElcSlider

class LIBELCBASEUI_API CElcSlider : public CWnd
{
	DECLARE_DYNAMIC(CElcSlider)

public:
	CElcSlider();
	virtual ~CElcSlider();

	BOOL Create(
		DWORD dwStyle, 
		const RECT& rect, 
		CWnd* pParent, 
		UINT nId, 
		BOOL bHorizontal=TRUE,
		LPCTSTR lpSkinStyle=ELC_SKIN_STYLE_SLIDER_BIG);

	void SetNotifyWindow(HWND hWnd, UINT nMsg) {
		m_hNotifyWnd = hWnd;
		m_nNotifyMsg = nMsg;
	}

	BOOL LoadSkin(LPCTSTR lpszStyle);
	void SetBackgroundColor(COLORREF cr);

	void SetRange(int nRange);
	int GetRange() {
		return m_nRange;
	}
	void SetPos(int nPos);
	int GetPos();

	void SetLevel(int nLevel);
	void SetLevelRange(int nRange);
	void ShowLevel(BOOL bShow) {
		m_bShowLevel = bShow;
	}

	void EnableDraging(BOOL bEnable);
	BOOL IsDraging();

	void EnableMovingNotify(BOOL bEnable);
	BOOL EnableWindow(BOOL bEnable = TRUE);

protected:
	void ReLayout();
	void CalcPos(CPoint point);
	void NotifyPos();

	CRect GetThumbRect();

	void Repaint();
	void Repaint(LPCRECT lpRect);

protected:
	enum ELC_SLIDER_STATE {
		ESS_NORMAL = 0,
		ESS_HOVER = 1,
		ESS_DOWN = 2,
		ESS_DISABLE = 3,
	};
	enum ELC_SLIDER_TIMER {
		EST_STATE_CHECK = 0x00010000,
		EST_LEVEL = 0x00020000,
	};

	ELC_SKIN_SLIDER		m_skin;

	HWND				m_hNotifyWnd;
	UINT				m_nNotifyMsg;

	int					m_nRange;
	int					m_nPos;
	int					m_nLevelRange;
	int					m_nLevel;
	BOOL				m_bShowLevel;

	DWORD				m_dwTimer;

	CRect				m_rcClient;
	CRect				m_rcBody;
	CSize				m_sizeThumb;

	int					m_nState;
	BOOL				m_bEnabldDrag;
	BOOL				m_bHorizontal;
	BOOL				m_bNotifyMoving;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


