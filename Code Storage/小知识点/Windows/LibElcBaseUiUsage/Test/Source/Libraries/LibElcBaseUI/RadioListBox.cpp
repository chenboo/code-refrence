// RadioListBox.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "RadioListBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define ITEM_MARGIN		4


// CRadioListBox

IMPLEMENT_DYNAMIC(CRadioListBox, CWnd)

CRadioListBox::CRadioListBox()
{
	m_dwStyleRLS = 0;
	m_hNotifyWnd = 0;
	m_dwNotifyMsg = 0;

	m_dwWidth = 0;
	m_dwHeight = 0;

	m_nControlID = 0;
	m_nListHeight = 0;

	m_pImgList = NULL;
	m_szImage.cx = 0;
	m_szImage.cy = 0;

	m_crText = RGB(0,0,0);
	m_crBk = RGB(255,255,255);

	m_hUnCheck = NULL;
	m_hCheck = NULL;

	m_bDrawFocusRect = TRUE;
}

CRadioListBox::~CRadioListBox()
{
}


BEGIN_MESSAGE_MAP(CRadioListBox, CWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CRadioListBox message handlers


BOOL CRadioListBox::Create(CRect rc, CWnd* pWndParent, UINT nID, DWORD dwStyle, DWORD dwStyleRLS)
{
	if (GetSafeHwnd())
		return TRUE;
	
	m_dwStyleRLS = dwStyleRLS;

	m_dwWidth = rc.Width();
	m_dwHeight = rc.Height();

	m_nControlID = nID;
	m_nListHeight = 0;

	DWORD dwStyleEx = 0;

	DWORD dwLayout;
	GetProcessDefaultLayout(&dwLayout);
	if (LAYOUT_RTL & dwLayout) {
		dwStyleEx |= WS_EX_LAYOUTRTL;
	}

	LPCTSTR pszClassName = ::AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW);

	if (!CreateEx(dwStyleEx, pszClassName, NULL, dwStyle, rc, pWndParent, nID))
		return FALSE;

	SetOwner(pWndParent);

	return TRUE;
}

void CRadioListBox::SetNotifyMsg(HWND hNotifyWnd, DWORD dwNotifyMsg)
{
	m_hNotifyWnd = hNotifyWnd;
	m_dwNotifyMsg = dwNotifyMsg;
}

CImageList* CRadioListBox::SetImageList(CImageList* pImageList)
{
	CImageList* pOldImgList = m_pImgList;
	m_pImgList = pImageList;

	IMAGEINFO info = {0};
	if (m_pImgList->GetImageInfo(0, &info)) {
		m_szImage.cx = info.rcImage.right - info.rcImage.left;
		m_szImage.cy = info.rcImage.bottom - info.rcImage.top;
	}

	return pOldImgList;
}

CImageList* CRadioListBox::GetImageList()
{
	return m_pImgList;
}

int CRadioListBox::AddItem(LPCTSTR lpszItem, int nItemImage, LPCTSTR lpszSubItem, int nSubItemImage)
{
	if (NULL == lpszItem)
		return -1;

	PRADIOLIST_ITEM pItem = new RADIOLIST_ITEM;
	if (NULL == pItem)
		return -1;

	pItem->nItem = (int)m_lstItem.GetCount();

	pItem->strItem = lpszItem;
	pItem->nItemImage = nItemImage;

	pItem->strSubItem = lpszSubItem;
	pItem->nSubItemImage = nSubItemImage;

	pItem->nHeight = CalculateItemHeight(lpszItem, lpszSubItem);
/*
	CRect rcItem(0, m_nListHeight, m_dwWidth, m_nListHeight + pItem->nHeight);
	pItem->rcItem.left = rcItem;
*/
	m_lstItem.AddTail(pItem);

	m_nListHeight += pItem->nHeight;

	UpdateScrollInfo();

	Invalidate();

	return pItem->nItem;
}

BOOL CRadioListBox::DeleteItem(int nItem)
{
	PRADIOLIST_ITEM pItem = GetItem(nItem);
	if (!pItem)
		return FALSE;

	POSITION pos = m_lstItem.Find(pItem);
	ASSERT(pos != NULL);

	m_lstItem.RemoveAt(pos);

	m_nListHeight -= pItem->nHeight;
	
	delete pItem;

	ResetIndex();

	UpdateScrollInfo();

	Invalidate();

	return TRUE;
}

