// ElcSkinFrame.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcSkinFrameBase.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



// CElcSkinFrameBase

CElcSkinFrameBase::CElcSkinFrameBase()
{
	memset(&m_skin, 0, sizeof(m_skin));
	m_hDerive = NULL;
	m_hMenu = NULL;
	m_hIcon = NULL;
	m_nWndType = ELCSWT_WND;

	m_nLeft = 0;
	m_nRight = 0;
	m_nCaption = 0;
	m_nMenuBar = 0;
	m_nBottom = 0;

	NONCLIENTMETRICS ncm = {0};
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

	m_ftMenu.CreateFontIndirect(&ncm.lfMenuFont);
	m_ftCaption.CreateFontIndirect(&ncm.lfCaptionFont);

	m_bMenuActivated = FALSE;
	m_nActiveItem = -1;
	m_bSelfCancelMenu = FALSE;

	m_nActiveButton = -1;
	m_nLBDownItem = ESNWT_NONE;

	m_bEnableSkin = TRUE;
	m_bDrawClientBackgnd = FALSE;
	m_bHasIcon = FALSE;
	m_bActivated = TRUE;

	m_rect.SetRectEmpty();
}

CElcSkinFrameBase::~CElcSkinFrameBase()
{
	UnBindCtrls();
	DelAllCustomButton();
}

BOOL CElcSkinFrameBase::LoadSkin(LPCTSTR lpStyle)
{
	if (!theSkin.IsValid())
		return FALSE;

	PELC_SKIN_WINDOW pSkin = theSkin.GetWindowSkin(lpStyle);
	if (!pSkin)
		return FALSE;

	memcpy(&m_skin, pSkin, sizeof(ELC_SKIN_WINDOW));

	m_nLeft = 0;
	m_nRight = 0;
	m_nCaption = 0;
	m_nMenuBar = 0;
	m_nBottom = 0;

	if (m_skin.left.pImage)
		m_nLeft = CElcSkinDoc::GetImageSize(&m_skin.left).cx;
	if (m_skin.right.pImage)
		m_nRight = CElcSkinDoc::GetImageSize(&m_skin.right).cx;
	if (m_skin.caption.pImage)
		m_nCaption = CElcSkinDoc::GetImageSize(&m_skin.caption).cy;
	if (m_skin.menubar.background.pImage)
		m_nMenuBar = CElcSkinDoc::GetImageSize(&m_skin.menubar.background).cy;
	if (m_skin.bottom.pImage)
		m_nBottom = CElcSkinDoc::GetImageSize(&m_skin.bottom).cy;

	if (m_brBackgnd.GetSafeHandle())
		m_brBackgnd.DeleteObject();
	m_brBackgnd.CreateSolidBrush(m_skin.client.crBackgnd);

	if (m_brPane.GetSafeHandle())
		m_brPane.DeleteObject();
	m_brPane.CreateSolidBrush(m_skin.client.pane.crBackgnd);

	return TRUE;
}

int CElcSkinFrameBase::GetLeftWidth()
{
	return m_nLeft;
}

int CElcSkinFrameBase::GetRightWidth()
{
	return m_nRight;
}

int CElcSkinFrameBase::GetCaptionHeight()
{
	return m_nCaption;
}

int CElcSkinFrameBase::GetMenuBarHeight()
{
	return m_nMenuBar;
}

int CElcSkinFrameBase::GetBottomHeight()
{
	return m_nBottom;
}

PELC_SKIN_WINDOW CElcSkinFrameBase::GetSkinStyle()
{
	return &m_skin;
}

void CElcSkinFrameBase::InitButtons()
{
	m_arrButton.clear();

	LONG style = ::GetWindowLong(m_hDerive, GWL_STYLE);
	if ((style & WS_SYSMENU) == 0 || _tcslen(m_skin.szStyle) <= 0)
		return;

	HMENU hMenu = ::GetSystemMenu(m_hDerive, 0);
	if (!::IsMenu(hMenu))
		return;
	BOOL bCloseEnable = TRUE;
	UINT nCloseItemState = GetMenuState(hMenu, SC_CLOSE, MF_BYCOMMAND);
	if ((nCloseItemState & MF_GRAYED) != 0
		|| nCloseItemState == 0xFFFFFFFF)
		bCloseEnable = FALSE;

	ELC_SKINFRAME_NWBUTTON item;

	if ((style & WS_MINIMIZEBOX) == 0
		&& (style & WS_MAXIMIZEBOX) == 0) 
	{
		item.type = ESNWT_CLOSE;
		item.pButton = &m_skin.singleclose;
		item.bEnable = bCloseEnable;
		m_arrButton.push_back(item);
	}
	else {
		item.type = ESNWT_MINIMIZE;
		item.pButton = &m_skin.minimize;
		item.bEnable = (style & WS_MINIMIZEBOX) != 0;
		m_arrButton.push_back(item);

		item.type = ESNWT_MAXIMIZE;
		item.pButton = &m_skin.maximize;
		item.bEnable = (style & WS_MAXIMIZEBOX) != 0;
		m_arrButton.push_back(item);

		item.type = ESNWT_CLOSE;
		item.pButton = &m_skin.close;
		item.bEnable = bCloseEnable;
		m_arrButton.push_back(item);
	}
}

void CElcSkinFrameBase::InitMenuItem()
{
	m_hMenu = ::GetMenu(m_hDerive);
	::SetMenu(m_hDerive, NULL);

	m_arrMenu.clear();

	if (_tcslen(m_skin.szStyle) <= 0 || !::IsMenu(m_hMenu)) {
		m_nMenuBar = 0;
		return;
	}

	HDC hdc = ::GetDC(m_hDerive);
	if (!hdc)
		return;

	HFONT hOldFont = (HFONT)::SelectObject(hdc, m_ftMenu);
	
	CRect rect(m_nLeft,0,0,0);

	ELC_SKINFRAME_MENUITEM item;
	CSize sizeBk = CElcSkinDoc::GetImageSize(&m_skin.menubar.itembk);

	int nCount = ::GetMenuItemCount(m_hMenu);
	for (int i=0; i < nCount; i ++) {

		::GetMenuString(m_hMenu, i, item.strItem.GetBuffer(256), 256, MF_BYPOSITION);
		item.strItem.ReleaseBuffer();

		SIZE size;
		::GetTextExtentPoint(hdc, item.strItem, item.strItem.GetLength(), &size);
		size.cx += MENU_ITEM_HMARGIN + MENU_ITEM_HMARGIN;
		size.cy += MENU_ITEM_VMARGIN + MENU_ITEM_VMARGIN;

		size.cy = max(sizeBk.cy, size.cy);
		size.cx = max(sizeBk.cx, size.cx);

		rect.right = rect.left + size.cx;
		rect.top = m_nCaption + (m_nMenuBar - size.cy) / 2;
		rect.bottom = rect.top + size.cy;

		item.rect = rect;

		m_arrMenu.push_back(item);

		rect.left = rect.right;
	}

	::SelectObject(hdc, hOldFont);
	::ReleaseDC(m_hDerive, hdc);
}

