// ElcBalloon.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcBalloon.h"

#define ELCBALL_MAXWIDTH				600
#define ELCBALL_MINWIDTH				200
#define ELCBALL_DEFHEIGHT				120

#define ELCBN_HMARGIN					8
#define ELCBN_VMARGIN					5

#define ELCBALL_DEF_CAPTION_HEIGHT		24

#define ID_TIMER_POPUP					1
#define ID_TIMER_OUT					2

#define ESCAPE_POPOUP					15
#define ESCAPE_TIMEOUT					100

#define ID_CHECK_BOX					1000
#define ID_BTN_CLOSE					1001

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CElcBalloon

IMPLEMENT_DYNAMIC(CElcBalloon, CWnd)

CElcBalloon::CElcBalloon(BALLOON_INFO balloon)
{
	LoadSkin(_T("style1"));

	m_ballInfo = balloon;
	m_nFactHeight = 0;
	m_bShowBall = TRUE;
	m_bBlock = FALSE;
	m_bLButtonDown = FALSE;
	m_nTimeroutCount = 0;
	m_bShowTrack = TRUE;
	m_bMirror = FALSE;
	m_pTitleIcon = 0;

	m_nTrackItemAlign = TRACKITEM_ALIGN_LEFT;
	m_nHotItem = -1;
}

CElcBalloon::~CElcBalloon()
{
}


