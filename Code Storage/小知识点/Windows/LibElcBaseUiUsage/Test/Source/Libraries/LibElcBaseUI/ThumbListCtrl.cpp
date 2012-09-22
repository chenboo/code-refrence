// ThumbListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ThumbListCtrl.h"
#include "GDIPlusSnippets.h"

#define ITEM_INTERNAL_MARGIN	2
#define ITEMS_HMARGIN			10
#define ITEMS_VMARGIN			10
#define SHADOW_WIDTH			5
#define SENSITIVE_WIDTH			20

#define WM_EDIT_LABEL_NOTIFY		(WM_APP + 100)

// Magic. Do not touch.

////////////////////////////////////////////
// CThumbListCtrlEdit

IMPLEMENT_DYNAMIC(CThumbListCtrlEdit, CEdit)

CThumbListCtrlEdit::CThumbListCtrlEdit()
{
	m_hNotifyWnd = 0;
	m_nNotifyMsg = 0;
	m_nNotifyType = ETENT_OK;

	m_nItem = -1;
	m_dwData = 0;
}

CThumbListCtrlEdit::~CThumbListCtrlEdit()
{
}


BEGIN_MESSAGE_MAP(CThumbListCtrlEdit, CEdit)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
END_MESSAGE_MAP()


int CThumbListCtrlEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CThumbListCtrlEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CThumbListCtrlEdit::OnEnChange()
{
	AutoFitSize();
}

void CThumbListCtrlEdit::OnKillFocus(CWnd* pNewWnd)
{
	if (pNewWnd != this && m_hNotifyWnd && m_nNotifyMsg) {
		::PostMessage(m_hNotifyWnd, m_nNotifyMsg, m_nNotifyType, 0);
	}
}

LRESULT CThumbListCtrlEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::WindowProc(message, wParam, lParam);
}

BOOL CThumbListCtrlEdit::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_ESCAPE) {
			m_nNotifyType = ETENT_CANCEL;
			::SetFocus(NULL);
		}
		else if (pMsg->wParam == VK_RETURN) {
			m_nNotifyType = ETENT_OK;
			::SetFocus(NULL);
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

CSize CThumbListCtrlEdit::GetIdeaSize()
{
	CSize size(0,0);

	CDC dcMem;
	if (dcMem.CreateCompatibleDC(NULL)) {
		CFont ftText;
		GetMSShellDlgFont(ftText);
		CFont* pOldFont = dcMem.SelectObject(&ftText);
		CString strText;
		GetWindowText(strText);
		if (!strText.IsEmpty()) {
			size = dcMem.GetTextExtent(strText);
		}
		else {
			size = dcMem.GetTextExtent(_T("A"));
		}
		dcMem.SelectObject(pOldFont);
		size.cx += 10;
		size.cy += 4;
	}

	return size;
}

void CThumbListCtrlEdit::AutoFitSize()
{
	CSize size = GetIdeaSize();
	if (size.cx <= 0 || size.cy <= 0)
		return;
	
	CRect rect;
	GetWindowRect(rect);
	if (rect.IsRectEmpty())
		return;

	if (rect.Width() != size.cx
		|| rect.Height() != size.cy) 
	{
		SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE);
	}
}


////////////////////////////////////////////////
// CThumbListCtrl

IMPLEMENT_DYNAMIC(CThumbListCtrl, CWnd)

CThumbListCtrl::CThumbListCtrl()
{
	m_hNotifyWnd = 0;
	m_nNotifyMsg = 0;

	m_sizeItem.cx = 96;
	m_sizeItem.cy = 96;

	m_rcClient.SetRectEmpty();

	m_crSelect = RGB(49,106,197);
	m_crText = RGB(19,47,71);
	m_crItemBk = RGB(154,190,231);
	m_crItemFrameBorder = RGB(190,198,235);
	m_crItemThumbBorder = RGB(241,244,248);
	m_crCtrlBk = RGB(255,255,255);
	m_crFocus = RGB(64,64,64);

	m_pLogoImage = NULL;
	m_pImageBackgnd = NULL;
	m_pImageList = NULL;

	m_nHorzSpacing = ITEMS_HMARGIN;
	m_nVertSpacing = ITEMS_VMARGIN;
	m_nBorderWidth = 3;
	m_nTitleHeight = 18;
	m_nLabelHeight = 18;

	m_dwButtonState = 0;

	m_pbmItemFrame = NULL;
	m_pCheckBtn = NULL;
	m_pFuncBtn = NULL;
	m_pStatusIcons = NULL;

	GetMSShellDlgFont(m_ftText);

	m_ptDownOld.x = m_ptDownOld.y = 0;
	m_ptMoving.x = m_ptMoving.y = 0;
	m_modifytype = ETIMT_NONE;
	m_bEnabeDragging = TRUE;

	m_prcItems = new CRect[30];
	m_nrcItemsBuffer = 30;
	m_nrcSelItems = 0;

	m_pFocusItem = NULL;

	m_cxStatusIcon = 16;
	m_cyStatusIcon = 16;

	m_bEnableEditLabel = TRUE;
}

CThumbListCtrl::~CThumbListCtrl()
{
	if (m_prcItems)
		delete []m_prcItems;
	m_nrcItemsBuffer = 0;
	m_nrcSelItems = 0;
}


BEGIN_MESSAGE_MAP(CThumbListCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CAPTURECHANGED()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()

	ON_MESSAGE(WM_EDIT_LABEL_NOTIFY, OnEditLabelNotify)
END_MESSAGE_MAP()



// CThumbListCtrl message handlers

int CThumbListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bMirror = IsLayoutRTL();

	if (!m_wndEdit.Create(
		WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|ES_MULTILINE|ES_WANTRETURN, 
		CRect(0,0,0,0), 
		this, 
		100))
		return -1;

	m_wndEdit.SetNotifyWindow(GetSafeHwnd(), WM_EDIT_LABEL_NOTIFY);

	ModifyStyle(0, WS_CLIPCHILDREN);

	CreateItemFrameBitmap();

	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		SetCheckButton(IDR_LIST_CHECK_BOX);
	}

	return 0;
}

