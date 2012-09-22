#include "stdafx.h"
#include "GraphLib.h"

CNTriangle::CNTriangle(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 3;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(3, 1);
	m_bhas3temppts = FALSE;
	m_nDashStyle = dashstyle;
}

CNTriangle::CNTriangle()
{
	m_nWidth = 3;
	m_nMinCount = 3;
	m_ptary.SetSize(3, 1);
	m_bhas3temppts = FALSE;
	m_crColor = Color::Black;
}

CNTriangle::CNTriangle(const CNTriangle& gph)
{
	CopyTheBaseDataMember(&gph);
}

CNTriangle & CNTriangle::operator = (const CNTriangle& gph)
{
	CopyTheBaseDataMember(&gph);
	return *this;
}

CNTriangle::~CNTriangle()
{
}

void CNTriangle::Draw(CDC* pDC,CRect& rcInvalid)
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
		PointF* pPt = m_ptary.GetData();

		if(m_bhas3temppts)
		{
			Pen penPrev(Color::Blue, (float)m_nWidth);
			penPrev.SetDashStyle(DashStyleDash);
			graph.DrawLine(&penPrev, m_ptary[0], m_ptary[2]);
		}

		graph.DrawLines(&penDraw, pPt, m_nPtCount + 1);
	}
	else if(m_nPtCount == 3)
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

void CNTriangle::FinishGraph()
{
	if(3 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void CNTriangle::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

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

void CNTriangle::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x, (float)point.y);
	if(MS_LBTNDOWN == nMouseState)
	{
		m_ptary[m_nPtCount] = pt;
		if(m_nPtCount == 0)
		{
			m_nPtCount++;
			m_ptary[m_nPtCount] = pt;
		}
	}
	else if(m_nPtCount >= 1)
	{
		if(MS_LBTNMOUSE == nMouseState)
		{
			m_ptary[m_nPtCount] = pt;
			if(m_nPtCount == 2)
			{
				m_bhas3temppts = TRUE;
			}
		}
		else if(MS_LBTNUP == nMouseState)
		{
			if(m_nPtCount == 1)
			{
				if(pt.Equals(m_ptary[0]))
				{
					return;
				}
			}

			m_ptary[m_nPtCount] = pt;
			m_nPtCount++;

			if(m_nPtCount == 3)
			{
				m_bFinish = TRUE;
				m_bhas3temppts = FALSE;
				InitHotData();
				rcUpdate = m_rcHot;
			}
			else
			{
				m_ptary[m_nPtCount] = pt;
			}
		}
	}
	else
	{
		rcUpdate.SetRect(0, 0, 1, 1);
	}
}
