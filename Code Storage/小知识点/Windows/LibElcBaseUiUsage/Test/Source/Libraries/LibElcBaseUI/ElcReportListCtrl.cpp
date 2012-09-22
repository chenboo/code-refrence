#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcReportListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_PROGRESS_WIDTH	30

PFNLVCOMPARE g_pfnSortProc = NULL;

IMPLEMENT_DYNAMIC(CElcReportListCtrl, CListCtrl)

CElcReportListCtrl::CElcReportListCtrl()
{
	memset(&m_skin, 0, sizeof(m_skin));

	m_skin.crText = RGB(21,66,139);
	m_skin.crItemA = RGB(241,244,248);
	m_skin.crItemB = RGB(255,255,255);
	m_skin.crSelectedBackgnd = RGB(206,236,250);
	m_skin.crTextSelected = RGB(21,66,139);

	m_crProgressBk = RGB(238,246,255);
	m_crProgressFill = RGB(2,174,249);
	m_crProgressBorder = RGB(48,146,244);
	m_crImageTransparentBk = RGB(255,255,255);

	m_nMinColumnWidth = -1;
	m_nItemHeight = 20;
	m_nProgressHeight = 12;

	m_pImageList = NULL;

	m_cxImage = 16;
	m_cyImage = 16;
	m_nHMargin = 5;

	m_sizeCheckBtn.SetSize(16,16);
	m_bEnableCheckBtn = FALSE;

	m_pImageListEx = NULL;
	m_cxUnitImagelistEx = 0;
	m_cyUnitImagelistEx = 0;

	LOGFONT lf = {0};
	GetMSShellDlgFont(lf);
	m_ftText.CreateFontIndirect(&lf);
	lf.lfWeight = FW_BOLD;
	m_ftTextBold.CreateFontIndirect(&lf);

	m_cchTip = 1024;
	m_pszTip = new wchar_t[m_cchTip];

	m_bMirror = FALSE;
	m_rcItem.SetRectEmpty();
}

CElcReportListCtrl::~CElcReportListCtrl()
{
	if (m_pszTip)
		delete m_pszTip;
}

BOOL CElcReportListCtrl::LoadSkin(LPCTSTR lpszStyle)
{
	PELC_SKIN_LIST pSkin = theSkin.GetListSkin(lpszStyle);
	if (!pSkin)
		return FALSE;

	memcpy(&m_skin, pSkin, sizeof(m_skin));
	m_SkinHeaderCtrl.LoadSkin(m_skin.szHeaderCtrlStyle);

	m_sizeCheckBtn = CElcSkinDoc::GetImageSize(&m_skin.checkbox);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CElcReportListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_NCPAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, &CElcReportListCtrl::OnToolTipText)
	ON_WM_MEASUREITEM_REFLECT()
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnLvnDeleteitem)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeyDown)
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CElcReportListCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();
	EnableToolTips();

	if (GetToolTips())
		GetToolTips()->SetMaxTipWidth(500);
}

int CElcReportListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bMirror = IsLayoutRTL();

	if (_tcslen(m_skin.szStyle) <= 0
		&& !LoadSkin(ELC_SKIN_STYLE_LIST_MAIN))
		return -1;

	m_SkinHeaderCtrl.SubclassWindow(CListCtrl::GetHeaderCtrl()->m_hWnd);

	if (GetToolTips())
		GetToolTips()->SetMaxTipWidth(500);

	ModifyStyle(WS_CLIPCHILDREN, 0);

	return 0;
}

void CElcReportListCtrl::OnPaint()
{
	if (GetItemCount() == 0) {
		CPaintDC dc(this);

		CRect rect, rcDraw;
		GetClientRect(rect);
		rcDraw = rect;

		CElcMemDC dcMem(&dc, rect);

		if (GetHeaderCtrl()) {
			CRect rcHeader;
			GetHeaderCtrl()->GetWindowRect(rcHeader);
			ScreenToClient(rcHeader);
			dcMem.ExcludeClipRect(rcHeader);
			rcDraw.top = rcHeader.bottom;
		}

		dcMem.FillSolidRect(rect, RGB(255, 255, 255));

		ELC_UI_DRAW draw;
		draw.hWnd = m_hWnd;
		draw.type = ELC_UI_DT_PREDRAW;
		draw.hdc = dcMem.GetSafeHdc();
		draw.rect = rcDraw;
		GetParent()->SendMessage(WM_ELC_UI_DRAW_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&draw);

		dcMem.DrawMe();
	}
	else {
		Default();
	}
}

void CElcReportListCtrl::OnNcPaint()
{
	Default();

	LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	if (lStyle & WS_BORDER) {
		CDC* pDC = GetWindowDC();
		if (pDC) {
			CRect rect;
			GetWindowRect(rect);
			rect.OffsetRect(-rect.left, -rect.top);
			CBrush br(m_skin.crBorder);
			pDC->FrameRect(rect, &br);
			ReleaseDC(pDC);
		}
	}
}

BOOL CElcReportListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	HD_NOTIFY *pHDN = (HD_NOTIFY*)lParam; 

	if (m_nMinColumnWidth > 0) {
		if((pHDN->hdr.code == HDN_ITEMCHANGING)
			&& pHDN-> pitem->cxy < m_nMinColumnWidth)
		{ 
			*pResult = TRUE;
			return TRUE;
		} 
	}

	return __super::OnNotify(wParam,   lParam,   pResult); 
}

void CElcReportListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_nItemHeight;
}

