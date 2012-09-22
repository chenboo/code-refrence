#include "stdafx.h"
#include "GraphLib.h"

C6Sides::C6Sides(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 6;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(8,1);
	m_nDashStyle = dashstyle;
	InitGraphData();
}

C6Sides::C6Sides()
{

}

C6Sides::C6Sides(const C6Sides& gph)
{
	CopyTheBaseDataMember(&gph);

	m_ptary.SetSize(6,1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);
	
	m_ptarytemp.RemoveAll();
	m_ptarytemp.Copy(gph.m_ptarytemp);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
}

C6Sides & C6Sides::operator = (const C6Sides& gph)
{
	CopyTheBaseDataMember(&gph);

	m_ptary.SetSize(6,1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);
	
	m_ptarytemp.RemoveAll();
	m_ptarytemp.Copy(gph.m_ptarytemp);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);

	return *this;
}

C6Sides::~C6Sides()
{
	m_ptarytemp.RemoveAll();
}

void C6Sides::InitGraphData()
{
	m_ptary[0].X = 17.500006f;
	m_ptary[0].Y = 4.689107f;
	m_ptary[1].X = 0.000000f;
	m_ptary[1].Y = 34.999996f;
	m_ptary[2].X = 17.499990f;
	m_ptary[2].Y = 65.310883f;
	m_ptary[3].X = 52.499996f;
	m_ptary[3].Y = 65.310890f;
	m_ptary[4].X = 70.000000f;
	m_ptary[4].Y = 35.000011f;
	m_ptary[5].X = 52.500015f;
	m_ptary[5].Y = 4.689120f;
}

void C6Sides::Draw(CDC* pDC,CRect& rcInvalid)
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
		graph.DrawPolygon(&penDraw,pPt,6);
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

void C6Sides::FinishGraph()
{
	m_ptarytemp.RemoveAll();
	m_ptary.RemoveAt(6,2);

	if(m_nPtCount == 6)
	{
		m_bFinish = TRUE;
		InitHotData();
	}
	
	return;
}

void C6Sides::SetRegion()
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

void C6Sides::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[6] = pt;
		m_nPtCount = 1;
	}
	else if(m_nPtCount >= 1)
	{
		m_ptary[7] = pt;
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

void C6Sides::NormalizeGraphData()
{	
	float fleft = min(m_ptary[6].X, m_ptary[7].X);
	float ftop = min(m_ptary[6].Y, m_ptary[7].Y);

	if(m_ptary[6].X == m_ptary[7].X
		&& m_ptary[6].Y == m_ptary[7].Y)
	{
		for(int i = 0; i < 6; i++)
		{
			m_ptary[i].X += fleft;
			m_ptary[i].Y += ftop;
		}
	}
	else
	{
		float fWidthOrg = m_ptary[4].X - m_ptary[1].X;
		float fHeightOrg = m_ptary[3].Y - m_ptary[5].Y;

		float fWidth = fabs(m_ptary[6].X - m_ptary[7].X);
		float fHeight = fabs(m_ptary[6].Y - m_ptary[7].Y);

		float fscalex = fWidth / fWidthOrg;
		float fscaley = fHeight / fHeightOrg;

		for(int i = 0; i < 6; i++)
		{
			m_ptary[i].X = m_ptary[i].X * fscalex + fleft;
			m_ptary[i].Y = m_ptary[i].Y * fscaley + ftop;
		}
	}
	
	m_ptary.RemoveAt(6,2);
	m_nPtCount = 6;
}

void C6Sides::NormalizeTempData()
{
	m_ptarytemp.Copy(m_ptary);

	float fleft = min(m_ptarytemp[6].X, m_ptarytemp[7].X);
	float ftop = min(m_ptarytemp[6].Y, m_ptarytemp[7].Y);

	float fWidthOrg = m_ptarytemp[4].X - m_ptarytemp[1].X;
	float fHeightOrg = m_ptarytemp[3].Y - m_ptarytemp[5].Y;

	float fWidth = fabs(m_ptarytemp[6].X - m_ptarytemp[7].X);
	float fHeight = fabs(m_ptarytemp[6].Y - m_ptarytemp[7].Y);

	float fscalex = fWidth / fWidthOrg;
	float fscaley = fHeight / fHeightOrg;

	for(int i = 0; i < 6; i++)
	{
		m_ptarytemp[i].X = m_ptarytemp[i].X * fscalex + fleft;
		m_ptarytemp[i].Y = m_ptarytemp[i].Y * fscaley + ftop;
	}
}