BOOL CElcSkinFrameBase::Initialize(HWND hThis)
{
	if (!m_bEnableSkin)
		return TRUE;
		
	if (!::IsWindow(hThis)) {
		m_hDerive = 0;
		ASSERT(0);
		return FALSE;
	}

	m_hDerive = hThis;

	LONG style = GetWindowLong(m_hDerive, GWL_STYLE);
	LONG exstyle = GetWindowLong(m_hDerive, GWL_EXSTYLE);

	style &= ~WS_CAPTION;
	style &= ~WS_BORDER;

	exstyle &= ~WS_EX_CLIENTEDGE;
	exstyle &= ~WS_EX_STATICEDGE;

	CWnd* pWnd = CWnd::FromHandle(m_hDerive);
	if (!pWnd) {
		return FALSE;
	}
	else if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog))) {
		m_nWndType = ELCSWT_DIALOG;

		if (style & WS_CHILDWINDOW) {
			m_nCaption = 0;
			m_nMenuBar = 0;
			m_nLeft = 0;
			m_nRight = 0;
			m_nBottom = 0;
		}
		else if (exstyle & WS_EX_WINDOWEDGE) {
			if ((style & WS_THICKFRAME) == 0)
				style |= WS_DLGFRAME;
		}

		m_bDrawClientBackgnd = TRUE;
	}
	else if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
		m_nWndType = ELCSWT_FRAMEWND;
	}
	else {
		if (style & WS_CHILDWINDOW) {
			m_nCaption = 0;
			m_nMenuBar = 0;
			m_nLeft = 0;
			m_nRight = 0;
			m_nBottom = 0;
		}

		m_nWndType = ELCSWT_WND;
	}

	::SetWindowLong(m_hDerive, GWL_STYLE, style);
	::SetWindowLong(m_hDerive, GWL_EXSTYLE, exstyle);

	InitButtons();
	InitMenuItem();

	::SetWindowPos(m_hDerive, NULL, 0, 0, 0, 0, 
		SWP_NOACTIVATE|SWP_NOZORDER|SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE);

	::SetTimer(m_hDerive, ELC_SKIN_FRAME_STATE_TIMER, 1000, NULL);

	return TRUE;
}

void CElcSkinFrameBase::Uninitialize()
{
	if (m_bMenuActivated) {
		m_bSelfCancelMenu = FALSE;
		::SendMessage(m_hDerive, WM_CANCELMODE, 0, 0);
	}
}

int CElcSkinFrameBase::FindButton(int type)
{
	int nIndex = 0;
	for (CFrameButtonArray::iterator it = m_arrButton.begin();
		it != m_arrButton.end();
		it ++, nIndex ++)
	{
		if ((*it).type == type) {
			return nIndex;
		}
	}

	return -1;
}

BOOL CElcSkinFrameBase::AddCustomButton(UINT nId, UINT nResId, LPCTSTR lpszType)
{
	ELC_SKINFRAME_NWBUTTON item;
	item.type = ESNWT_CUSTOM_BASE + nId;
	item.bEnable = TRUE;

	item.pButton = new ELC_SKIN_NOTWND_BUTTON;
	Image *pImage = NULL;
	if (!ImageFromIDResource(nResId, lpszType, pImage)) {
		delete item.pButton;
		return FALSE;
	}

	item.pButton->image.pImage = (PVOID)pImage;
	item.pButton->image.bVertical = TRUE;
	item.pButton->image.nCount = ELCBIM_FOUR_STATE;
	item.pButton->image.stretch = EIST_FIXED;
	item.pButton->image.nLeftSplit = -1;
	item.pButton->image.nRightSplit = -1;
	item.pButton->image.nTopSplit = -1;
	item.pButton->image.nBottomSplit = -1;

	item.pButton->pos.point.x = 0;
	item.pButton->pos.point.y = 0;
	item.pButton->pos.size.cx = pImage->GetWidth();
	item.pButton->pos.size.cy = pImage->GetHeight() / item.pButton->image.nCount;

	for (CFrameButtonArray::iterator it = m_arrButton.begin();
		it != m_arrButton.end();
		it ++)
	{
		if (item.pButton->pos.point.x > (*it).pButton->pos.point.x) {
			item.pButton->pos.point.x = (*it).pButton->pos.point.x;
			item.pButton->pos.point.y = (*it).pButton->pos.point.y + ((*it).pButton->pos.size.cy - item.pButton->pos.size.cy) / 2;
		}
	}

	item.pButton->pos.point.x -= 10;
	item.pButton->pos.point.x -= item.pButton->pos.size.cx;

	m_arrButton.push_back(item);
	return TRUE;
}

void CElcSkinFrameBase::DelCustomButton(UINT nId)
{
	int nItem = FindButton(nId + ESNWT_CUSTOM_BASE);
	if (nItem != -1) {
		delete (m_arrButton.begin() + nItem)->pButton;
		m_arrButton.erase(m_arrButton.begin() + nItem);
		RedrawCaption();
	}
}

void CElcSkinFrameBase::DelAllCustomButton()
{
	for (int i=(int)m_arrButton.size()-1; i>=0; i--) {
		if (m_arrButton[i].type >= ESNWT_CUSTOM_BASE) {
			delete m_arrButton[i].pButton;
			m_arrButton.erase(m_arrButton.begin() + i);
		}
	}
}

void CElcSkinFrameBase::EnableCustomButton(UINT nId, BOOL bEnable)
{
	int nItem = FindButton(nId + ESNWT_CUSTOM_BASE);
	if (nItem != -1) {
		m_arrButton[nItem].bEnable = bEnable;
		RedrawButton(m_arrButton[nItem].pButton);
	}
}

BOOL CElcSkinFrameBase::IsCustomButtonEnable(UINT nId)
{
	int nItem = FindButton(nId + ESNWT_CUSTOM_BASE);
	if (nItem != -1) 
		return m_arrButton[nItem].bEnable;
	return FALSE;
}

CRect CElcSkinFrameBase::GetCustomButtonRect(UINT nId)
{
	int nItem = FindButton(nId + ESNWT_CUSTOM_BASE);
	if (nItem != -1) 
		return CalcPos(m_arrButton[nItem].pButton);
	return CRect(0,0,0,0);
}

void CElcSkinFrameBase::RefreshFrameStatus()
{
	InitButtons();
	RedrawCaption();
}

void CElcSkinFrameBase::NcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (!bCalcValidRects)
		return;

	if (GetWindowLong(m_hDerive, GWL_EXSTYLE) & WS_EX_WINDOWEDGE) {

		lpncsp->rgrc[0].left += m_nLeft;
		lpncsp->rgrc[0].right -= m_nRight;
		lpncsp->rgrc[0].top += m_nCaption;

		if (!::IsZoomed(m_hDerive) || m_nWndType != ELCSWT_FRAMEWND)
			lpncsp->rgrc[0].bottom -= m_nBottom;

		if (m_nWndType == ELCSWT_FRAMEWND)
			lpncsp->rgrc[0].top += m_nMenuBar;

		if (GetWindowLong(m_hDerive, GWL_STYLE) & WS_THICKFRAME) {
			lpncsp->rgrc[0].left -= GetSystemMetrics(SM_CXFRAME);
			lpncsp->rgrc[0].right += GetSystemMetrics(SM_CXFRAME);
			lpncsp->rgrc[0].top -= GetSystemMetrics(SM_CYFRAME);

			if (!::IsZoomed(m_hDerive) || m_nWndType != ELCSWT_FRAMEWND)
				lpncsp->rgrc[0].bottom += GetSystemMetrics(SM_CYFRAME);
		}
		else {
			lpncsp->rgrc[0].left -= GetSystemMetrics(SM_CXDLGFRAME);
			lpncsp->rgrc[0].right += GetSystemMetrics(SM_CXDLGFRAME);
			lpncsp->rgrc[0].top -= GetSystemMetrics(SM_CYDLGFRAME);

			if (!::IsZoomed(m_hDerive) || m_nWndType != ELCSWT_FRAMEWND)
				lpncsp->rgrc[0].bottom += GetSystemMetrics(SM_CYDLGFRAME);
		}
	}
}

