#include "stdafx.h"
#include "GraphLib.h"

CDiamond::CDiamond(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 4;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(6,1);
	m_nDashStyle = dashstyle;
}

CDiamond::CDiamond()
{

}

CDiamond::CDiamond(const CDiamond& gph)
{
	CopyTheBaseDataMember(&gph);
}

CDiamond & CDiamond::operator = (const CDiamond& gph)
{
	CopyTheBaseDataMember(&gph);
	return *this;
}

CDiamond::~CDiamond()
{
}

void CDiamond::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount <= 0)
		return;

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

void CDiamond::FinishGraph()
{
	m_ptary.RemoveAt(4,2);
	if(4 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}
	return;
}

void CDiamond::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

	int nWidth = m_nWidth;

	Pen penDraw(Color(255, 0, 255,  0));
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);
	
	GraphicsPath path;
	path.AddPolygon(pPt,m_nPtCount);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CDiamond::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
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

void CDiamond::NormalizeGraphData()
{
	if(m_ptary[4].Equals(m_ptary[5]) && m_bFinish)
	{
		m_ptary[4].X -= 50;
		m_ptary[4].Y -= 50;
		
		m_ptary[5].X += 50;
		m_ptary[5].Y += 50;
	}

	PointF ptCenter((m_ptary[4].X + m_ptary[5].X) / 2, (m_ptary[4].Y + m_ptary[5].Y) / 2);
	
	float fTop = min(m_ptary[4].Y, m_ptary[5].Y);
	float fBottom = max(m_ptary[4].Y, m_ptary[5].Y);
	float fLeft = min(m_ptary[4].X, m_ptary[5].X);
	float fRight = max(m_ptary[4].X, m_ptary[5].X);
	
	m_ptary[0].X = fLeft;
	m_ptary[0].Y = ptCenter.Y;
	m_ptary[1].X = ptCenter.X;
	m_ptary[1].Y = fTop;
	m_ptary[2].X = fRight;
	m_ptary[2].Y = ptCenter.Y;
	m_ptary[3].X = ptCenter.X;
	m_ptary[3].Y = fBottom;
}
