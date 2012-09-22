#include "stdafx.h"
#include "GraphLib.h"

CTrapezoid::CTrapezoid(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 4;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(6,1);
	m_nDashStyle = dashstyle;
}

CTrapezoid::CTrapezoid()
{
}

CTrapezoid::CTrapezoid(const CTrapezoid& gph)
{
	CopyTheBaseDataMember(&gph);
}

CTrapezoid & CTrapezoid::operator = (const CTrapezoid& gph)
{
	CopyTheBaseDataMember(&gph);
	return *this;
}

CTrapezoid::~CTrapezoid()
{
}

void CTrapezoid::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetLineJoin(LineJoinRound);
	penDraw.SetDashStyle(m_nDashStyle);

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	if(m_nPtCount <= 1)
	{
		return;
	}
	else 
	{
		if(m_bFinish && (m_bSelected || m_bEdit))
		{
			DrawHotShape(graph);
		}

		graph.ExcludeClip(&m_RgnErase);
		PointF* pPt = m_ptary.GetData();
		graph.DrawPolygon(&penDraw,pPt,4);
	}
}

void CTrapezoid::FinishGraph()
{
	if(4 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	m_ptary.RemoveAt(4,2);
	return;
}

void CTrapezoid::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

	Pen penDraw(Color(255, 0, 255,  0), (float)m_nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);
	
	GraphicsPath path;
	path.AddPolygon(pPt,m_nPtCount);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CTrapezoid::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[4] = pt;
		m_nPtCount = 1;
	}
	else if(m_nPtCount >= 1)
	{
		m_ptary[5] = pt;
		m_nPtCount = 2;
		NormalizeGraphData();

		if(MS_LBTNUP == nMouseState)
		{
			m_bFinish = TRUE;
			NormalizeGraphData();
			m_ptary.RemoveAt(4,2);
			m_nPtCount = 4;
			InitHotData();
			rcUpdate = m_rcHot;
		}
	}
	else
	{
		rcUpdate.SetRect(0, 0, 1, 1);
	}
}

void CTrapezoid::NormalizeGraphData()
{
	if(m_ptary[4].Equals(m_ptary[5]) && m_bFinish)
	{
		m_ptary[4].X -= 50;
		m_ptary[4].Y -= 50;
		
		m_ptary[5].X += 50;
		m_ptary[5].Y += 50;
	}
	
	float fTop = min(m_ptary[4].Y, m_ptary[5].Y);
	float fBottom = max(m_ptary[4].Y, m_ptary[5].Y);
	float fLeft = min(m_ptary[4].X, m_ptary[5].X);
	float fRight = max(m_ptary[4].X, m_ptary[5].X);
	
	float fwidth = fabs(m_ptary[4].X - m_ptary[5].X);
	float fheight = fabs(m_ptary[4].Y - m_ptary[5].Y);
	
	float f1third = fLeft + fwidth / 3.0f;
	float f2third = fRight - fwidth / 3.0f; 
	
	m_ptary[0].X = fLeft;
	m_ptary[0].Y = fBottom;
	m_ptary[1].X = f1third;
	m_ptary[1].Y = fTop;
	m_ptary[2].X = f2third;
	m_ptary[2].Y = fTop;
	m_ptary[3].X = fRight;
	m_ptary[3].Y = fBottom;
}
