// ElcBaseBar.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcBaseBar.h"


#define ELCBAR_SEPARATOR			2
#define ELCBAR_SNAP_SIZE			6
#define ELCBAR_FOLD_INCREASE		2
#define ELCBAR_FOLD_REMAIN_HEIGHT	4

#define TIMER_ELAPSE_FOLD			10
#define TIMER_ELAPSE_TRACE_MOUSE	2000

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CElcBaseBar

IMPLEMENT_DYNAMIC(CElcBaseBar, CWnd)

CElcBaseBar::CElcBaseBar()
{
	m_barLayout = ELCBAR_LAYOUT_HORIZONTAL;

	m_nBarBorder = 0;

	m_nCtrlsSpace = 5;
	m_nBarMargin = 5;
	m_nBarWidth = 0;

	m_stretchItem.dwCtrl = 0;
	m_stretchItem.nType = 0;
	m_stretchItem.rect = CRect(0,0,0,0);

	m_hCursor = NULL;

	m_bUseGradienBk = FALSE;
	m_bPopupWindow = FALSE;

	m_bMirror = FALSE;
	m_bEnableDrag = FALSE;
	m_bLBtnDown = FALSE;
	m_bEnableAutoHide = FALSE;

	m_dwFoldState = FOLD_STATE_EXPAND;

	m_dwTimerId = ID_TIMER_NONE;

	LoadSkin(ELC_SKIN_STYLE_BUTTON_MAIN_TOOLBAR, FALSE);
}

CElcBaseBar::~CElcBaseBar()
{
}


