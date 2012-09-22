// ElcSkinFrame.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcSkinFrame.h"


// CElcSkinFrame

IMPLEMENT_DYNCREATE(CElcSkinFrame, CFrameWnd)

CElcSkinFrame::CElcSkinFrame()
{
	LoadSkin(ELC_SKIN_STYLE_WINDOW_FRAME);
}

CElcSkinFrame::~CElcSkinFrame()
{
}


BEGIN_MESSAGE_MAP(CElcSkinFrame, CFrameWnd)
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
	ON_WM_ENTERIDLE()
	ON_WM_CANCELMODE()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_INITMENU()
	ON_WM_INITMENUPOPUP()

	ON_MESSAGE(WM_MENUBAR_NOTIFY, OnPopupMenuNotify)
	ON_MESSAGE(WM_ELC_SKINFRAME_REPOSITIONBARS, OnElcRepositionBars)
END_MESSAGE_MAP()


// CElcSkinFrame message handlers
int CElcSkinFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!Initialize(GetSafeHwnd()))
		return -1;

	return 0;
}

void CElcSkinFrame::OnDestroy()
{
	Uninitialize();
	__super::OnDestroy();
}

void CElcSkinFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (IsEnableSkin()) {
		CRect rcWork;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
		lpMMI->ptMaxSize.y = rcWork.bottom;
	}

	__super::OnGetMinMaxInfo(lpMMI);
}

void CElcSkinFrame::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (IsEnableSkin()) {
		if (cx > 0 && cy > 0) 
			SetWindowRegion();
	}
}

void CElcSkinFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (IsEnableSkin()) 
		NcCalcSize(bCalcValidRects, lpncsp);

	__super::OnNcCalcSize(bCalcValidRects, lpncsp);
}

BOOL CElcSkinFrame::OnEraseBkgnd(CDC* pDC)
{
	if (IsEnableSkin()) {
		DrawClientBackground(pDC);
		return TRUE;
	}
	else {
		return __super::OnEraseBkgnd(pDC);
	}
}

void CElcSkinFrame::OnNcPaint()
{
	if (IsEnableSkin())
		NcPaint();
	else
		Default();
}

LRESULT CElcSkinFrame::OnNcHitTest(CPoint point)
{
	if (IsEnableSkin())
		return NcHitTest(point);
	else
		return __super::OnNcHitTest(point);
}

void CElcSkinFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if (IsEnableSkin())
		NcLButtonDblClk(nHitTest, point);
	else
		__super::OnNcLButtonDblClk(nHitTest, point);
	
	/*
	if (HTSYSMENU == nHitTest) {
		PostMessage(WM_SYSCOMMAND, SC_CLOSE);
	}
	else { 
		CFrameWnd::OnNcLButtonDblClk(nHitTest, point);
	}
	*/
}

void CElcSkinFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (!IsEnableSkin()
		|| !NcLButtonDown(nHitTest, point)) 
		__super::OnNcLButtonDown(nHitTest, point);
}

void CElcSkinFrame::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	if (IsEnableSkin())
		NcLButtonUp(nHitTest, point);
	else
		__super::OnNcLButtonUp(nHitTest, point);
}

void CElcSkinFrame::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (!NcMouseMove(nHitTest, point)) {
		__super::OnNcMouseMove(nHitTest, point);
	}
}

void CElcSkinFrame::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	if (IsEnableSkin()) {
		if (!NcRButtonDown(nHitTest, point)) {
		}
	}
	else {
		__super::OnNcRButtonDown(nHitTest, point);
	}
}


BOOL CElcSkinFrame::OnNcActivate(BOOL bActive)
{
	if (IsEnableSkin()) 
		return TRUE;
	else
		return __super::OnNcActivate(bActive);
}

LRESULT CElcSkinFrame::OnPopupMenuNotify(WPARAM wParam, LPARAM lParam)
{
	PopupMenu((int)wParam);
	return 0;
}

void CElcSkinFrame::OnEnterIdle(UINT nWhy, CWnd* pWho)
{
	__super::OnEnterIdle(nWhy, pWho);

	if (IsEnableSkin()) 
		EnterIdle(nWhy, pWho);
}

void CElcSkinFrame::OnCancelMode()
{
	__super::OnCancelMode();

	if (IsEnableSkin() && m_nActiveItem == -1)
		m_bMenuActivated = FALSE;
}

LRESULT CElcSkinFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!IsEnableSkin())
		return __super::WindowProc(message, wParam, lParam);

	if (WM_SETTEXT == message) {
		RedrawCaption();
	}
	else if (WM_INITMENUPOPUP == message) {
		if ((WPARAM)::GetSystemMenu(m_hDerive, 0) == wParam) {
			lParam |= 0x00010000;
		}
		else if (::IsMenu(m_hMenu)) {
			int nCount = ::GetMenuItemCount(m_hMenu);
			for (int i=0; i < nCount; i++) {
				if ((WPARAM)::GetSubMenu(m_hMenu, i) == wParam) {
					lParam = MAKELONG(i, 0);
				}
			}
		}
	}
	else if (WM_ELC_SKINFRAME_REINITIALIZE == message) {
		Initialize(m_hWnd);
		return 0;
	}

	return __super::WindowProc(message, wParam, lParam);
}

BOOL CElcSkinFrame::PreTranslateMessage(MSG* pMsg) 
{
	return __super::PreTranslateMessage(pMsg);
}

void CElcSkinFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CElcSkinFrame::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CElcSkinFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	__super::OnSysCommand(nID, lParam);
}

void CElcSkinFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	__super::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

void CElcSkinFrame::OnInitMenu(CMenu* pMenu)
{
	__super::OnInitMenu(pMenu);
}

void CElcSkinFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (ELC_SKIN_FRAME_STATE_TIMER == nIDEvent)
		__super::CheckStates();

	__super::OnTimer(nIDEvent);
}

LRESULT CElcSkinFrame::OnElcRepositionBars(WPARAM wParam, LPARAM lParam)
{
	RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposDefault);
	return 0;
}