BOOL CRadioListBox::SetItemText(int nItem, int nSubItem, CString strText)
{
	PRADIOLIST_ITEM pItem = GetItem(nItem);
	if (!pItem)
		return FALSE;

	if (nSubItem == 0)
		pItem->strItem = strText;
	else
		pItem->strSubItem = strText;

	pItem->nHeight = CalculateItemHeight(pItem->strItem, pItem->strSubItem);

	m_nListHeight = CalculateListHeight();

	Invalidate();

	return TRUE;
}

CString CRadioListBox::GetItemText(int nItem, int nSubItem)
{
	PRADIOLIST_ITEM pItem = GetItem(nItem);
	if (!pItem)
		return _T("");

	return (nSubItem == 0) ? pItem->strItem : pItem->strSubItem;
}

BOOL CRadioListBox::SetItemImage(int nItem, int nSubItem, int nImage)
{
	PRADIOLIST_ITEM pItem = GetItem(nItem);
	if (!pItem)
		return FALSE;
	
	if (nSubItem == 0)
		pItem->nItemImage = nImage;
	else
		pItem->nSubItemImage = nImage;

	Invalidate();

	return TRUE;
}

int CRadioListBox::GetItemImage(int nItem, int nSubItem)
{
	PRADIOLIST_ITEM pItem = GetItem(nItem);
	if (!pItem)
		return -1;

	return (nSubItem == 0) ? pItem->nItemImage : pItem->nSubItemImage;
}

BOOL CRadioListBox::SetItemData(int nItem, DWORD dwData)
{
	PRADIOLIST_ITEM pItem = GetItem(nItem);
	if (!pItem)
		return FALSE;

	pItem->dwItemData = dwData;

	return TRUE;
}

DWORD CRadioListBox::GetItemData(int nItem)
{
	PRADIOLIST_ITEM pItem = GetItem(nItem);
	if (!pItem)
		return 0;

	return pItem->dwItemData;
}

BOOL CRadioListBox::SetCheck(int nItem, BOOL bCheck)
{
	PRADIOLIST_ITEM pItem = GetItem(nItem);
	if (!pItem)
		return FALSE;

	pItem->bChecked = bCheck;

	Invalidate();

	return TRUE;
}

BOOL CRadioListBox::GetCheck(int nItem)
{
	PRADIOLIST_ITEM pItem = GetItem(nItem);
	if (!pItem)
		return FALSE;

	return pItem->bChecked;
}

int CRadioListBox::GetCheckedItem()
{
	POSITION pos = m_lstItem.GetHeadPosition();
	while (pos) {
		PRADIOLIST_ITEM pItem = m_lstItem.GetNext(pos);
		if (pItem->bChecked) 
			return pItem->nItem;
	}

	return -1;
}

void CRadioListBox::SetFont(CFont* pFont)
{
	if (pFont == NULL)
		return;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	pFont->GetLogFont(&lf);
	m_ftText.DeleteObject();
	m_ftText.CreateFontIndirect(&lf);
}

void CRadioListBox::SetRadioIcon(HICON hCheck, HICON hUnCheck)
{
	m_hUnCheck = hUnCheck;
	m_hCheck = hCheck;
}

void CRadioListBox::SetTextColor(COLORREF cr)
{
	m_crText = cr;
}

COLORREF CRadioListBox::GetTextColor()
{
	return m_crText;
}

void CRadioListBox::SetBkColor(COLORREF cr)
{
	m_crBk = cr;
}

COLORREF CRadioListBox::GetBkColor()
{
	return m_crBk;
}

PRADIOLIST_ITEM CRadioListBox::GetItem(int nItem)
{
	if ((nItem < 0) || (nItem >= m_lstItem.GetCount()))
		return NULL;

	POSITION pos = m_lstItem.GetHeadPosition();
	while (pos) {
		PRADIOLIST_ITEM pItem = m_lstItem.GetNext(pos);
		if (pItem->nItem == nItem)
			return pItem;
	}	

	return NULL;
}