void CElcSkinFrameBase::NcPaint()
{
	HDC hdc = ::GetWindowDC(m_hDerive);
	if (!hdc)
		return;

	::GetWindowRect(m_hDerive, &m_rect);
	m_rect.OffsetRect(-m_rect.left, -m_rect.top);

	CDC* pDC = CDC::FromHandle(hdc);
	CElcMemDC dcMem(pDC, m_rect);

	if (!m_hIcon) {
		m_hIcon = CWnd::FromHandle(m_hDerive)->GetIcon(FALSE);
	}

	if (::IsIconic(m_hDerive)) {
		DrawCaptionMin(&dcMem, m_hIcon);
	}
	else {
		CRect rcClient;
		rcClient.left = m_rect.left + m_nLeft;
		rcClient.top = m_rect.top + m_nCaption + m_nMenuBar;
		rcClient.right = m_rect.right - m_nRight;
		rcClient.bottom = m_rect.bottom - m_nBottom;

		dcMem.ExcludeClipRect(rcClient);

		DrawBorder(&dcMem, m_hIcon);
		DrawSkinMenuBar(&dcMem);

		pDC->ExcludeClipRect(rcClient);
	}

	dcMem.DrawMe();

	::ReleaseDC(m_hDerive, hdc);
}

void CElcSkinFrameBase::EnableDrawClientBackgnd(BOOL bEnable)
{
	m_bDrawClientBackgnd = bEnable;
}

int CElcSkinFrameBase::FindClientBackgndPos(UINT nId)
{
	for (size_t i = 0; i < m_arrClientImagePos.size(); i ++)
	{
		if (m_arrClientImagePos[i].nId == nId) {
			return (int)i;
		}
	}

	return -1;
}

BOOL CElcSkinFrameBase::AddClientBackgndPos(UINT nId, const LPPOINT lpTopLeft, const PSIZE pSize)
{
	if (!m_skin.client.pane.image.pImage)
		return FALSE;

	int nItem = FindClientBackgndPos(nId);
	// if the item exist, just modify it. otherwise, add it
	if (nItem != -1) {
		m_arrClientImagePos[nItem].pos.point = *lpTopLeft;
		m_arrClientImagePos[nItem].pos.size = *pSize;
		return TRUE;
	}

	// todo : add position illegal check
	// ...

	ELC_SKINFRAME_CLIENT_IMAGE_POS pos;
	pos.nId = nId;
	pos.pos.point = *lpTopLeft;
	pos.pos.size = *pSize;

	m_arrClientImagePos.push_back(pos);

	return TRUE;
}

void CElcSkinFrameBase::DelClientBackgndPos(UINT nId)
{
	for (CClientImagePosArray::iterator it = m_arrClientImagePos.begin();
		it != m_arrClientImagePos.end();
		it ++)
	{
		if (it->nId == nId) {
			m_arrClientImagePos.erase(it);
			break;
		}
	}
}

void CElcSkinFrameBase::ClearClientBackgndPos()
{
	m_arrClientImagePos.clear();
}

BOOL CElcSkinFrameBase::AdjustImagePanePos(UINT nID)
{
	return AdjustImagePanePos(&nID, 1);
}

BOOL CElcSkinFrameBase::AdjustImagePanePos(UINT* nIDs, int nCount)
{
	if (!::IsWindow(m_hDerive))
		return FALSE;

	CRect rcClient, rcCtrl;
	::GetClientRect(m_hDerive, &rcClient);

	for (int i = 0; i < nCount; i ++) {
		HWND hCtrl = ::GetDlgItem(m_hDerive, nIDs[i]);
		if (!::IsWindow(hCtrl))
			return FALSE;

		::GetWindowRect(hCtrl, &rcCtrl);
		::ScreenToClient(m_hDerive, &rcCtrl.TopLeft());
		::ScreenToClient(m_hDerive, &rcCtrl.BottomRight());

		if (rcCtrl.left < 0)
			rcCtrl.left = 0;
		if (rcCtrl.top < 0)
			rcCtrl.top = 0;
		if (rcCtrl.right > rcClient.right)
			rcCtrl.right = rcClient.right;
		if (rcCtrl.bottom > rcClient.bottom)
			rcCtrl.bottom = rcClient.bottom;

		if (!AddClientBackgndPos(nIDs[i], &rcCtrl.TopLeft(), &rcCtrl.Size()))
		{
			return FALSE;
		}
	}

	EnableDrawClientBackgnd(TRUE);

	CWnd::FromHandle(m_hDerive)->Invalidate();

	return TRUE;
}

void CElcSkinFrameBase::RelayoutClientBackgndPos()
{
	CWnd* pWnd = CWnd::FromHandle(m_hDerive);
	if (!pWnd)
		return;

	for (CClientImagePosArray::iterator it = m_arrClientImagePos.begin();
		it != m_arrClientImagePos.end();
		it ++)
	{
		AdjustImagePanePos(it->nId);
	}
	
}

HBRUSH CElcSkinFrameBase::CtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd->GetSafeHwnd() == m_hDerive
		|| CTLCOLOR_DLG == nCtlColor)
		return (HBRUSH)m_brBackgnd.GetSafeHandle();

	BOOL bIsCtrlInPane = IsCtrlInPane(pWnd);

	if (CTLCOLOR_BTN == nCtlColor) 
	{
		pDC->SetBkMode(TRANSPARENT);

		UINT type = ((CButton *)pWnd)->GetButtonStyle();
		type &= BS_TYPEMASK; 
		if (type == BS_CHECKBOX
			|| type == BS_AUTOCHECKBOX
			|| type == BS_RADIOBUTTON
			|| type == BS_3STATE
			|| type == BS_AUTO3STATE
			|| type == BS_GROUPBOX
			|| type == BS_AUTORADIOBUTTON
			|| type == BS_PUSHBOX
			|| type == BS_3STATE)
		{
			pDC->SetTextColor(bIsCtrlInPane ? m_skin.client.pane.crText : m_skin.client.crText);
		}

		return bIsCtrlInPane ? (HBRUSH)m_brPane.GetSafeHandle() : (HBRUSH)m_brBackgnd.GetSafeHandle();
	}
	else if (CTLCOLOR_STATIC == nCtlColor) 
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(bIsCtrlInPane ? m_skin.client.pane.crText : m_skin.client.crText);
		return bIsCtrlInPane ? (HBRUSH)m_brPane.GetSafeHandle() : (HBRUSH)m_brBackgnd.GetSafeHandle();
	}

	return NULL;
}

