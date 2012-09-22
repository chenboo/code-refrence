// SkinRichMenu.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "SkinRichMenu.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



#define HMARGIN 5
#define VMARGIN 5
#define DEFAULT_WEIGHT	400


// CSkinRichMenu

IMPLEMENT_DYNAMIC(CSkinRichMenu, CWnd)

CSkinRichMenu::CSkinRichMenu()
{
	m_hNotify = NULL;
	m_nMsg = NULL;

	m_nItemWidth = DEFAULT_WEIGHT;

	m_cItemNum = 0;
	m_nHoverItem = -1;

	m_nCapHeight = 0;
	m_nDescHeight = 0;

	m_cDescRow = 2;

	m_clrBackground = RGB(255,255,255);
	m_clrText = RGB(0,21,110);
	m_clrDisable = RGB(166,166,166);

	m_bDrawMemItem = FALSE;

	m_brFrame.CreateSolidBrush(RGB(134,134,134));
	m_brItem.CreateSolidBrush(RGB(219,206,153));
	m_brCheck.CreateSolidBrush(RGB(242,149,54));
}

CSkinRichMenu::~CSkinRichMenu()
{
}


BEGIN_MESSAGE_MAP(CSkinRichMenu, CWnd)
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CSkinRichMenu message handlers

BOOL CSkinRichMenu::Create(const RECT& rect, CWnd* pParentWnd)
{
	CString strClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW);

	return CWnd::CreateEx(WS_EX_TOOLWINDOW | WS_EX_STATICEDGE, 
		strClassName, 
		_T(""),
		WS_POPUP, 
		rect.left,
		rect.top,
		rect.right,
		rect.bottom, 
		pParentWnd->GetSafeHwnd(),
		0,
		NULL);
}

POINT CSkinRichMenu::GetAlignPos(int x, int y, int nFlag)
{
	int w = m_rcClient.Width();
	int h = m_rcClient.Height();

	POINT pt;

	DWORD dwLayout = 0;
	GetProcessDefaultLayout(&dwLayout);
	if (dwLayout == LAYOUT_RTL) {
		switch (nFlag)
		{
			case LEFT_TOP: 
				nFlag = RIGHT_TOP;
				break;
			case RIGHT_TOP:
				nFlag = LEFT_TOP;
				break;
			case LEFT_BOTTOM:
				nFlag = RIGHT_BOTTOM;
				break;
			case RIGHT_BOTTOM:
				nFlag = LEFT_BOTTOM;
				break;
			default:
				break;
		}
	}

	switch (nFlag)
	{
	case LEFT_TOP: 
		pt.x = x;
		pt.y = y;
		break;
	case RIGHT_TOP:
		pt.x = x - w;
		pt.y = y;
		break;
	case LEFT_BOTTOM:
		pt.x = x;
		pt.y = y - h;
		break;
	case RIGHT_BOTTOM:
		pt.x = x - w;
		pt.y = y - h;
		break;
	case TOP_CENTER:
		pt.x = x - w / 2;
		pt.y = y;
		break;
	case BOTTOM_CENTER:
		pt.x = x- w / 2;
		pt.y = y - h;
		break;
	default:
		pt.x = 0;
		pt.y = 0;
		break;
	}

	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);

	if (pt.x + w > cx)
		pt.x = cx - w;
	if (pt.y + h > cy)
		pt.y = cy - h;

	if (pt.x < 0)
		pt.x = 0;
	if (pt.y < 0)
		pt.y = 0;

	return pt;
}

void CSkinRichMenu::CalculateRect()
{
	m_rcItem.left = 0;
	m_rcItem.top = 0;
	m_rcItem.right = m_nItemWidth;
	m_rcItem.bottom = m_nCapHeight + m_nDescHeight * m_cDescRow + HMARGIN * 3;

	if (m_rcItem.bottom < m_rcIcon.Height() + HMARGIN * 2)
		m_rcItem.bottom = m_rcIcon.Height() + HMARGIN * 2;

	m_rcCap.left = m_rcIcon.Width() + VMARGIN * 2 + 4;
	m_rcCap.right = m_rcItem.right - VMARGIN;

	m_rcDesc.left = m_rcCap.left;
	m_rcDesc.right = m_rcCap.right;

	m_rcClient.left = 0;
	m_rcClient.top = 0;
	m_rcClient.right = m_nItemWidth;
	m_rcClient.bottom = m_rcItem.Height() * m_cItemNum + 2;
}

BOOL CSkinRichMenu::ShowPopMenu(int x, int y, int nFlag, CWnd* pParent)
{
	CalculateRect();

	if (!this->Create(m_rcClient, pParent))
		return FALSE;

	POINT pt = GetAlignPos(x, y, nFlag);

	::SetWindowPos(this->GetSafeHwnd(), HWND_TOPMOST, pt.x, pt.y, 0, 0, SWP_NOSIZE);

	MemDrawItem();

	this->ShowWindow(SW_SHOW);

	this->UpdateWindow();

	return TRUE;
}

