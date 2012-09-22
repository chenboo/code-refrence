#include "stdafx.h"
#include "GraphLib.h"
#include "DrawLib.h"

C5Stars::C5Stars(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 10;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(12,1);
	m_nDashStyle = dashstyle;
	InitGraphData();
}

C5Stars::C5Stars()
{

}

C5Stars::C5Stars(const C5Stars& gph)
{
	CopyTheBaseDataMember(&gph);

	m_ptary.SetSize(10,1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);
	
	m_ptarytemp.RemoveAll();
	m_ptarytemp.Copy(gph.m_ptarytemp);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
}

C5Stars & C5Stars::operator = (const C5Stars& gph)
{
	CopyTheBaseDataMember(&gph);

	m_ptary.SetSize(10,1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);
	
	m_ptarytemp.RemoveAll();
	m_ptarytemp.Copy(gph.m_ptarytemp);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);

	return *this;
}

C5Stars::~C5Stars()
{
	m_ptarytemp.RemoveAll();
}

void C5Stars::InitGraphData()
{
	m_ptary[0].X = 35.0f; 
	m_ptary[0].Y = 0.0f;
	m_ptary[1].X = 27.142f;
	m_ptary[1].Y = 24.184f;
	m_ptary[2].X = 1.713f;
	m_ptary[2].Y = 24.184f;
	m_ptary[3].X = 22.286f;
	m_ptary[3].Y = 39.131f;
	m_ptary[4].X = 14.428f;
	m_ptary[4].Y = 63.316f;
	m_ptary[5].X = 35.0f;
	m_ptary[5].Y = 48.369f;
	m_ptary[6].X = 55.572f;
	m_ptary[6].Y = 63.316f;
	m_ptary[7].X = 47.714f;
	m_ptary[7].Y = 39.131f;
	m_ptary[8].X = 68.287f;
	m_ptary[8].Y = 24.184f;
	m_ptary[9].X = 42.858f;
	m_ptary[9].Y = 24.184f;
}

void C5Stars::Draw(CDC* pDC,CRect& rcInvalid)
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
		graph.DrawPolygon(&penDraw, pPt, 10);
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

void C5Stars::FinishGraph()
{
	m_ptarytemp.RemoveAll();
	m_ptary.RemoveAt(10,2);
	if(m_nPtCount == 10)
	{
		m_bFinish = TRUE;
		InitHotData();
	}
	return;
}

void C5Stars::SetRegion()
{
	PointF* pPt = m_ptary.GetData();
	int nWidth = m_nWidth;

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

void C5Stars::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[10] = pt;
		m_nPtCount = 1;
	}
	else if(m_nPtCount >= 1)
	{
		m_ptary[11] = pt;
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

void C5Stars::NormalizeGraphData()
{	
	float fleft = min(m_ptary[10].X, m_ptary[11].X);
	float ftop = min(m_ptary[10].Y, m_ptary[11].Y);

	if(m_ptary[10].X == m_ptary[11].X
		&& m_ptary[10].Y == m_ptary[11].Y)
	{
		for(int i = 0; i < 10; i++)
		{
			m_ptary[i].X += fleft;
			m_ptary[i].Y += ftop;
		}
	}
	else
	{
		float fWidthOrg = m_ptary[8].X - m_ptary[2].X;
		float fHeightOrg = m_ptary[6].Y - m_ptary[0].Y;

		float fWidth = fabs(m_ptary[10].X - m_ptary[11].X);
		float fHeight = fabs(m_ptary[10].Y - m_ptary[11].Y);

		float fscalex = fWidth / fWidthOrg;
		float fscaley = fHeight / fHeightOrg;

		for(int i = 0; i < 10; i++)
		{
			m_ptary[i].X = m_ptary[i].X * fscalex + fleft;
			m_ptary[i].Y = m_ptary[i].Y * fscaley + ftop;
		}
	}
	
	m_ptary.RemoveAt(10,2);
	m_nPtCount = 10;
}

void C5Stars::NormalizeTempData()
{
	m_ptarytemp.Copy(m_ptary);

	float fleft = min(m_ptarytemp[10].X, m_ptarytemp[11].X);
	float ftop = min(m_ptarytemp[10].Y, m_ptarytemp[11].Y);

	float fWidthOrg = m_ptarytemp[8].X - m_ptarytemp[2].X;
	float fHeightOrg = m_ptarytemp[6].Y - m_ptarytemp[0].Y;

	float fWidth = fabs(m_ptarytemp[10].X - m_ptarytemp[11].X);
	float fHeight = fabs(m_ptarytemp[10].Y - m_ptarytemp[11].Y);

	float fscalex = fWidth / fWidthOrg;
	float fscaley = fHeight / fHeightOrg;

	for(int i = 0; i < 10; i++)
	{
		m_ptarytemp[i].X = m_ptarytemp[i].X * fscalex + fleft;
		m_ptarytemp[i].Y = m_ptarytemp[i].Y * fscaley + ftop;
	}
}