BEGIN_MESSAGE_MAP(CElcBalloon, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()

	ON_COMMAND(ID_CHECK_BOX, OnCheckBox)
	ON_COMMAND(ID_BTN_CLOSE, OnClickClose)
	ON_MESSAGE(WM_ELC_UI_DRAW_NOTIFY, OnElcUIDrawNotify)
END_MESSAGE_MAP()



// CElcBalloon message handlers

BOOL CElcBalloon::LoadSkin(LPCTSTR lpszStyle)
{
	memset(&m_skin, 0, sizeof(m_skin));

	PELC_SKIN_BALLOON pSkin = theSkin.GetBalloonSkin(lpszStyle);
	if (!pSkin) {
		// not find the style, pls check it
		ASSERT(0);
		return FALSE;
	}

	memcpy(&m_skin, pSkin, sizeof(ELC_SKIN_BALLOON));

	LOGFONT lf;
	if (!CElcSkinDoc::CreateSkinFont(&m_skin.ftCaption, m_ftCaption)) {
		GetMSShellDlgFont(lf);
		lf.lfWeight = FW_BOLD;

		if (m_ftCaption.GetSafeHandle())
			m_ftCaption.DeleteObject();
		m_ftCaption.CreateFontIndirect(&lf);
	}

	if (!CElcSkinDoc::CreateSkinFont(&m_skin.ftTitle, m_ftTitle)) {
		GetMSShellDlgFont(lf);
		lf.lfWeight = FW_BOLD;

		if (m_ftTitle.GetSafeHandle())
			m_ftTitle.DeleteObject();
		m_ftTitle.CreateFontIndirect(&lf);
	}

	if (CElcSkinDoc::CreateSkinFont(&m_skin.ftItem, lf)) {
		if (m_ballInfo.pMouseClicked)
			lf.lfUnderline = TRUE;

		if (m_ftItem.GetSafeHandle())
			m_ftItem.DeleteObject();
		m_ftItem.CreateFontIndirect(&lf);

		if (m_ftHotItem.GetSafeHandle())
			m_ftHotItem.DeleteObject();
		m_ftHotItem.CreateFontIndirect(&lf);
	}
	else {
		GetMSShellDlgFont(lf);
		if (m_ballInfo.pMouseClicked)
			lf.lfUnderline = TRUE;

		if (m_ftItem.GetSafeHandle())
			m_ftItem.DeleteObject();
		m_ftItem.CreateFontIndirect(&lf);

		if (m_ftHotItem.GetSafeHandle())
			m_ftHotItem.DeleteObject();
		m_ftHotItem.CreateFontIndirect(&lf);
	}

	return TRUE;
}

UINT CElcBalloon::GetId()
{
	return m_ballInfo.nId;
}

void CElcBalloon::ShowTrackFlag(BOOL bShow)
{
	LOGFONT lf = {0};
	GetMSShellDlgFont(lf);
	if (bShow && m_ballInfo.pMouseClicked)
		lf.lfUnderline = TRUE;

	if (m_ftItem.GetSafeHandle())
		m_ftItem.DeleteObject();

	m_bShowTrack = bShow;
	m_ftItem.CreateFontIndirect(&lf);
}

CElcBalloon* CElcBalloon::CreateBalloon(BALLOON_INFO& balloon)
{
	CElcBalloon* t_pBalloon = new CElcBalloon(balloon);
	return t_pBalloon;
}

BOOL CElcBalloon::Popup(CWnd* pParent, int nPopupPos)
{
	m_bMirror = IsLayoutRTL();
	DWORD dwStyle = WS_POPUP;
	DWORD dwExStyle = WS_EX_TOOLWINDOW;
	if (m_bMirror) {
		dwExStyle |= WS_EX_LAYOUTRTL;
	}

	LPCTSTR pClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW);

	CRect rect(0,0,0,0);

	BOOL bRet = CreateEx(dwExStyle, pClassName, NULL, dwStyle, rect, pParent, 0);
	if (!bRet)
		return FALSE;

	if (!m_btnCheck.Create(m_ballInfo.strCheckBox, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, rect, this, ID_CHECK_BOX))
		return FALSE;

	if (!m_btnClose.Create(NULL, WS_CHILD | WS_VISIBLE, rect, this, ID_BTN_CLOSE))
		return FALSE;
	m_btnClose.SetIconImage((Image *)m_skin.close.pImage, m_skin.close.nCount == 1 ? ELCBIM_SINGLE : ELCBIM_FOUR_STATE);

	if (m_ballInfo.nTitleIcon) {
		m_pTitleIcon = ImageFromIDResource(m_ballInfo.nTitleIcon, ElcResType2String(m_ballInfo.titleIconType));
	}

	RecalcLayout();

	if (!m_ballInfo.strCheckBox.IsEmpty()) {
		m_btnCheck.SetTextColor(m_skin.crTitle);
		m_btnCheck.SetCheck(m_ballInfo.bChecked);
		m_btnCheck.MoveWindow(m_rcCheckBox);
	}

	m_btnClose.MoveWindow(m_rcClose);

	CDC * pDC = GetDC();
	m_dcMemBk.CreateMe(pDC, m_rcWindow);

	if (m_skin.background.pImage) {
		if (m_bMirror)
			m_dcMemBk.SetLayout(LAYOUT_LTR);
		thePainter.DrawImageGDI(m_dcMemBk.GetSafeHdc(),
			&m_skin.background,
			m_rcWindow, 
			0,
			0,
			1,
			0,
			0);

		CBitmap bmMask;
		CDC dcMask;
		bmMask.CreateCompatibleBitmap(pDC, m_rcWindow.Width(), m_rcWindow.Height());
		dcMask.CreateCompatibleDC(pDC);
		CBitmap * pOldBmp = dcMask.SelectObject(&bmMask);
		dcMask.BitBlt(0, 0, m_rcWindow.Width(), m_rcWindow.Height(), &m_dcMemBk, 0, 0, SRCCOPY);
		dcMask.SelectObject(pOldBmp);

		CreateRegionFromColor(&m_rgnWindow, &bmMask, RGB(255,0,255), 0);
	}

	ReleaseDC(pDC);

	CRect rcDesktop;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);

	if (ELC_POPUP_LEFT == nPopupPos) {
		m_rcWindow.OffsetRect(rcDesktop.left + ELCBN_HMARGIN, rcDesktop.bottom - m_rcWindow.Height());
	}
	else if (ELC_POPUP_RIGHT == nPopupPos) {
		m_rcWindow.OffsetRect(rcDesktop.right - m_rcWindow.Width() - ELCBN_HMARGIN, rcDesktop.bottom - m_rcWindow.Height() - ELCBN_VMARGIN);
	}
	else {
		m_rcWindow.OffsetRect(
			rcDesktop.left+ (rcDesktop.Width() - m_rcWindow.Width()) / 2, 
			rcDesktop.bottom - m_rcWindow.Height());
	}

	if (::IsWindow(g_hLastWnd)) 
		::PostMessage(g_hLastWnd, WM_CLOSE, 0, 0);

	g_hLastWnd = NULL;

	ShowBalloon(TRUE);
	if (m_ballInfo.nTimeout > 0)
		SetTimer(ID_TIMER_OUT, ESCAPE_TIMEOUT, NULL);

	g_hLastWnd = GetSafeHwnd();

	return TRUE;
}