void CSkinRichMenu::DestroyMe()
{
	::PostMessage(m_hNotify, m_nMsg, 0, 0);

	m_dcMemHover.DeleteDC();
	m_dcMemItem.DeleteDC();

	m_ftCap.DeleteObject();
	m_ftDesc.DeleteObject();

	m_bDrawMemItem = FALSE;

	m_cDescRow = 2;
	m_nItemWidth = DEFAULT_WEIGHT;

	m_nCapHeight = 0;
	m_nDescHeight = 0;

	ClearItem();
	
	this->DestroyWindow();
}

void CSkinRichMenu::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	DestroyMe();
}

void CSkinRichMenu::AddItem(UINT nId, 
							LPCTSTR lpszItem, 
							LPCTSTR lpszDesc, 
							UINT nIconResId, 
							LPCTSTR lpsType, 
							BOOL bEnable, 
							BOOL bChecked)
{
	MENU_ITEM item;
	item.nID = nId;
	item.strCap = lpszItem;
	item.strDesc = lpszDesc;
	item.bEnable = bEnable;
	item.bCheck = bChecked;

	if (!ImageFromIDResource(nIconResId, lpsType, item.pImage))
		item.pImage = NULL;

	m_MenuItem.insert(CMenuItemMap::value_type(m_cItemNum, item));
	m_cItemNum ++;
}

void CSkinRichMenu::ClearItem()
{
	m_cItemNum = 0;
	m_nHoverItem = -1;
	m_MenuItem.clear();
}	

void CSkinRichMenu::SetNotifyWindow(HWND hWnd, UINT uMsg)
{
	m_hNotify = hWnd;
	m_nMsg = uMsg;
}

void CSkinRichMenu::SetIconSize(SIZE sz)
{
	m_rcIcon.left = 0;
	m_rcIcon.top = 0;
	m_rcIcon.right = sz.cx;
	m_rcIcon.bottom = sz.cy;
}

void CSkinRichMenu::SetMenuWidth(int nWidth)
{
	m_nItemWidth = nWidth;
}

int CSkinRichMenu::GetMenuWidth()
{
	return m_nItemWidth;
}

void CSkinRichMenu::SetDescRow(int nRow)
{
	if (nRow <= 2)
		m_cDescRow = 2;
	else
		m_cDescRow = nRow;
}

void CSkinRichMenu::SetBkColor(COLORREF clr)
{
	m_clrBackground = clr;
}

void CSkinRichMenu::SetTextColor(COLORREF clr)
{
	m_clrText = clr;
}

void CSkinRichMenu::SetCapFont(CFont* pFont)
{
	LOGFONT lf;
	
	pFont->GetLogFont(&lf);
	m_ftCap.DeleteObject();
	m_ftCap.CreateFontIndirect(&lf);

	m_nCapHeight = abs(lf.lfHeight);
}

void CSkinRichMenu::SetDescFont(CFont* pFont)
{
	LOGFONT lf;
	
	pFont->GetLogFont(&lf);
	m_ftDesc.DeleteObject();
	m_ftDesc.CreateFontIndirect(&lf);

	m_nDescHeight = abs(lf.lfHeight);
}

void CSkinRichMenu::SetHoverBmp(CBitmap* pBmp)
{
	BITMAP bmInfo;
	pBmp->GetObject(sizeof(bmInfo), &bmInfo);

	m_rcHover.left = 0;
	m_rcHover.top = 0;
	m_rcHover.right = bmInfo.bmWidth;
	m_rcHover.bottom = bmInfo.bmHeight;

	m_dcMemHover.CreateCompatibleDC(NULL);
	m_dcMemHover.SelectObject(pBmp);
}

BOOL CSkinRichMenu::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CSkinRichMenu::OnPaint()
{
	CPaintDC dc(this); // device context for painting 

	if (!m_bDrawMemItem)
		return;

	CElcMemDC dcMem(&dc, m_rcClient);
	dcMem.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), &m_dcMemItem, 0, 0, SRCCOPY);
	DrawHoverItem(&dcMem);
	dcMem.DrawMe();
}

void CSkinRichMenu::DrawHoverItem(CDC* pDC)
{	
	if (m_nHoverItem >= 0 && m_nHoverItem < m_cItemNum)
	{
		CRect rc;
		rc.left = 0;
		rc.right = m_rcItem.right-2;
		rc.top = m_rcItem.Height() * m_nHoverItem;
		rc.bottom = rc.top + m_rcItem.Height();

		if (m_MenuItem[m_nHoverItem].bEnable)
		{
			pDC->StretchBlt(rc.left, 
				rc.top,
				rc.Width(), 
				rc.Height(), 
				&m_dcMemHover, 
				0, 
				0,
				m_rcHover.Width(),
				m_rcHover.Height(),
				SRCCOPY);

			pDC->FrameRect(&rc, &m_brItem);
		}
//		else 
//			pDC->Rectangle(&rc);
	}

	DrawItem(pDC, &m_MenuItem[m_nHoverItem], m_nHoverItem);
}

