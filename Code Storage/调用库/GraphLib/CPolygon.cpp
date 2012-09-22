#include "stdafx.h"
#include "CPolygon.h"

CPolygon::CPolygon(int nWidth,Color crColor)
{
	m_bLbtnAdd = FALSE;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptNxt.X = 0;
	m_ptNxt.Y = 0;
	m_nMinCount = 3;
	m_ptary.SetSize(0,10);
	//m_ptary.SetSize(6,2);
}

CPolygon::CPolygon()
{

}

CPolygon::CPolygon(const CPolygon& gph)
{
	CopyBase(&gph);
	
	m_ptary.RemoveAll();
	m_ptary.SetSize(0,10);
	m_ptary.Copy(gph.m_ptary);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
	
	m_bLbtnAdd = gph.m_bLbtnAdd;
	m_ptNxt = gph.m_ptNxt;
}

CPolygon::~CPolygon()
{
	
}

void CPolygon::AddPoint(UINT nMouseState,CPoint point)
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

void CPolygon::FormGraphData(UINT nMouseState,CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN == nMouseState && !m_bLbtnAdd)
	{
		m_ptary.Add(pt);
		m_nPtCount++;
		m_bLbtnAdd = TRUE;
		m_ptNxt = pt;
	}
	else if(MS_LBTNMOUSE == nMouseState)
	{
		m_ptNxt = pt;
	}
	else if(MS_LBTNUP == nMouseState && m_bLbtnAdd)
	{
		if(CalGapOfpoints(pt,m_ptary[0]) <= MININTERVAL)
		{
			pt = m_ptary[0];
			m_ptary.Add(pt);
			m_nPtCount++;
			m_bFinish = TRUE;
			InitHotData();
		}
		else
		{
			m_ptary.Add(pt);
			m_nPtCount++;
		}
	}
	else if(MS_LDCLK == nMouseState)
	{
		m_bFinish = TRUE;
		InitHotData();
	}
}

void CPolygon::Draw(CDC* pDC)
{
	if(m_nPtCount == 0)
		return;
	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetLineJoin(LineJoinRound);

	PointF* pPt = m_ptary.GetData();

	if(m_bFinish)
	{
		graph.DrawPolygon(&penDraw,pPt,m_nPtCount);
	}
	else
	{
		graph.DrawLines(&penDraw,pPt,m_nPtCount);
		graph.DrawLine(&penDraw,m_ptary[m_nPtCount - 1],m_ptNxt);
	}

	if(m_bSelected)
	{
		DrawHotShape(graph);
	}
}

void CPolygon::FinishGraph()
{
	m_bFinish = TRUE;
	InitHotData();
	return;
}

void CPolygon::SetRegion()
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