BOOL CElcSkinFrameBase::IsCtrlInPane(UINT nID)
{
	CWnd* pThis = CWnd::FromHandle(m_hDerive);
	if (!pThis) {
		ASSERT(0);
		return FALSE;
	}

	return IsCtrlInPane(pThis->GetDlgItem(nID));
}

BOOL CElcSkinFrameBase::IsCtrlInPane(CWnd* pWnd)
{
	if (!m_bDrawClientBackgnd)
		return FALSE;

	if (!m_skin.client.pane.image.pImage)
		return FALSE;

	CWnd* pThis = CWnd::FromHandle(m_hDerive);
	if (!pThis)
		return FALSE;

	CRect rcThis, rcPane, rcCtrl;
	CPoint pt;

	pThis->GetClientRect(rcThis);
	pWnd->GetWindowRect(rcCtrl);
	pThis->ScreenToClient(&rcCtrl);
	pt.x = rcCtrl.left + rcCtrl.Width() / 2;
	pt.y = rcCtrl.top + rcCtrl.Height() / 2;

	for (CClientImagePosArray::iterator it = m_arrClientImagePos.begin();
		it != m_arrClientImagePos.end();
		it ++)
	{
		rcPane = CalcRect(rcThis, &(it->pos.point), &(it->pos.size));
		if (rcPane.PtInRect(pt))
			return TRUE;
	}

	return FALSE;
}

COLORREF CElcSkinFrameBase::GetCtrlBkColor(UINT nID)
{
	CWnd* pThis = CWnd::FromHandle(m_hDerive);
	if (!pThis) {
		ASSERT(0);
		return FALSE;
	}

	return GetCtrlBkColor(pThis->GetDlgItem(nID));
}

COLORREF CElcSkinFrameBase::GetCtrlBkColor(CWnd* pWnd)
{
	if (IsCtrlInPane(pWnd))
		return m_skin.client.pane.crBackgnd;
	else
		return m_skin.client.crBackgnd;
}

COLORREF CElcSkinFrameBase::GetClientBkColor()
{
	return m_skin.client.crBackgnd;
}

void CElcSkinFrameBase::SetClientBkColor(COLORREF cr)
{
	m_skin.client.crBackgnd = cr;

	if (m_brBackgnd.GetSafeHandle())
		m_brBackgnd.DeleteObject();
	m_brBackgnd.CreateSolidBrush(m_skin.client.crBackgnd);
}

COLORREF CElcSkinFrameBase::GetImagePaneColor()
{
	return m_skin.client.pane.crBackgnd;
}

void CElcSkinFrameBase::SetTitleIcon(UINT nResId, LPCTSTR lpszType)
{
	Image* pImage=NULL;
	if (!ImageFromIDResource(nResId, lpszType, pImage)) {
		if (pImage)
			delete pImage;
	}

	((Bitmap *)pImage)->GetHICON(&m_hIcon);

	CWnd* pWnd = CWnd::FromHandle(m_hDerive);
	pWnd->SetIcon(m_hIcon, FALSE);
	pWnd->SetIcon(m_hIcon, TRUE);

	delete (Bitmap *)pImage;
}

void CElcSkinFrameBase::BindCheckBox(UINT nID, LPCTSTR lpszStyle)
{
	BindButton(nID, lpszStyle);
}

void CElcSkinFrameBase::BindGroupBox(UINT nID, LPCTSTR lpszStyle)
{
	BindButton(nID, lpszStyle);
}

void CElcSkinFrameBase::BindButton(UINT* nIDs, int nCount, LPCTSTR lpszStyle)
{
	for (int i=0; i < nCount; i ++) {
		BindButton(nIDs[i], lpszStyle);
	}
}

void CElcSkinFrameBase::BindButton(UINT nID, LPCTSTR lpszStyle)
{
	if (GetSkinButton(nID))
		return;

	CWnd * pThis = CWnd::FromHandle(m_hDerive);
	if (!pThis)
		return;
	CWnd * pCtrl = pThis->GetDlgItem(nID);
	if (!pCtrl)
		return;

	if (!lpszStyle)
		lpszStyle = ELC_SKIN_STYLE_BUTTON_DLG;

	CElcButton* pButton = new CElcButton;
	pButton->LoadSkin(lpszStyle);
	pButton->SubclassDlgItem(nID, pThis);
	pButton->SetDlgCtrlID(nID);
	pButton->SetBkColor(GetCtrlBkColor(pCtrl));

	m_arrSkinCtrl.push_back(pButton);
}

CWnd* CElcSkinFrameBase::GetBindCtrl(UINT nCtrlId)
{
	for (CSkinCtrlArray::iterator it = m_arrSkinCtrl.begin();
		it != m_arrSkinCtrl.end();
		it ++)
	{
		if ((*it)->GetDlgCtrlID() == nCtrlId) {
			return (*it);
		}
	}

	return NULL;
}

CElcButton* CElcSkinFrameBase::GetSkinButton(UINT nID)
{
	return (CElcButton *)GetBindCtrl(nID);
}

void CElcSkinFrameBase::UnBindCtrl(UINT nID)
{
	for (CSkinCtrlArray::iterator it = m_arrSkinCtrl.begin();
		it != m_arrSkinCtrl.end();
		it ++)
	{
		if ((*it)->GetDlgCtrlID() == nID) {
			(*it)->UnsubclassWindow();
			delete (*it);
			m_arrSkinCtrl.erase(it);
			break;
		}
	}
}

void CElcSkinFrameBase::UnBindCtrls()
{
	for (CSkinCtrlArray::iterator it = m_arrSkinCtrl.begin();
		it != m_arrSkinCtrl.end();
		it ++)
	{
		if ((*it)->UnsubclassWindow())
			delete (*it);
	}

	m_arrSkinCtrl.clear();
}

void CElcSkinFrameBase::BindPictureCtrl(UINT nCtrlId, UINT nImageId, LPCTSTR lpszType, BOOL bRealImageSize)
{
	if (GetPictureCtrl(nCtrlId))
		return;

	CWnd * pThis = CWnd::FromHandle(m_hDerive);
	if (!pThis)
		return;
	CWnd * pCtrl = pThis->GetDlgItem(nCtrlId);
	if (!pCtrl)
		return;

	CElcStatic* pStatic = new CElcStatic;
	pStatic->SubclassDlgItem(nCtrlId, pThis);
	pStatic->SetDlgCtrlID(nCtrlId);
	pStatic->SetImage(nImageId, lpszType, bRealImageSize);

	m_arrSkinCtrl.push_back(pStatic);
}

CElcStatic* CElcSkinFrameBase::GetPictureCtrl(UINT nCtrlId)
{
	return (CElcStatic *)GetBindCtrl(nCtrlId);
}

