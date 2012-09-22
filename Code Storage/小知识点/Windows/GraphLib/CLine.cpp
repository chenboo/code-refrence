#include "stdafx.h"
#include "GraphLib.h"

CLine::CLine(LINETYPE LineType)
{
	m_nPtCount = 0;
	m_nMinCount = 2;
	m_ptary.SetSize(2,2);
	m_nWidth = LineType.nWidth;
	m_crColor = LineType.crColor;
	m_nEndCap = LineType.nEndCap;
	m_nStartCap = LineType.nStartCap;
	m_nDashStyle = LineType.nDashStyle;
}

CLine::CLine()
{
}

CLine::CLine(const CLine& gph)
{
	CopyTheBaseDataMember(&gph);
	
	m_nEndCap = gph.m_nEndCap;
	m_nStartCap = gph.m_nStartCap;
	m_nDashStyle = gph.m_nDashStyle;
}

CLine & CLine::operator = (const CLine& gph)
{
	CopyTheBaseDataMember(&gph);
	
	m_nEndCap = gph.m_nEndCap;
	m_nStartCap = gph.m_nStartCap;
	m_nDashStyle = gph.m_nDashStyle;
	
	return* this;
}

CLine::~CLine()
{
}

void CLine::SetLineType(LINETYPE LineType)
{
	m_nWidth = LineType.nWidth;
	m_crColor = LineType.crColor;
	m_nDashStyle = LineType.nDashStyle;
	m_nStartCap = LineType.nStartCap;
	m_nEndCap = LineType.nEndCap;
}

void CLine::SetDashStyle(DashStyle nDashStyle)
{
	m_nDashStyle = nDashStyle;

	if(m_bFinish)
	{
		SetRegion();
	}
}

void CLine::SetStartCap(LineCap nStartCap)
{
	m_nStartCap = nStartCap;

	if(m_bFinish)
	{
		SetRegion();
	}
}

void CLine::SetEndCap(LineCap nEndCap)
{
	m_nEndCap = nEndCap;

	if(m_bFinish)
	{
		SetRegion();
	}
}

void CLine::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Pen penDraw(m_crColor, (float)m_nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetStartCap(m_nStartCap);
	penDraw.SetEndCap(m_nEndCap);
	penDraw.SetLineJoin(LineJoinRound);

	if(m_nPtCount == 1)
	{
		graph.DrawLine(&penDraw, m_ptary[0], m_ptary[0]);
	}
	else
	{
		graph.ExcludeClip(&m_RgnErase);
		graph.DrawLine(&penDraw, m_ptary[0], m_ptary[1]);

		if(m_bSelected || m_bEdit)
		{
			DrawHotShape(graph);
		}
	}
}

void CLine::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x, (float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
	}
	else if(m_nPtCount >= 1)
	{
		m_ptary[1] = pt;
		m_nPtCount = 2;

		if(MS_LBTNUP == nMouseState)
		{
			if(CalcGapOfpoints(m_ptary[0], m_ptary[1]) <= 3)
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
	else
	{
		rcUpdate.SetRect(0, 0, 1, 1);
	}
}

void CLine::FinishGraph()
{
	if(2 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void CLine::SetRegion()
{
	Pen penDraw(m_crColor);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetStartCap(m_nStartCap);
	penDraw.SetEndCap(m_nEndCap);
	penDraw.SetLineJoin(LineJoinRound);
	penDraw.SetWidth((float)m_nWidth);

	GraphicsPath path;
	path.AddLine(m_ptary[0],m_ptary[1]);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CLine::DecideModiFyType(UINT nMouseState,CPoint point)
{
	if(nMouseState != MS_LBTNDOWN)
		return;

	PointF pt((float)point.x,(float)point.y);

	if(CalcGapOfpoints(pt,m_ptary[0]) <= m_nWidth)
	{
		m_ModifyType = MT_LT;
	}
	else if(CalcGapOfpoints(pt,m_ptary[1]) <= m_nWidth)
	{
		m_ModifyType = MT_RB;
	}
	else if(m_Region.IsVisible(pt))
	{
		m_ModifyType = MT_MOVE;
		m_ptModifyOrg.X = (float)point.x;
		m_ptModifyOrg.Y = (float)point.y;
	}
	else
	{
		m_ModifyType = MT_NONE;
	}
}

void CLine::ModifyGraph(UINT nMouseState,CPoint point)
{
	if(MS_LBTNMOUSE != nMouseState)
	{
		return;
	}

	if(MT_MOVE == m_ModifyType)
	{
		MoveGraph(point);
	}
	else if(MT_NONE != m_ModifyType)
	{
		StretchGraph(point);
	}
}

void CLine::StretchGraph(CPoint point)
{
	if(MT_LT == m_ModifyType)
	{
		m_ptary[0].X = (float)point.x;
		m_ptary[0].Y = (float)point.y;
	}
	else if(MT_RB == m_ModifyType)
	{
		m_ptary[1].X = (float)point.x;
		m_ptary[1].Y = (float)point.y;
	}

	m_rcHot.SetRect((int)m_ptary[0].X, (int)m_ptary[0].Y,
		(int)m_ptary[1].X, (int)m_ptary[1].Y);
	m_rcHot.NormalizeRect();
	m_rcHot.InflateRect(HOTINTERVAL,HOTINTERVAL);

	m_ptCenter.X = (float)m_rcHot.CenterPoint().x;
	m_ptCenter.Y = (float)m_rcHot.CenterPoint().y;
}

void CLine::DrawHotShape(Graphics& graph)
{
	SolidBrush sbrush(Color::White);
	Pen penDraw(Color::Blue,3);

	PointF ptLT;
	ptLT.X = m_ptary[0].X - m_nWidth;
	ptLT.Y = m_ptary[0].Y - m_nWidth;

	Rect rect((int)ptLT.X, (int)ptLT.Y, m_nWidth * 2, m_nWidth * 2);
	graph.DrawEllipse(&penDraw, rect);
	graph.FillEllipse(&sbrush, rect);

	ptLT.X = m_ptary[1].X - m_nWidth;
	ptLT.Y = m_ptary[1].Y - m_nWidth;

	rect.X = (int)ptLT.X;
	rect.Y = (int)ptLT.Y;
	rect.Width = m_nWidth * 2;
	rect.Height = m_nWidth * 2;

	graph.DrawEllipse(&penDraw, rect);
	graph.FillEllipse(&sbrush, rect);
}

ModifyType CLine::AroundHotPts(CPoint point)
{
	if(!m_bFinish)
	{
		return MT_NONE;
	}

	PointF pt((float)point.x,(float)point.y);

	if(CalcGapOfpoints(pt,m_ptary[0]) <= m_nWidth)
	{
		return MT_TC;
	}
	else if(CalcGapOfpoints(pt,m_ptary[1]) <= m_nWidth)
	{
		return MT_TC;
	}
	else if(m_Region.IsVisible(pt))
	{
		return MT_MOVE;
	}
	else
	{
		return MT_NONE;
	}
}

BOOL CLine::IsPtInModifyZone(CPoint point)
{
	BOOL bret = TRUE;
	PointF pt((float)point.x,(float)point.y);

	if(CalcGapOfpoints(pt,m_ptary[0]) <= m_nWidth)
	{
	}
	else if(CalcGapOfpoints(pt,m_ptary[1]) <= m_nWidth)
	{
	}
	else if(m_Region.IsVisible(pt))
	{
	}
	else
	{
		bret = FALSE;
	}

	return bret;
}
