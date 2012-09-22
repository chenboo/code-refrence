// BlendSelectWnd.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "BlendSelectWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


enum {
	TIMER_ID_ANIMATEIN = 1,
	TIMER_ID_SHOW,
	TIMER_ID_ANIMATEOUT
};

// CBlendSelectWnd

IMPLEMENT_DYNAMIC(CBlendSelectWnd, CWnd)

CBlendSelectWnd::CBlendSelectWnd()
{
	m_hIcon = NULL;

	m_crText = RGB(255, 255, 255);
	m_crBkgnd = RGB(95, 95, 95);

	m_dwAnimateDelay = 500;
	m_dwShowDelay = 3000;

	m_bAnimateInTimer = FALSE;
	m_bAnimateOutTimer = FALSE;
	m_bShowTimer = FALSE;

	m_wAlpha = 0;

	m_dwWidth = 500;
	m_dwHeight = 100;
	m_dwMargin = 20;

	m_dwTitleHeight = 0;

	m_hIconClose = NULL;
	m_hIconCloseDown = NULL;

	m_hIconCheck = NULL;
	m_hIconUnCheck = NULL;

	m_bCloseDown = FALSE;
}

CBlendSelectWnd::~CBlendSelectWnd()
{
}


BEGIN_MESSAGE_MAP(CBlendSelectWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CBlendSelectWnd message handlers

BOOL CBlendSelectWnd::Create(CRect rc, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	//if hwnd already exist - return TRUE;
	if (GetSafeHwnd())
		return TRUE;

	m_dwWidth = rc.Width();
	m_dwHeight = rc.Height();

	int nGap = 4;

	m_rcBtnClose.right = m_dwWidth - nGap;
	m_rcBtnClose.top = nGap;
	m_rcBtnClose.left = m_rcBtnClose.right - 16;
	m_rcBtnClose.bottom = m_rcBtnClose.top + 16;

	DWORD dwStyleEx = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED;

	DWORD dwLayout;
	GetProcessDefaultLayout(&dwLayout);
	if (LAYOUT_RTL & dwLayout) {
		dwStyleEx |= WS_EX_LAYOUTRTL;
	}

	LPCTSTR pszClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW);

	if (!CreateEx(dwStyleEx, pszClassName, NULL, dwStyle, rc, pParentWnd, nID))
		return FALSE;

	SetOwner(pParentWnd);

	return TRUE;
}

BOOL CBlendSelectWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	pDC->FillSolidRect(&rcClient, m_crBkgnd);

	return TRUE;
}

int CBlendSelectWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_dwTitleHeight = CalculateTitleHeight();

	int nBtnLeft = (m_hIcon == NULL) ? m_dwMargin : (m_dwMargin * 2 + 32);
	int nBtnWidth = (m_hIcon == NULL) ? (m_dwWidth - 2 * m_dwMargin) : (m_dwWidth - 3 * m_dwMargin - 32);
	int nButTop = 2 * m_dwMargin + m_dwTitleHeight;

	m_wndListBox.SetTextColor(m_crText);
	m_wndListBox.SetBkColor(m_crBkgnd);
	m_wndListBox.SetRadioIcon(m_hIconCheck, m_hIconUnCheck);

	m_wndListBox.SetFont(&m_ftOption);

	CRect rcListBox(nBtnLeft, nButTop, nBtnLeft + nBtnWidth, nButTop);
	if (!m_wndListBox.Create(rcListBox, this, 0, WS_CHILD | WS_VISIBLE|WS_VSCROLL, RLS_HAS_RADIO))
		return -1;

	m_dwHeight = m_dwMargin * 3 + m_dwTitleHeight + m_wndListBox.GetListHeight();

	CRgn CaptionRegion;
	CaptionRegion.CreateRoundRectRgn(0, 
		0, 
		m_dwWidth, 
		m_dwHeight, 
		10, 
		10);
	SetWindowRgn((HRGN)CaptionRegion.m_hObject, TRUE);

	if (m_dwAnimateDelay > 0) {
		m_wAlpha = 0;
		DWORD dwInterval = m_dwAnimateDelay / 16;
		::SetLayeredWindowAttributes(GetSafeHwnd(), 0, (BYTE)m_wAlpha, LWA_ALPHA);

		m_bAnimateInTimer = TRUE;
		SetTimer(TIMER_ID_ANIMATEIN, dwInterval, NULL);
	} else
	{
		m_wAlpha = 255;
		::SetLayeredWindowAttributes(GetSafeHwnd(), 0, (BYTE)m_wAlpha, LWA_ALPHA);

		if (m_dwShowDelay > 0) 
		{
			SetTimer(TIMER_ID_SHOW, m_dwShowDelay, NULL);
			m_bShowTimer = TRUE;
		}
	}

	return 0;
}