void CElcReportListCtrl::SetItemHeight(int nHeight) 
{
	if (m_nItemHeight == nHeight)
		return;

	m_nItemHeight = nHeight;

	if (::IsWindow(m_hWnd)) {
		CRect rect;
		GetClientRect(rect);
		MoveWindow(0,0,0,0);
		MoveWindow(rect);
	}
}

BOOL CElcReportListCtrl::OnEraseBkgnd(CDC* pDC)
{
	if (GetHeaderCtrl()) {
		CRect rcHeader;
		GetHeaderCtrl()->GetWindowRect(rcHeader);
		ScreenToClient(rcHeader);
		pDC->ExcludeClipRect(rcHeader);
	}

	return __super::OnEraseBkgnd(pDC);
}

void CElcReportListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (!pDC)
		return;

	int nColumnCount = GetHeaderCtrl()->GetItemCount();
	if (nColumnCount == 0)
		return;

	int *pColumnArray = new int[nColumnCount];
	GetHeaderCtrl()->GetOrderArray(pColumnArray, nColumnCount);

	PELC_REPORTLIST_ITEM_DATA pData = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(lpDrawItemStruct->itemID);

	m_rcItem = lpDrawItemStruct->rcItem;

	CBitmap bm;
	bm.CreateCompatibleBitmap(pDC, m_rcItem.Width(), m_rcItem.Height());

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	dcMem.SelectObject(&bm);
	dcMem.SetBkMode(TRANSPARENT);
	dcMem.SetTextColor(pData->iteminfo.crText);
	CFont *pOldfont = dcMem.SelectObject(pData->iteminfo.bBold ? &m_ftTextBold : &m_ftText);

	LVITEM lvi = {0};
	lvi.mask = LVIF_STATE;
	lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	lvi.iItem = lpDrawItemStruct->itemID;
	GetItem(&lvi);
	
	// draw background
	CRect rcBk = m_rcItem;
	rcBk.OffsetRect(-m_rcItem.left, -m_rcItem.top);

	if ((lvi.state & LVIS_DROPHILITED)
		|| ((lvi.state & LVIS_SELECTED) 
		&& ((GetFocus() == this) || (GetStyle() & LVS_SHOWSELALWAYS))))
	{
		dcMem.SetTextColor(m_skin.crTextSelected);
		dcMem.SetBkColor(m_skin.crSelectedBackgnd);
		dcMem.FillRect(rcBk, &CBrush(m_skin.crSelectedBackgnd));
	}
	else {
		dcMem.SetTextColor(pData->iteminfo.crText);
		COLORREF crBk = (lpDrawItemStruct->itemID & 1) == 0 ? m_skin.crItemA : m_skin.crItemB;
		dcMem.FillRect(rcBk, &CBrush(crBk));
	}

	// draw focus rect
	if (lvi.state & LVIS_FOCUSED) {
		rcBk.DeflateRect(1,1);
		dcMem.DrawFocusRect(rcBk);
	}
	
	// draw subitems
	PELC_REPORTLIST_ITEM_DATA pItem = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(lpDrawItemStruct->itemID);
	CRect rcSub;

	for (int i=0; i < nColumnCount; i ++) {

		CString strText = GetItemText(lpDrawItemStruct->itemID, pColumnArray[i]);
		GetSubItemRect(lpDrawItemStruct->itemID, pColumnArray[i], LVIR_BOUNDS, rcSub);

		CRect rcColumn;
		GetHeaderCtrl()->GetItemRect(pColumnArray[i], rcColumn);

		rcSub.OffsetRect(0, -m_rcItem.top);
		rcSub.left = rcColumn.left;
		rcSub.right = rcColumn.right;

		if (i == 0 && GetExtendedStyle() & LVS_EX_CHECKBOXES) {
			DrawCheckBtn(&dcMem, rcSub, GetCheck(lpDrawItemStruct->itemID));
			rcSub.left += m_nHMargin + m_sizeCheckBtn.cx;
		}

		DrawSubItem(&dcMem, rcSub, pItem->arrSubItem[pColumnArray[i]], strText);
	}

	delete []pColumnArray;

	dcMem.SelectObject(pOldfont);

	pDC->BitBlt(m_rcItem.left, 
		m_rcItem.top,
		m_rcItem.Width(),
		m_rcItem.Height(),
		&dcMem,
		0,
		0,
		SRCCOPY);
}

void CElcReportListCtrl::DrawCheckBtn(CDC *pDC, CRect rect, BOOL bChecked)
{
	if (!m_skin.checkbox.pImage)
		return;

	CRect rcDest;
	rcDest.left = rect.left + m_nHMargin;
	rcDest.top = rect.top + (rect.Height() - m_sizeCheckBtn.cy) / 2;
	rcDest.right = rcDest.left + m_sizeCheckBtn.cx;
	rcDest.bottom = rcDest.top + m_sizeCheckBtn.cy;

	if (m_bMirror) {
		rcDest = MirrorRect(m_rcItem.Width(), rcDest);
	}

	int nImageIndex = 0;
	if (bChecked)
		nImageIndex = 1;

	if (!m_bEnableCheckBtn)
		nImageIndex += 2;

	thePainter.DrawImageGDI(pDC->m_hDC,
		&m_skin.checkbox,
		rcDest,
		nImageIndex,
		0,
		0,
		0);
}

