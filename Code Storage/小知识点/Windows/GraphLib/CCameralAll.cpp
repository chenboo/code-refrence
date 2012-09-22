#include "stdafx.h"
#include "GraphLib.h"

CCameralAll::CCameralAll()
{
	m_nPtCount = 2;
	m_nMinCount = 2;
	m_ptary.SetSize(2, 2);
	m_nOrgWidth = GetSystemMetrics(SM_CXSCREEN);
	m_nOrgHeight = GetSystemMetrics(SM_CYSCREEN);

	m_ptary[0].X = 0.0f;
	m_ptary[0].Y = 0.0f;
	m_ptary[1].X = (float)m_nOrgWidth;
	m_ptary[1].Y = (float)m_nOrgHeight;
}

CCameralAll::CCameralAll(const CCameralAll& gph)
{
	CopyTheBaseDataMember(&gph);

	m_nOrgWidth = gph.m_nOrgWidth;
	m_nOrgHeight = gph.m_nOrgHeight;

	if(m_bitmap.m_hObject)
	{
		m_bitmap.DeleteObject();
	}

	HBITMAP hbmp = CopyBitmap((HBITMAP)(gph.m_bitmap.m_hObject));
	m_bitmap.Attach(hbmp);
}

CCameralAll& CCameralAll::operator = (const CCameralAll& gph)
{
	CopyTheBaseDataMember(&gph);

	m_nOrgWidth = gph.m_nOrgWidth;
	m_nOrgHeight = gph.m_nOrgHeight;

	if(m_bitmap.m_hObject)
	{
		m_bitmap.DeleteObject();
	}

	HBITMAP hbmp = CopyBitmap((HBITMAP)(gph.m_bitmap.m_hObject));
	m_bitmap.Attach(hbmp);

	return *this;
}

CCameralAll::~CCameralAll(void)
{
	if(m_bitmap.GetSafeHandle())
	{
		m_bitmap.DeleteObject();
	}
}

HBITMAP CCameralAll::CopyBitmap(HBITMAP hSourceHbitmap)
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

void CCameralAll::Draw(CDC* pDC,CRect& rcInvalid)
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

void CCameralAll::InitPhotoData()
{
	m_bEdit = TRUE;
	m_bSelected = TRUE;
	InitHotData();

	RectF rcf = CRect2RectF(m_rcHot);
	m_Region.MakeEmpty();
	m_Region.Union(rcf);
}

void CCameralAll::InitBitmap(CDC* pDC)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);

	m_bitmap.CreateCompatibleBitmap(pDC, m_nOrgWidth, m_nOrgHeight);
	CBitmap* pOldbmp = dcMem.SelectObject(&m_bitmap);
	dcMem.BitBlt(0, 0, m_nOrgWidth, m_nOrgHeight, pDC, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOldbmp);
	dcMem.DeleteDC();

	m_ptary[0].X = (float)100;
	m_ptary[0].Y = (float)100;
	m_ptary[1].X = (float)100 + m_nOrgWidth;
	m_ptary[1].Y = (float)100 + m_nOrgHeight;
	InitPhotoData();
}

void CCameralAll::InitHotData()
{
	if(m_nPtCount <= 0)
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

	if(m_bFinish)
	{
		SetRegion();
	}
}

void CCameralAll::FinishGraph()
{
	if(m_bitmap.GetSafeHandle())
	{
		m_bFinish = TRUE;
		m_bEdit = FALSE;
		m_bSelected = FALSE;
		InitHotData();
	}

	return;
}

void CCameralAll::SetRegion()
{
	if(!m_bFinish)
	{
		return;
	}
	
	m_Region.MakeEmpty();
	RectF rcf = CRect2RectF(m_rcHot);
	m_Region.Union(rcf);
}

void CCameralAll::AddPoint(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	if(!m_bFinish)
	{
		if(MS_LBTNDOWN == nMouseState && !IsPtInModifyZone(point))
		{
			m_bSelected = FALSE;
			SetEditSate(FALSE);
			m_bFinish = TRUE;
			InitHotData();
		}
	}
	else
	{
		rcUpdate = FormUpdateRect();

		if(MS_LBTNDOWN == nMouseState && !IsPtInModifyZone(point))
		{
			m_bSelected = FALSE;
			SetEditSate(FALSE);
			return;
		}
	}

	DecideModiFyType(nMouseState, point);
	ModifyGraph(nMouseState, point);

	if(MS_LBTNUP == nMouseState)
	{
		SetRegion();
	}
}

ModifyType CCameralAll::AroundHotPts(CPoint point)
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
