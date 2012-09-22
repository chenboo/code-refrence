#pragma once


// CVolumeSlider

class LIBELCBASEUI_API CVolumeSlider : public CWnd
{
	DECLARE_DYNAMIC(CVolumeSlider)

public:
	CVolumeSlider();
	virtual ~CVolumeSlider();

	// u can set "nCmdID=0"
	BOOL Create(
		const RECT& rect, 
		CWnd* pParentWnd, 
		UINT nID, 
		UINT nCmdID, 
		BOOL bHorizontal);
	
	// use postmessage to notify 
	// if u not want to use Command at " Create() -- nCmdID "
	// this function is written for the floatbar
	// the wParam special the Bar ID.
	// the lParam special the Bar Pos
	// send to the parent window
	void SetPosChangedNotify(UINT nMessage);
	// send message to the specified window
	void SetPosChangedNotify(HWND hWnd, UINT nMessage);

	// if postmessage when moving the thumb
	// default value is enable;
	void EnableNotifyMoving(BOOL bEnable);

public:
	void SetRange(int nRange);
	int GetRange();

	void SetPos(int nPos);
	int GetPos();

	void SetFillPos(int nPos);
	int GetFillPos();

	void SetLevel(int nLevel);
	void SetLevelMax(int nMax);
	int GetLevelRange();
	void SetLevelElapse(int nElapse);
	// set a bitmap for spectrum
	BOOL SetLevelImage(UINT nBitmap);

	// set Slider Bar and the Slider body's height (or width if vertical)
	// note: if u have set slider bk image, the "nSliderThickness" is ignored
	BOOL SetSliderBar(int nBarID, int nSliderThickness);
	void SetSliderThickness(int nSliderThickness);
	int GetSliderThickness();

	///////////////////////
	// Set border color 
	void SetBorderColor(COLORREF cr);
	COLORREF GetBorderColor();
	// Set Slider body color
	void SetSliderBodyColor(COLORREF cr);
	COLORREF GetSliderBodyColor();
	// Set background color
	void SetBkColor(COLORREF color);
	COLORREF GetBkColor();

	// set a background image for slider 
	// ignore the background color if the image is set
	// this draw mode is not support electrical level
	void SetSliderImage(UINT nImageID, int nHead, int nTail);
	void EnableBkImage(BOOL bOn = TRUE);

	void SetFillPosColor(COLORREF cr);
	COLORREF GetFillPosColor();

	// if draw scale
	void SetDrawScale(BOOL bDraw);
	BOOL IsDrawScale();
	void SetScaleColor(COLORREF color);
	COLORREF GetScaleColor();

	void EnableDrag(BOOL bEnable);
	BOOL IsDraging();

	void SetAutoFill(BOOL bAuto);
	BOOL IsAutoFill();

	BOOL EnableWindow(BOOL bEnable = TRUE);

protected:
	BOOL ParseBitmap(CBitmap* pbmSrc, CBitmap* pbmDest, int nNums);
	BOOL ParseBitmap(CBitmap* pbmSrc, CBitmap* pbmDest, CPoint ptSrc, CSize sDest);
	void Repaint();
	void RepaintLevel();

	void ReCalcSize();
	void CalcBarPos(CPoint point);

	void DrawBody(CDC* pDC);
	void DrawNormalBk(CDC* pDC);
	void DrawBkImage(CDC* pDC);
	void DrawBar(CDC* pDC);
	void DrawScale(CDC* pDC);

	void CreateSpectrumDC();

	double Round(double _x, int decimalnum);

private:
	enum BAR_STATE{
		BAR_NORMAL = 0,
		BAR_HOVER=1,
		BAR_DOWN=2,
		BAR_DISABLE=3
	};
	enum {
		ID_TIMER_LEVEL = 0x70000001,
		ID_TIMER_MOUSE_CHECK
	};

	COLORREF m_crBorder;
	COLORREF m_crBorderDisable;
	COLORREF m_crBackgnd;
	COLORREF m_crLevel;
	COLORREF m_crHightLevel;
	COLORREF m_crBody;
	COLORREF m_crBodyDisable;
	COLORREF m_crFill;
	COLORREF m_crFillDisable;
	COLORREF m_crScale;

	int m_nRange;
	int m_nPos;
	int m_nFillPos;
	int m_nLevel;
	int m_nLevelTop;
	int m_nLevelMax;
	BOOL m_bTimer;

	CDC	m_dcLevelSrc;
	CDC	m_dcLevel;
	BITMAP m_bmInfoLevel;

	int m_nLevelElapse;

	BOOL m_bMouseCheckTimer;

	UINT m_nID;
	UINT m_nCmdID;
	BOOL m_bHorizontal;

	HWND m_hParent;
	HWND m_hNotify;
	UINT m_nMsg;

	CSize	m_sBack[3];
	CBitmap m_bmBack[3];
	CDC		m_dcBack[3];
	BOOL	m_bEnableBKImage;

	CBitmap m_bmBar[4];
	BITMAP m_bmBarInfo;
	int m_nBarState;
	int m_nSliderThick;
	int m_nBarPos;

	CRect m_rcClient;
	CRect m_rcBody;
	CRect m_rcThumb;

	int m_bRepaint;	

	BOOL m_bLBDown;
	BOOL m_bDrawScale;
	BOOL m_bNotifyMoving;
	BOOL m_bEnableDrag;
	BOOL m_bAutoFill;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
};


