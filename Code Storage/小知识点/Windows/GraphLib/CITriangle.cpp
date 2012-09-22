#include "stdafx.h"
#include "GraphLib.h"

CITriangle::CITriangle(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 3;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(3, 1);
	m_nDashStyle = dashstyle;
}

CITriangle::CITriangle()
{

}

CITriangle::CITriangle(const CITriangle& gph)
{
	CopyTheBaseDataMember(&gph);
}

CITriangle & CITriangle::operator = (const CITriangle& gph)
{
	CopyTheBaseDataMember(&gph);
	return *this;
}

CITriangle::~CITriangle()
{
}

void CITriangle::Draw(CDC* pDC,CRect& rcInvalid)
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

void CITriangle::DrawPreView(Graphics& graph, Pen& penDraw)
{
	CPoint ptLT;
	CPoint ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;
	CRect rect(ptLT,ptRB);
	rect.NormalizeRect();

	PointF pt[3];
	pt[0].X = (float)rect.CenterPoint().x;
	pt[0].Y = (float)rect.top;
	pt[1].X = (float)rect.left;
	pt[1].Y = (float)rect.bottom;
	pt[2].X = (float)rect.right;
	pt[2].Y = (float)rect.bottom;

	graph.DrawPolygon(&penDraw,pt,3);
}

void CITriangle::FinishGraph()
{
	if( 3 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void CITriangle::SetRegion()
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

void CITriangle::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
	}

	if(m_nPtCount <= 0)
	{
		rcUpdate.SetRect(0, 0, 1, 1);
		return;
	}

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

void CITriangle::NormalizeGraphData()
{
	if(m_ptary[0].X == m_ptary[1].X
		&& m_ptary[0].Y == m_ptary[1].Y)
	{
		m_ptary[1].X = m_ptary[0].X - 30.0f;
		m_ptary[1].Y = m_ptary[0].Y + 60.0f;
		m_ptary[2].X = m_ptary[0].X + 30.0f;
		m_ptary[2].Y = m_ptary[0].Y + 60.0f;
	}
	else
	{
		float top = min(m_ptary[0].Y,m_ptary[1].Y);
		float left = min(m_ptary[0].X,m_ptary[1].X);
		float right = max(m_ptary[0].X,m_ptary[1].X);
		float bottom = max(m_ptary[0].Y,m_ptary[1].Y);

		m_ptary[0].X = (left + right) / 2;
		m_ptary[0].Y = top;
		m_ptary[1].X = left;
		m_ptary[1].Y = bottom;
		m_ptary[2].X = right;
		m_ptary[2].Y = bottom;
	}
}
