#pragma once


// CElcStatic

class LIBELCBASEUI_API CElcStatic : public CStatic
{
	DECLARE_DYNAMIC(CElcStatic)

public:
	CElcStatic();
	virtual ~CElcStatic();

	// Set custom color if necessary.
	// The best way is process the function OnCtlColor() in it's parent.
	// if bOn = FALSE, means does not use this color. set color by WM_CTLCOLORSTATIC in it's parent
	void SetTextColor(COLORREF cr, BOOL bOn=TRUE);
	void SetBKColor(COLORREF cr, BOOL bOn=TRUE);

	// Set these colors if you want to draw a custom split line.
	void SetEtchedColor(COLORREF cr1, COLORREF cr2);

	BOOL SetImage(UINT nResId, LPCTSTR lpszType=RT_PNG, BOOL bRealSizeImage=TRUE);

private:

	COLORREF	m_crEtched1;
	COLORREF	m_crEtched2;

	COLORREF	m_crText;
	BOOL		m_bUseTextColor;

	COLORREF	m_crBackgnd;
	BOOL		m_bUseBackgndColor;

	Image *		m_pImage;

	BOOL		m_bSubclass;

protected:
	void Repaint() {
		if (::IsWindow(GetSafeHwnd()))
			Invalidate();
	}

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};


