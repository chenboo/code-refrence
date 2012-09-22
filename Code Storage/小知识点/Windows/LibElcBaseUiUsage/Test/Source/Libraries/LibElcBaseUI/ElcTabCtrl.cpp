// ElcTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcTabCtrl.h"

#define ID_BUTTON_NEWITEM		50000
#define ID_BUTTON_CLOSE			50001
#define ID_BUTTON_DROPDOWN		50002
#define ID_MENU_BASE			55000
#define ID_DROP_BUTTON			100


#define ELC_TAB_STATE_CHECK		1



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CElcTabCtrl

IMPLEMENT_DYNAMIC(CElcTabCtrl, CWnd)

CElcTabCtrl::CElcTabCtrl()
{
	LOGFONT lf = {0};
	GetMSShellDlgFont(lf);
	m_ftText.CreateFontIndirect(&lf);
	lf.lfWeight = FW_BOLD;
	m_ftBoldText.CreateFontIndirect(&lf);

	memset(&m_skin, 0, sizeof(m_skin));

	m_skin.crBackgnd = RGB(236,233,216);
	m_skin.crTextNormal = RGB(0,0,0);
	m_skin.crTextHot = RGB(0,0,0);
	m_skin.crTextActive = RGB(0,0,0);

	m_bShowBorder = TRUE;

	m_nLeftMargin = 10;
	m_nRightMargin = 10;
	m_nItemsMargin = 4;
	m_nItemInnerMargin = 4;
	m_nTabsMargin = 1;

	m_nItemMinWidth = 40;
	m_nItemMaxWidth = 150;

	m_pImageList = NULL;
	m_pImageListEx = NULL;
	m_szImage = CSize(0,0);

	m_nBtnsWidth = 0;

	m_nActiveItem = -1;
	m_nHotItem = -1;
	m_nFirstVisibleItem = 0;
	m_nLastVisibleItem = 0;
	m_nFocusRectItem = -1;

	m_hNotify = 0;
	m_nNotify = 0;

	memset(&m_tbtnClose, 0, sizeof(m_tbtnClose));
	memset(&m_tbtnNew, 0, sizeof(m_tbtnNew));
	memset(&m_tbtnDrop, 0, sizeof(m_tbtnDrop));
	m_pHotButton = NULL;
}

CElcTabCtrl::~CElcTabCtrl()
{
	PELCTABCTRL_BUTTON pButton[] = { &m_tbtnClose, &m_tbtnNew, &m_tbtnDrop };
	for (int i=0; i < _countof(pButton); i ++) {
		if (pButton[i]->pStrTip)
			delete pButton[i]->pStrTip;
	}
}

BOOL CElcTabCtrl::LoadSkin(LPCTSTR lpszStyle)
{
	if (!lpszStyle || !theSkin.IsValid())
		return FALSE;

	PELC_SKIN_TABCTRL pSkin = theSkin.GetTabCtrlSkin(lpszStyle);
	if (!pSkin)
		return FALSE;
	
	memcpy(&m_skin, pSkin, sizeof(m_skin));

	CSize size;

	memset(&m_tbtnClose, 0, sizeof(m_tbtnClose));
	m_tbtnClose.nId = ID_BUTTON_CLOSE;
	memcpy(&m_tbtnClose.image, &m_skin.buttonclose, sizeof(m_skin.buttonclose));
	size = CElcSkinDoc::GetImageSize(&m_tbtnClose.image);
	m_tbtnClose.rect.right = size.cx;
	m_tbtnClose.rect.bottom = size.cy;

	memset(&m_tbtnNew, 0, sizeof(m_tbtnNew));
	m_tbtnNew.nId = ID_BUTTON_NEWITEM;
	memcpy(&m_tbtnNew.image, &m_skin.buttonnew, sizeof(m_skin.buttonnew));
	size = CElcSkinDoc::GetImageSize(&m_tbtnNew.image);
	m_tbtnNew.rect.right = size.cx;
	m_tbtnNew.rect.bottom = size.cy;

	memset(&m_tbtnDrop, 0, sizeof(m_tbtnDrop));
	m_tbtnDrop.nId = ID_BUTTON_DROPDOWN;
	memcpy(&m_tbtnDrop.image, &m_skin.dropdown, sizeof(m_skin.dropdown));
	size = CElcSkinDoc::GetImageSize(&m_tbtnDrop.image);
	m_tbtnDrop.rect.right = size.cx;
	m_tbtnDrop.rect.bottom = size.cy;
	m_tbtnDrop.bShow = TRUE;

	return TRUE;
}


BEGIN_MESSAGE_MAP(CElcTabCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()

	ON_COMMAND_RANGE(ID_MENU_BASE, ID_MENU_BASE + 200, OnActiveItem)
END_MESSAGE_MAP()



// CElcTabCtrl message handlers



int CElcTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!m_skin.item.pImage 
		&& !LoadSkin(ELC_SKIN_STYLE_TABCTRL_DOWNWARD)) 
		return -1;

	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bMirror = IsLayoutRTL();

	EnableToolTip();

	SetTimer(ELC_TAB_STATE_CHECK, 500, 0);

	return 0;
}

void CElcTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0)
		return;

	GetClientRect(&m_rcClient);

	Resize();
}

void CElcTabCtrl::Repaint()
{
	if (::IsWindow(m_hWnd))
		Invalidate();
}

void CElcTabCtrl::OnPaint()
{
	CPaintDC dc(this); 

	CElcMemDC dcMem(&dc, m_rcClient);

	if (m_skin.background.pImage) {
		thePainter.DrawImageGDI(dcMem.GetSafeHdc(),
			&m_skin.background,
			m_rcClient,
			0,
			0,
			1,
			0);
	}
	else {
		dcMem.FillSolidRect(m_rcClient, m_skin.crBackgnd);
	}

	DrawTab(&dcMem);
	DrawFocusRect(&dcMem);

	dcMem.DrawMe();
}

BOOL CElcTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CElcTabCtrl::DrawTab(CDC* pDC)
{
	DrawButton(pDC, &m_tbtnNew);

	if ((m_nLastVisibleItem - m_nFirstVisibleItem + 1) < (int)m_arrItem.size())
		DrawButton(pDC, &m_tbtnDrop);

	if (m_arrItem.empty())
		return;

	CFont* pOldFont = pDC->SelectObject(&m_ftText);

	int nImageIndex = 0;
	for (int i=m_nFirstVisibleItem; i <= m_nLastVisibleItem; i++)
	{
		if (!m_arrItem[i].bActive)
		{
			pDC->SelectObject(&m_ftText);
			pDC->SetTextColor(m_skin.crTextNormal);
			nImageIndex = m_nHotItem != i ? 0 : 1;
		}
		else
		{
			pDC->SelectObject(&m_ftBoldText);
			pDC->SetTextColor(m_skin.crTextActive);
			nImageIndex = 2;
		}

		DrawItem(pDC, (PELCTABITEM)&m_arrItem[i], nImageIndex);
	}

	if (!m_bShowBorder)
		return;

	CRect rcLeft, rcCenter, rcRight;
	rcLeft.left = m_rcClient.left;
	rcLeft.right = m_arrItem[m_nActiveItem].rcItem.left;
	rcRight.left = m_arrItem[m_nActiveItem].rcItem.right;
	rcRight.right = m_rcClient.right;
	rcCenter.left = rcLeft.right;
	rcCenter.right = rcRight.left;

	if (m_skin.bUpward)
	{
		rcLeft.top = m_rcClient.top;
		rcLeft.bottom = m_rcTabs.top;
		rcRight.top = m_rcClient.top;
		rcRight.bottom = m_rcTabs.top;
		rcCenter.top = m_rcClient.top;
		rcCenter.bottom = m_rcTabs.top;
	}
	else
	{
		rcLeft.top = m_rcTabs.bottom;
		rcLeft.bottom = m_rcClient.bottom;
		rcRight.top = m_rcTabs.bottom;
		rcRight.bottom = m_rcClient.bottom;
		rcCenter.top = m_rcTabs.bottom;
		rcCenter.bottom = m_rcClient.bottom;
	}

	if (m_skin.borderleft.pImage) {
		thePainter.DrawImageGDI(pDC->GetSafeHdc(),
			&m_skin.borderleft,
			rcLeft,
			0,
			0,
			1,
			0);
	}

	if (m_skin.bordercenter.pImage) {
		thePainter.DrawImageGDI(pDC->GetSafeHdc(),
			&m_skin.bordercenter,
			rcCenter,
			0,
			0,
			1,
			0);
	}

	if (m_skin.borderright.pImage) {
		thePainter.DrawImageGDI(pDC->GetSafeHdc(),
			&m_skin.borderright,
			rcRight,
			0,
			0,
			1,
			0);
	}

	pDC->SelectObject(pOldFont);
}

