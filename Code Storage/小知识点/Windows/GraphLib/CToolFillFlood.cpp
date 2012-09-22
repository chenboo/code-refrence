#include "stdafx.h"
#include "GraphLib.h"

CToolFillFlood::CToolFillFlood(Color clr)
{
	m_crColor = clr;
	m_nMinCount = 2;
	m_foffsetX = 0.0f;
	m_foffsetY = 0.0f;
	m_pBmpST = NULL;
	m_ptary.SetSize(2, 2);
	m_bRegionFormed = FALSE;
}

CToolFillFlood::CToolFillFlood(const CToolFillFlood& gph)
{
	CopyTheBaseDataMember(&gph);

	m_pBmpST = NULL;
	m_foffsetX = 0.0f;
	m_foffsetY = 0.0f;
	m_rcBound = gph.m_rcBound;
	m_bRegionFormed = gph.m_bRegionFormed;

	ASSERT(gph.m_pBmpST);
	m_pBmpST = gph.m_pBmpST;
	m_pBmpST->IncreaseRefrence();
}

CToolFillFlood & CToolFillFlood::operator = (const CToolFillFlood& gph)
{
	CopyTheBaseDataMember(&gph);

	m_pBmpST = NULL;
	m_foffsetX = 0.0f;
	m_foffsetY = 0.0f;
	m_rcBound = gph.m_rcBound;
	m_bRegionFormed = gph.m_bRegionFormed;

	m_pBmpST = gph.m_pBmpST;
	m_pBmpST->IncreaseRefrence();
	return *this;
}

CToolFillFlood::~CToolFillFlood()
{
	if(m_pBmpST)
	{
		m_pBmpST->DecreaseRefrence();

		if(m_pBmpST->m_cRefrencce == 0)
		{
			SAFE_DELETE(m_pBmpST);
		}
	}
}

void CToolFillFlood::Draw(CDC* pDC, CRect& rcInvalid)
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle, m_ptCenter);
	graph.SetTransform(&mat);

	if(!m_bRegionFormed)
	{
		FormRegion(pDC);
	}
	else if(m_pBmpST)
	{
		if(m_bSelected || m_bEdit)
		{
			DrawHotShape(graph);
		}

		CRect rct = m_rcHot;
		rct.DeflateRect(HOTINTERVAL, HOTINTERVAL);
		RectF rcf = CRect2RectF(rct);

		if(!m_RgnErase.IsEmpty(&graph))
		{
			graph.ExcludeClip(&m_RgnErase);
		}

		graph.DrawImage(m_pBmpST->m_pBmp, rcf, 0, 0, rcf.Width, rcf.Height, UnitPixel);
	}
}

void CToolFillFlood::DrawHotShape(Graphics& graph)
{
	SolidBrush sbrush(Color::Green);
	Pen penDraw(Color::Blue, 2);
	penDraw.SetDashStyle(DashStyleDot);

	graph.DrawRectangle(&penDraw, m_rcHot.left, m_rcHot.top, m_rcHot.Width(), m_rcHot.Height());
	graph.DrawLine(&penDraw, m_HotPts.ptTC, m_HotPts.ptRotate);
	graph.FillEllipse(&sbrush, m_HotPts.ptRotate.X - HOTINTERVAL,
		m_HotPts.ptRotate.Y - HOTINTERVAL, 2.0 * HOTINTERVAL, 2.0 * HOTINTERVAL);

}

int CToolFillFlood::MoveGraph(float offsetx, float offsety)
{
	SetHotPoints();
	m_foffsetX += offsetx;
	m_foffsetY += offsety;
	m_RgnErase.Translate(offsetx, offsety);
	m_rcHot.OffsetRect((int)offsetx,(int)offsety);
	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	return ER_NO_ERRO;
}

