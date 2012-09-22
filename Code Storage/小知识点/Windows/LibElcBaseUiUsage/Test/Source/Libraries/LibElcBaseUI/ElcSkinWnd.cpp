// ElcSkinWnd.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcSkinWnd.h"


// CElcSkinWnd

IMPLEMENT_DYNAMIC(CElcSkinWnd, CWnd)

CElcSkinWnd::CElcSkinWnd()
{
	LoadSkin(ELC_SKIN_STYLE_WINDOW_FRAME);
}

CElcSkinWnd::~CElcSkinWnd()
{
}


BEGIN_MESSAGE_MAP(CElcSkinWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCACTIVATE()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CElcSkinWnd message handlers

BOOL CElcSkinWnd::LoadSkin(LPCTSTR lpStyle)
{
	BOOL bRet = __super::LoadSkin(lpStyle);
	m_nMenuBar = 0;
	return bRet;
}

int CElcSkinWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!Initialize(GetSafeHwnd()))
		return -1;

	return 0;
}

void CElcSkinWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (m_bEnableSkin) {
		CRect rcWork;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
		lpMMI->ptMaxSize.y = rcWork.bottom;
	}

	__super::OnGetMinMaxInfo(lpMMI);
}

BOOL CElcSkinWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	return __super::PreCreateWindow(cs);
}

void CElcSkinWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (m_bEnableSkin)
		NcCalcSize(bCalcValidRects, lpncsp);
	__super::OnNcCalcSize(bCalcValidRects, lpncsp);
}

BOOL CElcSkinWnd::OnEraseBkgnd(CDC* pDC)
{
	if (m_bEnableSkin) {
		DrawClientBackground(pDC);
		return TRUE;
	}
	else {
		return __super::OnEraseBkgnd(pDC);
	}
}

void CElcSkinWnd::OnNcPaint()
{
	if (m_bEnableSkin)
		NcPaint();
	else 
		Default();
}

LRESULT CElcSkinWnd::OnNcHitTest(CPoint point)
{
	if (m_bEnableSkin)
		return NcHitTest(point);
	else
		return __super::OnNcHitTest(point);
}

void CElcSkinWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if (m_bEnableSkin && HTSYSMENU == nHitTest) {
		PostMessage(WM_SYSCOMMAND, SC_CLOSE);
	}
	else { 
		__super::OnNcLButtonDblClk(nHitTest, point);
	}
}

void CElcSkinWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (!m_bEnableSkin || !NcLButtonDown(nHitTest, point)) {
		__super::OnNcLButtonDown(nHitTest, point);
	}
}

void CElcSkinWnd::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	if (m_bEnableSkin)
		NcLButtonUp(nHitTest, point);
	else
		__super::OnNcLButtonUp(nHitTest, point);
}

void CElcSkinWnd::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (!m_bEnableSkin || !NcMouseMove(nHitTest, point)) {
		__super::OnNcMouseMove(nHitTest, point);
	}
}

void CElcSkinWnd::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	if (m_bEnableSkin) {
		if (!NcRButtonDown(nHitTest, point)) {
		}
	}
	else {
		__super::OnNcRButtonDown(nHitTest, point);
	}
}

BOOL CElcSkinWnd::OnNcActivate(BOOL bActive)
{
	if (m_bEnableSkin)
		return NcActivate(bActive);
	else
		return __super::OnNcActivate(bActive);
}

LRESULT CElcSkinWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_bEnableSkin)
		return __super::WindowProc(message, wParam, lParam);

	if (message == WM_SETTEXT) {
		RedrawCaption();
	}

	return __super::WindowProc(message, wParam, lParam);
}

void CElcSkinWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (ELC_SKIN_FRAME_STATE_TIMER == nIDEvent)
		__super::CheckStates();

	__super::OnTimer(nIDEvent);
}