void CElcTabCtrl::DrawItem(CDC* pDC, PELCTABITEM pItem, int nImageIndex)
{
	CRect rcItem = pItem->rcItem;
	if (m_bMirror)
		rcItem = MirrorRect(m_rcClient.Width(), pItem->rcItem);

	DWORD dwRet = pDC->GetLayout();
	thePainter.DrawImageGDI(pDC->GetSafeHdc(),
		&m_skin.item,
		rcItem,
		nImageIndex,
		0,
		1,
		0);

	UINT nFormat = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX;

	int nOffset = 0;
	if (m_pImageListEx && pItem->nImage >= 0) {
		RECT rcDest, rcSrc;
		rcDest.left = rcItem.left + m_nItemInnerMargin;
		rcDest.top = rcItem.top + (rcItem.Height() - m_szImage.cy)/2;
		rcDest.right = rcDest.left + m_szImage.cx;
		rcDest.bottom = rcDest.top + m_szImage.cy;

		rcSrc.left = pItem->nImage * m_szImage.cx;
		rcSrc.top = 0;
		rcSrc.right = rcSrc.left + m_szImage.cx;
		rcSrc.bottom = rcSrc.top + m_szImage.cy;

		thePainter.DrawImageGDI(pDC->GetSafeHdc(),
			m_pImageListEx,
			rcDest,
			rcSrc);
		
		nFormat |= DT_LEFT;

		nOffset = m_nItemInnerMargin + m_szImage.cx + m_nItemInnerMargin;
	}
	else if (m_pImageList && pItem->nImage >= 0) {
		CPoint pt;
		pt.x = rcItem.left + m_nItemInnerMargin;
		pt.y = rcItem.top + (rcItem.Height()-m_szImage.cy)/2;
		nOffset = m_nItemInnerMargin + m_szImage.cx + m_nItemInnerMargin;

		m_pImageList->Draw(pDC, pItem->nImage, pt, ILD_TRANSPARENT);

		nFormat |= DT_LEFT;
	}
	else {
		nOffset = m_nItemInnerMargin;
		nFormat |= DT_CENTER;
	}

	CRect rcText;
	rcText.top = rcItem.top;
	rcText.bottom = rcItem.bottom;
	rcText.left = rcItem.left + nOffset;
	rcText.right = rcItem.right - m_nItemInnerMargin;
	if (pItem->bActive 
		&& m_tbtnClose.bShow)
		rcText.right -= (m_tbtnClose.rect.right - m_tbtnClose.rect.left) + m_nItemInnerMargin;

	if (m_bMirror) {
		rcText = MirrorRect(m_rcClient.Width(), rcText);
		nFormat |= DT_RTLREADING;
	}

	pDC->DrawText(pItem->strText, rcText, nFormat);

	if (pItem->bActive)
		DrawButton(pDC, &m_tbtnClose);
}

void CElcTabCtrl::DrawButton(CDC* pDC, PELCTABCTRL_BUTTON pButton)
{
	if (!pButton->bShow)
		return;

	CRect rect = pButton->rect;
	if (m_bMirror) {
		rect = MirrorRect(m_rcClient.Width(), pButton->rect);
		rect.left -= 1;
	}


	thePainter.DrawImageGDI(pDC->GetSafeHdc(),
		&pButton->image,
		rect,
		pButton->state,
		0,
		0,
		0);
}

void CElcTabCtrl::DrawFocusRect(CDC* pDC)
{
	if (m_nFocusRectItem >= m_nFirstVisibleItem
		&& m_nFocusRectItem <= m_nLastVisibleItem)
	{
		pDC->DrawFocusRect(m_arrItem[m_nFocusRectItem].rcItem);
	}
	else if (m_nFocusRectItem == (int)m_arrItem.size()) {
		pDC->DrawFocusRect(&m_tbtnNew.rect);
	}
}

void CElcTabCtrl::CalcTabItemsPos()
{
	int nCount = (int)m_arrItem.size();
	if (nCount <= 0)
		return;

	CRect rc = m_rcTabs;
	int nWidth = (m_rcTabs.Width() - m_nTabsMargin * nCount) / nCount;

	for (int i=m_nFirstVisibleItem; i < nCount; i++) {

		rc.top = m_rcTabs.top;

		/*
		if (m_arrItem[i].bActive)
		{
			rc.right = rc.left 
				+ m_nItemInnerMargin + m_szImage.cx
				+ m_nItemInnerMargin + m_arrItem[i].size.cx
				+ m_nItemInnerMargin;

			if (m_tbtnClose.bShow)
				rc.right += (m_tbtnClose.rect.right - m_tbtnClose.rect.left) + m_nItemInnerMargin;
		}
		else
		{
			rc.right = rc.left 
				+ m_nItemInnerMargin + m_szImage.cx
				+ m_nItemInnerMargin + m_arrItem[i].size.cx + m_nItemInnerMargin;
		}
		*/

		rc.right = rc.left + nWidth;

		if (rc.Width() < m_nItemMinWidth)
			rc.right = rc.left + m_nItemMinWidth;
		else if (rc.Width() > m_nItemMaxWidth)
			rc.right = rc.left + m_nItemMaxWidth;

		if (rc.right <= m_rcTabs.right)
			m_nLastVisibleItem = i;

		m_arrItem[i].rcItem = rc;
		rc.left = rc.right + m_nTabsMargin;
	}
}

int CElcTabCtrl::CalcTabWidth(int nFirstItem, int nLastItem)
{
	if ((int)m_arrItem.size() <= nFirstItem || (int)m_arrItem.size() <= nLastItem)
	{
		ASSERT(0);
		return 0;
	}

	int nWidth = 0;
	for (int i=nFirstItem; i <= nLastItem; i++)
	{
		if (m_arrItem[i].bActive)
		{
			nWidth += m_nItemInnerMargin + m_szImage.cx
				+ m_nItemInnerMargin + m_arrItem[i].size.cx
				+ m_nItemInnerMargin + m_nTabsMargin;

			if (m_tbtnClose.bShow)
				nWidth += (m_tbtnClose.rect.right - m_tbtnClose.rect.left) + m_nItemInnerMargin;
		}
		else
		{
			nWidth += m_nItemInnerMargin + m_szImage.cx
				+ m_nItemInnerMargin + m_arrItem[i].size.cx + m_nItemInnerMargin + m_nTabsMargin;
		}
	}

	return nWidth;
}