void CToolFillFlood::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN == nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
		rcUpdate.SetRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	}
	else
	{
		m_nPtCount = 2;
		m_ptary[1] = pt;

		if(MS_LBTNUP == nMouseState)
		{
			if(NULL == m_pBmpST)
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
}

void CToolFillFlood::AddPoint(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	if(!m_bFinish)
	{
		FormGraphData(nMouseState, point, rcUpdate);
	}
	else
	{
		if(MS_LBTNDOWN == nMouseState && !IsPtInModifyZone(point))
		{
			SetSelectState(FALSE);
			SetEditSate(FALSE);
			return;
		}

		DecideModiFyType(nMouseState, point);
		if(MT_MOVE == m_ModifyType || MT_ROTATE == m_ModifyType)
		{
			rcUpdate = FormUpdateRect();
		}

		ModifyGraph(nMouseState, point);

		if(MS_LBTNUP == nMouseState)
		{
			SetRegion();
		}
	}
}

void CToolFillFlood::SetRegion()
{
	if(m_foffsetX != 0 || m_foffsetY != 0)
	{
		m_Region.Translate(m_foffsetX, m_foffsetY);
	}

	m_foffsetX = 0.0f;
	m_foffsetY = 0.0f;
	return;
}

void CToolFillFlood::FinishGraph()
{
	if(2 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void CToolFillFlood::InitHotData()
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	m_rcHot = m_rcBound;
	m_rcHot.InflateRect(HOTINTERVAL, HOTINTERVAL);

	SetHotPoints();
	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
}

ModifyType CToolFillFlood::AroundHotPts(CPoint point)
{
	if(!m_bFinish)
	{
		return MT_NONE;
	}

	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	CRect rect = m_rcHot;
	rect.InflateRect(7, 7);

	if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
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

void CToolFillFlood::DecideModiFyType(UINT nMouseState, CPoint point)
{
	if(nMouseState != MS_LBTNDOWN)
	{
		return;
	}

	//Translate the point for the point has been considered rotated so rotated back
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	CRect rect = m_rcHot;
	rect.InflateRect(7, 7);

	if(CalcGapOfpoints(pt,m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		m_ModifyType = MT_ROTATE;
	}
	else if(rect.PtInRect(CPoint((int)pt.X,(int)pt.Y)))
	{
		m_ModifyType = MT_MOVE;
		m_ptModifyOrg.X = (float)point.x;
		m_ptModifyOrg.Y = (float)point.y;
	}
	else
	{
		m_ModifyType = MT_NONE;
	}
}

COLORREF CToolFillFlood::GetPixel(int x, int y, int nbmWidthBytes, BYTE* pBmpBit)
{
	COLORREF clr = *((LONG *)(pBmpBit - y * nbmWidthBytes) + x);
	BYTE lr = GetBValue(clr);
	BYTE lg = GetGValue(clr);
	BYTE lb = GetRValue(clr);
	clr = RGB(lr, lg, lb);
	return clr;
}

void CToolFillFlood::SetPixel(int x, int y, int nbmWidthBytes, COLORREF clr, BYTE* pBmpBit)
{
	BYTE lr = GetBValue(clr);
	BYTE lg = GetGValue(clr);
	BYTE lb = GetRValue(clr);
	clr = RGB(lr, lg, lb);
	*((LONG *)(pBmpBit - y * nbmWidthBytes) + x) = clr;
}

void CToolFillFlood::FormRegion(CDC* pDC)
{
	ASSERT(m_nPtCount >= 0);
	CPoint pt((int)m_ptary[0].X, (int)m_ptary[0].Y);

	COLORREF clrReplace = pDC->GetPixel(pt);
	COLORREF clrFill = m_crColor.ToCOLORREF();
	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYSCREEN);
	CRect rect(0, 0, nWidth, nHeight);

	if(clrFill == clrReplace || !rect.PtInRect(pt))
	{
		return;
	}

	HDC hdcMem = CreateCompatibleDC(pDC->m_hDC);
	if(!hdcMem)
	{
		return;
	}

	BITMAPINFOHEADER bmpInfo = {
		sizeof(BITMAPINFOHEADER),
		nWidth,
		nHeight,
		1,
		32,
		BI_RGB,
		0,
		0,
		0,
		0,
		0
	};

	VOID* pbits = NULL;
	HBITMAP hbm = CreateDIBSection(hdcMem, (BITMAPINFO *)&bmpInfo, DIB_RGB_COLORS, &pbits, NULL, 0);
	if(!hbm)
	{
		return;
	}

	BITMAP bm;
	GetObject(hbm, sizeof(BITMAP), &bm);
	while(bm.bmWidthBytes % 4)
	{
		bm.bmWidthBytes++;
	}

	HBITMAP holdBmp = (HBITMAP)SelectObject(hdcMem, hbm);
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, pDC->m_hDC, 0, 0, SRCCOPY);
	BYTE *pBmBit = (BYTE *)bm.bmBits + (bm.bmHeight - 1) * bm.bmWidthBytes;
	SelectObject(hdcMem, holdBmp);

	BOOL bOk = FALSE;
	int nX = 0, nY = 0, nCount = 0;
	int nSize = nWidth * nHeight + 1;
	int nW = 0, nE = 0, nHead = 0, nEnd = 0;

	m_rcBound.left = pt.x;
	m_rcBound.right = 0;
	m_rcBound.top = pt.y;
	m_rcBound.bottom = 0;

	CPoint* pPt = new CPoint[nSize];
	pPt[nEnd].x = pt.x;
	pPt[nEnd].y = pt.y;
	nEnd++;

	Region rgn;
	rgn.MakeEmpty();
	Rect rc(0, 0, 1, 1);
	m_Region.MakeEmpty();

	while (nHead < nEnd)
	{
		nX = pPt[nHead].x;
		nY = pPt[nHead].y;
		m_rcBound.top = m_rcBound.top >= nY ? nY : m_rcBound.top;
		m_rcBound.bottom = m_rcBound.bottom <= nY ? nY : m_rcBound.bottom;

		if(GetPixel(nX, nY, bm.bmWidthBytes, pBmBit) == clrReplace)
		{
			nE = nW = nX;

			while( nW - 1 >= 0 
				&& GetPixel(nW - 1, nY, bm.bmWidthBytes, pBmBit)== clrReplace)
			{
				nW--;
			}

			while(nE + 1 < nWidth 
				&& GetPixel(nE + 1, nY, bm.bmWidthBytes, pBmBit) == clrReplace)
			{
				nE++;
			}

			rc.X = nW;
			rc.Y = nY;
			bOk = TRUE;
			rc.Height = 1;
			rc.Width = nE - nW + 1;
			rgn.Union(rc);
			m_rcBound.left = m_rcBound.left >= nW ? nW : m_rcBound.left;
			m_rcBound.right = m_rcBound.right <= nE ? nE : m_rcBound.right;

			for(int i = nW; i <= nE; i++)
			{
				SetPixel(i, nY, bm.bmWidthBytes, clrFill, pBmBit);

				if(nY - 1 >= 0 
					&& GetPixel(i, nY - 1, bm.bmWidthBytes, pBmBit) == clrReplace)
				{
					pPt[nEnd].x = i;
					pPt[nEnd].y = nY - 1;
					nEnd++;
				}

				if(nY + 1 < nHeight
					&& GetPixel(i, nY + 1, bm.bmWidthBytes, pBmBit) == clrReplace)
				{
					pPt[nEnd].x = i;
					pPt[nEnd].y = nY + 1;
					nEnd++;
				}
			}
		}

		nHead++;
		if(nHead % 500 == 0 && bOk)
		{
			m_Region.Union(&rgn);
			rgn.MakeEmpty();
			bOk = FALSE;
		}
	}

	SelectObject(hdcMem, hbm);
	BitBlt(pDC->m_hDC, 0, 0, nWidth, nHeight, hdcMem, 0, 0, SRCCOPY);
	DeleteObject(SelectObject(hdcMem, holdBmp));
	DeleteDC(hdcMem);

	delete [] pPt;
	pPt = NULL;
	m_Region.Union(&rgn);
	m_bRegionFormed = TRUE;
	Graphics graph(pDC->m_hDC);
	m_rcBound.InflateRect(1, 1);
	nWidth = m_rcBound.Width();
	nHeight = m_rcBound.Height();

	if(nWidth != 0 || nHeight != 0)
	{
		m_pBmpST = new Bitmap_ST(nWidth, nHeight);
		ASSERT(m_pBmpST);
		Graphics graph2(m_pBmpST->m_pBmp);

		Matrix mat;
		mat.Translate(- (float)m_rcBound.left, - (float)m_rcBound.top);
		graph2.SetTransform(&mat);
		SolidBrush br(m_crColor);
		graph2.FillRegion(&br, &m_Region);
	}

	return;
}
