#include "StdAfx.h"
#include "LibElcBaseUI.h"
#include "ElcSkinPainter.h"

CElcSkinPainter::CElcSkinPainter(void)
{
}

CElcSkinPainter::~CElcSkinPainter(void)
{
}

void CElcSkinPainter::DrawImageGDI(HDC hDC,
								   Image* pImage,
								   RECT rcDest,
								   PVOID pvImageAttributes)
{
	if (!hDC || !pImage)
		return;

	Rect rectSrc(0,0,pImage->GetWidth(), pImage->GetHeight());
	Rect rectDest(0, 0, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top);
	Bitmap bitmap(rectDest.Width, rectDest.Height);
	Graphics grph(&bitmap);

	DrawStretchImage(&grph, pImage, &rectDest, &rectSrc, pvImageAttributes);

	Graphics gOut(hDC);
	gOut.DrawImage(&bitmap, rcDest.left, rcDest.top);
}

void CElcSkinPainter::DrawImageGDI(HDC hDC,
								PELC_SKIN_IMAGE pSkin,
								RECT rcDest,
								int nImageIndex,
								COLORREF crBackground,
								BOOL bFillCenter,
								BOOL bUseBkColor,
								PVOID pvImageAttributes)
{
	if (!hDC || !pSkin || !pSkin->pImage)
		return;

	Rect rect(0, 0, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top);
	Bitmap bitmap(rect.Width, rect.Height);
	Graphics grph(&bitmap);

	if (bUseBkColor) {
		DrawImage(&grph, pSkin, &rect, crBackground, nImageIndex, bFillCenter, pvImageAttributes);
	}
	else {
		DrawImage(&grph, pSkin, &rect, nImageIndex, bFillCenter, pvImageAttributes);
	}

	Graphics gOut(hDC);
	gOut.DrawImage(&bitmap, rcDest.left, rcDest.top);
}

void CElcSkinPainter::DrawImageGDI(
		HDC hDC,
		Image* pImage,
		RECT rcDest,
		RECT rcSrc,
		PVOID pvImageAttributes)
{
	if (!hDC || !pImage)
		return;

	Rect rectSrc(rcSrc.left, rcSrc.top, rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top);
	Rect rectDest(0, 0, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top);
	Bitmap bitmap(rectDest.Width, rectDest.Height);
	Graphics grph(&bitmap);

	DrawStretchImage(&grph, pImage, &rectDest, &rectSrc, pvImageAttributes);

	Graphics gOut(hDC);
	gOut.DrawImage(&bitmap, rcDest.left, rcDest.top);
}