void CElcTabCtrl::SetTabLayout(ELCTAB_LAYOUT layout)
{
	m_skin.bUpward = layout;

	Resize();
	Repaint();
}

void CElcTabCtrl::ShowBorder(BOOL bShow)
{
	if (m_bShowBorder != bShow) {
		m_bShowBorder = bShow;
		Resize();
		Repaint();
	}
}

BOOL CElcTabCtrl::IsBorderVisible()
{
	return m_bShowBorder;
}

void CElcTabCtrl::SetLeftMargin(int nMargin)
{
	ASSERT(nMargin > 0);
	if (m_nLeftMargin == nMargin)
		return;

	m_nLeftMargin = nMargin;
	Resize();
}

int CElcTabCtrl::GetLeftMargin()
{
	return m_nLeftMargin;
}

void CElcTabCtrl::SetRightMargin(int nMargin)
{
	ASSERT(nMargin > 0);
	if (m_nRightMargin == nMargin)
		return;

	m_nRightMargin = nMargin;
	Resize();
}

int CElcTabCtrl::GetRightMargin()
{
	return m_nRightMargin;
}

void CElcTabCtrl::SetItemMinWidth(int nWidth)
{
	if (m_nItemMinWidth == nWidth)
		return;

	m_nItemMinWidth = nWidth;
	Resize();
}

int CElcTabCtrl::GetItemMinWidth()
{
	return m_nItemMinWidth;
}

void CElcTabCtrl::SetItemMaxWidth(int nWidth)
{
	if (m_nItemMaxWidth == nWidth)
		return;

	m_nItemMaxWidth = nWidth;
	Resize();
}

int CElcTabCtrl::GetItemMaxWidth()
{
	return m_nItemMaxWidth;
}

void CElcTabCtrl::Resize()
{
	m_tbtnDrop.bShow = !m_arrItem.empty();

	CSize size(m_tbtnDrop.rect.right-m_tbtnDrop.rect.left, m_tbtnDrop.rect.bottom - m_tbtnDrop.rect.top);

	m_tbtnDrop.rect.right = m_rcClient.right - m_nRightMargin;
	m_tbtnDrop.rect.left = m_tbtnDrop.rect.right - size.cx;

	m_rcTabs.left = m_nLeftMargin;
	m_rcTabs.right = m_rcClient.right - m_nRightMargin;
	if (m_tbtnDrop.bShow)
		m_rcTabs.right -= size.cx + m_nItemsMargin;
	if (m_tbtnNew.bShow)
		m_rcTabs.right -= (m_tbtnNew.rect.right - m_tbtnNew.rect.left) + m_nItemsMargin;

	if (m_skin.bUpward) {
		if (m_bShowBorder)
			m_rcTabs.top = m_rcClient.top + CElcSkinDoc::GetImageSize(&m_skin.borderleft).cy;
		else 
			m_rcTabs.top = m_rcClient.top;

		m_rcTabs.bottom = m_rcTabs.top + CElcSkinDoc::GetImageSize(&m_skin.item).cy;
	}
	else {
		if (m_bShowBorder)
			m_rcTabs.bottom = m_rcClient.bottom - CElcSkinDoc::GetImageSize(&m_skin.borderleft).cy;
		else
			m_rcTabs.bottom = m_rcClient.bottom;

		m_rcTabs.top = m_rcTabs.bottom - CElcSkinDoc::GetImageSize(&m_skin.item).cy;
	}

	m_tbtnDrop.rect.top = m_rcTabs.top + (m_rcTabs.Height() - size.cy) / 2;
	m_tbtnDrop.rect.bottom = m_tbtnDrop.rect.top + size.cy;

	CalcTabItemsPos();

	if (m_nActiveItem < m_nFirstVisibleItem)
	{
		m_nFirstVisibleItem = m_nActiveItem;
		if (m_nFirstVisibleItem < 0)
			m_nFirstVisibleItem = 0;

		CalcTabItemsPos();
	}
	else if (m_nActiveItem > m_nLastVisibleItem)
	{
		while (m_nActiveItem > m_nLastVisibleItem)
		{
			m_nFirstVisibleItem ++;
			CalcTabItemsPos();
		}
	}

	if (m_nFirstVisibleItem > 0 && m_nLastVisibleItem == (int)(m_arrItem.size()-1))
	{
		int nFirstItem = m_nFirstVisibleItem;
		int nTabWidth = CalcTabWidth(nFirstItem-1, m_nLastVisibleItem);;
		while (nTabWidth < m_rcTabs.Width())
		{
			nFirstItem --;
			if (nFirstItem < 0)
			{
				nFirstItem = 0;
				break;
			}

			nTabWidth = CalcTabWidth(nFirstItem, m_nLastVisibleItem);
		}

		if (nFirstItem < m_nFirstVisibleItem)
		{
			m_nFirstVisibleItem = nFirstItem;
			CalcTabItemsPos();
		}
	}

	SetBtnsPos();

	UpdateTooltips();
}

