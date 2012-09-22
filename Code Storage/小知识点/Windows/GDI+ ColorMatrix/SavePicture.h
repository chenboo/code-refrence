//save picture alter by colormatrix
//can be transprent


	CPaintDC dc(this); // device context for painting
	Graphics graph(dc.m_hDC);

	ColorMatrix colorMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.3f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	ImageAttributes imageAtt;
	imageAtt.SetColorMatrix( &colorMatrix );

	Bitmap Bmp(_T("res\\Drawer.bmp"));
	INT nWidth = Bmp.GetWidth();
	INT nHeight = Bmp.GetHeight();

	graph.DrawImage(
			&Bmp,
			Rect(0, 0, nWidth, nHeight),
			0,
			0,
			nWidth,
			nHeight,
			UnitPixel,
			&imageAtt);

	Bitmap bmpsave(nWidth, nHeight);

	Graphics graph2(&bmpsave);
	graph2.DrawImage(
			&Bmp,
			Rect(0, 0, nWidth, nHeight),
			0,
			0,
			nWidth,
			nHeight,
			UnitPixel,
			&imageAtt);

	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	bmpsave.Save(_T("res\\Mosaic2.png"), &pngClsid, NULL);




//-------------------------helper function

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}