int CElcBalloon::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CElcBalloon::ShowBalloon(BOOL bShow)
{
	m_bShowBall = bShow;
	SetTimer(ID_TIMER_POPUP, ESCAPE_POPOUP, NULL);
}

void CElcBalloon::RecalcLayout()
{
	if (m_arrItem.empty()) {
		ASSERT(0);
		return;
	}

	CSize sizeCap = CElcSkinDoc::GetStringSize(m_ballInfo.strCaption, &m_ftTitle);
	CSize sizeTitle = CElcSkinDoc::GetStringSize(m_ballInfo.strTitle, &m_ftTitle);
	CSize sizeItem = CElcSkinDoc::GetStringSize(m_arrItem[0].strText, &m_ftTitle);
	CSize sizeClose = m_btnClose.GetIdealSize();

	CSize sizeIcon(0, 0);
	if (m_ballInfo.bShowCaptonIcon)
		sizeIcon = CElcSkinDoc::GetImageSize(&m_skin.icon);

	CSize sizeCheckBox(0, 0);
	if (!m_ballInfo.strCheckBox.IsEmpty() && m_btnCheck.GetSafeHwnd())
		sizeCheckBox = CElcSkinDoc::GetStringSize(m_ballInfo.strCheckBox, m_btnCheck.GetFont());

	int nElcBallWidth = ELCBALL_MINWIDTH;
	if (m_ballInfo.bShowCaptonIcon) {
		if (nElcBallWidth < sizeCap.cx + sizeIcon.cx + sizeClose.cx + ELCBN_HMARGIN * 4)
			nElcBallWidth = sizeCap.cx + sizeIcon.cx + sizeClose.cx + ELCBN_HMARGIN * 4;
	}
	else {
		if (nElcBallWidth < sizeCap.cx + sizeClose.cx + ELCBN_HMARGIN * 3)
			nElcBallWidth = sizeCap.cx + sizeClose.cx + ELCBN_HMARGIN * 3;
	}

	int nCaptionHeight = sizeCap.cy + ELCBN_VMARGIN * 2;

	int nTitleHeight = sizeTitle.cy + ELCBN_VMARGIN * 2;
	if (m_pTitleIcon && (UINT)nTitleHeight < m_pTitleIcon->GetHeight() + ELCBN_VMARGIN * 2)
		nTitleHeight = m_pTitleIcon->GetHeight() + ELCBN_VMARGIN * 2;

	int nTitleWidth = sizeTitle.cx + ELCBN_HMARGIN * 2;
	if (m_pTitleIcon)
		nTitleWidth += m_pTitleIcon->GetWidth() + ELCBN_HMARGIN;
	if (nElcBallWidth < nTitleWidth)
		nElcBallWidth = nTitleWidth;

	int nTextHeight = ELCBALL_DEFHEIGHT - nCaptionHeight;
	if (!m_ballInfo.strTitle.IsEmpty())
		nTextHeight -= nTitleHeight;

	int nTextWidth = nElcBallWidth - ELCBN_HMARGIN * 4;
	if (m_pTitleIcon)
		nTextWidth -= m_pTitleIcon->GetWidth();

	CDC dc;
	if (!dc.CreateCompatibleDC(NULL))
		return;
	CFont* pOldFont = dc.SelectObject(&m_ftItem);

	if (m_arrItem.size() == 1) {
		while (1) {
			CRect rcItem(0, 0, nTextWidth, 0);
			dc.DrawText(m_arrItem[0].strText, rcItem, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX | DT_NOFULLWIDTHCHARBREAK);
			if (rcItem.Height() <= nTextHeight) {
				break;
			}
			else {
				nTextWidth += 20;
				nTextHeight += 12;
			}
		}

		if (m_pTitleIcon)
			m_arrItem[0].rect.left = m_pTitleIcon->GetWidth() + ELCBN_HMARGIN*2;
		else
			m_arrItem[0].rect.left = ELCBN_HMARGIN*2;

		m_arrItem[0].rect.right = m_arrItem[0].rect.left + nTextWidth;

		if (!m_ballInfo.strTitle.IsEmpty())
			m_arrItem[0].rect.top = nTitleHeight + nCaptionHeight;
		else
			m_arrItem[0].rect.top = nCaptionHeight + ELCBN_VMARGIN;

		m_arrItem[0].rect.bottom = m_arrItem[0].rect.top + nTextHeight;
	}
	else {
		CRect rcItem(ELCBN_HMARGIN*2, nCaptionHeight, 0, 0);
		if (m_pTitleIcon)
			rcItem.left += m_pTitleIcon->GetWidth();
		if (!m_ballInfo.strTitle.IsEmpty())
			rcItem.top += nTitleHeight;
		else 
			rcItem.top += ELCBN_VMARGIN;

		for (CTextArray::iterator it = m_arrItem.begin(); 
			it != m_arrItem.end(); 
			it ++)
		{
			CSize sizeDescr = dc.GetTextExtent(it->strText);

			rcItem.bottom = rcItem.top + sizeDescr.cy + ELCBN_VMARGIN;
			rcItem.right = rcItem.left + sizeDescr.cx;
			it->rect = rcItem;
			rcItem.top = rcItem.bottom;

			if (nTextWidth < sizeDescr.cx)
				nTextWidth = sizeDescr.cx;
		}

		if (ELCBALL_DEFHEIGHT < rcItem.bottom + ELCBN_VMARGIN)
			nTextHeight = rcItem.bottom + ELCBN_VMARGIN - nTitleHeight - nCaptionHeight;
	}

	if (m_pTitleIcon) {
		if (nElcBallWidth < (int)m_pTitleIcon->GetWidth() + nTextWidth + ELCBN_HMARGIN * 4)
			nElcBallWidth = (int)m_pTitleIcon->GetWidth() + nTextWidth + ELCBN_HMARGIN * 4;
	}
	else if (nElcBallWidth < nTextWidth + ELCBN_HMARGIN * 4)
		nElcBallWidth = nTextWidth + ELCBN_HMARGIN * 4;

	if (!m_ballInfo.strCheckBox.IsEmpty()
		&& nElcBallWidth < m_rcCheckBox.Width() + ELCBN_HMARGIN * 2)
		nElcBallWidth = m_rcCheckBox.Width() + ELCBN_HMARGIN * 2;

	dc.SelectObject(pOldFont);

	if (m_ballInfo.bShowCaptonIcon) {
		m_rcIcon.left = ELCBN_HMARGIN;
		m_rcIcon.right = m_rcIcon.left + sizeIcon.cx;
		m_rcIcon.top = (nCaptionHeight - sizeIcon.cy) / 2;
		m_rcIcon.bottom = m_rcIcon.top + sizeIcon.cy;
	}
	else {
		m_rcIcon.SetRectEmpty();
	}

	m_rcClose.right = nElcBallWidth - ELCBN_HMARGIN;
	m_rcClose.left = m_rcClose.right - sizeClose.cx;
	m_rcClose.top = (nCaptionHeight - sizeClose.cy) / 2;
	m_rcClose.bottom = m_rcClose.top + sizeClose.cy;

	m_rcCaption.left = m_rcIcon.right + ELCBN_HMARGIN;
	m_rcCaption.right = m_rcClose.left - ELCBN_HMARGIN;
	m_rcCaption.top = (nCaptionHeight - sizeCap.cy) / 2;
	m_rcCaption.bottom = m_rcCaption.top + sizeCap.cy;

	if (m_pTitleIcon) {
		m_rcTitleIcon.left = ELCBN_HMARGIN;
		m_rcTitleIcon.right = m_rcTitleIcon.left + m_pTitleIcon->GetWidth();
		m_rcTitleIcon.top = nCaptionHeight + ELCBN_VMARGIN;
		m_rcTitleIcon.bottom = m_rcTitleIcon.top + m_pTitleIcon->GetHeight();
	}
	else {
		m_rcTitleIcon.SetRectEmpty();
	}

	if (m_ballInfo.strTitle.IsEmpty()) {
		m_rcTitle.SetRectEmpty();
	}
	else {
		m_rcTitle.left = m_rcTitleIcon.right + ELCBN_HMARGIN;
		m_rcTitle.right = m_rcTitle.left + sizeTitle.cx;
		m_rcTitle.top = nCaptionHeight + (nTitleHeight - sizeTitle.cy) / 2;
		m_rcTitle.bottom = m_rcTitle.top + sizeTitle.cy;
	}

	if (m_ballInfo.strCheckBox.IsEmpty()) {
		m_rcCheckBox.SetRectEmpty();
	}
	else {
		m_rcCheckBox.left = ELCBN_HMARGIN;
		m_rcCheckBox.right = m_rcCheckBox.left + sizeCheckBox.cx + 20;
		
		if (m_ballInfo.strTitle.IsEmpty())
			m_rcCheckBox.top = nCaptionHeight + nTextHeight + ELCBN_VMARGIN;
		else
			m_rcCheckBox.top = nCaptionHeight + nTitleHeight + nTextHeight + ELCBN_VMARGIN;
		m_rcCheckBox.bottom = m_rcCheckBox.top + 16;
	}

	m_rcWindow.left = 0;
	m_rcWindow.right = nElcBallWidth;
	m_rcWindow.top = 0;

	if (!m_ballInfo.strCheckBox.IsEmpty())
		m_rcWindow.bottom = m_rcCheckBox.bottom + ELCBN_VMARGIN;
	else if (m_ballInfo.strTitle.IsEmpty())
		m_rcWindow.bottom = nCaptionHeight + nTextHeight + ELCBN_VMARGIN;
	else 
		m_rcWindow.bottom = nCaptionHeight + nTitleHeight + nTextHeight + ELCBN_VMARGIN;
}

