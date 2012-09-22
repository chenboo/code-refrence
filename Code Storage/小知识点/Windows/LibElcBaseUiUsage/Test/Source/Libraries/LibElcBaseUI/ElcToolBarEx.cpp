// ElcToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcToolBarEx.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define WM_BUTTON_CLICKED		WM_APP + 500
#define WM_LABEL_CLICKED		WM_APP + 501


// CElcToolBarEx

IMPLEMENT_DYNAMIC(CElcToolBarEx, CElcBaseBar)

CElcToolBarEx::CElcToolBarEx()
{
	m_hCmdDrainWindow = NULL;
	m_nButtonLayout = ELCBL_HORIZONTAL;
}

CElcToolBarEx::~CElcToolBarEx()
{
}


BEGIN_MESSAGE_MAP(CElcToolBarEx, CElcBaseBar)
	ON_MESSAGE(WM_BUTTON_CLICKED, OnButtonClicked)
	ON_MESSAGE(WM_LABEL_CLICKED, OnLabelClicked)
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpDateCmdUI)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CElcToolBarEx message handlers

BOOL CElcToolBarEx::Create(CWnd* pParent, const RECT& rect, DWORD dwStyle, UINT nID)
{
	dwStyle |= WS_CLIPCHILDREN;
	return CElcBaseBar::Create(NULL, NULL, dwStyle, rect, pParent, nID);
}

BOOL CElcToolBarEx::CreateMainBar(CWnd* pParent, int nHeight, UINT nID)
{
	if (!pParent 
		|| nID < AFX_IDW_CONTROLBAR_FIRST
		|| nID > AFX_IDW_CONTROLBAR_LAST)
	{
		ASSERT(0);
		return FALSE;
	}

	CRect rect;
	pParent->GetClientRect(rect);
	rect.bottom = rect.top + nHeight;

	return CElcBaseBar::Create(NULL, 
		NULL, 
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 
		rect, 
		pParent, 
		nID);
}

LRESULT CElcToolBarEx::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
	if (!IsWindowVisible())
		return 0;

	CRect rect;
	GetClientRect(rect);

	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	rect.OffsetRect(0, lpLayout->rect.top);
	lpLayout->rect.top += rect.Height();

	if (lpLayout->hDWP) {
		rect.right = lpLayout->rect.right;
		MoveWindow(rect);
	}

	return 0;
}

void CElcToolBarEx::SetCmdDrainWindow(HWND hWnd)
{
	m_hCmdDrainWindow = hWnd;
}

int CElcToolBarEx::FindButton(UINT nID)
{
	CETBItemArray::iterator it = m_arrTBItem.begin();
	for (int i=0; it != m_arrTBItem.end(); it ++, i ++)
	{
		if (it->nID == nID)
			return i;
	}

	return -1;
}

CBarButtonEx * CElcToolBarEx::FindUnionButton(UINT nID)
{
	CETBItemArray::iterator it = m_arrTBUnionBtnItem.begin();
	for (int i=0; it != m_arrTBUnionBtnItem.end(); it ++, i ++) {
		CBarButtonEx * pButton = ((CElcUnionButton *)it->pItem)->GetButton(nID);
		if (pButton)
			return pButton;
	}

	return NULL;
}

BOOL CElcToolBarEx::AddButton(UINT nID, UINT nResId, LPCTSTR lpszType, LPCTSTR lpszText, int nDropType, UINT nAlign)
{
	if (!::IsWindow(m_hWnd)) {
		ASSERT(m_hWnd);
		return FALSE;
	}

	ELCTB_ITEM item;
	item.nID = nID;
	CBarButtonEx *pButton = new CBarButtonEx;
	item.pItem = pButton;

	if (item.pItem == NULL)
		return FALSE;

	do 
	{
		pButton->LoadSkin(m_skin.szButtonStyle, FALSE);
		if (!pButton->Create(lpszText, WS_CHILD, CRect(0,0,0,0), this, nID))
			break;

		pButton->SetLayout(m_nButtonLayout);
		pButton->SetIconImage(nResId, lpszType);
		pButton->SetNotifyWindow(GetSafeHwnd(), WM_BUTTON_CLICKED);
		pButton->SetBackgndColor(m_skin.crBackgnd);
		pButton->SetButtonDropType(nDropType);

		CSize size = pButton->GetIdealSize();
		/*CSize sizeBk = item.pItem->GetBackgndImageSize();
		size.cx = max(size.cx, sizeBk.cx);
		size.cy = max(size.cy, sizeBk.cy);*/

		if (!AddItem(pButton, size, nAlign))
			break;

		m_arrTBItem.push_back(item);

		return TRUE;

	} while(0);
	
	if (pButton)
		delete pButton;

	return FALSE;
}