void CElcSkinFrameBase::DrawCaptionMin(CDC* pDC, HICON hIcon)
{
	CRect rect;
	int nImageIndex = m_bActivated ? 0 : 1;

	// draw background
	thePainter.DrawImageGDI(pDC->GetSafeHdc(), 
		&m_skin.caption_min, 
		m_rect, 
		nImageIndex, 
		0, 
		0, 
		0);

	// draw icon
	m_bHasIcon = FALSE;
	rect.left = FRAME_ICON_MARGIN + FRAME_ICON_MARGIN;
	rect.right = m_rect.right - FRAME_ICON_MARGIN - FRAME_ICON_MARGIN;;
	if (hIcon) {
		m_bHasIcon = TRUE;
		::DrawIconEx(pDC->GetSafeHdc(), 
			rect.left, 
			(m_rect.Height() - FRAME_ICON_SIZE) / 2, 
			hIcon, 
			FRAME_ICON_SIZE, 
			FRAME_ICON_SIZE, 
			0, 
			NULL, 
			DI_NORMAL);

		rect.left += FRAME_ICON_SIZE + FRAME_ICON_MARGIN;
	}

	// draw buttons
	for (size_t i = 0; i < m_arrButton.size(); i ++)
	{
		if (m_arrButton[i].type >= ESNWT_CUSTOM_BASE )
			continue;

		nImageIndex = 0;
		if (m_nLBDownItem != ESNWT_NONE && m_nLBDownItem == m_arrButton[i].type) {
			nImageIndex = 2;
		}
		else if (m_nActiveButton == (int)i) {
				nImageIndex = 1;
		}

		if (!m_arrButton[i].bEnable || !m_bActivated)
			nImageIndex = 3;

		if (m_arrButton[i].type == ESNWT_MINIMIZE) {
			if (::IsIconic(m_hDerive))
				nImageIndex += 4;
		}

		CRect rcBtn = CalcPos(m_arrButton[i].pButton);
		int nYPos = (m_rect.Height() - rcBtn.Height()) / 2 - rcBtn.top;
		rcBtn.OffsetRect(0, nYPos);
		thePainter.DrawImageGDI(
			pDC->GetSafeHdc(), 
			&m_arrButton[i].pButton->image, 
			rcBtn, 
			nImageIndex,
			0,
			0,
			0);

		if (rect.right > rcBtn.left - FRAME_ICON_MARGIN)
			rect.right = rcBtn.left - FRAME_ICON_MARGIN;
	}

	// draw text
	TCHAR szText[256] = {0};
	::GetWindowText(m_hDerive, szText, 256);
	CFont * pOldFont = pDC->SelectObject(&m_ftCaption);
	pDC->SetTextColor(m_skin.crCaption);
	rect.top = 0;
	rect.bottom = m_rect.Height();
	pDC->DrawText(szText, rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_WORD_ELLIPSIS);
	pDC->SelectObject(pOldFont);
}

void CElcSkinFrameBase::DrawBorder(CDC* pDC, HICON hIcon)
{
	if (m_nLeft == 0 
		&& m_nRight == 0
		&& m_nCaption == 0
		&& m_nMenuBar == 0
		&& m_nBottom == 0)
		return;

	CRect rect;
	int nImageIndex = m_bActivated ? 0 : 1;

	// draw caption
	if (m_nCaption > 0) {
		rect = m_rect;
		rect.bottom = m_nCaption;
		thePainter.DrawImageGDI(pDC->GetSafeHdc(), 
			&m_skin.caption, 
			rect, 
			nImageIndex, 
			0, 
			0, 
			0);
	}

	// draw icon
	if (hIcon) {
		m_bHasIcon = TRUE;
		rect.left = FRAME_ICON_MARGIN + FRAME_ICON_MARGIN;
		rect.right = rect.left + FRAME_ICON_SIZE;
		rect.top = (m_nCaption - FRAME_ICON_SIZE) / 2;
		rect.bottom = rect.top + FRAME_ICON_SIZE;
		::DrawIconEx(pDC->GetSafeHdc(), 
			rect.left, 
			rect.top, 
			hIcon, 
			FRAME_ICON_SIZE, 
			FRAME_ICON_SIZE, 
			0, 
			NULL, 
			DI_NORMAL);
	}
	else {
		m_bHasIcon = FALSE;
		rect.left = rect.right = FRAME_ICON_MARGIN + FRAME_ICON_MARGIN;
	}

	// draw text
	TCHAR szText[256] = {0};
	::GetWindowText(m_hDerive, szText, 256);
	CFont * pOldFont = pDC->SelectObject(&m_ftCaption);
	pDC->SetTextColor(m_skin.crCaption);
	rect.left = rect.right + FRAME_ICON_MARGIN;
	rect.right = rect.left + pDC->GetTextExtent(szText).cx;
	rect.top = 0;
	rect.bottom = m_nCaption;
	pDC->DrawText(szText, rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_WORD_ELLIPSIS);
	pDC->SelectObject(pOldFont);

	// draw left border
	if (m_nLeft > 0) {
		rect.left = 0;
		rect.right = m_nLeft;
		rect.top = m_nCaption + m_nMenuBar;
		rect.bottom = m_rect.bottom - m_nBottom;
		thePainter.DrawImageGDI(pDC->GetSafeHdc(), 
			&m_skin.left, 
			rect, 
			nImageIndex, 
			0, 
			0, 
			0);
	}

	// draw right border
	if (m_nRight > 0) {
		rect.left = m_rect.right - m_nRight;
		rect.right = m_rect.right;
		rect.top = m_nCaption + m_nMenuBar;
		rect.bottom = m_rect.bottom - m_nBottom;
		thePainter.DrawImageGDI(pDC->GetSafeHdc(), 
			&m_skin.right, 
			rect, 
			nImageIndex, 
			0, 
			0, 
			0);
	}

	// draw Bottom border
	if (m_nBottom > 0) {
		rect.left = 0;
		rect.right = m_rect.right;
		rect.top = m_rect.bottom - m_nBottom;
		rect.bottom = m_rect.bottom;
		thePainter.DrawImageGDI(pDC->GetSafeHdc(), 
			&m_skin.bottom, 
			rect, 
			nImageIndex, 
			0, 
			0, 
			0);
	}

	// draw buttons
	for (size_t i = 0; i < m_arrButton.size(); i ++)
	{
		nImageIndex = 0;
		if (m_nLBDownItem != ESNWT_NONE && m_nLBDownItem == m_arrButton[i].type) {
			nImageIndex = 2;
		}
		else if (m_nActiveButton == (int)i) {
				nImageIndex = 1;
		}

		if (!m_arrButton[i].bEnable || !m_bActivated)
			nImageIndex = 3;

		if (m_arrButton[i].type == ESNWT_MAXIMIZE) {
			if (::IsZoomed(m_hDerive))
				nImageIndex += 4;
		}

		CRect rcBtn = CalcPos(m_arrButton[i].pButton);
		thePainter.DrawImageGDI(
			pDC->GetSafeHdc(), 
			&m_arrButton[i].pButton->image, 
			rcBtn, 
			nImageIndex,
			0,
			0,
			0);
	}
}

