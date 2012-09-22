#include "StdAfx.h"
#include "GraphLib.h"

CFlatBrushPen::CFlatBrushPen()
{
	m_nWidth = 10;
	m_nMinCount = 2;
	m_ptary.SetSize(0,10);
	m_crColor = Color::Black;
	m_ftransAngle = 45.0f;

	m_OrgTop = 0;
	m_OrgLeft = 0;
	m_nOrgWidth = 0;
	m_nOrgHeight = 0;
	m_Region.MakeEmpty();
}

CFlatBrushPen::CFlatBrushPen(Color cr, int nWidth, float fTransAngle)
{
	m_crColor = cr;
	m_nMinCount = 2;
	m_nWidth = nWidth;
	m_ptary.SetSize(0,10);
	m_ftransAngle = fTransAngle;

	m_OrgTop = 0;
	m_OrgLeft = 0;
	m_nOrgWidth = 0;
	m_nOrgHeight = 0;
	m_Region.MakeEmpty();
}

CFlatBrushPen::CFlatBrushPen(const CFlatBrushPen& gph)
{
	CopyTheBaseDataMember(&gph);

	m_OrgTop = gph.m_OrgTop;
	m_OrgLeft = gph.m_OrgLeft;
	m_nOrgWidth = gph.m_nOrgWidth;
	m_nOrgHeight = gph.m_nOrgHeight;
	m_ftransAngle = gph.m_ftransAngle;
}

CFlatBrushPen& CFlatBrushPen::operator = (const CFlatBrushPen& gph)
{
	CopyTheBaseDataMember(&gph);

	m_nMinCount = 2;
	m_OrgTop = gph.m_OrgTop;
	m_OrgLeft = gph.m_OrgLeft;
	m_nOrgWidth = gph.m_nOrgWidth;
	m_nOrgHeight = gph.m_nOrgHeight;
	m_ftransAngle = gph.m_ftransAngle;
	return *this;
}

CFlatBrushPen::~CFlatBrushPen()
{
}

void CFlatBrushPen::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount <= 1)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeHighQuality);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	Pen penDraw(m_crColor, (float)m_nWidth * m_fScale);
	penDraw.ScaleTransform(1, 2.0f / m_nWidth, MatrixOrderAppend);
	penDraw.RotateTransform(m_ftransAngle, MatrixOrderAppend);

	if(m_bSelected || m_bEdit)
	{
		graph.SetSmoothingMode(SmoothingModeAntiAlias);
		DrawHotShape(graph);
	}

	if(!m_bFinish)
	{
		Rect	rc(rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height());
		graph.SetClip(rc);
	}

	graph.ExcludeClip(&m_RgnErase);

	PointF* pPt = m_ptary.GetData();
	graph.DrawLines(&penDraw, pPt, m_nPtCount);
}

void CFlatBrushPen::FinishGraph()
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	m_bFinish = TRUE;
	InitHotData();
}

void CFlatBrushPen::SetRegion()
{
	ASSERT(m_bFinish);
	PointF* pPt = m_ptary.GetData();
	float fwidth = m_nWidth * m_fScale;

	Pen penDraw(m_crColor, fwidth);
	penDraw.ScaleTransform(1, 2.0f / fwidth, MatrixOrderAppend);
	penDraw.RotateTransform(m_ftransAngle, MatrixOrderAppend);

	GraphicsPath path;
	path.AddCurve(pPt, m_nPtCount);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CFlatBrushPen::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x, (float)point.y);

	if(m_nPtCount == 0)
	{
		m_ptary.Add(pt);
		m_nPtCount++;
	}
	else
	{
		PointF ptf = m_ptary[m_nPtCount - 1];
		if(CalcGapOfpoints(pt, ptf) >= 3)
		{
			m_ptary.Add(pt);
			m_nPtCount++;
		}
	}

	if(m_nPtCount >= 2 && !m_bFinish)
	{
		CPoint ptLT((int)m_ptary[m_nPtCount - 1].X, (int)m_ptary[m_nPtCount - 1].Y);
		CPoint ptRB((int)m_ptary[m_nPtCount - 2].X, (int)m_ptary[m_nPtCount - 2].Y);

		rcUpdate.SetRect(ptLT, ptRB);
		rcUpdate.NormalizeRect();

		rcUpdate.InflateRect(3 * m_nWidth, 3 * m_nWidth);
	}

	if(MS_LBTNUP == nMouseState)
	{
		if(( m_nPtCount <= 1) ||
			( m_nPtCount <= 2 && 
			CalcGapOfpoints(m_ptary[0],m_ptary[1]) <= 3))
		{
			m_bDataInvalid = TRUE;
		}
		else
		{
			m_bFinish = TRUE;
			InitHotData();
			rcUpdate = m_rcHot;
		}
	}
}

void CFlatBrushPen::InitHotData()
{
	__super::InitHotData();
}

void CFlatBrushPen::SetTransAngle(float fTransAngle)
{
	m_ftransAngle = fTransAngle;
}