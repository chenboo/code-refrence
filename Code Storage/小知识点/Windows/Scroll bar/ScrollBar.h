void CSDIView::SetHScroll()
{
	CRect rect;
	GetClientRect(rect);
	SCROLLINFO scrollinfo;
	scrollinfo.cbSize = sizeof(SCROLLINFO);
	scrollinfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

	scrollinfo.nMax = m_nWidth;
	scrollinfo.nMin = m_nWidth;
	scrollinfo.nPage = rect.Width();
	scrollinfo.nPos = m_ptOffset.x;
	if(rect.Width() < m_nWidth)
	{
		scrollinfo.nMin = 0;
	}
	else
	{
		m_ptOffset.x = 0;
	}

	SetScrollInfo(SB_HORZ, &scrollinfo);
}

void CSDIView::SetVScroll()
{
	CRect rect;
	GetClientRect(rect);
	SCROLLINFO scrollinfo;
	scrollinfo.cbSize = sizeof(SCROLLINFO);
	scrollinfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	scrollinfo.nMax = m_nHeight;
	scrollinfo.nMin = m_nHeight;
	scrollinfo.nPage = rect.Height();
	scrollinfo.nPos = m_ptOffset.y;
	if(rect.Height() < m_nHeight)
	{
		scrollinfo.nMin = 0;
	}
	else 
	{
		m_ptOffset.y = 0;
	}

	SetScrollInfo(SB_VERT, &scrollinfo);
}


void CSDIView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_HORZ, &scrollinfo);

	int nDelta = 0;
	int nScrollPos = GetScrollPos(SB_HORZ);
	int n = nScrollPos;
	
	switch(nSBCode)
	{
	case SB_LINERIGHT:
		nDelta = 8;
		break;
	case SB_LINELEFT:
		nDelta = - 8;
		break;
	case SB_THUMBTRACK:
		nDelta = nPos - n;
		break;
	default:
		break;
	}

	n += nDelta;

	if(n < 0)
	{
		nDelta = - nScrollPos;
	}
	else if(n > scrollinfo.nMax)
	{
		nDelta = scrollinfo.nMax - nScrollPos;
	}

	nScrollPos += nDelta;

	SetScrollPos(SB_HORZ, nScrollPos);
	m_ptOffset.x = nScrollPos;
	Invalidate();

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSDIView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo);

	int nDelta = 0;
	int nScrollPos = GetScrollPos(SB_VERT);
	int n = nScrollPos;
	
	switch(nSBCode)
	{
	case SB_LINEDOWN:
		nDelta = 8;
		break;
	case SB_LINEUP:
		nDelta = - 8;
		break;
	case SB_THUMBTRACK:
		nDelta = nPos - n;
		break;
	default:
		break;
	}

	n += nDelta;
	if(n < 0)
	{
		nDelta = - nScrollPos;
	}
	else if(n > scrollinfo.nMax)
	{
		nDelta = scrollinfo.nMax - nScrollPos;
	}

	nScrollPos += nDelta;
	SetScrollPos(SB_VERT, nScrollPos);

	m_ptOffset.y = nScrollPos;
	Invalidate();

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CSDIView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int nScrollPos = GetScrollPos(SB_VERT);
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_HORZ, &scrollinfo);

	if(scrollinfo.nMax != scrollinfo.nMin)
	{
		if(zDelta == 120)
		{
			if(nScrollPos >= 70)
			{
				nScrollPos -= 70;
			}
			else
			{
				nScrollPos = 0;
			}
		}
		else if(zDelta == -120)
		{
			if( nScrollPos <= scrollinfo.nMax - 70 )
			{
				nScrollPos += 70;
			}
			else
			{
				nScrollPos = scrollinfo.nMax;
			}
		}

		SetScrollPos(SB_VERT, nScrollPos);
		Invalidate();
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}