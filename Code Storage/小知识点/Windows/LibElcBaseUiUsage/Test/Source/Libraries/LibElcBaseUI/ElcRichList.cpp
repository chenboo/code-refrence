// ElcRichList.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ElcRichList.h"

#pragma warning ( disable : 4554 )


#define ITEM_VSPACE			3
#define ITEM_HMARGIN		5
#define ITME_BUTTON_HMARGIN 2
#define ITEM_IMAGE_MARGIN	2
#define SUBITEM_INDENT		16

#define WM_BUTTON_CKICKED		(WM_USER + 100)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CElcRichList

IMPLEMENT_DYNAMIC(CElcRichList, CWnd)

CElcRichList::CElcRichList()
{
	LOGFONT lf = {0};
	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	m_ftItem.CreateFontIndirect(&lf);
	lf.lfWeight = FW_BOLD;
	m_ftBoldItem.CreateFontIndirect(&lf);

	m_nItemHeight = abs(lf.lfHeight) + ITEM_VSPACE * 2;

	m_crListBk = RGB(255,255,255);
	m_crItemText = RGB(0,0,0);
	m_crSubItemText = RGB(0,0,0);
	m_crSelectItemBk = RGB(49,106,197);
	m_crItemA = RGB(230,238,249);
	m_crItemB = RGB(250,250,250);

	m_nHMargin = ITEM_HMARGIN;
	m_nSubItemIndent = SUBITEM_INDENT;

	m_pImageList = NULL;

	m_hNotifyWnd = NULL;
	m_nNotifyMsg = 0;

	m_bEnableRedraw = TRUE;

	m_bEnableTip = TRUE;

	m_dwListState = ELCRLS_ITEM_IMAGE|ELCRLS_SUBITEM_IMAGE|ELCRLS_EXPAND_BTN;
	m_szExpandImage = CSize(9,9);
}

CElcRichList::~CElcRichList()
{
	CItemArray::iterator it = m_arrItem.begin();
	for (; it != m_arrItem.end(); it++)
	{
		RemoveButtons(&it->arrButton);
	}

	m_ExpandImage.Detach();
}


BEGIN_MESSAGE_MAP(CElcRichList, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()

	ON_MESSAGE(WM_BUTTON_CKICKED, OnButtonClicked)
END_MESSAGE_MAP()



// CElcRichList message handlers



int CElcRichList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_ELCRL_IMAGE_LIST);

	if (m_ExpandImage.GetSafeHandle())
		m_ExpandImage.Detach();

	m_ExpandImage.Create(9, 9, ILC_MASK | ILC_COLOR24, 0, 2);	
	m_ExpandImage.Add(&bitmap, RGB(255,0,255));

	IMAGEINFO info = {0};
	if (m_ExpandImage.GetImageInfo(0, &info)) 
	{
		m_szExpandImage.cx = info.rcImage.right - info.rcImage.left;
		m_szExpandImage.cy = info.rcImage.bottom - info.rcImage.top;
	}

	return 0;
}

void CElcRichList::SetNotifyWindow(HWND hWnd, UINT nMsg)
{
	m_hNotifyWnd = hWnd;
	m_nNotifyMsg = nMsg;
}

void CElcRichList::OnDestroy()
{
	CWnd::OnDestroy();
}

BOOL CElcRichList::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CElcRichList::GetHitItem(CPoint point, int& nItem, int& nSubItem, BOOL& bHitExpand)
{
	if (m_dwListState & ELCRLS_EXPAND_BTN)
	{
		if (point.x >= m_nHMargin && point.x < m_nHMargin + m_szExpandImage.cx + ITEM_IMAGE_MARGIN)
			bHitExpand = TRUE;
	}
	else
	{
		bHitExpand = FALSE;
	}

	int nScrollPos = GetScrollPos(SB_VERT);
	if (nScrollPos%m_nItemHeight)
		nScrollPos = nScrollPos + (m_nItemHeight - nScrollPos%m_nItemHeight);
	nScrollPos = nScrollPos / m_nItemHeight;

	int nHitItem = point.y/m_nItemHeight + nScrollPos;
	int nTestItem = 0;

	int nMaxItem = GetListHeight() / m_nItemHeight;

	if (nMaxItem < nHitItem)
	{
		nItem = -1;
		nSubItem = -1;
		return;
	}

	CItemArray::iterator it = m_arrItem.begin();
	for (nItem=0; it != m_arrItem.end(); it ++, nTestItem ++, nItem++)
	{
		if (nTestItem == nHitItem)
		{
			nSubItem = -1;
			return;
		}

		if (it->dwState & ELCRLS_EXPAND && !it->arrSubItem.empty())
		{
			if (nHitItem <= nTestItem + (int)it->arrSubItem.size())
			{
				nSubItem = nHitItem-nTestItem-1;
				return;
			}
			else
			{
				nTestItem += (int)it->arrSubItem.size();
			}
		}
	}

	nItem = -1;
	nSubItem = -1;
	bHitExpand = FALSE;
}

