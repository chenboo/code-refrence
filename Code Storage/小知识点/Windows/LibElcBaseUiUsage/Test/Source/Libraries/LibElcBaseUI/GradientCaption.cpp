#include "stdafx.h"
#include "GradientCaption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGradientCaption

CGradientCaption::CGradientCaption()
{
	m_clrStart = GetSysColor(COLOR_3DDKSHADOW);
	m_clrEnd = GetSysColor(COLOR_3DHIGHLIGHT);
	m_clrText = GetSysColor(COLOR_CAPTIONTEXT);
}

CGradientCaption::~CGradientCaption()
{
}

BEGIN_MESSAGE_MAP(CGradientCaption, CWnd)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGradientCaption message handlers

void CGradientCaption::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectClient;
	GetClientRect(&rectClient);

	DrawGradient(&dc, rectClient, m_clrStart, m_clrEnd);

	int nOldMode = dc.SetBkMode(TRANSPARENT);
	COLORREF clrOld = dc.SetTextColor(m_clrText);
	
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.left += 10;

	CFont *pOldFont = (CFont *)dc.SelectObject(GetFont());
	dc.DrawText(m_strText, &rcClient, DT_VCENTER | DT_LEFT | DT_NOCLIP | DT_SINGLELINE);
	dc.SelectObject(pOldFont);

	dc.SetBkMode(nOldMode);
	dc.SetTextColor(clrOld);
}

BOOL CGradientCaption::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CGradientCaption::DrawGradient(CDC *pDC, CRect &rcGrad, COLORREF clrStart, COLORREF clrEnd)
{
	float  fStep;
	CRect  rectFill;
	CBrush brush;
	
	if(clrStart == clrEnd) {
		pDC->FillSolidRect(rcGrad.left,rcGrad.top,rcGrad.Width(),rcGrad.Height(),clrStart);
		return;
	}

	int r, g, b;
	float rStep, gStep, bStep;

	r = (GetRValue(clrEnd) - GetRValue(clrStart));
	g = (GetGValue(clrEnd) - GetGValue(clrStart));
	b = (GetBValue(clrEnd) - GetBValue(clrStart));

	int nSteps = max(abs(r), max(abs(g), abs(b)));

	fStep = ((float)abs(rcGrad.right - rcGrad.left)) / (float)nSteps;

	rStep = r / (float)nSteps;
	gStep = g / (float)nSteps;
	bStep = b / (float)nSteps;

	r = GetRValue(clrStart);
	g = GetGValue(clrStart);
	b = GetBValue(clrStart);

	for (int iOnBand = 0; iOnBand < nSteps; iOnBand++) {
		rectFill.SetRect((int)(rcGrad.left + iOnBand * fStep), 
			rcGrad.top, rcGrad.left+(int)((iOnBand+1) * fStep), rcGrad.bottom);

		if (rectFill.left < rcGrad.left || rectFill.right > rcGrad.right)
			return;

		pDC->FillSolidRect(rectFill.left, rectFill.top, rectFill.Width(), rectFill.Height(), 
			RGB(r + rStep * iOnBand, g + gStep * iOnBand, b + bStep * iOnBand));
	}	
}

LONG CGradientCaption::OnSetText(WPARAM wParam, LPARAM lParam)
{
	m_strText = (LPCTSTR)lParam;
	Invalidate();
	return TRUE;
}