#if !defined(AFX_CBITMAPPICKERCOMBO_H__8AAE34F7_7B02_11D3_A615_0060085FE616__INCLUDED_)
#define AFX_CBITMAPPICKERCOMBO_H__8AAE34F7_7B02_11D3_A615_0060085FE616__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000 

class CBitmapCombo : public CComboBox
{
public:
	CBitmapCombo();
	virtual ~CBitmapCombo() {}

public:
	void SetComBoxBitmaps(UINT nBtm1, UINT nBtm2,int nCount);
	void InitItems(int nCount);
	void DrawBtm(CDC* pDC, int nItem, BOOL bSelected, CRect& rect);

public:
	CBitmap		m_bitmap1;
	CBitmap		m_bitmap2;

protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual int AddString(LPCTSTR lpszString) { return -1; }
	virtual int InsertString(int nIndex, LPCTSTR lpszString) { return -1; }
	virtual int DeleteString(int nIndex) { return -1; }
};

#endif // !defined(AFX_CBITMAPPICKERCOMBO_H__8AAE34F7_7B02_11D3_A615_0060085FE616__INCLUDED_)
