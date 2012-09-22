#ifndef __ELC_UI_SNIPPETS_H__
#define __ELC_UI_SNIPPETS_H__

#pragma once 


#define CALC_FONT_HEIGHT(height, size) { \
	HDC hDC = ::GetDC(NULL); \
	height = -MulDiv(size, GetDeviceCaps(hDC, LOGPIXELSY), 72); \
	::ReleaseDC(NULL, hDC); \
}


/********************************************************************************
* Get default font of system
********************************************************************************/
inline BOOL GetDefaultFont(LOGFONT& lf)
{
	memset(&lf, 0, sizeof(LOGFONT));
	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

	return TRUE;
}

inline BOOL GetDefaultFont(CFont& font)
{
	LOGFONT lf;
	if (!GetDefaultFont(lf))
		return FALSE;

	if (font.GetSafeHandle())
		font.DeleteObject();
	font.CreateFontIndirect(&lf);

	return TRUE;
}

inline BOOL GetMSShellDlgFont(LOGFONT& lf)
{
	GetDefaultFont(lf);
	_tcscpy_s(lf.lfFaceName, _T("MS Shell Dlg"));
	return TRUE;
}

inline BOOL GetMSShellDlgFont(CFont& font)
{
	LOGFONT lf;
	GetMSShellDlgFont(lf);
	if (font.GetSafeHandle())
		font.DeleteObject();
	return (font.CreateFontIndirect(&lf) != NULL);
}

inline BOOL IsFontSmoothingClearType()
{
	int nFontSmoothingType = FE_FONTSMOOTHINGSTANDARD;
	SystemParametersInfo(SPI_GETFONTSMOOTHINGTYPE, 0, &nFontSmoothingType, FALSE);
	if (FE_FONTSMOOTHINGCLEARTYPE == nFontSmoothingType)
		return TRUE;
	return FALSE;
}


/********************************************************************************
* Get the first sub string form a string whit divided by ","
* ex. U have a string :"220,190,201", u can get a sub string "220" and remain "190,201"
********************************************************************************/
inline CString GetFirstParam(CString& strName)
{
	int nPosition = strName.Find(_T(","));
	if (nPosition < 0)
		return strName;
	
	TCHAR szRes[512];
	wsprintf(szRes, _T("%s"), strName);
	szRes[nPosition] = _T('\0');

	strName.Delete(0, nPosition + 1);
	strName.TrimLeft();
	strName.TrimRight();

	return szRes;
}

/********************************************************************************
* get the first sub string form a string whit divided by specify sign
* ex. U have a string :"a;b;c", specify lpszDelimiter=";"
* and then u can get a sub string "a" and remain "b;c"
********************************************************************************/
inline CString GetFirstParam(CString& strName, LPCTSTR lpszDelimiter)
{
	int nPosition = strName.Find(lpszDelimiter);
	if (nPosition < 0)
		nPosition = strName.GetLength();
	
	CString strRet = strName.Left(nPosition);
	strName.Delete(0, nPosition + 1);
	strName.Trim();

	return strRet;
}


/********************************************************************************
* passe color form string
*
* parse hex string 
* such as : FF00FF, #FF00FF
********************************************************************************/
inline BOOL ParseColor(CString str, COLORREF &cr)
{
	if (str.GetAt(0) == _T('#'))
		str.Delete(0, 1);

	if (str.GetLength() != 6) return FALSE;

	int nRed, nGreen, nBlue;

	if (_stscanf( str.Mid( 0, 2 ), _T("%x"), &nRed ) != 1) return FALSE;
	if (_stscanf( str.Mid( 2, 2 ), _T("%x"), &nGreen ) != 1) return FALSE;
	if (_stscanf( str.Mid( 4, 2 ), _T("%x"), &nBlue ) != 1) return FALSE;

	cr = RGB( nRed, nGreen, nBlue );

	return TRUE;
}

inline BOOL ParseColorA(const char* pszColor, COLORREF& color)
{
	if (!pszColor || strlen(pszColor) != strlen("#000000"))
		return FALSE;

	char szValue[3][3] = {0};
	strncpy(szValue[0], pszColor+1, 2); 
	strncpy(szValue[1], pszColor+3, 2); 
	strncpy(szValue[2], pszColor+5, 2); 

	int nRed, nGreen, nBlue;

	if ( sscanf( szValue[0], "%x", &nRed ) != 1 ) return FALSE;
	if ( sscanf( szValue[1], "%x", &nGreen ) != 1 ) return FALSE;
	if ( sscanf( szValue[2], "%x", &nBlue ) != 1 ) return FALSE;
	
	color = RGB(nRed, nGreen, nBlue);

	return TRUE;
}


/********************************************************************************
* parse decimal string
* such as : RGB(xxx,xxx,xxx)
********************************************************************************/
inline BOOL ParseColorRGBDecimal(CString str, COLORREF &cr)
{
	if (str.Left(4).CompareNoCase(_T("rgb(")) != 0 ||
		str.Right(1) != _T(")") )
	{
		return FALSE;
	}

	str.Delete(0, 4);

	int nRed, nGreen, nBlue;

	nRed = _ttoi(GetFirstParam(str));
	nGreen = _ttoi(GetFirstParam(str));
	nBlue = _ttoi(GetFirstParam(str));

	cr = RGB( nRed, nGreen, nBlue );

	return TRUE;
}