void CElcSkinFrameBase::DrawSkinMenuBar(CDC* pDC)
{
	if (m_nMenuBar == 0)
		return;

	CRect rect;

	// draw menubar background
	rect.left = m_rect.left;
	rect.right = m_rect.right;
	rect.top = m_nCaption;
	rect.bottom = rect.top + m_nMenuBar;
	thePainter.DrawImageGDI(pDC->GetSafeHdc(), &m_skin.menubar.background, rect, 0, 0, 0, 0);

	CFont * pOldFont = pDC->SelectObject(&m_ftMenu);
	UINT nFormat = DT_VCENTER | DT_CENTER | DT_SINGLELINE;

	int nIndex = 0;
	for (CMenuItemArray::iterator it = m_arrMenu.begin();
		it != m_arrMenu.end();
		it ++, nIndex ++)
	{
		pDC->SetTextColor(m_skin.menubar.crTextNormal);

		if (nIndex == m_nActiveItem) {
			int nImageIndex = m_bMenuActivated ? 2 : 1;
			thePainter.DrawImageGDI(pDC->GetSafeHdc(),
				&m_skin.menubar.itembk,
				it->rect,
				nImageIndex,
				0,
				0,
				0);

			if (m_bMenuActivated)
				pDC->SetTextColor(m_skin.menubar.crTextDown);
			else
				pDC->SetTextColor(m_skin.menubar.crTextHot);
		}

		pDC->DrawText(it->strItem, it->rect, nFormat);
	}

	pDC->SelectObject(pOldFont);
}

void CElcSkinFrameBase::DrawClientBackground(CDC* pDC)
{
	if (!m_bDrawClientBackgnd)
		return;

	CRect rcClient, rcImage;
	::GetClientRect(m_hDerive, &rcClient);

	CElcMemDC dcMem(pDC, rcClient);

	// fill background
	dcMem.FillSolidRect(rcClient, m_skin.client.crBackgnd);

	if (m_skin.client.pane.image.pImage) 
	{
		for (CClientImagePosArray::iterator it = m_arrClientImagePos.begin();
			it != m_arrClientImagePos.end();
			it ++) 
		{
			rcImage = CalcRect(&rcClient, &(it->pos.point), &(it->pos.size));
			thePainter.DrawImageGDI(dcMem.GetSafeHdc(),
				&m_skin.client.pane.image,
				rcImage,
				0,
				0,
				1,
				0);
		}
	}

	dcMem.DrawMe();
}

LRESULT CElcSkinFrameBase::NcHitTest(CPoint point)
{
	GetOppositePoint(point);

	if (::IsIconic(m_hDerive)) {
		CRect rc;

		for (CFrameButtonArray::iterator it = m_arrButton.begin();
			it != m_arrButton.end();
			it ++)
		{
			if (it->type >= ESNWT_CUSTOM_BASE)
				continue;

			rc = CalcPos(it->pButton);
			if (rc.PtInRect(point)) {
				return it->type;
			}
		}

		if (m_bHasIcon) {
			rc.left = FRAME_ICON_MARGIN + FRAME_ICON_MARGIN;
			rc.right = rc.left + FRAME_ICON_SIZE;
			rc.top = (m_nCaption - FRAME_ICON_SIZE) / 2;
			rc.bottom = rc.top + FRAME_ICON_SIZE;
			if (rc.PtInRect(point))
				return HTSYSMENU;
		}
	}
	else {
		CRect rcClient, rc;

		rcClient.left = m_rect.left + m_nLeft;
		rcClient.right = m_rect.right - m_nRight;
		rcClient.top = m_rect.top + m_nCaption + m_nMenuBar;
		rcClient.bottom = m_rect.bottom - m_nCaption;

		CRect rcLT(0, 0, RESIZE_BRODER, RESIZE_BRODER);
		CRect rcLB(0, m_rect.bottom-m_nBottom, m_nLeft, m_rect.bottom);
		CRect rcRT(m_rect.right-m_nRight, 0, m_rect.right, m_nRight);
		CRect rcRB(m_rect.right-m_nRight, m_rect.bottom-m_nBottom, m_rect.right, m_rect.bottom);

		CRect rcLeft(rcLT.left, rcLT.bottom, rcLT.right, rcLB.top);
		CRect rcTop(rcLT.right, rcLT.top, rcRT.left, rcLT.top+RESIZE_BRODER);
		CRect rcRight(rcRT.left, rcRT.bottom, rcRT.right, rcRB.top);
		CRect rcBottom(rcLB.right, rcLB.top, rcRB.left, rcRB.bottom);

		LONG style = GetWindowLong(m_hDerive, GWL_STYLE);

		if (rcClient.PtInRect(point)) {
			return HTCLIENT;
		}
		else {
			if (style & WS_THICKFRAME) {
				LRESULT lRet = TrackBorderHitTest(point);
				if (lRet != HTCAPTION)
					return lRet;
			}

			for (CFrameButtonArray::iterator it = m_arrButton.begin();
				it != m_arrButton.end();
				it ++)
			{
				rc = CalcPos(it->pButton);
				if (rc.PtInRect(point)) {
					return it->type;
				}
			}

			if (m_nMenuBar > 0) {
				rc = m_rect;
				rc.top = m_nCaption;
				rc.bottom = m_nCaption + m_nMenuBar;
				if (rc.PtInRect(point))
					return HTMENU;
			}

			if (m_bHasIcon) {
				rc.left = FRAME_ICON_MARGIN + FRAME_ICON_MARGIN;
				rc.right = rc.left + FRAME_ICON_SIZE;
				rc.top = (m_nCaption - FRAME_ICON_SIZE) / 2;
				rc.bottom = rc.top + FRAME_ICON_SIZE;
				if (rc.PtInRect(point))
					return HTSYSMENU;
			}
		}
	}

	return HTCAPTION;
}

LRESULT CElcSkinFrameBase::TrackBorderHitTest(CPoint point)
{
	CRect rcLT(0, 0, RESIZE_BRODER, RESIZE_BRODER);
	CRect rcLB(0, m_rect.bottom-m_nBottom, m_nLeft, m_rect.bottom);
	CRect rcRT(m_rect.right-m_nRight, 0, m_rect.right, m_nRight);
	CRect rcRB(m_rect.right-m_nRight, m_rect.bottom-m_nBottom, m_rect.right, m_rect.bottom);

	CRect rcLeft(rcLT.left, rcLT.bottom, rcLT.right, rcLB.top);
	CRect rcTop(rcLT.right, rcLT.top, rcRT.left, rcLT.top+RESIZE_BRODER);
	CRect rcRight(rcRT.left, rcRT.bottom, rcRT.right, rcRB.top);
	CRect rcBottom(rcLB.right, rcLB.top, rcRB.left, rcRB.bottom);

	if (rcLT.PtInRect(point))
		return HTTOPLEFT;
	else if (rcLB.PtInRect(point))
		return HTBOTTOMLEFT;
	else if (rcRT.PtInRect(point))
		return HTTOPRIGHT;
	else if (rcRB.PtInRect(point))
		return HTBOTTOMRIGHT;
	else if (rcLeft.PtInRect(point))
		return HTLEFT;
	else if (rcTop.PtInRect(point))
		return HTTOP;
	else if (rcRight.PtInRect(point))
		return HTRIGHT;
	else if (rcBottom.PtInRect(point))
		return HTBOTTOM;
	else 
		return HTCAPTION;
}