void CElcSkinPainter::DrawImage(PVOID pGraphics, 
								PELC_SKIN_IMAGE pSkin, 
								PVOID pRectDest, 
								int nImageIndex,
								BOOL bFillCenter, 
								PVOID pvImageAttributes)
{
	if (!pGraphics || !pSkin || !pSkin->pImage)
		return ;

	Graphics * pGrph = (Graphics *)pGraphics;
	Rect rcDest = *((Rect*)pRectDest);
	ImageAttributes* pImgAttr = (ImageAttributes*)pvImageAttributes;

	BOOL bImageSplitted = FALSE;
	Image * pImage = (Image*)pSkin->pImage;
	if (pSkin->nCount > 1) {
		if (nImageIndex < 0 || nImageIndex >= pSkin->nCount) {
			ASSERT(0);
			nImageIndex = 0;
		}

		LPVOID lpvImage = NULL;
		SplitImage(lpvImage, pSkin->pImage, nImageIndex, pSkin->nCount, pSkin->bVertical);
		if (!lpvImage)
			return;
		pImage = (Image *)lpvImage;
		bImageSplitted = TRUE;
	}

	int cx = pImage->GetWidth();
	int cy = pImage->GetHeight();

	Rect rect, rc;

	// left top
	rect.X = rect.Y = 0;
	if (pSkin->stretch & EISR_HORIZONTAL)
		rect.Width = pSkin->nLeftSplit;
	else
		rect.Width = cx;
	if (pSkin->stretch & EISR_VERTICAL)
		rect.Height = pSkin->nTopSplit;
	else
		rect.Height = cy;
	DrawFixedImage(pGrph, pImage, CPoint(rcDest.X, rcDest.Y), &rect, pImgAttr);

	if (pSkin->stretch & EISR_HORIZONTAL) {
		// top center
		rect.X = pSkin->nLeftSplit;
		rect.Width = pSkin->nRightSplit - pSkin->nLeftSplit;
		rc.X = rcDest.X+rect.X;
		rc.Y = rcDest.Y;
		rc.Width = rcDest.Width - (cx - (pSkin->nRightSplit - pSkin->nLeftSplit));
		rc.Height = rect.Height;
		DrawStretchImage(pGrph, pImage, &rc, &rect, pImgAttr);

		// right top
		rect.X = pSkin->nRightSplit;
		rect.Width = cx - pSkin->nRightSplit;
		DrawFixedImage(pGrph, pImage, CPoint(rcDest.X + rcDest.Width-rect.Width, rcDest.Y), &rect, pImgAttr);
	}

	rect.X = rect.Y = 0;
	if (pSkin->stretch & EISR_HORIZONTAL)
		rect.Width = pSkin->nLeftSplit;
	else
		rect.Width = cx;
	if (pSkin->stretch & EISR_VERTICAL) {
		// left center
		rect.Y = pSkin->nTopSplit;
		rect.Height = pSkin->nBottomSplit - pSkin->nTopSplit;
		rc.X = rcDest.X;
		rc.Y = rcDest.Y + rect.Y;
		rc.Width = rect.Width;
		rc.Height = rcDest.Height - (cy - (pSkin->nBottomSplit - pSkin->nTopSplit));
		DrawStretchImage(pGrph, pImage, &rc, &rect, pImgAttr);

		// left bottom 
		rect.Y = pSkin->nBottomSplit;
		rect.Height = cy - pSkin->nBottomSplit;
		DrawFixedImage(pGrph, pImage, CPoint(rcDest.X, rcDest.Y+rcDest.Height-rect.Height), &rect, pImgAttr);
	}

	if (pSkin->stretch == EISR_BOTH) {
		// right center
		rect.X = pSkin->nRightSplit;
		rect.Y = pSkin->nTopSplit;
		rect.Width = cx - pSkin->nRightSplit;
		rect.Height = pSkin->nBottomSplit - pSkin->nTopSplit;
		rc.X = rcDest.X + rcDest.Width - rect.Width;
		rc.Y = rcDest.Y + rect.Y;
		rc.Width = rect.Width;
		rc.Height = rcDest.Height - (cy - (pSkin->nBottomSplit - pSkin->nTopSplit));
		DrawStretchImage(pGrph, pImage, &rc, &rect, pImgAttr);

		// bottom center
		rect.X = pSkin->nLeftSplit;
		rect.Y = pSkin->nBottomSplit;
		rect.Width = pSkin->nRightSplit - pSkin->nLeftSplit;
		rect.Height = cy - pSkin->nBottomSplit;
		rc.X = rcDest.X + rect.X;
		rc.Y = rcDest.Y + rcDest.Height - rect.Height;
		rc.Width = rcDest.Width - (cx - (pSkin->nRightSplit - pSkin->nLeftSplit));
		rc.Height = rect.Height;
		DrawStretchImage(pGrph, pImage, &rc, &rect, pImgAttr);

		// right bottom
		rect.X = pSkin->nRightSplit;
		rect.Y = pSkin->nBottomSplit;
		rect.Width = cx - pSkin->nRightSplit;
		rect.Height = cy - pSkin->nBottomSplit;
		rc.X = rcDest.X + rcDest.Width - rect.Width;
		rc.Y = rcDest.Y + rcDest.Height - rect.Height;
		DrawFixedImage(pGrph, pImage, CPoint(rc.X, rc.Y), &rect, pImgAttr);

		if (bFillCenter) {
			rect.X = pSkin->nLeftSplit;
			rect.Y = pSkin->nTopSplit;
			rect.Width = pSkin->nRightSplit - pSkin->nLeftSplit;
			rect.Height = pSkin->nBottomSplit - pSkin->nTopSplit;
			rc.X = rcDest.X + rect.X;
			rc.Y = rcDest.Y + rect.Y;
			rc.Width = rcDest.Width - (cx - rect.Width);
			rc.Height = rcDest.Height - (cy - rect.Height);
			DrawStretchImage(pGrph, pImage, &rc, &rect, pImgAttr);
		}
	}

	if (bImageSplitted)
		delete (Bitmap *)pImage;
}