void CElcTabCtrl::SetBtnsPos()
{
	if (!m_arrItem.empty())
	{
		int cx = m_tbtnClose.rect.right - m_tbtnClose.rect.left;
		int cy = m_tbtnClose.rect.bottom - m_tbtnClose.rect.top;

		m_tbtnClose.rect.right = m_arrItem[m_nActiveItem].rcItem.right - m_nItemInnerMargin;
		m_tbtnClose.rect.left = m_tbtnClose.rect.right - cx;
		m_tbtnClose.rect.top = m_rcTabs.top + (m_rcTabs.Height() - cy) / 2;
		m_tbtnClose.rect.bottom = m_tbtnClose.rect.top + cy;

		cx = m_tbtnNew.rect.right - m_tbtnNew.rect.left;
		cy = m_tbtnNew.rect.bottom - m_tbtnNew.rect.top;

		m_tbtnNew.rect.left = m_arrItem[m_nLastVisibleItem].rcItem.right + m_nItemsMargin;
		m_tbtnNew.rect.right = m_tbtnNew.rect.left + cx;
		m_tbtnNew.rect.top = m_rcTabs.top + (m_rcTabs.Height() - cy) / 2;
		m_tbtnNew.rect.bottom = m_tbtnNew.rect.top + cy;
	}
	else {
		int cx = m_tbtnNew.rect.right - m_tbtnNew.rect.left;
		int cy = m_tbtnNew.rect.bottom - m_tbtnNew.rect.top;

		m_tbtnNew.rect.left = m_rcTabs.left + m_nItemsMargin;
		m_tbtnNew.rect.right = m_tbtnNew.rect.left + cx;
		m_tbtnNew.rect.top = m_rcTabs.top + (m_rcTabs.Height() - cy) / 2;
		m_tbtnNew.rect.bottom = m_tbtnNew.rect.top + cy;
	}
}

void CElcTabCtrl::UpdateTooltips()
{
	if (!m_wndToolTip.GetSafeHwnd())
		return;

	PELCTABCTRL_BUTTON pButton[] = { &m_tbtnClose, &m_tbtnNew, &m_tbtnDrop };
	for (int i=0; i < _countof(pButton); i ++) {
		if (pButton[i]->pStrTip)
			m_wndToolTip.DelTool(this, pButton[i]->nId);
	}

	for (int i=0; i < _countof(pButton); i ++) {
		if (pButton[i]->pStrTip) {
			m_wndToolTip.AddTool(this, *pButton[i]->pStrTip, &pButton[i]->rect, pButton[i]->nId);
		}
	}

	if (!m_arrItem.empty()) {
		for (CELCTabItemArray::iterator it = m_arrItem.begin();
			it != m_arrItem.end();
			it ++)
		{
			m_wndToolTip.DelTool(this, it->nId);
		}

		if (m_nFirstVisibleItem != m_nLastVisibleItem) {
			for (int i = m_nFirstVisibleItem; i <= m_nLastVisibleItem; i ++) {
				m_wndToolTip.AddTool(this, m_arrItem[i].strText, m_arrItem[i].rcItem, m_arrItem[i].nId);
			}
		}
	}
}

BOOL CElcTabCtrl::SetImageListEx(UINT nResId, LPCTSTR lpszType, int cxUnit, int cyUnit)
{
	m_pImageListEx = ImageFromIDResource(nResId, lpszType);
	m_szImage.cx = cxUnit;
	m_szImage.cy = cyUnit;
	return (m_pImageListEx != NULL);
}

CImageList* CElcTabCtrl::SetImageList(CImageList* pImageList)
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

CImageList* CElcTabCtrl::GetImageList( ) const
{
	return m_pImageList;
}

CSize CElcTabCtrl::GetStringSize(LPCTSTR lpszText)
{
	CSize size(0,0);

	CDC* pDC = GetDC();
	if (!pDC) {
		ASSERT(0);
		return size;
	}

	CFont* pOldFont = pDC->SelectObject(&m_ftBoldText);
	size = pDC->GetTextExtent(lpszText);
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	/*
	if (size.cx < MIN_TABITEM_WIDTH)
		size.cx = MIN_TABITEM_WIDTH;
	else if (size.cx > MAX_TABITEM_WIDTH)
		size.cx = MAX_TABITEM_WIDTH;
	*/

	return size;
}

int CElcTabCtrl::AddItem(LPCTSTR pszText, int nImage)
{
	return InsertItem((int)m_arrItem.size(), pszText, nImage);
}

int CElcTabCtrl::InsertItem(int nItem, LPCTSTR pszText, int nImage)
{
	ELCTABITEM item;
	item.nId = GetTickCount();
	item.nImage = nImage;
	item.strText = pszText;
	item.size = GetStringSize(pszText);
	item.bActive = FALSE;
	item.dwData = 0;
	
	if (nItem < 0)
		nItem = 0;
	else if (nItem >= (int)m_arrItem.size())
		nItem = (int)m_arrItem.size();

	if (m_arrItem.empty())
	{
		item.bActive = TRUE;
		m_nActiveItem = 0;
	}
	else {
		if (m_nActiveItem >= nItem)
			m_nActiveItem ++;
	}

	m_arrItem.insert(m_arrItem.begin() + nItem, item);

	Resize();
	Repaint();

	return nItem;
}

