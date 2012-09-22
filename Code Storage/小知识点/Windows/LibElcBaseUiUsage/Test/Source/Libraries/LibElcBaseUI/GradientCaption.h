#pragma once

class LIBELCBASEUI_API CGradientCaption : public CWnd
{
public:
	CGradientCaption();
	virtual ~CGradientCaption();

public:
	void SetTextColor(COLORREF clrText) { m_clrText = clrText; }
	COLORREF GetTextColor() const { return m_clrText; }

	COLORREF GetGradientStart() const { return m_clrStart; }
	COLORREF GetGradientEnd() const { return m_clrEnd; }

	void SetGradientStart(COLORREF clrStart) { m_clrStart = clrStart; }
	void SetGradientEnd(COLORREF clrEnd) { m_clrEnd = clrEnd; }

protected:
	void DrawGradient(CDC* pDC, CRect& rcGrad, COLORREF clrStart, COLORREF clrEnd);

protected:
	COLORREF m_clrStart;
	COLORREF m_clrEnd;
	COLORREF m_clrText;
	CString  m_strText;
	
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
};