void CBlendSelectWnd::SetWindowCenter()
{
	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(NULL, (cxScreen - m_dwWidth) / 2, (cyScreen - m_dwHeight) / 2, 
		m_dwWidth, m_dwHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
}

void CBlendSelectWnd::OnClose()
{
	CWnd::OnClose();

	delete this;
}

void CBlendSelectWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_ANIMATEIN) 
	{
		m_wAlpha += 16;
		if (m_wAlpha < 255) {
			::SetLayeredWindowAttributes(GetSafeHwnd(), 0, (BYTE)m_wAlpha, LWA_ALPHA);
		} else
		{
			m_wAlpha = 255;
			::SetLayeredWindowAttributes(GetSafeHwnd(), 0, (BYTE)m_wAlpha, LWA_ALPHA);

			KillTimer(TIMER_ID_ANIMATEIN);

			m_bAnimateInTimer = FALSE;

			if (m_dwShowDelay > 0) 
			{
				SetTimer(TIMER_ID_SHOW, m_dwShowDelay, NULL);
				m_bShowTimer = TRUE;
			}
		}
	} else if (nIDEvent == TIMER_ID_SHOW) 
	{
		KillTimer(TIMER_ID_SHOW);
		m_bShowTimer = FALSE;

		if (m_dwAnimateDelay > 0) {

			m_wAlpha = 240;
			DWORD dwInterval = m_dwAnimateDelay / 16;
			::SetLayeredWindowAttributes(GetSafeHwnd(), 0, (BYTE)m_wAlpha, LWA_ALPHA);

			m_bAnimateOutTimer = TRUE;
			SetTimer(TIMER_ID_ANIMATEOUT, dwInterval, NULL);
		} else
		{
			PostMessage(WM_CLOSE, 0, 0);
		}
	} else if (nIDEvent == TIMER_ID_ANIMATEOUT)
	{
		m_wAlpha -= 16;

		if (m_wAlpha > 0) {
			::SetLayeredWindowAttributes(GetSafeHwnd(), 0, (BYTE)m_wAlpha, LWA_ALPHA);
		} else
		{
			KillTimer(TIMER_ID_ANIMATEOUT);
			m_bAnimateOutTimer = FALSE;

			PostMessage(WM_CLOSE, 0, 0);
		}
	}

	CWnd::OnTimer(nIDEvent);
}

