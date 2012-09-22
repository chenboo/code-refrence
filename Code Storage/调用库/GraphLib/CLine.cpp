#include "stdafx.h"
#include "CLine.h"

CLine::CLine(LINETYPE LineType)
{
	m_nWidth = LineType.nWidth;
	m_crColor = LineType.crColor;
	m_nDashStyle = LineType.nDashStyle;
	m_nStartCap = LineType.nStartCap;
	m_nEndCap = LineType.nEndCap;
	m_ptary.SetSize(2,2);
	m_nPtCount = 0;
	m_nMinCount = 2;
}

CLine::CLine()
{

} 

CLine::CLine(const CLine& gph)
{
	CopyBase(&gph);

	m_ptary.RemoveAll();
	m_ptary.SetSize(2,2);
	m_ptary.Copy(gph.m_ptary);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
	
	m_nDashStyle = gph.m_nDashStyle;
	m_nStartCap = gph.m_nStartCap;
	m_nEndCap = gph.m_nEndCap;
}


CLine::~CLine()
{
}

void CLine::SetLineType(LINETYPE LineType)
{
	m_nWidth = LineType.nWidth;
	m_crColor = LineType.crColor;
	m_nDashStyle = LineType.nDashStyle;
	m_nStartCap = LineType.nStartCap;
	m_nEndCap = LineType.nEndCap;
}

void CLine::SetDashStyle(DashStyle nDashStyle)
{
	m_nDashStyle = nDashStyle;
}

void CLine::SetStartCap(LineCap nStartCap)
{
	m_nStartCap = nStartCap;
}

void CLine::SetEndCap(LineCap nEndCap)
{
	m_nEndCap = nEndCap;
}

void CLine::Draw(CDC* pDC)
{
	if(m_nPtCount <= 0)
		return;
		
	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetStartCap(m_nStartCap);
	penDraw.SetEndCap(m_nEndCap);
	penDraw.SetLineJoin(LineJoinRound);

	if(m_nPtCount == 1)
	{
		graph.DrawLine(&penDraw,m_ptary[0],m_ptary[0]);
	}
	else
	{
		graph.DrawLine(&penDraw,m_ptary[0],m_ptary[1]);

		if(m_bSelected)
		{
			DrawHotShape(graph);
		}
	}
}

void CLine::AddPoint(UINT nMouseState,CPoint point)
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

void CLine::FormGraphData(UINT nMouseState,CPoint point)
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
		InitHotData();
	}
}

void CLine::FinishGraph()
{
	m_bFinish = TRUE;
	InitHotData();
	return;
}

void CLine::SetRegion()
{
	int nWidth = m_nWidth;
	if(m_nWidth < 6)
	{
		nWidth = 6;
	}

	Pen penDraw(m_crColor,(float)nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetStartCap(m_nStartCap);
	penDraw.SetEndCap(m_nEndCap);
	penDraw.SetLineJoin(LineJoinRound);

	GraphicsPath path;
	path.AddLine(m_ptary[0],m_ptary[1]);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}