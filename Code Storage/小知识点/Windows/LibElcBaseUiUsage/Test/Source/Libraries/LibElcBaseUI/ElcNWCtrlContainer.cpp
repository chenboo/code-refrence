#include "StdAfx.h"
#include "LibElcBaseUI.h"
#include "ElcNWCtrlContainer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNWCtrlContainer::CNWCtrlContainer()
{
	m_hParent = NULL;
	m_hNotify = NULL;
}

CNWCtrlContainer::~CNWCtrlContainer()
{
	for (CNWItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (it->nType == ENWCT_BUTTON)
			delete (CElcNonWndButton *)it->pItem;
		else if (it->nType == ENWCT_LABEL)
			delete (CElcNonWndLabel *)it->pItem;
	}

	m_arrItem.clear();
}

void CNWCtrlContainer::Initialize(HWND hParent, HWND hNotify)
{
	m_hParent = hParent;
	m_hNotify = hNotify;
}

CSize CNWCtrlContainer::GetTextSize(LPCTSTR lpszText, CFont* pFont)
{
	if (!lpszText || _tcslen(lpszText) == 0)
		return CSize(0,0);

	HDC hdc = ::GetDC(m_hParent);

	HGDIOBJ hFont = NULL;
	if (pFont) {
		hFont = ::SelectObject(hdc, pFont->m_hObject);
	}
	else {
		hFont = ::SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
	}

	CSize size;
	::GetTextExtentExPoint(hdc, 
		lpszText,
		(int)_tcslen(lpszText),
		0,
		NULL,
		NULL,
		&size);

	if (hFont)
		::SelectObject(hdc, hFont);
	::ReleaseDC(m_hParent, hdc);

	return size;
}

PELC_NW_ITEM CNWCtrlContainer::FindItem(UINT nId)
{
	for (CNWItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (it->pItem->GetId() == nId)
			return &(*it);
	}

	return NULL;
}

void CNWCtrlContainer::AddButton(UINT nId, UINT nResId, LPCTSTR lpszType, LPCTSTR lpszText, CRect rect, int nCount, BOOL bVertical)
{
	CElcNonWndButton * pItem = new CElcNonWndButton;
	pItem->Create(lpszText, m_hParent, rect, nId);
	pItem->SetNotifyWindow(m_hNotify);
	pItem->SetIcon(nResId, lpszType, nCount, bVertical);

	ELC_NW_ITEM item;
	item.pItem = pItem;
	item.nType = ENWCT_BUTTON;

	m_arrItem.push_back(item);
}

void CNWCtrlContainer::AddButton(UINT nId, UINT nResId, LPCTSTR lpszType, LPCTSTR lpszText, CPoint point, CSize size, int nCount, BOOL bVertical)
{
	CRect rect(point, size);
	AddButton(nId, nResId, lpszType, lpszText, rect, nCount, bVertical);
}

void CNWCtrlContainer::AddButton(UINT nId,
								 PELC_SKIN_IMAGE pIcon, 
								 CPoint ptTopLeft,
								 HWND hNotifyWnd,
								 UINT nNotifyMsg)
{
	CRect rect(0,0,0,0);

	CElcNonWndButton * pItem = new CElcNonWndButton;
	pItem->Create(_T(""), m_hParent, rect, nId);
	pItem->SetNotifyWindow(hNotifyWnd, nNotifyMsg);
	pItem->SetIcon(pIcon);

	if (ptTopLeft.x < 0)
		rect.left = m_rect.right + ptTopLeft.x;
	else
		rect.left = ptTopLeft.x;
	if (ptTopLeft.y < 0)
		rect.top = m_rect.bottom + ptTopLeft.y;
	else
		rect.top = ptTopLeft.y;

	CSize size = pItem->GetIconSize();
	rect.right = rect.left + size.cx;
	rect.bottom = rect.top + size.cy;
	pItem->Move(rect, FALSE);

	ELC_NW_ITEM item;
	item.pItem = pItem;
	item.nType = ENWCT_BUTTON;
	item.ptTopLeft = ptTopLeft;

	m_arrItem.push_back(item);
}

void CNWCtrlContainer::AddButton(UINT nId, LPCTSTR lpBtnBkStyle, CPoint ptTopLeft, HWND hNotifyWnd, UINT nNotifyMsg)
{
	CRect rect(0,0,0,0);

	CElcNonWndButton * pItem = new CElcNonWndButton;
	pItem->Create(_T(""), m_hParent, rect, nId);
	pItem->SetNotifyWindow(hNotifyWnd, nNotifyMsg);
	pItem->LoadBackgroundSkin(lpBtnBkStyle);

	if (ptTopLeft.x < 0)
		rect.left = m_rect.right + ptTopLeft.x;
	else
		rect.left = ptTopLeft.x;
	if (ptTopLeft.y < 0)
		rect.top = m_rect.bottom + ptTopLeft.y;
	else
		rect.top = ptTopLeft.y;

	CSize size = pItem->GetBkImageSize();
	rect.right = rect.left + size.cx;
	rect.bottom = rect.top + size.cy;
	pItem->Move(rect, FALSE);

	ELC_NW_ITEM item;
	item.pItem = pItem;
	item.nType = ENWCT_BUTTON;
	item.ptTopLeft = ptTopLeft;

	m_arrItem.push_back(item);
}

void CNWCtrlContainer::AddTextButton(UINT nId, LPCTSTR lpszText, LPCTSTR lpBtnBkStyle, CPoint ptTopLeft)
{
	AddTextButton(nId, lpszText, lpBtnBkStyle, ptTopLeft, NULL, NULL);
}

