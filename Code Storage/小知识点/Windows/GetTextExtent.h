//获得字符串的大小

BOOL GetTextExtentExPoint(
  HDC hdc, 
  LPCTSTR lpszStr, 
  int cchString, 
  int nMaxExtent, 
  LPINT lpnFit, 
  LPINT alpDx, 
  LPSIZE lpSize
); 


//常用法

//(一)
//1>先选字体
if (pFont) 
{
	hFont = ::SelectObject(hdc, pFont->m_hObject);
}
else
{ 
	hFont = ::SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
}

//2>再根据字符串进行计算
::GetTextExtentExPoint(hdc, 
	lpszText,
	(int)_tcslen(lpszText),
	0,
	NULL,
	NULL,
	&size);


//(二)
HGDIOBJ hFont = NULL;
if (pFont) 
{
	hFont = ::SelectObject(hdc, pFont->m_hObject);
}
else
{ 
	hFont = ::SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
}

if(hFont)
{
	::DrawText(hdc, 
		lpszText, 
		-1, 
		&rect, 
		DT_WORDBREAK | DT_NOPREFIX | DT_NOFULLWIDTHCHARBREAK | DT_CALCRECT);
		::SelectObject(hdc, hFont);
}