void CBlendSelectWnd::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient(0,0,m_dwWidth, m_dwMargin + m_dwTitleHeight + m_dwMargin);

	CDC dcMem;
	if (!dcMem.CreateCompatibleDC(&dc))
		return;

	CBitmap bm;
	bm.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	dcMem.SelectObject(&bm);

	dcMem.FillSolidRect(&rcClient, m_crBkgnd);

	dcMem.SetTextColor(m_crText);
	dcMem.SetBkColor(m_crBkgnd);

	// Draw Close Button
	::DrawIconEx(dcMem.GetSafeHdc(), m_rcBtnClose.left, m_rcBtnClose.top, 
		m_bCloseDown ? m_hIconCloseDown : m_hIconClose, 16, 16, 0, NULL, DI_NORMAL);

	// Draw Icon
	int nIconSize = 0;

	if (m_hIcon) {
		dcMem.DrawIcon(rcClient.left + m_dwMargin, rcClient.top + m_dwMargin, m_hIcon);
		nIconSize = 32;
	}

	// Draw Title
	CRect rcTitle = rcClient;
	rcTitle.left += (nIconSize > 0) ? m_dwMargin * 2 + nIconSize : m_dwMargin;
	rcTitle.right -= m_dwMargin;
	rcTitle.top += m_dwMargin;
	rcTitle.bottom = rcClient.bottom;

	if (m_ftTitle.GetSafeHandle()) {
		dcMem.SelectObject(&m_ftTitle);
	}

	dcMem.DrawText(m_strTitle, m_strTitle.GetLength(), &rcTitle, DT_WORDBREAK | DT_NOPREFIX);

	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(),
		&dcMem, 0, 0, SRCCOPY);
}

void CBlendSelectWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
/*
	if (!m_dwShowDelay) {
		PostMessage(WM_CLOSE, 0, 0);
	}
*/

	if (m_rcBtnClose.PtInRect(point)) {
		m_bCloseDown = TRUE;
		InvalidateRect(&m_rcBtnClose);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CBlendSelectWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_rcBtnClose.PtInRect(point)) {
		if (m_bCloseDown) {
			PostMessage(WM_CLOSE, 0, 0);
		}
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CBlendSelectWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_rcBtnClose.PtInRect(point)) {
		if (m_bCloseDown) {
			m_bCloseDown = FALSE;
			InvalidateRect(&m_rcBtnClose);
		}
	}
/*
	CRect rcClient;
	GetClientRect(&rcClient);

	BOOL bInRect = rcClient.PtInRect(point);
	if (bInRect) {

		SetCapture();

		if (m_bAnimateInTimer) {
			m_wAlpha = 255;
			::SetLayeredWindowAttributes(GetSafeHwnd(), 0, (BYTE)m_wAlpha, LWA_ALPHA);

			KillTimer(TIMER_ID_ANIMATEIN);
			m_bAnimateInTimer = FALSE;
		}

		if (m_bShowTimer) {
			KillTimer(TIMER_ID_SHOW);
			m_bShowTimer = FALSE;
		}

		if (m_bAnimateOutTimer) {
			KillTimer(TIMER_ID_ANIMATEOUT);
			m_bAnimateOutTimer = FALSE;
		}
	} else
	{
		ReleaseCapture();

		if ((m_dwShowDelay > 0) && (!m_bShowTimer))
		{
			SetTimer(TIMER_ID_SHOW, m_dwShowDelay, NULL);
			m_bShowTimer = TRUE;
		}
	}
*/
	CWnd::OnMouseMove(nFlags, point);
}

void CBlendSelectWnd::SetIcon(HICON hIcon)
{
	m_hIcon = hIcon;
}

void CBlendSelectWnd::SetCloseIcon(HICON hNormal, HICON hDown)
{
	m_hIconClose = hNormal;
	m_hIconCloseDown = hDown;
}

void CBlendSelectWnd::SetRadioIcon(HICON hCheck, HICON hUncheck)
{
	m_hIconCheck = hCheck;
	m_hIconUnCheck = hUncheck;
}

void CBlendSelectWnd::SetTitle(CString strTitle)
{
	m_strTitle = strTitle;
}

