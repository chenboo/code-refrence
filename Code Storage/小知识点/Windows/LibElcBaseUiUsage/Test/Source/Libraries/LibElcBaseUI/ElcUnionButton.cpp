// ElcUnionButton.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcUnionButton.h"

#define WM_BUTTON_CLICKED		(WM_APP + 100)

// CElcUnionButton

IMPLEMENT_DYNAMIC(CElcUnionButton, CWnd)

CElcUnionButton::CElcUnionButton()
{
	m_sizeLeft.cx = m_sizeLeft.cy = 0;
	m_sizeRight.cx = m_sizeRight.cy = 0;
	m_layout = LAYOUT_HORIZONTAL;
	m_hNotifyWnd = 0;
}

CElcUnionButton::~CElcUnionButton()
{
}


BEGIN_MESSAGE_MAP(CElcUnionButton, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()

	ON_MESSAGE(WM_ELC_UI_DRAW_NOTIFY, OnElcUIDrawNotify)
	ON_MESSAGE(WM_BUTTON_CLICKED, OnButtonClicked)
END_MESSAGE_MAP()



// CElcUnionButton message handlers

BOOL CElcUnionButton::Create(PELC_UNION_BUTTON pLeftbutton, 
							 PELC_UNION_BUTTON pRightbutton,
							 DWORD dwStyle,
							 CWnd * pWndParent,
							 UINT nId)
{
	if (!CWnd::Create(NULL, NULL, dwStyle, CRect(0,0,0,0), pWndParent, nId))
		return FALSE;

	if (!CreateButton(pLeftbutton, &m_btnLeft, &m_sizeLeft)
		|| !CreateButton(pRightbutton, &m_btnRight, &m_sizeRight))
		return FALSE;

	return TRUE;
}

int CElcUnionButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bMirror = IsLayoutRTL();

	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CElcUnionButton::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (cx <=0 
		|| cy <= 0
		|| !m_btnLeft.GetSafeHwnd()
		|| !m_btnRight.GetSafeHwnd())
		return;

	Relayout();
}

void CElcUnionButton::Relayout()
{
	CRect rect, rcLeft, rcRight;
	GetClientRect(rect);

	rcLeft.left = 0;
	rcLeft.top = 0;

	if (LAYOUT_HORIZONTAL == m_layout) {
		rcLeft.right = m_sizeLeft.cx;
		rcLeft.bottom = rect.bottom;

		rcRight.left = rcLeft.right;
		rcRight.right = rect.right;
		rcRight.top = 0;
		rcRight.bottom = rect.bottom;
	}
	else {
		rcLeft.right = rect.right;
		rcLeft.bottom = m_sizeLeft.cy;

		rcRight.left = 0;
		rcRight.right = rect.right;
		rcRight.top = rcLeft.bottom;
		rcRight.bottom = rect.bottom;
	}

	/*
	if (m_bMirror) {
		rcLeft = MirrorRect(rect.Width(), rcLeft);
		rcRight = MirrorRect(rect.Width(), rcRight);
	}
	*/

	m_btnLeft.MoveWindow(rcLeft);
	m_btnRight.MoveWindow(rcRight);
}

BOOL CElcUnionButton::CreateButton(const PELC_UNION_BUTTON pButtonStruct, CBarButtonEx * pButton, CSize * pSize)
{
	if (::IsWindow(pButton->GetSafeHwnd()))
		pButton->DestroyWindow();

	if (!pButtonStruct->strSkinStyle.IsEmpty())
		pButton->LoadSkin(pButtonStruct->strSkinStyle);

	if (!pButton->Create(pButtonStruct->strCaption, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, pButtonStruct->nId))
		return FALSE;

	pButton->SetNotifyWindow(GetSafeHwnd(), WM_BUTTON_CLICKED);
	pButton->SetIconImage(pButtonStruct->nResId, pButtonStruct->strType, pButtonStruct->mode);
	*pSize = pButton->GetIdealSize();

	return TRUE;
}

BOOL CElcUnionButton::CreateButton(const PELC_UNION_BUTTON pButtonStruct, BOOL bLeft)
{
	CBarButtonEx * pButton = NULL;
	CSize * pSize = NULL;
	if (bLeft) {
		pButton = &m_btnLeft;
		pSize = &m_sizeLeft;
	}
	else {
		pButton = &m_btnRight;
		pSize = &m_sizeRight;
	}

	return CreateButton(pButtonStruct, pButton, pSize);
}

CBarButtonEx * CElcUnionButton::GetButton(UINT nId)
{
	if (m_btnLeft.GetDlgCtrlID() == nId)
		return &m_btnLeft;
	else if (m_btnRight.GetDlgCtrlID() == nId)
		return &m_btnRight;
	else
		return NULL;
}

CSize CElcUnionButton::GetIdeaSize()
{
	CSize size(0, 0);
	if (LAYOUT_HORIZONTAL == m_layout) {
		size.cx = m_sizeLeft.cx + m_sizeRight.cx;
		size.cy = max(m_sizeLeft.cy, m_sizeRight.cy);
	}
	else {
		size.cx = max(m_sizeLeft.cx, m_sizeRight.cx);
		size.cy = m_sizeLeft.cy + m_sizeRight.cy;
	}

	return size;
}

void CElcUnionButton::SetButtonSize(int nLeft)
{
	if (LAYOUT_HORIZONTAL == m_layout) {
		m_sizeLeft.cx = nLeft;
	}
	else {
		m_sizeLeft.cy = nLeft;
	}

	Relayout();
}

BOOL CElcUnionButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

LRESULT CElcUnionButton::OnElcUIDrawNotify(WPARAM wParam, LPARAM lParam)
{
	if (GetParent())
		return GetParent()->SendMessage(WM_ELC_UI_DRAW_NOTIFY, wParam, lParam);
	return 0;
}

LRESULT CElcUnionButton::OnButtonClicked(WPARAM wParam, LPARAM lParam)
{
	if (m_hNotifyWnd)
		::PostMessage(m_hNotifyWnd, WM_COMMAND, wParam, lParam);
	else if (GetParent())
		GetParent()->PostMessage(WM_COMMAND, wParam, lParam);
	return 0;
}