void CElcSkinPainter::DrawImage(PVOID pGraphics, 
								PELC_SKIN_IMAGE pSkin, 
								PVOID pRectDest, 
								COLORREF crBackground,
								int nImageIndex, 
								BOOL bFillCenter, 
								PVOID pvImageAttributes)
{
	if (!pGraphics || !pSkin || !pSkin->pImage)
		return ;

	Graphics * pGrph = (Graphics *)pGraphics;
	Rect rcDest = *((Rect*)pRectDest);
	ImageAttributes* pImgAttr = (ImageAttributes*)pvImageAttributes;

	Color crBk;
	crBk.SetFromCOLORREF(crBackground);
	SolidBrush brBackgnd(crBk);

	BOOL bImageSplitted = FALSE;
	Image * pImage = (Image*)pSkin->pImage;
	if (pSkin->nCount > 1) {
		if (nImageIndex < 0 || nImageIndex >= pSkin->nCount) {
			ASSERT(0);
			nImageIndex = 0;
		}

		LPVOID lpvImage = NULL;
		SplitImage(lpvImage, pSkin->pImage, nImageIndex, pSkin->nCount, pSkin->bVertical);
		if (!lpvImage)
			return;
		pImage = (Image *)lpvImage;
		bImageSplitted = TRUE;
	}

	int cx = pImage->GetWidth();
	int cy = pImage->GetHeight();

	Rect rect, rc;

	// left top
	rect.X = rect.Y = 0;
	if (pSkin->stretch & EISR_HORIZONTAL)
		rect.Width = pSkin->nLeftSplit;
	else
		rect.Width = cx;
	if (pSkin->stretch & EISR_VERTICAL)
		rect.Height = pSkin->nTopSplit;
	else
		rect.Height = cy;
	rc = rect;
	rc.X = rcDest.X;
	rc.Y = rcDest.Y;
	pGrph->FillRectangle(&brBackgnd, rc);
	DrawFixedImage(pGrph, pImage, CPoint(rcDest.X, rcDest.Y), &rect, pImgAttr);

	if (pSkin->stretch & EISR_HORIZONTAL) {
		// top center
		rect.X = pSkin->nLeftSplit;
		rect.Width = pSkin->nRightSplit - pSkin->nLeftSplit;
		rc.X = rcDest.X+rect.X;
		rc.Y = rcDest.Y;
		rc.Width = rcDest.Width - (cx - (pSkin->nRightSplit - pSkin->nLeftSplit));
		rc.Height = rect.Height;
		pGrph->FillRectangle(&brBackgnd, rc);
		DrawStretchImage(pGrph, pImage, &rc, &rect, pImgAttr);

		// right top
		rect.X = pSkin->nRightSplit;
		rect.Width = cx - pSkin->nRightSplit;
		rc = rect;
		rc.X = rcDest.Width-rect.Width;
		rc.Y = rcDest.Y;
		pGrph->FillRectangle(&brBackgnd, rc);
		DrawFixedImage(pGrph, pImage, CPoint(rcDest.Width-rect.Width, rcDest.Y), &rect, pImgAttr);
	}

	rect.X = rect.Y = 0;
	if (pSkin->stretch & EISR_HORIZONTAL)
		rect.Width = pSkin->nLeftSplit;
	else
		rect.Width = cx;
	if (pSkin->stretch & EISR_VERTICAL) {
		// left center
		rect.Y = pSkin->nTopSplit;
		rect.Height = pSkin->nBottomSplit - pSkin->nTopSplit;
		rc.X = rcDest.X;
		rc.Y = rcDest.Y + rect.Y;
		rc.Width = rect.Width;
		rc.Height = rcDest.Height - (cy - (pSkin->nBottomSplit - pSkin->nTopSplit));
		pGrph->FillRectangle(&brBackgnd, rc);
		DrawStretchImage(pGrph, pImage, &rc, &rect, pImgAttr);

		// left bottom 
		rect.Y = pSkin->nBottomSplit;
		rect.Height = cy - pSkin->nBottomSplit;
		rc = rect;
		rc.X = rcDest.X;
		rc.Y = rcDest.Height-rect.Height;
		pGrph->FillRectangle(&brBackgnd, rc);
		DrawFixedImage(pGrph, pImage, CPoint(rcDest.X, rcDest.Height-rect.Height), &rect, pImgAttr);
	}

	if (pSkin->stretch == EISR_BOTH) {
		// right center
		rect.X = pSkin->nRightSplit;
		rect.Y = pSkin->nTopSplit;
		rect.Width = cx - pSkin->nRightSplit;
		rect.Height = pSkin->nBottomSplit - pSkin->nTopSplit;
		rc.X = rcDest.X + rcDest.Width - rect.Width;
		rc.Y = rcDest.Y + rect.Y;
		rc.Width = rect.Width;
		rc.Height = rcDest.Height - (cy - (pSkin->nBottomSplit - pSkin->nTopSplit));
		pGrph->FillRectangle(&brBackgnd, rc);
		DrawStretchImage(pGrph, pImage, &rc, &rect, pImgAttr);

		// bottom center
		rect.X = pSkin->nLeftSplit;
		rect.Y = pSkin->nBottomSplit;
		rect.Width = pSkin->nRightSplit - pSkin->nLeftSplit;
		rect.Height = cy - pSkin->nBottomSplit;
		rc.X = rcDest.X + rect.X;
		rc.Y = rcDest.Y + rcDest.Height - rect.Height;
		rc.Width = rcDest.Width - (cx - (pSkin->nRightSplit - pSkin->nLeftSplit));
		rc.Height = rect.Height;
		pGrph->FillRectangle(&brBackgnd, rc);
		DrawStretchImage(pGrph, pImage, &rc, &rect, pImgAttr);

		// right bottom
		rect.X = pSkin->nRightSplit;
		rect.Y = pSkin->nBottomSplit;
		rect.Width = cx - pSkin->nRightSplit;
		rect.Height = cy - pSkin->nBottomSplit;
		rc = rect;
		rc.X = rcDest.X + rcDest.Width - rect.Width;
		rc.Y = rcDest.Y + rcDest.Height - rect.Height;
		pGrph->FillRectangle(&brBackgnd, rc);
		DrawFixedImage(pGrph, pImage, CPoint(rc.X, rc.Y), &rect, pImgAttr);

		if (bFillCenter) {
			rect.X = pSkin->nLeftSplit;
			rect.Y = pSkin->nTopSplit;
			rect.Width = pSkin->nRightSplit - pSkin->nLeftSplit;
			rect.Height = pSkin->nBottomSplit - pSkin->nTopSplit;
			rc.X = rcDest.X + rect.X;
			rc.Y = rcDest.Y + rect.Y;
			rc.Width = rcDest.Width - (cx - rect.Width);
			rc.Height = rcDest.Height - (cy - rect.Height);
			pGrph->FillRectangle(&brBackgnd, rc);
			DrawStretchImage(pGrph, pImage, &rc, &rect, pImgAttr);
		}
	}

	if (bImageSplitted)
		delete (Bitmap *)pImage;
}

