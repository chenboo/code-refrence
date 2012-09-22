#include "stdafx.h"
#include "GraphLib.h"

CCameralRgn::CCameralRgn()
{
	m_nMinCount = 2;
	m_bFormed = FALSE;
	m_ptary.SetSize(2, 2);
}

CCameralRgn::CCameralRgn(const CCameralRgn& gph)
{
	CopyTheBaseDataMember(&gph);

	if(m_bitmap.m_hObject)
	{
		m_bitmap.DeleteObject();
	}

	HBITMAP hbmp = CopyBitmap((HBITMAP)(gph.m_bitmap.m_hObject));
	m_bitmap.Attach(hbmp);
	m_bFormed = gph.m_bFormed;
}

CCameralRgn & CCameralRgn::operator = (const CCameralRgn& gph)
{
	CopyTheBaseDataMember(&gph);

	if(m_bitmap.m_hObject)
	{
		m_bitmap.DeleteObject();
	}

	HBITMAP hbmp = CopyBitmap((HBITMAP)(gph.m_bitmap.m_hObject));
	m_bitmap.Attach(hbmp);

	m_bFormed = gph.m_bFormed;
	return *this;
}

HBITMAP CCameralRgn::CopyBitmap(HBITMAP hSourceHbitmap)
{
	CDC destDC;
	CDC sourceDC;
	destDC.CreateCompatibleDC(NULL);
	sourceDC.CreateCompatibleDC(NULL);

	BITMAP bm = {0};
	::GetObject(hSourceHbitmap, sizeof(bm), &bm);

	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), bm.bmWidth, bm.bmHeight);
	HBITMAP hbmOldSource = (HBITMAP)::SelectObject(sourceDC.m_hDC, hSourceHbitmap);
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject(destDC.m_hDC, hbmResult);
	destDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, SRCCOPY );
	::SelectObject(sourceDC.m_hDC, hbmOldSource);
	::SelectObject(destDC.m_hDC, hbmOldDest);
	::DeleteObject(sourceDC.m_hDC);
	::DeleteObject(destDC.m_hDC);

	return hbmResult;
}

CCameralRgn::~CCameralRgn()
{
	if(m_bitmap.GetSafeHandle())
	{
		m_bitmap.DeleteObject();
	}
}

void CCameralRgn::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount <= 1)
	{
		return;
	}
		
	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle, m_ptCenter);
	graph.SetTransform(&mat);

	CPoint ptLT;
	CPoint ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;
	CRect rc(ptLT, ptRB);
	rc.NormalizeRect();
	
	if(!m_bFormed)
	{
		Pen penDraw(Color::Blue, 3.0f);
		penDraw.SetDashStyle(DashStyleDash);
		
		INT offsetX = (rc.right - rc.left) * 20 / 100;
		INT offsetY = (rc.bottom - rc.top) * 20 / 100;
		GraphicsPath pt;
		pt.AddArc(rc.right - offsetX, rc.top, offsetX, offsetY, 270, 90);
		pt.AddArc(rc.right - offsetX, rc.bottom - offsetY, offsetX, offsetY, 0, 90);
		pt.AddArc(rc.left, rc.bottom - offsetY, offsetX, offsetY, 90, 90);
		pt.AddArc(rc.left, rc.top, offsetX, offsetY, 180, 90);
		pt.AddLine(rc.left + offsetX, rc.top, rc.right - offsetX / 2, rc.top);

		graph.DrawPath(&penDraw,&pt);
	}
	else
	{
		if(!m_bitmap.GetSafeHandle())
		{
			InitBitmap(pDC);
		}

		graph.ExcludeClip(&m_RgnErase);
		Bitmap bmp((HBITMAP)(m_bitmap.m_hObject), NULL);
		graph.DrawImage(&bmp, m_rcHot.left, m_rcHot.top, m_rcHot.Width(), m_rcHot.Height());

		if(m_bSelected || m_bEdit)
		{
			graph.SetClip(&m_RgnErase, CombineModeUnion);
			DrawHotShape(graph);
		}
	}
}