BOOL CThumbListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN) {
		if (m_bEnableEditLabel && pMsg->wParam == VK_F2) {
			int nItem = GetNextItem(-1, ETINI_FOCUSED);
			if (nItem != -1)
				EditLabel(nItem);
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

void CThumbListCtrl::SetBackgndColor(COLORREF color)
{
	m_crCtrlBk = color;
	RepaintRect(NULL);
}

COLORREF CThumbListCtrl::GetBackgndColor()
{
	return m_crCtrlBk;
}

void CThumbListCtrl::SetItemBodyColor(COLORREF color)
{
	m_crItemBk = color;
	RepaintRect(NULL);
}

COLORREF CThumbListCtrl::GetItemBodyColor()
{
	return m_crItemBk;
}

void CThumbListCtrl::SetTextColor(COLORREF color)
{
	m_crText = color;
	RepaintRect(NULL);
}

COLORREF CThumbListCtrl::GetTextColor()
{
	return m_crText;
}

void CThumbListCtrl::SetTextFont(CFont *pFont)
{
	ASSERT(pFont);

	m_ftText.DeleteObject();
	LOGFONT lf = {0};
	pFont->GetLogFont(&lf);
	m_ftText.CreateFontIndirect(&lf);

	RepaintRect(NULL);
}

CFont* CThumbListCtrl::GetTextFont()
{
	return &m_ftText;
}

void CThumbListCtrl::SetLogoImage(UINT nId, LPCTSTR lpszType)
{
	if (m_pLogoImage)
		delete m_pLogoImage;
	m_pLogoImage = NULL;

	if (!ImageFromIDResource(nId, lpszType, m_pLogoImage)) {
		if (m_pLogoImage)
			delete m_pLogoImage;
		m_pLogoImage = NULL;
	}

	RepaintRect(NULL);
}

void CThumbListCtrl::SetLogoImage(LPCTSTR lpszStyle)
{
	PELC_SKIN_PICTURE pPicture = GetSkinDoc()->GetPicture(lpszStyle);
	if (!pPicture)
		return;

	if (m_pLogoImage)
		delete m_pLogoImage;
	m_pLogoImage = NULL;

	m_pLogoImage = (Image*)(pPicture->image.pImage);
	m_crCtrlBk = pPicture->crBackgnd;

	RepaintRect(NULL);
}

BOOL CThumbListCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CThumbListCtrl::OnPaint()
{
	CPaintDC dc(this); 

	if (m_rcClient.IsRectEmpty())
		return;

	CElcMemDC dcMem(&dc, m_rcClient);
	DrawItems(&dcMem);
	dcMem.DrawMe();
}

void CThumbListCtrl::DrawItems(CDC * pDC)
{
	Color crCtrlBk, crSelect, crFocus, crText;
	crCtrlBk.SetFromCOLORREF(m_crCtrlBk);
	crSelect.SetFromCOLORREF(m_crSelect);
	crFocus.SetFromCOLORREF(m_crFocus);
	crText.SetFromCOLORREF(m_crText);

	Graphics g(pDC->GetSafeHdc());
	g.Clear(crCtrlBk);

	int nXPos = GetScrollPos(SB_HORZ);
	int nYPos = GetScrollPos(SB_VERT);

	int nMinPos, nMaxPos;
	GetScrollRange(SB_HORZ, &nMinPos, &nMaxPos);

	int nSrcXPos = nXPos;
	if (m_bMirror && nMaxPos > m_rcClient.Width())
		nSrcXPos = nMaxPos - m_rcClient.Width() - nXPos;
	nMaxPos = max(nMaxPos, m_rcClient.Width());

	Rect rcClt(nSrcXPos, nYPos, m_rcClient.Width(), m_rcClient.Height());
	Bitmap bmMem(m_bMirror ? nMaxPos: (rcClt.X + rcClt.Width), 
		rcClt.Y + rcClt.Height);
	Graphics grph(&bmMem);
	grph.Clear(crCtrlBk);

	if (m_pLogoImage) {
		Rect rcDest;
		rcDest.Width = m_pLogoImage->GetWidth();
		rcDest.Height = m_pLogoImage->GetHeight();
		rcDest.X = rcClt.X + (rcClt.Width - rcDest.Width) / 2;
		rcDest.Y = rcClt.Y + (rcClt.Height - rcDest.Height) / 2;
		grph.DrawImage(m_pLogoImage, rcDest, 0, 0, rcDest.Width, rcDest.Height, UnitPixel);
	}

	LOGFONT lf = {0};
	m_ftText.GetLogFont(&lf);
	Font ftText(pDC->m_hDC, &lf);

	StringFormat stringfmt;
	stringfmt.SetAlignment(StringAlignmentCenter);
	stringfmt.SetLineAlignment(StringAlignmentCenter);
	stringfmt.SetFormatFlags(StringFormatFlagsNoWrap);
	stringfmt.SetTrimming(StringTrimmingEllipsisCharacter);
	stringfmt.SetHotkeyPrefix(HotkeyPrefixShow);

	if (!IsFontSmoothingClearType())
		grph.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
	else 
		grph.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	CRect rect;

	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		if ((*it)->rect.bottom <= rcClt.Y
			|| (*it)->rect.top >= rcClt.Y + rcClt.Height
			|| (*it)->rect.right < rcClt.X
			|| (*it)->rect.left > nXPos + rcClt.Width)
			continue;

		rect = m_bMirror ? MirrorRect(nMaxPos, (*it)->rect) : rect = (*it)->rect;
		grph.DrawImage((Image *)m_pbmItemFrame, rect.left, rect.top);

		// title
		//if ((*it)->dwState & ETIS_TITLE && !(*it)->strTitle.IsEmpty()) {
		//	RectF rcText((REAL)(*it)->rcTitle.left, (REAL)(*it)->rcTitle.top,
		//		(REAL)(*it)->rcTitle.Width(), (REAL)(*it)->rcTitle.Height());
		//	grph.DrawString((*it)->strTitle, -1, &ftText, rcText, &stringfmt, &SolidBrush(crText));
		//}

		// thumbnail
		DrawThumbnail(&grph, *it);

		// draw buttons
		DrawButtons(&grph, *it);

		// draw status icons
		DrawStatusIcons(&grph, *it);

		// draw label
		BOOL bItemLabelEditing = (m_wndEdit.IsWindowVisible() && m_wndEdit.m_dwData == (DWORD_PTR)(*it));
		if (bItemLabelEditing) {
			// not need to draw item label
			// do nothing
		}
		else if (((*it)->dwState & ETIS_SELECTED) == 0) {
			rect = m_bMirror ? MirrorRect(nMaxPos, (*it)->rcName) : (*it)->rcName;

			RectF rcText((REAL)rect.left, (REAL)rect.top,
				(REAL)rect.Width(), (REAL)rect.Height());
			grph.DrawString((*it)->strName, -1, &ftText, rcText, &stringfmt, &SolidBrush(crText));
		}
		else {
			Pen pen(crSelect, 3);
			rect = m_bMirror ? MirrorRect(nMaxPos, (*it)->rect) : (*it)->rect;
			rect.bottom = rect.bottom - (*it)->rcName.Height();
			rect.DeflateRect(1, 1);
			grph.DrawRectangle(&pen, rect.left, rect.top, rect.Width()-1, rect.Height()-1);

			rect = m_bMirror ? MirrorRect(nMaxPos, (*it)->rcName) : (*it)->rcName;
			rect.DeflateRect(2, 2);
			//rect.OffsetRect(-1,-1);
			grph.FillRectangle(&SolidBrush(crSelect), rect.left, rect.top, rect.Width(), rect.Height());

			rect.InflateRect(2, 2);
			RectF rcText((REAL)rect.left, (REAL)rect.top,
				(REAL)rect.Width(), (REAL)rect.Height());
			grph.DrawString((*it)->strName, -1, &ftText, rcText, &stringfmt, &SolidBrush(Color::White));
		}

		// draw focus
		if ((*it) == m_pFocusItem && !bItemLabelEditing) {
			rect = m_bMirror ? MirrorRect(nMaxPos, m_pFocusItem->rcName) : m_pFocusItem->rcName;
			rect.DeflateRect(4, 4);
			rect.OffsetRect(-1,-1);
			Pen pen(crFocus);
			pen.SetDashStyle(DashStyleDot);
			grph.DrawRectangle(&pen, rect.left, rect.top, rect.Width(), rect.Height());
		}
	}

	// draw selecting rectangle
	if (m_modifytype == ETIMT_SELECTING) {
		int left = min(m_ptDownOld.x, m_ptMoving.x);
		int top = min(m_ptDownOld.y, m_ptMoving.y);
		int width = abs(m_ptDownOld.x - m_ptMoving.x);
		int height = abs(m_ptDownOld.y - m_ptMoving.y);

		if (m_bMirror)
			left = m_rcClient.Width() - (left + width);

		Pen pen(crSelect);
		grph.DrawRectangle(&pen, left, top, width, height);
	}
	// draw moving items
	else if (m_modifytype == ETIMT_MOVEING) {
		DrawMovingItems(&grph, &m_mapSelItem);
	}

	g.DrawImage(&bmMem, 0, 0, nSrcXPos, rcClt.Y, m_rcClient.Width(), m_rcClient.Height(), UnitPixel);

	ELC_UI_DRAW draw;
	draw.hWnd = m_hWnd;
	draw.type = ELC_UI_DT_DRAWED;
	draw.hdc = pDC->GetSafeHdc();
	draw.rect = m_rcClient;
	GetParent()->SendMessage(WM_ELC_UI_DRAW_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&draw);
}

void CThumbListCtrl::DrawButtons(LPVOID lpGraphics, PELC_THUMBNAIL_ITEM pItem)
{
	int nMinPos, nMaxPos;
	GetScrollRange(SB_HORZ, &nMinPos, &nMaxPos);
	nMaxPos = max(nMaxPos, m_rcClient.Width());

	// function button
	int iconindex = 0;
	if (m_dwButtonState & ETIS_FUNCBTN_VISIBLED) {
		if (pItem->dwState & ETIS_FUNCBTN_CHECKED)
			iconindex = 1;
		else
			iconindex = 0;

		if ((m_dwButtonState & ETIS_FUNCBTN_ENABLE) == 0)
			iconindex = iconindex + 2;

		DrawButton(lpGraphics, m_pFuncBtn, iconindex, m_bMirror ? MirrorRect(nMaxPos, pItem->rcFuncBtn) : pItem->rcFuncBtn);
	}

	// select button
	if (m_dwButtonState & ETIS_SELBTN_VISIBLED) {
		if (pItem->dwState & ETIS_SELBTN_CHECKED)
			iconindex = 1;
		else
			iconindex = 0;

		if ((m_dwButtonState & ETIS_SELBTN_ENABLE) == 0)
			iconindex = iconindex + 2;

		DrawButton(lpGraphics, m_pCheckBtn, iconindex, m_bMirror ? MirrorRect(nMaxPos, pItem->rcCheckBtn) : pItem->rcCheckBtn);
	}
}

void CThumbListCtrl::DrawButton(LPVOID lpGraphics, LPVOID lpImage, int nIndex, CRect rcBtn)
{
	if (!lpGraphics || !lpImage)
		return;

	Graphics * pGraphics = (Graphics * )lpGraphics;
	Image * pImage = (Image *)lpImage;

	int srcwidth = pImage->GetWidth();
	int srcheight = pImage->GetHeight() >> 2;
	int srcy = srcheight * nIndex;

	Rect rc(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height());

	pGraphics->DrawImage(pImage, rc, 0, srcy, srcwidth, srcheight, UnitPixel);
}

void CThumbListCtrl::DrawThumbnail(LPVOID lpGraphics, PELC_THUMBNAIL_ITEM pItem)
{
	int nMinPos, nMaxPos;
	GetScrollRange(SB_HORZ, &nMinPos, &nMaxPos);
	nMaxPos = max(nMaxPos, m_rcClient.Width());

	CRect rect = m_bMirror ? MirrorRect(nMaxPos, pItem->rcThumb) : pItem->rcThumb;

	Graphics *pGraphics = (Graphics * )lpGraphics;
	Rect rcThumb(rect.left, rect.top, rect.Width(), rect.Height());

	if (pItem->nThumbnailIndex != -1) {
		// draw background
		if (m_pImageBackgnd) {
			pGraphics->DrawImage(m_pImageBackgnd, 
				rcThumb,
				0,
				0, 
				m_pImageBackgnd->GetWidth(),
				m_pImageBackgnd->GetHeight(),
				UnitPixel);
		}

		// draw icon
		if (m_pImageList) {
			Image *pImage = (Image *)m_pImageList;

			Rect rcIcon;
			if (rcThumb.Width < m_cxImageUnit
				|| (UINT)rcThumb.Height < pImage->GetHeight())
			{
				rcIcon = rcThumb;
			}
			else {
				rcIcon.X = rect.left + (rect.Width() - m_cxImageUnit) / 2;
				rcIcon.Y = rect.top + (rect.Height() - pImage->GetHeight()) / 2;
				rcIcon.Width = m_cxImageUnit;
				rcIcon.Height = pImage->GetHeight();
			}

			pGraphics->DrawImage(pImage, 
				rcIcon,
				pItem->nThumbnailIndex * m_cxImageUnit,
				0, 
				m_cxImageUnit,
				pImage->GetHeight(),
				UnitPixel);
		}
	}
	else if (pItem->lpThumbnail) {
		int nStride = pItem->rcThumb.Width() * pItem->nThumbnailBits >> 3;
		Bitmap bmThumb(pItem->rcThumb.Width(), 
			pItem->rcThumb.Height(),
			nStride,
			PixelFormat24bppRGB,
			pItem->lpThumbnail);

		pGraphics->DrawImage(&bmThumb, rcThumb, 0, 0, rcThumb.Width, rcThumb.Height, UnitPixel);
	}
	else {
		pGraphics->FillRectangle(&SolidBrush(Color::Black), rcThumb); 
	}
}

void CThumbListCtrl::DrawStatusIcons(LPVOID lpGraphics, PELC_THUMBNAIL_ITEM pItem)
{
	if (!m_pStatusIcons)
		return;

	int nMinPos, nMaxPos;
	GetScrollRange(SB_HORZ, &nMinPos, &nMaxPos);
	nMaxPos = max(nMaxPos, m_rcClient.Width());

	CRect rect = m_bMirror ? MirrorRect(nMaxPos, pItem->rcThumb) : pItem->rcThumb;

	Graphics * pGraphics = (Graphics * )lpGraphics;
	Image * pImage = (Image *)m_pStatusIcons;

	Rect rc;
	int nImageIndex = 0;
	int nXPos = rect.right;

	rc.Width  = m_cxStatusIcon;
	rc.Height = m_cyStatusIcon;
	rc.Y = rect.bottom - ITEM_INTERNAL_MARGIN - m_cyStatusIcon;

	if (pItem->dwState & ETIS_STATE_ICON_1) {
		nImageIndex = pItem->dwImageIndex & 0x000000FF;
		rc.X = nXPos - ITEM_INTERNAL_MARGIN - m_cxStatusIcon;
		nXPos = rc.X;
		if (nImageIndex < 0x000000FF) {
			pGraphics->DrawImage(
				pImage, 
				rc, 
				m_cxStatusIcon * nImageIndex, 
				0, 
				m_cxStatusIcon, 
				m_cyStatusIcon, 
				UnitPixel);
		}
	}

	if (pItem->dwState & ETIS_STATE_ICON_2) {
		nImageIndex = (pItem->dwImageIndex & 0x0000FF00) >> 8;
		rc.X = nXPos - ITEM_INTERNAL_MARGIN - m_cxStatusIcon;
		nXPos = rc.X;
		if (nImageIndex < 0x000000FF) {
			pGraphics->DrawImage(
				pImage, 
				rc, 
				m_cxStatusIcon * nImageIndex, 
				0, 
				m_cxStatusIcon, 
				m_cyStatusIcon, 
				UnitPixel);
		}
	}

	if (pItem->dwState & ETIS_STATE_ICON_3) {
		nImageIndex = (pItem->dwImageIndex & 0x00FF0000) >> 16;
		rc.X = nXPos - ITEM_INTERNAL_MARGIN - m_cxStatusIcon;
		nXPos = rc.X;
		if (nImageIndex < 0x000000FF) {
			pGraphics->DrawImage(
				pImage, 
				rc, 
				m_cxStatusIcon * nImageIndex, 
				0, 
				m_cxStatusIcon, 
				m_cyStatusIcon, 
				UnitPixel);
		}
	}

	if (pItem->dwState & ETIS_STATE_ICON_4) {
		nImageIndex = (pItem->dwImageIndex & 0xFF000000) >> 24;
		rc.X = nXPos - ITEM_INTERNAL_MARGIN - m_cxStatusIcon;
		nXPos = rc.X;
		if (nImageIndex < 0x000000FF) {
			pGraphics->DrawImage(
				pImage, 
				rc, 
				m_cxStatusIcon * nImageIndex, 
				0, 
				m_cxStatusIcon, 
				m_cyStatusIcon, 
				UnitPixel);
		}
	}
}

void CThumbListCtrl::DrawMovingItems(LPVOID lpGraphics, CThumbnailItemMap *pmapItem)
{
	int nMinPos, nMaxPos;
	GetScrollRange(SB_HORZ, &nMinPos, &nMaxPos);
	nMaxPos = max(nMaxPos, m_rcClient.Width());

	Graphics * pGraphics = (Graphics *)lpGraphics;
	ImageAttributes imageattributes;
	imageattributes.SetColorMatrix(&g_TransparentColorMatrix_50);
	CPoint pt = m_ptMoving - m_ptDownOld;

	for (CThumbnailItemMap::iterator it = pmapItem->begin();
		it != pmapItem->end();
		it ++)
	{
		Rect destRect(it->second->rect.left + pt.x,
			it->second->rect.top + pt.y,
			it->second->rect.Width() + 5,
			it->second->rect.Height());

		if (m_bMirror)
			destRect  = MirrorRect(nMaxPos, destRect);

		pGraphics->DrawImage((Image *)m_pbmItemFrame, 
			destRect, 
			0, 
			0,
			destRect.Width,
			destRect.Height,
			UnitPixel,
			&imageattributes);
	}
}

void CThumbListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0)
		return;

	GetClientRect(m_rcClient);

	ResetScrollInfo();
}

