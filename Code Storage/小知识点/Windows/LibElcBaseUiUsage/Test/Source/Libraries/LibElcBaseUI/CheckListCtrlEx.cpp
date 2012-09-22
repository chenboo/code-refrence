// CheckListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "CheckListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckListCtrlEx

CCheckListCtrlEx::CCheckListCtrlEx()
{
	m_bEnabled = TRUE;
}

CCheckListCtrlEx::~CCheckListCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CCheckListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CCheckListCtrlEx)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckListCtrlEx message handlers

int CCheckListCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CCheckListCtrlEx::EnableCheck(UINT uIdBitmap)
{
	if (m_imgListState.GetSafeHandle())
		return;

	m_imgListState.Create(uIdBitmap, 16, 1, RGB(255, 0, 255));
	SetImageList(&m_imgListState, LVSIL_STATE);
}

BOOL CCheckListCtrlEx::GetCheck(int nItem)
{
	UINT uState = GetItemState(nItem, LVIS_STATEIMAGEMASK);
	return (BOOL)(uState == INDEXTOSTATEIMAGEMASK(STATE_CHECK));
}

void CCheckListCtrlEx::SetCheck(int nItem, BOOL bChecked)
{
	int nIndex = bChecked ? STATE_CHECK : STATE_UNCHECK;
	if (!m_bEnabled)
		nIndex += 2;

	SetItemState(nItem,
		INDEXTOSTATEIMAGEMASK(nIndex), LVIS_STATEIMAGEMASK);
}

void CCheckListCtrlEx::SetDisable(int nItem, BOOL bDisable)
{
	int nIndex = bDisable ? STATE_DISABLE : STATE_UNCHECK;
	if (!m_bEnabled)
		nIndex += 2;

	SetItemState(nItem,
		INDEXTOSTATEIMAGEMASK(nIndex), LVIS_STATEIMAGEMASK);
}

BOOL CCheckListCtrlEx::IsDisable(int nItem)
{
	UINT uState = GetItemState(nItem, LVIS_STATEIMAGEMASK);
	return (BOOL)(uState == INDEXTOSTATEIMAGEMASK(STATE_DISABLE));
}

void CCheckListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_bEnabled) {
		CListCtrl::OnLButtonDown(nFlags, point);
		return;
	}

	UINT uFlags = 0;
	int nHitItem = HitTest(point, &uFlags);

	BOOL bHit = FALSE;
	if (uFlags == LVHT_ONITEM && (GetStyle() & LVS_OWNERDRAWFIXED)) {
		CRect rect;
		GetItemRect(nHitItem, rect, LVIR_ICON);

		if (point.x < rect.left)
			bHit = TRUE;
	}
	else if (uFlags & LVHT_ONITEMSTATEICON)
		bHit = TRUE;

	if (bHit && !IsDisable(nHitItem)) {
		SetCheck(nHitItem, !GetCheck(nHitItem));
		GetParent()->SendMessage(WM_COMMAND,  
			(WPARAM)MAKELONG(GetDlgCtrlID(), EN_CHANGE), 
			(LPARAM)GetSafeHwnd());
	}
	else
		CListCtrl::OnLButtonDown(nFlags, point);
}

void CCheckListCtrlEx::EnableSelect(BOOL bEnable)
{
	m_bEnabled = bEnable;
	for (int i = 0; i < GetItemCount(); i++) {
		SetCheck(i, GetCheck(i));
	}
}

void CCheckListCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (!m_bEnabled) {
		CListCtrl::OnLButtonDown(nFlags, point);
		return;
	}

	UINT uFlags = 0;
	int nHitItem = HitTest(point, &uFlags);

	BOOL bHit = FALSE;
	if (uFlags == LVHT_ONITEM && (GetStyle() & LVS_OWNERDRAWFIXED)) {
		CRect rect;
		GetItemRect(nHitItem, rect, LVIR_ICON);

		if (point.x < rect.left)
			bHit = TRUE;
	}
	else if (uFlags & LVHT_ONITEMSTATEICON)
		bHit = TRUE;

	if (bHit && !IsDisable(nHitItem)) {
		SetCheck(nHitItem, !GetCheck(nHitItem));
		GetParent()->SendMessage(WM_COMMAND, 
			(WPARAM)MAKELONG(GetDlgCtrlID(), EN_CHANGE), 
			(LPARAM)GetSafeHwnd());
	}

	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CCheckListCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar) {
	case VK_SPACE:
		if (!m_bEnabled)
			break;

		int nItem = GetNextItem(-1, LVIS_FOCUSED);
		if (-1 != nItem) {
			SetCheck(nItem, !GetCheck(nItem));

			GetParent()->SendMessage(WM_COMMAND,  
				(WPARAM)MAKELONG(GetDlgCtrlID(), EN_CHANGE), 
				(LPARAM)GetSafeHwnd());
		}
		return;
	}

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
