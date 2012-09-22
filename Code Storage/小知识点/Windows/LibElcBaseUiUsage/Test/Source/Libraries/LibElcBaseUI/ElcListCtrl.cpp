#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CElcListCtrl, CListCtrl)

CElcListCtrl::CElcListCtrl()
{
	memset(&m_skin, 0, sizeof(m_skin));
	m_bSkinedHeaderCtrl = FALSE;
}

CElcListCtrl::~CElcListCtrl()
{
}

BEGIN_MESSAGE_MAP(CElcListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeyDownNotify)
END_MESSAGE_MAP()

BOOL CElcListCtrl::LoadSkin(LPCTSTR lpszStyle)
{
	PELC_SKIN_LIST pSkin = theSkin.GetListSkin(lpszStyle);
	if (!pSkin)
		return FALSE;

	memcpy(&m_skin, pSkin, sizeof(m_skin));
	m_SkinHeaderCtrl.LoadSkin(m_skin.szHeaderCtrlStyle);

	return TRUE;
}

void CElcListCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();
	
	if (!m_bSkinedHeaderCtrl) {
		SetSkinHeader(TRUE);
	}
}

int CElcListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (_tcslen(m_skin.szStyle) <= 0
		&& !LoadSkin(ELC_SKIN_STYLE_LIST_MAIN))
		return -1;

	if (!m_bSkinedHeaderCtrl) {
		SetSkinHeader(TRUE);
	}

	return 0;
}

void CElcListCtrl::OnNcPaint()
{
	Default();

	LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	if (lStyle & WS_BORDER) {
		CDC* pDC = GetWindowDC();
		if (pDC) {
			CRect rect;
			GetWindowRect(rect);
			rect.OffsetRect(-rect.left, -rect.top);
			CBrush br(m_skin.crBorder);
			pDC->FrameRect(rect, &br);
			ReleaseDC(pDC);
		}
	}
}

BOOL CElcListCtrl::SetSkinHeader(BOOL bEnable) 
{
	CHeaderCtrl* pHeaderCtrl = CListCtrl::GetHeaderCtrl();
	if (!pHeaderCtrl
		|| !::IsWindow(pHeaderCtrl->m_hWnd))
		return FALSE;

	if (bEnable) {
		if (!m_bSkinedHeaderCtrl
			&& m_SkinHeaderCtrl.SubclassWindow(pHeaderCtrl->m_hWnd)) 
		{
			m_bSkinedHeaderCtrl = TRUE;
			m_SkinHeaderCtrl.LoadSkin(ELC_SKIN_STYLE_HEADERCTRL_MAIN);
		}
	}
	else if (m_bSkinedHeaderCtrl) {
		m_SkinHeaderCtrl.UnsubclassWindow();
		m_bSkinedHeaderCtrl = FALSE;
	}
	return m_bSkinedHeaderCtrl;
}

void CElcListCtrl::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (m_SkinHeaderCtrl.GetSafeHwnd())
		m_SkinHeaderCtrl.Invalidate();
}

void CElcListCtrl::OnKeyDownNotify(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	*pResult = 0;

	if (pLVKeyDow->wVKey == VK_F2) {
		int nItem = GetNextItem(-1, LVNI_FOCUSED);
		if (nItem != -1) {
			EditLabel(nItem);
		}
	}
}