void CNWCtrlContainer::AddTextButton(UINT nId, LPCTSTR lpszText, LPCTSTR lpBtnBkStyle, CPoint ptTopLeft, HWND hNotifyWnd, UINT nNotifyMsg)
{
	CRect rect(0,0,0,0);

	CElcNonWndButton * pItem = new CElcNonWndButton;
	pItem->Create(lpszText, m_hParent, rect, nId);
	pItem->SetNotifyWindow(hNotifyWnd, nNotifyMsg);
	pItem->LoadBackgroundSkin(lpBtnBkStyle);

	if (ptTopLeft.x < 0)
		rect.left = m_rect.right + ptTopLeft.x;
	else
		rect.left = ptTopLeft.x;
	if (ptTopLeft.y < 0)
		rect.top = m_rect.bottom + ptTopLeft.y;
	else
		rect.top = ptTopLeft.y;

	CSize size = pItem->GetIdealSize();
	rect.right = rect.left + size.cx;
	rect.bottom = rect.top + size.cy;
	pItem->Move(rect, FALSE);

	ELC_NW_ITEM item;
	item.pItem = pItem;
	item.nType = ENWCT_BUTTON;
	item.ptTopLeft = ptTopLeft;

	m_arrItem.push_back(item);
}

void CNWCtrlContainer::EnableButton(UINT nId, BOOL bEnable)
{
	PELC_NW_ITEM pItem = FindItem(nId);
	if (pItem && pItem->nType == ENWCT_BUTTON) {
		((CElcNonWndButton *)pItem->pItem)->Enable(bEnable);
	}
}

BOOL CNWCtrlContainer::IsButtonEnable(UINT nId)
{
	PELC_NW_ITEM pItem = FindItem(nId);
	if (pItem && pItem->nType == ENWCT_BUTTON) {
		return ((CElcNonWndButton *)pItem->pItem)->IsEnable();
	}

	return FALSE;
}

void CNWCtrlContainer::CheckButton(UINT nId, BOOL bChecked)
{
	PELC_NW_ITEM pItem = FindItem(nId);
	if (pItem && pItem->nType == ENWCT_BUTTON) {
		((CElcNonWndButton *)pItem->pItem)->SetCheck(bChecked);
	}
}

BOOL CNWCtrlContainer::ISButtonChecked(UINT nId)
{
	PELC_NW_ITEM pItem = FindItem(nId);
	if (pItem && pItem->nType == ENWCT_BUTTON) {
		return ((CElcNonWndButton *)pItem->pItem)->IsChecked();
	}

	return FALSE;
}

void CNWCtrlContainer::SetButtonImage(UINT nId, UINT nResId, LPCTSTR lpszType, int nCount, BOOL bVertical)
{
	PELC_NW_ITEM pItem = FindItem(nId);
	if (pItem && pItem->nType == ENWCT_BUTTON) {
		((CElcNonWndButton *)pItem->pItem)->SetIcon(nResId, lpszType, nCount, bVertical);
	}
}

void CNWCtrlContainer::AddLabel(UINT nId, LPCTSTR lpszText, CRect rect, int align, COLORREF crText)
{
	CElcNonWndLabel * pItem = new CElcNonWndLabel ;
	pItem->Create(lpszText, m_hParent, rect, nId);
	pItem->SetTextColor(crText);
	pItem->SetAlign(ENWLA_CENTER);

	ELC_NW_ITEM item;
	item.pItem = pItem;
	item.nType = ENWCT_LABEL;

	m_arrItem.push_back(item);
}

void CNWCtrlContainer::SetLabel(UINT nId, LPCTSTR lpszText)
{
	PELC_NW_ITEM pItem = FindItem(nId);
	if (pItem && pItem->nType == ENWCT_LABEL) {
		((CElcNonWndLabel *)pItem->pItem)->SetText(lpszText);
	}
}

void CNWCtrlContainer::MouseDown(CPoint point)
{
	for (CNWItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (it->pItem->MouseDown(point))
			break;
	}
}

void CNWCtrlContainer::MouseUp(CPoint point)
{
	for (CNWItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (it->pItem->MouseUp(point))
			break;
	}
}

void CNWCtrlContainer::MouseMove(CPoint point)
{
	for (CNWItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		it->pItem->MouseMove(point);
	}

	//::RedrawWindow(m_hParent, m_rect, NULL, RDW_INTERNALPAINT|RDW_INVALIDATE|RDW_FRAME);
}

void CNWCtrlContainer::CaptureChanged()
{
	for (CNWItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		it->pItem->CaptureChanged();
	}
}

void CNWCtrlContainer::Draw(PVOID pvGraphics)
{
	for (CNWItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		it->pItem->Draw(pvGraphics);
	}
}

void CNWCtrlContainer::Draw(HDC hDC, CRect rect)
{
	Bitmap bitmap(rect.Width(), rect.Height());
	Graphics grph(&bitmap);
	Draw(&grph);
	Graphics g(hDC);
	g.DrawImage(&bitmap, rect.left, rect.top);
}

void CNWCtrlContainer::Move(CRect rect)
{
	m_rect = rect;

	for (CNWItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (it->ptTopLeft.x >= 0 && it->ptTopLeft.y >= 0)
			continue;

		CRect rcItem;
		if (it->ptTopLeft.x < 0)
			rcItem.left = rect.right + it->ptTopLeft.x;
		else
			rcItem.left = it->ptTopLeft.x;
		if (it->ptTopLeft.y < 0)
			rcItem.top = rect.bottom + it->ptTopLeft.y;
		else
			rcItem.top = it->ptTopLeft.y;

		rcItem.right = rcItem.left + it->pItem->GetRect().Width();
		rcItem.bottom = rcItem.top + it->pItem->GetRect().Height();

		it->pItem->Move(rcItem, FALSE);
	}
}
