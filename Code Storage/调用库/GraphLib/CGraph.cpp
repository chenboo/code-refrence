#include "stdafx.h"
#include "CGraph.h"
#include <math.h>

CGraph::CGraph()
{
	m_nPtCount = 0;
	m_ptCenter.X = 0.0f;
	m_ptCenter.Y = 0.0f;
	m_fAngle = 0.0f;
	m_bFinish = FALSE;
	m_bSelected = FALSE;
	m_ModifyType = MT_NONE;
}

CGraph::CGraph(const CGraph& gph)
{
	CopyBase(&gph);
}

void CGraph::CopyBase(const CGraph* pgph)
{
	m_nWidth = pgph->m_nWidth;
	m_nPtCount = pgph->m_nPtCount;
	m_nMinCount = pgph->m_nMinCount;
	m_crColor = pgph->m_crColor;
	m_ModifyType = pgph->m_ModifyType;

	m_HotPts = pgph->m_HotPts;		
	m_rcHot = pgph->m_rcHot;		
		
	m_bFinish = pgph->m_bFinish;		
	m_bSelected = pgph->m_bSelected;	

	m_ptCenter = pgph->m_ptCenter;		
	m_ptModifyOrg = pgph->m_ptModifyOrg;
	m_fAngle = pgph->m_fAngle;	
}

CGraph::~CGraph()
{
}

void CGraph::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
}

void CGraph::SetColor(Color crColor)
{
	m_crColor = crColor;
}

void CGraph::InitHotData(BOOL bExtend)
{
	if(m_nPtCount <= 0)
	{
		return;
	}
	m_rcHot.left = m_rcHot.right = (int)m_ptary[0].X;
	m_rcHot.top = m_rcHot.bottom = (int)m_ptary[0].Y;
	for(int i = 0;i < m_nPtCount;i++)
	{
		if(m_rcHot.left > (int)m_ptary[i].X)
		{
			m_rcHot.left = (int)m_ptary[i].X;
		}
		else if(m_rcHot.right < (int)m_ptary[i].X)
		{
			m_rcHot.right = (int)m_ptary[i].X;
		}
		if(m_rcHot.top > (int)m_ptary[i].Y)
		{
			m_rcHot.top = (int)m_ptary[i].Y;
		}
		else if(m_rcHot.bottom < (int)m_ptary[i].Y)
		{
			m_rcHot.bottom = (int)m_ptary[i].Y;
		}
	}

	if(bExtend)
	{
		m_rcHot.InflateRect(2 * HOTINTERVAL,2 * HOTINTERVAL);
	}
	else
	{
		m_rcHot.InflateRect(HOTINTERVAL,HOTINTERVAL);
	}

	SetHotPoints();
	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
	SetRegion();
}

void CGraph::DeceideModiFyType(UINT nMouseState,CPoint point)
{
	if(nMouseState != MS_LBTNDOWN)
		return;

	PointF pt((float)point.x,(float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle,m_ptCenter);
	mat.TransformPoints(&pt,1);

	if(CalGapOfpoints(pt,m_HotPts.ptLT) <= HOTINTERVAL)
	{
		m_ModifyType = MT_LT;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptTC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_TC;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRT) <= HOTINTERVAL)
	{
		m_ModifyType = MT_RT;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptLC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_LC;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_RC;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptLB) <= HOTINTERVAL)
	{
		m_ModifyType = MT_LB;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptBC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_BC;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRB) <= HOTINTERVAL)
	{
		m_ModifyType = MT_RB;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		m_ModifyType = MT_ROTATE;
	}
	else if(m_rcHot.PtInRect(CPoint((int)pt.X,(int)pt.Y)))
	{
		m_ModifyType = MT_MOVE;
		m_ptModifyOrg.X = (float)point.x;
		m_ptModifyOrg.Y = (float)point.y;
	}
	else
	{
		m_ModifyType = MT_NONE;
	}

	AdjustGraph(m_fAngle);
}

void CGraph::ModifyGraph(UINT nMouseState,CPoint point)
{
	if(MS_LBTNMOUSE != nMouseState)
		return;
	if(MT_NONE == m_ModifyType)
	{
		return;
	}
	else if(MT_MOVE == m_ModifyType)
	{
		MoveGraph(point);
	}
	else if(MT_ROTATE == m_ModifyType)
	{
		RotateGraph(point);
	}
	else
	{
		StretchGraph(point);
	}
}