int CRadioListBox::CalculateItemHeight(CString strItemText, CString strSubItemText)
{
	// Calculate Text Width
	int nTextWidth = m_dwWidth - 2 * ITEM_MARGIN;

	if (m_dwStyleRLS & RLS_HAS_RADIO)
		nTextWidth -= 16 + ITEM_MARGIN;

	if (m_szImage.cx > 0) {
		nTextWidth -= m_szImage.cx + ITEM_MARGIN;
	}

	// Prepare DC
	CDC *pDC = GetDC();
	if (NULL == pDC)
		return 0;
	
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	//CFont *pOldFont = memDC.SelectObject(&m_ftText);

	// Calculate 
	int nItemTextHeight = 0;

	if (m_dwStyleRLS & RLS_HAS_RADIO)
		nItemTextHeight = 16;

	if (m_szImage.cy > nItemTextHeight)
		nItemTextHeight = m_szImage.cy;

	CRect rcItem(0,0,nTextWidth,0);
	memDC.DrawText(strItemText, strItemText.GetLength(), rcItem, DT_WORDBREAK|DT_NOPREFIX|DT_CALCRECT);
	if (rcItem.Height() > nItemTextHeight)
		nItemTextHeight = rcItem.Height();

	int nSubItemTextHeigh = 0;

	if (!strSubItemText.IsEmpty()) {
		CRect rcSubItem(0,0,nTextWidth,0);
		memDC.DrawText(strSubItemText, strSubItemText.GetLength(), rcSubItem, DT_WORDBREAK|DT_NOPREFIX|DT_CALCRECT);
		nSubItemTextHeigh = rcSubItem.Height();

		if (m_szImage.cy > nSubItemTextHeigh)
			nSubItemTextHeigh = m_szImage.cy;
	}

	// Total Item Height
	int nTotalItemHeight = 2 * ITEM_MARGIN + nItemTextHeight;
	if (nSubItemTextHeigh > 0) 
		nTotalItemHeight += nSubItemTextHeigh + ITEM_MARGIN;

	return nTotalItemHeight;
}

int CRadioListBox::CalculateListHeight()
{
	int nListHeight = 0;

	POSITION pos = m_lstItem.GetHeadPosition();
	while (pos) {
		PRADIOLIST_ITEM pItem = m_lstItem.GetNext(pos);
		nListHeight += pItem->nHeight;
	}

	return nListHeight;
}

void CRadioListBox::ResetIndex()
{
	int nItem = 0;
	POSITION pos = m_lstItem.GetHeadPosition();
	while (pos) {
		PRADIOLIST_ITEM pItem = m_lstItem.GetNext(pos);
		pItem->nItem = nItem ++;
	}
}

int CRadioListBox::HitTest(CPoint point)
{
	int nTop = 0;
	point.y += GetScrollPos(SB_VERT);

	POSITION pos = m_lstItem.GetHeadPosition();
	while (pos) {
		PRADIOLIST_ITEM pItem = m_lstItem.GetNext(pos);

		if ((point.y >= nTop) && (point.y < nTop + pItem->nHeight))
			return pItem->nItem;
		
		nTop += pItem->nHeight;
	}

	return -1;
}

void CRadioListBox::SetAllItemCheck(BOOL bCheck)
{
	POSITION pos = m_lstItem.GetHeadPosition();
	while (pos) {
		PRADIOLIST_ITEM pItem = m_lstItem.GetNext(pos);
		pItem->bChecked = bCheck;
	}

	Invalidate();
}

void CRadioListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	CWnd::OnMouseMove(nFlags, point);
}

void CRadioListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nItem = HitTest(point);
	if (nItem == -1)
		return;

	SetAllItemCheck(FALSE);
	SetCheck(nItem, TRUE);

	if (m_hNotifyWnd) {
		::PostMessage((HWND)m_hNotifyWnd, m_dwNotifyMsg, (WPARAM)m_nControlID, nItem);
	}
	
//	CWnd::OnLButtonDown(nFlags, point);
}

void CRadioListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnLButtonUp(nFlags, point);
}

