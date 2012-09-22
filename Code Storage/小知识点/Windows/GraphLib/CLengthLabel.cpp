#include "stdafx.h"
#include "GraphLib.h"

CLengthLabel::CLengthLabel(int nWidth, Color cr, DashStyle dashstyle)
{
	m_nPtCount = 0;
	m_nMinCount = 2;
	m_crColor =  cr;
	m_nWidth = nWidth;
	m_ptary.SetSize(2,2);
	m_nDashStyle = dashstyle;
}

CLengthLabel::CLengthLabel()
{
	m_nWidth = 6;
	m_crColor = Color::Black;
	m_ptary.SetSize(2,2);
	m_nPtCount = 0;
	m_nMinCount = 2;
}

CLengthLabel::CLengthLabel(const CLengthLabel& gph)
{
	CopyTheBaseDataMember(&gph);
	m_strLength = gph.m_strLength;
}

CLengthLabel & CLengthLabel::operator = (const CLengthLabel& gph)
{
	CopyTheBaseDataMember(&gph);
	m_strLength = gph.m_strLength;
	return *this;
}

CLengthLabel::~CLengthLabel()
{
}

void CLengthLabel::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount <= 0)
	{
		return;
	}
		
	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetDashStyle(m_nDashStyle);

	if(m_nPtCount == 1)
	{
		graph.DrawLine(&penDraw,m_ptary[0],m_ptary[0]);
	}
	else
	{
		graph.ExcludeClip(&m_RgnErase);
		graph.DrawLine(&penDraw,m_ptary[0],m_ptary[1]);

		int nLength = (int)(CalcGapOfpoints(m_ptary[0], m_ptary[1]) + 0.5f);
		m_strLength.Format(_T("%d"),nLength);

		Font myFont(_T("Arial"), 12);
		SolidBrush brush(Color::Red);
		StringFormat format;
		format.SetAlignment(StringAlignmentFar);

		PointF ptDraw((m_ptary[0].X + m_ptary[1].X) / 2, (m_ptary[0].Y + m_ptary[1].Y) / 2);
		graph.SetTextRenderingHint(TextRenderingHintAntiAlias);
		graph.DrawString(m_strLength, m_strLength.GetLength(), &myFont, ptDraw, &format, &brush);

		if(m_bSelected || m_bEdit)
		{
			DrawHotShape(graph);
		}
	}
}

void CLengthLabel::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
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

void CLengthLabel::FinishGraph()
{
	if(2 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void CLengthLabel::SetRegion()
{
	Pen penDraw(m_crColor);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);

	GraphicsPath path;
	path.AddLine(m_ptary[0],m_ptary[1]);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CLengthLabel::DecideModiFyType(UINT nMouseState,CPoint point)
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

void CLengthLabel::ModifyGraph(UINT nMouseState,CPoint point)
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

void CLengthLabel::StretchGraph(CPoint point)
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

void CLengthLabel::DrawHotShape(Graphics& graph)
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

ModifyType CLengthLabel::AroundHotPts(CPoint point)
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

BOOL CLengthLabel::IsPtInModifyZone(CPoint point)
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

void CLengthLabel::InitHotData()
{
	if(m_nPtCount <= 0 || !m_bFinish)
	{
		return;
	}

	int nleft = (int)min(m_ptary[0].X, m_ptary[1].X);
	int ntop = (int)min(m_ptary[0].Y, m_ptary[1].Y);
	int nright = (int)max(m_ptary[0].X, m_ptary[1].X);
	int nbottom = (int)max(m_ptary[0].Y, m_ptary[1].Y);
	m_rcHot.SetRect(nleft, ntop, nright, nbottom);

	if(m_rcHot.Width() <= 100)
	{
		m_rcHot.InflateRect(5 * HOTINTERVAL, HOTINTERVAL);
	}
	else if(m_rcHot.Height() <= 100)
	{
		m_rcHot.InflateRect(HOTINTERVAL, 5 * HOTINTERVAL);
	}
	else
	{
		m_rcHot.InflateRect(2 * HOTINTERVAL, 2 * HOTINTERVAL);
	}

	m_ptCenter.X = (float)m_rcHot.CenterPoint().x;
	m_ptCenter.Y = (float)m_rcHot.CenterPoint().y;
	SetRegion();
}