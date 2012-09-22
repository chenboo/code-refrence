/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////



#pragma once

#include <vector>
using std::vector;

// CElcBaseBar

// bar layout
enum ELCBAR_LAYOUT
{
	ELCBAR_LAYOUT_HORIZONTAL	= 1,
	ELCBAR_LAYOUT_VERTICAL		= 2,
};

// child controls' alignment
// if is a vertical bar: lift=top, right=bottom
enum ELCBAR_ALIGN
{
	ELCBAR_ALIGN_LEFT		= 1,
	ELCBAR_ALIGN_RIGHT		= 2,
	ELCBAR_ALIGN_STRETCH	= 3,
};

// control's type
enum ELCBAR_CTRL_TYPE
{
	ELCBAR_TYPE_CONTROL		= 1,
	ELCBAR_TYPE_SEPARATOR	= 2,
	ELCBAR_TYPE_SPACE		= 3,
};

// draw border
enum ELCBAR_BORDER
{
	ELCBAR_BORDER_LEFT		= 0x00000001,
	ELCBAR_BORDER_RIGHT		= 0x00000002,
	ELCBAR_BORDER_TOP		= 0x00000004,
	ELCBAR_BORDER_BOTTOM	= 0x00000008,

	ELCBAR_BORDER_ALL		= 0x0000000F,
};

enum ELCBAR_BACKGROUND_STATE
{
	ELCBAR_BK_SINGLE = 1,
	ELCBAR_BK_GRADIENT,
	ELCBAR_BK_GRADIENT_TOP,
	ELCBAR_BK_GRADIENT_BOTTOM,
	ELCBAR_BK_GRADIENT_TOP_BOTTOM
};


class LIBELCBASEUI_API CElcBaseBar : public CWnd
{
	DECLARE_DYNAMIC(CElcBaseBar)

public:
	typedef struct ELCBAR_ITEM
	{
		UINT		nType;
		DWORD_PTR	dwCtrl;
		CRect		rect;
		BOOL		bShow;

	public:
		ELCBAR_ITEM()
		{
			nType = 0;
			dwCtrl = 0;
			bShow = TRUE;
			rect.left = 0;
			rect.top = 0;
			rect.right = 0;
			rect.bottom = 0;
		}
	}*PELCBAR_ITEM;

	typedef vector<ELCBAR_ITEM> CItemArray;

public:
	CElcBaseBar();
	virtual ~CElcBaseBar();

	BOOL LoadSkin(LPCTSTR lpStyle, BOOL bRedraw=TRUE);

	// add controls: child window
	BOOL AddItem(CWnd* pWnd, CSize size, UINT nAlign, BOOL bShow=TRUE);
	void DelItem(CWnd* pWnd);

	void AddSeparator(UINT nID, UINT nAlign, BOOL bShow=TRUE);
	void DelSeparator(UINT nID);

	void AddSpace(UINT nID, UINT nAlign, int nSpace, BOOL bShow=TRUE);
	void DelSpace(UINT nID);
	// change the "Space" width
	void SetSpace(UINT nID, int nSpace);

	// notice : the nID is various with different item
	// The Window item : nID = CWnd*
	// The Separator & Space item : nID = nID
	BOOL SetItemSize(UINT_PTR nID, CSize size, BOOL bReCalc=TRUE);
	CSize GetItemSize(UINT_PTR nID);

	// add tip for controls
	void EnableToolTip();
	void AddToolTip(CWnd* pWnd, UINT nIDText);
	void AddToolTip(CWnd* pWnd, LPCTSTR lpszText);
	void DelToolTip(CWnd* pWnd);

	
	// for window control
	BOOL ShowItem(CWnd* pWnd, BOOL bShow);
	// for Separator
	BOOL ShowItem(UINT nID, BOOL bShow);

	//////////////////////////////////////////////
	// set bar properties

	// specify a Horiztal bar or a Vertical bar
	void SetBarLayout(ELCBAR_LAYOUT layout);

