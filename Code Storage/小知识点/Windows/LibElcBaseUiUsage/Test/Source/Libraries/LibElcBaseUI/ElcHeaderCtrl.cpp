#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcHeaderCtrl.h"


// CElcHeaderCtrl

IMPLEMENT_DYNAMIC(CElcHeaderCtrl, CHeaderCtrl)

CElcHeaderCtrl::CElcHeaderCtrl()
{
	memset(&m_skin, 0, sizeof(m_skin));

	m_skin.crText = RGB(21,66,139);
	m_skin.crTextShadow = RGB(255,255,255);

	m_crBorder = RGB(129,175,236);
	m_crBackgnd = RGB(230,240,250);

	GetDefaultFont(m_ftText);

	m_nHeaderHeight = 24;
}

CElcHeaderCtrl::~CElcHeaderCtrl()
{
}

BOOL CElcHeaderCtrl::LoadSkin(LPCTSTR lpszStyle)
{
	PELC_SKIN_HEADERCTRL pSkin = theSkin.GetHeaderCtrlSkin(lpszStyle);
	if (!pSkin)
		return FALSE;

	memcpy(&m_skin, pSkin, sizeof(m_skin));
	m_nHeaderHeight = CElcSkinDoc::GetImageSize(&m_skin.background).cy + 2;

	return TRUE;
}


BEGIN_MESSAGE_MAP(CElcHeaderCtrl, CHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(HDM_LAYOUT, OnLayout)
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CElcHeaderCtrl message handlers
void CElcHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this);

	GetClientRect(m_rcClient);

	CElcMemDC dcMem(&dc, m_rcClient);
	dcMem.FillSolidRect(m_rcClient, RGB(255,255,255));
	CFont* pOldFont = dcMem.SelectObject(&m_ftText);

	if (m_skin.background.pImage 
		&& m_skin.separator.pImage 
		&& m_skin.sort.pImage)
	{
		DrawSkinHeader(&dcMem);
	}
	else
	{
		DrawSimpleHeader(&dcMem);
	}

	dcMem.SelectObject(pOldFont);
	dcMem.DrawMe();
}