void CElcBalloon::SetTitleFont(CFont* pFont)
{
	if (pFont == NULL)
		return ;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	pFont->GetLogFont(&lf);
	m_ftTitle.DeleteObject();
	m_ftTitle.CreateFontIndirect(&lf);
}

void CElcBalloon::SetDescrFont(CFont* pFont)
{
	if (pFont == NULL)
		return ;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	pFont->GetLogFont(&lf);
	m_ftItem.DeleteObject();
	m_ftItem.CreateFontIndirect(&lf);
}

void CElcBalloon::SetCheck(BOOL bCheck)
{
	m_ballInfo.bChecked = bCheck;
	if (m_btnCheck.GetSafeHwnd())
		m_btnCheck.SetCheck(bCheck);
}

BOOL CElcBalloon::GetCheck()
{
	return m_ballInfo.bChecked;
}

BOOL CElcBalloon::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CElcBalloon::OnPaint()
{
	CPaintDC dc(this); 

	CRect rcClient;
	GetClientRect(&rcClient);

	CElcMemDC dcMem(&dc, rcClient);
	DrawBalloon(&dcMem);
	dcMem.DrawMe();
}

void CElcBalloon::DrawBalloon(CDC* pDC)
{
	CRect rcDest;
	GetClientRect(&rcDest);

	if (m_dcMemBk.GetSafeHdc()) {
		pDC->BitBlt(0, 0, m_rcWindow.Width(), m_rcWindow.Height(), &m_dcMemBk, 0, 0, SRCCOPY);
	}

	UINT uFormat = DT_WORDBREAK|DT_NOPREFIX|DT_WORD_ELLIPSIS|DT_VCENTER|DT_SINGLELINE;

	if (m_ballInfo.bShowCaptonIcon && m_skin.icon.pImage) {
		thePainter.DrawImageGDI(pDC->GetSafeHdc(),
			&m_skin.icon,
			m_bMirror ? MirrorRect(m_rcWindow.Width(), m_rcIcon) : m_rcIcon,
			0,
			0,
			1,
			0,
			0);
	}

	/*
	if (m_skin.close.pImage) {
		thePainter.DrawImageGDI(pDC->GetSafeHdc(),
			&m_skin.close,
			m_rcClose,
			0,
			0,
			1,
			0,
			0);
	}
	*/

	if (m_pTitleIcon) {
		thePainter.DrawImageGDI(pDC->GetSafeHdc(),
			m_pTitleIcon,
			m_bMirror ? MirrorRect(m_rcWindow.Width(), m_rcTitleIcon) : m_rcTitleIcon,
			0);
	}

	CFont * pOldFont = pDC->SelectObject(&m_ftCaption);
	pDC->SetTextColor(m_skin.crCaption);
	pDC->DrawText(m_ballInfo.strCaption, m_rcCaption, uFormat);

	pDC->SelectObject(&m_ftTitle);
	pDC->SetTextColor(m_skin.crTitle);
	pDC->DrawText(m_ballInfo.strTitle, m_rcTitle, uFormat);
	
	pDC->SelectObject(&m_ftItem);
	pDC->SetTextColor(m_skin.crItem);

	if (m_arrItem.size() == 1) {
		if (m_nHotItem != -1 && m_ballInfo.pMouseClicked) {
			pDC->SelectObject(&m_ftHotItem);
			pDC->SetTextColor(m_skin.crHotItem);
		}

		pDC->DrawText(m_arrItem[0].strText, m_arrItem[0].rect, DT_WORDBREAK | DT_NOPREFIX | DT_WORD_ELLIPSIS);
	}
	else {
		if (m_nTrackItemAlign == TRACKITEM_ALIGN_LEFT)
			uFormat |= DT_LEFT;
		else if (m_nTrackItemAlign == TRACKITEM_ALIGN_CENTER)
			uFormat |= DT_CENTER;
		else if (m_nTrackItemAlign == TRACKITEM_ALIGN_RIGHT)
			uFormat |= DT_RIGHT;

		for (int i=0; i<(int)m_arrItem.size(); i++) {
			if (m_nHotItem == i)
				continue;
			
			pDC->DrawText(m_arrItem[i].strText, m_arrItem[i].rect, uFormat);
		}

		if (m_nHotItem != -1) {
			pDC->SelectObject(&m_ftHotItem);
			pDC->SetTextColor(m_skin.crHotItem);
			pDC->DrawText(m_arrItem[m_nHotItem].strText, m_arrItem[m_nHotItem].rect, uFormat);
		}
	}	
	
	pDC->SelectObject(pOldFont);
}

