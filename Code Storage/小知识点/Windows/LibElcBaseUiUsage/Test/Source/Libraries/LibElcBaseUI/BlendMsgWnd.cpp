#include "StdAfx.h"
#include "LibElcBaseUI.h"
#include "BlendMsgWnd.h"

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

IMPLEMENT_DYNAMIC(CBlendMsgWnd, CWnd)

CBlendMsgWnd::CBlendMsgWnd()
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
}

CBlendMsgWnd::~CBlendMsgWnd(void)
{
}

BEGIN_MESSAGE_MAP(CBlendMsgWnd, CWnd)
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

BOOL CBlendMsgWnd::Create(CWnd* pParentWnd)
{
	//if hwnd already exist - return TRUE;
	if (GetSafeHwnd())
		return TRUE;

	DWORD dwStyleEx = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED ;
	CRect rect(0, 0, m_dwWidth, m_dwHeight);

	DWORD dwLayout;
	GetProcessDefaultLayout(&dwLayout);
	if (LAYOUT_RTL & dwLayout) {
		dwStyleEx |= WS_EX_LAYOUTRTL;
	}

	LPCTSTR pszClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW);

	if (!CreateEx(dwStyleEx, pszClassName, NULL, WS_VISIBLE | WS_POPUP, rect, pParentWnd, 0))
		return FALSE;

	SetOwner(pParentWnd);

	return TRUE;
}

BOOL CBlendMsgWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

int CBlendMsgWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	int nTitleHeight = CalculateTitleHeight();
	int nDesHeight = CalculateDesHeight();

	m_dwHeight = nTitleHeight + nDesHeight + 3 * m_dwMargin;

	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(NULL, (cxScreen - m_dwWidth) / 2, (cyScreen - m_dwHeight) / 2, 
		m_dwWidth, m_dwHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

	CRgn CaptionRegion;
	CaptionRegion.CreateRoundRectRgn(0, 
		0, 
		m_dwWidth, 
		m_dwHeight, 
		20, 
		20);
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

void CBlendMsgWnd::OnClose()
{
	CWnd::OnClose();

	delete this;
}

void CBlendMsgWnd::OnTimer(UINT_PTR nIDEvent)
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

void CBlendMsgWnd::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient(0,0,m_dwWidth,m_dwHeight);

	CFont* pOldFont;

	CDC dcMem;
	if (!dcMem.CreateCompatibleDC(&dc))
		return;

	CBitmap bm;
	bm.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	dcMem.SelectObject(&bm);

	dcMem.FillSolidRect(&rcClient, m_crBkgnd);

	dcMem.SetTextColor(m_crText);
	dcMem.SetBkColor(m_crBkgnd);

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
	rcTitle.bottom = rcTitle.top;

	if (m_ftTitle.GetSafeHandle()) {
		pOldFont = dcMem.SelectObject(&m_ftTitle);
	}

	dcMem.DrawText(m_strTitle, m_strTitle.GetLength(), &rcTitle, DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
	dcMem.DrawText(m_strTitle, m_strTitle.GetLength(), &rcTitle, DT_WORDBREAK | DT_NOPREFIX);

	if (m_ftTitle.GetSafeHandle()) {
		dcMem.SelectObject(pOldFont);
	}

	// Draw Desc
	CRect rcDesc = rcClient;
	rcDesc.left += (nIconSize > 0) ? m_dwMargin * 2 + nIconSize : m_dwMargin;
	rcDesc.right -= m_dwMargin;
	rcDesc.top = rcTitle.bottom + m_dwMargin;
	rcDesc.bottom = rcDesc.top;

	if (m_ftDesc.GetSafeHandle()) {
		pOldFont = dcMem.SelectObject(&m_ftDesc);
	}

	dcMem.DrawText(m_strDesc, m_strDesc.GetLength(), &rcDesc, DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
	dcMem.DrawText(m_strDesc, m_strDesc.GetLength(), &rcDesc, DT_WORDBREAK | DT_NOPREFIX);

	if (m_ftDesc.GetSafeHandle()) {
		dcMem.SelectObject(pOldFont);
	}

	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(),
		&dcMem, 0, 0, SRCCOPY);
}

void CBlendMsgWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_CLOSE, 0, 0);
	CWnd::OnLButtonDown(nFlags, point);
}

void CBlendMsgWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
}

void CBlendMsgWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
}

void CBlendMsgWnd::SetIcon(HICON hIcon)
{
	m_hIcon = hIcon;
}

void CBlendMsgWnd::SetTitle(CString strTitle)
{
	m_strTitle = strTitle;
}

void CBlendMsgWnd::SetDecs(CString strDecs)
{
	m_strDesc = strDecs;
}

void CBlendMsgWnd::SetTitleFont(CFont* pFont)
{
	if (pFont == NULL)
		return;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	pFont->GetLogFont(&lf);
	m_ftTitle.DeleteObject();
	m_ftTitle.CreateFontIndirect(&lf);
}

void CBlendMsgWnd::SetDescFont(CFont* pFont)
{
	if (pFont == NULL)
		return;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	pFont->GetLogFont(&lf);
	m_ftDesc.DeleteObject();
	m_ftDesc.CreateFontIndirect(&lf);
}

void CBlendMsgWnd::SetColors(COLORREF crText, COLORREF crBkgnd)
{
	m_crText = crText;
	m_crBkgnd = crBkgnd;
}

void CBlendMsgWnd::SetAnimateDelay(DWORD dwTime)
{
	m_dwAnimateDelay = dwTime;
}

void CBlendMsgWnd::SetShowDelay(DWORD dwTime)
{
	m_dwShowDelay = dwTime;
}

int CBlendMsgWnd::CalculateTitleHeight()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	CFont* pOldFont;

	int nIconSize = 0;
	if (m_hIcon) {
		nIconSize = 32;
	}

	// Prepare DC
	CDC *pDC = GetDC();
	if (NULL == pDC)
		return 0;
	
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	if (m_ftTitle.GetSafeHandle()) {
		pOldFont = memDC.SelectObject(&m_ftTitle);
	}

	CRect rcTitle = rcClient;
	rcTitle.left += (nIconSize > 0) ? m_dwMargin * 2 + nIconSize : m_dwMargin;
	rcTitle.right -= m_dwMargin;
	rcTitle.bottom = rcTitle.top;

	memDC.DrawText(m_strTitle, m_strTitle.GetLength(), &rcTitle, DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);

	if (m_ftTitle.GetSafeHandle()) {
		memDC.SelectObject(pOldFont);
	}

	return rcTitle.Height();
}

int CBlendMsgWnd::CalculateDesHeight()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	CFont* pOldFont;

	int nIconSize = 0;
	if (m_hIcon) {
		nIconSize = 32;
	}

	// Prepare DC
	CDC *pDC = GetDC();
	if (NULL == pDC)
		return 0;
	
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	if (m_ftDesc.GetSafeHandle()) {
		pOldFont = memDC.SelectObject(&m_ftDesc);
	}

	CRect rcDesc = rcClient;
	rcDesc.left += (nIconSize > 0) ? m_dwMargin * 2 + nIconSize : m_dwMargin;
	rcDesc.right -= m_dwMargin;
	rcDesc.bottom = rcDesc.top;

	memDC.DrawText(m_strDesc, m_strDesc.GetLength(), &rcDesc, DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);

	if (m_ftDesc.GetSafeHandle()) {
		memDC.SelectObject(pOldFont);
	}

	return rcDesc.Height();
}