void CElcReportListCtrl::DrawSubItem(CDC *pDC, CRect rect, PELC_REPORT_LIST_SUBITEM pSubItem, LPCTSTR lpszText)
{
	if (!pSubItem) {
		rect.left += m_nHMargin;
	}
	else if (ERLST_IMAGE == pSubItem->nType) {
		rect.left = DrawImages(pDC, rect, (PELC_SUBITEM_IMAGE)pSubItem);
		if (rect.left == -1) {
			// not enough space to draw text, so return.
			return;
		}
	}
	else if (ERLST_PROGRESS == pSubItem->nType) {
		rect.left = DrawProgress(pDC, rect, (PELC_SUBITEM_PROGRESS)pSubItem, lpszText);
	}

	if (lpszText && _tcslen(lpszText) > 0) {
		UINT nDTFlag = DT_LEFT \
			| DT_SINGLELINE \
			| DT_VCENTER \
			| DT_END_ELLIPSIS \
			| DT_WORDBREAK \
			| DT_NOFULLWIDTHCHARBREAK \
			| DT_NOPREFIX;
		rect.right -= m_nHMargin;
		pDC->DrawText(lpszText, rect, nDTFlag);
	}
}

int CElcReportListCtrl::DrawImages(CDC *pDC, CRect rect, PELC_SUBITEM_IMAGE pImagesItem)
{
	POINT pt;
	pt.x = rect.left + m_nHMargin;
	pt.y = rect.top + (rect.Height() - m_cyImage) / 2;

	CDC dcImage;
	dcImage.CreateCompatibleDC(pDC);

	int nCount = (int)pImagesItem->arrImage.size();
	for (int i=0; i < nCount; i ++) 
	{
		if (pt.x + m_cxImage > rect.right)
			return -1;

		if (pImagesItem->arrImage[i].nImageType == ELCRLIT_IMAGELIST) {
			if (m_pImageList)
				m_pImageList->Draw(pDC, (int)pImagesItem->arrImage[i].dwImage, pt, ILD_TRANSPARENT);
		}
		else if (pImagesItem->arrImage[i].nImageType == ELCRLIT_IMAGELIST_EX) {
			if (m_pImageListEx) {
				pt.x = rect.left + m_nHMargin;
				pt.y = rect.top + (rect.Height() - m_cyUnitImagelistEx) / 2;
				int srcx = (int)pImagesItem->arrImage[i].dwImage * m_cxUnitImagelistEx;
				int srcy = 0;

				int x = m_bMirror ? (m_rcItem.Width() - pt.x - m_cxUnitImagelistEx) : pt.x;

				Graphics grph(pDC->GetSafeHdc());
				grph.DrawImage(m_pImageListEx, 
					x, 
					pt.y, 
					srcx, 
					srcy, 
					m_cxUnitImagelistEx, 
					m_cyUnitImagelistEx, 
					UnitPixel);
			}
		}
		else if (pImagesItem->arrImage[i].nImageType == ELCRLIT_BITMAP_HANDLE) {
			HGDIOBJ hOldGdi = ::SelectObject(dcImage.GetSafeHdc(), (HBITMAP)pImagesItem->arrImage[i].dwImage);
			pDC->TransparentBlt(pt.x, pt.y, m_cxImage, m_cyImage, &dcImage, 0, 0, m_cxImage, m_cyImage, m_crImageTransparentBk);
			::SelectObject(dcImage, hOldGdi);
		}

		pt.x += m_cxImage + m_nHMargin;
	}

	return pt.x;
}

int CElcReportListCtrl::DrawProgress(CDC *pDC, CRect rect, PELC_SUBITEM_PROGRESS pProgress, LPCTSTR lpszText)
{
	CRect rcProgress = rect;
	rcProgress.left += m_nHMargin;
	rcProgress.top = rect.top + (rect.Height() - m_nProgressHeight) / 2;
	rcProgress.bottom = rcProgress.top + m_nProgressHeight;
	rcProgress.right = rect.right - m_nHMargin;

	if (lpszText && _tcslen(lpszText) > 0) {
		rcProgress.right -= pDC->GetTextExtent(lpszText).cx;
		if (rcProgress.right - rcProgress.left < MIN_PROGRESS_WIDTH)
			rcProgress.right = rcProgress.left + MIN_PROGRESS_WIDTH;
	}

	CRect rcFill = rcProgress;
	rcFill.DeflateRect(1, 1);
	rcFill.right = rcFill.left + pProgress->nPos * rcFill.Width() / 100;

	CBrush brBk(m_crProgressBk);
	CBrush *pOldBrush = pDC->SelectObject(&brBk);

	CPen pen(PS_SOLID, 1, m_crProgressBorder);
	CPen* pOldPen = pDC->SelectObject(&pen);

	pDC->Rectangle(rcProgress);
	pDC->FillSolidRect(&rcFill, m_crProgressFill);

	pDC->SelectObject(&pOldBrush);
	pDC->SelectObject(&pOldPen);

	CString strPercent;
	strPercent.Format(_T("%d%%"), pProgress->nPos);
	pDC->DrawText(strPercent, 
		rcProgress, 
		DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS|DT_WORDBREAK|DT_NOFULLWIDTHCHARBREAK);

	return rcProgress.right + m_nHMargin;
}

void CElcReportListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int iItem = HitTest(point);
	if (iItem != -1 
		&& GetExtendedStyle() & LVS_EX_CHECKBOXES) 
	{
		CRect rcItem;
		GetItemRect(iItem, rcItem, LVIR_BOUNDS); 

		CRect rcCheck;
		rcCheck.left = rcItem.left + m_nHMargin;
		rcCheck.top = rcItem.top + (rcItem.Height() - m_sizeCheckBtn.cy) / 2;
		rcCheck.right = rcCheck.left + m_sizeCheckBtn.cx;
		rcCheck.bottom = rcCheck.top + m_sizeCheckBtn.cy;

		if (rcCheck.PtInRect(point)) {
			if (m_bEnableCheckBtn)
				SetCheck(iItem, !GetCheck(iItem));

			if (GetParent()) {
				ELC_MOUSE_NOTIFY notify;
				notify.hWnd = m_hWnd;
				notify.type = ELC_MOUSE_LBUTTON_DOWN;
				notify.nFlags = nFlags;
				::GetCursorPos(&notify.point);
				GetParent()->SendMessage(WM_ELC_MOUSE_NOTIFY, GetDlgCtrlID(), (LPARAM)&notify); 
			}

			return;
		}
	}

	CListCtrl::OnLButtonDown(nFlags, point);

	if (GetParent()) {
		ELC_MOUSE_NOTIFY notify;
		notify.hWnd = m_hWnd;
		notify.type = ELC_MOUSE_LBUTTON_DOWN;
		notify.nFlags = nFlags;
		::GetCursorPos(&notify.point);
		GetParent()->SendMessage(WM_ELC_MOUSE_NOTIFY, GetDlgCtrlID(), (LPARAM)&notify); 
	}
}

void CElcReportListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonUp(nFlags, point);
}

int CElcReportListCtrl::InsertColumn(int nCol, const LVCOLUMN* pColumn)
{
	int nCount = GetItemCount();
	for (int i=0; i < nCount; i ++) {
		CRLSubItemArray* pSubItem = GetItemInfo(i);
		pSubItem->insert(pSubItem->begin() + nCol, (PELC_REPORT_LIST_SUBITEM)NULL);
	}

	return __super::InsertColumn(nCol, pColumn);
}

int CElcReportListCtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	int nCount = GetItemCount();
	for (int i=0; i < nCount; i ++) {
		CRLSubItemArray* pSubItem = GetItemInfo(i);
		pSubItem->insert(pSubItem->begin() + nCol, (PELC_REPORT_LIST_SUBITEM)NULL);
	}

	return __super::InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);
}

BOOL CElcReportListCtrl::SetColumnWidth(int nCol, int cx)
{
	BOOL bRet = __super::SetColumnWidth(nCol, cx);
	if (m_SkinHeaderCtrl.GetSafeHwnd())
		m_SkinHeaderCtrl.Invalidate();
	return bRet;
}

int CElcReportListCtrl::InsertItem(int nItem, LPCTSTR lpszItem)
{
	return InsertItem(nItem, lpszItem, -1);
}

int CElcReportListCtrl::InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
{
	int nColumnCount = GetHeaderCtrl()->GetItemCount();
	if (nColumnCount <= 0)
		return -1;

	nItem = CListCtrl::InsertItem(nItem, lpszItem);
	if (nItem == -1)
		return -1;

	PELC_REPORTLIST_ITEM_DATA pItem = new ELC_REPORTLIST_ITEM_DATA;
	pItem->dwData = 0;
	pItem->iteminfo.bBold = FALSE;
	pItem->iteminfo.crText = m_skin.crText;
	for (int i=0; i < nColumnCount; i++) {
		pItem->arrSubItem.push_back((PELC_REPORT_LIST_SUBITEM)NULL);
	}

	if (nImage != -1) {
		ELC_RL_IMAGEITEM imageindex;
		imageindex.nId = 0;
		imageindex.nImageType = ELCRLIT_IMAGELIST;
		imageindex.dwImage = nImage;
		imageindex.dwData = 0;
		PELC_SUBITEM_IMAGE pSubItem = new ELC_SUBITEM_IMAGE;
		pSubItem->nType = ERLST_IMAGE;
		pSubItem->arrImage.push_back(imageindex);
		pItem->arrSubItem[0] = pSubItem;
	}

	__super::SetItemData(nItem, (DWORD_PTR)pItem);

	return nItem; 
}

int CElcReportListCtrl::InsertItem(const LVITEM* pItem)
{
	// not support, do not call this function pls
	ASSERT(0);
	return -1;
}

int CElcReportListCtrl::InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam)
{
	// not support, do not call this function pls
	ASSERT(0);
	return -1;
}

BOOL CElcReportListCtrl::SetItem(const LVITEM* pItem)
{
	// only support LVIF_IMAGE and LVIF_TEXT
	if (pItem->mask != (pItem->mask & (LVIF_IMAGE | LVIF_TEXT))) {
		ASSERT(0);
	}

	if (pItem->mask & LVIF_TEXT)
		SetItemText(pItem->iItem, pItem->iSubItem, pItem->pszText);

	if (pItem->mask & LVIF_IMAGE)
		SetImage(pItem->iItem, pItem->iSubItem, 0, pItem->iImage);

	return TRUE;
}

BOOL CElcReportListCtrl::SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem, int nImage, UINT nState, UINT nStateMask, LPARAM lParam)
{
	// only support LVIF_IMAGE and LVIF_TEXT
	if (nMask != (nMask & (LVIF_IMAGE | LVIF_TEXT))) {
		ASSERT(0);
	}

	if (nMask & LVIF_TEXT)
		SetItemText(nItem, nSubItem, lpszItem);

	if (nMask & LVIF_IMAGE)
		SetImage(nItem, nSubItem, 0, nImage);

	return TRUE;
}