LRESULT CElcBalloon::OnElcUIDrawNotify(WPARAM wParam, LPARAM lParam)
{
	PELC_UI_DRAW pDraw = (PELC_UI_DRAW)lParam;
	if (pDraw) {
		CPoint pt(pDraw->rect.left, pDraw->rect.top);
		::ClientToScreen(pDraw->hWnd, &pt);
		ScreenToClient(&pt);
		::BitBlt(pDraw->hdc, 
			0, 
			0, 
			pDraw->rect.right - pDraw->rect.left, 
			pDraw->rect.bottom - pDraw->rect.top, 
			m_dcMemBk.GetSafeHdc(),
			pt.x,
			pt.y,
			SRCCOPY);

		return 1;
	}

	return 0;
}

void CElcBalloon::OnCheckBox()
{
	m_ballInfo.bChecked = m_btnCheck.GetCheck();
	if (m_ballInfo.pMouseClicked) {
		ELC_BALLOON_CALLBACK callback;
		callback.pThis = this;
		callback.nTrackId = ELC_BALLOON_CHECKBOX;
		callback.dwUser = m_ballInfo.dwUser;
		m_ballInfo.pMouseClicked(&callback);
	}
}

void CElcBalloon::OnClickClose()
{
	OnClose();
}

void CElcBalloon::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bBlock)
		return;

	m_bLButtonDown = TRUE;
	SetCapture();

	__super::OnLButtonDown(nFlags, point);
}

