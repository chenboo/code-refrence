#include "stdafx.h"
#include "GraphLib.h"

C5Sides::C5Sides(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 5;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(7,1);
	m_nDashStyle = dashstyle;
	InitGraphData();
}

C5Sides::C5Sides()
{
}

C5Sides::C5Sides(const C5Sides& gph)
{
	CopyTheBaseDataMember(&gph);

	m_ptary.SetSize(5,1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);
	
	m_ptarytemp.RemoveAll();
	m_ptarytemp.Copy(gph.m_ptarytemp);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
}

C5Sides & C5Sides::operator = (const C5Sides& gph)
{
	CopyTheBaseDataMember(&gph);

	m_ptary.SetSize(5,1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);
	
	m_ptarytemp.RemoveAll();
	m_ptarytemp.Copy(gph.m_ptarytemp);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);

	return *this;
}

C5Sides::~C5Sides()
{
	m_ptarytemp.RemoveAll();
}

void C5Sides::InitGraphData()
{
	m_ptary[0].X = 35.000004f;
	m_ptary[0].Y = 0.000000f;
	m_ptary[1].X = 1.713023f;
	m_ptary[1].Y = 24.184404f;
	m_ptary[2].X = 14.427510f;
	m_ptary[2].Y = 63.315590f;
	m_ptary[3].X = 55.572475f;
	m_ptary[3].Y = 63.315598f;
	m_ptary[4].X = 68.286980f;
	m_ptary[4].Y = 24.184412f;
}

void C5Sides::Draw(CDC* pDC,CRect& rcInvalid)
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

	if(!m_bFinish)
	{
		if(m_nPtCount <= 1)
		{
			return;
		}

		PointF* pPt = m_ptarytemp.GetData();
		graph.DrawPolygon(&penDraw,pPt,5);
	}
	else
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

void C5Sides::FinishGraph()
{
	m_ptarytemp.RemoveAll();
	m_ptary.RemoveAt(5,2);

	if(m_nPtCount == 5)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void C5Sides::SetRegion()
{
	PointF* pPt = m_ptary.GetData();
	int nWidth = m_nWidth;

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

void C5Sides::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[5] = pt;
		m_nPtCount = 1;
	}
	else if(m_nPtCount >= 1)
	{
		m_ptary[6] = pt;
		m_nPtCount = 2;
		NormalizeTempData();

		if(MS_LBTNUP == nMouseState)
		{
			m_bFinish = TRUE;
			m_ptarytemp.RemoveAll();
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

void C5Sides::NormalizeGraphData()
{	
	float fleft = min(m_ptary[5].X, m_ptary[6].X);
	float ftop = min(m_ptary[5].Y, m_ptary[6].Y);

	if(m_ptary[5].X == m_ptary[6].X
		&& m_ptary[5].Y == m_ptary[6].Y)
	{
		for(int i = 0; i < 5; i++)
		{
			m_ptary[i].X += fleft;
			m_ptary[i].Y += ftop;
		}
	}
	else
	{
		float fWidthOrg = m_ptary[4].X - m_ptary[1].X;
		float fHeightOrg = m_ptary[3].Y - m_ptary[0].Y;

		float fWidth = fabs(m_ptary[5].X - m_ptary[6].X);
		float fHeight = fabs(m_ptary[5].Y - m_ptary[6].Y);

		float fscalex = fWidth / fWidthOrg;
		float fscaley = fHeight / fHeightOrg;

		for(int i = 0; i < 5; i++)
		{
			m_ptary[i].X = m_ptary[i].X * fscalex + fleft;
			m_ptary[i].Y = m_ptary[i].Y * fscaley + ftop;
		}
	}
	
	m_ptary.RemoveAt(5,2);
	m_nPtCount = 5;
}

void C5Sides::NormalizeTempData()
{
	m_ptarytemp.Copy(m_ptary);

	float fleft = min(m_ptarytemp[5].X, m_ptarytemp[6].X);
	float ftop = min(m_ptarytemp[5].Y, m_ptarytemp[6].Y);

	float fWidthOrg = m_ptarytemp[4].X - m_ptarytemp[1].X;
	float fHeightOrg = m_ptarytemp[3].Y - m_ptarytemp[0].Y;

	float fWidth = fabs(m_ptarytemp[5].X - m_ptarytemp[6].X);
	float fHeight = fabs(m_ptarytemp[5].Y - m_ptarytemp[6].Y);

	float fscalex = fWidth / fWidthOrg;
	float fscaley = fHeight / fHeightOrg;

	for(int i = 0; i < 5; i++)
	{
		m_ptarytemp[i].X = m_ptarytemp[i].X * fscalex + fleft;
		m_ptarytemp[i].Y = m_ptarytemp[i].Y * fscaley + ftop;
	}
}