void CThumbListCtrl::SetThumbSize(CSize size)
{
	if (m_sizeThumb == size)
		return;

	m_sizeThumb = size;

	m_sizeItem.cx = size.cx + m_nBorderWidth + m_nBorderWidth;
	m_sizeItem.cy = m_nBorderWidth + size.cy + m_nBorderWidth + m_nLabelHeight;

	m_nHorzSpacing = m_sizeItem.cx / 10;
	m_nVertSpacing = m_sizeItem.cy / 10;

	if (m_nHorzSpacing < ITEMS_HMARGIN)
		m_nHorzSpacing = ITEMS_HMARGIN;
	if (m_nVertSpacing < ITEMS_VMARGIN)
		m_nVertSpacing = ITEMS_VMARGIN;

	CreateItemFrameBitmap();

	ResizeItems();

	ResetScrollInfo();

	Invalidate();
}

inline CSize CThumbListCtrl::GetThumbSize()
{
	return m_sizeThumb;
}

inline CSize CThumbListCtrl::GetItemSize()
{
	return m_sizeItem;
}

void CThumbListCtrl::ClickButton(int nItem, DWORD dwType, DWORD dwState)
{
	ELC_THUMBNAIL_LIST_NOTIFY notify;
	notify.hwndFrom = m_hWnd;
	notify.nCtrlId = GetDlgCtrlID();
	notify.nItem = nItem;
	notify.item = *m_listItem[nItem];

	::SendMessage(m_hNotifyWnd, m_nNotifyMsg, dwType, (LPARAM)&notify);
}

void CThumbListCtrl::MouseDown(UINT nFlags, CPoint point)
{
	point.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));

	SetFocus();

	int nItem, nHitType;

	if (GetHitTest(point, nItem, nHitType)) 
	{
		if (nHitType == EHT_FUNCBTN 
			&& (m_dwButtonState & ETIS_FUNCBTN_VISIBLED) == 0)
		{
			nHitType = EHT_ITEM;
		}
		else if (nHitType == EHT_SELECT_BTN 
			&& (m_dwButtonState & ETIS_SELBTN_VISIBLED) == 0)
		{
			nHitType = EHT_ITEM;
		}

		switch (nHitType)
		{
		case EHT_FUNCBTN:
			{
				if (m_dwButtonState & ETIS_FUNCBTN_ENABLE)
					ClickButton(nItem, ETCN_FUNC_BTN_CLICKED, ETIS_FUNCBTN_CHECKED);
			}
			break;
		case EHT_SELECT_BTN:
			{
				if (m_dwButtonState & ETIS_SELBTN_ENABLE)
					ClickButton(nItem, ETCN_SEL_BTN_CHECKED, ETIS_SELBTN_CHECKED);
			}
			break;
		case EHT_LABEL:
			{
				if (m_listItem[nItem]->dwState & ETIS_SELECTED) {
					CRgn rgn, rgn2;

					ClearSelItem(rgn);
					AddSelItem(m_listItem[nItem]);

					rgn2.CreateRectRgn(m_listItem[nItem]->rect.left,
						m_listItem[nItem]->rect.top,
						m_listItem[nItem]->rect.right,
						m_listItem[nItem]->rect.bottom);
					rgn.CombineRgn(&rgn, &rgn2, RGN_OR);
					RepaintRgn(&rgn);

					// modify label
					if ((nFlags & MK_LBUTTON) != 0
						&& m_bEnableEditLabel)
						m_modifytype = ETIMT_EDITLABEL;

					PostSelectChangedNotify();

					break;
				}
			}
		case EHT_ITEM:
			{
				PELC_THUMBNAIL_ITEM pItem = m_listItem[nItem];
				if (pItem->dwState & ETIS_SELECTED) {
					if (MK_CONTROL & nFlags) {
						RemoveSelItem(pItem);
						PostSelectChangedNotify();
					}
					else {
						SetCapture();
						m_modifytype = ETIMT_HITTING;
						m_ptDownOld = point;
						m_ptMoving = point;
						GetSelItemsRect();
					}
				}
				else {
					if (MK_CONTROL & nFlags) {
						AddSelItem(pItem);
					}
					else {
						CRgn rgn;
						ClearSelItem(rgn);
						AddSelItem(pItem);
						MoveItemToTail(nItem);
						RepaintRgn(&rgn);

						SetCapture();
						m_modifytype = ETIMT_HITTING;
						m_ptDownOld = point;
						m_ptMoving = point;
						GetSelItemsRect();
					}
					PostSelectChangedNotify();
				}

				SetFocusItem(pItem);
				RepaintRect(pItem->rect);
			}
			break;
		}
	}
	else {
		m_modifytype = ETIMT_SELECTING;
		m_ptDownOld = point;
		m_ptMoving = point;

		CRgn rgn;
		ClearSelItem(rgn);
		RepaintRgn(&rgn);

		SetCapture();

		PostSelectChangedNotify();
	}
}

void CThumbListCtrl::MouseUp(UINT nFlags, CPoint point)
{
	point.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));

	if (m_modifytype == ETIMT_EDITLABEL
		&& m_bEnableEditLabel) 
	{
		int nItem, nHitType;
		if (GetHitTest(point, nItem, nHitType)
			&& nHitType == EHT_LABEL) 
		{
			EditLabel(nItem);
		}
		return;
	}

	if (CWnd::GetCapture() != this) {
		return;
	}

	if (m_modifytype == ETIMT_SELECTING) {
		CRect rc;
		rc.left = min(m_ptDownOld.x, m_ptMoving.x);
		rc.top = min(m_ptDownOld.y, m_ptMoving.y);
		rc.right = rc.left + abs(m_ptDownOld.x - m_ptMoving.x);
		rc.bottom = rc.top + abs(m_ptDownOld.y - m_ptMoving.y);

		CalcSelItems(rc);

		MouseNotify(ELC_MOUSE_LBUTTON_UP, nFlags);
	}
	else if (m_modifytype == ETIMT_MOVEING) {
		CPoint pt = m_ptMoving - m_ptDownOld;
		if (pt.x !=0 && pt.y != 0) 
		{
			if (MouseNotify(ELC_MOUSE_LBUTTON_UP, nFlags) == 0
				&& m_bEnabeDragging)
			{
				CRgn rgn;
				CPoint ptLT, ptRB;
				OffsetSelItems(pt, rgn, &ptLT, &ptRB);

				ptLT.Offset(-m_nHorzSpacing, -m_nVertSpacing);
				ptRB.Offset(m_nHorzSpacing, m_nVertSpacing);

				if (ptLT.x < 0 || ptLT.y < 0) {
					if (ptLT.x < 0)
						ptLT.x = -ptLT.x;
					else
						ptLT.x = 0;
					if (ptLT.y < 0)
						ptLT.y = -ptLT.y;
					else
						ptLT.y = 0;

					rgn.OffsetRgn(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));
					OffsetAllItems(ptLT);
				}

				ResetScrollInfo();
			}
		}
	}
	else if (m_modifytype == ETIMT_HITTING) {
		int nItem, nHitType;
		GetHitTest(point, nItem, nHitType);
		if (nItem != -1) {
			CRgn rgn;
			ClearSelItem(rgn, m_listItem[nItem]);
			RepaintRgn(&rgn);
			PostSelectChangedNotify();
		}

		MouseNotify(ELC_MOUSE_LBUTTON_UP, nFlags);
	}

	ReleaseCapture();

	Invalidate();
}

void CThumbListCtrl::OnCaptureChanged(CWnd *pWnd)
{
	m_ptDownOld.x = m_ptDownOld.y = 0;
	m_ptMoving.x = m_ptMoving.y = 0;
	m_modifytype = ETIMT_NONE;

	CWnd::OnCaptureChanged(pWnd);
}

void CThumbListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bCtrl = ((GetKeyState(VK_CONTROL) & (1<<(sizeof(SHORT)*8-1))) != 0);
	switch (nChar)
	{
	case 'A':
	case 'a':
		{
			if (bCtrl) {
				SetAllItemsState(ETIS_SELECTED, ETIS_SELECTED);
			}
		}
		break;
	case VK_LEFT:
	case VK_UP:
	case VK_RIGHT:
	case VK_DOWN:
		SelectNext(nChar);
		break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CThumbListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int nScrollPos = GetScrollPos(SB_VERT);
	if (zDelta < 0)
		nScrollPos += (m_sizeItem.cy + m_nVertSpacing);
	else 
		nScrollPos -= (m_sizeItem.cy + m_nVertSpacing);

	ScrollBarPos(SB_VERT, nScrollPos);

	Invalidate();

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CThumbListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nItem, nHitType;
	CPoint ptOffset = point;
	ptOffset.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));

	if (GetHitTest(ptOffset, nItem, nHitType)
		&& (nHitType == EHT_ITEM || nHitType == EHT_LABEL)) 
	{
		ELC_THUMBNAIL_LIST_NOTIFY notify;
		notify.hwndFrom = m_hWnd;
		notify.nCtrlId = GetDlgCtrlID();
		notify.nItem = nItem;
		notify.item = *m_listItem[nItem];
		::SendMessage(m_hNotifyWnd, m_nNotifyMsg, ETCN_ITEM_DBLCLK, (LPARAM)&notify);
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CThumbListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_RBUTTON) == 0)
		MouseDown(nFlags, point);

	CWnd::OnLButtonDown(nFlags, point);
}

void CThumbListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_RBUTTON) == 0) {
		MouseUp(nFlags, point);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CThumbListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	point.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));

	if (m_modifytype == ETIMT_NONE) {
	}
	else if (m_modifytype == ETIMT_SELECTING) {
		m_ptMoving = point;

		CRect rc;
		rc.left = min(m_ptDownOld.x, m_ptMoving.x);
		rc.top = min(m_ptDownOld.y, m_ptMoving.y);
		rc.right = rc.left + abs(m_ptDownOld.x - m_ptMoving.x);
		rc.bottom = rc.top + abs(m_ptDownOld.y - m_ptMoving.y);

		CalcSelItems(rc);

		Invalidate();
	}
	else if (m_modifytype == ETIMT_HITTING
		|| m_modifytype == ETIMT_MOVEING) 
	{
		MouseNotify(ELC_MOUSE_MOVE, nFlags);
		m_modifytype = ETIMT_MOVEING;
		m_ptMoving = point;
		Invalidate();
	}
	else if (m_modifytype == ETIMT_EDITLABEL)
	{
		m_modifytype = ETIMT_NONE;
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CThumbListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == 0) {
		CPoint ptOffset = point;
		ptOffset.Offset(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
		int nItem, nHitType;
		if (GetHitTest(ptOffset, nItem, nHitType)) 
		{
			if (nHitType == EHT_FUNCBTN 
				&& (m_dwButtonState & ETIS_FUNCBTN_VISIBLED) == 0)
			{
				nHitType = EHT_ITEM;
			}
			else if (nHitType == EHT_SELECT_BTN 
				&& (m_dwButtonState & ETIS_SELBTN_VISIBLED) == 0)
			{
				nHitType = EHT_ITEM;
			}

			if (nHitType == EHT_ITEM) {

				if (m_listItem[nItem]->dwState & ETIS_SELECTED) {
				}
				else {
					CRgn rgn;
					ClearSelItem(rgn);
					AddSelItem(m_listItem[nItem]);
					RepaintRgn(&rgn);
					PostSelectChangedNotify();
				}

				SetFocusItem(m_listItem[nItem]);
				RepaintRect(m_listItem[nItem]->rect);
			}
		}
		else {
			CRgn rgn;
			ClearSelItem(rgn);
			RepaintRgn(&rgn);
			PostSelectChangedNotify();

			MouseDown(nFlags, point);
		}
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CThumbListCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == 0) {
		MouseUp(nFlags, point);
	}

	CWnd::OnRButtonUp(nFlags, point);
}