BOOL CElcSkinFrameBase::NcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if (HTSYSMENU == nHitTest) {
		PostMessage(m_hDerive, WM_SYSCOMMAND, SC_CLOSE, 0);
	}
	else if (HTCAPTION == nHitTest) {
		if (::IsZoomed(m_hDerive))
			PostMessage(m_hDerive, WM_SYSCOMMAND, SC_RESTORE, 0);
		else
			PostMessage(m_hDerive, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else {
		return FALSE;
	}

	return TRUE;
}

BOOL CElcSkinFrameBase::NcLButtonDown(UINT nHitTest, CPoint point)
{
	GetOppositePoint(point);

	m_nLBDownItem = ESNWT_NONE;

	if (HTMENU == nHitTest) {
		int nIndex = PtInMenuItem(point);
		if (nIndex != -1) {
			m_bMenuActivated = !m_bMenuActivated;
				m_nActiveItem = nIndex;

				if (m_bMenuActivated) {
					RedrawMenuBar();
					//m_bSelfCancelMenu = TRUE;
					PopupMenu(nIndex);
				}
				else {
					m_bSelfCancelMenu = TRUE;
					::SendMessage(m_hDerive, WM_CANCELMODE, 0, 0);
					RedrawMenuBar();
				}
		}
	}
	else if (HTSYSMENU == nHitTest) {
		PopupSysMenu();
	}
	else {
		int nItem = FindButton(nHitTest);
		if (nItem != -1) {
			m_nLBDownItem = nHitTest;
			RedrawButton(m_arrButton[nItem].pButton);
			return TRUE;
		}

		m_nActiveItem = -1;
		m_bMenuActivated = FALSE;
		m_nActiveButton = -1;
		m_nLBDownItem = ESNWT_NONE;
		RedrawMenuBar();
		return FALSE;
	}

	return TRUE;
}

BOOL CElcSkinFrameBase::NcLButtonUp(UINT nHitTest, CPoint point)
{
	PressFrameButton(nHitTest);

	m_nActiveItem = -1;
	m_bMenuActivated = FALSE;
	m_nActiveButton = -1;
	m_nLBDownItem = ESNWT_NONE;

	RedrawCaption();
	RedrawMenuBar();

	return TRUE;
}

BOOL CElcSkinFrameBase::NcMouseMove(UINT nHitTest, CPoint point)
{
	GetOppositePoint(point);

	BOOL bChanged = FALSE;

	int nIndex = 0;
	int nCount = 0;
	if (nHitTest == HTMENU) {
		nIndex = PtInMenuItem(point);
		if (nIndex != -1) {
			if (m_nActiveItem != nIndex) {
				m_nActiveItem = nIndex;
				RedrawMenuBar();
			}
			return TRUE;
		}

		if (m_nActiveItem != -1 && !m_bMenuActivated) {
			m_nActiveItem = -1;
			RedrawMenuBar();
		}
	}

	nCount = (int)m_arrButton.size();
	for (nIndex = 0; nIndex < nCount; nIndex ++) {
		CRect rc = CalcPos(m_arrButton[nIndex].pButton);
		if (rc.PtInRect(point)) {
			if (m_nActiveButton != nIndex) {
				m_nActiveButton = nIndex;
				RedrawButton(m_arrButton[nIndex].pButton);
			}
			return TRUE;
		}
	}

	if (m_nActiveButton != -1) {
		nIndex = m_nActiveButton;
		m_nActiveButton = -1;
		RedrawButton(m_arrButton[nIndex].pButton);
	}

	return FALSE;
}

BOOL CElcSkinFrameBase::NcRButtonDown(UINT nHitTest, CPoint point)
{
	m_nActiveItem = -1;
	m_bMenuActivated = FALSE;
	m_nActiveButton = -1;
	m_nLBDownItem = ESNWT_NONE;

	RedrawCaption();
	RedrawMenuBar();

	PopupSysMenu();

	return TRUE;
}

BOOL CElcSkinFrameBase::NcActivate(BOOL bActive)
{
	m_bActivated = bActive;
	if (!bActive) {
		m_nActiveItem = -1;
		m_bMenuActivated = FALSE;
		m_nActiveButton = -1;
		m_nLBDownItem = ESNWT_NONE;
	}

	RedrawFrame();

	return TRUE;
}

void CElcSkinFrameBase::SetWindowRegion()
{
	/*
	HRGN hRgn = CreateRectRgn(0,0,0,0);
	::GetWindowRgn(m_hDerive, hRgn);
	::SetWindowRgn(m_hDerive, NULL, FALSE);
	::DeleteObject(hRgn);

	CRect rect;
	GetWindowRect(m_hDerive, rect);
	hRgn = CreateRoundRectRgn(0, 0, rect.Width(), rect.Height(), 12, 12);
	::SetWindowRgn(m_hDerive, hRgn, TRUE);
	*/
}

void CElcSkinFrameBase::PressFrameButton(UINT nHitTest)
{
	if (m_nLBDownItem != nHitTest)
		return;

	int nItem = FindButton(m_nLBDownItem);
	if (nItem == -1 
		|| !m_arrButton[nItem].bEnable)
		return;

	switch (nHitTest) 
	{
	case ESNWT_MINIMIZE:
		{
			if (::IsIconic(m_hDerive))
				::PostMessage(m_hDerive, WM_SYSCOMMAND, SC_RESTORE, 0);
			else
				::PostMessage(m_hDerive, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
		break;
	case ESNWT_MAXIMIZE:
		{
			if (::IsZoomed(m_hDerive))
				::PostMessage(m_hDerive, WM_SYSCOMMAND, SC_RESTORE, 0);
			else
				::PostMessage(m_hDerive, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}
		break;
	case ESNWT_CLOSE:
		PostMessage(m_hDerive, WM_SYSCOMMAND, SC_CLOSE, 0);
		break;
	default:
		PostMessage(m_hDerive, WM_COMMAND, m_nLBDownItem-ESNWT_CUSTOM_BASE, 0);
		break;
	}
}

CRect CElcSkinFrameBase::CalcPos(const PELC_SKIN_NOTWND_BUTTON pButton)
{
	CRect rect(0,0,0,0);

	if (pButton->pos.point.x < 0)
		rect.left = m_rect.right + pButton->pos.point.x;
	else
		rect.left = pButton->pos.point.x;
	if (pButton->pos.point.y < 0)
		rect.top = m_rect.bottom + pButton->pos.point.y;
	else 
		rect.top = pButton->pos.point.y;

	if (pButton->image.bVertical) {
		rect.right = rect.left + ((Image *)pButton->image.pImage)->GetWidth();
		rect.bottom = rect.top + ((Image *)pButton->image.pImage)->GetHeight() / pButton->image.nCount;
	}
	else {
		rect.right = rect.left + ((Image *)pButton->image.pImage)->GetWidth() / pButton->image.nCount;
		rect.bottom = rect.top + ((Image *)pButton->image.pImage)->GetHeight();
	}
	
	return rect;
}

CRect CElcSkinFrameBase::CalcRect(const LPRECT lprcDest, const PPOINT lpTopLeft, const PSIZE pSize)
{
	CRect rect(0,0,0,0);

	if (lpTopLeft->x < 0)
		rect.left = lprcDest->right + lpTopLeft->x;
	else
		rect.left = lprcDest->left + lpTopLeft->x;
	if (lpTopLeft->y < 0)
		rect.top = lprcDest->bottom + lpTopLeft->y;
	else
		rect.top = lprcDest->top + lpTopLeft->y;

	if (pSize->cx <= 0)
		rect.right = lprcDest->right + pSize->cx;
	else
		rect.right = rect.left + pSize->cx;
	if (pSize->cy <= 0)
		rect.bottom = lprcDest->bottom + pSize->cy;
	else
		rect.bottom = rect.top + pSize->cy;

	return rect;
}

void CElcSkinFrameBase::GetOppositePoint(CPoint &point)
{
	RECT rect;
	::GetWindowRect(m_hDerive, &rect);
	point.Offset(-rect.left, -rect.top);
}

void CElcSkinFrameBase::GetOppositeRect(CRect &rect)
{
	RECT rc;
	::GetWindowRect(m_hDerive, &rc);
	rect.OffsetRect(-rc.left, -rc.top);
}

void CElcSkinFrameBase::GetScreenPoint(CPoint &point)
{
	RECT rect;
	::GetWindowRect(m_hDerive, &rect);
	point.Offset(rect.left, rect.top);
}

void CElcSkinFrameBase::RedrawMenuBar()
{
	if (m_nMenuBar <= 0)
		return;

	CRect rect;
	rect.left = 0;
	rect.top = m_nCaption;
	rect.right = m_rect.right;
	rect.bottom = rect.top + m_nMenuBar;

	::RedrawWindow(m_hDerive, &rect, NULL, RDW_FRAME|RDW_INVALIDATE|RDW_NOCHILDREN);
}

void CElcSkinFrameBase::RedrawButton(int type)
{
	for (CFrameButtonArray::iterator it = m_arrButton.begin();
		it != m_arrButton.end();
		it ++)
	{
		if (it->type == type) {
			RedrawButton(it->pButton);
			break;
		}
	}
}

void CElcSkinFrameBase::RedrawButton(PELC_SKIN_NOTWND_BUTTON pButton)
{
	CRect rect = CalcPos(pButton);
	::RedrawWindow(m_hDerive, &rect, NULL, RDW_FRAME|RDW_INVALIDATE|RDW_NOCHILDREN);
}

void CElcSkinFrameBase::RedrawCaption()
{
	CRect rect = m_rect;
	rect.bottom = m_nCaption;
	::RedrawWindow(m_hDerive, &rect, NULL, RDW_FRAME|RDW_INVALIDATE|RDW_NOCHILDREN);
}

void CElcSkinFrameBase::RedrawFrame()
{
	CRect rect, rcWindow;
	GetWindowRect(m_hDerive, &rcWindow);
	if (rcWindow.IsRectEmpty())
		return;

	GetClientRect(m_hDerive, &rect);
	::ClientToScreen(m_hDerive, &rect.TopLeft());
	::ClientToScreen(m_hDerive, &rect.BottomRight());
	GetOppositeRect(rect);

	CRgn rgn1, rgn2;
	rgn1.CreateRectRgnIndirect(rcWindow);
	rgn2.CreateRectRgnIndirect(rect);
	rgn1.CombineRgn(&rgn1, &rgn2, RGN_XOR);

	::RedrawWindow(m_hDerive, NULL, (HRGN)rgn1.GetSafeHandle(), RDW_FRAME|RDW_INVALIDATE);
}

BOOL CElcSkinFrameBase::PtInIcon(CPoint point)
{
	GetOppositePoint(point);

	CRect rect;
	rect.left = FRAME_ICON_MARGIN + FRAME_ICON_MARGIN;
	rect.right = rect.left + FRAME_ICON_SIZE;
	rect.top = (m_nCaption - FRAME_ICON_SIZE) / 2;
	rect.bottom = rect.top + FRAME_ICON_SIZE;

	return rect.PtInRect(point);
}

void CElcSkinFrameBase::PopupMenu(int nIndex)
{
	m_nActiveItem = nIndex;

	if (m_nActiveItem == -1) {
		return;
	}

	HMENU hMenu = ::GetSubMenu(m_hMenu, nIndex);
	if (!::IsMenu(hMenu)) {
		ASSERT(0);
		return;
	}

	CPoint point(m_arrMenu[nIndex].rect.left, m_arrMenu[nIndex].rect.bottom);
	GetScreenPoint(point);

	int nRet = ::TrackPopupMenu(hMenu, 
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
		point.x,
		point.y,
		0,
		m_hDerive,
		NULL);

	if (nRet != 0) {
		m_bMenuActivated = FALSE;
		m_nActiveItem = -1;

		::PostMessage(m_hDerive, WM_COMMAND, nRet, 0);
	}

	if (!m_bSelfCancelMenu) {
		m_bMenuActivated = FALSE;
		m_nActiveItem = -1;
	}
	else
		m_bSelfCancelMenu = FALSE;

	RedrawMenuBar();
}

void CElcSkinFrameBase::PopupSysMenu()
{
	HMENU hMenu = ::GetSystemMenu(m_hDerive, 0);
	if (!::IsMenu(hMenu))
		return;

	CPoint point;
	GetCursorPos(&point);

	int nRet = ::TrackPopupMenu(hMenu, 
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
		point.x,
		point.y,
		0,
		m_hDerive,
		NULL);

	if (nRet > 0)
		SendMessage(m_hDerive, WM_SYSCOMMAND, nRet, 0);
}

void CElcSkinFrameBase::EnterIdle(UINT nWhy, CWnd* pWho)
{
	if (nWhy == MSGF_MENU && m_bMenuActivated) {
		CPoint point;
		GetCursorPos(&point);
		GetOppositePoint(point);

		int nIndex = PtInMenuItem(point);
		if (nIndex != -1 
			&& m_nActiveItem != nIndex)
		{
			m_bSelfCancelMenu = TRUE;
			::SendMessage(m_hDerive, WM_CANCELMODE, 0, 0);

			m_nActiveItem = nIndex;
			RedrawMenuBar();

			::PostMessage(m_hDerive, WM_MENUBAR_NOTIFY, nIndex, 0);
		}
	}
}

int CElcSkinFrameBase::PtInMenuItem(CPoint point)
{
	int nCount = (int)m_arrMenu.size();
	for (int nIndex=0; nIndex < nCount; nIndex ++) {
		if (m_arrMenu[nIndex].rect.PtInRect(point)) {
			return nIndex;
		}
	}
	return -1;
}

void CElcSkinFrameBase::CheckStates()
{
	CPoint point;
	GetCursorPos(&point);
	GetOppositePoint(point);

	if (m_nActiveButton != -1) {
		CRect rect = CalcPos(m_arrButton[m_nActiveButton].pButton);
		if (!rect.PtInRect(point)) {
			m_nActiveButton = -1;
			m_nLBDownItem = ESNWT_NONE;
			RedrawCaption();
		}
	}

	if (m_nActiveItem != -1) {
		if (!m_arrMenu[m_nActiveItem].rect.PtInRect(point)) {
			if (!m_bMenuActivated) {
				m_nActiveItem = -1;
				RedrawMenuBar();
			}
		}
	}
}
