#include "stdafx.h"
#include "GraphLib.h"

CNormalPen::CNormalPen(LINETYPE LineType)
{
	m_noffst = 0;
	m_nMinCount = 2;
	m_ptary.SetSize(0,10);

	m_nWidth = LineType.nWidth;
	m_crColor = LineType.crColor;
	m_nEndCap = LineType.nEndCap;
	m_nStartCap = LineType.nStartCap;
	m_nDashStyle = LineType.nDashStyle;
}

CNormalPen::CNormalPen()
{
	m_noffst = 0;
	m_nMinCount = 2;
	m_ptary.SetSize(0,10);
}

CNormalPen::CNormalPen(const CNormalPen& gph)
{
	CopyTheBaseDataMember(&gph);
	
	m_noffst = gph.m_noffst;
	m_nEndCap = gph.m_nEndCap;
	m_nStartCap = gph.m_nStartCap;
	m_nDashStyle = gph.m_nDashStyle;
}

CNormalPen & CNormalPen::operator = (const CNormalPen& gph)
{
	CopyTheBaseDataMember(&gph);
	
	m_noffst = gph.m_noffst;
	m_nEndCap = gph.m_nEndCap;
	m_nStartCap = gph.m_nStartCap;
	m_nDashStyle = gph.m_nDashStyle;
	
	return *this;
}

CNormalPen::~CNormalPen()
{
}

void CNormalPen::SetLineType(LINETYPE LineType)
{
	m_nWidth = LineType.nWidth;
	m_crColor = LineType.crColor;
	m_nEndCap = LineType.nEndCap;
	m_nStartCap = LineType.nStartCap;
	m_nDashStyle = LineType.nDashStyle;
}

void CNormalPen::SetDashStyle(DashStyle nDashStyle)
{
	m_nDashStyle = nDashStyle;

	if(m_bFinish)
	{
		SetRegion();
	}
}

void CNormalPen::SetStartCap(LineCap nStartCap)
{
	m_nStartCap = nStartCap;

	if(m_bFinish)
	{
		SetRegion();
	}
}

void CNormalPen::SetEndCap(LineCap nEndCap)
{
	m_nEndCap = nEndCap;

	if(m_bFinish)
	{
		SetRegion();
	}
}

void CNormalPen::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount == 0)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);
	Rect rc(rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height());

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}

	graph.ExcludeClip(&m_RgnErase);
	
	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetStartCap(m_nStartCap);
	penDraw.SetLineJoin(LineJoinRound);

	int nCount = (int)m_ptary.GetCount();
	PointF* pPt = m_ptary.GetData();

	if(!m_bFinish)
	{
		graph.SetClip(rc);

		if(DashStyleSolid == m_nDashStyle)
		{
			penDraw.SetEndCap(m_nEndCap);
		}
	}
	else
	{
		penDraw.SetEndCap(m_nEndCap);
		penDraw.SetWidth(m_fScale * m_nWidth);
	}

	graph.DrawCurve(&penDraw, pPt, nCount);
}

void CNormalPen::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);

	if(m_nPtCount == 0)
	{
		m_ptary.Add(pt);
		m_nPtCount++;
	}
	else
	{
		PointF ptf = m_ptary[m_nPtCount - 1];
		if(CalcGapOfpoints(pt, ptf) >= 4)
		{
			m_ptary.Add(pt);
			m_nPtCount++;
		}
	}

	if(MS_LBTNUP == nMouseState)
	{
		if(( m_nPtCount <= 1) ||
			( m_nPtCount <= 2 && 
			CalcGapOfpoints(m_ptary[0],m_ptary[1]) <= 3 ))
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

	if(m_nPtCount >= 2 /*&& !m_bFinish*/)
	{
		CPoint ptLT((int)m_ptary[m_nPtCount - 1].X, (int)m_ptary[m_nPtCount - 1].Y);
		CPoint ptRB((int)m_ptary[m_nPtCount - 2].X, (int)m_ptary[m_nPtCount - 2].Y);

		rcUpdate.SetRect(ptLT, ptRB);
		rcUpdate.NormalizeRect();
		rcUpdate.InflateRect(3 * m_nWidth, 3 * m_nWidth);
	}
}

void CNormalPen::FinishGraph()
{
	m_bFinish = TRUE;
	InitHotData();
	return;
}

void CNormalPen::SetRegion()
{
	ASSERT(m_bFinish);
	PointF* pPt = m_ptary.GetData();
	Pen penDraw(m_crColor);
	penDraw.SetWidth(m_nWidth * m_fScale);

	penDraw.SetEndCap(m_nEndCap);
	penDraw.SetStartCap(m_nStartCap);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetLineJoin(LineJoinRound);

	GraphicsPath path;
	path.AddCurve(pPt,m_nPtCount);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}
	
DashStyle CNormalPen::GetNormalPenDashStyle()
{
	return m_nDashStyle;
}