BOOL CElcToolBarEx::AddButton(UINT nID, LPCTSTR lpszText, UINT nResId, LPCTSTR lpszType, int nDropType, UINT nAlign)
{
	return AddButton(nID, nResId, lpszType, lpszText, nDropType, nAlign);
}

BOOL CElcToolBarEx::AddFourStateButton(UINT nID, UINT nResId, LPCTSTR lpszType, LPCTSTR lpszText, int nDropType, UINT nAlign)
{
	if (!::IsWindow(m_hWnd)) {
		ASSERT(m_hWnd);
		return FALSE;
	}

	ELCTB_ITEM item;
	item.nID = nID;
	CBarButtonEx * pButton = new CBarButtonEx;
	item.pItem = pButton;

	if (item.pItem == NULL)
		return FALSE;

	do 
	{
		pButton->LoadSkin(m_skin.szButtonStyle, FALSE);
		if (!pButton->Create(lpszText, WS_CHILD, CRect(0,0,0,0), this, nID))
			break;

		pButton->SetLayout(m_nButtonLayout);
		pButton->SetHMargin(0);
		pButton->SetVMargin(0);
		pButton->SetAnimateDown(FALSE);
		pButton->SetIconImage(nResId, lpszType, ELCBIM_FOUR_STATE);
		pButton->SetBackgndImage(0);
		pButton->SetNotifyWindow(GetSafeHwnd(), WM_BUTTON_CLICKED);
		pButton->SetBackgndColor(m_skin.crBackgnd);
		pButton->SetButtonDropType(nDropType);

		CSize size = pButton->GetIdealSize();

		if (!AddItem(pButton, size, nAlign))
			break;

		m_arrTBItem.push_back(item);

		return TRUE;

	} while(0);
	
	if (pButton)
		delete pButton;

	return FALSE;
}

BOOL CElcToolBarEx::AddUnionButton(UINT nLeftId, 
		UINT nLeftResId,
		LPCTSTR lpszLeftText,
		UINT nRightId, 
		UINT nRightResId,
		LPCTSTR lpszRightText,
		UINT nAlign)
{
	ELC_UNION_BUTTON left, right;

	left.nId = nLeftId;
	left.nResId = nLeftResId;
	left.strType = RT_PNG;
	left.strCaption = lpszLeftText;
	left.strSkinStyle = m_skin.szUnionBtnStyle1;
	left.mode = ELCBIM_SINGLE;

	right.nId = nRightId;
	right.nResId = nRightResId;
	right.strType = RT_PNG;
	right.strCaption = lpszRightText;
	right.strSkinStyle = m_skin.szUnionBtnStyle2;
	right.mode = ELCBIM_SINGLE;

	CElcUnionButton * pUniconButton = new CElcUnionButton;
	pUniconButton->Create(&left, &right, WS_CHILD | WS_VISIBLE, this, 0);
	if (m_hCmdDrainWindow)
		pUniconButton->SetCmdDrainWindow(m_hCmdDrainWindow);
	else
		pUniconButton->SetCmdDrainWindow(GetParent()->GetSafeHwnd());

	if (!AddItem(pUniconButton, pUniconButton->GetIdeaSize(), nAlign)) {
		delete pUniconButton;
		return FALSE;
	}

	ELCTB_ITEM item;
	item.nID = 0;
	item.type = ELCIT_UNION_BUTTON;
	item.pItem = pUniconButton;

	m_arrTBUnionBtnItem.push_back(item);

	return TRUE;
}

