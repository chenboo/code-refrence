#pragma once

#include "ElcSkinDoc.h"


////////// CElcMemDC //////////
// 
class LIBELCBASEUI_API CElcMemDC : public CDC
{
public:
	CElcMemDC() : CDC() {
		m_bitmap = NULL;
		m_oldBitmap = NULL;
		m_pDC = NULL;
		m_rect.SetRectEmpty();
	}
	CElcMemDC(CDC* pDC, const CRect& rcBounds) : CDC() {
		CreateCompatibleDC(pDC);
		m_bitmap = new CBitmap;
		m_bitmap->CreateCompatibleBitmap(pDC, rcBounds.right, rcBounds.bottom);
		m_oldBitmap = SelectObject(m_bitmap);
		m_pDC = pDC;
		m_rect = rcBounds;
		SetBkMode(TRANSPARENT);
	}
	~CElcMemDC() {
		if (GetSafeHdc()) {
			SelectObject(m_oldBitmap);
			if (m_bitmap != NULL) 
				delete m_bitmap;
		}
	}

	BOOL CreateMe(CDC* pDC, const CRect& rcBounds) {
		DeleteDC();
		if (!CreateCompatibleDC(pDC))
			return FALSE;
		if (m_bitmap) {
			m_bitmap->DeleteObject();
			delete m_bitmap;
			m_bitmap = NULL;
		}
		m_bitmap = new CBitmap;
		m_bitmap->CreateCompatibleBitmap(pDC, rcBounds.right, rcBounds.bottom);
		m_oldBitmap = SelectObject(m_bitmap);
		m_pDC = pDC;
		m_rect = rcBounds;
		SetBkMode(TRANSPARENT);
		return TRUE;
	}

	BOOL DrawMe()
	{
		if (m_pDC && m_pDC->GetSafeHdc())
		{
			return m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), 
				this, m_rect.left, m_rect.top, SRCCOPY);
		}
		else
		{
			return FALSE;
		}
	}

protected:
	CBitmap*	m_bitmap;
	CBitmap*	m_oldBitmap;
	CDC*		m_pDC;
	CRect		m_rect;
};

////////// CElcMemGraphics //////////
//
class LIBELCBASEUI_API CElcMemGraphics
{
public:
	CElcMemGraphics(const CRect& rcBounds)
	{
		m_pBitmap = new Bitmap(rcBounds.right, rcBounds.top);
		m_pGrph = new Graphics(m_pBitmap);
		m_rect = rcBounds;
	}
	~CElcMemGraphics()
	{
		if (m_pGrph)
			delete m_pGrph;
		if (m_pBitmap)
			delete m_pBitmap;
	}

	Graphics * GetGraphics() {
		return m_pGrph;
	}
	Bitmap * GetBitmap() {
		return m_pBitmap;
	}

	void DrawMe(CDC* pDC) 
	{
		if (pDC) {
			Graphics gOut(pDC->GetSafeHdc());
			gOut.DrawImage(m_pBitmap, m_rect.left, m_rect.top);
		}
	}

protected:
	Bitmap *			m_pBitmap;
	Graphics *			m_pGrph;
	CRect				m_rect;
};

////////// CElcSkinPainter //////////
// 
class LIBELCBASEUI_API CElcSkinPainter
{
public:
	CElcSkinPainter(void);
	~CElcSkinPainter(void);

public:
	void DrawImageGDI(
		HDC hDC,
		PELC_SKIN_IMAGE pSkin,
		RECT rcDest,
		int nImageIndex,
		COLORREF crBackground,
		BOOL bFillCenter,
		BOOL bUseBkColor,
		PVOID pvImageAttributes=NULL);

	void DrawImageGDI(
		HDC hDC,
		Image* pImage,
		RECT rcDest,
		PVOID pvImageAttributes=NULL);

	void DrawImageGDI(
		HDC hDC,
		Image* pImage,
		RECT rcDest,
		RECT rcSrc,
		PVOID pvImageAttributes=NULL);

	void DrawImage(
		PVOID pGraphics, 
		PELC_SKIN_IMAGE pSkin, 
		PVOID pRectDest, 
		int nImageIndex=0, 
		BOOL bFillCenter=TRUE, 
		PVOID pvImageAttributes=NULL);

	void DrawImage(
		PVOID pGraphics, 
		PELC_SKIN_IMAGE pSkin, 
		PVOID pRectDest, 
		COLORREF crBackground,
		int nImageIndex, 
		BOOL bFillCenter, 
		PVOID pvImageAttributes);

	void DrawStretchImage(
		PVOID pGraphics, 
		PVOID pImage, 
		PVOID pRectDest, 
		PVOID pRectSrc, 
		PVOID pvImageAttributes=NULL);

	void DrawFixedImage(
		PVOID pGraphics, 
		PVOID pImage, 
		CPoint point, 
		PVOID pRectSrc, 
		PVOID pvImageAttributes=NULL);

	void DrawFixedImage(
		PVOID pGraphics, 
		PVOID pImage,
		int nIndexImage,
		int nCount,
		BOOL bVertical,
		CPoint ptDest,
		PVOID pvImageAttributes=NULL);

	static void SplitImage(
		LPVOID &lpvDestImage, 
		PVOID pSrcImage, 
		int nIndex, 
		int nCount, 
		BOOL bVertical=TRUE);
};


////////// CElcImagePainter //////////
//

#define ELC_ID_IMAGE_ITEM_FIRST		100

enum ELC_IMAGEPAINT_ALIGN {
	ELCIPA_NONE = 0,
	ELCIPA_AUTO_CENTER,
};

class LIBELCBASEUI_API CElcImagePaintMgr
{
public:
	CElcImagePaintMgr(void);
	~CElcImagePaintMgr(void);

public:
	// load a background image
	BOOL LoadSkin(LPCTSTR lpszStyle);

	BOOL AddImage(UINT nId, LPCTSTR lpszStyle);
	BOOL AddImage(UINT nId, UINT nImageId, LPCTSTR lpszType=RT_PNG);

	void DelImage(UINT nId);
	void DelAllImages();

	void SetImagePos(UINT nId, const RECT& rcDest);
	void SetImagePos(UINT nId, const POINT& ptRest);
	void SetImageAutoCenter(UINT nId);

	CSize GetImageSrcSize(UINT nId);
	CRect GetImageDestRect(UINT nId);

	void Draw(CDC* pDC, CRect rcDest);

private:
	int FindItem(UINT nId);
	void Release();
	
private:
	typedef struct ELC_MGR_IMAGE_ITEM {
		UINT	nId;
		int		align;
		Image*	pImage;
		BOOL	bResId; // specified the image come from Resource dll or CElcSkinDoc
		Rect	rcSrc;
		Rect	rcDest;
	} *PELC_MGR_IMAGE_ITEM;

	typedef std::vector<ELC_MGR_IMAGE_ITEM>		CElcImageArray;
	CElcImageArray								m_arrItem;

	ELC_MGR_IMAGE_ITEM							m_background;

	CElcSkinPainter								m_painter;
};