void CThumbListCtrl::CreateItemFrameBitmap()
{
	if (m_pbmItemFrame)
		delete (Bitmap *)m_pbmItemFrame;
	m_pbmItemFrame = NULL;

	Rect rect(0, 0, m_sizeItem.cx, m_sizeItem.cy - m_nLabelHeight);
	Bitmap * pImage = new Bitmap(rect.Width+SHADOW_WIDTH, rect.Height+SHADOW_WIDTH); 

	Graphics graphics(pImage);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	int nRoundDiameter = 6;

	Rect rc1(rect.X, rect.Y, nRoundDiameter, nRoundDiameter); // left-top
	Rect rc2(rect.X + rect.Width - nRoundDiameter, rect.Y, nRoundDiameter, nRoundDiameter); // right-top
	Rect rc3(rect.X + rect.Width - nRoundDiameter, rect.Y + rect.Height - nRoundDiameter, nRoundDiameter, nRoundDiameter); // right-bottom
	Rect rc4(rect.X, rect.Y + rect.Height - nRoundDiameter, nRoundDiameter, nRoundDiameter); // left-bottom

	GraphicsPath grph;

	grph.AddArc(rc1, 180, 90);
	grph.AddLine(rc1.X + rc1.Width/2, rc1.Y, rc2.X + rc2.Width/2, rc2.Y);

	grph.AddArc(rc2, 270, 90);
	grph.AddLine(rc2.X + rc2.Width, rc2.Y + rc2.Height/2, rc3.X + rc3.Width, rc3.Y + rc3.Height/2);

	grph.AddArc(rc3, 0, 90);
	grph.AddLine(rc3.X + rc3.Width/2, rc3.Y + rc3.Height, rc4.X + rc4.Width/2, rc4.Y + rc4.Height);

	grph.AddArc(rc4, 90, 90);
	grph.AddLine(rc4.X, rc4.Y + rc4.Height/2, rc1.X, rc1.Y + rc1.Height/2);

	Color crItemBk;
	crItemBk.SetFromCOLORREF(m_crItemBk);

	SolidBrush brush(crItemBk);
	graphics.FillPath(&brush, &grph);

	Color crItemBorder;
	crItemBorder.SetFromCOLORREF(m_crItemFrameBorder);

	Pen pen(crItemBorder, 1);
	graphics.DrawPath(&pen, &grph);

	Region ItemRegion(&grph);
	Matrix ShadowMatrix;
	ShadowMatrix.Translate(3, 3);
	grph.Transform(&ShadowMatrix);
	Region ShadowRegion(&grph);
	ShadowRegion.Exclude(&ItemRegion);

	PathGradientBrush brGB(&grph);
	brGB.SetCenterColor(Color(0,0,0));
	Color colors[] = {Color(0, 0, 0, 0)};
	int count = 1;
	brGB.SetSurroundColors(colors, &count);
	brGB.SetFocusScales(0.75f, 0.75f);

	graphics.FillRegion(&brGB, &ShadowRegion);

	rect.X = m_nBorderWidth;
	rect.Y = m_nBorderWidth;
	rect.Width = m_sizeThumb.cx;
	rect.Height = m_sizeThumb.cy;
	graphics.SetSmoothingMode(SmoothingModeDefault);
	graphics.FillRectangle(&SolidBrush(Color::Black), rect);

	Color crThumbBorder;
	crThumbBorder.SetFromCOLORREF(m_crItemThumbBorder);
	pen.SetColor(crThumbBorder);
	rect.Inflate(1,1);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.DrawRectangle(&pen, rect);

	m_pbmItemFrame = pImage;
}

inline PELC_THUMBNAIL_ITEM CThumbListCtrl::FindItem(int nItem)
{
	if (nItem < 0 || nItem >= (int)m_listItem.size()) {
		ASSERT(0);
		return NULL;
	}

	return m_listItem[nItem]; 
}

inline int CThumbListCtrl::FindItem(PELC_THUMBNAIL_ITEM pItem)
{
	int nIndex = 0;
	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++, nIndex ++)
	{
		if ((*it) == pItem)
			return nIndex;
	}

	return -1;
}

int CThumbListCtrl::InsertItem(int nItem, LPCTSTR lpszItem, LPBYTE lpThumbnail, int	nThumbnailBits, POINT ptPosition, DWORD dwState)
{
	if (m_wndEdit.IsWindowEnabled())
		m_wndEdit.ShowWindow(SW_HIDE);

	PELC_THUMBNAIL_ITEM pItem = new ELC_THUMBNAIL_ITEM;
	pItem->strName = lpszItem;
	pItem->rect.SetRect(0, 0, m_sizeItem.cx, m_sizeItem.cy);
	pItem->lpThumbnail = lpThumbnail;
	pItem->nThumbnailBits = nThumbnailBits;
	pItem->nThumbnailIndex = -1;
	pItem->dwImageIndex = 0xFFFFFFFF;
	pItem->dwState = dwState;

	if (ptPosition.x == -1 || ptPosition.y == -1) {
		pItem->rect.OffsetRect(FindIdealInsertPosition());
	}
	else {
		pItem->rect.OffsetRect(ptPosition);
	}

	CalcItemSize(pItem, pItem->rect);

	// insert position
	if (nItem < (int)m_listItem.size()) {
		if (nItem < 0)
			nItem = 0;
		m_listItem.insert(m_listItem.begin() + nItem, pItem);
	}
	else {
		m_listItem.push_back(pItem);
		nItem = (int)(m_listItem.size() - 1);
	}

	if (m_pFocusItem == NULL)
		SetFocusItem(pItem);

	CRect rect = pItem->rect;
	rect.InflateRect(SHADOW_WIDTH, 0);
	RepaintRect(rect);

	ResetScrollInfo();

	return nItem;
}

void CThumbListCtrl::DeleteItem(int nItem)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) 
	{
		if (m_wndEdit.IsWindowEnabled()
			&& pItem == (PELC_THUMBNAIL_ITEM)m_wndEdit.m_dwData) 
		{
			m_wndEdit.m_dwData = NULL;
			m_wndEdit.ShowWindow(SW_HIDE);
		}

		if (m_pFocusItem == pItem) {
			if (nItem == 0) {
				if (nItem + 1 < (int)m_listItem.size())
					SetFocusItem(m_listItem[nItem + 1]);
			}
			else {
				SetFocusItem(m_listItem[nItem - 1]);
			}
		}

		if (pItem->dwState & ETIS_SELECTED) {
			RemoveSelItem(pItem);
		}
		m_listItem.erase(m_listItem.begin() + nItem);

		CRect rect = pItem->rect;
		rect.right += SHADOW_WIDTH;
		rect.bottom += SHADOW_WIDTH;
		delete pItem;

		RepaintRect(rect);

		PostSelectChangedNotify();

		ResetScrollInfo();
	}
}

void CThumbListCtrl::DeleteAllItems()
{
	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		delete (*it);
	}

	m_listItem.clear();
	m_mapSelItem.clear();
	m_pFocusItem = NULL;
	ResetScrollInfo();

	if (m_wndEdit.IsWindowEnabled())
		m_wndEdit.ShowWindow(SW_HIDE);

	Invalidate();
}

BOOL CThumbListCtrl::SetThumbnailBackground(UINT nResId, LPCTSTR lpszType)
{
	if (!m_pImageBackgnd)
		delete m_pImageBackgnd;
	m_pImageBackgnd = NULL;
	return ImageFromIDResource(nResId, lpszType, m_pImageBackgnd);
}

void CThumbListCtrl::SetThumbnailImageList(UINT nResId, LPCTSTR lpszType, int cxUnit)
{
	Image * pImage = NULL;
	if (ImageFromIDResource(nResId, lpszType, pImage)) {
		m_pImageList = pImage;
		m_cxImageUnit = cxUnit;
	}
	else {
		m_pImageList = 0;
		m_cxImageUnit = 0;
	}
}

void CThumbListCtrl::SetItemThumbnail(int nItem, int nIndex)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		pItem->lpThumbnail = NULL;
		pItem->nThumbnailBits = 0;
		pItem->nThumbnailIndex = nIndex;
		RepaintRect(pItem->rect);
	}
}

void CThumbListCtrl::SetItemThumbnail(int nItem, LPBYTE lpThumbnail, int nThumbnailBits)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		pItem->lpThumbnail = lpThumbnail;
		pItem->nThumbnailBits = nThumbnailBits;
		pItem->nThumbnailIndex = -1;
		RepaintRect(pItem->rect);
	}
}

void CThumbListCtrl::SetItemPosition(int nItem, POINT pt)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		CPoint ptOffset;
		ptOffset.x = pt.x - pItem->rect.left;
		ptOffset.y = pt.y - pItem->rect.top;
		OffsetItem(pItem, ptOffset);
	}

	ResetScrollInfo();
}

BOOL CThumbListCtrl::GetItemPosition(int nItem, LPPOINT lpPoint) 
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		lpPoint->x = pItem->rect.left;
		lpPoint->y = pItem->rect.top;
		return TRUE;
	}
	return FALSE;
}

BOOL CThumbListCtrl::SetItemData(int nItem, DWORD_PTR dwData)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		pItem->dwData = dwData;
		return TRUE;
	}
	return FALSE;
}

DWORD_PTR CThumbListCtrl::GetItemData(int nItem)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		return pItem->dwData;
	}
	return 0;
}

void CThumbListCtrl::AutoArrange()
{
	if (m_rcClient.IsRectEmpty())
		return;

	CRect rc(m_nHorzSpacing, m_nVertSpacing, 0, 0), rcTemp;

	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		if (rc.right + m_nHorzSpacing + m_sizeItem.cx + m_nHorzSpacing <= m_rcClient.right) {
			rcTemp.left = rc.right + m_nHorzSpacing;
			rcTemp.right = rcTemp.left + m_sizeItem.cx;
			rcTemp.top = rc.top;
			rcTemp.bottom = rcTemp.top + m_sizeItem.cy;
		}
		else {
			rcTemp.left = m_rcClient.left + m_nHorzSpacing;
			rcTemp.right = rcTemp.left + m_sizeItem.cx;
			rcTemp.top = rc.bottom + m_nVertSpacing;
			rcTemp.bottom = rcTemp.top + m_sizeItem.cy;
		}

		int xOffset = rcTemp.left-(*it)->rect.left;
		int yoffset = rcTemp.top-(*it)->rect.top;
		(*it)->rcFuncBtn.OffsetRect(xOffset, yoffset);
		(*it)->rcCheckBtn.OffsetRect(xOffset, yoffset);
		(*it)->rcTitle.OffsetRect(xOffset, yoffset);
		(*it)->rcThumb.OffsetRect(xOffset, yoffset);
		(*it)->rcName.OffsetRect(xOffset, yoffset);
		(*it)->rect = rcTemp;

		rc = rcTemp;
	}

	ResetScrollInfo();

	Invalidate();
}