BEGIN_MESSAGE_MAP(CElcBaseBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()

	ON_MESSAGE(WM_ELC_UI_DRAW_NOTIFY, OnElcUIDrawNotify)
END_MESSAGE_MAP()



// CElcBaseBar message handlers

BOOL CElcBaseBar::LoadSkin(LPCTSTR lpStyle, BOOL bRedraw)
{
	do {
		if (!theSkin.IsValid() || !lpStyle)
			break;

		PELC_SKIN_TOOLBAR pSkin = theSkin.GetToolbarSkin(lpStyle);
		if (!pSkin)
			break;

		memcpy(&m_skin, pSkin, sizeof(ELC_SKIN_TOOLBAR));

		if (bRedraw && ::IsWindow(m_hWnd)) {
			Invalidate();
		}

		return TRUE;

	} while (0);

	memset(&m_skin, 0, sizeof(m_skin));

	m_skin.crBackgnd = RGB(205,220,237);
	m_skin.crSeparator[0] = RGB(192,192,192);
	m_skin.crSeparator[1] = RGB(255,255,255);
	m_skin.crBorder = RGB(80,110,170);


	return FALSE;
}

void CElcBaseBar::SetBarLayout(ELCBAR_LAYOUT layout)
{
	m_barLayout = layout;

	if (::IsWindow(m_hWnd))
		SetCtrlsPos();
}

void CElcBaseBar::SetBarBorder(UINT nBorder)
{
	m_nBarBorder = nBorder;

	if (::IsWindow(m_hWnd))
		Invalidate();
}

void CElcBaseBar::SetBarBorderColor(COLORREF cr)
{
	m_skin.crBorder = cr;

	if (::IsWindow(m_hWnd))
		Invalidate();
}

COLORREF CElcBaseBar::GetBarBorderColor()
{
	return m_skin.crBorder;
}

void CElcBaseBar::SetBkColor(COLORREF cr)
{
	m_skin.crBackgnd = cr;
	if (::IsWindow(m_hWnd))
		Invalidate();
}

COLORREF CElcBaseBar::GetBkColor()
{
	return m_skin.crBackgnd;
}

void CElcBaseBar::SetBackgndImage(UINT nResId, LPCTSTR lpszType, int left, int top, int right, int bottom)
{
	if (m_skin.background.pImage) {
		delete m_skin.background.pImage;
		m_skin.background.pImage = NULL;
	}

	if (nResId > 0) {
		m_skin.background.pImage = ImageFromIDResource(nResId, lpszType);
		m_skin.background.nLeftSplit = left;
		m_skin.background.nRightSplit = right;
		m_skin.background.nTopSplit = top;
		m_skin.background.nBottomSplit = bottom;

		if (left > 0 && right > 0)
			m_skin.background.stretch |= EISR_HORIZONTAL;
		if (top > 0 && bottom > 0)
			m_skin.background.stretch |= EISR_VERTICAL;
	}
}

void CElcBaseBar::SetSeparatorColor(COLORREF crLeft, COLORREF crRight)
{
	m_skin.crSeparator[0] = crLeft;
	m_skin.crSeparator[1] = crRight;

	Invalidate();
}

void CElcBaseBar::GetSeparatorColor(COLORREF& crLeft, COLORREF& crRight)
{
	crLeft = m_skin.crSeparator[0];
	crRight = m_skin.crSeparator[1];
}

void CElcBaseBar::SetCtrlsSpace(int nSpace)
{
	ASSERT(nSpace >= 0);
	if (nSpace < 0)
		return;

	m_nCtrlsSpace = nSpace;

	if (::IsWindow(m_hWnd))
		SetCtrlsPos();
}

int CElcBaseBar::GetCtrlsSpace()
{
	return m_nCtrlsSpace;
}

void CElcBaseBar::SetBarMargin(int nWidth)
{
	ASSERT(nWidth >= 0);
	if (nWidth < 0)
		return;

	m_nBarMargin = nWidth;

	if (::IsWindow(m_hWnd))
		SetCtrlsPos();
}

int CElcBaseBar::GetBarMargin()
{
	return m_nBarMargin;
}

int CElcBaseBar::GetBarWidth()
{
	ASSERT(::IsWindow(m_hWnd));
	return m_nBarWidth;
}

BOOL CElcBaseBar::ShowItem(CWnd* pWnd, BOOL bShow)
{
	ASSERT(pWnd);
	if (pWnd == NULL)
		return FALSE;

	return ShowItem((UINT)pWnd, bShow);
}

BOOL CElcBaseBar::ShowItem(UINT nID, BOOL bShow)
{
	BOOL bFoundCtrl = TRUE;

	int nItem = FindItem((DWORD_PTR)nID, TRUE);
	if (nItem != -1)
	{
		m_arrLeftItem[nItem].bShow = bShow;
	}
	else
	{
		nItem = FindItem((DWORD_PTR)nID, FALSE);
		if (nItem != -1)
		{
			m_arrRightItem[nItem].bShow = bShow;
		}
		else if (m_stretchItem.dwCtrl == (DWORD_PTR)nID)
		{
			m_stretchItem.bShow = bShow;
		}
		else
		{
			bFoundCtrl = FALSE;
		}
	}

	if (bFoundCtrl)
	{
		SetCtrlsPos();
		Invalidate();
	}

	return bFoundCtrl;
}

BOOL CElcBaseBar::AddCtrl(UINT nType, DWORD_PTR dwCtrl, CRect rect, UINT nAlign, BOOL bShow)
{
	if (nType == ELCBAR_TYPE_CONTROL)
	{
		if (FindItem(dwCtrl, TRUE) != -1 || FindItem(dwCtrl, FALSE) != -1)
			return FALSE;

		if (nAlign == ELCBAR_ALIGN_STRETCH)
		{
			// only allow to add ONE stretch-control in ONE bar
			ASSERT(m_stretchItem.dwCtrl == NULL);

			m_stretchItem.dwCtrl = dwCtrl;
			m_stretchItem.nType = nType;
			m_stretchItem.rect = rect;
			m_stretchItem.bShow = bShow;

			SetCtrlsPos();

			return TRUE;
		}
	}
	else
	{
		// not allow ELCBAR_ALIGN_STRETCH for other controls
		ASSERT(nAlign != ELCBAR_ALIGN_STRETCH);
	}

	ELCBAR_ITEM item;
	item.bShow = bShow;
	item.nType = nType;
	item.dwCtrl = dwCtrl;
	item.rect = rect;

	if (nAlign == ELCBAR_ALIGN_LEFT)
		m_arrLeftItem.push_back(item);
	else if (nAlign == ELCBAR_ALIGN_RIGHT)
		m_arrRightItem.push_back(item);

	if (nType == ELCBAR_TYPE_CONTROL) {
		CWnd * pWnd = (CWnd *)dwCtrl;
		pWnd->SetParent(this);
	}

	SetCtrlsPos();

	return TRUE;
}

BOOL CElcBaseBar::DelCtrl(DWORD_PTR dwCtrl)
{
	BOOL bFound = FALSE;
	int nItem = FindItem(dwCtrl, TRUE);
	if (nItem >= 0)
	{
		m_arrLeftItem.erase(m_arrLeftItem.begin() + nItem);
		bFound = TRUE;
	}
	else
	{
		nItem = FindItem(dwCtrl, FALSE);
		if (nItem >= 0)
		{
			m_arrRightItem.erase(m_arrRightItem.begin() + nItem);
			bFound = TRUE;
		}
	}

	if (!bFound)
	{
		if (m_stretchItem.dwCtrl == dwCtrl)
		{
			m_stretchItem.dwCtrl = 0;
			m_stretchItem.nType = 0;
			m_stretchItem.rect = CRect(0,0,0,0);
			bFound = TRUE;
		}
	}

	if (bFound)
		SetCtrlsPos();

	return bFound;
}

BOOL CElcBaseBar::AddItem(CWnd* pWnd, CSize size, UINT nAlign, BOOL bShow)
{
	ASSERT(pWnd && pWnd->m_hWnd);

	if (pWnd == NULL || !::IsWindow(pWnd->GetSafeHwnd()))
		return FALSE;

	return AddCtrl(ELCBAR_TYPE_CONTROL, (DWORD_PTR)pWnd, CRect(0,0,size.cx, size.cy), nAlign, bShow);
}

void CElcBaseBar::DelItem(CWnd* pWnd)
{
	if (pWnd == NULL)
		return;

	pWnd->ShowWindow(SW_HIDE);

	DelCtrl((DWORD_PTR)pWnd);
}

void CElcBaseBar::AddSeparator(UINT nID, UINT nAlign, BOOL bShow)
{
	// not allow to add two adjacent separators
	if (nAlign == ELCBAR_ALIGN_LEFT)
	{
		if (!m_arrLeftItem.empty())
		{
			if (m_arrLeftItem[m_arrLeftItem.size()-1].nType == ELCBAR_TYPE_SEPARATOR)
				return;
		}
	}
	else
	{
		if (nAlign == ELCBAR_ALIGN_LEFT)
		{
			if (!m_arrRightItem.empty())
			{
				if (m_arrRightItem[m_arrRightItem.size()-1].nType == ELCBAR_TYPE_SEPARATOR)
					return;
			}
		}
	}

	AddCtrl(ELCBAR_TYPE_SEPARATOR, (DWORD_PTR)nID, CRect(0,0,ELCBAR_SEPARATOR,0), nAlign, bShow);
}

void CElcBaseBar::DelSeparator(UINT nID)
{
	DelCtrl((DWORD_PTR)nID);
}

void CElcBaseBar::AddSpace(UINT nID, UINT nAlign, int nSpace, BOOL bShow)
{
	AddCtrl(ELCBAR_TYPE_SPACE, (DWORD_PTR)nID, CRect(0,0,nSpace,0), nAlign, bShow);
}

void CElcBaseBar::DelSpace(UINT nID)
{
	DelCtrl((DWORD_PTR)nID);
}

void CElcBaseBar::SetSpace(UINT nID, int nSpace)
{
	int nItem = FindItem(nID, TRUE);
	if (nItem != -1)
	{
		(m_arrLeftItem.begin()+ nItem)->rect = CRect(0,0,nSpace,0);
	}
	else
	{
		nItem = FindItem(nID, FALSE);
		if (nItem != -1)
		{
			(m_arrRightItem.begin()+ nItem)->rect = CRect(0,0,nSpace,0);
		}
	}

	if (nItem != -1)
		SetCtrlsPos();
}

BOOL CElcBaseBar::SetItemSize(UINT_PTR nID, CSize size, BOOL bReCalc)
{
	int nItem = FindItem(nID, TRUE);
	if (nItem != -1) {
		m_arrLeftItem[nItem].rect.SetRect(0, 0, size.cx, size.cy);
	}
	else {
		nItem = FindItem(nID, FALSE);
		if (nItem != -1) {
			m_arrRightItem[nItem].rect.SetRect(0, 0, size.cx, size.cy);
		}
		else {
			// can not set the stretch item's size
			return FALSE;
		}
	}

	if (bReCalc)
		SetCtrlsPos();

	return TRUE;
}

CSize CElcBaseBar::GetItemSize(UINT_PTR nID)
{
	CSize size(0,0);
	int nItem = FindItem(nID, TRUE);
	if (nItem != -1) {
		size.cx = m_arrLeftItem[nItem].rect.Width();
		size.cy = m_arrLeftItem[nItem].rect.Height();
	}
	else {
		nItem = FindItem(nID, FALSE);
		if (nItem != -1) {
			size.cx = m_arrRightItem[nItem].rect.Width();
			size.cy = m_arrRightItem[nItem].rect.Height();
		}
		else {
			// do not get the stretch item's size
			ASSERT(0);
		}
	}

	return size;
}

inline int CElcBaseBar::FindItem(DWORD_PTR dwCtrl, BOOL bLeftArray)
{
	if (bLeftArray)
	{
		for (int i=0; i < (int)m_arrLeftItem.size(); i++)
		{
			if (m_arrLeftItem[i].dwCtrl == dwCtrl)
				return i;
		}
	}
	else
	{
		for (int i=0; i < (int)m_arrRightItem.size(); i++)
		{
			if (m_arrRightItem[i].dwCtrl == dwCtrl)
				return i;
		}
	}

	return -1;
}

BOOL CElcBaseBar::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CElcBaseBar::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0)
		return;

	if (m_dcBkMem.GetSafeHdc()) {
		m_dcBkMem.DeleteDC();
	}

	CDC * pDC = GetDC();
	CRect rect(0, 0, cx, cy);
	m_dcBkMem.CreateMe(pDC, rect);
	ReleaseDC(pDC);

	ShowBarDirect();
	SetCtrlsPos();
}

