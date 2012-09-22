#include "stdafx.h"
#include "BitmapCombo.h"

CBitmapCombo::CBitmapCombo()
{

}

void CBitmapCombo::PreSubclassWindow() 
{
	ASSERT(GetStyle() & CBS_DROPDOWNLIST);
	ASSERT(GetStyle() & CBS_OWNERDRAWVARIABLE);
	ASSERT(GetStyle() & CBS_HASSTRINGS);

	CComboBox::PreSubclassWindow();
}


void CBitmapCombo::SetComBoxBitmaps(UINT nBtm1, UINT nBtm2,int nCount)
{
	m_bitmap1.LoadBitmap(nBtm1);
	m_bitmap2.LoadBitmap(nBtm2);
	InitItems(nCount);
}

void CBitmapCombo::InitItems(int nCount)
{
	ASSERT(m_bitmap1.GetSafeHandle() && m_bitmap2.GetSafeHandle());
	BITMAP bm;
	m_bitmap1.GetBitmap(&bm);
	int nHeight = bm.bmHeight / nCount;

	for(int i = 0; i < nCount; i++)
	{
		int n = CComboBox::InsertString(i, _T(""));
		if (n != CB_ERR && n != CB_ERRSPACE) 
		{
			SetItemHeight(i, nHeight);
		}
	}
}

void CBitmapCombo::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{ 

}

void CBitmapCombo::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	int nIndex = lpDIS->itemID;
	BOOL bSelected = lpDIS->itemState & ODS_SELECTED;
	CRect rect = lpDIS->rcItem;
	DrawBtm(pDC, nIndex, bSelected, rect);
}

void CBitmapCombo::DrawBtm(CDC* pDC, int nItem, BOOL bSelected, CRect& rect)
{
	ASSERT(m_bitmap1.GetSafeHandle() && m_bitmap2.GetSafeHandle());
	BITMAP bm;
	m_bitmap1.GetBitmap(&bm);
	int nUnitHeight = bm.bmHeight / 5;
	int nWidth = bm.bmWidth;
	int	nyPos = nUnitHeight * nItem;

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap* pOldbmp = NULL;

	if(bSelected)
	{
		pOldbmp = dcMem.SelectObject(&m_bitmap1);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, nyPos, SRCCOPY);
	}
	else
	{
		pOldbmp = dcMem.SelectObject(&m_bitmap2);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, nyPos, SRCCOPY);
	}

	dcMem.SelectObject(pOldbmp);
	dcMem.DeleteDC();
}

