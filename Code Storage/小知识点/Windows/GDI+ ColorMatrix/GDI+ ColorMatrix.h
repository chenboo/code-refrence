	//利用GDI+颜色矩阵(ColorMatrix)生成灰度图片
	CPaintDC dc(this); // device context for painting
	Graphics graph(dc.m_hDC);

	//灰度颜色矩阵
	ColorMatrix colorMatrix = {
		0.3f,  0.3f,  0.3f,  0.0f, 0.0f,
		0.59f, 0.59f, 0.59f, 0.0f, 0.0f,
		0.11f, 0.11f, 0.11f, 0.0f, 0.0f,
		0.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.0f,  0.0f,  0.0f,  0.0f, 1.0f};

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
		UnitPixel);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, nWidth, nHeight);
	CBitmap* pOldbmp = dcMem.SelectObject(&bmp);

	Graphics graph2(dcMem.m_hDC);
	graph2.DrawImage(
		&Bmp,
		Rect(0, 0, nWidth, nHeight),
		0,
		0,
		nWidth,
		nHeight,
		UnitPixel,
		&imageAtt);

	dcMem.SelectObject(pOldbmp);
	dcMem.DeleteDC();

	CImage image;
	image.Attach((HBITMAP)(bmp.m_hObject));
	image.Save(_T("res\\Drawer3.bmp"), ImageFormatBMP);

	image.Detach();
	bmp.DeleteObject();