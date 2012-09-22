#include "stdafx.h"
#include "GraphLib.h"

CParallelogram::CParallelogram(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(6,1);
	m_nMinCount = 4;
	m_nDashStyle = dashstyle;
}

CParallelogram::CParallelogram()
{

}

CParallelogram::CParallelogram(const CParallelogram& gph)
{
	CopyTheBaseDataMember(&gph);
}

CParallelogram & CParallelogram::operator = (const CParallelogram& gph)
{
	CopyTheBaseDataMember(&gph);
	return *this;
}

CParallelogram::~CParallelogram()
{
}

void CParallelogram::Draw(CDC* pDC,CRect& rcInvalid)
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

void CParallelogram::FinishGraph()
{
	if(4 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	m_ptary.RemoveAt(4,2);
	return;
}

void CParallelogram::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

	Pen penDraw(Color(255, 0, 255, 0));
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);
	
	GraphicsPath path;
	path.AddPolygon(pPt, m_nPtCount);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CParallelogram::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x, (float)point.y);
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
			m_ptary.RemoveAt(4, 2);
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

void CParallelogram::NormalizeGraphData()
{
	if(m_ptary[4].Equals(m_ptary[5]) && m_bFinish)
	{
		m_ptary[4].X -= 50;
		m_ptary[4].Y -= 50;
		
		m_ptary[5].X += 50;
		m_ptary[5].Y += 50;
	}
	
	float fwidth = fabs(m_ptary[4].X - m_ptary[5].X);
	float fheight = fabs(m_ptary[4].Y - m_ptary[5].Y);
	
	float fhalfwidth = fwidth / 2.0f;
	float fhalfheight = fheight / 2.0f;
	
	float fTop = min(m_ptary[4].Y, m_ptary[5].Y);
	float fBottom = max(m_ptary[4].Y, m_ptary[5].Y);
	float fLeft = min(m_ptary[4].X, m_ptary[5].X) - fhalfwidth;
	float fRight = max(m_ptary[4].X, m_ptary[5].X) + fhalfwidth;
	
	m_ptary[0] = m_ptary[4];
	m_ptary[2] = m_ptary[5];
	
	m_ptary[1].Y = m_ptary[2].Y;
	m_ptary[3].Y = m_ptary[0].Y;
	
	if(m_ptary[2].X > m_ptary[0].X)
	{
		m_ptary[1].X = fLeft - fhalfwidth;
		m_ptary[3].X = fRight + fhalfwidth;
	}
	else
	{
		m_ptary[1].X = fRight + fhalfwidth;
		m_ptary[3].X = fLeft - fhalfwidth;
	}
}