BOOL CElcTabCtrl::DelItem(int nItem)
{
	if (nItem < 0 || nItem >= (int)m_arrItem.size())
		return FALSE;

	if (nItem <= m_nActiveItem)
		m_arrItem[m_nActiveItem].bActive = FALSE;

	m_arrItem.erase(m_arrItem.begin() + nItem);

	if (!m_arrItem.empty())
	{
		if (m_nActiveItem >= (int)m_arrItem.size())
			m_nActiveItem --;

		m_arrItem[m_nActiveItem].bActive = TRUE;

		SetCurSel(m_nActiveItem);
	}
	else 
	{
		m_nActiveItem = -1;
		Resize();

		Repaint();
	}
	
	return TRUE;
}

void CElcTabCtrl::DelAllItem()
{
	m_arrItem.clear();

	m_nActiveItem = -1;
	m_nFirstVisibleItem = 0;
	m_nLastVisibleItem = 0;

	Repaint();
}

int CElcTabCtrl::GetItemCount()
{
	return (int)m_arrItem.size();
}

BOOL CElcTabCtrl::SetItemText(int nItem, LPCTSTR pszText)
{
	if (nItem < 0 || nItem >= (int)m_arrItem.size())
	{
		ASSERT(0);
		return FALSE;
	}

	if (m_arrItem[nItem].strText.Compare(pszText) == 0)
		return TRUE;

	m_arrItem[nItem].size = GetStringSize(pszText);
	m_arrItem[nItem].strText = pszText;

	Resize();
	Repaint();

	return TRUE;
}

CString CElcTabCtrl::GetItemText(int nItem)
{
	if (nItem < 0 || nItem >= (int)m_arrItem.size())
	{
		ASSERT(0);
		return _T("");
	}

	return m_arrItem[nItem].strText;	
}

BOOL CElcTabCtrl::SetItemImage(int nItem, int nImage)
{
	if (nItem < 0 || nItem >= (int)m_arrItem.size())
		return FALSE;

	m_arrItem[nItem].nImage = nImage;

	InvalidateRect(m_rcTabs);

	return TRUE;
}

int CElcTabCtrl::GetItemImage(int nItem)
{
	if (nItem < 0 || nItem >= (int)m_arrItem.size())
	{
		ASSERT(0);
		return -1;
	}

	return m_arrItem[nItem].nImage;
}

BOOL CElcTabCtrl::SetItemData(int nItem, DWORD_PTR dwData)
{
	if (nItem < 0 || nItem >= (int)m_arrItem.size())
	{
		ASSERT(0);
		return FALSE;
	}

	m_arrItem[nItem].dwData = dwData;

	return TRUE;
}

DWORD_PTR CElcTabCtrl::GetItemData(int nItem)
{
	if (nItem < 0 || nItem >= (int)m_arrItem.size())
	{
		//ASSERT(0);
		return NULL;
	}

	return m_arrItem[nItem].dwData;
}

void CElcTabCtrl::SetCurSel(int nItem)
{
	if (nItem < 0)
	{
		ASSERT(0);
		return ;
	}
	else if (nItem >= (int)m_arrItem.size())
	{
		ASSERT(0);
		return;
	}

	LPARAM lParam;
	lParam = MAKELONG(m_nActiveItem, nItem);
	
	m_arrItem[m_nActiveItem].bActive = FALSE;
	m_arrItem[nItem].bActive = TRUE;

	m_nActiveItem = nItem;

	Resize();

	Repaint();

	if (m_hNotify && m_nNotify)
		::PostMessage(m_hNotify, m_nNotify, ELCTAB_ITEMCHANGED, lParam);
}

int CElcTabCtrl::GetCurSel()
{
	return m_nActiveItem;
}

void CElcTabCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//if (m_bMirror)
	//	point.x = m_rcClient.Width() - point.x;

	int nSel = GetActiveItem(point);
	if (nSel != -1) {
		if (m_hNotify && m_nNotify) {
			::SendMessage(m_hNotify, m_nNotify, ELCTAB_ITEM_DOUBLE_CLICK, nSel);
		}
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CElcTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	//if (m_bMirror)
	//	point.x = m_rcClient.Width() - point.x;

	m_pHotButton = TrackMouse(point);
	int nSel = GetActiveItem(point);
	
	if (nSel != -1 && m_nActiveItem != nSel) {
		if (m_hNotify && m_nNotify) {
			if (::SendMessage(m_hNotify, m_nNotify, ELCTAB_ITEMCHANGING, MAKELONG(m_nActiveItem, nSel)) != 0)
				return;
		}

		SetCurSel(nSel);
	}
	else if (m_pHotButton) {
		SetCapture();
		m_pHotButton->state = ELCBS_DOWN;
		InvalidateRect(&m_pHotButton->rect);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CElcTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	//if (m_bMirror)
	//	point.x = m_rcClient.Width() - point.x;

	if (m_pHotButton) {
		if (TrackMouse(point) == m_pHotButton)
			ButtonPressed(m_pHotButton);

		m_pHotButton->state = ELCBS_NORMAL;
		InvalidateRect(&m_pHotButton->rect);
		m_pHotButton = NULL;
		ReleaseCapture();
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CElcTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	//if (m_bMirror)
	//	point.x = m_rcClient.Width() - point.x;

	CheckMouseInButton(point);

	int nSel = GetActiveItem(point);
	if (nSel != m_nHotItem) {
		m_nHotItem = nSel;
		Repaint();
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CElcTabCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);
}

void CElcTabCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags, point);
}

