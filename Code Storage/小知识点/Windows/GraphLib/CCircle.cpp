#include "stdafx.h"
#include "GraphLib.h"

CCircle::CCircle(int nCStyle, int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_nDashStyle = dashstyle;
	m_nCStyle = (CircleStyle)nCStyle;

	m_ptary.SetSize(2,2);
	m_nPtCount = 0;
	m_nMinCount = 2;
	m_fRadius = 0.0f;
	m_ptCenter.X = 0.0;
	m_ptCenter.Y = 0.0;
}

CCircle::CCircle()
{
	m_nCStyle = CS_CIRCLE;
	m_nWidth = 5;
	m_crColor = Color::Red;

	m_ptary.SetSize(2,2);
	m_nPtCount = 0;
	m_nMinCount = 2;
	m_fRadius = 0.0f;
	m_ptCenter.X = 0.0f;
	m_ptCenter.Y = 0.0f;
}

CCircle::CCircle(const CCircle& gph)
{
	CopyTheBaseDataMember(&gph);
	
	m_nCStyle = gph.m_nCStyle;
	m_fRadius = gph.m_fRadius;
}

CCircle& CCircle::operator = (const CCircle& gph)
{
	CopyTheBaseDataMember(&gph);
	
	m_nCStyle = gph.m_nCStyle;
	m_fRadius = gph.m_fRadius;
	
	return *this;
}


CCircle::~CCircle()
{
}

void CCircle::SetCircleStyle(int nCStyle)
{
	m_nCStyle = (CircleStyle)nCStyle;
}

void CCircle::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}

	graph.ExcludeClip(&m_RgnErase);

	if(m_nPtCount == 1)
	{
		return;
	}

	CPoint ptLT;
	CPoint ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;

	CRect rc(ptLT,ptRB);
	rc.NormalizeRect();

	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	graph.DrawEllipse(&penDraw, rc.left, rc.top, rc.Width(), rc.Height());
}

void CCircle::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);

	if(MS_LBTNDOWN	== nMouseState)
	{
		if(CS_CIRCLE == m_nCStyle)
		{
			m_ptCenter = pt;
		}
		else if(CS_ELLIPSE == m_nCStyle)
		{
			m_ptary[0] = pt;
		}
		m_nPtCount = 1;
	}
	else
	{
		if(m_nPtCount <= 0)
		{
			return;
		}
		if(CS_CIRCLE == m_nCStyle)
		{
			m_fRadius = CalcGapOfpoints(m_ptCenter,pt);
			m_ptary[0].X = m_ptCenter.X - m_fRadius;
			m_ptary[0].Y = m_ptCenter.Y - m_fRadius;
			m_ptary[1].X = m_ptCenter.X + m_fRadius;
			m_ptary[1].Y = m_ptCenter.Y + m_fRadius;
		}
		else if(CS_ELLIPSE == m_nCStyle)
		{
			m_ptary[1] = pt;
		}
		m_nPtCount = 2;

		if(MS_LBTNUP == nMouseState)
		{
			if(CS_CIRCLE == m_nCStyle)
			{
				if(m_fRadius <= 5.0f)
				{
					m_fRadius = 40.0f;
				}
				m_ptary[0].X = m_ptCenter.X - m_fRadius;
				m_ptary[0].Y = m_ptCenter.Y - m_fRadius;
				m_ptary[1].X = m_ptCenter.X + m_fRadius;
				m_ptary[1].Y = m_ptCenter.Y + m_fRadius;
			}
			else if(CS_ELLIPSE == m_nCStyle)
			{
				if(abs(m_ptary[1].X - m_ptary[0].X) < 5.0f
					|| abs(m_ptary[1].Y - m_ptary[0].Y) < 5.0f)
				{
					m_ptary[1].X = m_ptary[0].X + 40.0f;
					m_ptary[1].Y = m_ptary[0].Y + 60.0f;
				}
			}

			m_bFinish = TRUE;
			NormalizeData();
			InitHotData();
			rcUpdate = m_rcHot;
		}
	}
}

void CCircle::FinishGraph()
{
	if(2 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void CCircle::NormalizeData()
{
	CPoint ptLT;
	CPoint ptRB;
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

void CCircle::SetRegion()
{
	CRect rc((int)m_ptary[0].X,(int)m_ptary[0].Y,
		(int)m_ptary[1].X,(int)m_ptary[1].Y);
	rc.NormalizeRect();

	int nWidth = m_nWidth;

	Pen penDraw(Color(255, 0, 255, 0));
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);

	GraphicsPath path;
	path.AddEllipse(rc.left,rc.top,rc.Width(),rc.Height());
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}