void CElcRichList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	BOOL bHitExpand = FALSE;
	int nItem, nSubItem;
	GetHitItem(point, nItem, nSubItem, bHitExpand);

	if (bHitExpand)
		return;

	ELCRICHLIST list;
	list.code = ELCRLN_DBLCLK;
	list.nItem = nItem;
	list.nSubItem = nSubItem;
	list.nID = GetDlgCtrlID();

	if (m_hNotifyWnd && m_nNotifyMsg)
		::SendMessage(m_hNotifyWnd, m_nNotifyMsg, list.code, (LPARAM)&list);

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CElcRichList::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL bHitExpand = FALSE;
	int nItem, nSubItem;
	GetHitItem(point, nItem, nSubItem, bHitExpand);

	if (bHitExpand)
	{
		if (nItem != -1 && nSubItem == -1)
		{
			ExpandItem(nItem, (m_arrItem[nItem].dwState & ELCRLS_EXPAND) == 0);
			return;
		}
	}

	CItemArray::iterator it = m_arrItem.begin();
	for (; it != m_arrItem.end(); it ++)
	{
		it->dwState &= ~ELCRLS_SELECT;
	}

	if (nItem != -1)
	{
		m_arrItem[nItem].dwState |= ELCRLS_SELECT;
	}

	ELCRICHLIST list;
	list.code = ELCRLN_SELECT;
	list.nItem = nItem;
	list.nSubItem = nSubItem;
	list.nID = GetDlgCtrlID();

	if (m_hNotifyWnd && m_nNotifyMsg)
		::SendMessage(m_hNotifyWnd, m_nNotifyMsg, list.code, (LPARAM)&list);

	Invalidate();

	CWnd::OnLButtonDown(nFlags, point);
}

void CElcRichList::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
}

void CElcRichList::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
}

inline void CElcRichList::Repaint()
{
	if (::IsWindow(m_hWnd))
		Invalidate();
}

void CElcRichList::OnPaint()
{
	if (!m_bEnableRedraw)
		return;

	CPaintDC dc(this);

	CBitmap bm;
	CDC dcMem;
	bm.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height());
	dcMem.CreateCompatibleDC(&dc);
	dcMem.SelectObject(&bm);
	dcMem.SetBkMode(TRANSPARENT);

	dcMem.FillSolidRect(m_rcClient, m_crListBk);

	DrawItem(&dcMem);

	dc.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), 
		&dcMem, 0, 0, SRCCOPY);
}

void CElcRichList::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_rcClient = CRect(0,0,cx,cy);
	if (m_rcClient.IsRectEmpty())
		return;

	UpdateScrollInfo();
}

WORD CElcRichList::GetButtonsHiId()
{
	while (1) {
		WORD wHiId = LOWORD(GetTickCount());
		BOOL bAvailable = TRUE;
		for (CItemArray::iterator it = m_arrItem.begin();
			it != m_arrItem.end();
			it ++)
		{
			if (it->wBtnsHiId == wHiId) {
				bAvailable = FALSE;
				break;
			}
		}

		if (bAvailable)
			return wHiId;
	};

	return 0;
}

int CElcRichList::InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
{
	if (nItem < 0) {
		nItem = 0;
	}
	else if (nItem >= (int)m_arrItem.size()) {
		nItem = (int)m_arrItem.size();
	}

	ELCRLIST_ITEM item;
	item.dwData = 0;
	item.nImage = nImage;
	item.strItem = lpszItem;
	item.dwState = 0;
	item.crText = m_crItemText;
	item.wBtnsHiId = LOWORD(GetTickCount());

	item.nExpandHeight = m_nItemHeight;

	if (m_arrItem.empty())
	{
		m_arrItem.push_back(item);
	}
	else
	{
		m_arrItem.insert(m_arrItem.begin() + nItem, item);
	}

	UpdateScrollInfo();

	return nItem;
}

BOOL CElcRichList::DeleteItem(int nItem)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return FALSE;

	if (!m_arrItem[nItem].arrButton.empty())
	{
		RemoveButtons(&m_arrItem[nItem].arrButton);
	}

	m_arrItem.erase(m_arrItem.begin() + nItem);

	UpdateScrollInfo();

	return TRUE;
}