float CGraph::CalGapOfpoints(PointF ptStart,PointF ptEnd)
{
	float x = ptEnd.X - ptStart.X;
	float y = ptEnd.Y - ptStart.Y;
	float l = sqrt((float)(x * x + y * y));
	return l;
}

float CGraph::CalGapOfpoints(CPoint ptStart,CPoint ptEnd)
{
	PointF pts((float)ptStart.x,(float)ptStart.y);
	PointF pte((float)ptEnd.x,(float)ptEnd.y);
	return CalGapOfpoints(pts,pte);
}

BOOL CGraph::bPtInHotRegion(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle,m_ptCenter);
	mat.TransformPoints(&pt,1);

	return m_Region.IsVisible(pt);
}

void CGraph::DrawHotShape(Graphics& graph)
{
	SolidBrush sbrush(Color::Green);
	Pen penDraw(Color::Blue,2);
	penDraw.SetDashStyle(DashStyleDot);

	graph.DrawRectangle(&penDraw,m_rcHot.left,m_rcHot.top,m_rcHot.Width(),m_rcHot.Height());
	graph.DrawLine(&penDraw,m_HotPts.ptTC,m_HotPts.ptRotate);
	graph.FillEllipse(&sbrush,m_HotPts.ptRotate.X - HOTINTERVAL,
		m_HotPts.ptRotate.Y - HOTINTERVAL,2.0 * HOTINTERVAL,2.0 * HOTINTERVAL);

	DrawEightpoints(graph);
}

void CGraph::MoveGraph(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	float offsetx = pt.X - m_ptModifyOrg.X;
	float offsety = pt.Y - m_ptModifyOrg.Y;
	m_ptModifyOrg.X = pt.X;
	m_ptModifyOrg.Y = pt.Y;
	MoveGraph(offsetx,offsety);
}

void CGraph::StretchGraph(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle,m_ptCenter);
	mat.TransformPoints(&pt,1);

	CPoint ptLT = m_rcHot.TopLeft();
	CPoint ptRB = m_rcHot.BottomRight();
	float fsx = 0.0,fsy = 0.0;
	PointF pts;

	if(MT_LT == m_ModifyType)
	{
		ptLT.x = (int)pt.X;
		ptLT.y = (int)pt.Y;
		pts = m_HotPts.ptRB;
	}
	else if(MT_LB == m_ModifyType)
	{
		ptLT.x = (int)pt.X;
		ptLT.y = (int)m_rcHot.top;
		ptRB.x = (int)m_rcHot.right;
		ptRB.y = (int)pt.Y;
		pts = m_HotPts.ptRT;
	}
	else if(MT_RT == m_ModifyType)
	{
		ptLT.x = m_rcHot.left;
		ptLT.y = (int)pt.Y;
		ptRB.x = (int)pt.X;
		ptRB.y = m_rcHot.bottom;
		pts = m_HotPts.ptLB;
	}
	else if(MT_RB == m_ModifyType)
	{
		ptRB.x = (int)pt.X;
		ptRB.y = (int)pt.Y;
		pts = m_HotPts.ptLT;
	}
	else if(MT_TC == m_ModifyType)
	{
		ptLT.x = m_rcHot.left;
		ptLT.y = (int)pt.Y;
		pts = m_HotPts.ptBC;
	}
	else if(MT_LC == m_ModifyType)
	{
		ptLT.x = (int)pt.X;
		ptLT.y = m_rcHot.top;
		pts = m_HotPts.ptRC;
	}
	else if(MT_BC == m_ModifyType)
	{
		ptRB.x = m_rcHot.right;
		ptRB.y = (int)pt.Y;
		pts = m_HotPts.ptTC;
	}
	else if(MT_RC == m_ModifyType)
	{
		ptRB.x = (int)pt.X;
		ptRB.y = m_rcHot.bottom;
		pts = m_HotPts.ptLC;
	}

	fsx = (float)(ptRB.x - ptLT.x) / m_rcHot.Width();
	fsy = (float)(ptRB.y - ptLT.y) / m_rcHot.Height();


	if(ptRB.x - ptLT.x <= 10 || ptRB.y - ptLT.y <= 10)
	{
		return;
	}

	PointF* pPt = m_ptary.GetData();
	for(int i = 0;i < m_nPtCount;i++)
	{
		pPt[i].X = pts.X + (pPt[i].X - pts.X) * fsx;
		pPt[i].Y = pts.Y + (pPt[i].Y - pts.Y) * fsy;
	}

	m_rcHot.SetRect(ptLT,ptRB);
	m_rcHot.NormalizeRect();
	SetHotPoints();
}