BOOL CElcReportListCtrl::SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem, int nImage, UINT nState, UINT nStateMask, LPARAM lParam, int nIndent)
{
	// only support LVIF_IMAGE and LVIF_TEXT
	if (nMask != (nMask & (LVIF_IMAGE | LVIF_TEXT))) {
		ASSERT(0);
	}

	if (nMask & LVIF_TEXT)
		SetItemText(nItem, nSubItem, lpszItem);

	if (nMask & LVIF_IMAGE)
		SetImage(nItem, nSubItem, 0, nImage);

	return TRUE;
}

void CElcReportListCtrl::SetItemBold(int nItem, BOOL bBold)
{
	PELC_REPORTLIST_ITEM_DATA pData = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(nItem);
	if (pData) {
		pData->iteminfo.bBold = bBold;
		Invalidate();
	}
}

BOOL CElcReportListCtrl::GetItemBold(int nItem)
{
	PELC_REPORTLIST_ITEM_DATA pData = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(nItem);
	if (pData) {
		return pData->iteminfo.bBold;
	}
	return FALSE;
}

void CElcReportListCtrl::SetItemTextColor(int nItem, COLORREF crText)
{
	PELC_REPORTLIST_ITEM_DATA pData = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(nItem);
	if (pData) {
		pData->iteminfo.crText = crText;
		Invalidate();
	}
}

COLORREF CElcReportListCtrl::GetItemTextColor(int nItem)
{
	PELC_REPORTLIST_ITEM_DATA pData = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(nItem);
	if (pData) {
		return pData->iteminfo.crText;
	}
	return m_skin.crText;
}

BOOL CElcReportListCtrl::SetItemData(int nItem, DWORD_PTR dwData)
{
	PELC_REPORTLIST_ITEM_DATA pItem = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(nItem);
	if (pItem) {
		pItem->dwData = dwData;
		return TRUE;
	}
	return FALSE;
}

DWORD_PTR CElcReportListCtrl::GetItemData(int nItem) const
{
	PELC_REPORTLIST_ITEM_DATA pItem = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(nItem);
	if (pItem) 
		return pItem->dwData;
	else
		return NULL;
}

CRLSubItemArray* CElcReportListCtrl::GetItemInfo(int nItem)
{
	PELC_REPORTLIST_ITEM_DATA pItem = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(nItem);
	return &pItem->arrSubItem;
}

BOOL CElcReportListCtrl::FindSubItem(int nItem, 
									 int nSubItem, 
									 PELC_REPORTLIST_ITEM_DATA &pItem, 
									 PELC_REPORT_LIST_SUBITEM &pSubItem)
{
	pSubItem = NULL;
	pItem = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(nItem);
	if (!pItem)
		return FALSE;

	if (nSubItem >= (int)pItem->arrSubItem.size()) {
		ASSERT(0);
		return FALSE;
	}

	pSubItem = pItem->arrSubItem[nSubItem];
	return TRUE;
}

BOOL CElcReportListCtrl::AddImage(int nItem, int nSubItem, UINT nImgId, DWORD_PTR dwImage, int nImageType, LPCTSTR lpszTip)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return FALSE;

	BOOL bSuccessful = FALSE;
	if (!pSubItem) {
		PELC_SUBITEM_IMAGE pSubItemImage = new ELC_SUBITEM_IMAGE;
		pSubItemImage->nType = ERLST_IMAGE;
		pSubItemImage->Add(nImgId, dwImage, nImageType, lpszTip);
		pItem->arrSubItem[nSubItem] = pSubItemImage;
		bSuccessful = TRUE;
	}
	else if (pSubItem->nType == ERLST_IMAGE) {
		PELC_SUBITEM_IMAGE pSubItemImage = (PELC_SUBITEM_IMAGE)pSubItem;
		if (pSubItemImage->Add(nImgId, dwImage, nImageType, lpszTip))
			bSuccessful = TRUE;
		else
			bSuccessful = FALSE;
	}

	if (bSuccessful) {
		CRect rcItem;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rcItem);
		InvalidateRect(rcItem);
	}

	return bSuccessful;
}

void CElcReportListCtrl::DelImage(int nItem, int nSubItem, UINT nImgId)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return;

	if (pSubItem && pSubItem->nType == ERLST_IMAGE) {
		PELC_SUBITEM_IMAGE pSubItemImage = (PELC_SUBITEM_IMAGE)pSubItem;
		pSubItemImage->Delete(nImgId);

		CRect rcItem;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rcItem);
		InvalidateRect(rcItem);
	}
}

void CElcReportListCtrl::DelAllImages(int nItem, int nSubItem)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return;

	if (pSubItem && pSubItem->nType == ERLST_IMAGE) {
		PELC_SUBITEM_IMAGE pSubItemImage = (PELC_SUBITEM_IMAGE)pSubItem;
		pSubItemImage->DeleteAll();

		CRect rcItem;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rcItem);
		InvalidateRect(rcItem);
	}
}

int CElcReportListCtrl::GetImageCount(int nItem, int nSubItem)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (FindSubItem(nItem, nSubItem, pItem, pSubItem)
		&& pSubItem 
		&& pSubItem->nType == ERLST_IMAGE) 
	{
		PELC_SUBITEM_IMAGE pSubItemImage = (PELC_SUBITEM_IMAGE)pSubItem;
		return (int)pSubItemImage->arrImage.size();
	}
	return 0;
}

void CElcReportListCtrl::SetImage(int nItem, int nSubItem, UINT nImgId, DWORD_PTR dwImage, int nImageType)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return;

	if (pSubItem && pSubItem->nType == ERLST_IMAGE) {
		PELC_SUBITEM_IMAGE pSubItemImage = (PELC_SUBITEM_IMAGE)pSubItem;
		CImageIndexArray::iterator it = pSubItemImage->Find(nImgId);
		if (it != pSubItemImage->arrImage.end()) {
			it->dwImage = dwImage;
			it->nImageType = nImageType;
		}

		CRect rcItem;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rcItem);
		InvalidateRect(rcItem);
	}
}