inline BOOL ParseRectA(const char* pszRect, CRect& rect)
{
	if (!pszRect || strlen(pszRect) < strlen("0,0,0,0"))
		return FALSE;

	char szValue[4][8] = {0}; 

	const char* pComma = strstr(pszRect, ",");
	strncpy(szValue[0], pszRect, pComma-pszRect);

	pszRect += (pComma-pszRect+1);
	pComma = strstr(pszRect, ",");
	strncpy(szValue[1], pszRect, pComma-pszRect);

	pszRect += (pComma-pszRect+1);
	pComma = strstr(pszRect, ",");
	strncpy(szValue[2], pszRect, pComma-pszRect);

	pszRect += (pComma-pszRect+1);
	strcpy(szValue[3], pszRect);

	rect.left = atoi(szValue[0]);
	rect.top = atoi(szValue[1]); 
	rect.right = rect.left + atoi(szValue[2]);
	rect.bottom = rect.top + atoi(szValue[3]);

	return TRUE;
}


/********************************************************************************
* rect(x,y,cx,cy) --> point = (x,y) size=(cx,cy)
* the x,y,cx,cy may be a negative value to indicate the relative position
********************************************************************************/
inline BOOL ParseAnchor(const char* pszRect, POINT &point, SIZE &size)
{
	if (!pszRect || strlen(pszRect) < strlen("0,0,0,0"))
		return FALSE;

	char szValue[4][8] = {0}; 

	const char* pComma = strstr(pszRect, ",");
	strncpy(szValue[0], pszRect, pComma-pszRect);

	pszRect += (pComma-pszRect+1);
	pComma = strstr(pszRect, ",");
	strncpy(szValue[1], pszRect, pComma-pszRect);

	pszRect += (pComma-pszRect+1);
	pComma = strstr(pszRect, ",");
	strncpy(szValue[2], pszRect, pComma-pszRect);

	pszRect += (pComma-pszRect+1);
	strcpy(szValue[3], pszRect);

	point.x = atoi(szValue[0]);
	point.y = atoi(szValue[1]); 
	size.cx = atoi(szValue[2]);
	size.cy = atoi(szValue[3]);

	return TRUE;
}


/********************************************************************************
* rect(x,y,cx,cy) --> point = (x,y) size=(cx,cy)
********************************************************************************/
inline BOOL ParseSize(const char* pszSize, SIZE &size)
{
	size.cx = size.cy = 0;
	if (!pszSize || strlen(pszSize) < strlen("0,0"))
		return FALSE;

	char szValue[2][8] = {0}; 

	const char* pComma = strstr(pszSize, ",");
	strncpy(szValue[0], pszSize, pComma-pszSize);

	pszSize = pComma + 1;
	strcpy(szValue[1], pszSize);

	size.cx = atoi(szValue[0]);
	size.cy = atoi(szValue[1]);

	return TRUE;
}


/********************************************************************************
* Create a region from specified color
********************************************************************************/
inline BOOL CreateRegionFromColor(CRgn *pRgn, CBitmap *pBitmap, COLORREF keyColor, COLORREF crTolerance = 0x101010)
{
	if (!pRgn 
		|| !pBitmap) 
		return FALSE;

	// Tolerance Range
	BYTE lr = (BYTE)max(0, (int)GetRValue(keyColor) - (int)GetRValue(crTolerance));
	BYTE lg = (BYTE)max(0, (int)GetGValue(keyColor) - (int)GetGValue(crTolerance));
	BYTE lb = (BYTE)max(0, (int)GetBValue(keyColor) - (int)GetBValue(crTolerance));
	BYTE hr = (BYTE)min((int)0xff, (int)GetRValue(keyColor) + (int)GetRValue(crTolerance));
	BYTE hg = (BYTE)min((int)0xff, (int)GetGValue(keyColor) + (int)GetGValue(crTolerance));
	BYTE hb = (BYTE)min((int)0xff, (int)GetBValue(keyColor) + (int)GetBValue(crTolerance));

	BITMAP bm = {0};
	pBitmap->GetBitmap(&bm);

	CDC					memDC;
	CBitmap				*pOldMemBmp = NULL;
	int					x, y;
	int					n=0;		//number of transparent pixels
	CRgn				rgnTemp;

	if (!memDC.CreateCompatibleDC(NULL))
		return FALSE;
	pOldMemBmp = memDC.SelectObject(pBitmap);
	pRgn->CreateRectRgn(0, 0, bm.bmWidth, bm.bmHeight);

	COLORREF m_Color = memDC.GetPixel(0, 0);
	for (x = 0; x <= bm.bmWidth; x++) {
		for (y = 0; y <= bm.bmHeight; y++) {
			COLORREF cr = memDC.GetPixel(x,y);
			BYTE b = GetRValue(cr);
			if (lr <= b && b <= hr) {
				b = GetGValue(cr);
				if (lg <= b && b <= hg) {
					b = GetBValue(cr);
					if (lb <= b && b <= hb) {
						rgnTemp.CreateRectRgn(x, y, x + 1, y + 1);
						pRgn->CombineRgn(pRgn, &rgnTemp, RGN_XOR);
						rgnTemp.DeleteObject();
						++n;
					}
				}
			}
		}
	}

	if (pOldMemBmp) 
		memDC.SelectObject(pOldMemBmp);
	memDC.DeleteDC();

	return n > 0;
}


/********************************************************************************
* Mirror rect
********************************************************************************/
inline CRect MirrorRect(int cx, RECT rect)
{
	CRect rcRet = rect;
	int nWidth = rcRet.Width();
	rcRet.left = cx - (rcRet.left + nWidth);
	rcRet.right = rcRet.left + nWidth;
	return rcRet;
}

inline Rect MirrorRect(int cx, Rect rect)
{
	Rect rcRet = rect;
	rcRet.X = cx - (rcRet.X + rcRet.Width);
	return rcRet;
}

inline BOOL IsLayoutRTL()
{
	DWORD dwDefaultLayout;
	GetProcessDefaultLayout(&dwDefaultLayout);
	return ((LAYOUT_RTL & dwDefaultLayout) != 0);
}


#endif //__ELC_UI_SNIPPETS_H__