#include "StdAfx.h"
#include "GraphLib.h"

CWritingBrush::CWritingBrush()
{
	m_nWidth = 10;
	m_nMinCount = 2;
	m_ptary.SetSize(0,10);
	m_crColor = Color::Black;
	m_Region.MakeEmpty();
}

CWritingBrush::CWritingBrush(Color cr, int nWidth)
{
	m_crColor = cr;
	m_nMinCount = 2;
	m_nWidth = nWidth;
	m_ptary.SetSize(0,10);
	m_Region.MakeEmpty();
}

CWritingBrush::CWritingBrush(const CWritingBrush& gph)
{
	CopyTheBaseDataMember(&gph);
	m_nMinCount = 2;
}

CWritingBrush& CWritingBrush::operator = (const CWritingBrush& gph)
{
	CopyTheBaseDataMember(&gph);
	m_nMinCount = 2;

	return *this;
}

CWritingBrush::~CWritingBrush(void)
{
}

void CWritingBrush::SetRegion()
{
	ASSERT(m_bFinish);

	GraphicsPath path;
	Pen penDraw(m_crColor, m_nWidth * m_fScale);
	penDraw.SetStartCap(LineCapRound);
	penDraw.SetEndCap(LineCapRound);

	PointF* pPt = m_ptary.GetData();
	path.AddCurve(pPt, m_nPtCount);

	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CWritingBrush::FinishGraph()
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	m_bFinish = TRUE;
	InitHotData();
}

void CWritingBrush::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount <= 1)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeHighQuality);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	if(m_bSelected || m_bEdit)
	{
		graph.SetSmoothingMode(SmoothingModeAntiAlias);
		DrawHotShape(graph);
	}

	graph.ExcludeClip(&m_RgnErase);

	if(!m_bFinish)
	{
		Rect	rc(rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height());
		graph.SetClip(rc);
	}

	DrawWritingBrush(graph);
}

void CWritingBrush::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
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
		if(CalcGapOfpoints(pt, ptf) >= 1.0f)
		{
			m_ptary.Add(pt);
			m_nPtCount++;
		}
	}

	//form the invalid rect
	if(m_nPtCount >= 2 && !m_bFinish)
	{
		CPoint ptLT((int)m_ptary[m_nPtCount - 1].X, (int)m_ptary[m_nPtCount - 1].Y);
		CPoint ptRB((int)m_ptary[m_nPtCount - 2].X, (int)m_ptary[m_nPtCount - 2].Y);

		rcUpdate.SetRect(ptLT, ptRB);
		rcUpdate.NormalizeRect();

		rcUpdate.InflateRect(3 * m_nWidth, 3 * m_nWidth);
	}

	if(MS_LBTNUP == nMouseState)
	{
		if(( m_nPtCount <= 1))
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
}

void CWritingBrush::DrawWritingBrush(Graphics& graph)
{
	if(m_nPtCount < 2)
	{
		return;
	}

	float fwidth = (float)m_nWidth;

	if(m_bFinish)
	{
		fwidth *= m_fScale;
	}

	float fDrawWidth = fwidth;
	Pen penDraw(m_crColor, fDrawWidth);
	penDraw.SetStartCap(LineCapRound);
	penDraw.SetEndCap(LineCapRound);
	
	float fgap = 0;

	for(int i = 0; i < m_nPtCount - 1; i++)
	{
		fgap = CalcGapOfpoints(m_ptary[i], m_ptary[i + 1]);
		fDrawWidth = fwidth / (fwidth + fgap * 2) * fwidth;

		if(fDrawWidth <= 3)
		{
			fDrawWidth = 3;
		}

		penDraw.SetWidth(fDrawWidth);
		graph.DrawCurve(&penDraw, &m_ptary[i], 2);
	}
}