void CElcRichList::DeleteAllItem()
{
	CItemArray::iterator it = m_arrItem.begin();
	for (; it != m_arrItem.end(); it++)
	{
		RemoveButtons(&it->arrButton);
	}

	m_arrItem.clear();

	UpdateScrollInfo();
}

int CElcRichList::GetItemCount()
{
	return (int)m_arrItem.size();
}

BOOL CElcRichList::AddSubItem(int nItem, LPCTSTR lpszText, int nImage)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return FALSE;

	LIST_SUBITEM subitem;
	subitem.nImage = nImage;
	subitem.strText = lpszText;
	subitem.crText = m_crSubItemText;

	m_arrItem[nItem].arrSubItem.push_back(subitem);

	m_arrItem[nItem].nExpandHeight += m_nItemHeight;

	UpdateScrollInfo();

	return TRUE;
}

BOOL CElcRichList::DeleteSubItem(int nItem, int nSubItem)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return FALSE;

	CSubItemArray* pSubItem = &m_arrItem[nItem].arrSubItem;
	if (pSubItem->empty() || nSubItem >= (int)pSubItem->size())
		return FALSE;

	m_arrItem[nItem].nExpandHeight -= m_nItemHeight;

	pSubItem->erase(pSubItem->begin() + nSubItem);

	UpdateScrollInfo();

	return TRUE;
}

BOOL CElcRichList::DeleteAllSubItem(int nItem)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return FALSE;

	int nSubItemCount = (int)m_arrItem[nItem].arrSubItem.size();
	if (nSubItemCount == 0)
		return TRUE;

	m_arrItem[nItem].nExpandHeight -= m_nItemHeight * nSubItemCount;
	m_arrItem[nItem].arrSubItem.clear();

	UpdateScrollInfo();

	return TRUE;
}

int CElcRichList::GetSubItemCount(int nItem)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return -1;

	return (int)m_arrItem[nItem].arrSubItem.size();
}

BOOL CElcRichList::SetItemData(int nItem, DWORD_PTR dwData)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return FALSE;

	m_arrItem[nItem].dwData = dwData;

	return TRUE;
}

DWORD_PTR CElcRichList::GetItemData(int nItem)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return 0;

	return m_arrItem[nItem].dwData;
}

void CElcRichList::ExpandItem(int nItem, BOOL bExpand)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return;

	if (m_arrItem[nItem].arrSubItem.empty())
		return;

	if (bExpand)
		m_arrItem[nItem].dwState |= ELCRLS_EXPAND;
	else
		m_arrItem[nItem].dwState &= ~ELCRLS_EXPAND;

	UpdateScrollInfo();
}

void CElcRichList::ExpandAll(BOOL bExpand)
{
	DWORD dwState = ELCRLS_EXPAND;
	if (!bExpand)
		dwState = (DWORD)(~ELCRLS_EXPAND);

	CItemArray::iterator it = m_arrItem.begin();
	for (; it != m_arrItem.end(); it ++)
	{
		if (bExpand)
			it->dwState |= ELCRLS_EXPAND;
		else
			it->dwState &= ~ELCRLS_EXPAND;
	}

	UpdateScrollInfo();
}

void CElcRichList::SetItemText(int nItem, LPCTSTR lpszText)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return;

	m_arrItem[nItem].strItem = lpszText;

	Repaint();
}

CString CElcRichList::GetItemText(int nItem)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return _T("");

	return m_arrItem[nItem].strItem;
}

void CElcRichList::SetSubItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return;

	if (nSubItem >= (int)m_arrItem[nItem].arrSubItem.size() || nSubItem < 0)
		return;

	m_arrItem[nItem].arrSubItem[nSubItem].strText = lpszText;

	Repaint();
}

CString CElcRichList::GetItemText(int nItem, int nSubItem)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return _T("");

	if (nSubItem >= (int)m_arrItem[nItem].arrSubItem.size() || nSubItem < 0)
		return _T("");

	return m_arrItem[nItem].arrSubItem[nSubItem].strText;
}

void CElcRichList::SetItemState(int nItem, DWORD dwState, DWORD dwMask)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return;

	m_arrItem[nItem].dwState &= ~dwMask;
	m_arrItem[nItem].dwState |= (dwState & dwMask);

	UpdateScrollInfo();
}

DWORD CElcRichList::GetItemState(int nItem, DWORD dwMask)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return 0;

	return (m_arrItem[nItem].dwState & dwMask);
}

BOOL CElcRichList::SetItemImage(int nItem, int nImage)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return FALSE;

	m_arrItem[nItem].nImage = nImage;

	Repaint();

	return TRUE;
}

int CElcRichList::GetItemImage(int nItem)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return -1;

	return m_arrItem[nItem].nImage;
}

