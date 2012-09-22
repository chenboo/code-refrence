#include "stdafx.h"
#include "GraphLib.h"

CErro::CErro(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_nMinCount = 12;
	m_ptary.SetSize(14,1);
	m_nDashStyle = dashstyle;
	InitGraphData();
}

CErro::CErro()
{
}

CErro::CErro(const CErro& gph)
{
	CopyTheBaseDataMember(&gph);
}

CErro & CErro::operator = (const CErro& gph)
{
	CopyTheBaseDataMember(&gph);

	return *this;
}

CErro::~CErro()
{
	m_ptarytemp.RemoveAll();
}

void CErro::InitGraphData()
{
	m_ptary[0].X = 4.0f; 
	m_ptary[0].Y = 0.0f;
	m_ptary[1].X = 35.0f;
	m_ptary[1].Y = 31.0f;
	m_ptary[2].X = 66.0f;
	m_ptary[2].Y = 0.0f;
	m_ptary[3].X = 70.0f;
	m_ptary[3].Y = 4.0f;
	m_ptary[4].X = 39.0f;
	m_ptary[4].Y = 35.0f;
	m_ptary[5].X = 70.0f;
	m_ptary[5].Y = 66.0f;
	
	m_ptary[6].X = 66.0f; 
	m_ptary[6].Y = 70.0f;
	m_ptary[7].X = 35.0f;
	m_ptary[7].Y = 39.0f;
	m_ptary[8].X = 4.0f;
	m_ptary[8].Y = 70.0f;
	m_ptary[9].X = 0.0f;
	m_ptary[9].Y = 66.0f;
	m_ptary[10].X = 31.0f;
	m_ptary[10].Y = 35.0f;
	m_ptary[11].X = 0.0f;
	m_ptary[11].Y = 4.0f;
}

void CErro::Draw(CDC* pDC,CRect& rcInvalid)
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
		graph.DrawPolygon(&penDraw,pPt,12);
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

void CErro::FinishGraph()
{
	m_ptarytemp.RemoveAll();
	m_ptary.RemoveAt(12,2);
	if(12 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void CErro::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

	int nWidth = m_nWidth;

	Pen penDraw(Color(255, 0, 255,  0), (float)nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);
	
	GraphicsPath path;
	path.AddPolygon(pPt,m_nPtCount);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CErro::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN == nMouseState)
	{
		m_ptary[12] = pt;
		m_nPtCount = 1;
	}
	else if(m_nPtCount >= 1)
	{
		m_ptary[13] = pt;
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

void CErro::NormalizeGraphData()
{	
	float fleft = min(m_ptary[12].X, m_ptary[13].X);
	float ftop = min(m_ptary[12].Y, m_ptary[13].Y);

	if(m_ptary[12].X == m_ptary[13].X
		&& m_ptary[12].Y == m_ptary[13].Y)
	{
		for(int i = 0; i < 12; i++)
		{
			m_ptary[i].X += fleft;
			m_ptary[i].Y += ftop;
		}
	}
	else
	{
		float fWidthOrg = m_ptary[3].X - m_ptary[11].X;
		float fHeightOrg = m_ptary[8].Y - m_ptary[0].Y;

		float fWidth = fabs(m_ptary[12].X - m_ptary[13].X);
		float fHeight = fabs(m_ptary[12].Y - m_ptary[13].Y);

		float fscalex = fWidth / fWidthOrg;
		float fscaley = fHeight / fHeightOrg;

		for(int i = 0; i < 12; i++)
		{
			m_ptary[i].X = m_ptary[i].X * fscalex + fleft;
			m_ptary[i].Y = m_ptary[i].Y * fscaley + ftop;
		}
	}
	
	m_ptary.RemoveAt(12,2);
	m_nPtCount = 12;
}

void CErro::NormalizeTempData()
{
	m_ptarytemp.Copy(m_ptary);

	float fleft = min(m_ptarytemp[12].X, m_ptarytemp[13].X);
	float ftop = min(m_ptarytemp[12].Y, m_ptarytemp[13].Y);

	float fWidthOrg = m_ptarytemp[3].X - m_ptarytemp[11].X;
	float fHeightOrg = m_ptarytemp[8].Y - m_ptarytemp[0].Y;

	float fWidth = fabs(m_ptarytemp[12].X - m_ptarytemp[13].X);
	float fHeight = fabs(m_ptarytemp[12].Y - m_ptarytemp[13].Y);

	float fscalex = fWidth / fWidthOrg;
	float fscaley = fHeight / fHeightOrg;

	for(int i = 0; i < 12; i++)
	{
		m_ptarytemp[i].X = m_ptarytemp[i].X * fscalex + fleft;
		m_ptarytemp[i].Y = m_ptarytemp[i].Y * fscaley + ftop;
	}
}