void CElcTabCtrl::OnCaptureChanged(CWnd *pWnd)
{
	if (m_pHotButton) {
		m_pHotButton->state = ELCBS_NORMAL;
		InvalidateRect(&m_pHotButton->rect);
		m_pHotButton = NULL;
	}

	CWnd::OnCaptureChanged(pWnd);
}

int CElcTabCtrl::GetActiveItem(CPoint point)
{
	if (m_arrItem.size() <= 0)
		return -1;

	for (int i=m_nFirstVisibleItem; i <= m_nLastVisibleItem; i++)
	{
		if (m_arrItem[i].rcItem.PtInRect(point))
		{
			return i;
		}
	}

	return -1;
}

PELCTABCTRL_BUTTON CElcTabCtrl::TrackMouse(CPoint point)
{
	if (::PtInRect(&m_tbtnClose.rect, point)) {
		if (m_tbtnClose.bShow && m_tbtnClose.state != ELCBS_DISABLED)
			return &m_tbtnClose;
	}
	else if (::PtInRect(&m_tbtnNew.rect, point)) {
		if (m_tbtnNew.bShow && m_tbtnNew.state != ELCBS_DISABLED)
			return &m_tbtnNew;
	}
	else if (::PtInRect(&m_tbtnDrop.rect, point)) {
		if (m_tbtnDrop.bShow && m_tbtnDrop.state != ELCBS_DISABLED)
			return &m_tbtnDrop;
	}

	return NULL;
}

void CElcTabCtrl::CheckMouseInButton(CPoint point)
{
	if (CheckButtonState(point, &m_tbtnClose))
		InvalidateRect(&m_tbtnClose.rect);

	if (CheckButtonState(point, &m_tbtnNew))
		InvalidateRect(&m_tbtnNew.rect);

	if (CheckButtonState(point, &m_tbtnDrop))
		InvalidateRect(&m_tbtnDrop.rect);
}

BOOL CElcTabCtrl::CheckButtonState(CPoint point, PELCTABCTRL_BUTTON pButton)
{
	if (!pButton->bShow || pButton->state == ELCBS_DISABLED)
		return FALSE;

	if (::PtInRect(&pButton->rect, point)) {
		if (pButton == m_pHotButton) {
			if (pButton->state != ELCBS_DOWN) {
				pButton->state = ELCBS_DOWN;
				return TRUE;
			}
		}
		else if (pButton->state != ELCBS_HOT) {
			pButton->state = ELCBS_HOT;
			return TRUE;
		}
	}
	else {
		if (pButton->state != ELCBS_NORMAL) {
			pButton->state = ELCBS_NORMAL;
			return TRUE;
		}
	}

	return FALSE;
}

void CElcTabCtrl::OnActiveItem(UINT uIndex)
{
	if (m_hNotify && m_nNotify)
	{
		if (::SendMessage(m_hNotify, m_nNotify, ELCTAB_ITEMCHANGING, MAKELONG(m_nActiveItem, uIndex-ID_MENU_BASE)) != 0)
			return;
	}
	
	SetCurSel(uIndex-ID_MENU_BASE);
}

void CElcTabCtrl::ButtonPressed(PELCTABCTRL_BUTTON pButton)
{
	WPARAM wParam = 0;
	LPARAM lParam = 0;

	if (pButton->nId == ID_BUTTON_NEWITEM) {
		wParam = ELCTAB_NEWITEM;
	}
	else if (pButton->nId == ID_BUTTON_CLOSE) {
		wParam = ELCTAB_CLOSEITEM;
		lParam = m_nActiveItem;
	}
	else if (pButton->nId == ID_BUTTON_DROPDOWN) {
		CMenu menu;
		menu.CreatePopupMenu();

		CELCTabItemArray::iterator it = m_arrItem.begin();
		for (int i=0; it != m_arrItem.end(); it++, i++) {
			menu.AppendMenu(MF_STRING, ID_MENU_BASE+i, it->strText);
			if (it->bActive)
				menu.CheckMenuItem(ID_MENU_BASE+i, MF_BYCOMMAND|MF_CHECKED);
		}

		CRect rect = m_tbtnDrop.rect;
		ClientToScreen(&rect);
		menu.TrackPopupMenu(TPM_LEFTALIGN, rect.left, rect.bottom, this);

		return;
	}

	if (m_hNotify && m_nNotify)
		::PostMessage(m_hNotify, m_nNotify, wParam, lParam);
}