BOOL CElcRichList::SetSubItemImage(int nItem, int nSubItem, int nImage)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return FALSE;

	if (nSubItem >= (int)m_arrItem[nItem].arrSubItem.size() || nSubItem < 0)
		return FALSE;

	m_arrItem[nItem].arrSubItem[nSubItem].nImage = nImage;

	Repaint();

	return TRUE;
}

int CElcRichList::GetSubItemImage(int nItem, int nSubItem)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return -1;

	if (nSubItem >= (int)m_arrItem[nItem].arrSubItem.size() || nSubItem < 0)
		return -1;

	return m_arrItem[nItem].arrSubItem[nSubItem].nImage;
}

BOOL CElcRichList::SetItemTextColor(int nItem, COLORREF color, BOOL bReDraw)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return FALSE;

	m_arrItem[nItem].crText = color;

	if (bReDraw)
		Repaint();

	return TRUE;
}

BOOL CElcRichList::SetSubItemTextColor(int nItem, int nSubItem, COLORREF color, BOOL bReDraw)
{
	if (nItem >= (int)m_arrItem.size() || nItem < 0)
		return FALSE;

	if (nSubItem >= (int)m_arrItem[nItem].arrSubItem.size() || nSubItem < 0)
		return FALSE;

	m_arrItem[nItem].arrSubItem[nSubItem].crText = color;

	if (bReDraw)
		Repaint();

	return TRUE;
}