void CThumbListCtrl::AlignSelItems(BOOL bHorizontal)
{
	// find the leftmost rect (topmost rect)
	int nPos = 0x0fffffff;
	for (CThumbnailItemMap::iterator it = m_mapSelItem.begin();
		it != m_mapSelItem.end();
		it ++)
	{
		if (bHorizontal) 
			nPos = min(it->second->rect.top, nPos);
		else
			nPos = min(it->second->rect.left, nPos);
	}

	for (CThumbnailItemMap::iterator it = m_mapSelItem.begin();
		it != m_mapSelItem.end();
		it ++)
	{
		if (bHorizontal) 
			OffsetItem(it->second, CPoint(0, nPos-it->second->rect.top));
		else
			OffsetItem(it->second, CPoint(nPos-it->second->rect.left, 0));
	}

	ResetScrollInfo();

	Invalidate();
}

void CThumbListCtrl::AlignToGrid()
{
	CPoint pt;
	int nXUint = m_sizeItem.cx + m_nHorzSpacing;
	int nYUint = m_sizeItem.cx + m_nVertSpacing;

	BOOL bChanged = FALSE;

	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		pt.x = (*it)->rect.left - m_nHorzSpacing;
		pt.y = (*it)->rect.top - m_nVertSpacing;
		if (pt.x % nXUint || pt.y % nYUint)
		{
			pt.x = nXUint * ((pt.x + (nXUint>>1)) / nXUint) + m_nHorzSpacing - (*it)->rect.left;
			pt.y = nYUint * ((pt.y + (nYUint>>1)) / nYUint) + m_nVertSpacing - (*it)->rect.top;
			OffsetItem((*it), pt);

			bChanged = TRUE;
		}
	}

	if (bChanged) {
		ResetScrollInfo();
		Invalidate();
	}
}

void CThumbListCtrl::AlignSelItemToGrid()
{
	CPoint pt;
	int nXUint = m_sizeItem.cx + m_nHorzSpacing;
	int nYUint = m_sizeItem.cx + m_nVertSpacing;

	BOOL bChanged = FALSE;

	for (CThumbnailItemMap::iterator it = m_mapSelItem.begin();
		it != m_mapSelItem.end();
		it ++)
	{
		pt.x = it->second->rect.left - m_nHorzSpacing;
		pt.y = it->second->rect.top - m_nVertSpacing;
		if (pt.x % nXUint || pt.y % nYUint)
		{
			pt.x = nXUint * ((pt.x + (nXUint>>1)) / nXUint) + m_nHorzSpacing - it->second->rect.left;
			pt.y = nYUint * ((pt.y + (nYUint>>1)) / nYUint) + m_nVertSpacing - it->second->rect.top;
			OffsetItem(it->second, pt);

			bChanged = TRUE;
		}
	}

	if (bChanged) {
		ResetScrollInfo();
		Invalidate();
	}
}

void CThumbListCtrl::ResizeItems()
{
	CRect rect;

	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		rect.left = (*it)->rect.left;
		rect.top = (*it)->rect.top;
		rect.right = rect.left + m_sizeItem.cx;
		rect.bottom = rect.top + m_sizeItem.cy;

		CalcItemSize(*it, rect);
	}
}

void CThumbListCtrl::CalcItemSize(PELC_THUMBNAIL_ITEM pItem, CRect rect)
{
	pItem->rect = rect;

	pItem->rcName.left = rect.left;
	pItem->rcName.right = rect.right;
	pItem->rcName.bottom = rect.bottom;
	pItem->rcName.top = pItem->rcName.bottom - m_nLabelHeight;

	pItem->rcThumb.left = rect.left + m_nBorderWidth;
	pItem->rcThumb.right = rect.right - m_nBorderWidth;
	pItem->rcThumb.top = rect.top + m_nBorderWidth;
	pItem->rcThumb.bottom = pItem->rcName.top-m_nBorderWidth;

	int cx = 13, cy = 13;
	Image * pImage = (Image *)m_pFuncBtn;
	if (pImage) {
		cx = pImage->GetWidth();
		cy = pImage->GetHeight()>>2;
	}

	pItem->rcFuncBtn.left = pItem->rcThumb.left + ITEM_INTERNAL_MARGIN;
	pItem->rcFuncBtn.top = pItem->rcThumb.top + ITEM_INTERNAL_MARGIN;
	pItem->rcFuncBtn.right = pItem->rcFuncBtn.left + cx;
	pItem->rcFuncBtn.bottom = pItem->rcFuncBtn.top + cy;

	cx = cy = 13;
	pImage = (Image *)m_pCheckBtn;
	if (pImage) {
		cx = pImage->GetWidth();
		cy = pImage->GetHeight()>>2;
	}

	pItem->rcCheckBtn.right = pItem->rcThumb.right - ITEM_INTERNAL_MARGIN;
	pItem->rcCheckBtn.top = pItem->rcThumb.top + ITEM_INTERNAL_MARGIN;
	pItem->rcCheckBtn.left = pItem->rcCheckBtn.right - cx;
	pItem->rcCheckBtn.bottom = pItem->rcCheckBtn.top + cy;

	pItem->rcTitle.left = pItem->rcFuncBtn.right + ITEM_INTERNAL_MARGIN;
	pItem->rcTitle.right = pItem->rcCheckBtn.left - ITEM_INTERNAL_MARGIN;
	pItem->rcTitle.top = rect.top;
	pItem->rcTitle.bottom = rect.top + m_nTitleHeight;
}

void CThumbListCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	DeleteAllItems();

	if (m_pbmItemFrame)
		delete (Bitmap *)m_pbmItemFrame;
	if (m_pCheckBtn)
		delete (Bitmap *)m_pCheckBtn;
	if (m_pFuncBtn)
		delete (Bitmap *)m_pFuncBtn;
	if (m_pStatusIcons)
		delete (Bitmap *)m_pStatusIcons;
}

void CThumbListCtrl::UpdateItem(int nItem)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		RepaintRect(pItem->rect);
	}
}

void CThumbListCtrl::UpdateItemThumbnail(int nItem)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		RepaintRect(pItem->rcThumb);
	}
}

void CThumbListCtrl::UpdateItemThumbnail(int nItem, LPRECT lpRect)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		CRect rect = lpRect;
		rect.OffsetRect(pItem->rcThumb.left, pItem->rcThumb.top);
		RepaintRect(rect);
	}
}

void CThumbListCtrl::UpdateAllItem()
{
	Invalidate();
}

BOOL CThumbListCtrl::SetStatusImageListRes(UINT nResId, LPCTSTR lpszType, int cxUnit)
{
	Image * pImage = NULL;
	ImageFromIDResource(nResId, lpszType, pImage);
	m_pStatusIcons = pImage;

	m_cyStatusIcon = pImage->GetHeight();
	m_cxStatusIcon = cxUnit;

	return (m_pStatusIcons != NULL);
}

void CThumbListCtrl::SetItemStatusImage(int nItem, DWORD dwStatusIndex, int nImage, BOOL bRedraw)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		dwStatusIndex &= ETIS_STATE_ICON_MASK;
		//pItem->dwState &= ~dwStatusIndex;
		//pItem->dwState |= dwStatusIndex;

		nImage &= 0x000000FF;
		if (dwStatusIndex & ETIS_STATE_ICON_1) {
			pItem->dwImageIndex &= 0xFFFFFF00;
			pItem->dwImageIndex |= nImage;
		}
		else if (dwStatusIndex & ETIS_STATE_ICON_2) {
			pItem->dwImageIndex &= 0xFFFF00FF;
			pItem->dwImageIndex |= (nImage << 8);
		}
		else if (dwStatusIndex & ETIS_STATE_ICON_3) {
			pItem->dwImageIndex &= 0xFF00FFFF;
			pItem->dwImageIndex |= (nImage << 16);
		}
		else if (dwStatusIndex & ETIS_STATE_ICON_4) {
			pItem->dwImageIndex &= 0x00FFFFFF;
			pItem->dwImageIndex |= (nImage << 24);
		}

		if (bRedraw)
			RepaintRect(pItem->rect);
	}
}

DWORD CThumbListCtrl::GetItemStatusIcon(int nItem)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem)
		return (pItem->dwState & ETIS_STATE_ICON_MASK);
	return 0;
}

void CThumbListCtrl::SetAllItemsStatusIcons(DWORD dwStatusIndex, int nImage, BOOL bRedraw)
{
	int nCount = (int)m_listItem.size();
	for (int i=0; i < nCount; i ++)
	{
		SetItemStatusImage(i, dwStatusIndex, nImage, FALSE);
	}

	if (bRedraw)
		Invalidate();
}

int CThumbListCtrl::GetFocusItem()
{
	return FindItem(m_pFocusItem);
}

BOOL CThumbListCtrl::SetCheckButton(UINT nResId, LPCTSTR lpszType)
{
	Image * pImage = NULL;
	ImageFromIDResource(nResId, lpszType, pImage);
	m_pCheckBtn = pImage;

	return (m_pCheckBtn != NULL);
}

void CThumbListCtrl::ShowCheckButton(BOOL bShow)
{
	if (bShow)
		m_dwButtonState |= ETIS_SELBTN_VISIBLED;
	else
		m_dwButtonState &= ~ETIS_SELBTN_VISIBLED;

	RepaintRect(NULL);
}

BOOL CThumbListCtrl::IsCheckButtonVisible()
{
	return (m_dwButtonState & ETIS_SELBTN_VISIBLED) != 0;
}

void CThumbListCtrl::EnableCheckButton(BOOL bEnabled)
{
	if (bEnabled)
		m_dwButtonState |= ETIS_SELBTN_ENABLE;
	else
		m_dwButtonState &= ~ETIS_SELBTN_ENABLE;

	RepaintRect(NULL);
}

BOOL CThumbListCtrl::IsCheckButtonEnabled()
{
	return (m_dwButtonState & ETIS_SELBTN_ENABLE) != 0;
}

void CThumbListCtrl::SetItemCheck(int nItem, BOOL bChecked, BOOL bRedraw)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		if (bChecked) {
			pItem->dwState |= ETIS_SELBTN_CHECKED;
		}
		else {
			pItem->dwState &= ~ETIS_SELBTN_CHECKED;
		}

		if (bRedraw)
			RepaintRect(pItem->rcCheckBtn);
	}
}

BOOL CThumbListCtrl::IsItemChecked(int nItem)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem)
		return ((pItem->dwState & ETIS_SELBTN_CHECKED) != 0);
	return FALSE;
}

void CThumbListCtrl::CheckAllItems(BOOL bChecked)
{
	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		if (bChecked)
			(*it)->dwState |= ETIS_SELBTN_CHECKED;
		else
			(*it)->dwState &= ~ETIS_SELBTN_CHECKED;
	}

	Invalidate();
}

int CThumbListCtrl::GetCheckedItemCount()
{
	int nCount = 0;
	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		if ((*it)->dwState & ETIS_SELBTN_CHECKED)
			nCount ++;
	}

	return nCount;
}

BOOL CThumbListCtrl::SetFuncButton(UINT nResId, LPCTSTR lpszType)
{
	Image * pImage = NULL;
	ImageFromIDResource(nResId, lpszType, pImage);
	m_pFuncBtn = pImage;

	return (m_pFuncBtn != NULL);
}

void CThumbListCtrl::ShowFuncButton(BOOL bShow)
{
	if (bShow)
		m_dwButtonState |= ETIS_FUNCBTN_VISIBLED;
	else
		m_dwButtonState &= ~ETIS_FUNCBTN_VISIBLED;

	RepaintRect(NULL);
}

BOOL CThumbListCtrl::IsFuncButtonVisible()
{
	return (m_dwButtonState & ETIS_FUNCBTN_VISIBLED) != 0;
}