void CElcTabCtrl::SetNotifyWindow(HWND hWnd, UINT nMessage)
{
	m_hNotify = hWnd;
	m_nNotify = nMessage;
}

void CElcTabCtrl::SetBkColor(COLORREF cr)
{
	m_skin.crBackgnd = cr;
	Repaint();
}

COLORREF CElcTabCtrl::GetBkColor()
{
	return m_skin.crBackgnd;
}

BOOL CElcTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (m_wndToolTip.GetSafeHwnd())
		m_wndToolTip.RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}

void CElcTabCtrl::EnableToolTip()
{
	if (!::IsWindow(m_wndToolTip.GetSafeHwnd()))
	{
		m_wndToolTip.Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX);
		m_wndToolTip.Activate(TRUE);
		m_wndToolTip.SetDelayTime(500);
	}
}

void CElcTabCtrl::EnableCloseButton(BOOL bEnable)
{
	m_tbtnClose.state = bEnable ? ELCBS_NORMAL : ELCBS_DISABLED;
	Repaint();
}

BOOL CElcTabCtrl::IsCloseButtonEnable()
{
	return m_tbtnClose.state != ELCBS_DISABLED;
}

void CElcTabCtrl::SetCloseButtonTip(LPCTSTR lpszText)
{
	if (m_tbtnClose.pStrTip == 0)
		m_tbtnClose.pStrTip = new CString;
		
	*m_tbtnClose.pStrTip = lpszText;
	UpdateTooltips();
}

void CElcTabCtrl::ShowCloseButton(BOOL bShow)
{
	m_tbtnClose.bShow = bShow;
	Resize();
	Repaint();
}

BOOL CElcTabCtrl::IsCloseButtonVisiable()
{
	return m_tbtnClose.bShow;
}

void CElcTabCtrl::EnableNewButton(BOOL bEnable)
{
	m_tbtnNew.state = bEnable ? ELCBS_NORMAL : ELCBS_DISABLED;
	Repaint();
}

BOOL CElcTabCtrl::IsNewButtonEnable()
{
	return m_tbtnNew.state != ELCBS_DISABLED;
}

void CElcTabCtrl::SetNewButtonTip(LPCTSTR lpszText)
{
	if (m_tbtnNew.pStrTip == 0)
		m_tbtnNew.pStrTip = new CString;

	*m_tbtnNew.pStrTip = lpszText;
	UpdateTooltips();
}

void CElcTabCtrl::SetNewButtonTip(UINT nTextId)
{
	if (m_tbtnNew.pStrTip == 0)
		m_tbtnNew.pStrTip = new CString;

	m_tbtnNew.pStrTip->LoadString(nTextId);
	UpdateTooltips();
}

void CElcTabCtrl::ShowNewButton(BOOL bShow)
{
	m_tbtnNew.bShow = bShow;
	Resize();
	Repaint();
}

BOOL CElcTabCtrl::IsNewButtonVisiable()
{
	return m_tbtnNew.bShow;
}

void CElcTabCtrl::SetDropdownButtonTip(LPCTSTR lpszText)
{
	if (m_tbtnDrop.pStrTip == 0)
		m_tbtnDrop.pStrTip = new CString;

	*m_tbtnDrop.pStrTip = lpszText;
	UpdateTooltips();
}

void CElcTabCtrl::SetDropdownButtonTip(UINT nTextId)
{
	if (m_tbtnDrop.pStrTip == 0)
		m_tbtnDrop.pStrTip = new CString;

	m_tbtnDrop.pStrTip->LoadString(nTextId);
	UpdateTooltips();
}

void CElcTabCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (ELC_TAB_STATE_CHECK == nIDEvent) {
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		//if (m_bMirror)
		//	pt.x = m_rcClient.Width() - pt.x;

		int nItem = GetActiveItem(pt);
		if (m_nHotItem != nItem) {
			if (m_nHotItem != -1 && m_nHotItem < (int)m_arrItem.size())
				InvalidateRect(m_arrItem[m_nHotItem].rcItem);
			m_nHotItem = nItem;
		}

		CheckMouseInButton(pt);
	}

	CWnd::OnTimer(nIDEvent);
}

int CElcTabCtrl::PtInItem(CPoint point)
{
	ScreenToClient(&point);

	for (int i=m_nFirstVisibleItem; i <= m_nLastVisibleItem; i++)
	{
		if (m_arrItem[i].rcItem.PtInRect(point)) {
			m_nHotItem = i;
			return i;
		}
	}
	m_nHotItem = -1;

	if (::PtInRect(&m_tbtnNew.rect, point))
		return (int)m_arrItem.size();

	return -1;
}

void CElcTabCtrl::ShowFocusRect(int nItem)
{
	if (m_nFocusRectItem != nItem) {
		m_nFocusRectItem = nItem;
		Invalidate();
	}
}

void CElcTabCtrl::ShowFocusRect(CPoint point)
{
	ShowFocusRect(PtInItem(point));
}