BOOL CElcToolBarEx::DelButton(UINT nID)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	DelItem(m_arrTBItem[nItem].pItem);

	delete (CBarButtonEx *)m_arrTBItem[nItem].pItem;
	m_arrTBItem.erase(m_arrTBItem.begin() + nItem);

	return TRUE;
}

CBarButtonEx* CElcToolBarEx::GetButton(UINT nID)
{
	int nItem = FindButton(nID);
	if (nItem != -1) {
		return (CBarButtonEx *)m_arrTBItem[nItem].pItem;
	}

	return FindUnionButton(nID);
}

BOOL CElcToolBarEx::EnableButton(UINT nID, BOOL bEnable)
{
	int nItem = FindButton(nID);
	if (nItem != -1)
		m_arrTBItem[nItem].pItem->EnableWindow(bEnable);
	else {
		CBarButtonEx * pButton = FindUnionButton(nID);
		if (pButton)
			pButton->EnableWindow(bEnable);
	}
	

	return TRUE;
}

BOOL CElcToolBarEx::IsButtonEnable(UINT nID)
{
	int nItem = FindButton(nID);
	if (nItem != -1) {
		return m_arrTBItem[nItem].pItem->IsWindowEnabled();
	}
	else {
		CBarButtonEx * pButton = FindUnionButton(nID);
		if (pButton)
			return pButton->IsWindowEnabled();
	}

	return FALSE;
}

BOOL CElcToolBarEx::SetButtonTip(UINT nID, UINT nTipResId)
{
	CString strTip;
	strTip.LoadString(nTipResId);

	return SetButtonTip(nID, strTip);
}

BOOL CElcToolBarEx::SetButtonTip(UINT nID, LPCTSTR lpszText)
{
	if (!::IsWindow(m_wndToolTip.GetSafeHwnd()))
		EnableToolTip();

	int nItem = FindButton(nID);
	if (nItem != -1) {
		AddToolTip(m_arrTBItem[nItem].pItem, lpszText);
	}
	else {
		CBarButtonEx * pButton = FindUnionButton(nID);
		if (pButton)
			AddToolTip(pButton, lpszText);
	}

	return TRUE;
}

BOOL CElcToolBarEx::SetButtonText(UINT nID, UINT nResId)
{
	CString strText;
	strText.LoadString(nResId);

	return SetButtonText(nID, strText);
}

BOOL CElcToolBarEx::SetButtonText(UINT nID, LPCTSTR lpszText)
{
	int nItem = FindButton(nID);
	if (nItem != -1) {
		CBarButtonEx * pButton = (CBarButtonEx *)m_arrTBItem[nItem].pItem;
		pButton->SetText(lpszText);
		SetItemSize((UINT_PTR)pButton, pButton->GetIdealSize());
	}
	else {
		CBarButtonEx * pButton = FindUnionButton(nID);
		if (pButton) {
			pButton->SetText(lpszText);
			// TBD
			//SetItemSize((UINT_PTR)pButton, pButton->GetIdealSize());
		}
	}

	return TRUE;
}

BOOL CElcToolBarEx::SetButtonDropType(UINT nID, int nDropType)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	CBarButtonEx * pButton = (CBarButtonEx *)m_arrTBItem[nItem].pItem;

	pButton->SetButtonDropType(nDropType);

	SetItemSize((UINT_PTR)m_arrTBItem[nItem].pItem, pButton->GetIdealSize());

	return TRUE;
}

BOOL CElcToolBarEx::SetButtonsLayout(int layout)
{
	if (m_nButtonLayout == layout)
		return TRUE;

	m_nButtonLayout = layout;

	CETBItemArray::iterator it = m_arrTBItem.begin();
	for (int i=0; it != m_arrTBItem.end(); it ++, i ++)
	{
		if (it->type != ELCIT_LABEL)
		{
			((CBarButtonEx *)it->pItem)->SetLayout(m_nButtonLayout);
			SetItemSize((UINT_PTR)it->pItem, ((CBarButtonEx *)it->pItem)->GetIdealSize(), FALSE);
		}
	}

	SetCtrlsPos();

	Invalidate();

	return TRUE;
}