void CElcSkinPainter::DrawStretchImage(PVOID pGraphics, PVOID pImage, PVOID pRectDest, PVOID pRectSrc, PVOID pvImageAttributes)
{
	Graphics * pGrph = (Graphics *)pGraphics;
	Rect rcDest = *((Rect*)pRectDest);
	Rect rcSrc = *((Rect*)pRectSrc);
	ImageAttributes* pImgAttr = (ImageAttributes*)pvImageAttributes;

	if (!pGrph || !pImage)
		return;

	pGrph->DrawImage((Image*)pImage,
		rcDest, 
		rcSrc.X,
		rcSrc.Y,
		rcSrc.Width,
		rcSrc.Height,
		UnitPixel,
		pImgAttr);
}

void CElcSkinPainter::DrawFixedImage(PVOID pGraphics, PVOID pImage, CPoint point, PVOID pRectSrc, PVOID pvImageAttributes)
{
	Graphics * pGrph = (Graphics *)pGraphics;
	ImageAttributes* pImgAttr = (ImageAttributes*)pvImageAttributes;

	Rect rcSrc;
	if (pRectSrc)
		rcSrc = *((Rect*)pRectSrc);
	else {
		rcSrc.X = 0;
		rcSrc.Y = 0;
		rcSrc.Width = ((Image*)pImage)->GetWidth();
		rcSrc.Height = ((Image*)pImage)->GetHeight();
	}

	if (!pGrph || !pImage)
		return;

	Rect rcDest(point.x, point.y, rcSrc.Width, rcSrc.Height);

	pGrph->DrawImage((Image*)pImage,
		rcDest, 
		rcSrc.X,
		rcSrc.Y,
		rcSrc.Width,
		rcSrc.Height,
		UnitPixel,
		pImgAttr);
}

