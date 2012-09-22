#pragma once
// CIconButton

class CIconButton : public CButton
{
	DECLARE_DYNAMIC(CIconButton)

public:
	CIconButton();
	virtual ~CIconButton();

public:
	void SetIcon(UINT nIcon);
	void SetBitmap(UINT nBmID);
	
	void SizeToContent();
	void DrawBackGround(CDC* pDC);
	void DrawIcon(CDC* pDC,CRect& rect);
	void DrawBitmap(CDC* pDC,CRect& rect);

public:
	HICON					m_hIcon;
	CBitmap					m_bitmap;


	int					m_nImageWidth;
	int					m_nImageHeight;

	BOOL					m_bhover;
	BOOL					m_bToggle;
	BOOL					m_bTracking;

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
};