void CThumbListCtrl::EnableFuncButton(BOOL bEnabled)
{
	if (bEnabled)
		m_dwButtonState |= ETIS_FUNCBTN_ENABLE;
	else
		m_dwButtonState &= ~ETIS_FUNCBTN_ENABLE;

	RepaintRect(NULL);
}

BOOL CThumbListCtrl::IsFuncButtonEnabled()
{
	return (m_dwButtonState & ETIS_FUNCBTN_ENABLE) != 0; 
}

void CThumbListCtrl::SetItemFuncBtnCheck(int nItem, BOOL bChecked, BOOL bRedraw)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		if (bChecked)
			pItem->dwState |= ETIS_FUNCBTN_CHECKED | ETIS_FUNCBTN_VISIBLED;
		else 
			pItem->dwState &= ~ETIS_FUNCBTN_CHECKED;

		if (bRedraw)
			RepaintRect(pItem->rcFuncBtn);
	}
}

BOOL CThumbListCtrl::IsItemFuncBtnChecked(int nItem)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem)
		return ((pItem->dwState & ETIS_FUNCBTN_CHECKED) != 0);
	return FALSE;
}

void CThumbListCtrl::CheckAllItemsFuncBtn(BOOL bChecked)
{
	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		if (bChecked)
			(*it)->dwState |= ETIS_FUNCBTN_CHECKED;
		else
			(*it)->dwState &= ~ETIS_FUNCBTN_CHECKED;
	}

	Invalidate();
}

int CThumbListCtrl::GetFuncBtnCheckedItemCount()
{
	int nCount = 0;
	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		if ((*it)->dwState & ETIS_FUNCBTN_CHECKED)
			nCount ++;
	}

	return nCount;
}

void CThumbListCtrl::SetItemText(int nItem, LPCTSTR lpszText, BOOL bRedraw)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem == NULL)
		return ;

	pItem->strName = lpszText;

	if (bRedraw)
		RepaintRect(pItem->rcName);
}

CString CThumbListCtrl::GetItemText(int nItem)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem == NULL)
		return _T("");

	return pItem->strName;
}

void CThumbListCtrl::SetItemTitle(int nItem, LPCTSTR lpszText, BOOL bRedraw)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem == NULL)
		return;

	pItem->strTitle = lpszText;

	if (bRedraw)
		RepaintRect(pItem->rcName);
}

CString CThumbListCtrl::GetItemTitle(int nItem)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem == NULL)
		return _T("");

	return pItem->strTitle;
}

void CThumbListCtrl::SetItemState(int nItem, DWORD dwState, DWORD dwMask, BOOL bRedraw)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem == NULL)
		return;

	if (dwMask & ETIS_SELECTED) {
		if (dwState & ETIS_SELECTED) {
			AddSelItem(pItem);
		}
		else {
			RemoveSelItem(pItem);
		}
	}

	pItem->dwState &= ~dwMask;
	pItem->dwState |= (dwState & dwMask);

	if (dwMask & ETIS_FOCUS) {
		SetFocusItem(pItem);
	}

	if (bRedraw)
		RepaintRect(pItem->rect);
}

DWORD CThumbListCtrl::GetItemState(int nItem, DWORD dwMask)
{
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem == NULL)
		return 0;

	return (pItem->dwState & dwMask);
}

void CThumbListCtrl::SetAllItemsState(DWORD dwState, DWORD dwMask, BOOL bRedraw)
{
	if (dwMask & ETIS_SELECTED) {
		m_mapSelItem.clear();
	}

	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		(*it)->dwState &= ~dwMask;
		(*it)->dwState |= dwState;

		if (dwMask & ETIS_SELECTED && dwState & ETIS_SELECTED) {
			m_mapSelItem.insert(CThumbnailItemMap::value_type((DWORD_PTR)(*it), *it));
		}
	}

	if (bRedraw)
		Invalidate();
}

CSize CThumbListCtrl::GetMaxSize()
{
	CSize size(0,0);
	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		size.cx = max(size.cx, (*it)->rect.right);
		size.cy = max(size.cy, (*it)->rect.bottom);
	}

	size.cx += m_nHorzSpacing;
	size.cy += m_nVertSpacing;

	return size;
}

void CThumbListCtrl::ScrollBarPos(int nBar, int nPos)
{
	int nLimit = 0;

	if (SB_VERT == nBar)
		nLimit = GetScrollLimit(SB_VERT);
	else
		nLimit = GetScrollLimit(SB_HORZ);

	if (nPos < 0)
		nPos = 0;
	else if (nPos > nLimit)
		nPos = nLimit;

	SetScrollPos(nBar, nPos);
}

void CThumbListCtrl::ResetScrollInfo()
{
	CSize size = GetMaxSize();

	SCROLLINFO scrlinfo;
	scrlinfo.cbSize = sizeof(scrlinfo);

	// vertical
	scrlinfo.fMask = SIF_PAGE|SIF_RANGE;
	scrlinfo.nMax = size.cy;
	scrlinfo.nMin = 0;
	scrlinfo.nPage = m_rcClient.Height();
	scrlinfo.nPos = GetScrollPos(SB_VERT);;
	SetScrollInfo(SB_VERT, &scrlinfo);

	// horizontal
	scrlinfo.fMask = SIF_PAGE|SIF_RANGE;
	scrlinfo.nMax = size.cx;
	scrlinfo.nMin = 0;
	scrlinfo.nPage = m_rcClient.Width();
	scrlinfo.nPos = GetScrollPos(SB_HORZ);;
	SetScrollInfo(SB_HORZ, &scrlinfo);
}

void CThumbListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SetFocus();

	int nScrollPos = GetScrollPos(SB_VERT);
	int nLimit = GetScrollLimit(SB_VERT);
	int nScroll = nLimit;
	int SCROLL_AMT_Y = m_sizeItem.cy + m_nVertSpacing;
	//nScrollPos = SCROLL_AMT_Y * (nScrollPos / SCROLL_AMT_Y);

	switch(nSBCode) 
	{
	case SB_LINEUP:      // Scroll up.      
	case SB_PAGEUP:
		if(nScrollPos <= 0)
		{
			return;
		}
		nScroll = min(nScrollPos, SCROLL_AMT_Y);
		SetScrollPos(SB_VERT, nScrollPos - nScroll);
		break;   
	case SB_LINEDOWN:   // Scroll down.
	case SB_PAGEDOWN:
		if(nScrollPos >= nLimit)
		{
			return;
		}
		nScroll = min(nScroll - nScrollPos, SCROLL_AMT_Y);
		SetScrollPos(SB_VERT, nScrollPos + nScroll);
		break;
	case SB_THUMBPOSITION:
		{
			HWND hWndScroll;
			if (pScrollBar == NULL)
				hWndScroll = m_hWnd;
			else
				hWndScroll = pScrollBar->m_hWnd;

			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_TRACKPOS;
			::GetScrollInfo(hWndScroll, SB_VERT, &info);
			nPos = info.nTrackPos;
			//nPos = SCROLL_AMT_Y * (nPos / SCROLL_AMT_Y);

			SetScrollPos(SB_VERT, nPos);
		}
		break;
	case SB_THUMBTRACK:
		{
			HWND hWndScroll;
			if (pScrollBar == NULL)
				hWndScroll = m_hWnd;
			else
				hWndScroll = pScrollBar->m_hWnd;

			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_TRACKPOS;
			::GetScrollInfo(hWndScroll, SB_VERT, &info);
			nPos = info.nTrackPos;

			SetScrollPos(SB_VERT, nPos, FALSE);
		}
		break;
	}	

	Invalidate();

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CThumbListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SetFocus();

	int nScrollPos = GetScrollPos(SB_HORZ);
	int nLimit = GetScrollLimit(SB_HORZ);
	int nScroll = nLimit;
	int SCROLL_AMT_X = m_sizeItem.cx + m_nHorzSpacing;
	//nScrollPos = SCROLL_AMT_Y * (nScrollPos / SCROLL_AMT_Y);

	switch(nSBCode) 
	{
	case SB_LINELEFT:      // Scroll left.      
	case SB_PAGELEFT:
		if(nScrollPos <= 0)
		{
			return;
		}
		nScroll = min(nScrollPos, SCROLL_AMT_X);
		SetScrollPos(SB_HORZ, nScrollPos - nScroll);
		break;   
	case SB_LINERIGHT:   // Scroll right.
	case SB_PAGERIGHT:
		if(nScrollPos >= nLimit)
		{
			return;
		}
		nScroll = min(nScroll-nScrollPos, SCROLL_AMT_X);
		SetScrollPos(SB_HORZ, nScrollPos + nScroll);
		break;
	case SB_THUMBPOSITION:
		{
			HWND hWndScroll;
			if (pScrollBar == NULL)
				hWndScroll = m_hWnd;
			else
				hWndScroll = pScrollBar->m_hWnd;

			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_TRACKPOS;
			::GetScrollInfo(hWndScroll, SB_HORZ, &info);
			nPos = info.nTrackPos;

			SetScrollPos(SB_HORZ, nPos);
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
			::GetScrollInfo(hWndScroll, SB_HORZ, &info);
			nPos = info.nTrackPos;

			SetScrollPos(SB_HORZ, nPos, FALSE);
		}
		break;
	}	

	Invalidate();

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CThumbListCtrl::GetHitTest(CPoint point, int& nItem, int& nHitType)
{
	for (int i=(int)m_listItem.size()-1; i >= 0; i --) 
	{
		if (m_listItem[i]->rect.PtInRect(point)) {
			nItem = i;

			if (m_listItem[i]->rcFuncBtn.PtInRect(point)
				&& (m_dwButtonState & ETIS_FUNCBTN_VISIBLED) > 0) 
			{
				nHitType = EHT_FUNCBTN;
			}
			else if (m_listItem[i]->rcCheckBtn.PtInRect(point)
				&& (m_dwButtonState & ETIS_SELBTN_VISIBLED) > 0)
			{
				nHitType = EHT_SELECT_BTN;
			}
			else if (m_listItem[i]->rcName.PtInRect(point)) {
				nHitType = EHT_LABEL;
			}
			else {
				nHitType = EHT_ITEM;
			}

			return TRUE;
		}
	}

	nItem = -1;
	nHitType = EHT_NONE;
	return FALSE;
}

void CThumbListCtrl::CalcSelItems(CRect rect)
{
	m_mapSelItem.clear();

	CPoint pt1, pt2;

	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		pt1.x = max(rect.left, (*it)->rect.left+SENSITIVE_WIDTH);
		pt1.y = max(rect.top, (*it)->rect.top+SENSITIVE_WIDTH);
		pt2.x = min(rect.right, (*it)->rect.right-SENSITIVE_WIDTH);
		pt2.y = min(rect.bottom, (*it)->rect.bottom-SENSITIVE_WIDTH);

		if (pt1.x <= pt2.x && pt1.y <= pt2.y) 
			AddSelItem((*it));
		else
			RemoveSelItem((*it));
	}

	PostSelectChangedNotify();
}

void CThumbListCtrl::ClearState(DWORD dwState)
{
	if (dwState & ETIS_SELECTED)
		m_mapSelItem.clear();

	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		(*it)->dwState &= ~dwState;
	}
}

void CThumbListCtrl::GetSelectedItems(int *pItem, int &nCount)
{
	int nIndex = 0;
	int nItemIndex = 0;
	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++, nIndex ++)
	{
		if ((*it)->dwState & ETIS_SELECTED) {
			if (pItem && nItemIndex < nCount)
				pItem[nItemIndex] = nIndex;
			nItemIndex ++;
		}
	}

	nCount = nItemIndex;
}

inline void CThumbListCtrl::AddSelItem(PELC_THUMBNAIL_ITEM pItem)
{
	pItem->dwState |= ETIS_SELECTED;
	CThumbnailItemMap::iterator it = m_mapSelItem.find((DWORD_PTR)pItem);
	if (it == m_mapSelItem.end()) {
		m_mapSelItem.insert(CThumbnailItemMap::value_type((DWORD_PTR)pItem, pItem));
	}
}