void CRadioListBox::OnPaint()
{
	CPaintDC dc(this);

	CDC dcMem;
	if (!dcMem.CreateCompatibleDC(&dc))
		return;

	CBitmap bm;
	bm.CreateCompatibleBitmap(&dc, m_dwWidth, m_nListHeight);
	dcMem.SelectObject(&bm);

	if (m_ftText.GetSafeHandle())
		dcMem.SelectObject(&m_ftText);

	dcMem.SetTextColor(m_crText);
	dcMem.SetBkColor(m_crBk);

	int nTop = 0;

	POSITION pos = m_lstItem.GetHeadPosition();
	while (pos) {
		PRADIOLIST_ITEM pItem = m_lstItem.GetNext(pos);

		CRect rcItem(0, nTop, m_dwWidth, nTop + pItem->nHeight);
		DrawItem(&dcMem, &rcItem, pItem);

		nTop += pItem->nHeight;
	}

	int nScrollPos = GetScrollPos(SB_VERT);

	dc.BitBlt(0, 0, m_dwWidth, m_dwHeight,
		&dcMem, 0, nScrollPos, SRCCOPY);
}

int CRadioListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	UpdateScrollInfo();

	return 0;
}

void CRadioListBox::OnClose()
{
	CWnd::OnClose();
}

BOOL CRadioListBox::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	pDC->FillSolidRect(&rcClient, m_crBk);

	return TRUE;
}

void CRadioListBox::DrawItem(CDC* pDC, RECT* pRect, PRADIOLIST_ITEM pItem)
{
	// Draw Back Ground
	if (((m_dwStyleRLS & RLS_HAS_GRID) && ((pItem->nItem % 2) == 0)) ||
		((m_dwStyleRLS & RLS_HAS_GRID) == 0)) 
	{
		pDC->FillSolidRect(pRect, m_crBk);
	} else {
		pDC->FillSolidRect(pRect, RGB(255,255,255));
	}

	// Draw Focus
	if (pItem->bChecked && m_bDrawFocusRect) {
		pDC->DrawFocusRect(pRect);
	}

	// Draw Check Icon
	int nCheckIconLeft = 0;

	if (m_dwStyleRLS & RLS_HAS_RADIO) {
		nCheckIconLeft = ITEM_MARGIN;
		::DrawIconEx(pDC->GetSafeHdc(), pRect->left + nCheckIconLeft, pRect->top + ITEM_MARGIN, 
			pItem->bChecked ? m_hCheck : m_hUnCheck, 16, 16, 0, NULL, DI_NORMAL);
	}

	// Draw Item Icon

	int nItemIconLeft = 0;
	if (m_pImgList) {
		if (m_dwStyleRLS & RLS_HAS_RADIO) {
			nItemIconLeft = nCheckIconLeft + 16 + ITEM_MARGIN;
		}
		else
		{
			nItemIconLeft = ITEM_MARGIN;
		}

		if (pItem->nItemImage != -1) {
			CPoint pt(pRect->left + nItemIconLeft, pRect->top + ITEM_MARGIN);
			m_pImgList->Draw(pDC, pItem->nItemImage, pt, ILD_TRANSPARENT);
		}
	}

	// Draw Item Text
	int nItemTextLeft = ITEM_MARGIN;
	if (nCheckIconLeft > 0)
		nItemTextLeft += 16 + ITEM_MARGIN;
	if (nItemIconLeft > 0)
		nItemTextLeft += m_szImage.cx + ITEM_MARGIN;

	CRect rcItemText(pRect->left + nItemTextLeft, pRect->top + ITEM_MARGIN, pRect->right - ITEM_MARGIN, pRect->top + ITEM_MARGIN);
	pDC->DrawText(pItem->strItem, pItem->strItem.GetLength(), &rcItemText, DT_WORDBREAK|DT_NOPREFIX|DT_CALCRECT);

	int nItemTextHeight = rcItemText.Height();
	int nItemHeight = max(max(16, m_szImage.cy), nItemTextHeight);
	rcItemText.top += (nItemHeight - nItemTextHeight) / 2;
	rcItemText.bottom = rcItemText.top + nItemTextHeight;

	pDC->DrawText(pItem->strItem, pItem->strItem.GetLength(), &rcItemText, DT_WORDBREAK|DT_NOPREFIX);

	if (pItem->strSubItem.IsEmpty())
		return;

	// Draw SubItem Icon
	if (m_pImgList && (pItem->nSubItemImage != -1)) {
		CPoint pt(pRect->left + nItemIconLeft, pRect->top + ITEM_MARGIN + nItemHeight + ITEM_MARGIN);
		m_pImgList->Draw(pDC, pItem->nSubItemImage, pt, ILD_TRANSPARENT);
	}

	// Draw SubItem Text
	CRect rcSubItemText(pRect->left + nItemTextLeft, pRect->top + ITEM_MARGIN + nItemHeight + ITEM_MARGIN,
		pRect->right - ITEM_MARGIN, pRect->bottom - ITEM_MARGIN);
	pDC->DrawText(pItem->strSubItem, pItem->strSubItem.GetLength(), &rcSubItemText, DT_WORDBREAK|DT_NOPREFIX|DT_CALCRECT);

	int nSubItemTextHeight = rcSubItemText.Height();
	//int nSubItemHeight = max(max(16, m_szImage.cy), nSubItemTextHeight);
	rcSubItemText.top += (nItemHeight - nItemTextHeight) / 2;
	rcSubItemText.bottom = rcSubItemText.top + nSubItemTextHeight;

	pDC->DrawText(pItem->strSubItem, pItem->strSubItem.GetLength(), &rcSubItemText, DT_WORDBREAK|DT_NOPREFIX);
}

