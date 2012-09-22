#include "stdafx.h"
#include "GraphLib.h"
#include "DrawLib.h"

CBalloon::CBalloon(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 8;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(10, 1);
	m_nDashStyle = dashstyle;
	InitGraphData();
}

CBalloon::CBalloon()
{

}

CBalloon::CBalloon(const CBalloon& gph)
{
	CopyTheBaseDataMember(&gph);

	m_ptary.SetSize(8, 1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);
	
	m_ptarytemp.RemoveAll();
	m_ptarytemp.Copy(gph.m_ptarytemp);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
}

CBalloon & CBalloon::operator = (const CBalloon& gph)
{
	CopyTheBaseDataMember(&gph);

	m_ptary.SetSize(8, 1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);
	
	m_ptarytemp.RemoveAll();
	m_ptarytemp.Copy(gph.m_ptarytemp);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);

	return *this;
}

CBalloon::~CBalloon()
{
	m_ptarytemp.RemoveAll();
}

void CBalloon::InitGraphData()
{
	m_ptary[0].X = 10.0f;
	m_ptary[0].Y = 10.0f;
	m_ptary[1].X = 110.0f;
	m_ptary[1].Y = 80.0f;

	m_ptary[2].X = 68.0f;
	m_ptary[2].Y = 78.0f;
	m_ptary[3].X = 100.0f;
	m_ptary[3].Y = 96.0f;

	m_ptary[4].X = 75.0f;
	m_ptary[4].Y = 96.0f;
	m_ptary[5].X = 95.0f;
	m_ptary[5].Y = 106.0f;

	m_ptary[6].X = 78.0f;
	m_ptary[6].Y = 106.0f;
	m_ptary[7].X = 93.0f;
	m_ptary[7].Y = 114.0f;
}

void CBalloon::Draw(CDC* pDC,CRect& rcInvalid)
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

		graph.DrawEllipse(
			&penDraw,
			pPt[0].X,
			pPt[0].Y,
			pPt[1].X - pPt[0].X,
			pPt[1].Y - pPt[0].Y);

		graph.DrawEllipse(
			&penDraw,
			pPt[2].X,
			pPt[2].Y,
			pPt[3].X - pPt[2].X,
			pPt[3].Y - pPt[2].Y);

		graph.DrawEllipse(
			&penDraw,
			pPt[4].X,
			pPt[4].Y,
			pPt[5].X - pPt[4].X,
			pPt[5].Y - pPt[4].Y);

		graph.DrawEllipse(
			&penDraw,
			pPt[6].X,
			pPt[6].Y,
			pPt[7].X - pPt[6].X,
			pPt[7].Y - pPt[6].Y);
	}
	else
	{
		if(m_bSelected || m_bEdit)
		{
			DrawHotShape(graph);
		}

		graph.ExcludeClip(&m_RgnErase);
		PointF* pPt = m_ptary.GetData();

		graph.DrawEllipse(
			&penDraw,
			pPt[0].X,
			pPt[0].Y,
			pPt[1].X - pPt[0].X,
			pPt[1].Y - pPt[0].Y);

		graph.DrawEllipse(
			&penDraw,
			pPt[2].X,
			pPt[2].Y,
			pPt[3].X - pPt[2].X,
			pPt[3].Y - pPt[2].Y);

		graph.DrawEllipse(
			&penDraw,
			pPt[4].X,
			pPt[4].Y,
			pPt[5].X - pPt[4].X,
			pPt[5].Y - pPt[4].Y);

		graph.DrawEllipse(
			&penDraw,
			pPt[6].X,
			pPt[6].Y,
			pPt[7].X - pPt[6].X,
			pPt[7].Y - pPt[6].Y);
	}
}

void CBalloon::FinishGraph()
{
	m_ptarytemp.RemoveAll();
	m_ptary.RemoveAt(8, 2);
	if(m_nPtCount == 8)
	{
		m_bFinish = TRUE;
		InitHotData();
	}
	return;
}

void CBalloon::SetRegion()
{
	PointF* pPt = m_ptary.GetData();
	int nWidth = m_nWidth;

	Pen penDraw(Color(255, 0, 255, 0));
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);
	
	GraphicsPath path;

	path.AddEllipse(
		pPt[0].X,
		pPt[0].Y,
		pPt[1].X - pPt[0].X,
		pPt[1].Y - pPt[0].Y);

	path.AddEllipse(
		pPt[2].X,
		pPt[2].Y,
		pPt[3].X - pPt[2].X,
		pPt[3].Y - pPt[2].Y);

	path.AddEllipse(
		pPt[4].X,
		pPt[4].Y,
		pPt[5].X - pPt[4].X,
		pPt[5].Y - pPt[4].Y);

	path.AddEllipse(
		pPt[6].X,
		pPt[6].Y,
		pPt[7].X - pPt[6].X,
		pPt[7].Y - pPt[6].Y);

	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CBalloon::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[8] = pt;
		m_nPtCount = 1;
	}
	else if(m_nPtCount >= 1)
	{
		m_ptary[9] = pt;
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

void CBalloon::NormalizeGraphData()
{	
	float fleft = min(m_ptary[8].X, m_ptary[9].X);
	float ftop = min(m_ptary[8].Y, m_ptary[9].Y);

	if(m_ptary[8].X == m_ptary[9].X
		&& m_ptary[8].Y == m_ptary[9].Y)
	{
		for(int i = 0; i < 8; i++)
		{
			m_ptary[i].X += fleft;
			m_ptary[i].Y += ftop;
		}
	}
	else
	{
		float fWidthOrg = m_ptary[1].X - m_ptary[0].X;
		float fHeightOrg = m_ptary[7].Y - m_ptary[0].Y;

		float fWidth = fabs(m_ptary[8].X - m_ptary[9].X);
		float fHeight = fabs(m_ptary[8].Y - m_ptary[9].Y);

		float fscalex = fWidth / fWidthOrg;
		float fscaley = fHeight / fHeightOrg;

		for(int i = 0; i < 8; i++)
		{
			m_ptary[i].X = m_ptary[i].X * fscalex + fleft;
			m_ptary[i].Y = m_ptary[i].Y * fscaley + ftop;
		}
	}
	
	m_ptary.RemoveAt(8, 2);
	m_nPtCount = 8;
}

void CBalloon::NormalizeTempData()
{
	m_ptarytemp.Copy(m_ptary);

	float fleft = min(m_ptarytemp[8].X, m_ptarytemp[9].X);
	float ftop = min(m_ptarytemp[8].Y, m_ptarytemp[9].Y);

	float fWidthOrg = m_ptarytemp[1].X - m_ptarytemp[0].X;
	float fHeightOrg = m_ptarytemp[7].Y - m_ptarytemp[0].Y;

	float fWidth = fabs(m_ptarytemp[8].X - m_ptarytemp[9].X);
	float fHeight = fabs(m_ptarytemp[8].Y - m_ptarytemp[9].Y);

	float fscalex = fWidth / fWidthOrg;
	float fscaley = fHeight / fHeightOrg;

	for(int i = 0; i < 8; i++)
	{
		m_ptarytemp[i].X = m_ptarytemp[i].X * fscalex + fleft;
		m_ptarytemp[i].Y = m_ptarytemp[i].Y * fscaley + ftop;
	}
}