void CElcRichList::DrawItem(CDC* pDC)
{
	CFont* pFont = pDC->SelectObject(&m_ftItem);

	int nOffsetPos = 0;
	int nScrollPos = GetScrollPos(SB_VERT);
	if (nScrollPos%m_nItemHeight)
		nScrollPos = nScrollPos + (m_nItemHeight - nScrollPos%m_nItemHeight);

	CRect rect;
	rect.top = 0;
	rect.left = m_nHMargin;
	rect.right = m_rcClient.right - m_nHMargin;

	CItemArray::iterator it = m_arrItem.begin();
	for (; it != m_arrItem.end(); it ++)
	{
		if (nOffsetPos + m_nItemHeight <= nScrollPos)
		{
			nOffsetPos += m_nItemHeight;

			if (!(it->dwState & ELCRLS_EXPAND) || it->arrSubItem.empty())
			{
				HideButtons(&it->arrButton);
				continue;
			}

			HideButtons(&it->arrButton);

			rect.left = m_nHMargin + m_nSubItemIndent;

			if (m_dwListState & ELCRLS_EXPAND_BTN)
			{
				rect.left += m_szExpandImage.cx + ITEM_IMAGE_MARGIN;
			}

			CSubItemArray::iterator itSub = it->arrSubItem.begin();
			for (; itSub != it->arrSubItem.end(); itSub ++)
			{
				if (nOffsetPos + m_nItemHeight <= nScrollPos)
				{
					nOffsetPos += m_nItemHeight;
					continue;
				}
				else
				{
					CRect rcItemBk = m_rcClient;
					rcItemBk.top = rect.top;
					rcItemBk.bottom = rect.top + m_nItemHeight;

					if ((it - m_arrItem.begin()) % 2)
						pDC->FillSolidRect(rcItemBk, m_crItemB);
					else
						pDC->FillSolidRect(rcItemBk, m_crItemA);

					rect.bottom = rect.top + m_nItemHeight;
					DrawSubItem(pDC, (PLIST_SUBITEM)&(*itSub), rect);
					rect.top = rect.bottom;
				}
			}

			rect.left = m_nHMargin;

			continue;
		}

		rect.bottom = rect.top + m_nItemHeight;

		// fill item background
		CRect rcItemBk = m_rcClient;
		rcItemBk.top = rect.top;
		rcItemBk.bottom = rect.top + m_nItemHeight;
		if (it->dwState & ELCRLS_EXPAND)
			rcItemBk.bottom += (int)it->arrSubItem.size() * m_nItemHeight;

		COLORREF crItemBk = m_crItemA;
		if ((it - m_arrItem.begin()) % 2)
			crItemBk = m_crItemB;
		pDC->FillSolidRect(rcItemBk, crItemBk);

		// set buttons' pos
		if (!it->arrButton.empty())
		{
			CRect rcBtn = rect;
			BOOL bHasButtonShow = FALSE;
			for (int i=(int)it->arrButton.size()-1; i>=0; i--)
			{
				if (!it->arrButton[i].bShow)
				{
					it->arrButton[i].pBtn->ShowWindow(SW_HIDE);
					continue;
				}

				rcBtn.left = rcBtn.right - it->arrButton[i].rcBtn.Width();
				rcBtn.top = rect.top + (rect.Height() - it->arrButton[i].rcBtn.Height())/2;
				rcBtn.bottom = rcBtn.top + it->arrButton[i].rcBtn.Height();

				it->arrButton[i].pBtn->SetBackgndColor(crItemBk);
				it->arrButton[i].pBtn->MoveWindow(rcBtn);
				it->arrButton[i].pBtn->ShowWindow(SW_SHOW);

				rcBtn.right = rcBtn.left - ITME_BUTTON_HMARGIN;

				bHasButtonShow = TRUE;
			}

			if (bHasButtonShow)
				rect.right = rcBtn.left - ITME_BUTTON_HMARGIN;
		}

		// draw expand / fold iamge
		if (m_dwListState & ELCRLS_EXPAND_BTN)
		{			
			CPoint pt;
			pt.x = rect.left;
			pt.y = rect.top + (m_nItemHeight-m_szExpandImage.cy)/2;

			if (!it->arrSubItem.empty())
			{
				int nImage = it->dwState & ELCRLS_EXPAND ? ELCRLS_EI_FOLD : ELCRLS_EI_EXPAND;
				m_ExpandImage.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
			}

			rect.left += m_szExpandImage.cx + ITEM_IMAGE_MARGIN;
		}

		// draw image
		if (m_pImageList && (m_dwListState & ELCRLS_ITEM_IMAGE))
		{
			CPoint pt;
			pt.x = rect.left;
			pt.y = rect.top + (m_nItemHeight-m_szImage.cy)/2;
			m_pImageList->Draw(pDC, it->nImage, pt, ILD_TRANSPARENT);

			rect.left += m_szImage.cx + ITEM_IMAGE_MARGIN;
		}

		// draw select item's background
		COLORREF crText = it->crText;
		if (it->dwState & ELCRLS_SELECT)
		{
			crText = RGB(255,255,255);
			pDC->FillSolidRect(rect, m_crSelectItemBk);
		}

		// draw item text
		if (it->dwState & ELCRLS_BOLD)
		{
			pDC->SelectObject(&m_ftBoldItem);
		}
		else
		{
			pDC->SelectObject(&m_ftItem);
		}

		pDC->SetTextColor(crText);
		pDC->DrawText(it->strItem, rect, DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		rect.top = rect.bottom;


		// draw sub item
		if (it->dwState & ELCRLS_EXPAND)
		{
			rect.top = DrawSubItem(pDC, &it->arrSubItem, rect.top);
		}


		rect.left = m_nHMargin;
		rect.right = m_rcClient.right - m_nHMargin;

		if (rect.top  >= m_rcClient.bottom)
			break;
	}

	pDC->SelectObject(pFont);
}

int CElcRichList::DrawSubItem(CDC* pDC, CSubItemArray* pSubItem, int nTop)
{
	CFont* pFont = pDC->SelectObject(&m_ftItem);

	CRect rect;
	rect.left = m_nHMargin + m_nSubItemIndent;
	rect.right = m_rcClient.right - m_nHMargin;
	rect.top = nTop;

	if (m_dwListState & ELCRLS_EXPAND_BTN)
	{
		rect.left += m_szExpandImage.cx + ITEM_IMAGE_MARGIN;
	}

	CSubItemArray::iterator it = pSubItem->begin();
	for (; it != pSubItem->end(); it ++)
	{
		rect.bottom = rect.top + m_nItemHeight;

		DrawSubItem(pDC, (PLIST_SUBITEM)&(*it), rect);

		rect.top = rect.bottom;
		if (rect.top >= m_rcClient.bottom)
			break;
	}

	pDC->SelectObject(pFont);

	return rect.top;
}

void CElcRichList::DrawSubItem(CDC* pDC, PLIST_SUBITEM pItem, CRect rect)
{
	if (m_pImageList && (m_dwListState & ELCRLS_SUBITEM_IMAGE))
	{
		CPoint pt;
		pt.x = rect.left;
		pt.y = rect.top + (m_nItemHeight-m_szImage.cy)/2;
		m_pImageList->Draw(pDC, pItem->nImage, pt, ILD_TRANSPARENT);

		rect.left += m_szImage.cx + ITEM_IMAGE_MARGIN;
	}

	pDC->SetTextColor(pItem->crText);
	pDC->DrawText(pItem->strText, rect, DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
}

CImageList* CElcRichList::SetImageList(CImageList* pImageList)
{
	CImageList* pOldImgList = m_pImageList;
	m_pImageList = pImageList;

	IMAGEINFO info = {0};
	if (m_pImageList->GetImageInfo(0, &info)) 
	{
		m_szImage.cx = info.rcImage.right - info.rcImage.left;
		m_szImage.cy = info.rcImage.bottom - info.rcImage.top;
	}

	return pOldImgList;
}

CImageList* CElcRichList::GetImageList() const
{
	return m_pImageList;
}

void CElcRichList::UpdateScrollInfo()
{
	SCROLLINFO scrlinfo;
	scrlinfo.cbSize = sizeof(scrlinfo);

	scrlinfo.fMask = SIF_PAGE|SIF_RANGE;
	scrlinfo.nMax = GetListHeight();
	scrlinfo.nMin = 0;
	scrlinfo.nPage = m_rcClient.Height();
	scrlinfo.nPos = 0;

	SetScrollInfo(SB_VERT, &scrlinfo);

	HideAllButtons();

	Invalidate();
}

int CElcRichList::GetListHeight()
{
	int nListHeight = 0;

	CItemArray::iterator it = m_arrItem.begin();
	for (; it != m_arrItem.end(); it ++)
	{
		nListHeight += m_nItemHeight;
		if (it->dwState & ELCRLS_EXPAND)
		{
			nListHeight += m_nItemHeight * (int)it->arrSubItem.size();
		}
	}

	return nListHeight;
}

void CElcRichList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

				SetScrollPos(SB_VERT,nPos,FALSE);
			}
			break;
	}	

	HideAllButtons();

	InvalidateRect(m_rcClient);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CElcRichList::SetBkColor(COLORREF color)
{
	m_crListBk = color;

	Repaint();
}

