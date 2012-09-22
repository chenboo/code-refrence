#include "stdafx.h"
#include "GraphLib.h"

CErasePen::CErasePen(Color clr, HBITMAP hbmp)
{
	if(hbmp == NULL)
	{
		m_crColor = clr;
	}

	m_hBmp = hbmp;
	m_nMinCount = 2;
	m_ptary.SetSize(0, 10);
}

CErasePen::CErasePen(const CErasePen& gph)
{
	CopyTheBaseDataMember(&gph);
}

CErasePen & CErasePen::operator = (const CErasePen& gph)
{
	CopyTheBaseDataMember(&gph);
	return *this;
}

CErasePen::~CErasePen()
{
	if(m_hBmp)
	{
		::DeleteObject(m_hBmp);
		m_hBmp = NULL;
	}
}

void CErasePen::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount == 0)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	Rect rc(rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height());

	int nCount = (int)m_ptary.GetCount();
	PointF* pPt = m_ptary.GetData();

	if(!m_bFinish)
	{
		graph.SetClip(rc);
	}

	if(m_hBmp)
	{
		Bitmap bm(m_hBmp, NULL);
		TextureBrush tbrush(&bm);
		Pen penDraw(&tbrush, 30);
		penDraw.SetEndCap(LineCapRound);
		penDraw.SetStartCap(LineCapRound);
		penDraw.SetLineJoin(LineJoinRound);
		graph.DrawCurve(&penDraw, pPt, nCount);

		graph.FillEllipse(&tbrush, m_ptary[0].X - 15.0f, m_ptary[0].Y - 15.0f, 30.0f, 30.0f);
		graph.DrawCurve(&penDraw, pPt, nCount);
	}
	else
	{
		Pen penDraw(m_crColor, 30);
		penDraw.SetEndCap(LineCapRound);
		penDraw.SetStartCap(LineCapRound);
		penDraw.SetLineJoin(LineJoinRound);

		SolidBrush brush(m_crColor);
		graph.FillEllipse(&brush, m_ptary[0].X - 15.0f, m_ptary[0].Y - 15.0f, 30.0f, 30.0f);
		graph.DrawCurve(&penDraw, pPt, nCount);
	}
}

void CErasePen::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);

	if(m_nPtCount == 0)
	{
		m_ptary.Add(pt);
		m_nPtCount++;
		rcUpdate.SetRect((int)pt.X - 16, (int)pt.Y - 16, (int)pt.X + 16, (int)pt.Y + 16);
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
		if(m_nPtCount == 1)
		{
			m_ptary.Add(pt);
			m_nPtCount++;
		}

		m_bFinish = TRUE;
		SetRegion();
	}

	if(m_nPtCount >= 2 && !m_bFinish)
	{
		CPoint ptLT((int)m_ptary[m_nPtCount - 1].X, (int)m_ptary[m_nPtCount - 1].Y);
		CPoint ptRB((int)m_ptary[m_nPtCount - 2].X, (int)m_ptary[m_nPtCount - 2].Y);

		rcUpdate.SetRect(ptLT, ptRB);
		rcUpdate.NormalizeRect();
		rcUpdate.InflateRect(16, 16);
	}
}

void CErasePen::FinishGraph()
{
	m_bFinish = TRUE;
	InitHotData();
	return;
}

void CErasePen::SetRegion()
{
	ASSERT(m_bFinish);
	GraphicsPath path;
	PointF* pPt = m_ptary.GetData();

	if(m_nPtCount == 2 && CalcGapOfpoints(pPt[0], pPt[1]) <= 3)
	{
		RectF rcf(pPt[0].X - 15, pPt[1].Y - 15, 30.0f, 30.0f);
		path.AddEllipse(rcf);
	}
	else
	{
		Pen penDraw(m_crColor, 30.0f);
		penDraw.SetEndCap(LineCapRound);
		penDraw.SetStartCap(LineCapRound);
		penDraw.SetLineJoin(LineJoinRound);
		penDraw.SetDashStyle(DashStyleSolid);

		path.AddCurve(pPt, m_nPtCount);
		path.Widen(&penDraw);
		path.Outline();
	}

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