void CGraph::AdjustGraph(float fAngle)
{
	Matrix mat;
	mat.RotateAt(fAngle,m_ptCenter);

	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
	PointF pt = m_ptCenter;

	mat.TransformPoints(&m_ptCenter,1);

	float offx = m_ptCenter.X - pt.X;
	float offy = m_ptCenter.Y - pt.Y;

	if(offx != 0 && offy != 0)
	{
		MoveGraph(offx,offy);
	}
}

void CGraph::SetHotPoints()
{
	m_HotPts.ptLT.X = (float)m_rcHot.left;
	m_HotPts.ptLT.Y = (float)m_rcHot.top;
	m_HotPts.ptRB.X = (float)m_rcHot.right;
	m_HotPts.ptRB.Y = (float)m_rcHot.bottom;
	m_HotPts.ptLB.X = (float)m_rcHot.left;
	m_HotPts.ptLB.Y = (float)m_rcHot.bottom;
	m_HotPts.ptRT.X = (float)m_rcHot.right;
	m_HotPts.ptRT.Y = (float)m_rcHot.top;
	m_HotPts.ptTC.Y = (float)m_rcHot.top;
	m_HotPts.ptTC.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_HotPts.ptLC.X = (float)m_rcHot.left;
	m_HotPts.ptLC.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
	m_HotPts.ptRC.X = (float)m_rcHot.right;
	m_HotPts.ptRC.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
	m_HotPts.ptBC.Y = (float)m_rcHot.bottom;
	m_HotPts.ptBC.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_HotPts.ptRotate.X = m_HotPts.ptTC.X;		//Ðý×ªµãÎ»ÖÃ
	m_HotPts.ptRotate.Y = m_HotPts.ptTC.Y - 3.0f * HOTINTERVAL;
}

void CGraph::DrawEightpoints(Graphics& graph)
{
	Pen penDraw(Color::CadetBlue,3);
	graph.DrawRectangle(&penDraw,m_HotPts.ptBC.X - 5,m_HotPts.ptBC.Y - 5,10.0,10.0);
	graph.DrawRectangle(&penDraw,m_HotPts.ptLB.X - 5,m_HotPts.ptLB.Y - 5,10.0,10.0);
	graph.DrawRectangle(&penDraw,m_HotPts.ptLC.X - 5,m_HotPts.ptLC.Y - 5,10.0,10.0);
	graph.DrawRectangle(&penDraw,m_HotPts.ptLT.X - 5,m_HotPts.ptLT.Y - 5,10.0,10.0);
	graph.DrawRectangle(&penDraw,m_HotPts.ptRB.X - 5,m_HotPts.ptRB.Y - 5,10.0,10.0);
	graph.DrawRectangle(&penDraw,m_HotPts.ptRC.X - 5,m_HotPts.ptRC.Y - 5,10.0,10.0);
	graph.DrawRectangle(&penDraw,m_HotPts.ptRT.X - 5,m_HotPts.ptRT.Y - 5,10.0,10.0);
	graph.DrawRectangle(&penDraw,m_HotPts.ptTC.X - 5,m_HotPts.ptTC.Y - 5,10.0,10.0);
}

ModifyType CGraph::AroundHotPts(CPoint point)
{
	BOOL bret = TRUE;
	PointF pt((float)point.x,(float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle,m_ptCenter);
	mat.TransformPoints(&pt,1);													

	if(CalGapOfpoints(pt,m_HotPts.ptLT) <= HOTINTERVAL)
	{
		return MT_LT;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptTC) <= HOTINTERVAL)
	{
		return MT_TC;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRT) <= HOTINTERVAL)
	{
		return MT_RT;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptLC) <= HOTINTERVAL)
	{
		return MT_LC;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRC) <= HOTINTERVAL)
	{
		return MT_RC;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptLB) <= HOTINTERVAL)
	{
		return MT_LB;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptBC) <= HOTINTERVAL)
	{
		return MT_BC;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRB) <= HOTINTERVAL)
	{
		return MT_RB;
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRotate) <= MININTERVAL)
	{
		return MT_ROTATE;
	}
	else if(m_rcHot.PtInRect(CPoint((int)pt.X,(int)pt.Y)))
	{
		return MT_MOVE;
	}
	else
	{
		return MT_NONE;
	}
}