BOOL CElcReportListCtrl::GetImage(int nItem, int nSubItem, UINT nImgId, DWORD_PTR& dwImage, int& nImageType)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return FALSE;

	if (pSubItem && pSubItem->nType == ERLST_IMAGE) {
		PELC_SUBITEM_IMAGE pSubItemImage = (PELC_SUBITEM_IMAGE)pSubItem;
		CImageIndexArray::iterator it = pSubItemImage->Find(nImgId);
		if (it != pSubItemImage->arrImage.end()) {
			nImageType = it->nImageType;
			dwImage = (DWORD)it->dwImage;
			return TRUE;
		}
	}

	return FALSE;
}

void CElcReportListCtrl::SetImageTip(int nItem, int nSubItem, UINT nImgId, LPCTSTR lpszTip)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return;

	if (pSubItem && pSubItem->nType == ERLST_IMAGE) {
		PELC_SUBITEM_IMAGE pSubItemImage = (PELC_SUBITEM_IMAGE)pSubItem;
		CImageIndexArray::iterator it = pSubItemImage->Find(nImgId);
		if (it != pSubItemImage->arrImage.end()) 
			it->strTip = lpszTip;
	}
}

void CElcReportListCtrl::SetImageData(int nItem, int nSubItem, UINT nImgId, DWORD_PTR dwData)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return;

	if (pSubItem && pSubItem->nType == ERLST_IMAGE) {
		PELC_SUBITEM_IMAGE pSubItemImage = (PELC_SUBITEM_IMAGE)pSubItem;
		CImageIndexArray::iterator it = pSubItemImage->Find(nImgId);
		if (it != pSubItemImage->arrImage.end()) 
			it->dwData = dwData;
	}
}

DWORD_PTR CElcReportListCtrl::GetImageData(int nItem, int nSubItem, UINT nImgId)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (FindSubItem(nItem, nSubItem, pItem, pSubItem)) {
		if (pSubItem && pSubItem->nType == ERLST_IMAGE) {
			PELC_SUBITEM_IMAGE pSubItemImage = (PELC_SUBITEM_IMAGE)pSubItem;
			CImageIndexArray::iterator it = pSubItemImage->Find(nImgId);
			if (it != pSubItemImage->arrImage.end()) 
				return it->dwData;
		}
	}

	return 0;
}

void CElcReportListCtrl::SetImageTransparentColor(COLORREF color)
{
	m_crImageTransparentBk = color;
}

BOOL CElcReportListCtrl::AddProgress(int nItem, int nSubItem, int nPos)
{
	if (nSubItem == 0) {
		// do not add progress ctrl in the first subitem
		ASSERT(0);
		return FALSE;
	}

	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return FALSE;

	if (nPos < 0)
		nPos = 0;
	else if (nPos > 100)
		nPos = 100;

	BOOL bSuccessful = FALSE;
	if (!pSubItem) {
		PELC_SUBITEM_PROGRESS pSubItemProgress = new ELC_SUBITEM_PROGRESS;
		pSubItemProgress->nType = ERLST_PROGRESS;
		pSubItemProgress->nPos = nPos;
		pItem->arrSubItem[nSubItem] = pSubItemProgress;
		bSuccessful = TRUE;
	}
	else if (pSubItem->nType == ERLST_PROGRESS) {
		PELC_SUBITEM_PROGRESS pSubItemProgress = (PELC_SUBITEM_PROGRESS)pSubItem;
		pSubItemProgress->nPos = nPos;
		bSuccessful = TRUE;
	}

	if (bSuccessful) {
		CRect rcItem;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rcItem);
		InvalidateRect(rcItem);
	}

	return bSuccessful;
}

void CElcReportListCtrl::DelProgress(int nItem, int nSubItem)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return;

	if (pSubItem && pSubItem->nType == ERLST_PROGRESS) {
		delete (PELC_SUBITEM_PROGRESS)pSubItem;
		pItem->arrSubItem[nSubItem] = NULL;
	}
}

BOOL CElcReportListCtrl::SetProgressPos(int nItem, int nSubItem, int nPos)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return FALSE;

	if (nPos < 0)
		nPos = 0;
	else if (nPos > 100)
		nPos = 100;

	if (pSubItem && pSubItem->nType == ERLST_PROGRESS) {
		PELC_SUBITEM_PROGRESS pSubItemProgress = (PELC_SUBITEM_PROGRESS)pSubItem;
		pSubItemProgress->nPos = nPos;
		CRect rcItem;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rcItem);
		InvalidateRect(rcItem);
		return TRUE;
	}

	return FALSE;
}

int CElcReportListCtrl::GetProgressPos(int nItem, int nSubItem)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return FALSE;

	if (pSubItem && pSubItem->nType == ERLST_PROGRESS) {
		PELC_SUBITEM_PROGRESS pSubItemProgress = (PELC_SUBITEM_PROGRESS)pSubItem;
		return pSubItemProgress->nPos;
	}

	return 0;
}