void CCameralRgn::InitBitmap(CDC* pDC)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	int nWidth = m_rcHot.Width();
	int nHeight = m_rcHot.Height();

	m_bitmap.CreateCompatibleBitmap(pDC, nWidth, nHeight);
	CBitmap* pOldbmp = dcMem.SelectObject(&m_bitmap);
	dcMem.BitBlt(0, 0, nWidth, nHeight, pDC, m_rcHot.left, m_rcHot.top, SRCCOPY);
	dcMem.SelectObject(pOldbmp);
	dcMem.DeleteDC();
	
	ASSERT(m_bitmap.m_hObject);
}

void CCameralRgn::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x, (float)point.y);
	if(MS_LBTNDOWN == nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
	}
	else
	{
		m_ptary[1] = pt;
		m_nPtCount = 2;
		if(MS_LBTNUP == nMouseState)
		{
			m_bFormed = TRUE;
			m_bSelected = TRUE;
			NormalizeData();
			InitHotData();
			rcUpdate = FormUpdateRect();
		}
	}
}

void CCameralRgn::NormalizeData()
{
	CPoint ptLT, ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;
	CRect rct(ptLT,ptRB);
	rct.NormalizeRect();
	CPoint ptCenter = rct.CenterPoint();

	if(rct.Width() < 5 || rct.Height() < 5)
	{
		m_ptary[0].X = (float)(ptCenter.x - 20);
		m_ptary[0].Y = (float)(ptCenter.y - 30);

		m_ptary[1].X = (float)(ptCenter.x + 20);
		m_ptary[1].Y = (float)(ptCenter.y + 30);
	}
	else
	{
		m_ptary[0].X = (float)rct.left;
		m_ptary[0].Y = (float)rct.top;
		m_ptary[1].X = (float)rct.right;
		m_ptary[1].Y = (float)rct.bottom;
	}
}

void CCameralRgn::SetRegion()
{
	if(!m_bFinish)
	{
		return;
	}
	
	m_Region.MakeEmpty();
	RectF rcf = CRect2RectF(m_rcHot);
	m_Region.Union(rcf);
}

void CCameralRgn::InitHotData()
{
	if(m_nPtCount <= 1)
	{
		return;
	}

	m_rcHot.left = (int)m_ptary[0].X;
	m_rcHot.top = (int)m_ptary[0].Y;
	m_rcHot.right = (int)m_ptary[1].X;
	m_rcHot.bottom = (int)m_ptary[1].Y;
	
	SetHotPoints();
	m_nOrgWidth = m_rcHot.Width();
	m_nOrgHeight = m_rcHot.Height();

	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	if(m_bFormed)
	{
		SetRegion();
	}
}

void CCameralRgn::FinishGraph()
{
	if(2 == m_nPtCount)
	{
		m_bFinish = TRUE;
		m_bEdit = FALSE;
		m_bSelected = FALSE;
		InitHotData();
	}

	return;
}

ModifyType CCameralRgn::AroundHotPts(CPoint point)
{
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	CRect rect = m_rcHot;
	rect.InflateRect(7, 7);

	if(CalcGapOfpoints(pt, m_HotPts.ptLT) <= HOTINTERVAL)
	{
		return MT_LT;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptTC) <= HOTINTERVAL)
	{
		return MT_TC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRT) <= HOTINTERVAL)
	{
		return MT_RT;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptLC) <= HOTINTERVAL)
	{
		return MT_LC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRC) <= HOTINTERVAL)
	{
		return MT_RC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptLB) <= HOTINTERVAL)
	{
		return MT_LB;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptBC) <= HOTINTERVAL)
	{
		return MT_BC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRB) <= HOTINTERVAL)
	{
		return MT_RB;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		return MT_ROTATE;
	}
	else if(rect.PtInRect(CPoint((int)pt.X, (int)pt.Y)))
	{
		return MT_MOVE;
	}
	else
	{
		return MT_NONE;
	}
}

void CCameralRgn::AddPoint(UINT nMouseState,CPoint point, CRect& rcUpdate)
{
	if(!m_bFormed)
	{
		FormGraphData(nMouseState, point, rcUpdate);
	}
	else if(MS_LBTNDOWN == nMouseState && !IsPtInModifyZone(point))
	{
		m_bSelected = FALSE;
		SetEditSate(FALSE);
		m_bFinish = TRUE;
		InitHotData();
		rcUpdate = FormUpdateRect();
		return;
	}

	DecideModiFyType(nMouseState, point);
	ModifyGraph(nMouseState, point);

	if(MS_LBTNUP == nMouseState)
	{
		SetRegion();
	}
}