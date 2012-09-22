#pragma once
#include "ElcUICommon.h"

static ColorMatrix g_GrayColorMatrix = {
	0.3f,  0.3f,  0.3f,  0.0f, 0.0f,
	0.59f, 0.59f, 0.59f, 0.0f, 0.0f,
	0.11f, 0.11f, 0.11f, 0.0f, 0.0f,
	0.0f,  0.0f,  0.0f,  0.8f, 0.0f,
	0.0f,  0.0f,  0.0f,  0.0f, 0.5f 
};

static ColorMatrix g_TransparentColorMatrix_10 = {
	1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.1f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.0f,  1.0f 
};

static ColorMatrix g_TransparentColorMatrix_20 = {
	1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.2f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.0f,  1.0f 
};

static ColorMatrix g_TransparentColorMatrix_30 = {
	1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.3f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.0f,  1.0f 
};

static ColorMatrix g_TransparentColorMatrix_40 = {
	1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.4f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.0f,  1.0f 
};

static ColorMatrix g_TransparentColorMatrix_50 = {
	1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.5f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.0f,  1.0f 
};

static ColorMatrix g_TransparentColorMatrix_60 = {
	1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.6f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.0f,  1.0f 
};

static ColorMatrix g_TransparentColorMatrix_70 = {
	1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.7f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.0f,  1.0f 
};

static ColorMatrix g_TransparentColorMatrix_80 = {
	1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.8f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.0f,  1.0f 
};

static ColorMatrix g_TransparentColorMatrix_90 = {
	1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.9f,  0.0f,
	0.0f,  0.0f,  0.0f,  0.0f,  1.0f 
};


inline BOOL ImageFromIDResource(HINSTANCE hInst, UINT nId, LPCTSTR lpType, Image*& pImg)
{
	if (lpType == RT_BITMAP) {
		pImg = Bitmap::FromResource(hInst, MAKEINTRESOURCE(nId));
	}
	else if (lpType == RT_ICON) {
		HICON hIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(nId), 
		IMAGE_ICON, 0, 0, LR_CREATEDIBSECTION);
		pImg = Bitmap::FromHICON(hIcon);
	}
	else {
		HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nId), lpType); // type
		if (!hRsrc) {
			DWORD dwErrorCode = GetLastError();
			return FALSE;
		}

		// load resource into memory
		DWORD len = SizeofResource(hInst, hRsrc);
		BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
		if (!lpRsrc) {
			DWORD dwErrorCode = GetLastError();
			return FALSE;
		}

		// Allocate global memory on which to create stream
		HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, len);
		BYTE* pmem = (BYTE*)GlobalLock(hMem);
		memcpy(pmem,lpRsrc,len);
		IStream* pstm;
		CreateStreamOnHGlobal(hMem,FALSE,&pstm);

		// load from stream
		pImg = Gdiplus::Bitmap::FromStream(pstm);

		// free/release stuff
		GlobalUnlock(hMem);
		pstm->Release();
		FreeResource(lpRsrc);
	}

	return (pImg && ((Image *)pImg)->GetLastStatus() == Ok);
}

inline BOOL ImageFromIDResource(UINT nId, LPCTSTR lpType, Image*& pImg)
{
	return ImageFromIDResource(
		AfxGetResourceHandle(),
		nId, 
		lpType, 
		pImg);
}

