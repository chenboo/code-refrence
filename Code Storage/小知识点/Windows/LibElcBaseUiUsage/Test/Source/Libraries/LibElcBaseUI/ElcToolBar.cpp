// ElcToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "ElcToolBar.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define WM_BUTTON_CLICKED		WM_APP + 500


// CElcToolBar

IMPLEMENT_DYNAMIC(CElcToolBar, CElcBaseBar)

CElcToolBar::CElcToolBar()
{
	m_hCmdDrainWindow = NULL;
}

CElcToolBar::~CElcToolBar()
{
	CETBItemArray::iterator it = m_arrTBItem.begin();
	for (; it != m_arrTBItem.end(); it ++)
	{
		delete it->pItem;
	}
}


BEGIN_MESSAGE_MAP(CElcToolBar, CElcBaseBar)
	ON_MESSAGE(WM_BUTTON_CLICKED, OnButtonClicked)
END_MESSAGE_MAP()



// CElcToolBar message handlers

BOOL CElcToolBar::Create(CWnd* pParentWnd, const RECT& rect, DWORD dwStyle, UINT nID)
{
	return CElcBaseBar::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID);
}

void CElcToolBar::SetCmdDrainWindow(HWND hWnd)
{
	m_hCmdDrainWindow = hWnd;
}

int CElcToolBar::FindButton(UINT nID)
{
	CETBItemArray::iterator it = m_arrTBItem.begin();
	for (int i=0; it != m_arrTBItem.end(); it ++, i ++)
	{
		if (it->nID == nID)
			return i;
	}

	return -1;
}

BOOL CElcToolBar::AddButton(UINT nID, UINT nBitmap, UINT nAlign)
{
	ASSERT(m_hWnd);

	ELCTB_ITEM item;
	item.nID = nID;
	item.pItem = new CBarButton;

	if (item.pItem == NULL)
		return FALSE;

	do 
	{
		if (!item.pItem->Create(_T(""), WS_CHILD, CRect(0,0,0,0), this, nID))
			break;

		item.pItem->SetBitmap(nBitmap);
		CSize size = item.pItem->GetBitmapSize();
		item.pItem->SetNotifyWindow(GetSafeHwnd(), WM_BUTTON_CLICKED);
		item.pItem->SetBkColor(m_pcrGradientBk[0]);

		if (!AddItem(item.pItem, size, nAlign))
			break;

		m_arrTBItem.push_back(item);

		return TRUE;

	} while(0);
	
	if (item.pItem)
		delete item.pItem;

	return FALSE;
}

BOOL CElcToolBar::DelButton(UINT nID)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	DelItem(m_arrTBItem[nItem].pItem);

	delete m_arrTBItem[nItem].pItem;
	m_arrTBItem.erase(m_arrTBItem.begin() + nItem);

	return TRUE;
}

CBarButton* CElcToolBar::GetButton(UINT nID)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return NULL;

	return m_arrTBItem[nItem].pItem;
}

BOOL CElcToolBar::EnableButton(UINT nID, BOOL bEnable)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	m_arrTBItem[nItem].pItem->EnableWindow(bEnable);

	return TRUE;
}

BOOL CElcToolBar::IsButtonEnable(UINT nID)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	return m_arrTBItem[nItem].pItem->IsWindowEnabled();
}

BOOL CElcToolBar::SetButtonTip(UINT nID, UINT nTipResId)
{
	CString strTip;
	strTip.LoadString(nTipResId);

	return SetButtonTip(nID, strTip);
}

BOOL CElcToolBar::SetButtonTip(UINT nID, LPCTSTR lpszText)
{
	if (!::IsWindow(m_wndToolTip.GetSafeHwnd()))
		EnableToolTip();

	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	AddToolTip(m_arrTBItem[nItem].pItem, lpszText);

	return TRUE;
}

BOOL CElcToolBar::SetButtonChecked(UINT nID, BOOL bChecked)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	m_arrTBItem[nItem].pItem->SetCheck(bChecked);

	return TRUE;
}

BOOL CElcToolBar::IsButtonChecked(UINT nID)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	return m_arrTBItem[nItem].pItem->GetCheck();
}

BOOL CElcToolBar::SetButtonBitmap(UINT nID, UINT nBitmapResId)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	m_arrTBItem[nItem].pItem->SetBitmap(nBitmapResId);

	return TRUE;
}

BOOL CElcToolBar::SetButtonVisible(UINT nID, BOOL bVisible)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	return ShowItem(m_arrTBItem[nItem].pItem, bVisible);
}

LRESULT CElcToolBar::OnButtonClicked(WPARAM wParam, LPARAM lParam)
{
	if (m_hCmdDrainWindow)
	{
		::PostMessage(m_hCmdDrainWindow, WM_COMMAND, MAKEWPARAM((WORD)wParam, 0), 0);
	}
	else if (GetParent())
	{
		::PostMessage(GetParent()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM((WORD)wParam, 0), 0);
	}
	else
	{
		// the clicked command cannot send
		ASSERT(0);
	}

	return 0;
}

LRESULT CElcToolBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	/*
	if (WM_BUTTON_CLICKED == message)
	{
		if (m_hCmdDrainWindow)
		{
			::PostMessage(m_hCmdDrainWindow, WM_COMMAND, MAKEWPARAM((WORD)wParam, 0), 0);
		}
		else if (GetParent())
		{
			::PostMessage(GetParent()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM((WORD)wParam, 0), 0);
		}
		else
		{
			// the clicked command cannot send
			ASSERT(0);
		}

		return 0;
	}
	*/

	return CElcBaseBar::WindowProc(message, wParam, lParam);
}

BOOL CElcToolBar::AddLable(UINT nID, LPCTSTR lpszText, COLORREF crText, UINT nAlign)
{
	ASSERT(m_hWnd);

	if (FindButton(nID) != -1)
		return FALSE;

	ELCTB_ITEM item;
	item.nID = nID;
	item.pItem = new CBarButton;

	if (item.pItem == NULL)
		return FALSE;

	do 
	{
		if (!item.pItem->Create(lpszText, WS_CHILD, CRect(0,0,0,0), this, nID))
			break;

		CDC* pDC = GetDC();
		if (!pDC)
			break;

		CFont * pft = item.pItem->GetFont();
		LOGFONT lf = {0};
		pft->GetLogFont(&lf);
		CFont ft;
		ft.CreateFontIndirect(&lf);
		CFont * poldft = pDC->SelectObject(&ft);
		CSize size = pDC->GetTextExtent(lpszText);
		pDC->SelectObject(poldft);
		ReleaseDC(pDC);

		item.pItem->SetNotifyWindow(GetSafeHwnd(), WM_BUTTON_CLICKED);
		item.pItem->SetBkColor(m_pcrGradientBk[0]);
		item.pItem->SetFontColor(crText);

		if (!AddItem(item.pItem, size, nAlign))
			break;

		m_arrTBItem.push_back(item);

		return TRUE;

	} while(0);
	
	if (item.pItem)
		delete item.pItem;

	return FALSE;
}

BOOL CElcToolBar::SetLableText(UINT nID, LPCTSTR lpszText)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	m_arrTBItem[nItem].pItem->SetCaption(lpszText);

	return TRUE;
}

BOOL CElcToolBar::SetLableColor(UINT nID, COLORREF crText)
{
	int nItem = FindButton(nID);
	if (nItem == -1)
		return FALSE;

	m_arrTBItem[nItem].pItem->SetFontColor(crText);

	return TRUE;
}