BOOL CGraph::bPtInModifyZone(CPoint point)
{
	BOOL bret = TRUE;
	PointF pt((float)point.x,(float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle,m_ptCenter);
	mat.TransformPoints(&pt,1);

	if(CalGapOfpoints(pt,m_HotPts.ptLT) <= HOTINTERVAL)
	{
		
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptTC) <= HOTINTERVAL)
	{
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRT) <= HOTINTERVAL)
	{
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptLC) <= HOTINTERVAL)
	{
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRC) <= HOTINTERVAL)
	{
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptLB) <= HOTINTERVAL)
	{
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptBC) <= HOTINTERVAL)
	{
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRB) <= HOTINTERVAL)
	{
	}
	else if(CalGapOfpoints(pt,m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
	}
	else if(m_rcHot.PtInRect(CPoint((int)pt.X,(int)pt.Y)))
	{
	}
	else
	{
		bret = FALSE;
	}
	return bret;
}

void CGraph::RotateGraph(CPoint point)
{

	float dx = point.x - m_ptCenter.X;
	float dy = point.y - m_ptCenter.Y;
	float b= 0.0f;
	float alpha= 0.0f;
	float PI = 3.1415926f;
	if ((dx>0)&(dy>0))
	{
		b= 90;
		alpha= (float)abs((atan((double)(dy/dx)) * (180/PI)));
	}
	else if ((dx<=0)&(dy>=0))
	{
		b= 180;
		if (dy > 0)
		{
			alpha = (float)abs((atan((double)(dx / dy)) * (180 /PI)));
		}
		else if (dy == 0)
		{
			b = 270;
		}
	}
	else if ((dx<0)&(dy<0))
	{
		b= 270;
		alpha= (float)abs((atan((double)(dy/dx)) * (180/ PI)));
	}
	else
	{
		b= 0;
		alpha= (float)abs((atan((double)(dx/dy)) * (180/PI)));
	}
	m_fAngle = (b+ alpha);
}

void CGraph::InputGraphData(CPoint* pPt,int nCount)
{
	if(nCount < m_nMinCount)
	{
		AfxMessageBox(IDS_DATA_LESS);
		return;
	}
	m_ptary.RemoveAll();
	m_nPtCount = nCount;
	PointF	pt;
	for(int i = 0; i < m_nPtCount; i++)
	{
		pt.X = (float)pPt[i].x;
		pt.Y = (float)pPt[i].y;
		m_ptary.Add(pt);
	}
	m_bFinish = TRUE;
	InitHotData();
}

void CGraph::RotateGraph(float nAngle)
{
	if(!m_bFinish)
	{
		AfxMessageBox(IDS_NOT_DRAW_FINISHED);
		return;
	}
	m_fAngle += nAngle;
}

void CGraph::StretchGraph(float xStret, float yStret)
{
	if(!m_bFinish)
	{
		AfxMessageBox(IDS_NOT_DRAW_FINISHED);
		return;
	}

	if(xStret < 0 || yStret < 0)
	{
		AfxMessageBox(IDS_NEGATIVE_ERRO);
		return;
	}
	PointF* pPt = m_ptary.GetData();
	for(int i = 0;i < m_nPtCount;i++)
	{
		pPt[i].X = m_ptCenter.X + (pPt[i].X - m_ptCenter.X) * xStret;
		pPt[i].Y = m_ptCenter.Y + (pPt[i].Y - m_ptCenter.Y) * yStret;
	}
	InitHotData();
}

void CGraph::MoveGraph(float offsetx,float offsety)
{
	if(!m_bFinish)
	{
		AfxMessageBox(IDS_NOT_DRAW_FINISHED);
		return;
	}
	PointF* pPt = m_ptary.GetData();
	for(int i = 0;i < m_nPtCount;i++)
	{
		pPt[i].X += offsetx;
		pPt[i].Y += offsety;
	}
	m_rcHot.OffsetRect((int)offsetx,(int)offsety);
	SetHotPoints();

	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
}


HRGN CGraph::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)
{
	HRGN hRgn = NULL;

	if (hBmp)
	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			// Get bitmap size
			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);

			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {
				sizeof(BITMAPINFOHEADER),	// biSize 
				bm.bmWidth,					// biWidth; 
				bm.bmHeight,				// biHeight; 
				1,							// biPlanes; 
				32,							// biBitCount 
				BI_RGB,						// biCompression; 
				0,							// biSizeImage; 
				0,							// biXPelsPerMeter; 
				0,							// biYPelsPerMeter; 
				0,							// biClrUsed; 
				0							// biClrImportant; 
			};
			VOID * pbits32; 
			HBITMAP hbm32 = CreateDIBSection(hMemDC,
				(BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}
				DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}
	}

	return hRgn;

}