void CSkinRichMenu::MemDrawItem()
{
	CDC* pDC = GetDC();

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, m_rcClient.Width(), m_rcClient.Height());
	m_dcMemItem.CreateCompatibleDC(pDC);

	m_dcMemItem.SelectObject(&bmp);

	m_dcMemItem.FillSolidRect(&m_rcClient, m_clrBackground);

	for (int i=0; i < m_cItemNum; i++)
	{
		DrawItem(&m_dcMemItem, &m_MenuItem[i], i);
	}

	m_bDrawMemItem = TRUE;
}

void CSkinRichMenu::DrawItem(CDC* pDC, PMENU_ITEM pItem, int nIndex)
{
	int nHIconMargin = (m_rcItem.Height() - m_rcIcon.Height()) / 2;

	// Draw check
	if (pItem->bCheck) {
		CRect rcCheck(VMARGIN, m_rcItem.Height() * nIndex + nHIconMargin, 
			VMARGIN + m_rcIcon.Width(), m_rcItem.Height() * nIndex + nHIconMargin + m_rcIcon.Height());
		rcCheck.DeflateRect(-4, -4);
		pDC->FillSolidRect(&rcCheck, RGB(255,227,149));
		pDC->FrameRect(&rcCheck, &m_brCheck);
	}

	// draw icon
	CRect rcIcon;
	rcIcon.left = VMARGIN;
	rcIcon.top = m_rcItem.Height() * nIndex + nHIconMargin;
	rcIcon.right = rcIcon.left + m_rcIcon.Width();
	rcIcon.bottom = rcIcon.top + m_rcIcon.Height();
	thePainter.DrawImageGDI(pDC->GetSafeHdc(), pItem->pImage, rcIcon);

	if (pItem->bEnable)
		pDC->SetTextColor(m_clrText);
	else 
		pDC->SetTextColor(m_clrDisable);

	pDC->SetBkMode(TRANSPARENT);

	// draw caption
	m_rcCap.top = m_rcItem.Height() * nIndex + VMARGIN;
	m_rcCap.bottom = m_rcCap.top + m_nCapHeight;
	CFont* pFont = pDC->SelectObject(&m_ftCap);
	pDC->DrawText(pItem->strCap, &m_rcCap, DT_WORDBREAK | DT_NOPREFIX);

	// draw description
	m_rcDesc.top = m_rcCap.bottom + VMARGIN;
	m_rcDesc.bottom = m_rcItem.Height() * (nIndex + 1) - VMARGIN;
	pDC->SelectObject(&m_ftDesc);
	pDC->DrawText(pItem->strDesc, &m_rcDesc, DT_WORDBREAK | DT_NOPREFIX);

	pDC->SelectObject(pFont);
}

void CSkinRichMenu::OnNcPaint()
{
	CWindowDC dc(this);
	dc.FrameRect(m_rcClient, &m_brFrame);
}

void CSkinRichMenu::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_rcClient.PtInRect(point) && m_MenuItem[m_nHoverItem].bEnable) 
	{
		::PostMessage(m_hNotify, m_nMsg, m_MenuItem[m_nHoverItem].nID, 0);
		DestroyMe();
	}
	else
		CWnd::OnLButtonDown(nFlags, point);
}

void CSkinRichMenu::OnMouseMove(UINT nFlags, CPoint point)
{
	SetCapture();

	if (m_rcClient.PtInRect(point)) 
	{
		int nHoverItem = point.y / m_rcItem.Height();
		if (m_nHoverItem != nHoverItem) {
			CRect rcOld = m_rcItem, rcNew = m_rcItem;
			rcOld.top = m_rcItem.Height() * m_nHoverItem;
			rcOld.bottom = rcOld.top + m_rcItem.Height();
			rcNew.top = m_rcItem.Height() * nHoverItem;
			rcNew.bottom = rcNew.top + m_rcItem.Height();
			
			m_nHoverItem = nHoverItem;
			InvalidateRect(&rcOld, FALSE);
			InvalidateRect(&rcNew, FALSE);
		}
	}
	else {
		ReleaseCapture();
		m_nHoverItem = -1;
		Invalidate(FALSE);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CSkinRichMenu::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_ESCAPE == nChar) {
		DestroyMe();
		return ;
	}
	else if (VK_UP == nChar) {
		m_nHoverItem --;
		if (m_nHoverItem < 0)
			m_nHoverItem = m_cItemNum - 1;
		Invalidate(FALSE);
	}
	else if (VK_DOWN == nChar) {
		m_nHoverItem ++;
		if (m_nHoverItem >= m_cItemNum)
			m_nHoverItem = 0;
		Invalidate(FALSE);
	}
	else if (VK_SPACE == nChar || VK_RETURN == nChar) {
		if (m_nHoverItem >= 0 
			&& m_nHoverItem < m_cItemNum 
			&& m_MenuItem[m_nHoverItem].bEnable)
		{
			::PostMessage(m_hNotify, m_nMsg, m_MenuItem[m_nHoverItem].nID, 0);
			DestroyMe();
		}
	}
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSkinRichMenu::OnDestroy()
{
	::PostMessage(m_hNotify, m_nMsg, 0, 0);

	CWnd::OnDestroy();

	delete this;
}