COLORREF CElcRichList::GetBkColor()
{
	return m_crListBk;
}

void CElcRichList::SetItemTextColor(COLORREF color)
{
	m_crItemText = color;

	Repaint();
}

COLORREF CElcRichList::GetItemTextColor()
{
	return m_crItemText;
}

void CElcRichList::SetSubItemTextColor(COLORREF color)
{
	m_crSubItemText = color;

	Repaint();
}

COLORREF CElcRichList::GetSubItemTextColor()
{
	return m_crSubItemText;
}

void CElcRichList::SetSelectedItemBkColor(COLORREF color)
{
	m_crSelectItemBk = color;

	Repaint();
}

COLORREF CElcRichList::GetSelectedItemBkColor()
{
	return m_crSelectItemBk;
}

void CElcRichList::SetItemFont(CFont* pFont)
{
	LOGFONT lf = { 0 };
	pFont->GetLogFont(&lf);

	if (m_ftItem.GetSafeHandle())
		m_ftItem.DeleteObject();
	if (m_ftBoldItem.GetSafeHandle())
		m_ftBoldItem.DeleteObject();

	m_ftItem.CreateFontIndirect(&lf);
	lf.lfWeight = FW_BOLD;
	m_ftBoldItem.CreateFontIndirect(&lf);
}

CFont* CElcRichList::GetItemFont()
{
	return &m_ftItem;
}

void CElcRichList::SetHMargin(int nMargin)
{
	m_nHMargin = nMargin;

	Repaint();
}

int CElcRichList::GetHMargin()
{
	return m_nHMargin;
}

void CElcRichList::SetSubItemIndent(int nIndent)
{
	m_nSubItemIndent = nIndent;

	Repaint();
}

int CElcRichList::GetSubItemIndent()
{
	return m_nSubItemIndent;
}

void CElcRichList::SetRedraw(BOOL bRedraw)
{
	m_bEnableRedraw = bRedraw;

	if (m_bEnableRedraw)
	{
		Repaint();
	}
}

int CElcRichList::FindButton(int nItem, UINT nBtnID)
{
	if (nItem < 0 || nItem >= (int)m_arrItem.size())
		return -1;

	// convert button's id
	nBtnID = MAKELONG(nBtnID, m_arrItem[nItem].wBtnsHiId);
	for (int i=0; i<(int)m_arrItem[nItem].arrButton.size(); i++)
	{
		if (nBtnID == m_arrItem[nItem].arrButton[i].nID)
			return i;
	}

	return -1;
}

BOOL CElcRichList::FindButtonInItem(UINT nBtnID, int& nItem)
{
	for (nItem=0; nItem<(int)m_arrItem.size(); nItem++)
	{
		for (int i=0; i<(int)m_arrItem[nItem].arrButton.size(); i++)
		{
			if (nBtnID == m_arrItem[nItem].arrButton[i].nID)
				return TRUE;
		}
	}

	nItem = -1;
	return FALSE;
}

BOOL CElcRichList::AddButton(int nItem, UINT nBtnID, UINT nBitmapID, LPCTSTR lpszTip, BOOL bShow)
{
	return AddButton(nItem, nBtnID, nBitmapID, RT_BITMAP, _T(""), lpszTip, bShow);
}

