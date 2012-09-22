#include "stdafx.h"
#include "GraphLib.h"

CPolygon::CPolygon(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nMinCount = 3;
	m_bDbclk = FALSE;
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(0, 10);
	m_nDashStyle = dashstyle;
}

CPolygon::CPolygon()
{
	m_nWidth = 10;
	m_nMinCount = 3;
	m_bDbclk = FALSE;
	m_ptary.SetSize(0, 10);
	m_crColor = Color::Black;
}

CPolygon::CPolygon(const CPolygon& gph)
{
	m_bDbclk = gph.m_bDbclk;
	CopyTheBaseDataMember(&gph);
}

CPolygon & CPolygon::operator = (const CPolygon& gph)
{
	m_bDbclk = gph.m_bDbclk;
	CopyTheBaseDataMember(&gph);
	return *this;
}

CPolygon::~CPolygon()
{
	
}

void CPolygon::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x, (float)point.y);
	if(MS_LBTNDOWN == nMouseState)
	{
		if(m_nPtCount == 0)
		{
			m_ptary.Add(pt);
			m_ptary.Add(pt);
			m_nPtCount++;
		}
		else
		{
			m_ptary[m_nPtCount] = pt;
		}
	}
	else if(m_nPtCount >= 1)
	{
		if(MS_LBTNMOUSE == nMouseState)
		{
			m_ptary[m_nPtCount] = pt;
		}
		else if(MS_LBTNUP == nMouseState)
		{
			if(m_bDbclk)
			{
				m_bFinish = TRUE;
				InitHotData();
				rcUpdate = m_rcHot;
				return;
			}

			if(CalcGapOfpoints(pt, m_ptary[0]) <= MININTERVAL &&
				m_nPtCount >= 2)
			{
				m_ptary[m_nPtCount] = m_ptary[0];
				m_nPtCount++;
				m_bFinish = TRUE;
				InitHotData();
				rcUpdate = m_rcHot;
			}
			else
			{
				if(m_nPtCount == 1 && pt.Equals(m_ptary[0]))
				{
					return;
				}

				m_ptary[m_nPtCount] = pt;
				m_ptary.Add(pt);
				m_nPtCount++;
			}
		}
		else if(MS_LDCLK == nMouseState)
		{
			m_bDbclk = TRUE;
		}
	}
	else
	{
		rcUpdate.SetRect(0, 0, 1, 1);
	}
}

void CPolygon::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount == 0)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetLineJoin(LineJoinRound);
	penDraw.SetDashStyle(m_nDashStyle);

	PointF* pPt = m_ptary.GetData();

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}

	if(m_bFinish)
	{
		graph.ExcludeClip(&m_RgnErase);
		graph.DrawPolygon(&penDraw,pPt,m_nPtCount);
	}
	else
	{
		int nCount = (int)m_ptary.GetCount();
		graph.DrawLines(&penDraw,pPt,nCount);
	}
}

void CPolygon::FinishGraph()
{
	m_bFinish = TRUE;
	m_ptary.SetSize(m_nPtCount);
	InitHotData();
	return;
}

void CPolygon::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

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