inline Bitmap* ImageFromIDResource(UINT nId, LPCTSTR lpType)
{
	if (nId == 0 || lpType == NULL)
		return NULL;

	Bitmap* pImg = NULL;
	HINSTANCE hInst = AfxGetResourceHandle();

	if (lpType == RT_BITMAP) {
		pImg = Bitmap::FromResource(hInst, MAKEINTRESOURCE(nId));
	}
	else if (lpType == RT_ICON) {
		HICON hIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(nId), 
		IMAGE_ICON, 0, 0, LR_CREATEDIBSECTION);
		pImg = Bitmap::FromHICON(hIcon);
	}
	else {
		HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nId), lpType); // type
		if (!hRsrc) {
			DWORD dwErrorCode = GetLastError();
			return NULL;
		}

		// load resource into memory
		DWORD len = SizeofResource(hInst, hRsrc);
		BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
		if (!lpRsrc) {
			DWORD dwErrorCode = GetLastError();
			return NULL;
		}

		// Allocate global memory on which to create stream
		HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, len);
		BYTE* pmem = (BYTE*)GlobalLock(hMem);
		memcpy(pmem,lpRsrc,len);
		IStream* pstm;
		CreateStreamOnHGlobal(hMem,FALSE,&pstm);

		// load from stream
		pImg = Gdiplus::Bitmap::FromStream(pstm);

		// free/release stuff
		GlobalUnlock(hMem);
		pstm->Release();
		FreeResource(lpRsrc);
	}

	return pImg;
}


inline void CreateRoundRect(GraphicsPath* pGraphicsPath, Rect rect, int nRoundDiameter)
{
	Rect rc1(rect.X, rect.Y, nRoundDiameter, nRoundDiameter); // left-top
	Rect rc2(rect.X + rect.Width - nRoundDiameter - 1, rect.Y, nRoundDiameter, nRoundDiameter); // right-top
	Rect rc3(rect.X + rect.Width - nRoundDiameter - 1, rect.Y + rect.Height - nRoundDiameter - 1, nRoundDiameter, nRoundDiameter); // right-bottom
	Rect rc4(rect.X, rect.Y + rect.Height - nRoundDiameter - 1, nRoundDiameter, nRoundDiameter); // left-bottom

	pGraphicsPath->AddArc(rc1, 180, 90);
	pGraphicsPath->AddLine(rc1.X + rc1.Width/2, rc1.Y, rc2.X + rc2.Width/2, rc2.Y);

	pGraphicsPath->AddArc(rc2, 270, 90);
	pGraphicsPath->AddLine(rc2.X + rc2.Width, rc2.Y + rc2.Height/2, rc3.X + rc3.Width, rc3.Y + rc3.Height/2);

	pGraphicsPath->AddArc(rc3, 0, 90);
	pGraphicsPath->AddLine(rc3.X + rc3.Width/2, rc3.Y + rc3.Height, rc4.X + rc4.Width/2, rc4.Y + rc4.Height);

	pGraphicsPath->AddArc(rc4, 90, 90);
	pGraphicsPath->AddLine(rc4.X, rc4.Y + rc4.Height/2, rc1.X, rc1.Y + rc1.Height/2);
}

inline Color COLORREF2Color(COLORREF cr, BYTE alpha=255)
{
	Color color(alpha, GetRValue(cr), GetGValue(cr), GetBValue(cr));
	return color;
}

inline Rect CRect2Rect(CRect rc)
{
	Rect rect(rc.left, rc.top, rc.Width(), rc.Height());
	return rect;
}

inline RectF CRect2RectF(CRect rc)
{
	RectF rect((REAL)rc.left, (REAL)rc.top, (REAL)rc.Width(), (REAL)rc.Height());
	return rect;
}

inline CRect Rect2CRect(Rect rc)
{
	CRect rect(rc.X, rc.Y, rc.X + rc.Width, rc.Y + rc.Height);
	return rect;
}

inline CRect RectF2CRect(RectF rc)
{
	CRect rect((int)rc.X, (int)rc.Y, (int)rc.X + (int)rc.Width, (int)rc.Y + (int)rc.Height);
	return rect;
}

inline LPCTSTR ElcResType2String(ELC_RESOURCE_TYPE type)
{
	switch (type)
	{
	case ELCRT_PNG:
		return RT_PNG;
	case ELCRT_CURSOR:
		return RT_CURSOR;
	case ELCRT_BITMAP:
		return RT_BITMAP;
	case ELCRT_ICON:
		return RT_ICON;
	default:
		return RT_PNG;
	}
}