BOOL CElcRichList::AddButton(int nItem, UINT nBtnID, UINT nResId, LPCTSTR lpszType, 
							 LPCTSTR lpszBtnText, LPCTSTR lpszTip, BOOL bShow)
{
	if (nItem < 0 || nItem >= (int)m_arrItem.size())
		return FALSE;

	ELCLIST_BUTTON button;

	if ((nBtnID & 0xFFFF0000) != 0) {
		// nBtnID must less than 0x0000FFFF
		ASSERT(0);
		return FALSE;
	}
	else {
		button.nID = MAKELONG(nBtnID, m_arrItem[nItem].wBtnsHiId);
	}
	button.bShow = bShow;
	button.pBtn = new CBarButtonEx;

	// create button
	if (!button.pBtn->Create(lpszBtnText, WS_CHILD, CRect(0,0,0,0), this, button.nID))
		return FALSE;

	button.pBtn->SetIconImage(nResId, lpszType);
	CSize size = button.pBtn->GetIdealSize();
	button.rcBtn = CRect(0,0,size.cx,size.cy);

	button.pBtn->SetNotifyWindow(GetSafeHwnd(), WM_BUTTON_CKICKED);
	button.pBtn->MoveWindow(button.rcBtn);

	// add tip
	if (!::IsWindow(m_wndToolTip.GetSafeHwnd())) {
		m_wndToolTip.Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX);
		m_wndToolTip.Activate(TRUE);
		m_wndToolTip.SetDelayTime(500);
	}

	if (lpszTip != NULL)
		m_wndToolTip.AddTool(button.pBtn, lpszTip);

	// add to the array
	m_arrItem[nItem].arrButton.push_back(button);

	return TRUE;
}

BOOL CElcRichList::AddTextlinkerButton(int nItem, 
									   UINT nBtnID, 
									   LPCTSTR lpszText, 
									   int nUnderlineStyle, 
									   COLORREF 
									   crNormal, 
									   COLORREF crHot, 
									   BOOL bAnimateDown)
{
	if (nItem < 0 || nItem >= (int)m_arrItem.size())
		return FALSE;

	ELCLIST_BUTTON button;

	if ((nBtnID & 0xFFFF0000) != 0) {
		// nBtnID must less than 0x0000FFFF
		ASSERT(0);
		return FALSE;
	}
	else {
		button.nID = MAKELONG(nBtnID, m_arrItem[nItem].wBtnsHiId);
	}
	button.bShow = TRUE;
	button.pBtn = new CBarButtonEx;

	// create button
	if (!button.pBtn->Create(lpszText, WS_CHILD, CRect(0,0,0,0), this, button.nID))
		return FALSE;

	button.pBtn->SetAnimateDown(bAnimateDown);
	CSize size = button.pBtn->SetTextLinkerButton(lpszText, nUnderlineStyle, crNormal, crHot);;
	button.rcBtn = CRect(0,0,size.cx,size.cy);

	button.pBtn->SetNotifyWindow(GetSafeHwnd(), WM_BUTTON_CKICKED);
	button.pBtn->MoveWindow(button.rcBtn);

	// add to the array
	m_arrItem[nItem].arrButton.push_back(button);

	return TRUE;

}

BOOL CElcRichList::DelButton(int nItem, UINT nBtnID)
{
	int nButton = FindButton(nItem, nBtnID);
	if (nButton == -1)
		return FALSE;

	delete m_arrItem[nItem].arrButton[nButton].pBtn;

	m_arrItem[nItem].arrButton.erase(m_arrItem[nItem].arrButton.begin() + nButton);

	Repaint();

	return TRUE;
}

BOOL CElcRichList::ShowButton(int nItem, UINT nBtnID, BOOL bShow)
{
	int nButton = FindButton(nItem, nBtnID);
	if (nButton == -1)
		return FALSE;

	m_arrItem[nItem].arrButton[nButton].bShow = bShow;

	Repaint();

	return TRUE;
}

BOOL CElcRichList::EnableButton(int nItem, UINT nBtnID, BOOL bEnable)
{
	int nButton = FindButton(nItem, nBtnID);
	if (nButton == -1)
		return FALSE;

	m_arrItem[nItem].arrButton[nButton].pBtn->EnableWindow(bEnable);

	return TRUE;
}

BOOL CElcRichList::SetButtonText(int nItem, UINT nBtnID, LPCTSTR lpszText)
{
	int nButton = FindButton(nItem, nBtnID);
	if (nButton == -1)
		return FALSE;

	m_arrItem[nItem].arrButton[nButton].pBtn->SetText(lpszText);
	CSize size = m_arrItem[nItem].arrButton[nButton].pBtn->GetIdealSize();

	CRect * pRc = &m_arrItem[nItem].arrButton[nButton].rcBtn;
	pRc->right = pRc->left + size.cx;
	pRc->bottom = pRc->top + size.cy;

	Repaint();

	return TRUE;
}