void CElcSkinPainter::DrawFixedImage(PVOID pGraphics, 
									 PVOID pImage,
									 int nIndexImage,
									 int nCount,
									 BOOL bVertical,
									 CPoint ptDest,
									 PVOID pvImageAttributes)
{
	if (!pGraphics || !pImage)
		return;

	Graphics * pGrph = (Graphics *)pGraphics;
	ImageAttributes* pImgAttr = (ImageAttributes*)pvImageAttributes;

	if (nIndexImage < 0 || nIndexImage >= nCount) {
		ASSERT(0);
		nIndexImage = 0;
	}

	int x=0, y=0;
	int cx = ((Image *)pImage)->GetWidth();
	int cy = ((Image *)pImage)->GetHeight();

	if (bVertical) {
		cy = cy / nCount;
		y = cy * nIndexImage;
	}
	else {
		cx = cx / nCount;
		x = cx * nIndexImage;
	}

	Rect rcDest(ptDest.x, ptDest.y, cx, cy);

	pGrph->DrawImage((Image*)pImage,
		rcDest, 
		x,
		y,
		cx,
		cy,
		UnitPixel,
		pImgAttr);
}

void CElcSkinPainter::SplitImage(LPVOID &lpvDestImage, PVOID pSrcImage, int nIndex, int nCount, BOOL bVertical)
{
	Image* pImage = (Image*)pSrcImage;

	int x=0, y=0;
	int cx = pImage->GetWidth();
	int cy = pImage->GetHeight();
	

	if (bVertical) {
		cy = cy / nCount;
		y = cy * nIndex;
	}
	else {
		cx = cx / nCount;
		x = cx * nIndex;
	}

	Rect rcDest(0, 0, cx, cy);
	Bitmap * pDestBitmap = new Bitmap(cx, cy);
	Graphics g(pDestBitmap);
	g.DrawImage(pImage,
		rcDest, 
		x,
		y,
		cx,
		cy,
		UnitPixel);

	lpvDestImage = pDestBitmap;
}



//////////////////////////////////////////////
// CElcImagePaintMgr

CElcImagePaintMgr::CElcImagePaintMgr(void)
{
	memset(&m_background, 0, sizeof(m_background));
}

CElcImagePaintMgr::~CElcImagePaintMgr(void)
{
	DelAllImages();
}

int CElcImagePaintMgr::FindItem(UINT nId)
{
	for (size_t i = 0; i < m_arrItem.size(); i++) {
		if (m_arrItem[i].nId == nId)
			return (int)i;
	}

	return -1;
}

BOOL CElcImagePaintMgr::LoadSkin(LPCTSTR lpszStyle)
{
	PELC_SKIN_PICTURE pSkin = theSkin.GetPicture(lpszStyle);
	if (!pSkin) {
		ASSERT(0);
		return FALSE;
	}

	CSize size = CElcSkinDoc::GetImageSize(&pSkin->image);

	m_background.nId = 0x7FFFFFFF;
	m_background.pImage = (Image *)pSkin->image.pImage;
	m_background.rcSrc.X = 0;
	m_background.rcSrc.Y = 0;
	m_background.rcSrc.Width = size.cx;
	m_background.rcSrc.Height = size.cy;

	return TRUE;
}

BOOL CElcImagePaintMgr::AddImage(UINT nId, LPCTSTR lpszStyle)
{
	PELC_SKIN_PICTURE pSkin = theSkin.GetPicture(lpszStyle);
	if (!pSkin)
		return FALSE;

	if (FindItem(nId) != -1)
		return FALSE;

	ELC_MGR_IMAGE_ITEM item = {0};
	item.nId = nId;
	item.bResId = FALSE;
	item.align = ELCIPA_NONE;
	item.pImage = (Image *)pSkin->image.pImage;
	item.rcSrc.X = 0;
	item.rcSrc.Y = 0;
	item.rcSrc.Width = item.pImage->GetWidth();
	item.rcSrc.Height = item.pImage->GetHeight();

	item.rcDest = item.rcSrc;

	m_arrItem.push_back(item);

	return TRUE;
}

