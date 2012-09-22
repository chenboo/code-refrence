#include "stdafx.h"
#include "GraphLib.h"

CLTriangle::CLTriangle(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 3;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(3, 1);
	m_nDashStyle = dashstyle;
}

CLTriangle::CLTriangle()
{

}

CLTriangle::CLTriangle(const CLTriangle& gph)
{
	CopyTheBaseDataMember(&gph);
}

CLTriangle & CLTriangle::operator = (const CLTriangle& gph)
{
	CopyTheBaseDataMember(&gph);
	return *this;
}

CLTriangle::~CLTriangle()
{
}

void CLTriangle::Draw(CDC* pDC,CRect& rcInvalid)
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

	if(m_nPtCount == 2)
	{
		DrawPreView(graph, penDraw);
	}
	else if(m_nPtCount == 3)
	{
		if(m_bSelected || m_bEdit)
		{
			DrawHotShape(graph);
		}

		graph.ExcludeClip(&m_RgnErase);
		PointF* pPt = m_ptary.GetData();
		graph.DrawPolygon(&penDraw,pPt,m_nPtCount);
	}
}

void CLTriangle::DrawPreView(Graphics& graph, Pen& penDraw)
{
	PointF pt[3];
	pt[0] = m_ptary[0];
	pt[1] = m_ptary[1];
	pt[2].X = pt[0].X;
	pt[2].Y = pt[1].Y;

	graph.DrawPolygon(&penDraw,pt,3);
}

void CLTriangle::FinishGraph()
{
	if(3 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void CLTriangle::SetRegion()
{
	PointF* pPt = m_ptary.GetData();
	Pen penDraw(Color(255, 0, 255, 0));
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);
	
	GraphicsPath path;
	path.AddPolygon(pPt,m_nPtCount);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CLTriangle::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN == nMouseState)
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
			m_bFinish = TRUE;
			m_nPtCount = 3;
			NormalizeGraphData();
			InitHotData();
			rcUpdate = m_rcHot;
		}
	}
	else
	{
		rcUpdate.SetRect(0, 0, 1, 1);
	}
}

void CLTriangle::NormalizeGraphData()
{
	if(m_ptary[0].X == m_ptary[1].X
		&& m_ptary[0].Y == m_ptary[1].Y)
	{
		m_ptary[1].X = m_ptary[0].X + 30.0f;
		m_ptary[1].Y = m_ptary[0].Y + 60.0f;
		m_ptary[2].X = m_ptary[0].X;
		m_ptary[2].Y = m_ptary[1].Y;
	}

	m_ptary[2].X = m_ptary[0].X;
	m_ptary[2].Y = m_ptary[1].Y;
}