void CElcBalloon::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bLButtonDown)
		return;

	ReleaseCapture();

	if (m_nHotItem != -1)
	{
		UINT nID = 0;
		if (!m_arrItem.empty())
			nID = m_arrItem[m_nHotItem].nID;

		m_bBlock = TRUE;

		if (m_bShowTrack && m_ballInfo.pMouseClicked) {
			ShowWindow(SW_HIDE);
			ELC_BALLOON_CALLBACK callback;
			callback.pThis = this;
			callback.nTrackId = nID;
			callback.dwUser = m_ballInfo.dwUser;
			m_ballInfo.pMouseClicked(&callback);
		}

		DestroyMe();

		return;
	}

	m_bLButtonDown = FALSE;

	__super::OnLButtonUp(nFlags, point);
}

void CElcBalloon::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bBlock)
		return;

	int nNewHotItem = -1;

	if (m_bShowTrack && m_ballInfo.pMouseClicked) {
		for (int i=0; i < (int)m_arrItem.size(); i++) {
			if (m_arrItem[i].rect.PtInRect(point)) {
				nNewHotItem = i;
				break;
			}
		}

		if (nNewHotItem != m_nHotItem) {
			m_nHotItem = nNewHotItem;
			Invalidate();
		}
	}

	if (nNewHotItem != -1 && m_ballInfo.pMouseClicked)
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
	else
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	__super::OnMouseMove(nFlags, point);
}

