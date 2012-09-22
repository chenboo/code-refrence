/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////




#pragma once

enum ELC_FLOATBAR_ITEM_TYPE {
	ELCFIT_NONE = 0,
	ELCFIT_NWBUTTON,
	ELCFIT_BUTTON,
	ELCFIT_LABEL,
	ELCFIT_IMAGE,
	ELCFIT_SEPARATOR,
	ELCFIT_SPACE,
};

enum ELC_FLOATBAR_BUTTON_AM_STATE {
	ELCFBS_AM_NONE,
	ELCFBS_AM_UP,
	ELCFBS_AM_DOWN,
	ELCFBS_AM_BACK,
	ELCFBS_AM_WAIT,
	ELCFBS_AM_MAX_WAIT = ELCFBS_AM_WAIT + 10,
};

typedef struct ELC_FLOATBAR_NWBUTTON {
	UINT					nState;
	BOOL					bChecked;
	ELC_SKIN_NOTWND_BUTTON	button;
} *PELC_FLOATBAR_NWBUTTON;

typedef struct ELC_FLOATBAR_BUTTON {
	UINT		nState;
	Image *		pImage;
	BOOL		bChecked;
	int			nDropType;

	UINT		nAmState;
} *PELC_FLOATBAR_BUTTON;

typedef struct ELC_FLOATBAR_ITEM {
	UINT		nId;
	UINT		type;
	Rect		rcItem;
	BOOL		bShow;

	PVOID		pvCtrl;

	LPTSTR		lpszText;
	int			cchText;
	SIZE		sizeText;

	LPTSTR		lpszTip;
	int			cchTip;
	SIZE		sizeTip;
} *PELC_FLOATBAR_ITEM;


typedef std::vector<PELC_FLOATBAR_ITEM>			CElcFloatBarItem;


class LIBELCBASEUI_API CElcFloatBar : public CWnd
{
	DECLARE_DYNAMIC(CElcFloatBar)

public:
	CElcFloatBar();
	virtual ~CElcFloatBar();


	BOOL CreateBar(CWnd* pParent);
	void ShowBar(BOOL bShow=TRUE);

	void SetNotifyWindow(HWND hWnd) {
		m_hNotifyWnd = hWnd;
	}

public:
	BOOL LoadSkin(LPCTSTR lpszStyle);

	void SetTitle(LPCTSTR lpszTitle);
	void SetTitleIcon(UINT nResId, LPCTSTR lpszType=RT_PNG);

	// add item
	BOOL AddButton(UINT nId, LPCTSTR lpszText, UINT nResId, LPCTSTR lpszType=RT_PNG, LPCTSTR lpszTip=NULL, ELC_BUTTON_DROP_TYPE type=ELCBDT_NORMAL);
	BOOL AddButton(UINT nId, UINT nResId, LPCTSTR lpszType=RT_PNG, LPCTSTR lpszTip=NULL, ELC_BUTTON_DROP_TYPE type=ELCBDT_NORMAL);
	BOOL AddSeparator(UINT nId);
	BOOL AddLabel(UINT nId, LPCTSTR lpszText);
	BOOL AddImage(UINT nId, UINT nResId, LPCTSTR lpszType=RT_PNG);
	BOOL AddSpace(UINT nId, int nSpaceWidth);

	// romve item
	void DelItem(UINT nId);
	void DelAllItem();

	// notice : the return rect is screen coordinate.
	CRect GetItemRect(UINT nId);

	// modify item
	// button
	void SetButtonCheck(UINT nId, BOOL bChecked);
	BOOL IsButtonChecked(UINT nId);

	void SetButtonImage(UINT nId, UINT nResId, LPCTSTR lpszType=RT_PNG);

	void ShowButtonText(BOOL bShow);
	BOOL IsShowButtonText();

	void SetButtonText(UINT nId, LPCTSTR lpszText);
	void SetButtonText(UINT nId, UINT nTextResId);

	void SetButtonTip(UINT nId, LPCTSTR lpszTip);
	void SetButtonTip(UINT nId, UINT nTipResId);

	void EnableButton(UINT nId, BOOL bEnable);
	BOOL IsButtonEnable(UINT nId);

	// label
	void SetLabel(UINT nId, LPCTSTR lpszText, BOOL bAutoAdjustSize=TRUE);
	CString GetLabel(UINT nId);
	void SetLabelColor(COLORREF cr);
	COLORREF GetLabelColor();

	// image
	void SetImage(UINT nId, UINT nResId, LPCTSTR lpszType=RT_PNG);

	void SetBarAutoHide(BOOL bEnable);
	BOOL IsBarAutoHide();

	void ResetBarPosition(HMONITOR hMonitor);

protected:
	void Repaint();
	void DrawBar(Graphics* pGraphics);
	void DrawItems(Graphics* pGraphics);
	void DrawButtons(Graphics* pGraphics, PELC_FLOATBAR_ITEM pItem);
	void DrawLabels(Graphics* pGraphics, PELC_FLOATBAR_ITEM pItem);
	void DrawTip(Graphics* pGraphics);

	int FindItem(UINT nId);
	PELC_FLOATBAR_ITEM FindItemPtr(UINT nId);
	CSize CalcItemsPos();
	void Relayout();

	PELC_FLOATBAR_ITEM GetHitItem(CPoint point);

	void FoldBar(BOOL bFold);
	void ShowBarDirect(BOOL bShow=TRUE);

	void EnablePinButton(BOOL bEnable);
	PELC_FLOATBAR_ITEM FindInnerItemPtr(UINT nId);

	PELC_FLOATBAR_ITEM CreateFloatbarItem(UINT nId, PELC_SKIN_NOTWND_BUTTON pSkinNMButton);

protected:
	enum {
		ID_TIMER_CHECK_STATE	= 0x70000001,
		ID_TIMER_FOLD			= 0x70000002,
	};

	enum FOLD_STATE {
		FOLD_STATE_FOLDED = 1,
		FOLD_STATE_FOLDING,
		FOLD_STATE_EXPAND,
		FOLD_STATE_EXPANDING,
	};

	HWND					m_hNotifyWnd;

	Image*					m_pTitleIcon;
	CString					m_strTitle;
	Rect					m_rcTitle;

	CElcFloatBarItem		m_arrItem;
	CElcFloatBarItem		m_arrInnerItem;
	PELC_FLOATBAR_ITEM		m_pCurItem;

	ELC_SKIN_FLOATBAR		m_skin;
	CRect					m_rcWindow;
	CRect					m_rcDrag;
	CSize					m_sizeBar;
	CSize					m_sizeDropImage;

	int						m_nHMargin;
	int						m_nVMargin;
	int						m_nTipHeight;

	int						m_nButtonMargin;
	int						m_nMaxItemHeight;

	BOOL					m_bEnableAutoHide;
	BOOL					m_bFoldBar;
	int						m_nFactHeight;
	DWORD					m_dwFoldState;
	int						m_nAutoHideCount;
	BOOL					m_bShowButtonText;

	LOGFONT					m_lfLabel;
	LOGFONT					m_lfTitle;
	LOGFONT					m_lfText;
	LOGFONT					m_lfTip;

	BOOL					m_bMirror;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnFlodBar();
	afx_msg void OnPinBar();
};