BOOL CElcImagePaintMgr::AddImage(UINT nId, UINT nImageId, LPCTSTR lpszType)
{
	if (FindItem(nId) != -1)
		return FALSE;

	ELC_MGR_IMAGE_ITEM item = {0};
	item.nId = nId;
	item.bResId = TRUE;
	item.align = ELCIPA_NONE;

	if (!ImageFromIDResource(nImageId, lpszType, item.pImage)) {
		if (item.pImage)
			delete item.pImage;
		return FALSE;
	}

	item.rcSrc.X = 0;
	item.rcSrc.Y = 0;
	item.rcSrc.Width = item.pImage->GetWidth();
	item.rcSrc.Height = item.pImage->GetHeight();

	item.rcDest = item.rcSrc;

	m_arrItem.push_back(item);

	return TRUE;
}

void CElcImagePaintMgr::DelImage(UINT nId)
{
	int nItem = FindItem(nId);
	if (nItem != -1) {
		if (m_arrItem[nItem].pImage && m_arrItem[nItem].bResId)
			delete m_arrItem[nItem].pImage;
		m_arrItem.erase(m_arrItem.begin() + nItem);
	}
}

void CElcImagePaintMgr::DelAllImages()
{
	for (CElcImagePaintMgr::CElcImageArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (it->pImage && it->bResId)
			delete it->pImage;
	}

	m_arrItem.clear();
}

void CElcImagePaintMgr::SetImagePos(UINT nId, const RECT& rcDest)
{
	int nItem = FindItem(nId);
	if (nItem == -1)
		return;

	m_arrItem[nItem].rcDest.X = rcDest.left;
	m_arrItem[nItem].rcDest.Y = rcDest.top;
	m_arrItem[nItem].rcDest.Width = rcDest.right - rcDest.left;
	m_arrItem[nItem].rcDest.Height = rcDest.bottom - rcDest.top;
}

void CElcImagePaintMgr::SetImagePos(UINT nId, const POINT& ptRest)
{
	int nItem = FindItem(nId);
	if (nItem == -1)
		return;

	m_arrItem[nItem].rcDest.X = ptRest.x;
	m_arrItem[nItem].rcDest.Y = ptRest.y;
}

void CElcImagePaintMgr::SetImageAutoCenter(UINT nId)
{
	int nItem = FindItem(nId);
	if (nItem == -1)
		return;

	m_arrItem[nItem].align = ELCIPA_AUTO_CENTER;
}

CSize CElcImagePaintMgr::GetImageSrcSize(UINT nId)
{
	CSize size(0,0);
	int nItem = FindItem(nId);
	if (nItem == -1)
		return size;

	size.cx = m_arrItem[nItem].rcSrc.Width;
	size.cy = m_arrItem[nItem].rcSrc.Height;
	return size;
}

CRect CElcImagePaintMgr::GetImageDestRect(UINT nId)
{
	CRect rect(0,0,0,0);
	int nItem = FindItem(nId);
	if (nItem == -1)
		return rect;

	rect.left = m_arrItem[nItem].rcDest.X;
	rect.right = rect.left + m_arrItem[nItem].rcDest.Width;
	rect.top = m_arrItem[nItem].rcDest.Y;
	rect.bottom = rect.top + m_arrItem[nItem].rcDest.Height;

	return rect;
}

void CElcImagePaintMgr::Draw(CDC* pDC, CRect rcDest)
{
	Bitmap bitmap(rcDest.Width(), rcDest.Height());
	Graphics gMem(&bitmap);

	m_background.rcDest.X = rcDest.left;
	m_background.rcDest.Y = rcDest.top;
	m_background.rcDest.Width = rcDest.Width();
	m_background.rcDest.Height = rcDest.Height();

	m_painter.DrawStretchImage(&gMem,
		m_background.pImage,
		&m_background.rcDest,
		&m_background.rcSrc);

	for (CElcImagePaintMgr::CElcImageArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		// auto center
		if (it->align == ELCIPA_AUTO_CENTER) {
			it->rcDest.X = (rcDest.Width() - it->rcSrc.Width) / 2;
			it->rcDest.Y = (rcDest.Height() - it->rcSrc.Height) / 2;
		}

		m_painter.DrawStretchImage(&gMem,
			it->pImage,
			&it->rcDest,
			&it->rcSrc);
	}

	Graphics gOut(pDC->GetSafeHdc());
	gOut.DrawImage(&bitmap, rcDest.left, rcDest.top);
}