void CElcBaseBar::SetCtrlsPos()
{
	GetClientRect(&m_rcClient);

	if (m_rcClient.IsRectEmpty())
		return;

	if (m_barLayout == ELCBAR_LAYOUT_HORIZONTAL)
	{
		SetCtrlsHPos();
	}
	else if (m_barLayout == ELCBAR_LAYOUT_VERTICAL)
	{
		SetCtrlsVPos();
	}
	else
	{
		// undefined value
		ASSERT(0);
	}
}

void CElcBaseBar::SetCtrlsHPos()
{
	CRect rcStretchItem;
	CRect rcItem = m_rcClient;
	rcItem.left = m_nBarMargin;
	rcItem.right = m_nBarMargin;

	CItemArray::iterator it = m_arrLeftItem.begin();
	for (; it != m_arrLeftItem.end(); it ++)
	{
		if (!it->bShow)
		{
			if (it->nType == ELCBAR_TYPE_CONTROL)
			{
				((CWnd*)(it->dwCtrl))->ShowWindow(SW_HIDE);
			}
			continue;
		}

		BOOL bNeedMove = FALSE;
		if (it->nType == ELCBAR_TYPE_CONTROL)
		{
			if (rcItem.left != m_nBarMargin)
				rcItem.left += m_nCtrlsSpace;
			else
				bNeedMove = TRUE;

			rcItem.top = (m_rcClient.Height() - it->rect.Height()) / 2;
			rcItem.bottom = rcItem.top + it->rect.Height();
			rcItem.right = rcItem.left + it->rect.Width();

			if (it->rect != rcItem)
			{
				bNeedMove = TRUE;
				it->rect = rcItem;
			}

			rcItem.left = rcItem.right;
		}
		else if (it->nType == ELCBAR_TYPE_SEPARATOR)
		{
			if (rcItem.left != m_nBarMargin)
				rcItem.left += m_nCtrlsSpace;

			rcItem.right = rcItem.left + it->rect.Width();
			it->rect = rcItem;

			rcItem.left = rcItem.right;
		}
		else if (it->nType == ELCBAR_TYPE_SPACE)
		{
			rcItem.right = rcItem.left + it->rect.Width();
			it->rect = rcItem;
			rcItem.left = rcItem.right;
		}
		else
		{
			// undefined type
			ASSERT(0);
		}

		if (it->nType == ELCBAR_TYPE_CONTROL)
		{
			CWnd* pWnd = (CWnd*)it->dwCtrl;
			ASSERT(pWnd);

			if (bNeedMove)
				pWnd->MoveWindow(it->rect);
			if (!pWnd->IsWindowVisible())
				pWnd->ShowWindow(SW_SHOW);
		}
	}

	m_nBarWidth = rcItem.right;

	rcItem = m_rcClient;
	rcItem.right -= m_nBarMargin;
	rcItem.left = rcItem.right;
	for(int i=(int)m_arrRightItem.size()-1; i >= 0; i--)
	{
		CItemArray::iterator it = m_arrRightItem.begin() + i;

		if (!it->bShow)
		{
			if (it->nType == ELCBAR_TYPE_CONTROL)
			{
				((CWnd*)(it->dwCtrl))->ShowWindow(SW_HIDE);
			}
			continue;
		}

		BOOL bNeedMove = FALSE;
		if (it->nType == ELCBAR_TYPE_CONTROL)
		{
			if (rcItem.right != m_rcClient.right - m_nBarMargin)
				rcItem.right -= m_nCtrlsSpace;

			rcItem.left = rcItem.right - it->rect.Width();
			rcItem.top = (m_rcClient.Height() - it->rect.Height()) / 2;
			rcItem.bottom = rcItem.top + it->rect.Height();

			if (it->rect != rcItem)
			{
				bNeedMove = TRUE;
				it->rect = rcItem;
			}

			rcItem.right = rcItem.left;
		}
		else if (it->nType == ELCBAR_TYPE_SEPARATOR)
		{
			if (rcItem.right != m_rcClient.right - m_nBarMargin)
				rcItem.right -= m_nCtrlsSpace;

			rcItem.left = rcItem.right - it->rect.Width();
			it->rect = rcItem;

			rcItem.right = rcItem.left;
		}
		else if (it->nType == ELCBAR_TYPE_SPACE)
		{
			rcItem.left = rcItem.right - it->rect.Width();
			it->rect = rcItem;
			rcItem.right = rcItem.left;
		}
		else
		{
			// undefined type
			ASSERT(0);
		}

		if (it->nType == ELCBAR_TYPE_CONTROL)
		{
			CWnd* pWnd = (CWnd*)it->dwCtrl;
			ASSERT(pWnd);

			if (bNeedMove)
				pWnd->MoveWindow(it->rect);
			if (!pWnd->IsWindowVisible())
				pWnd->ShowWindow(SW_SHOW);
		}
	}

	if (!m_arrRightItem.empty()) {
		rcStretchItem.left = m_nBarWidth + m_nCtrlsSpace;
		rcStretchItem.right = rcItem.left;
		m_nBarWidth += m_rcClient.right - rcItem.left + m_nCtrlsSpace;
	}
	else {
		rcStretchItem.left = m_nBarWidth + m_nCtrlsSpace;
		rcStretchItem.right = rcItem.left - m_nCtrlsSpace;
		m_nBarWidth += m_nBarMargin;
	}

	if (m_stretchItem.dwCtrl != 0)
	{
		if (!m_stretchItem.bShow)
		{
			((CWnd*)m_stretchItem.dwCtrl)->ShowWindow(SW_HIDE);
		}
		else
		{
			rcItem.top = (m_rcClient.Height() - m_stretchItem.rect.Height()) / 2;
			rcItem.bottom = rcItem.top + m_stretchItem.rect.Height();
			rcItem.left = rcStretchItem.left;
			rcItem.right = rcStretchItem.right;

			m_stretchItem.rect = rcItem;

			CWnd* pWnd = (CWnd*)m_stretchItem.dwCtrl;
			pWnd->MoveWindow(m_stretchItem.rect);
			pWnd->ShowWindow(SW_SHOW);
		}
	}
}

