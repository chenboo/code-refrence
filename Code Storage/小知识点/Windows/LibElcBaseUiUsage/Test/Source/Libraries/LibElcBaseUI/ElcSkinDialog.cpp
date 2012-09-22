// ElcSkinDialog.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcSkinDialog.h"

#define MENU_ITEM_HMARGIN	6
#define MENU_ITEM_VMARGIN	3
#define FRAME_ICON_MARGIN	4
#define FRAME_ICON_SIZE		16
#define RESIZE_BRODER		4


// CElcSkinDialog dialog

IMPLEMENT_DYNAMIC(CElcSkinDialog, CDialog)

CElcSkinDialog::CElcSkinDialog(CWnd* pParent /*=NULL*/)
: CDialog(CElcSkinDialog::IDD, pParent)
{
	LoadSkin(ELC_SKIN_STYLE_WINDOW_DLG);
}

CElcSkinDialog::CElcSkinDialog(UINT nIDTemplate, CWnd* pParent)
: CDialog(nIDTemplate, pParent)
{
	LoadSkin(ELC_SKIN_STYLE_WINDOW_DLG);
}

CElcSkinDialog::~CElcSkinDialog()
{
}

void CElcSkinDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

LRESULT CElcSkinDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_SETTEXT == message) {
		RedrawCaption();
	}
	else if (WM_ELC_SKINFRAME_REINITIALIZE == message) {
		Initialize(m_hWnd);
		return 0;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


BEGIN_MESSAGE_MAP(CElcSkinDialog, CDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_NCACTIVATE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_NCPAINT()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CElcSkinDialog message handlers


BOOL CElcSkinDialog::LoadSkin(LPCTSTR lpStyle)
{
	if (!CElcSkinFrameBase::LoadSkin(lpStyle))
		return FALSE;

	CElcSkinFrameBase::EnableDrawClientBackgnd(TRUE);

	m_nMenuBar = 0;

	return TRUE;
}

int CElcSkinDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!Initialize(GetSafeHwnd()))
		return -1;

	return 0;
}

void CElcSkinDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (m_bEnableSkin) {
		CRect rcWork;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);

		lpMMI->ptMaxSize.y = rcWork.bottom;
	}

	__super::OnGetMinMaxInfo(lpMMI);
}

BOOL CElcSkinDialog::OnNcActivate(BOOL bActive)
{
	return __super::NcActivate(bActive);
}

void CElcSkinDialog::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (m_bEnableSkin)
		NcCalcSize(bCalcValidRects, lpncsp);
	__super::OnNcCalcSize(bCalcValidRects, lpncsp);
}

LRESULT CElcSkinDialog::OnNcHitTest(CPoint point)
{
	if (m_bEnableSkin)
		return NcHitTest(point);
	else
		return __super::OnNcHitTest(point);
}

void CElcSkinDialog::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if (m_bEnableSkin && HTSYSMENU == nHitTest) {
		PostMessage(WM_SYSCOMMAND, SC_CLOSE);
	}
	else { 
		__super::OnNcLButtonDblClk(nHitTest, point);
	}
}

void CElcSkinDialog::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (!m_bEnableSkin || !NcLButtonDown(nHitTest, point)) {
		__super::OnNcLButtonDown(nHitTest, point);
	}
}

void CElcSkinDialog::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	if (m_bEnableSkin)
		NcLButtonUp(nHitTest, point);
	else
		__super::OnNcLButtonUp(nHitTest, point);
}

void CElcSkinDialog::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (m_bEnableSkin)
		NcMouseMove(nHitTest, point);
	__super::OnNcMouseMove(nHitTest, point);
}

void CElcSkinDialog::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	if (m_bEnableSkin)
		NcRButtonDown(nHitTest, point);
	else
		__super::OnNcRButtonDown(nHitTest, point);
}

void CElcSkinDialog::OnNcPaint()
{
	if (m_bEnableSkin)
		NcPaint();
	else
		Default();
}

BOOL CElcSkinDialog::OnEraseBkgnd(CDC* pDC)
{
	if (m_bEnableSkin) {
		DrawClientBackground(pDC);
		return TRUE;
	}
	else {
		return __super::OnEraseBkgnd(pDC);
	}
}

HBRUSH CElcSkinDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (!m_bEnableSkin)
		return __super::OnCtlColor(pDC, pWnd, nCtlColor);

	HBRUSH hbr = CtlColor(pDC, pWnd, nCtlColor);
	if (hbr)
		return hbr;
	else
		return __super::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CElcSkinDialog::OnDestroy()
{
	UnBindCtrls();
	__super::OnDestroy();
}

void CElcSkinDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (ELC_SKIN_FRAME_STATE_TIMER == nIDEvent)
		__super::CheckStates();

	__super::OnTimer(nIDEvent);
}