	// see the enum ELCBAR_BORDER
	void SetBarBorder(UINT nBorder);
	void SetBarBorderColor(COLORREF cr);
	COLORREF GetBarBorderColor();

	void SetBkColor(COLORREF cr);
	COLORREF GetBkColor();

	void SetBackgndImage(UINT nResId, LPCTSTR lpszType, int left=-1, int top=-1, int right=-1, int bottom=-1);
	
	void SetSeparatorColor(COLORREF crLeft, COLORREF crRight);
	void GetSeparatorColor(COLORREF& crLeft, COLORREF& crRight);

	void SetCtrlsSpace(int nSpace);
	int GetCtrlsSpace();

	void SetBarMargin(int nWidth);
	int GetBarMargin();

	// not include the STRETCH Control's width
	// if specify ELCBAR_LAYOUT_VERTICAL layout,
	// the return value is the Height of the bar.
	int GetBarWidth();

	// Set Enable Moving bar
	void SetEnableDrag(BOOL bEnable=TRUE);
	BOOL IsEnableDrag();

	// enable bar auto hide
	void SetBarAutoHide(BOOL bEnable);
	BOOL IsBarAutoHide();

protected:
	BOOL AddCtrl(UINT nType, DWORD_PTR dwCtrl, CRect rect, UINT nAlign, BOOL bShow);
	BOOL DelCtrl(DWORD_PTR dwCtrl);

	int FindItem(DWORD_PTR dwCtrl, BOOL bLeftArray);

	void SetCtrlsPos();
	void SetCtrlsHPos();
	void SetCtrlsVPos();

	void FoldBar(BOOL bFold);
	void ShowBarDirect();

	void StartBarTimer(UINT_PTR nIDEvent);
	void StopBarTimer(UINT_PTR nIDEvent);

	void DragMoving(CPoint point);

	BOOL IsBarDocked();

	//////////////////////////////////
	// draw functions
	void DrawBackground(CDC* pDC);
	void DrawGradienRect(CDC* pDC, CPoint ptStart, CPoint ptEnd, COLORREF crStart, COLORREF crEnd);

	void DrawSeparator(CDC* pDC);
	void DrawSeparator(CDC* pDC, CRect rect);

	void DrawBorder(CDC* pDC);

protected:
	enum ID_TIMER {
		ID_TIMER_NONE = 0x00000000,
		ID_TIMER_FOLD = 0x70000001,
		ID_TIMER_TRACE_MOUSE = 0x70000002,
	};

	enum FOLD_STATE {
		FOLD_STATE_FOLDED = 1,
		FOLD_STATE_FOLDING,
		FOLD_STATE_EXPAND,
		FOLD_STATE_EXPANDING,
	};


	CItemArray		m_arrLeftItem;
	CItemArray		m_arrRightItem;
	ELCBAR_ITEM		m_stretchItem;

	ELCBAR_LAYOUT	m_barLayout;
	UINT			m_nBarBorder;

	int				m_nCtrlsSpace;
	int				m_nBarMargin;
	int				m_nBarWidth;
	
	CRect			m_rcClient;

	BOOL			m_bUseGradienBk;
	BOOL			m_bPopupWindow;

	COLORREF		m_crBackgnd;
	CElcMemDC		m_dcBkMem;

	CToolTipCtrl	m_wndToolTip;

	HCURSOR			m_hCursor;

	CPoint			m_ptOldPoint;
	CPoint			m_ptDown;
	BOOL			m_bLBtnDown;
	BOOL			m_bEnableDrag;
	BOOL			m_bMirror;

	BOOL			m_bFoldBar;
	BOOL			m_bEnableAutoHide;

	int				m_nFactHeight;
	DWORD			m_dwFoldState;
	DWORD			m_dwTimerId;

	ELC_SKIN_TOOLBAR	m_skin;

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();

	afx_msg LRESULT OnElcUIDrawNotify(WPARAM wParam, LPARAM lParam);
};


