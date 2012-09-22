#include "stdafx.h"
#include "CTriangle.h"

CTriangle::CTriangle(int nWidth,Color crColor)
{
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(3,1);
	m_nMinCount = 3;
}

CTriangle::CTriangle()
{

}

CTriangle::CTriangle(const CTriangle& gph)
{
	CopyBase(&gph);	

	m_ptary.SetSize(3,1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
}

CTriangle::~CTriangle()
{
}

void CTriangle::Draw(CDC* pDC)
{
	if(m_nPtCount <= 0)
		return;

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetLineJoin(LineJoinRound);

	if(m_nPtCount == 2)
	{

		Pen pent(Color::Blue,2);
		pent.SetDashStyle(DashStyleDot);

		CPoint ptLT;
		CPoint ptRB;
		ptLT.x = (int)m_ptary[0].X;
		ptLT.y = (int)m_ptary[0].Y;
		ptRB.x = (int)m_ptary[1].X;
		ptRB.y = (int)m_ptary[1].Y;
		CRect rc(ptLT,ptRB);
		rc.NormalizeRect();
		graph.DrawRectangle(&pent,rc.left,rc.top,rc.Width(),rc.Height());
	}
	else if(m_nPtCount == 3)
	{
		PointF* pPt = m_ptary.GetData();
		graph.DrawPolygon(&penDraw,pPt,m_nPtCount);

		if(m_bSelected)
		{
			DrawHotShape(graph);
		}
	}
}

void CTriangle::AddPoint(UINT nMouseState,CPoint point)
{
	if(!m_bFinish)
	{
		FormGraphData(nMouseState,point);
	}
	else
	{
		DeceideModiFyType(nMouseState,point);
		ModifyGraph(nMouseState,point);
	}
}

void CTriangle::FinishGraph()
{
	m_bFinish = TRUE;
	InitHotData();
	return;
}

void CTriangle::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

	int nWidth = m_nWidth;
	if(m_nWidth < 6)
	{
		nWidth = 6;
	}

	Pen greenPen(Color(255, 0, 255,  0), (float)nWidth);
	GraphicsPath path;
	path.AddPolygon(pPt,m_nPtCount);
	path.Widen(&greenPen);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CTriangle::FormGraphData(UINT nMouseState,CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
	}
	else
	{
		m_ptary[1] = pt;
		m_nPtCount = 2;
	}
	if(MS_LBTNUP == nMouseState)
	{
		m_bFinish = TRUE;
		m_nPtCount = 3;
		CalGraphData();
		InitHotData();
	}
}

void CTriangle::CalGraphData()
{
	if(m_ptary[0].X == m_ptary[1].X
		&& m_ptary[0].Y == m_ptary[1].Y)
	{
		m_ptary[1].X = m_ptary[0].X - 20.0f;
		m_ptary[1].Y = m_ptary[0].Y + 40.0f;
		m_ptary[2].X = m_ptary[0].X + 20.0f;
		m_ptary[2].Y = m_ptary[0].Y + 40.0f;
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