int CRadioListBox::GetListHeight()
{
	return m_nListHeight;
}

void CRadioListBox::OnDestroy()
{
	CWnd::OnDestroy();

	POSITION pos = m_lstItem.GetHeadPosition();
	while (pos) {
		PRADIOLIST_ITEM pItem = m_lstItem.GetNext(pos);
		delete pItem;
	}	
}

void CRadioListBox::UpdateScrollInfo()
{
	//Set the scrolling info
	SCROLLINFO scrlinfo;
	scrlinfo.cbSize = sizeof(scrlinfo);

	scrlinfo.fMask = SIF_PAGE|SIF_RANGE;
	scrlinfo.nMax = m_nListHeight;
	scrlinfo.nMin = 0;
	scrlinfo.nPage = m_dwHeight + 1;
	scrlinfo.nPos = 0;

	SetScrollInfo(SB_VERT, &scrlinfo);
}
void CRadioListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nScrollPos = GetScrollPos(SB_VERT);
	int nLimit = GetScrollLimit(SB_VERT);

	int nScroll = nLimit;

	int SCROLL_AMT_Y = 50;

	switch(nSBCode) {
		case SB_LINEUP:      // Scroll up.      
		case SB_PAGEUP:
			if(nScrollPos <= 0)
			{
				return;
			}
			nScroll = min(nScrollPos,SCROLL_AMT_Y);
			SetScrollPos(SB_VERT,nScrollPos - nScroll);
			break;   
		case SB_LINEDOWN:   // Scroll down.
		case SB_PAGEDOWN:
			if(nScrollPos >= nLimit)
			{
				return;
			}
			nScroll = min(nScroll-nScrollPos,SCROLL_AMT_Y);
			SetScrollPos(SB_VERT,nScrollPos + nScroll);
		    break;
		case SB_THUMBPOSITION:
			{
				HWND hWndScroll;
				if ( pScrollBar == NULL )
					hWndScroll = m_hWnd;
				else
					hWndScroll = pScrollBar->m_hWnd;
				
				SCROLLINFO info;
				info.cbSize = sizeof(SCROLLINFO);
				info.fMask = SIF_TRACKPOS;
				::GetScrollInfo(hWndScroll, SB_VERT, &info);
				
				nPos = info.nTrackPos;

				int nScr = nScrollPos - nPos;
				
				SetScrollPos(SB_VERT,nPos);
			}
			break;
		case SB_THUMBTRACK:
			{
				HWND hWndScroll;
				if ( pScrollBar == NULL )
					hWndScroll = m_hWnd;
				else
					hWndScroll = pScrollBar->m_hWnd;
				
				SCROLLINFO info;
				info.cbSize = sizeof(SCROLLINFO);
				info.fMask = SIF_TRACKPOS;
				::GetScrollInfo(hWndScroll, SB_VERT, &info);
				
				nPos = info.nTrackPos;

				int nScr = nScrollPos - nPos;
				
				SetScrollPos(SB_VERT,nPos,FALSE);
			}
			break;
	}	
	
	Invalidate();
	UpdateWindow();	

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CRadioListBox::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_dwWidth = cx;
	m_dwHeight = cy;

	UpdateScrollInfo();
}

void CRadioListBox::SetDrawFocusRect(BOOL bOn)
{
	m_bDrawFocusRect = bOn;

	Invalidate();
}

int CRadioListBox::GetItemCount()
{
	return (int)m_lstItem.GetCount();
}