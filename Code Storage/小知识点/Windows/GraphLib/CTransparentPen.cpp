#include "StdAfx.h"
#include "GraphLib.h"

CTransparentPen::CTransparentPen(Color cr, int nWidth)
{
	m_noffst = 0;
	m_nDCount = 0;
	m_nAlpha = 150;
	m_nMinCount = 2;
	m_nWidth = nWidth;
	m_ptary.SetSize(0,10);
	SetColor(cr);
}

CTransparentPen::CTransparentPen(void)
{
	m_noffst = 0;
	m_nDCount = 0;
	m_nAlpha = 150;
	m_nMinCount = 2;
	m_nWidth = 5;
	m_ptary.SetSize(0,10);
}

CTransparentPen::CTransparentPen(const CTransparentPen& gph)
{
	CopyTheBaseDataMember(&gph);
	
	m_noffst = gph.m_noffst;
	m_nAlpha = gph.m_nAlpha;
	m_nDCount = gph.m_nDCount;
}

CTransparentPen& CTransparentPen::operator = (const CTransparentPen& gph)
{
	CopyTheBaseDataMember(&gph);
	
	m_noffst = gph.m_noffst;
	m_nAlpha = gph.m_nAlpha;
	m_nDCount = gph.m_nDCount;
	
	return *this;
}

CTransparentPen::~CTransparentPen(void)
{
}

void CTransparentPen::Draw(CDC* pDC,CRect& rcInvalid)
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
	penDraw.SetStartCap(LineCapRound);
	penDraw.SetEndCap(LineCapRound);
	penDraw.SetLineJoin(LineJoinRound);

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}

	graph.ExcludeClip(&m_RgnErase);

	if(!m_bFinish)
	{
		Rect rc(rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height());
		graph.SetClip(rc);
	}
	else
	{
		penDraw.SetWidth(m_fScale * m_nWidth);
	}

	int nCount = (int)m_ptary.GetCount();
	PointF* pPt = m_ptary.GetData();
	graph.DrawLines(&penDraw, pPt, nCount);
}

void CTransparentPen::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(m_nPtCount == 0)
	{
		m_ptary.Add(pt);
		m_nPtCount++;
	}
	else
	{
		PointF ptf = m_ptary[m_nPtCount - 1];
		if(CalcGapOfpoints(pt, ptf) >= 4)
		{
			m_ptary.Add(pt);
			m_nPtCount++;
		}
	}

	if(MS_LBTNUP == nMouseState)
	{
		if(( m_nPtCount <= 1) ||
			( m_nPtCount <= 2 && 
			CalcGapOfpoints(m_ptary[0],m_ptary[1]) <= 3 ))
		{
			m_bDataInvalid = TRUE;
		}
		else
		{
			m_bFinish = TRUE;
			InitHotData();
			rcUpdate = m_rcHot;
		}
	}

	if(m_nPtCount >= 2 && !m_bFinish)
	{
		CPoint ptLT((int)m_ptary[m_nPtCount - 1].X, (int)m_ptary[m_nPtCount - 1].Y);
		CPoint ptRB((int)m_ptary[m_nPtCount - 2].X, (int)m_ptary[m_nPtCount - 2].Y);

		rcUpdate.SetRect(ptLT, ptRB);
		rcUpdate.NormalizeRect();

		rcUpdate.InflateRect(m_nWidth,m_nWidth);
	}
}

void CTransparentPen::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

	Pen penDraw(m_crColor);
	penDraw.SetStartCap(LineCapRound);
	penDraw.SetEndCap(LineCapRound);
	penDraw.SetLineJoin(LineJoinRound);
	penDraw.SetWidth(m_nWidth * m_fScale);

	GraphicsPath path;
	path.AddCurve(pPt,m_nPtCount,0.8f);
	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CTransparentPen::FinishGraph()
{
	m_bFinish = TRUE;
	InitHotData();
	return;
}

void CTransparentPen::InitHotData()
{
	__super::InitHotData();
}

void CTransparentPen::SetColor(Color crColor)
{
	BYTE r = crColor.GetR();
	BYTE g = crColor.GetG();
	BYTE b = crColor.GetB();

	ARGB argb = Color::MakeARGB(m_nAlpha, r, g, b);
	m_crColor.SetValue(argb);
}

void CTransparentPen::SetAlpha(BYTE nalpha)
{
	m_nAlpha = nalpha;
	BYTE r = m_crColor.GetR();
	BYTE g = m_crColor.GetG();
	BYTE b = m_crColor.GetB();

	ARGB argb = Color::MakeARGB(nalpha, r, g, b);
	m_crColor.SetValue(argb);
}

BYTE CTransparentPen::GetAlpha()
{
	return m_nAlpha;
}