void CElcBaseBar::SetCtrlsVPos()
{
	CRect rcItem = m_rcClient;
	rcItem.top = m_nBarMargin;

	CItemArray::iterator it = m_arrLeftItem.begin();
	for (; it != m_arrLeftItem.end(); it ++)
	{
		if (!it->bShow)
		{
			if (it->nType == ELCBAR_TYPE_CONTROL)
			{
				((CWnd*)(it->dwCtrl))->ShowWindow(SW_HIDE);
			}
			continue;
		}

		BOOL bNeedMove = FALSE;
		if (it->nType == ELCBAR_TYPE_CONTROL)
		{
			if (rcItem.top != m_nBarMargin) 
				rcItem.top += m_nCtrlsSpace;
			else
				bNeedMove = TRUE;

			rcItem.left = (m_rcClient.Width() - it->rect.Width()) / 2;
			rcItem.right = rcItem.left + it->rect.Width();
			rcItem.bottom = rcItem.top + it->rect.Height();

			if (it->rect != rcItem)
			{
				bNeedMove = TRUE;
				it->rect = rcItem;
			}

			rcItem.top = rcItem.bottom;
		}
		else if (it->nType == ELCBAR_TYPE_SEPARATOR)
		{
			if (rcItem.top != m_nBarMargin)
				rcItem.top += m_nCtrlsSpace;

			rcItem.bottom = rcItem.top + it->rect.Height();
			it->rect = rcItem;

			rcItem.top = rcItem.bottom;
		}
		else if (it->nType == ELCBAR_TYPE_SPACE)
		{
			rcItem.bottom = rcItem.top + it->rect.Height();
			it->rect = rcItem;
			rcItem.top = rcItem.bottom;
		}
		else
		{
			// undefined type
			ASSERT(0);
		}

		if (it->nType == ELCBAR_TYPE_CONTROL)
		{
			CWnd* pWnd = (CWnd*)it->dwCtrl;
			ASSERT(pWnd);

			if (bNeedMove)
				pWnd->MoveWindow(it->rect);
			if (!pWnd->IsWindowVisible())
				pWnd->ShowWindow(SW_SHOW);
		}
	}

	m_nBarWidth = rcItem.bottom;

	rcItem = m_rcClient;
	rcItem.bottom -= m_nBarMargin;
	for (int i=(int)m_arrRightItem.size()-1; i >= 0; i--)
	{
		CItemArray::iterator it = m_arrRightItem.begin() + i;

		if (!it->bShow)
		{
			if (it->nType == ELCBAR_TYPE_CONTROL)
			{
				((CWnd*)(it->dwCtrl))->ShowWindow(SW_HIDE);
			}
			continue;
		}

		if (it->nType == ELCBAR_TYPE_CONTROL)
		{
			if (rcItem.bottom != m_rcClient.bottom - m_nBarMargin)
				rcItem.bottom -= m_nCtrlsSpace;

			rcItem.top = rcItem.bottom - it->rect.Height();
			rcItem.left = (m_rcClient.Width() - it->rect.Width()) / 2;
			rcItem.right = rcItem.left + it->rect.Width();

			it->rect = rcItem;

			rcItem.bottom = rcItem.top;
		}
		else if (it->nType == ELCBAR_TYPE_SEPARATOR)
		{
			if (rcItem.bottom != m_rcClient.bottom - m_nBarMargin)
				rcItem.bottom -= m_nCtrlsSpace;

			rcItem.top = rcItem.bottom - it->rect.Height();
			it->rect = rcItem;

			rcItem.bottom = rcItem.top;
		}
		else if (it->nType == ELCBAR_TYPE_SPACE)
		{
			rcItem.top = rcItem.bottom - it->rect.Height();
			it->rect = rcItem;
			rcItem.bottom = rcItem.top;
		}
		else
		{
			// undefined type
			ASSERT(0);
		}

		if (it->nType == ELCBAR_TYPE_CONTROL)
		{
			CWnd* pWnd = (CWnd*)it->dwCtrl;
			ASSERT(pWnd);

			pWnd->MoveWindow(it->rect);
			pWnd->ShowWindow(SW_SHOW);
		}
	}

	if (!m_arrRightItem.empty())
		m_nBarWidth += m_rcClient.bottom - rcItem.top + m_nCtrlsSpace;
	else
		m_nBarWidth += m_nBarMargin;

	if (m_stretchItem.dwCtrl != 0)
	{
		if (!m_stretchItem.bShow)
		{
			((CWnd*)m_stretchItem.dwCtrl)->ShowWindow(SW_HIDE);
		}
		else
		{
			rcItem.left = (m_rcClient.Width() - m_stretchItem.rect.Width()) / 2;
			rcItem.right = rcItem.left + m_stretchItem.rect.Width();

			if (m_arrLeftItem.empty())
			{
				rcItem.top = m_nBarMargin;
			}
			else
			{
				rcItem.top = (m_arrLeftItem.end()-1)->rect.bottom + m_nCtrlsSpace;
			}

			if (m_arrRightItem.empty())
			{
				rcItem.bottom = m_rcClient.bottom - m_nBarMargin;
			}
			else
			{
				rcItem.bottom = m_arrRightItem.begin()->rect.top - m_nBarMargin;
			}

			m_stretchItem.rect = rcItem;

			CWnd* pWnd = (CWnd*)m_stretchItem.dwCtrl;
			pWnd->MoveWindow(m_stretchItem.rect);
			pWnd->ShowWindow(SW_SHOW);
		}
	}
}

