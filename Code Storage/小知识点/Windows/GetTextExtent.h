//����ַ����Ĵ�С

BOOL GetTextExtentExPoint(
  HDC hdc, 
  LPCTSTR lpszStr, 
  int cchString, 
  int nMaxExtent, 
  LPINT lpnFit, 
  LPINT alpDx, 
  LPSIZE lpSize
); 


//���÷�

//(һ)
//1>��ѡ����
if (pFont) 
{
	hFont = ::SelectObject(hdc, pFont->m_hObject);
}
else
{ 
	hFont = ::SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
}

//2>�ٸ����ַ������м���
::GetTextExtentExPoint(hdc, 
	lpszText,
	(int)_tcslen(lpszText),
	0,
	NULL,
	NULL,
	&size);


//(��)
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
