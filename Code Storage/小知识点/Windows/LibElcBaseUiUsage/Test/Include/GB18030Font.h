#pragma once

#define LANGID_ZH_CN 2052 // chinese(simplified) language id 


inline BOOL CreateGB18030LogFont(
	LOGFONT& lf,
	LPCTSTR lpszFaceName,
	BOOL bBold = FALSE,
	int nSize = 9,
	int nDPI = 72
)
{
	if (lpszFaceName == NULL)
		return FALSE;
	
	memset(&lf, 0, sizeof(lf));

	HDC hDC = ::GetDC(NULL);
	if (hDC == NULL)
		return FALSE;
	lf.lfHeight = -MulDiv(nSize, GetDeviceCaps(hDC, LOGPIXELSY), nDPI);
	::ReleaseDC(NULL, hDC);

	if (bBold)
		lf.lfWeight = FW_BOLD;
	else
		lf.lfWeight = FW_NORMAL;

	lf.lfItalic 		= FALSE;
	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfQuality		= DEFAULT_QUALITY;
	lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfPitchAndFamily	= DEFAULT_PITCH|FF_DONTCARE;
	_tcscpy(lf.lfFaceName, lpszFaceName);

	return TRUE;
}



int CALLBACK EnumFontFamExProc(
	ENUMLOGFONTEX *lpelfe,    // logical-font data
	NEWTEXTMETRICEX *lpntme,  // physical-font data
	DWORD FontType,           // type of font
	LPARAM lParam             // application-defined data
	)
{
	if ( _tcslen (lpelfe->elfFullName) < 5 )
		return TRUE;
	if (lpelfe->elfFullName[0] == _T('@'))
		return TRUE;

	// get 18030 
	LPCTSTR lpszName = lpelfe->elfFullName + _tcslen(lpelfe->elfFullName) - 5;

	if (_tcscmp(lpszName, _T("18030")) == 0)
	{
		_tcscpy((LPTSTR)lParam, lpelfe->elfFullName);
		return FALSE;
	}
	
	if (_tcscmp(lpelfe->elfFullName, _T("宋体-方正超大字符集")) == 0
		|| _tcsicmp(lpelfe->elfFullName , _T("simsun (founder extended)")) == 0
		|| _tcsicmp(lpelfe->elfFullName , _T("simsun(founder extended)")) == 0)
	{
		_tcscpy((LPTSTR)lParam, lpelfe->elfFullName);
	}

	return TRUE;
}

inline BOOL FindGB18030Font(LPTSTR lpszFaceName, int cchMax)
{
	if (lpszFaceName == NULL || cchMax < LF_FACESIZE)
		return FALSE;

	HDC hDC = ::GetDC(NULL);
	if (hDC == NULL)
		return FALSE;

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	memset(lpszFaceName, 0, cchMax * sizeof(TCHAR));
	EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)lpszFaceName, (DWORD)0 );
	::ReleaseDC(NULL, hDC);

	if (_tcslen(lpszFaceName) > 0)
		return TRUE;
	else 
		return FALSE;
}