void CElcBaseBar::OnPaint()
{
	CPaintDC dc(this);

	DrawBackground(&m_dcBkMem);
	DrawSeparator(&m_dcBkMem);
	DrawBorder(&m_dcBkMem);

	dc.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), &m_dcBkMem, 0, 0, SRCCOPY);
	//m_dcBkMem.DrawMe();
}

void CElcBaseBar::DrawBackground(CDC* pDC)
{
	if (m_skin.background.pImage) {
		thePainter.DrawImageGDI(pDC->GetSafeHdc(),
			&m_skin.background,
			m_rcClient,
			0,
			0,
			1,
			0,
			0);
	}
	else {
		pDC->FillSolidRect(m_rcClient, m_skin.crBackgnd);
	}
}

LRESULT CElcBaseBar::OnElcUIDrawNotify(WPARAM wParam, LPARAM lParam)
{
	PELC_UI_DRAW pDraw = (PELC_UI_DRAW)lParam;
	if (pDraw 
		&& pDraw->type == ELC_UI_DT_PREDRAW
		&& m_dcBkMem.GetSafeHdc()) 
	{
		POINT ptLT;
		ptLT.x = pDraw->rect.left;
		ptLT.y = pDraw->rect.top;
		::ClientToScreen(pDraw->hWnd, &ptLT);
		::ScreenToClient(m_hWnd, &ptLT);
		::BitBlt(pDraw->hdc, 
			pDraw->rect.left, 
			pDraw->rect.top, 
			pDraw->rect.right - pDraw->rect.left,
			pDraw->rect.bottom - pDraw->rect.top,
			m_dcBkMem.GetSafeHdc(),
			ptLT.x,
			ptLT.y,
			SRCCOPY);

		return 1;
	}
	return 0;
}

