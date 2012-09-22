
void ExtendDC(CDC* pDC,
			CDC* pExDC,
			CBitmap* pExtBmp,
			CBitmap* pOldExtBmp,
			int nWidth,
			int nHeight)
{
	ASSERT(pDC->GetSafeHdc() && pExDC->GetSafeHdc());

	BITMAP bm;
	pExtBmp->GetBitmap(&bm);

	if(nWidth <= bm.width && nHeight <= bm.bmHeight)
	{
		return;
	}

	int nNewWidth = max(nWidth, bm.bmWidth);
	int nNewHeight = max(nHeight, bm.bmHeight);

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, bm.bmWidth, bm.bmHeight);
	CBitmap* pOldbmp = dcMem.SelectObject(&bmp);

	dcMem.FillSolidRect(0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 255, 255));
	dcMem.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, pExDC, 0, 0, SRCCOPY);

	pExDC->SelectObject(pOldExtBmp);
	pExtBmp->DeleteObject();
	pExtBmp->CreateCompatibleBitmap(pDC, nNewWidth, nNewHeight);
	pOldExtBmp = pExDC->SelectObject(&pExtBmp);

	pExDC.FillSolidRect(0, 0, nNewWidth, nNewHeight, RGB(255, 255, 255));
	pExDC.BitBlt(0, 0, nNewWidth, nNewHeight, &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(pOldbmp);
	dcMem.DeleteDC();
	bmp.DeleteObject();
}