BOOL CElcToolBarEx::SetButtonChecked(UINT nID, BOOL bChecked)
{
	CBarButtonEx * pButton = NULL;
	int nItem = FindButton(nID);
	if (nItem != -1) {
		pButton = (CBarButtonEx *)m_arrTBItem[nItem].pItem;
	}
	else {
		pButton = FindUnionButton(nID);
	}

	if (pButton)
		pButton->SetCheck(bChecked);

	return TRUE;
}

BOOL CElcToolBarEx::IsButtonChecked(UINT nID)
{
	int nItem = FindButton(nID);
	if (nItem != -1) {
		return ((CBarButtonEx *)m_arrTBItem[nItem].pItem)->GetCheck();
	}
	else {
		CBarButtonEx * pButton = FindUnionButton(nID);
		if (pButton)
			pButton->GetCheck();
	}

	return FALSE;
}

BOOL CElcToolBarEx::SetButtonImage(UINT nID, UINT nResId, LPCTSTR lpszType, ELC_BUTTON_ICON_TYPE mode)
{
	CBarButtonEx * pButton = NULL;
	int nItem = FindButton(nID);
	if (nItem != -1) {
		pButton = (CBarButtonEx *)m_arrTBItem[nItem].pItem;
	}
	else {
		pButton = FindUnionButton(nID);
	}

	if (pButton) {
		pButton->SetIconImage(nResId, lpszType, mode);
		pButton->Invalidate();
	}

	return TRUE;
}

BOOL CElcToolBarEx::SetButtonVisible(UINT nID, BOOL bVisible)
{
	int nItem = FindButton(nID);
	if (nItem != -1)
		return ShowItem(m_arrTBItem[nItem].pItem, bVisible);
	else {
		// ...
		return FALSE;
	}
}

LRESULT CElcToolBarEx::OnButtonClicked(WPARAM wParam, LPARAM lParam)
{
	if (m_hCmdDrainWindow)
	{
		if (m_hCmdDrainWindow != GetSafeHwnd())
			::PostMessage(m_hCmdDrainWindow, WM_COMMAND, wParam, 0);
	}
	else if (GetParent())
	{
		::PostMessage(GetParent()->GetSafeHwnd(), WM_COMMAND, wParam, 0);
	}
	else
	{
		// the clicked command cannot send
		// oh year, error coming
		ASSERT(0);
	}

	return 0;
}

LRESULT CElcToolBarEx::OnLabelClicked(WPARAM wParam, LPARAM lParam)
{
	// do sth.
	// ...

	return 0;
}

BOOL CElcToolBarEx::AddLabel(UINT nID, LPCTSTR lpszText, UINT nAlign, BOOL bAdaptation, CSize size)
{
	ASSERT(m_hWnd);
	if (!::IsWindow(m_hWnd))
		return FALSE;

	if (FindButton(nID) != -1)
		return FALSE;

	ELCTB_ITEM item;
	item.nID = nID;
	item.type = ELCIT_LABEL;
	CBarButtonEx * pButton = new CBarButtonEx;
	item.pItem = pButton;

	if (item.pItem == NULL)
		return FALSE;

	do 
	{
		pButton->LoadSkin(m_skin.szButtonStyle, FALSE);
		if (!pButton->Create(lpszText, WS_CHILD, CRect(0,0,0,0), this, nID))
			break;

		pButton->SetBackgndImage(0, 0);
		pButton->SetNotifyWindow(GetSafeHwnd(), WM_LABEL_CLICKED);
		pButton->SetBackgndColor(m_skin.crBackgnd);
		pButton->SetAnimateDown(FALSE);
		item.dwData = bAdaptation;

		if (bAdaptation)
			size = pButton->GetIdealSize();

		if (!AddItem(pButton, size, nAlign))
			break;

		m_arrTBItem.push_back(item);

		return TRUE;

	} while(0);
	
	if (pButton)
		delete pButton;

	return FALSE;
}

