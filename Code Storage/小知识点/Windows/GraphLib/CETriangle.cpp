#include "stdafx.h"
#include "GraphLib.h"

CETriangle::CETriangle(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 3;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(3,1);
	m_nDashStyle = dashstyle;
}

CETriangle::CETriangle()
{

}

CETriangle::CETriangle(const CETriangle& gph)
{
	CopyTheBaseDataMember(&gph);
}

CETriangle & CETriangle::operator = (const CETriangle& gph)
{
	CopyTheBaseDataMember(&gph);
	return *this;
}

CETriangle::~CETriangle()
{
}

void CETriangle::Draw(CDC* pDC,CRect& rcInvalid)
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

	if(m_nPtCount == 2)
	{
		DrawPreView(graph, penDraw);
	}
	else if(m_nPtCount == 3 && m_bFinish)
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

void CETriangle::DrawPreView(Graphics& graph, Pen& penDraw)
{
	CPoint ptLT;
	CPoint ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;
	CRect rect(ptLT,ptRB);
	rect.NormalizeRect();
	
	float fhalfsidelength = rect.Height() / sqrt(3.0f);

	PointF pt[3];
	pt[0] = m_ptary[0];
	pt[1].X = pt[0].X - fhalfsidelength;
	pt[1].Y = m_ptary[1].Y;
	pt[2].X = pt[0].X + fhalfsidelength;
	pt[2].Y = pt[1].Y;

	graph.DrawPolygon(&penDraw, pt, 3);
}

void CETriangle::FinishGraph()
{
	if(m_nPtCount == 3)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void CETriangle::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

	Pen penDraw(Color(255, 0, 255,  0));
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);
	
	GraphicsPath path;
	path.AddPolygon(pPt, m_nPtCount);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CETriangle::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x, (float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
	}
	else if(m_nPtCount >= 1)
	{
		m_ptary[1] = pt;
		m_nPtCount = 2;

		if(MS_LBTNUP == nMouseState)
		{
			if(m_nPtCount >= 1)
			{
				m_bFinish = TRUE;
				m_nPtCount = 3;
				NormalizeGraphData();
				InitHotData();
				rcUpdate = m_rcHot;
			}
		}
	}
	else
	{
		rcUpdate.SetRect(0, 0, 1, 1);
	}
}

void CETriangle::NormalizeGraphData()
{
	if(m_ptary[0].X == m_ptary[1].X
		&& m_ptary[0].Y == m_ptary[1].Y)
	{
		m_ptary[1].X = m_ptary[0].X - 60.0f / sqrt(3.0f);
		m_ptary[1].Y = m_ptary[0].Y + 60.0f;
		m_ptary[2].X = m_ptary[0].X + 60.0f / sqrt(3.0f);
		m_ptary[2].Y = m_ptary[0].Y + 60.0f;
	}
	else
	{
		CPoint ptLT;
		CPoint ptRB;
		ptLT.x = (int)m_ptary[0].X;
		ptLT.y = (int)m_ptary[0].Y;
		ptRB.x = (int)m_ptary[1].X;
		ptRB.y = (int)m_ptary[1].Y;
		CRect rect(ptLT,ptRB);
		rect.NormalizeRect();

		float fhalfsidelength = rect.Height() / sqrt(3.0f);

		m_ptary[1].X = m_ptary[0].X - fhalfsidelength;
		m_ptary[2].X = m_ptary[0].X + fhalfsidelength;
		m_ptary[2].Y = m_ptary[1].Y;
	}
}
