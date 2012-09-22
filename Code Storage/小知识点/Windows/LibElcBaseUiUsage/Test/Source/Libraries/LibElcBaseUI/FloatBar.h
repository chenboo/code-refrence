#pragma once


#include "BarButton.h"
#include "VolumeSlider.h"
#include "RollLabel.h"

#include <vector>
using std::vector;

// CFloatBar
enum {
	ITEMTYPE_BUTTON = 1,
	ITEMTYPE_SLIDER,
	ITEMTYPE_ICON,
	ITEMTYPE_TEXT
};

typedef struct tagBUTTON_INFO{
	DWORD			nType;
	DWORD			nID;
	BOOL			bEnable;
	BOOL			bSeparator;
	BOOL			bNoHide;
	CString			strTip;
	CBitmap*		Bitmap[4];
	CRect			rcBtn;
	CBarButton*		pButton;
	CWnd*			pWnd;
	HICON			hIcon;
	
public:
	tagBUTTON_INFO() {
		for (int i=0; i<4; i++)
			Bitmap[i] = NULL;

		pButton = NULL;
		pWnd = NULL;
		hIcon = 0;
	}
}BUTTON_INFO, *PBUTTON_INFO;


typedef vector<BUTTON_INFO> CFloatBarBtnArray;

#define ID_FLOATBAR_BTN_SHOW		WM_APP + 5101
#define ID_FLOATBAR_BTN_HIDE		WM_APP + 5102
#define ID_FLOATBAR_BTN_CLICKED		WM_APP + 5103
#define ID_FLOATBAR_SLIDER_POS		WM_APP + 5104

typedef enum {
	FLOATVAR_ALIGN_NONE = 0x00,
	FLOATBAR_ALIGN_LEFT,
	FLOATBAR_ALIGN_RIGHT,
	FLOATBAR_ALIGN_CENTER
}FLOATBAR_ALIGN_FLAG;

class LIBELCBASEUI_API CFloatBar : public CWnd
{
	DECLARE_DYNAMIC(CFloatBar)

public:
	CFloatBar();
	virtual ~CFloatBar();

public:
	// call these two function at first.
	BOOL Create(CWnd* pParentWnd);
	void SetNotifyWindow(HWND hWnd, UINT uMsg);

	/* call SetShowHideBtn() & AddButton() first, and then call this function.
	   Show(or hide) floatbar
	*/
	void ShowFloadBar(
		BOOL bShow,
		int x, int y, 
		const CWnd* pWndInsertAfter = &wndTopMost
	);
	void ShowFloadBar(
		BOOL bShow = TRUE,
		int nAlign = FLOATBAR_ALIGN_CENTER, 
		const CWnd* pWndInsertAfter = &wndTopMost
	);

	// Set "Show/Hide" Button
	BOOL SetShowHideBtn(
		CBitmap* pbmpShow, 
		CBitmap* pbmpHide, 
		CString strShowTip = _T(""), 
		CString strHideTip = _T("")
	);

	// add "Function" button
	BOOL AddButton(
		CBitmap* pBmp,
		UINT nID,
		CString strTip = _T(""),
		BOOL bSeparator = FALSE,
		BOOL bNoHide = FALSE
	);
	BOOL AddButton(
		UINT nBitmapID, // the button bitmap resource id
		UINT nID, // control id
		LPCTSTR lpszTip = NULL,
		BOOL bSeparator = FALSE,
		BOOL bNoHide = FALSE
	);
	// not implement
	BOOL RemoveButton(UINT nID);

	// add volume slider
	BOOL AddVolumeBar(
		UINT nBarBitmapID, // the slider plate bitmap resource id
		UINT nID, // control's id
		int nRange, // slider range
		int nLevelMax, // electrical level
		int nElapse, // timer value for level
		LPCTSTR lpszTip = NULL,
		int nSliderLength = 50, // slider length in pixel
		BOOL bHorizontal = TRUE,
		BOOL bNoHide = FALSE // if hide when the floatbar folded
	);
	BOOL RemoveVolumeBar(UINT nID); // not implement

	BOOL SetVolumeBarPos(UINT nID, int nPos);
	int GetVulumeBarPos(UINT nID);
	BOOL SetVolumeBarLevel(UINT nID, int nLevel); // electrical level


	// add a icon
	BOOL AddIcon(
		UINT nID,
		HICON hIcon, 
		SIZE size, 
		BOOL bNoHide = FALSE
	);
	BOOL SetIcon(UINT nID, HICON hNewIcon, SIZE size);


	// add a label
	BOOL AddLabel(
		UINT nID,
		LPCTSTR lpszText,
		int nWidth, // length, Pixel
		CFont* pFont = NULL,
		COLORREF crText = RGB(0,0,0),
		BOOL bNoHide = FALSE
	);
	BOOL SetLabelText(UINT nID, LPCTSTR lpszText);
	CString GetLabelText(UINT nID);
	BOOL SetLabelColor(UINT nID, COLORREF crText);
	COLORREF GetLabelColor(UINT nID);
	BOOL SetLabelFont(UINT nID, CFont* pFont);
	CFont* GetLabelTextFont(UINT nID);
	BOOL SetLabelRollRate(UINT nID, int nMilleSec);
	int GetLabelRollRate(UINT nID);
	


