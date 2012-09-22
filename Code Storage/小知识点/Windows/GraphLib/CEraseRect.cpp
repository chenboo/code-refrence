#include "stdafx.h"
#include "GraphLib.h"

CEraseRect::CEraseRect(Color clr, HBITMAP hbmp)
{
	if(hbmp == NULL)
	{
		m_crColor = clr;
	}

	m_hBmp = hbmp;
	m_nMinCount = 2;
	m_ptary.SetSize(2, 2);
}

CEraseRect::CEraseRect(const CEraseRect& gph)
{
	CopyTheBaseDataMember(&gph);
}

CEraseRect & CEraseRect::operator = (const CEraseRect& gph)
{
	CopyTheBaseDataMember(&gph);
	return *this;
}

CEraseRect::~CEraseRect()
{
	if(m_hBmp)
	{
		::DeleteObject(m_hBmp);
	}
}

void CEraseRect::Draw(CDC* pDC, CRect& rcInvalid)
{
	if(m_nPtCount <= 1)
	{
		return;
	}
		
	Graphics graph(pDC->m_hDC);

	CPoint ptLT;
	CPoint ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;
	CRect rc(ptLT,ptRB);
	rc.NormalizeRect();
	RectF rcf = CRect2RectF(rc);

	if(m_hBmp)
	{
		Bitmap bm(m_hBmp, NULL);
		graph.DrawImage(&bm, rcf, rcf.X, rcf.Y, rcf.Width, rcf.Height, UnitPixel);
	}
	else
	{
		SolidBrush br(m_crColor);
		graph.FillRectangle(&br, rcf);
	}

	Pen penDraw(Color::Red, 2.0f);
	penDraw.SetDashStyle(DashStyleDash);
	graph.DrawRectangle(&penDraw, rcf);

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}
}

void CEraseRect::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN == nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
	}
	else
	{
		m_ptary[1] = pt;
		m_nPtCount = 2;

		if(MS_LBTNUP == nMouseState)
		{
			m_bFinish = TRUE;
			NormalizeData();
			InitHotData();
			rcUpdate = m_rcHot;
		}
	}
}

void CEraseRect::NormalizeData()
{
	CPoint ptLT, ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;
	CRect rct(ptLT,ptRB);
	rct.NormalizeRect();

	m_ptary[0].X = (float)rct.left;
	m_ptary[0].Y = (float)rct.top;
	m_ptary[1].X = (float)rct.right;
	m_ptary[1].Y = (float)rct.bottom;
}

void CEraseRect::SetRegion()
{
	float left = min(m_ptary[0].X, m_ptary[1].X);
	float top = min(m_ptary[0].Y, m_ptary[1].Y);
	float width = fabs(m_ptary[0].X - m_ptary[1].X);
	float height = fabs(m_ptary[0].Y - m_ptary[1].Y);

	RectF rcf(left, top, width, height);

	m_Region.MakeEmpty();
	m_Region.Union(rcf);
}

void CEraseRect::FinishGraph()
{
	if(2 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

ModifyType CEraseRect::AroundHotPts(CPoint point)
{
	return MT_NONE;
}