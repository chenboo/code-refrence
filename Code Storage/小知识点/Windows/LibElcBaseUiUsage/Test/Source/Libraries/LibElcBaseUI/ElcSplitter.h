#pragma once




class LIBELCBASEUI_API CElcSplitter : public CWnd
{
	DECLARE_DYNAMIC(CElcSplitter)

public:
	CElcSplitter();
	virtual ~CElcSplitter();

	BOOL Create(CWnd* pParent, UINT nID, const RECT& rect, BOOL bHorizontal=TRUE);
	void SetNotifyWindow(HWND hWnd, UINT nMessage);

	void SetThickness(int nThickness);
	int GetThickness();
	
	void SetRange(int nStart, int nEnd);
	void GetRange(int& nStart, int& nEnd);

	void SetPos(int nPos); // start pos, notice: the splitter body's thickness
	int GetPos();
	
	void SetBkColor(COLORREF cr);
	COLORREF GetBkColor();
	void SetBorderColor(COLORREF cr);
	COLORREF GetBorderColor();

	void SetCustomCursor(HINSTANCE hInstance, UINT nResID);
	void UseDesktopDC(BOOL bUse);

protected:
	void InverTracker(const POINT& point);
	void CheckRange(CPoint& point);
	void GetRelativePos(CRect& rc);

protected:
	CWnd*		m_pParent;
	HCURSOR		m_hCursor;
	BOOL		m_bUseDesktopDC;

	BOOL		m_bHorizontal;
	BOOL		m_bLBDown;

	CPoint		m_ptOld;
	CPoint		m_ptOffset;

	int			m_nStartPos;
	int			m_nEndPos;

	CRect		m_rcClient;

	COLORREF	m_crBk;
	COLORREF	m_crBorder;

	HWND		m_hNotifyWnd;
	UINT		m_nNotifyMsg;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
};