void CElcBaseBar::DrawGradienRect(CDC* pDC, CPoint ptStart, CPoint ptEnd, COLORREF crStart, COLORREF crEnd)
{
	TRIVERTEX vert[2];
	GRADIENT_RECT gRect;
	vert[0].x = ptStart.x;
	vert[0].y = ptStart.y;
	vert[0].Red = ((int)GetRValue(crStart)) << 8;
	vert[0].Green = ((int)GetGValue(crStart)) << 8;
	vert[0].Blue = ((int)GetBValue(crStart)) << 8;
	vert[0].Alpha = 0x0000;

	vert[1].x = ptEnd.x;
	vert[1].y = ptEnd.y;
	vert[1].Red = ((int)GetRValue(crEnd)) << 8;
	vert[1].Green = ((int)GetGValue(crEnd)) << 8;
	vert[1].Blue = ((int)GetBValue(crEnd)) << 8;
	vert[1].Alpha = 0xFF00;

	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;

	GradientFill(pDC->GetSafeHdc(), vert, 2, &gRect, 1, 
		m_barLayout == ELCBAR_LAYOUT_HORIZONTAL ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
}

void CElcBaseBar::DrawSeparator(CDC* pDC)
{
	CItemArray::iterator it = m_arrLeftItem.begin();
	for (; it != m_arrLeftItem.end(); it ++)
	{
		if (it->nType == ELCBAR_TYPE_SEPARATOR && it->bShow)
		{
			DrawSeparator(pDC, it->rect);
		}
	}

	it = m_arrRightItem.begin();
	for (; it != m_arrRightItem.end(); it ++)
	{
		if (it->nType == ELCBAR_TYPE_SEPARATOR && it->bShow)
		{
			DrawSeparator(pDC, it->rect);
		}
	}
}

void CElcBaseBar::DrawSeparator(CDC* pDC, CRect rect)
{
	CPen pen, *pOldPen=NULL;

	pen.CreatePen(PS_SOLID, 1, m_skin.crSeparator[0]);
	pOldPen = pDC->SelectObject(&pen);

	if (m_barLayout == ELCBAR_LAYOUT_HORIZONTAL)
	{
		pDC->MoveTo(rect.left, rect.top);
		pDC->LineTo(rect.left, rect.bottom-1);
	}
	else
	{
		pDC->MoveTo(rect.left, rect.top);
		pDC->LineTo(rect.right-1, rect.top);
	}

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	pen.CreatePen(PS_SOLID, 1, m_skin.crSeparator[1]);
	pDC->SelectObject(&pen);

	if (m_barLayout == ELCBAR_LAYOUT_HORIZONTAL)
	{
		pDC->MoveTo(rect.left+1, rect.top+1);
		pDC->LineTo(rect.left+1, rect.bottom);
	}
	else
	{
		pDC->MoveTo(rect.left+1, rect.bottom);
		pDC->LineTo(rect.right, rect.bottom);
	}

	pDC->SelectObject(pOldPen);
}

void CElcBaseBar::DrawBorder(CDC* pDC)
{
	if (m_nBarBorder == 0)
		return;

	CPen pen, *pOldPen=NULL;
	pen.CreatePen(PS_SOLID, 1, m_skin.crBorder);
	pOldPen = pDC->SelectObject(&pen);

	if (m_nBarBorder & ELCBAR_BORDER_TOP)
	{
		pDC->MoveTo(0, 0);
		pDC->LineTo(m_rcClient.right, 0);
	}
	if (m_nBarBorder & ELCBAR_BORDER_BOTTOM)
	{
		pDC->MoveTo(0, m_rcClient.bottom-1);
		pDC->LineTo(m_rcClient.right, m_rcClient.bottom-1);
	}
	if (m_nBarBorder & ELCBAR_BORDER_LEFT)
	{
		pDC->MoveTo(0, 0);
		pDC->LineTo(0, m_rcClient.bottom);
	}
	if (m_nBarBorder & ELCBAR_BORDER_RIGHT)
	{
		pDC->MoveTo(m_rcClient.right-1, 0);
		pDC->LineTo(m_rcClient.right-1, m_rcClient.bottom);
	}

	pDC->SelectObject(pOldPen);
}

BOOL CElcBaseBar::PreTranslateMessage(MSG* pMsg)
{
	if (m_wndToolTip.GetSafeHwnd())
		m_wndToolTip.RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}

void CElcBaseBar::EnableToolTip()
{
	if (!::IsWindow(m_wndToolTip.GetSafeHwnd()))
	{
		m_wndToolTip.Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX);
		m_wndToolTip.Activate(TRUE);
		m_wndToolTip.SetDelayTime(500);
	}
}

