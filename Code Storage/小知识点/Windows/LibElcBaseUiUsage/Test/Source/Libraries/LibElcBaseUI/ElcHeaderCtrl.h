#pragma once


// CElcHeaderCtrl

class LIBELCBASEUI_API CElcHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CElcHeaderCtrl)

public:
	CElcHeaderCtrl();
	virtual ~CElcHeaderCtrl();

	BOOL LoadSkin(LPCTSTR lpszStyle);


	// set colors
	void SetTextColor(COLORREF cr) {
		m_skin.crText = cr;
	}
	COLORREF GetTextColor() {
		return m_skin.crText;
	}
	void SetBorderColor(COLORREF cr) {
		m_crBorder = cr;
	}
	COLORREF GetBorderColor() {
		return m_crBorder;
	}
	void SetBackgndColor(COLORREF cr) {
		m_crBackgnd = cr;
	}
	COLORREF SetBackgndColor() {
		return m_crBackgnd;
	}

	// set header height
	// call this function before list been created best
	void SetHeaderHeight(int nHeight) {
		m_nHeaderHeight = nHeight;
	}
	int GetHeaderHeight() {
		return m_nHeaderHeight;
	}

	// Set Sort Image
	BOOL SetItemImage(int nCol, int nImage);
	int GetItemImage(int nCol);
	void RemoveSortImage();

protected:
	ELC_SKIN_HEADERCTRL		m_skin;

	COLORREF				m_crBorder;
	COLORREF				m_crBackgnd;
	int						m_nHeaderHeight;
	CFont					m_ftText;
	CRect					m_rcClient;

protected:
	void DrawSimpleHeader(CDC* pDC);
	void DrawSkinHeader(CDC* pDC);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnLayout(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


