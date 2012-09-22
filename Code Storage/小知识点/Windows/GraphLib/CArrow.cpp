#include "stdafx.h"
#include "GraphLib.h"
#include "DrawLib.h"

CArrow::CArrow(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 7;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(9, 1);
	m_nDashStyle = dashstyle;
	InitGraphData();
}

CArrow::CArrow()
{

}

CArrow::CArrow(const CArrow& gph)
{
	CopyTheBaseDataMember(&gph);

	m_ptary.SetSize(7, 1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);
	
	m_ptarytemp.RemoveAll();
	m_ptarytemp.Copy(gph.m_ptarytemp);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
}

CArrow & CArrow::operator = (const CArrow& gph)
{
	CopyTheBaseDataMember(&gph);

	m_ptary.SetSize(7, 1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);
	
	m_ptarytemp.RemoveAll();
	m_ptarytemp.Copy(gph.m_ptarytemp);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);

	return *this;
}

CArrow::~CArrow()
{
	m_ptarytemp.RemoveAll();
}

void CArrow::InitGraphData()
{
	m_ptary[0].X = 40.0f;
	m_ptary[0].Y = 5.0f;

	m_ptary[1].X = 0.0f;
	m_ptary[1].Y = 15.0f;

	m_ptary[2].X = 25.0f;
	m_ptary[2].Y = 15.0f;

	m_ptary[3].X = 25.0f;
	m_ptary[3].Y = 35.0f;

	m_ptary[4].X = 55.0f;
	m_ptary[4].Y = 35.0f;

	m_ptary[5].X = 55.0f;
	m_ptary[5].Y = 15.0f;

	m_ptary[6].X = 80.0f;
	m_ptary[6].Y = 15.0f;
}

void CArrow::Draw(CDC* pDC,CRect& rcInvalid)
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
	
	if(!m_bFinish)
	{
		if(m_nPtCount <= 1)
		{
			return;
		}

		PointF* pPt = m_ptarytemp.GetData();
		graph.DrawPolygon(&penDraw, pPt, 7);
	}
	else
	{
		if(m_bSelected || m_bEdit)
		{
			DrawHotShape(graph);
		}

		graph.ExcludeClip(&m_RgnErase);
		PointF* pPt = m_ptary.GetData();
		
		graph.DrawPolygon(&penDraw, pPt, m_nPtCount);
	}
}

void CArrow::FinishGraph()
{
	m_ptarytemp.RemoveAll();
	m_ptary.RemoveAt(7, 2);
	if(m_nPtCount == 7)
	{
		m_bFinish = TRUE;
		InitHotData();
	}
	return;
}

void CArrow::SetRegion()
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

void CArrow::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[7] = pt;
		m_nPtCount = 1;
	}
	else if(m_nPtCount >= 1)
	{
		m_ptary[8] = pt;
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

void CArrow::NormalizeGraphData()
{	
	float fleft = min(m_ptary[7].X, m_ptary[8].X);
	float ftop = min(m_ptary[7].Y, m_ptary[8].Y);

	if(m_ptary[7].X == m_ptary[8].X
		&& m_ptary[7].Y == m_ptary[8].Y)
	{
		for(int i = 0; i < 7; i++)
		{
			m_ptary[i].X += fleft;
			m_ptary[i].Y += ftop;
		}
	}
	else
	{
		float fWidthOrg = m_ptary[6].X - m_ptary[1].X;
		float fHeightOrg = m_ptary[3].Y - m_ptary[0].Y;

		float fWidth = fabs(m_ptary[7].X - m_ptary[8].X);
		float fHeight = fabs(m_ptary[7].Y - m_ptary[8].Y);

		float fscalex = fWidth / fWidthOrg;
		float fscaley = fHeight / fHeightOrg;

		for(int i = 0; i < 7; i++)
		{
			m_ptary[i].X = m_ptary[i].X * fscalex + fleft;
			m_ptary[i].Y = m_ptary[i].Y * fscaley + ftop;
		}
	}
	
	m_ptary.RemoveAt(7, 2);
	m_nPtCount = 7;
}

void CArrow::NormalizeTempData()
{
	m_ptarytemp.Copy(m_ptary);

	float fleft = min(m_ptarytemp[7].X, m_ptarytemp[8].X);
	float ftop = min(m_ptarytemp[7].Y, m_ptarytemp[8].Y);

	float fWidthOrg = m_ptarytemp[6].X - m_ptarytemp[1].X;
	float fHeightOrg = m_ptarytemp[3].Y - m_ptarytemp[0].Y;

	float fWidth = fabs(m_ptarytemp[7].X - m_ptarytemp[8].X);
	float fHeight = fabs(m_ptarytemp[7].Y - m_ptarytemp[8].Y);

	float fscalex = fWidth / fWidthOrg;
	float fscaley = fHeight / fHeightOrg;

	for(int i = 0; i < 7; i++)
	{
		m_ptarytemp[i].X = m_ptarytemp[i].X * fscalex + fleft;
		m_ptarytemp[i].Y = m_ptarytemp[i].Y * fscaley + ftop;
	}
}
