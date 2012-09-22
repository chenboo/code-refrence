void CChildView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Graphics graph(dc.GetSafeHdc());
	
	// �Զ�����ñ
	GraphicsPath startPath, endPath; 
	startPath.AddRectangle(Rect(-10, -5, 20, 10));
	Point polygonPoints[4] = {Point(0, 9), Point(10, -11), Point(0, -1), Point(-10, -11)};

	endPath.AddPolygon(polygonPoints, 4);
	CustomLineCap startCap(&endPath,NULL);
	CustomLineCap endCap(&endPath,NULL);

	Pen pen(Color::Blue, 20);
	Pen redPen(Color::Red);

	CString cstrs[] = {L"ƽ��ñ", L"����ñ", L"Բ��ñ", L"������ñ", L"��ê��ñ", L"��ê��ñ", L"Բê��ñ", L"��ê��ñ", L"��ê��ñ", L"������ñ"};
	CString estrs[] = {L"LineCapFlat", L"LineCapSquare", L"LineCapRound", 
		L"LineCapTriangle", L"LineCapNoAnchor", L"LineCapSquareAnchor", 
		L"LineCapRoundAnchor", L"LineCapDiamondAnchor", 
		L"LineCapArrowAnchor", L"LineCapCustom"};

	FontFamily fontFamily(L"Times New Roman");
	Font font(&fontFamily, 10.5);

	SolidBrush brush(Color::Blue);

	for(int i = 0; i <= 9; i++)
	{
		LineCap lc = (LineCap)(i < 4 ? i : i + 12);
		if ( i < 9)
		{
			pen.SetLineCap(lc, lc, DashCapFlat);
		}
		else
		{
			pen.SetCustomStartCap(&startCap);
			pen.SetCustomEndCap(&endCap);
			pen.SetWidth(3.0f);
		}

		int y = 20 + i * 40;
		graph.DrawLine(&pen, 100, y, 400, y);
		Pen penBlue(Color::Blue, 3.0f);
		graph.DrawLine(&penBlue, 100, y, 400, y);
		
		graph.DrawString(cstrs[i], -1, &font, PointF(15.0f, y - 8.0f), &brush);
		graph.DrawString(estrs[i], -1, &font, PointF(425.0f, y - 8.0f), &brush);
	}
}