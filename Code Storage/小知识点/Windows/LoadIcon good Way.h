//A good way to get the icon handle

HICON hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);

//For example want to draw a icon in the view

//Code:

void CSDIView::OnDraw(CDC* pDC)
{
	CSDIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//load the icon in the current application
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);

	//get the icon's information
	ICONINFO iconinfo;
	::GetIconInfo(hIcon, &iconinfo);
	int nWidht = iconinfo.xHotspot * 2;
	int nHeight = iconinfo.yHotspot * 2;

	if (iconinfo.hbmColor)
	{
		::DeleteObject(iconinfo.hbmColor);
	}
	if (iconinfo.hbmMask)
	{
		::DeleteObject(iconinfo.hbmMask);
	}
	
	UINT nFlags = DSS_NORMAL | DST_ICON;
	CPoint ptLT(100, 100);
	pDC->DrawState(ptLT, CSize(nWidht, nHeight), hIcon, nFlags, (CBrush *)NULL);
}