	// for recording
	BOOL AddTwinkling(CBitmap* pBmp, int nStateNums);
	void StartTwinkling(int nFreq = 500);
	void StopTwinkling();
	// pause or resume
	void PauseTwinkling(BOOL bPause = TRUE);
	// not implement
	void RemoveTwinkling();


	// Float bar Property
	// background
	void SetBkBitmap(int nActiveID, int nUnactiveID, int nLeftWidth, int nRightWidth);
	
	// can move (default)
	BOOL IsEnableDrag();
	void EnableDrag(BOOL bEnable = TRUE);
	

	// button's state
	BOOL IsButtonCheck(UINT nID);
	void SetButtonCheck(UINT nID, BOOL bCheck = TRUE);
	BOOL IsButtonEnable(UINT nID);
	void EnableButton(UINT nID, BOOL bEnable = TRUE);
	
	// change button's bitmap
	BOOL SetButtonBitmap(UINT nID, CBitmap* pBmp);

	void ResetBarPosition(HMONITOR hMonitor);
	void SetBarPosition(HMONITOR hMonitor);
	void GetButtonRect(UINT nID, RECT* pRect);


	// enable bar auto hide
	void SetBarAutoHide(BOOL bEnable);
	BOOL IsBarAutoHide();

protected:
	void InitButtonInfo(PBUTTON_INFO pBtnInfo);
	void ReleaseButtonInfo(PBUTTON_INFO pBtnInfo);

	BOOL CreateButton(PBUTTON_INFO pBtnInfo, CBitmap* pBmpSrc);
	void HideAllBtn();

	void SetButtonsPos();
	void CalcFloatBarSize(int& cx, int& cy);

	void AnimateExpand(int nCx, BOOL bExpand);
	void SetWndRound();

	void DragMoving(CPoint point);

	BOOL ParseBitmap(CBitmap* pbmSrc, CBitmap* pbmDest[4]);
	BOOL ParseBitmap(
		CBitmap* pbmSrc, 
		CBitmap*& pbmNormal,
		CBitmap*& pbmHover,
		CBitmap*& pbmDown,
		CBitmap*& pbmDisable
	);
	BOOL ParseBitmap(CBitmap* pbmSrc, CBitmap* pbmDest, int nNums);

	void DrawIcon(CDC* pDC);
	void DrawBk(CDC* pDC);
	void SetBkDC();

	void DrawTwinkling(CDC* pDC);	

	virtual BOOL OnSliderPosChanged(UINT nID, int nPos);

	BOOL IsBarDocked();
	void AutoHideBar(BOOL bHide);
	void ShowBarDirect();

private:
	enum ID_TIMER {
		ID_TIMER_NONE = 0x00000000,
		ID_TIMER_TWINKLING = 0x00000001,
		ID_TIMER_HIDE = 0x00000002,
		ID_TIMER_TRACE_MOUSE = 0x00000004,
	};

	enum AUTOHIDE_STATE {
		AUTOHIDE_STATE_HIDE = 1,
		AUTOHIDE_STATE_HIDING,
		AUTOHIDE_STATE_SHOW,
		AUTOHIDE_STATE_SHOWING,
	};

	HWND	m_hNotifyHwnd;
	UINT	m_uNotifyMsg;

	CDC			m_dcDrawBk;
	CBitmap		m_bmActive[3];
	CBitmap		m_bmNA[3];
	CDC			m_dcActive[3];
	CDC			m_dcNA[3];
	int			m_nLeftWidth;
	int			m_nRightWidth;
	int			m_nCenterWidth;

	BOOL	m_bActive;

	BOOL	m_bMirror;
	BOOL	m_bExpandBar;
	CRect	m_rcClient;
	int		m_nAlignFlag;

	int		m_nDefaultbtnCx;
	int		m_nDefaultbtnCy;
	int		m_nSeparatorWidth;
	int		m_nBtnSpace;

	int		m_nFloatBarWidth;
	int		m_nFloatBarHeight;

	CPoint	m_ptOldPoint;
	CPoint	m_ptDown;
	BOOL	m_bLBtnDown;
	BOOL	m_bEnableDrag;

	BUTTON_INFO			m_BtnShow;
	BUTTON_INFO			m_BtnHide;
	CFloatBarBtnArray	m_arrBarBtn;


	CBitmap*	m_pbmTwinkling;
	int			m_nStateNums;
	int			m_nCurState;
	int			m_nFrequency;
	BOOL		m_bShowTwinkling;
	BOOL		m_bTimerOn;
	CRect		m_rcTwinkling;
	CString		m_strTime;
	CRect		m_rcTime;
	DWORD		m_dwOldTime;
	DWORD		m_dwEscapeTime;

	CToolTipCtrl	m_wndTooltip;

	BOOL		m_bEnableAutoHide;
	BOOL		m_bHideBar;
	int			m_nHideTopPos;
	DWORD		m_dwHideState;

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	LRESULT OnFloatBarBtnCliked(WPARAM wParam, LPARAM lParam);
	LRESULT OnDisplayChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnFloatBarSliderPosChanged(WPARAM wParam, LPARAM lParam);
};