inline void CThumbListCtrl::RemoveSelItem(PELC_THUMBNAIL_ITEM pItem)
{
	pItem->dwState &= ~ETIS_SELECTED;
	CThumbnailItemMap::iterator it = m_mapSelItem.find((DWORD_PTR)pItem);
	if (it != m_mapSelItem.end()) {
		m_mapSelItem.erase(it);
	}
}

void CThumbListCtrl::ClearSelItem(CRgn& rgnPaint, PELC_THUMBNAIL_ITEM pExceptItem)
{
	rgnPaint.CreateRectRgn(0,0,0,0);
	for (CThumbnailItemMap::iterator it = m_mapSelItem.begin();
		it != m_mapSelItem.end();
		it ++)
	{
		if (it->second == pExceptItem)
			continue;

		it->second->dwState &= ~ETIS_SELECTED;

		CRgn rgn;
		rgn.CreateRectRgn(it->second->rect.left,
			it->second->rect.top,
			it->second->rect.right,
			it->second->rect.bottom);
		rgnPaint.CombineRgn(&rgnPaint, &rgn, RGN_OR);
	}

	m_mapSelItem.clear();
	if (pExceptItem) {
		if ((pExceptItem->dwState & ETIS_SELECTED) == 0) {
			pExceptItem->dwState |= ETIS_SELECTED;
			CRgn rgn;
			rgn.CreateRectRgn(pExceptItem->rect.left,
				pExceptItem->rect.top,
				pExceptItem->rect.right,
				pExceptItem->rect.bottom);
			rgnPaint.CombineRgn(&rgnPaint, &rgn, RGN_OR);
		}
		m_mapSelItem.insert(CThumbnailItemMap::value_type((DWORD_PTR)pExceptItem, pExceptItem));
	}
}

void CThumbListCtrl::OffsetSelItems(CPoint ptOffset, CRgn& rgnPaint, CPoint *lpLeftTopmost, CPoint *lpRightBottommost)
{
	rgnPaint.CreateRectRgn(0,0,0,0);

	if (lpLeftTopmost) 
		lpLeftTopmost->SetPoint(0x0FFFFFFF, 0x0FFFFFFF);
	if (lpRightBottommost)
		lpRightBottommost->SetPoint(0, 0);

	for (CThumbnailItemMap::iterator it = m_mapSelItem.begin();
		it != m_mapSelItem.end();
		it ++)
	{
		CRgn rgn;
		rgn.CreateRectRgn(it->second->rect.left,
			it->second->rect.top,
			it->second->rect.right + SHADOW_WIDTH,
			it->second->rect.bottom + SHADOW_WIDTH);

		rgnPaint.CombineRgn(&rgnPaint, &rgn, RGN_OR);

		OffsetItem(it->second, ptOffset);

		rgn.DeleteObject();
		rgn.CreateRectRgn(it->second->rect.left,
			it->second->rect.top,
			it->second->rect.right + SHADOW_WIDTH,
			it->second->rect.bottom + SHADOW_WIDTH);

		rgnPaint.CombineRgn(&rgnPaint, &rgn, RGN_OR);

		if (lpLeftTopmost) {
			lpLeftTopmost->x = min(it->second->rect.left, lpLeftTopmost->x);
			lpLeftTopmost->y = min(it->second->rect.top, lpLeftTopmost->y);
		}
		if (lpRightBottommost) {
			lpRightBottommost->x = max(it->second->rect.right, lpLeftTopmost->x);
			lpRightBottommost->y = max(it->second->rect.bottom, lpLeftTopmost->y);
		}
	}
}

void CThumbListCtrl::OffsetItem(PELC_THUMBNAIL_ITEM pItem, CPoint ptOffset)
{
	if (ptOffset.x == 0 && ptOffset.y == 0)
		return;
	pItem->rect.OffsetRect(ptOffset);
	pItem->rcFuncBtn.OffsetRect(ptOffset);
	pItem->rcCheckBtn.OffsetRect(ptOffset);
	pItem->rcTitle.OffsetRect(ptOffset);
	pItem->rcThumb.OffsetRect(ptOffset);
	pItem->rcName.OffsetRect(ptOffset);
}

void CThumbListCtrl::GetSelItemsRect()
{
	m_nrcSelItems = (int)m_mapSelItem.size();
	if (m_nrcSelItems > m_nrcItemsBuffer) {
		m_nrcItemsBuffer = m_nrcSelItems;
		if (m_prcItems)
			delete []m_prcItems;
		m_prcItems = new CRect[m_nrcItemsBuffer];
	}

	CRect * pRect = m_prcItems;

	for (CThumbnailItemMap::iterator it = m_mapSelItem.begin();
		it != m_mapSelItem.end();
		it ++)
	{
		*pRect = it->second->rect;
		pRect ++;
	}
}

void CThumbListCtrl::OffsetRects(CRect * pRects, int nCount, CPoint ptOffset)
{
	for (int i=0; i<nCount; i ++) {
		pRects[i].OffsetRect(ptOffset);
	}
}

void CThumbListCtrl::OffsetAllItems(CPoint ptOffset)
{
	for (CThumbnailItemArray::iterator it = m_listItem.begin();
		it != m_listItem.end();
		it ++)
	{
		OffsetItem((*it), ptOffset);
	}
}

void CThumbListCtrl::CreatePaintRegion(CRgn &rgn)
{
	rgn.CreateRectRgn(0,0,0,0);

	for (int i=0; i<m_nrcSelItems; i ++) {
		CRgn rgnTemp;
		rgnTemp.CreateRectRgn(
			m_prcItems[i].left,
			m_prcItems[i].top,
			m_prcItems[i].right,
			m_prcItems[i].bottom);
		rgn.CombineRgn(&rgn, &rgnTemp, RGN_OR);
	}
}

CPoint CThumbListCtrl::FindIdealInsertPosition()
{
	CPoint pt(m_nHorzSpacing, m_nVertSpacing);
	if (m_rcClient.IsRectEmpty())
		return pt;

	CRect rect;
	rect.left = m_nHorzSpacing;
	rect.top = m_nVertSpacing;
	rect.right = rect.left + m_sizeItem.cx;
	rect.bottom = rect.top + m_sizeItem.cy;

	int xoffset = m_sizeItem.cx + m_nHorzSpacing;
	int yoffset = m_sizeItem.cy + m_nVertSpacing;

	BOOL bFound = TRUE;
	do 
	{
		for (CThumbnailItemArray::iterator it = m_listItem.begin();
			it != m_listItem.end();
			it ++)
		{
			if ((*it)->rect.PtInRect(CPoint(rect.left, rect.top))
				|| (*it)->rect.PtInRect(CPoint(rect.right, rect.top))
				|| (*it)->rect.PtInRect(CPoint(rect.right, rect.bottom))
				|| (*it)->rect.PtInRect(CPoint(rect.left, rect.bottom)))
			{
				bFound = FALSE;
				break;
			}
		}

		if (!bFound) {
			rect.OffsetRect(xoffset, 0);
			if (rect.right > m_rcClient.right + m_nHorzSpacing)
				rect.OffsetRect(m_nHorzSpacing-rect.left, yoffset);
			bFound = TRUE;
		}
		else {
			pt.x = rect.left;
			pt.y = rect.top;
			break;
		}
	} while (1);

	return pt;
}

void CThumbListCtrl::RepaintRect(LPRECT lpRect)
{
	if (lpRect == NULL) {
		Invalidate();
	}
	else {
		CRect rect = lpRect;
		rect.OffsetRect(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));
		InvalidateRect(rect);
	}
}

void CThumbListCtrl::RepaintRgn(CRgn *rgn)
{
	rgn->OffsetRgn(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));
	InvalidateRgn(rgn);
}

void CThumbListCtrl::MoveItemToTail(int nItem)
{
	if (nItem < 0 
		|| nItem >= (int)(m_listItem.size() - 1))
		return;

	PELC_THUMBNAIL_ITEM pItem = m_listItem[nItem];
	m_listItem.erase(m_listItem.begin() + nItem);
	m_listItem.push_back(pItem);
}

void CThumbListCtrl::SetFocusItem(PELC_THUMBNAIL_ITEM pItem)
{
	if (m_pFocusItem == pItem)
		return;

	if (m_pFocusItem)
		m_pFocusItem->dwState &= ~ETIS_FOCUS;

	if (pItem)
		pItem->dwState |= ETIS_FOCUS;

	m_pFocusItem = pItem;
}

int CThumbListCtrl::GetNextItem(int nItem, int nFlags)
{
	if (ETINI_ALL == nFlags) {
		nItem ++;
		if (nItem == (int)m_listItem.size())
			return -1;
		else
			return nItem;
	}
	else if (ETINI_FOCUSED == nFlags) {
		return FindItem(m_pFocusItem);
	}
	else {
		nItem ++;
		int nCount = (int)m_listItem.size();

		DWORD dwState = 0;
		if (ETINI_SELECTED == nFlags)
			dwState = ETIS_SELECTED;
		else if (ETINI_CHECKED == nFlags)
			dwState = ETIS_SELBTN_CHECKED;
		else if (ETINI_CHECKED == nFlags)
			dwState = ETIS_FUNCBTN_CHECKED;

		for (int i=nItem; i<nCount; i++)
		{
			if (m_listItem[i]->dwState & dwState)
				return i;
		}
		return -1;
	}
}

int CThumbListCtrl::GetFirstSelectItem()
{
	return GetNextItem(-1, LVNI_SELECTED);
}

int CThumbListCtrl::GetNextSelectedItem(int nItem)
{
	return GetNextItem(nItem, LVNI_SELECTED);
}

int CThumbListCtrl::GetSelectedCount()
{
	return (int)m_mapSelItem.size();
}

void CThumbListCtrl::SetItemSpacing(int nHorzSpacing, int nVertSpacing)
{
	int x = nHorzSpacing - m_nHorzSpacing;
	int y = nVertSpacing - m_nVertSpacing;

	if (x == 0 && y == 0)
		return;

	m_nHorzSpacing = nHorzSpacing;
	m_nVertSpacing = nVertSpacing;

	OffsetAllItems(CPoint(x, y));
	ResetScrollInfo();
	Invalidate();
}

void CThumbListCtrl::GetItemSpacing(int* pnHorzSpacing, int* pnVertSpacing)
{
	if (pnHorzSpacing)
		*pnHorzSpacing = m_nHorzSpacing;
	if (pnVertSpacing)
		*pnVertSpacing = m_nVertSpacing;
}

void CThumbListCtrl::SelectNext(UINT nFlag)
{
	if (m_listItem.empty())
		return;

	if (m_mapSelItem.empty()) 
	{
		if (!m_pFocusItem)
			return;

		int nFind;
		if (VK_LEFT == nFlag) {
			nFind = FindLeftItem(m_pFocusItem);
		}
		else if (VK_UP == nFlag) {
			nFind = FindAboveItem(m_pFocusItem);
		}
		else if (VK_RIGHT == nFlag) {
			nFind = FindRightItem(m_pFocusItem);
		}
		else if (VK_DOWN == nFlag) {
			nFind = FindBelowItem(m_pFocusItem);
		}

		if (nFind == -1) {
			nFind = FindItem(m_pFocusItem);
		}

		AddSelItem(m_listItem[nFind]);
		SetFocusItem(m_listItem[nFind]);
		if (!IsItemVisible(nFind))
			EnsureVisible(nFind);
		else
			RepaintRect(m_listItem[nFind]->rect);
	}
	else
	{
		int nCount = (int)m_listItem.size();
		int nIndex = 0;
		for (; nIndex<nCount; nIndex ++)
		{
			if (m_listItem[nIndex]->dwState & ETIS_SELECTED) {
				break;
			}
		}

		int nFind;
		if (VK_LEFT == nFlag) {
			nFind = FindLeftItem(m_listItem[nIndex]);
		}
		else if (VK_UP == nFlag) {
			nFind = FindAboveItem(m_listItem[nIndex]);
		}
		else if (VK_RIGHT == nFlag) {
			nFind = FindRightItem(m_listItem[nIndex]);
		}
		else if (VK_DOWN == nFlag) {
			nFind = FindBelowItem(m_listItem[nIndex]);
		}

		if (nFind == -1)
			return;

		CRgn rgn;
		ClearSelItem(rgn, m_listItem[nFind]);
		SetFocusItem(m_listItem[nFind]);

		if (!IsItemVisible(nFind))
			EnsureVisible(nFind);
		else
			RepaintRgn(&rgn);
	}

	PostSelectChangedNotify();
}

