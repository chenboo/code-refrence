BOOL ����::OnEraseBkgnd(CDC* pDC)
{
 CRect rect;
 GetClientRect(&rect);//���ر���λͼ

  CString strFullPath;
 strFullPath =_T("\\bg.bmp");

 CFile   file; 
 file.Open( strFullPath,CFile::modeRead); 
 int   len   =   file.GetLength(); 
 char   *buf   =   new   char[len]; 
 file.Read(buf,len); 
 file.Close(); 
 CBrush   brush; 
 const   void   *buf2   =   buf   +   sizeof(BITMAPFILEHEADER); 
 brush.CreateDIBPatternBrush(buf2,DIB_RGB_COLORS); 

 pDC-> SelectObject(&brush); 
 pDC-> FillRect(rect,&brush); 
 delete   []buf; 
 return TRUE;
}