int CBlendSelectWnd::AddItem(CString strItem)
{
	int nItem = m_wndListBox.AddItem(strItem, 1, NULL, -1);

	// Resize
	int nListBoxHeight = m_wndListBox.GetListHeight();
	m_dwHeight = m_dwMargin + m_dwTitleHeight + m_dwMargin + nListBoxHeight + m_dwMargin;

	CRgn CaptionRegion;
	CaptionRegion.CreateRoundRectRgn(0, 
		0, 
		m_dwWidth, 
		m_dwHeight, 
		10, 
		10);
	SetWindowRgn((HRGN)CaptionRegion.m_hObject, TRUE);

	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(NULL, 0, 0, m_dwWidth, m_dwHeight, SWP_NOMOVE | SWP_NOZORDER);

	// Set List Box Position
	int nBtnLeft = (m_hIcon == NULL) ? m_dwMargin : (m_dwMargin * 2 + 32);
	int nBtnWidth = (m_hIcon == NULL) ? (m_dwWidth - 2 * m_dwMargin) : (m_dwWidth - 3 * m_dwMargin - 32);
	int nButTop = 2 * m_dwMargin + m_dwTitleHeight;
	m_wndListBox.MoveWindow(nBtnLeft, nButTop, nBtnWidth, nListBoxHeight);

	Invalidate();

	return nItem;
}

BOOL CBlendSelectWnd::DelItem(int nItem)
{
	if (!m_wndListBox.DeleteItem(nItem))
		return FALSE;

	// Resize 
	int nListBoxHeight = m_wndListBox.GetListHeight();
	m_dwHeight = m_dwMargin + m_dwTitleHeight + m_dwMargin + nListBoxHeight + m_dwMargin;

	CRgn CaptionRegion;
	CaptionRegion.CreateRoundRectRgn(0, 
		0, 
		m_dwWidth, 
		m_dwHeight, 
		10, 
		10);
	SetWindowRgn((HRGN)CaptionRegion.m_hObject, TRUE);

	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(NULL, 0, 0, m_dwWidth, m_dwHeight, SWP_NOMOVE | SWP_NOZORDER);

	// Set List Box Position
	int nBtnLeft = (m_hIcon == NULL) ? m_dwMargin : (m_dwMargin * 2 + 32);
	int nBtnWidth = (m_hIcon == NULL) ? (m_dwWidth - 2 * m_dwMargin) : (m_dwWidth - 3 * m_dwMargin - 32);
	int nButTop = 2 * m_dwMargin + m_dwTitleHeight;
	m_wndListBox.MoveWindow(nBtnLeft, nButTop, nBtnWidth, nListBoxHeight);

	Invalidate();

	return TRUE;
}

void CBlendSelectWnd::SetTitleFont(CFont* pFont)
{
	if (pFont == NULL)
		return;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	pFont->GetLogFont(&lf);
	m_ftTitle.DeleteObject();
	m_ftTitle.CreateFontIndirect(&lf);
}

void CBlendSelectWnd::SetOptionFont(CFont* pFont)
{
	if (pFont == NULL)
		return;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	pFont->GetLogFont(&lf);
	m_ftOption.DeleteObject();
	m_ftOption.CreateFontIndirect(&lf);
}

void CBlendSelectWnd::SetColors(COLORREF crText, COLORREF crBkgnd)
{
	m_crText = crText;
	m_crBkgnd = crBkgnd;
}

void CBlendSelectWnd::SetAnimateDelay(DWORD dwTime)
{
	m_dwAnimateDelay = dwTime;
}

void CBlendSelectWnd::SetShowDelay(DWORD dwTime)
{
	m_dwShowDelay = dwTime;
}

int CBlendSelectWnd::CalculateTitleHeight()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	// Prepare DC
	CDC *pDC = GetDC();
	if (NULL == pDC)
		return 0;
	
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	if (m_ftTitle.GetSafeHandle()) {
		memDC.SelectObject(&m_ftTitle);
	}

	int nIconSize = m_hIcon ? 32 : 0;

	// Draw Title
	CRect rcTitle = rcClient;
	rcTitle.left += (nIconSize > 0) ? m_dwMargin * 2 + nIconSize : m_dwMargin;
	rcTitle.right -= m_dwMargin;
	rcTitle.top += m_dwMargin;
	rcTitle.bottom = rcTitle.top;

	memDC.DrawText(m_strTitle, m_strTitle.GetLength(), &rcTitle, DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);

	return rcTitle.Height();
}