int CThumbListCtrl::FindLeftItem(PELC_THUMBNAIL_ITEM pItem)
{
	int nFind = -1;
	CPoint pt(0x0FFFFFFF, 0x0FFFFFFF);
	int nCount = (int)m_listItem.size();
	for (int i=0; i<nCount; i++)
	{
		if (m_listItem[i]->rect.left >= pItem->rect.left)
			continue;

		int x = pItem->rect.left - m_listItem[i]->rect.left;
		if (pt.x >= x) {
			int y = abs(pItem->rect.top - m_listItem[i]->rect.top);
			if (pt.y >= y) {
				pt.x = x;
				pt.y = y;
				nFind = i;
			}
		}
	}

	return nFind;
}

int CThumbListCtrl::FindRightItem(PELC_THUMBNAIL_ITEM pItem)
{
	int nFind = -1;
	CPoint pt(0x0FFFFFFF, 0x0FFFFFFF);
	int nCount = (int)m_listItem.size();
	for (int i=0; i<nCount; i++)
	{
		if (m_listItem[i]->rect.left <= pItem->rect.left)
			continue;

		int x = m_listItem[i]->rect.left - pItem->rect.left;
		if (pt.x >= x) {
			int y = abs(pItem->rect.top - m_listItem[i]->rect.top);
			if (pt.y >= y) {
				pt.x = x;
				pt.y = y;
				nFind = i;
			}
		}
	}

	return nFind;
}

int CThumbListCtrl::FindAboveItem(PELC_THUMBNAIL_ITEM pItem)
{
	int nFind = -1;
	CPoint pt(0x0FFFFFFF, 0x0FFFFFFF);
	int nCount = (int)m_listItem.size();
	for (int i=0; i<nCount; i++)
	{
		if (m_listItem[i]->rect.top >= pItem->rect.top)
			continue;

		int y = pItem->rect.top - m_listItem[i]->rect.top;
		if (pt.y >= y) {
			int x = abs(pItem->rect.left - m_listItem[i]->rect.left);
			if (pt.x >= x) {
				pt.x = x;
				pt.y = y;
				nFind = i;
			}
		}
	}

	return nFind;
}

int CThumbListCtrl::FindBelowItem(PELC_THUMBNAIL_ITEM pItem)
{
	int nFind = -1;
	CPoint pt(0x0FFFFFFF, 0x0FFFFFFF);
	int nCount = (int)m_listItem.size();
	for (int i=0; i<nCount; i++)
	{
		if (m_listItem[i]->rect.top <= pItem->rect.top)
			continue;

		int y = m_listItem[i]->rect.top - pItem->rect.top;
		if (pt.y >= y) {
			int x = abs(pItem->rect.left - m_listItem[i]->rect.left);
			if (pt.x >= x) {
				pt.x = x;
				pt.y = y;
				nFind = i;
			}
		}
	}

	return nFind;
}

BOOL CThumbListCtrl::EnsureVisible(int nItem)
{
	CPoint pt(0,0);
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		CRect rect = m_rcClient;
		int xpos = GetScrollPos(SB_HORZ);
		int ypos = GetScrollPos(SB_VERT);
		rect.OffsetRect(xpos, ypos);

		int x = pItem->rect.left - rect.left;
		if (x > 0) {
			x = pItem->rect.right - rect.right;
			if (x < 0)
				x = 0;
		}

		int y = pItem->rect.top - rect.top;
		if (y > 0) {
			y = pItem->rect.bottom - rect.bottom;
			if (y < 0)
				y = 0;
		}

		if (x != 0)
			ScrollBarPos(SB_HORZ, xpos+x);
		if (y != 0)
			ScrollBarPos(SB_VERT, ypos+y);

		Invalidate();

		return TRUE;
	}

	return FALSE;
}

BOOL CThumbListCtrl::IsItemVisible(int nItem)
{
	int nVisible = 0;
	PELC_THUMBNAIL_ITEM pItem = FindItem(nItem);
	if (pItem) {
		CRect rect = m_rcClient;
		rect.OffsetRect(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
		if (rect.PtInRect(CPoint(pItem->rect.left, pItem->rect.top)))
			nVisible ++;
		if (rect.PtInRect(CPoint(pItem->rect.right, pItem->rect.top)))
			nVisible ++;
		if (rect.PtInRect(CPoint(pItem->rect.right, pItem->rect.bottom)))
			nVisible ++;
		if (rect.PtInRect(CPoint(pItem->rect.left, pItem->rect.bottom)))
			nVisible ++;

		return (nVisible == 4);
	}

	return FALSE;
}

BOOL CThumbListCtrl::SortItems(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData)
{
	int nCount = (int)m_listItem.size();
	PELC_THUMBNAIL_ITEM *pItems = new PELC_THUMBNAIL_ITEM[nCount];
	for (int i=0; i < nCount; i ++) {
		pItems[i] = m_listItem[i];
	}

	if (Sort(pItems, nCount, pfnCompare, dwData)) {
		for (int i=0; i < nCount; i ++) {
			m_listItem[i] = pItems[i];
		}
	}

	AutoArrange();

	delete []pItems;

	return TRUE;
}

BOOL CThumbListCtrl::Sort(PELC_THUMBNAIL_ITEM *pItems, int nCount, PFNLVCOMPARE pfnCompare, DWORD_PTR dwData)
{
	if (nCount <= 1)
		return TRUE;

	int nSmall=1, nBig=nCount-1;
	PELC_THUMBNAIL_ITEM key=pItems[0];
	BOOL bProteron = TRUE;
	int nFree = 0;

	for(int i=0; i < nCount-1; i++)
	{
		if(bProteron)
		{
			if(pfnCompare(pItems[nBig]->dwData, key->dwData, dwData) <= 0)
			{
				pItems[nFree] = pItems[nBig];
				nFree = nBig;

				bProteron = FALSE;
			}
			nBig --;
		}
		else
		{
			if(pfnCompare(pItems[nSmall]->dwData, key->dwData, dwData) > 0)
			{
				pItems[nFree] = pItems[nSmall];
				nFree = nSmall;
				bProteron = TRUE;
			}
			nSmall++;
		}
	}

	if (nFree != 0)
	{
		pItems[nFree] = key;
	}

	if (Sort(&pItems[0], nFree, pfnCompare, dwData)
		&& Sort(&pItems[nFree+1], nCount-(nFree+1), pfnCompare, dwData))
	{
		return TRUE;
	}

	return FALSE;
}

CEdit* CThumbListCtrl::GetEditControl()
{
	return &m_wndEdit;
}

void CThumbListCtrl::EnableEditLabel(BOOL bEnable)
{
	m_bEnableEditLabel = bEnable;
	if (m_wndEdit.GetSafeHwnd() && m_wndEdit.IsWindowVisible())
		m_wndEdit.ShowWindow(SW_HIDE);
}

CEdit* CThumbListCtrl::EditLabel(int nItem)
{
	if (nItem < 0
		|| nItem >= (int)m_listItem.size()
		|| m_listItem.empty())
		return NULL;

	ELC_THUMBNAIL_LIST_NOTIFY notify;
	notify.hwndFrom = m_hWnd;
	notify.nCtrlId = GetDlgCtrlID();
	notify.nItem = nItem;
	notify.item = *m_listItem[nItem];
	// if return no-zero, cancel edit label
	if (::SendMessage(m_hNotifyWnd, m_nNotifyMsg, ETCN_BEGINLABELEDIT, (LPARAM)&notify)) {
		return NULL;
	}

	SetFocus();

	m_modifytype = ETIMT_NONE;
	m_wndEdit.m_nItem = nItem;
	m_wndEdit.m_dwData = (DWORD_PTR)m_listItem[nItem];
	m_wndEdit.SetWindowText(m_listItem[nItem]->strName);

	CRect rect;
	CSize size = m_wndEdit.GetIdeaSize();
	rect.left = m_listItem[nItem]->rcName.left + (m_listItem[nItem]->rcName.Width() - size.cx) / 2;
	rect.right = rect.left + size.cx;
	rect.top = m_listItem[nItem]->rcName.top + (m_listItem[nItem]->rcName.Height() - size.cy) / 2;
	rect.bottom = rect.top + size.cy;
	rect.OffsetRect(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));
	m_wndEdit.MoveWindow(rect);
	m_wndEdit.ShowWindow(SW_SHOW);
	m_wndEdit.SetFocus();
	m_wndEdit.SetFont(&m_ftText);
	m_wndEdit.SetSel(0, -1);

	RepaintRect(m_listItem[nItem]->rcName);

	return &m_wndEdit;
}

LRESULT CThumbListCtrl::OnEditLabelNotify(WPARAM wParam, LPARAM lParam)
{
	if (m_wndEdit.m_nItem < 0
		|| m_wndEdit.m_nItem >= (int)m_listItem.size())
		return 0;

	ELC_THUMBNAIL_LIST_NOTIFY notify;
	notify.hwndFrom = m_hWnd;
	notify.nCtrlId = GetDlgCtrlID();
	notify.nItem = m_wndEdit.m_nItem;
	notify.item = *((PELC_THUMBNAIL_ITEM)m_wndEdit.m_dwData);

	m_wndEdit.ShowWindow(SW_HIDE);
	m_wndEdit.m_nItem = -1;
	m_wndEdit.m_dwData = 0;

	if (ETENT_OK == wParam) {
		m_wndEdit.GetWindowText(notify.item.strName);
		::SendMessage(m_hNotifyWnd, m_nNotifyMsg, ETCN_ENDLABELEDIT, (LPARAM)&notify);
	}
	else if (ETENT_CANCEL == wParam) {
		notify.item.strName.Empty();
		::SendMessage(m_hNotifyWnd, m_nNotifyMsg, ETCN_ENDLABELEDIT, (LPARAM)&notify);
	}

	RepaintRect(m_listItem[notify.nItem]->rcName);

	SetFocus();

	return 0;
}

void CThumbListCtrl::PostSelectChangedNotify()
{
	ELC_THUMBNAIL_LIST_NOTIFY notify;
	notify.hwndFrom = m_hWnd;
	notify.nCtrlId = GetDlgCtrlID();
	notify.nItem = -1;

	::SendMessage(m_hNotifyWnd, m_nNotifyMsg, ETCN_SELECT_CHANGED, (LPARAM)&notify);
}

LRESULT CThumbListCtrl::MouseNotify(ELC_MOUSE_NOTIFY_TYPE type, UINT nFlags)
{
	ELC_MOUSE_NOTIFY notify;
	notify.hWnd = m_hWnd;
	notify.type = type;
	notify.nFlags = nFlags;
	GetCursorPos(&notify.point);

	if (m_hNotifyWnd)
		return ::SendMessage(m_hNotifyWnd, WM_ELC_MOUSE_NOTIFY, GetDlgCtrlID(), (LPARAM)&notify);
	else
		return GetParent()->SendMessage(WM_ELC_MOUSE_NOTIFY, GetDlgCtrlID(), (LPARAM)&notify);
}