BOOL CElcToolBarEx::AddLabel(UINT nID, LPCTSTR lpszText, COLORREF crText, UINT nAlign, BOOL bAdaptation, CSize size)
{
	ASSERT(m_hWnd);
	if (!::IsWindow(m_hWnd))
		return FALSE;

	if (FindButton(nID) != -1)
		return FALSE;

	ELCTB_ITEM item;
	item.nID = nID;
	item.type = ELCIT_LABEL;
	CBarButtonEx * pButton = new CBarButtonEx;
	item.pItem = pButton;

	if (item.pItem == NULL)
		return FALSE;

	do 
	{
		pButton->LoadSkin(m_skin.szButtonStyle, FALSE);
		if (!pButton->Create(lpszText, WS_CHILD, CRect(0,0,0,0), this, nID))
			break;

		//item.pItem->SetHMargin(4);
		//item.pItem->SetVMargin(2);
		pButton->SetBackgndImage(0, 0);
		pButton->SetNotifyWindow(GetSafeHwnd(), WM_LABEL_CLICKED);
		pButton->SetBackgndColor(m_skin.crBackgnd);
		pButton->SetTextColor(crText);
		pButton->SetAnimateDown(FALSE);
		item.dwData = bAdaptation;

		if (bAdaptation)
			size = pButton->GetIdealSize();

		if (!AddItem(pButton, size, nAlign))
			break;

		m_arrTBItem.push_back(item);

		return TRUE;

	} while(0);
	
	if (pButton)
		delete pButton;

	return FALSE;
}

BOOL CElcToolBarEx::SetLabelText(UINT nID, LPCTSTR lpszText)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	((CBarButtonEx *)m_arrTBItem[nItem].pItem)->SetText(lpszText);

	if (m_arrTBItem[nItem].dwData)
		SetItemSize((UINT_PTR)m_arrTBItem[nItem].pItem, ((CBarButtonEx *)m_arrTBItem[nItem].pItem)->GetIdealSize());

	return TRUE;
}

BOOL CElcToolBarEx::SetLabelColor(UINT nID, COLORREF crText)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	((CBarButtonEx *)m_arrTBItem[nItem].pItem)->SetTextColor(crText);

	return TRUE;
}

void CElcToolBarEx::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CElcBaseBar::OnShowWindow(bShow, nStatus);

	UINT nId = GetDlgCtrlID();
	if (AFX_IDW_CONTROLBAR_FIRST <= nId && nId <= AFX_IDW_CONTROLBAR_LAST) {
		CWnd* pParent = GetParent();
		if (pParent)
			pParent->PostMessage(WM_ELC_SKINFRAME_REPOSITIONBARS);
	}
}

LRESULT CElcToolBarEx::OnIdleUpDateCmdUI(WPARAM wParam, LPARAM lParam)
{
	CCmdTarget* pCmdTarget = GetParent();
	if (::IsWindow(m_hCmdDrainWindow))
		pCmdTarget = CWnd::FromHandle(m_hCmdDrainWindow);

	if (!pCmdTarget)
		return 0;

	CCmdUI state;
	
	for (CETBItemArray::iterator it = m_arrTBItem.begin();
		it != m_arrTBItem.end();
		it ++)
	{
		if (it->type == ELCIT_BUTTON) {
			state.m_nID = it->nID;
			state.m_pOther = it->pItem;
			//it->pItem->EnableWindow(TRUE);
			state.DoUpdate(pCmdTarget, FALSE);
		}
	}

	for (CETBItemArray::iterator it = m_arrTBUnionBtnItem.begin();
		it != m_arrTBUnionBtnItem.end();
		it ++)
	{
		if (it->type == ELCIT_UNION_BUTTON) {
			// ...
			//state.m_nID = it->nID;
			//state.m_pOther = it->pItem;
			//state.DoUpdate(pCmdTarget, FALSE);
		}
	}

	return 0;
}

void CElcToolBarEx::OnDestroy()
{
	__super::OnDestroy();

	CETBItemArray::iterator it = m_arrTBItem.begin();
	for (; it != m_arrTBItem.end(); it ++) {
		delete (CBarButtonEx *)it->pItem;
	}

	it = m_arrTBUnionBtnItem.begin();
	for (; it != m_arrTBUnionBtnItem.end(); it ++) {
		delete (CElcUnionButton *)it->pItem;
	}
}