void CElcHeaderCtrl::DrawSimpleHeader(CDC* pDC)
{
	CRect rcClient = m_rcClient;
	rcClient.InflateRect(-1, -1);
	rcClient.right -= 2;

	CRect rcItem;

	pDC->SetTextColor(m_skin.crText);
	pDC->FillSolidRect(rcClient, m_crBackgnd);

	CPen pen(PS_SOLID, 1, m_crBorder);
	pDC->SelectObject(&pen);
	pDC->SelectStockObject(NULL_BRUSH);

	int nItems = GetItemCount();
	for(int i=0; i < nItems; i++)
	{
		TCHAR szText[256] = {0};

		HD_ITEM hditem = {0};
		hditem.iImage = -1;

		hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER | HDI_IMAGE;
		hditem.pszText = szText;
		hditem.cchTextMax = 255;
		GetItem(i, &hditem);
		GetItemRect(i, rcItem);

		if (rcItem.IsRectEmpty())
			continue;

		rcItem.DeflateRect(1, 1, 0, 1);

		pDC->Rectangle(rcItem);
		
		UINT uFormat = DT_SINGLELINE
			| DT_VCENTER
			| DT_END_ELLIPSIS
			| DT_WORDBREAK
			| DT_NOFULLWIDTHCHARBREAK
			| DT_NOPREFIX;

		if ((hditem.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT) {
			uFormat |= DT_LEFT;
		}
		else if ((hditem.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_CENTER) {
			uFormat |= DT_CENTER;
		}
		if ((hditem.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT) {
			uFormat |= DT_RIGHT;
		}

		if (hditem.iImage != -1 
			&& m_skin.sort.pImage) 
		{

			CSize sizeSort = CElcSkinDoc::GetImageSize(&m_skin.sort);

			CRect rcDest;
			rcDest.right = rcItem.right - 4;
			rcDest.left = rcDest.right - sizeSort.cx;
			rcDest.top = rcItem.top + (rcItem.Height() - sizeSort.cy) / 2;
			rcDest.bottom = rcDest.top + sizeSort.cy;

			thePainter.DrawImageGDI(pDC->m_hDC,
				&m_skin.sort,
				rcDest,
				hditem.iImage,
				0,
				0,
				0);

			rcItem.right = rcDest.left - 4;
		}
		else {
			rcItem.right -= 4;
		}

		rcItem.DeflateRect(5, 0);
		pDC->DrawText(szText, rcItem, uFormat);
	}
}

void CElcHeaderCtrl::DrawSkinHeader(CDC* pDC)
{
	CRect rcClient = m_rcClient;
	rcClient.InflateRect(-1, -1);
	rcClient.right -= 2;

	BOOL bMirror = (pDC->GetLayout() == LAYOUT_RTL);

	CRect rcItem;

	int nItems = GetItemCount();
	if (nItems > 0) {
		int nLastItem = OrderToIndex(GetItemCount()-1);
		GetItemRect(nLastItem, rcItem);
		rcItem.left = rcClient.left;
		rcItem.top = rcClient.top;
		rcItem.bottom = rcClient.bottom;
		rcItem.right = max(rcClient.right, rcItem.right);
	}
	else {
		rcItem = rcClient;
	}

	if (bMirror)
		rcItem = MirrorRect(m_rcClient.Width(), rcItem);

	thePainter.DrawImageGDI(pDC->m_hDC,
		&m_skin.background,
		rcItem,
		0,
		0,
		1,
		0);

	CSize sizeSeparator = CElcSkinDoc::GetImageSize(&m_skin.separator);
	CSize sizeSort = CElcSkinDoc::GetImageSize(&m_skin.sort);

	for(int i=0; i < nItems; i++)
	{
		TCHAR szText[256] = {0};

		HD_ITEM hditem = {0};
		hditem.iImage = -1;

		hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER | HDI_IMAGE;
		hditem.pszText = szText;
		hditem.cchTextMax = 255;
		GetItem(i, &hditem);
		GetItemRect(i, rcItem);

		if (rcItem.IsRectEmpty())
			continue;

		CRect rcSeparator;
		rcSeparator.left = rcItem.right - sizeSeparator.cx;
		rcSeparator.top = (rcItem.Height() - sizeSeparator.cy) / 2;
		rcSeparator.right = rcSeparator.left + sizeSeparator.cx;
		rcSeparator.bottom = rcSeparator.top + sizeSeparator.cy;

		thePainter.DrawImageGDI(pDC->m_hDC,
			&m_skin.separator,
			bMirror ? MirrorRect(m_rcClient.Width(), rcSeparator) : rcSeparator,
			0,
			0,
			1,
			0);

		CRect rcText = rcItem;
		rcText.left += 4;
		rcText.right -= 4;

		UINT nFormat = DT_SINGLELINE
			| DT_VCENTER
			| DT_END_ELLIPSIS
			| DT_WORDBREAK
			| DT_NOFULLWIDTHCHARBREAK
			| DT_NOPREFIX;

		if ((hditem.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT) 
			nFormat |= DT_LEFT;
		else if ((hditem.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_CENTER)
			nFormat |= DT_CENTER;
		else if ((hditem.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
			nFormat |= DT_RIGHT;

		if (hditem.iImage != -1 
			&& m_skin.sort.pImage) 
		{
			CRect rcDest;
			rcDest.right = rcItem.right - 4;
			rcDest.left = rcDest.right - sizeSort.cx;
			rcDest.top = rcItem.top + (rcItem.Height() - sizeSort.cy) / 2;
			rcDest.bottom = rcDest.top + sizeSort.cy;

			thePainter.DrawImageGDI(pDC->m_hDC,
				&m_skin.sort,
				bMirror ? MirrorRect(m_rcClient.Width(), rcDest) : rcDest,
				hditem.iImage,
				0,
				0,
				0);

			rcText.right = rcDest.left - 4;
		}

		rcText.OffsetRect(1, 1);
		pDC->SetTextColor(m_skin.crTextShadow);
		pDC->DrawText(szText, rcText, nFormat);
		rcText.OffsetRect(-1, -1);
		pDC->SetTextColor(m_skin.crText);

		pDC->DrawText(szText, rcText, nFormat);
	}
}

BOOL CElcHeaderCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

LRESULT CElcHeaderCtrl::OnLayout(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, wParam, lParam);

	HDLAYOUT& hdLayout = *reinterpret_cast<LPHDLAYOUT>(lParam);
	if (m_nHeaderHeight > 0) 
		hdLayout.prc->top = hdLayout.pwpos->cy = m_nHeaderHeight;

	return lResult;
}

BOOL CElcHeaderCtrl::SetItemImage(int nCol, int nImage)
{
	RemoveSortImage();

	HDITEM hditem = {0};
	hditem.mask = HDI_IMAGE;
	hditem.iImage = nImage;
	return SetItem(nCol, &hditem);
}

int CElcHeaderCtrl::GetItemImage(int nCol)
{
	HDITEM hditem = {0};
	hditem.mask = HDI_IMAGE;
	GetItem(nCol, &hditem);
	return hditem.iImage;
}

void CElcHeaderCtrl::RemoveSortImage()
{
	int nItems = GetItemCount();
	for(int i=0; i < nItems; i++) {
		HDITEM hditem = {0};
		hditem.mask = HDI_IMAGE;
		hditem.iImage = -1;
		SetItem(i, &hditem);
	}
}

void CElcHeaderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CHeaderCtrl::OnMouseMove(nFlags, point);

	if (MK_LBUTTON == nFlags) {
		Invalidate();
	}
}

void CElcHeaderCtrl::OnSize(UINT nType, int cx, int cy)
{
	CHeaderCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}