void CElcReportListCtrl::DeleteCustomData(int nItem)
{
	PELC_REPORTLIST_ITEM_DATA pItem = (PELC_REPORTLIST_ITEM_DATA)__super::GetItemData(nItem);
	if (!pItem)
		return;

	for (CRLSubItemArray::iterator it = pItem->arrSubItem.begin();
		it != pItem->arrSubItem.end();
		it ++)
	{
		if ((*it)) {
			if ((*it)->nType == ERLST_IMAGE) {
				delete (PELC_SUBITEM_IMAGE)(*it);
			}
			else if ((*it)->nType == ERLST_PROGRESS) {
				delete (PELC_SUBITEM_PROGRESS)(*it);
			}
		}
	}

	delete pItem;
}

INT_PTR CElcReportListCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	LVHITTESTINFO Info;
	Info.pt = point;

	int nItem = ListView_SubItemHitTest(m_hWnd, &Info);
	if (Info.flags & LVHT_ONITEMICON || Info.flags & LVHT_ONITEMLABEL) {
		pTI->hwnd = m_hWnd;
		pTI->uId = UINT(nItem);
		pTI->lpszText = LPSTR_TEXTCALLBACK;
		CRect rcSub;
		GetItemRect(nItem, &rcSub, LVIR_BOUNDS);
		pTI->rect = rcSub;
	}
	return CListCtrl::OnToolHitTest(point, pTI);
}

BOOL CElcReportListCtrl::OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

	// get mouse position
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	if (pt.y <= 20) 
		return TRUE;

	LVHITTESTINFO Info;
	Info.pt = pt;
	int nItem = ListView_SubItemHitTest(m_hWnd, &Info);
	if (nItem == -1)
		return TRUE;

	CRect rcSub, rcTemp;
	GetSubItemRect(nItem, Info.iSubItem, LVIR_BOUNDS, rcSub);
	GetHeaderCtrl()->GetItemRect(Info.iSubItem, rcTemp);
	rcSub.left = rcTemp.left;
	rcSub.right = rcTemp.right;

	if (rcSub.PtInRect(pt)) 
	{
		PELC_REPORTLIST_ITEM_DATA pItem = NULL;
		PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
		if (!FindSubItem(nItem, Info.iSubItem, pItem, pSubItem))
			return TRUE;

		BOOL bCheckText = TRUE;

		if (pSubItem) {
			if (pSubItem->nType == ERLST_IMAGE) {
				PELC_SUBITEM_IMAGE pImages = (PELC_SUBITEM_IMAGE)pSubItem;
				int nImageIndex = (pt.x - rcSub.left) / (m_cxImage + m_nHMargin);
				if (nImageIndex < (int)pImages->arrImage.size()) {
					int nLength = pImages->arrImage[nImageIndex].strTip.GetLength();
					if (nLength > 0) {
						if (m_cchTip < nLength + 1) {
							m_cchTip = nLength + 1;
							delete m_pszTip;
							m_pszTip = new wchar_t[m_cchTip];
						}

						_tcscpy(m_pszTip, pImages->arrImage[nImageIndex].strTip);
						pTTTW->lpszText = m_pszTip;
					}

					bCheckText = FALSE;
				}
			}
			else if (pSubItem->nType == ERLST_PROGRESS) {
			}
		}

		CString strText = GetItemText(nItem, Info.iSubItem);
		if (bCheckText && !strText.IsEmpty()) {
			CRect rcTextDraw = GetSubItemTextDrawRect(rcSub, pSubItem, strText, pItem->iteminfo.bBold);
			int nWidth = CElcSkinDoc::GetStringSize(strText, pItem->iteminfo.bBold ? &m_ftTextBold : &m_ftText).cx;
			if (rcTextDraw.Width() < nWidth && rcTextDraw.PtInRect(pt)) {
				int nLength = strText.GetLength();
				if (nLength > 0) {
					if (m_cchTip < nLength + 1) {
						m_cchTip = nLength + 1;
						delete m_pszTip;
						m_pszTip = new wchar_t[m_cchTip];
					}

					_tcscpy(m_pszTip, strText);
					pTTTW->lpszText = m_pszTip;
				}
			}
		}
	}

	*pResult = 0;
	return TRUE;
}

void CElcReportListCtrl::GetRLHitTest(CPoint point, PELC_REPORTLIST_HITTESTINFO pInfo)
{
	pInfo->nItem = -1;
	pInfo->nSubItem = -1;
	pInfo->nImageIndex = -1;
	pInfo->nImageId = 0xFFFFFFFF;
	pInfo->dwData = 0;

	ScreenToClient(&point);

	LVHITTESTINFO Info;
	Info.pt = point;
	int nItem = ListView_SubItemHitTest(m_hWnd, &Info);
	if (nItem == -1)
		return ;

	CRect rcSub, rcTemp;
	GetSubItemRect(nItem, Info.iSubItem, LVIR_BOUNDS, rcSub);
	GetHeaderCtrl()->GetItemRect(Info.iSubItem, rcTemp);
	rcSub.left = rcTemp.left;
	rcSub.right = rcTemp.right;

	if (rcSub.PtInRect(point)) 
	{
		PELC_REPORTLIST_ITEM_DATA pItem = NULL;
		PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
		if (!FindSubItem(nItem, Info.iSubItem, pItem, pSubItem))
			return;

		pInfo->nItem = nItem;
		pInfo->nSubItem = Info.iSubItem;
		
		if (pSubItem && pSubItem->nType == ERLST_IMAGE) {
			PELC_SUBITEM_IMAGE pImages = (PELC_SUBITEM_IMAGE)pSubItem;
			int nImageIndex = (point.x - rcSub.left) / (m_cxImage + m_nHMargin);
			if (nImageIndex < (int)pImages->arrImage.size()) {
				pInfo->nImageIndex = nImageIndex;
				pInfo->nImageId = pImages->arrImage[nImageIndex].nId;
				pInfo->dwImage = pImages->arrImage[nImageIndex].dwImage;
				pInfo->dwData = pImages->arrImage[nImageIndex].dwData;
			}
		}
	}
}