void CElcBalloon::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) 
	{
	case ID_TIMER_POPUP:
		{
			if (m_bShowBall)  {
				m_nFactHeight += 10;
				if (m_nFactHeight >= m_rcWindow.Height()) {
					KillTimer(ID_TIMER_POPUP);
					SetWindowPos(&CWnd::wndTopMost,
						m_rcWindow.left, 
						m_rcWindow.top,
						m_rcWindow.Width(),
						m_rcWindow.Height(),
						SWP_SHOWWINDOW);

					m_nFactHeight = m_rcWindow.Height();

					if (m_rgnWindow.GetSafeHandle() != NULL)
						SetWindowRgn((HRGN)m_rgnWindow.m_hObject, TRUE);

					return;
				}
			}
			else  {
				m_nFactHeight -= 10;
				if (m_nFactHeight <= 10) {
					KillTimer(ID_TIMER_POPUP);
					DestroyMe();
					return;
				}
			}

			CRect rect;
			rect.bottom = m_rcWindow.bottom;
			rect.top = rect.bottom - m_nFactHeight;
			rect.left = m_rcWindow.left;
			rect.right = m_rcWindow.right;

			SetWindowPos(&CWnd::wndTopMost,
				rect.left, 
				rect.top,
				rect.Width(),
				rect.Height(),
				SWP_SHOWWINDOW);

			if (m_rgnWindow.GetSafeHandle() != NULL)
				SetWindowRgn((HRGN)m_rgnWindow.m_hObject, TRUE);
		}
		break;
	case ID_TIMER_OUT:
		{
			CRect rcWindow;
			GetWindowRect(rcWindow);
			CPoint point;
			GetCursorPos(&point);
			if (!rcWindow.PtInRect(point)) {
				m_nTimeroutCount += ESCAPE_TIMEOUT;
				if (m_nTimeroutCount >= m_ballInfo.nTimeout) {
					m_bShowBall = FALSE;
					SetTimer(ID_TIMER_POPUP, ESCAPE_POPOUP, NULL);
				}
			}
			else {
				m_nTimeroutCount = 0;

				if (m_nFactHeight < m_rcWindow.Height()) {
					m_bShowBall = TRUE;
					SetTimer(ID_TIMER_POPUP, ESCAPE_POPOUP, NULL);
				}
			}
		}
		break;
	default:
		break;
	}

	__super::OnTimer(nIDEvent);
}

void CElcBalloon::DestroyMe()
{
	SetWindowRgn(NULL, FALSE);
	if (m_rgnWindow.GetSafeHandle() != NULL)
		m_rgnWindow.DeleteObject();

	if (g_hLastWnd == m_hWnd)
		g_hLastWnd = NULL;

	if (::IsWindow(m_hWnd)) {
		DestroyWindow();
	}
}

void CElcBalloon::OnClose()
{
	DestroyMe();
}

void CElcBalloon::OnDestroy()
{
	__super::OnDestroy();

	delete this;
}

void CElcBalloon::AddTrackItem(UINT nID, LPCTSTR lpszText)
{
	TRACKITEM item;
	item.nID = nID;
	item.strText = lpszText;
	item.bHot = FALSE;

	m_arrItem.push_back(item);
}

void CElcBalloon::SetTrackItemAlign(TRACKITEM_ALIGN align)
{
	m_nTrackItemAlign = align;
}