void CElcBaseBar::AddToolTip(CWnd* pWnd, UINT nIDText)
{
	// Call EnableToolTip() first
	ASSERT(::IsWindow(m_wndToolTip.GetSafeHwnd()));

	if (m_wndToolTip.GetSafeHwnd())
	{
		m_wndToolTip.AddTool(pWnd, nIDText);
	}
}

void CElcBaseBar::AddToolTip(CWnd* pWnd, LPCTSTR lpszText)
{
	// Call EnableToolTip() first
	ASSERT(::IsWindow(m_wndToolTip.GetSafeHwnd()));

	if (m_wndToolTip.GetSafeHwnd())
	{
		m_wndToolTip.AddTool(pWnd, lpszText);
	}
}

void CElcBaseBar::DelToolTip(CWnd* pWnd)
{
	ASSERT(::IsWindow(m_wndToolTip.GetSafeHwnd()));

	if (m_wndToolTip.GetSafeHwnd())
	{
		m_wndToolTip.DelTool(pWnd);
	}
}


int CElcBaseBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwLayout;
	GetProcessDefaultLayout(&dwLayout);
	m_bMirror = dwLayout == LAYOUT_RTL ? TRUE : FALSE;

	m_bPopupWindow = ((GetStyle() & WS_CHILD) != 0) ? FALSE : TRUE;

	m_hCursor = LoadCursor(NULL, IDC_ARROW);

	return 0;
}

BOOL CElcBaseBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_hCursor) {
		SetCursor(m_hCursor);
		return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CElcBaseBar::SetEnableDrag(BOOL bEnable)
{
	m_bEnableDrag = bEnable;
}

BOOL CElcBaseBar::IsEnableDrag()
{
	return m_bEnableDrag;
}

void CElcBaseBar::SetBarAutoHide(BOOL bEnable)
{
	m_bEnableAutoHide = bEnable;

	if (m_bEnableAutoHide)
	{
		StartBarTimer(ID_TIMER_TRACE_MOUSE);
	}
	else 
	{
		StopBarTimer(ID_TIMER_TRACE_MOUSE);
	}
}

BOOL CElcBaseBar::IsBarAutoHide()
{
	return m_bEnableAutoHide;
}

void CElcBaseBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bEnableDrag)
	{
		m_ptOldPoint = point;
		m_ptDown = point;
		ClientToScreen(&m_ptOldPoint);
		m_bLBtnDown = TRUE;
		SetCapture();
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CElcBaseBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bLBtnDown) 
		return;

	ReleaseCapture();
	m_bLBtnDown =FALSE;

	CWnd::OnLButtonUp(nFlags, point);
}

void CElcBaseBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLBtnDown)
	{
		DragMoving(point);
	}
	else
	{
		if (m_bEnableAutoHide)
		{
			if (m_dwFoldState != FOLD_STATE_EXPAND ||
				m_dwFoldState != FOLD_STATE_EXPANDING)
			{
				FoldBar(FALSE);
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CElcBaseBar::DragMoving(CPoint point)
{
	CWnd* pParent = NULL;

	if (!m_bPopupWindow)
	{
		pParent = GetParent();
		ASSERT(pParent);

		if (!pParent)
			return;
	}

	int nMaxCx = 0;
	int nMaxCy = 0;

	if (m_bPopupWindow)
	{
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(MonitorFromWindow(GetParent()->m_hWnd, MONITOR_DEFAULTTONEAREST), &mi);

		CRect rcDesktop;
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);

		nMaxCx = rcDesktop.Width() + mi.rcMonitor.left;
		nMaxCy = rcDesktop.Height() + mi.rcMonitor.top;
	}
	else
	{
		CRect rc;
		pParent->GetClientRect(rc);

		nMaxCx = rc.Width();
		nMaxCy = rc.Height();
	}

	ClientToScreen(&point);
	int cx = point.x - m_ptOldPoint.x;
	int cy = point.y - m_ptOldPoint.y;

	CRect rect;
	GetWindowRect(&rect);

	if (!m_bPopupWindow)
		pParent->ScreenToClient(&rect);

	int nWidth = rect.Width();
	int nHeight = rect.Height();

	rect.left += cx;
	rect.top += cy;

	if (rect.left < 0)
		rect.left = 0;
	if (rect.top < 0)
		rect.top = 0;
	if (rect.right + cx > nMaxCx)
		rect.left = nMaxCx - nWidth;
	if (rect.bottom + cy > nMaxCy)
		rect.top = nMaxCy - nHeight;	

	SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE|SWP_NOACTIVATE);

	GetWindowRect(&rect);

	if (m_bMirror) 
		m_ptOldPoint.x = rect.left + rect.Width() - m_ptDown.x;
	else
		m_ptOldPoint.x = rect.left + m_ptDown.x;

	m_ptOldPoint.y = rect.top + m_ptDown.y;
}

void CElcBaseBar::OnCaptureChanged(CWnd *pWnd)
{
	m_bLBtnDown = FALSE;

	CWnd::OnCaptureChanged(pWnd);
}

void CElcBaseBar::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanging(lpwndpos);

	if (m_dwFoldState == FOLD_STATE_EXPANDING ||
		m_dwFoldState == FOLD_STATE_FOLDING)
	{
		return;
	}

	CRect rcWork;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);

	if (abs(lpwndpos->x) <= (rcWork.left + ELCBAR_SNAP_SIZE))
	{
		lpwndpos->x = rcWork.left;
	}
	else if ((lpwndpos->x + lpwndpos->cx) >= (rcWork.right - ELCBAR_SNAP_SIZE) &&
		(lpwndpos->x + lpwndpos->cx) <= (rcWork.right + ELCBAR_SNAP_SIZE))
	{
		lpwndpos->x = rcWork.right - lpwndpos->cx;
	}

	if (abs( lpwndpos->y) <= (rcWork.top + ELCBAR_SNAP_SIZE ))
	{
		lpwndpos->y = rcWork.top;
	}
	else if ((lpwndpos->y + lpwndpos->cy) >= (rcWork.bottom - ELCBAR_SNAP_SIZE) &&
		(lpwndpos->y + lpwndpos->cy) <= (rcWork.bottom + ELCBAR_SNAP_SIZE))
	{
		lpwndpos->y = rcWork.bottom-lpwndpos->cy;
	}
}