void CElcReportListCtrl::SetImageList(CImageList* pImageList, int nImageListType)
{
	CImageList* pTemp = GetHeaderCtrl()->GetImageList();
	CListCtrl::SetImageList(pImageList, nImageListType);
	GetHeaderCtrl()->SetImageList(pTemp);

	m_pImageList = pImageList;

	if (m_pImageList) {
		IMAGEINFO info = {0};
		m_pImageList->GetImageInfo(0, &info);
		m_cxImage = info.rcImage.right - info.rcImage.left;
		m_cyImage = info.rcImage.bottom - info.rcImage.top;
	}
	else {
		m_cxImage = 16;
		m_cyImage = 16;
	}
}

CImageList* CElcReportListCtrl::GetImageList()
{
	return m_pImageList;
}

BOOL CElcReportListCtrl::SetImageListEx(UINT nResId, LPCTSTR lpszType, int cxUnit, int cyUnit)
{
	m_pImageListEx = ImageFromIDResource(nResId, lpszType);
	m_cxUnitImagelistEx = cxUnit;
	m_cyUnitImagelistEx = cyUnit;
	return m_pImageListEx != NULL;
}

int CElcReportListCtrl::GetSubItemType(int nItem, int nSubItem)
{
	PELC_REPORTLIST_ITEM_DATA pItem = NULL;
	PELC_REPORT_LIST_SUBITEM pSubItem = NULL;
	if (!FindSubItem(nItem, nSubItem, pItem, pSubItem))
		return ERLST_NORMAL;

	return pSubItem->nType;
}

void CElcReportListCtrl::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	DeleteCustomData(pNMLV->iItem);
	*pResult = 0;
}

BOOL CElcReportListCtrl::SortItems(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData)
{
	g_pfnSortProc = pfnCompare;
	return __super::SortItems(InternalSortProc, dwData);
}

int CALLBACK CElcReportListCtrl::InternalSortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	PELC_REPORTLIST_ITEM_DATA pData1 = (PELC_REPORTLIST_ITEM_DATA)lParam1;
	PELC_REPORTLIST_ITEM_DATA pData2 = (PELC_REPORTLIST_ITEM_DATA)lParam2;
	return g_pfnSortProc(pData1->dwData, pData2->dwData, lParam3);
}

void CElcReportListCtrl::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (m_SkinHeaderCtrl.GetSafeHwnd())
		m_SkinHeaderCtrl.Invalidate();
}

void CElcReportListCtrl::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	*pResult = 0;

	if (pLVKeyDow->wVKey == VK_F2) {
		int nItem = GetNextItem(-1, LVNI_FOCUSED);
		if (nItem != -1) {
			EditLabel(nItem);
		}
	}
}

CRect CElcReportListCtrl::GetSubItemTextDrawRect(CRect rect, PELC_REPORT_LIST_SUBITEM pSubItem, LPCTSTR lpszText, BOOL bBoldFont)
{
	if (!pSubItem) {
		rect.left += m_nHMargin;
	}
	else if (ERLST_IMAGE == pSubItem->nType) {
		rect.left = GetImageItemWidth(rect, (PELC_SUBITEM_IMAGE)pSubItem);
		if (rect.left == -1) {
			// not enough space to draw text.
			return CRect(0,0,0,0);
		}
	}
	else if (ERLST_PROGRESS == pSubItem->nType) {
		rect.left = GetProgressItemWidth(rect, (PELC_SUBITEM_PROGRESS)pSubItem, lpszText, bBoldFont);
	}

	rect.right -= m_nHMargin;
	return rect;
}

int CElcReportListCtrl::GetImageItemWidth(CRect rect, PELC_SUBITEM_IMAGE pImagesItem)
{
	POINT pt;
	pt.x = rect.left + m_nHMargin;

	int nCount = (int)pImagesItem->arrImage.size();
	for (int i=0; i < nCount; i ++) 
	{
		if (pt.x + m_cxImage > rect.right)
			return -1;

		if (pImagesItem->arrImage[i].nImageType == ELCRLIT_IMAGELIST) {
		}
		else if (pImagesItem->arrImage[i].nImageType == ELCRLIT_IMAGELIST_EX) {
			if (m_pImageListEx) {
				pt.x = rect.left + m_nHMargin;
				pt.y = rect.top + (rect.Height() - m_cyUnitImagelistEx) / 2;
			}
		}
		else if (pImagesItem->arrImage[i].nImageType == ELCRLIT_BITMAP_HANDLE) {
		}

		pt.x += m_cxImage + m_nHMargin;
	}

	return pt.x;
}

int CElcReportListCtrl::GetProgressItemWidth(CRect rect, PELC_SUBITEM_PROGRESS pProgress, LPCTSTR lpszText, BOOL bBoldFont)
{
	CRect rcProgress = rect;
	rcProgress.left += m_nHMargin;
	rcProgress.right = rect.right - m_nHMargin;

	if (lpszText && _tcslen(lpszText) > 0) {
		rcProgress.right -= CElcSkinDoc::GetStringSize(lpszText, bBoldFont ? &m_ftTextBold : &m_ftText).cx;
		if (rcProgress.right - rcProgress.left < MIN_PROGRESS_WIDTH)
			rcProgress.right = rcProgress.left + MIN_PROGRESS_WIDTH;
	}

	return rcProgress.right + m_nHMargin;
}