CString CElcRichList::GetButtonText(int nItem, UINT nBtnID)
{
	int nButton = FindButton(nItem, nBtnID);
	if (nButton == -1)
		return _T("");

	return m_arrItem[nItem].arrButton[nButton].pBtn->GetText();
}

void CElcRichList::HideAllButtons()
{
	CItemArray::iterator it = m_arrItem.begin();
	for (; it != m_arrItem.end(); it++)
	{
		HideButtons(&it->arrButton);
	}
}

void CElcRichList::HideButtons(CButtonArray* parrButton)
{
	if (!parrButton)
		return;

	CButtonArray::iterator it = parrButton->begin();
	for (; it != parrButton->end(); it ++)
	{
		it->pBtn->ShowWindow(SW_HIDE);
	}
}

void CElcRichList::RemoveButtons(CButtonArray* parrButton)
{
	CButtonArray::iterator it = parrButton->begin();
	for (; it != parrButton->end(); it ++)
	{
		if (it->pBtn)
		{
			delete it->pBtn;
			it->pBtn = NULL;
		}
	}
}

BOOL CElcRichList::PreTranslateMessage(MSG* pMsg)
{
	if (m_bEnableTip && m_wndToolTip.GetSafeHwnd())
		m_wndToolTip.RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}

void CElcRichList::EnableToolTip(BOOL bEnable)
{
	m_bEnableTip = bEnable;

	if (bEnable)
	{
		if (!::IsWindow(m_wndToolTip.GetSafeHwnd()))
		{
			m_wndToolTip.Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX);
			m_wndToolTip.Activate(TRUE);
			m_wndToolTip.SetDelayTime(500);
		}
	}
}

LRESULT CElcRichList::OnButtonClicked(WPARAM wParam, LPARAM lParam)
{
	ELCRICHLIST_BUTTON_NOTIFY listnotify;
	listnotify.code = ELCRLN_BTN_CLICKED;
	listnotify.nSubItem = -1;
	listnotify.nID = GetDlgCtrlID();
	listnotify.nBtnID = LOWORD(wParam);;

	if (!FindButtonInItem((UINT)wParam, listnotify.nItem))
		return 0;

	if (m_hNotifyWnd && m_nNotifyMsg)
		::SendMessage(m_hNotifyWnd, m_nNotifyMsg, listnotify.code, (LPARAM)&listnotify);

	return 0;
}

void CElcRichList::SetItemImageShow(BOOL bShow)
{
	if (bShow)
		m_dwListState |= ELCRLS_ITEM_IMAGE;
	else
		m_dwListState &= ~ELCRLS_ITEM_IMAGE;

	Repaint();
}

BOOL CElcRichList::IsItemImageShow()
{
	return (m_dwListState & ELCRLS_ITEM_IMAGE > 0);
}

void CElcRichList::SetSubItemImageShow(BOOL bShow)
{
	if (bShow)
		m_dwListState |= ELCRLS_SUBITEM_IMAGE;
	else
		m_dwListState &= ~ELCRLS_SUBITEM_IMAGE;

	Repaint();
}

BOOL CElcRichList::IsSubItemImageShow()
{
	return (m_dwListState & ELCRLS_SUBITEM_IMAGE > 0);
}

void CElcRichList::SetExpandButtonShow(BOOL bShow)
{
	if (bShow)
		m_dwListState |= ELCRLS_EXPAND_BTN;
	else
		m_dwListState &= ~ELCRLS_EXPAND_BTN;

	Repaint();
}

BOOL CElcRichList::IsExpandButtonShow()
{
	return (m_dwListState & ELCRLS_EXPAND_BTN > 0);
}

void CElcRichList::SetItemABColor(COLORREF crItemA, COLORREF crItemB, BOOL bReDraw)
{
	m_crItemA = crItemA;
	m_crItemB = crItemB;

	if (bReDraw)
		Repaint();
}

void CElcRichList::GetItemABColor(COLORREF& crItemA, COLORREF& crItemB)
{
	crItemA = m_crItemA;
	crItemB = m_crItemB;
}

void CElcRichList::SetItemHeight(int nHeight)
{
	if (nHeight < ITEM_VSPACE + ITEM_VSPACE)
		nHeight = ITEM_VSPACE + ITEM_VSPACE;

	m_nItemHeight = nHeight;
}

int CElcRichList::GetItemHeight()
{
	return m_nItemHeight;
}