void CElcBaseBar::StartBarTimer(UINT_PTR nIDEvent)
{
	UINT nElapse = 0;

	switch (nIDEvent)
	{
	case ID_TIMER_FOLD:
		nElapse = TIMER_ELAPSE_FOLD;
		break;
	case ID_TIMER_TRACE_MOUSE:
		nElapse = TIMER_ELAPSE_TRACE_MOUSE;
		break;
	default:
		ASSERT(0);
		return;
	}

	SetTimer(nIDEvent, nElapse, NULL);
	m_dwTimerId |= nIDEvent;
}

void CElcBaseBar::StopBarTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);
	m_dwTimerId &= ~nIDEvent;
}

void CElcBaseBar::FoldBar(BOOL bFold)
{
	m_bFoldBar = bFold;
	if (m_bFoldBar)
	{
		if (m_dwFoldState != FOLD_STATE_EXPAND)
			return;

		m_dwFoldState = FOLD_STATE_FOLDING;
		m_nFactHeight = 0;	
	}
	else
	{
		if (m_dwFoldState == FOLD_STATE_EXPAND ||
			m_dwFoldState != FOLD_STATE_FOLDED)
			return;

		m_nFactHeight = -(m_rcClient.Height() - ELCBAR_FOLD_REMAIN_HEIGHT);
	}

	StartBarTimer(ID_TIMER_FOLD);
}

void CElcBaseBar::ShowBarDirect()
{
	if (m_dwFoldState == FOLD_STATE_EXPAND)
		return;

	CRect rc;
	GetWindowRect(rc);
	if (!m_bPopupWindow)
		GetParent()->ScreenToClient(rc);

	StopBarTimer(ID_TIMER_FOLD);
	m_dwFoldState = FOLD_STATE_EXPAND;
	SetWindowPos(NULL, rc.left, 0, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
}

BOOL CElcBaseBar::IsBarDocked()
{
	CRect rc;
	GetWindowRect(rc);

	if (!m_bPopupWindow)
		GetParent()->ScreenToClient(rc);

	return (rc.top == 0);
}

void CElcBaseBar::OnTimer(UINT_PTR nIDEvent)
{
	if (ID_TIMER_FOLD == nIDEvent)
	{
		CRect rc;
		GetWindowRect(rc);

		if (!m_bPopupWindow)
			GetParent()->ScreenToClient(rc);

		if (m_bFoldBar)
		{
			m_nFactHeight -= ELCBAR_FOLD_INCREASE;

			if (rc.Height() - abs(m_nFactHeight) <= ELCBAR_FOLD_REMAIN_HEIGHT)
			{				
				m_dwFoldState = FOLD_STATE_FOLDED;
				StopBarTimer(ID_TIMER_FOLD);
			}
			else
			{
				m_dwFoldState = FOLD_STATE_FOLDING;
			}
		}
		else
		{
			m_nFactHeight += ELCBAR_FOLD_INCREASE;

			if (m_nFactHeight >= 0)
			{
				m_nFactHeight = 0;
				StopBarTimer(ID_TIMER_FOLD);
				m_dwFoldState = FOLD_STATE_EXPAND;
			}
			else
			{
				m_dwFoldState = FOLD_STATE_EXPANDING;
			}
		}

		SetWindowPos(NULL, rc.left, m_nFactHeight, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
	else if (ID_TIMER_TRACE_MOUSE == nIDEvent)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		if (!m_rcClient.PtInRect(pt))
		{		
			if (m_dwFoldState == FOLD_STATE_EXPAND)
			{
				if (IsBarDocked())
					FoldBar(TRUE);
			}
		}
	}

	CWnd::OnTimer(nIDEvent);
}

void CElcBaseBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		ShowBarDirect();
	}

	CWnd::OnShowWindow(bShow, nStatus);
}

void CElcBaseBar::OnDestroy()
{
	CWnd::OnDestroy();

	m_arrLeftItem.clear();
	m_arrRightItem.clear();
}
