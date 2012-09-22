#include "stdafx.h"
#include "CNormalPen.h"

CNormalPen::CNormalPen(LINETYPE LineType)
{
	m_nWidth = LineType.nWidth;
	m_crColor = LineType.crColor;
	m_nDashStyle = LineType.nDashStyle;
	m_nStartCap = LineType.nStartCap;
	m_nEndCap = LineType.nEndCap;
	m_nMinCount = 2;
	m_ptary.SetSize(0,10);
	m_noffst = 0;
}

CNormalPen::CNormalPen()
{

}

CNormalPen::CNormalPen(const CNormalPen& gph)
{
	CopyBase(&gph);

	m_ptary.RemoveAll();
	m_ptary.SetSize(0,4);
	m_ptary.Copy(gph.m_ptary);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
	
	m_nDashStyle = gph.m_nDashStyle;
	m_nStartCap = gph.m_nStartCap;
	m_nEndCap = gph.m_nEndCap;
	m_noffst = gph.m_noffst;
}


CNormalPen::~CNormalPen()
{
}

void CNormalPen::SetLineType(LINETYPE LineType)
{
	m_nWidth = LineType.nWidth;
	m_crColor = LineType.crColor;
	m_nDashStyle = LineType.nDashStyle;
	m_nStartCap = LineType.nStartCap;
	m_nEndCap = LineType.nEndCap;
}

void CNormalPen::SetDashStyle(DashStyle nDashStyle)
{
	m_nDashStyle = nDashStyle;
}

void CNormalPen::SetStartCap(LineCap nStartCap)
{
	m_nStartCap = nStartCap;
}

void CNormalPen::SetEndCap(LineCap nEndCap)
{
	m_nEndCap = nEndCap;
}

void CNormalPen::AddPoint(UINT nMouseState,CPoint point)
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

void CNormalPen::Draw(CDC* pDC)
{
	if(m_nPtCount == 0)
		return;
	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);
	
	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetStartCap(m_nStartCap);

	PointF* pPt = m_ptary.GetData();
	if(m_bFinish)
	{
		penDraw.SetEndCap(m_nEndCap);
		penDraw.SetLineJoin(LineJoinRound);
		graph.DrawCurve(&penDraw,pPt,m_nPtCount,0.8f);
	}
	else
	{
		graph.DrawCurve(&penDraw,pPt,m_nPtCount - 1,0.8f);
	}
	if(m_bSelected)
	{
		DrawHotShape(graph);
	}
}

void CNormalPen::FormGraphData(UINT nMouseState,CPoint point)
{
	PointF pt((float)point.x,(float)point.y);

	//int nCount = m_ptary.GetCount();
	//if(nCount == 0)
	//{
	//	m_ptary.Add(pt);
	//	m_nPtCount++;
	//}
	//else if(CalGapOfpoints(pt,m_ptary[nCount - 1]) > 5)
	//{
	//	m_ptary.Add(pt);
	//	m_nPtCount++;
	//}

	if(m_noffst % 3 == 0)
	{
		m_ptary.Add(pt);
		m_nPtCount++;
	}
	m_noffst++;

	if(MS_LBTNUP == nMouseState)
	{
		m_bFinish = TRUE;
		InitHotData();
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
	PointF* pPt = m_ptary.GetData();

	int nWidth = m_nWidth;
	if(m_nWidth < 3)
	{
		nWidth = 3;
	}

	Pen penDraw(m_crColor,(float)nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetStartCap(m_nStartCap);
	penDraw.SetEndCap(m_nEndCap);
	penDraw.SetLineJoin(LineJoinRound);

	GraphicsPath path;
	path.AddCurve(pPt,m_nPtCount);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}
	
