// ElcRichMenu.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcRichMenu.h"


// CElcRichMenu

IMPLEMENT_DYNAMIC(CElcRichMenu, CWnd)

CElcRichMenu::CElcRichMenu()
{
	m_sIcon.SetSize(32, 32);
	m_rcBody.SetRectEmpty();
	m_nInterMargin = 4;
	m_nBorderMargin = 2;
	m_nHotItem = -1;
	m_nMaxDescWidth = 250;

	m_pParent = NULL;

	LOGFONT lf = {0};
	GetMSShellDlgFont(lf);
	m_ftDesc.CreateFontIndirect(&lf);
	lf.lfWeight = FW_BOLD;
	m_ftText.CreateFontIndirect(&lf);

	memset(&m_skin, 0, sizeof(m_skin));
	LoadSkin(ELC_SKIN_STYLE_RICHMENU_MAIN);
}

CElcRichMenu::~CElcRichMenu()
{
}

BOOL CElcRichMenu::LoadSkin(LPCTSTR lpszStyle)
{
	PELC_SKIN_RICH_MENU pSkin = theSkin.GetRichMenuSkin(lpszStyle);
	if (!pSkin)
		return FALSE;

	memcpy(&m_skin, pSkin, sizeof(m_skin));

	if (::IsWindow(m_hWnd)) 
		Invalidate();

	return TRUE;
}


BEGIN_MESSAGE_MAP(CElcRichMenu, CWnd)
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CElcRichMenu message handlers

POINT CElcRichMenu::GetAlignPos(int x, int y, int nFlag)
{
	int w = m_rcBody.Width();
	int h = m_rcBody.Height();

	POINT pt;

	DWORD dwLayout = 0;
	GetProcessDefaultLayout(&dwLayout);
	if (dwLayout == LAYOUT_RTL) {
		switch (nFlag)
		{
			case ELCRMA_LEFT_TOP: 
				nFlag = ELCRMA_RIGHT_TOP;
				break;
			case ELCRMA_RIGHT_TOP:
				nFlag = ELCRMA_LEFT_TOP;
				break;
			case ELCRMA_LEFT_BOTTOM:
				nFlag = ELCRMA_RIGHT_BOTTOM;
				break;
			case ELCRMA_RIGHT_BOTTOM:
				nFlag = ELCRMA_LEFT_BOTTOM;
				break;
			default:
				break;
		}
	}

	switch (nFlag)
	{
	case ELCRMA_LEFT_TOP: 
		pt.x = x;
		pt.y = y;
		break;
	case ELCRMA_RIGHT_TOP:
		pt.x = x - w;
		pt.y = y;
		break;
	case ELCRMA_LEFT_BOTTOM:
		pt.x = x;
		pt.y = y - h;
		break;
	case ELCRMA_RIGHT_BOTTOM:
		pt.x = x - w;
		pt.y = y - h;
		break;
	case ELCRMA_TOP_CENTER:
		pt.x = x - w / 2;
		pt.y = y;
		break;
	case ELCRMA_BOTTOM_CENTER:
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

BOOL CElcRichMenu::Popup(int x, int y, UINT nFlag, CWnd* pParent, UINT nId)
{
	Relayout();

	if (!CreateEx(WS_EX_TOOLWINDOW, 
		AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW), 
		_T(""),
		WS_POPUP, 
		m_rcBody,
		pParent,
		0))
		return FALSE;

	m_pParent = pParent;
	m_nId = nId;

	DrawMemDC();

	POINT pt = GetAlignPos(x, y, nFlag);
	SetWindowPos(&CWnd::wndTopMost, 
		pt.x, 
		pt.y, 
		m_rcBody.Width(), 
		m_rcBody.Height(), 
		SWP_SHOWWINDOW);

	return TRUE;
}

int CElcRichMenu::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CElcRichMenu::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	Destroy();
}

BOOL CElcRichMenu::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CElcRichMenu::OnPaint()
{
	CPaintDC dc(this);

	CElcMemDC dcMem(&dc, m_rcBody);
	dcMem.BitBlt(0, 0, m_rcBody.Width(), m_rcBody.Height(),
		&m_dcMem, 0, 0, SRCCOPY);

	if (m_nHotItem != -1 
		&& (m_arrItem[m_nHotItem].dwState & MF_DISABLED) == 0) {
		CRect rcItem;
		rcItem.left = m_rcBody.left + m_nBorderMargin;
		rcItem.right = m_rcBody.right - m_nBorderMargin;
		rcItem.top = m_rcBody.top + m_nBorderMargin + m_nItemHeight * m_nHotItem;
		rcItem.bottom = rcItem.top + m_nItemHeight;
		DrawItem(&dcMem, rcItem, m_nHotItem, TRUE);
	}
	
	dcMem.DrawMe();
}

void CElcRichMenu::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	if (m_nHotItem != -1 && m_pParent) {
		if (m_arrItem[m_nHotItem].dwState & MF_DISABLED)
			return;

		m_pParent->PostMessage(WM_COMMAND, m_arrItem[m_nHotItem].nId);
	}

	Destroy();
}

void CElcRichMenu::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);

	if (m_nHotItem != -1 && m_pParent) {
		if (m_arrItem[m_nHotItem].dwState & MF_DISABLED)
			return;

		m_pParent->PostMessage(WM_COMMAND, m_arrItem[m_nHotItem].nId);
	}

	Destroy();
}

void CElcRichMenu::OnMouseMove(UINT nFlags, CPoint point)
{
	SetCapture();

	if (m_rcBody.PtInRect(point)) {
		int nHotItem = point.y / m_nItemHeight;
		if (nHotItem < 0 || nHotItem >= (int)m_arrItem.size())
			nHotItem = -1;

		if (m_nHotItem != nHotItem) {
			m_nHotItem = nHotItem;
			Invalidate();
		}
	}
	else {
		ReleaseCapture();
		m_nHotItem = -1;
		Invalidate();
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CElcRichMenu::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_ESCAPE == nChar) {
		Destroy();
		return ;
	}
	else if (VK_UP == nChar) {
		m_nHotItem --;
		if (m_nHotItem < 0)
			m_nHotItem = (int)m_arrItem.size() - 1;
		Invalidate();
	}
	else if (VK_DOWN == nChar) {
		m_nHotItem ++;
		if (m_nHotItem >= (int)m_arrItem.size())
			m_nHotItem = 0;
		Invalidate();
	}
	else if (VK_SPACE == nChar || VK_RETURN == nChar) {
		if (m_nHotItem >= 0 
			&& m_nHotItem < (int)m_arrItem.size() 
			&& (m_arrItem[m_nHotItem].dwState & MF_DISABLED)  == 0
			&& m_pParent)
		{
			m_pParent->PostMessage(WM_COMMAND, m_arrItem[m_nHotItem].nId);
		}

		Destroy();
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CElcRichMenu::OnDestroy()
{
	CWnd::OnDestroy();

	if (m_pParent)
		m_pParent->PostMessage(WM_ELC_RICH_MENU_NOTIFY, m_nId);

	delete this;
}

BOOL CElcRichMenu::AddItem(UINT nId, 
						   UINT nTextResId, 
						   UINT nDescResId, 
						   UINT nIconResId, 
						   LPCTSTR lpszIconType)
{
	CString strText, strDesc;
	strText.LoadString(nTextResId);
	strDesc.LoadString(nDescResId);

	return AddItem(nId, strText, strDesc, nIconResId, lpszIconType);
}

BOOL CElcRichMenu::AddItem(UINT nId, 
						   LPCTSTR lpszText, 
						   LPCTSTR lpszDesc, 
						   UINT nIconResId, 
						   LPCTSTR lpszIconType)
{
	ELC_RICHMENU_ITEM item;
	item.nId = nId;
	item.strText = lpszText;
	item.strDesc = lpszDesc;
	item.dwState = 0;
	item.pIcon = ImageFromIDResource(nIconResId, lpszIconType);
	item.sText = CElcSkinDoc::GetStringSize(item.strText, &m_ftText);
	item.sDesc = CElcSkinDoc::GetStringSize(item.strDesc, &m_ftDesc);

	m_arrItem.push_back(item);

	return TRUE;
}

void CElcRichMenu::ClearItems()
{
	for (CItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (it->pIcon)
			delete it->pIcon;
	}

	m_arrItem.clear();
}

int CElcRichMenu::FindItem(UINT nId)
{
	for (int i=0; i < (int)m_arrItem.size(); i++) {
		if (m_arrItem[i].nId == nId)
			return i;
	}
	return -1;
}

void CElcRichMenu::CheckItem(UINT nId, BOOL bChecked)
{
	int nItem = FindItem(nId);
	if (nItem == -1)
		return;

	if (bChecked)
		m_arrItem[nItem].dwState |= MF_CHECKED;
	else
		m_arrItem[nItem].dwState &= ~MF_CHECKED;
}

BOOL CElcRichMenu::IsItemChecked(UINT nId)
{
	int nItem = FindItem(nId);
	if (nItem == -1)
		return FALSE;

	return (m_arrItem[nItem].dwState & MF_CHECKED) != 0;
}

void CElcRichMenu::EnableItem(UINT nId, BOOL bEnabled)
{
	int nItem = FindItem(nId);
	if (nItem == -1)
		return;

	if (bEnabled)
		m_arrItem[nItem].dwState &= ~MF_DISABLED;
	else
		m_arrItem[nItem].dwState |= MF_DISABLED;
}

BOOL CElcRichMenu::IsItemEnabled(UINT nId)
{
	int nItem = FindItem(nId);
	if (nItem == -1)
		return FALSE;

	return (m_arrItem[nItem].dwState & MF_DISABLED) == 0;
}

void CElcRichMenu::Relayout()
{
	int nMaxTextWidth = 0;
	int nMaxDescWidth = 0;
	for (CItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (nMaxTextWidth < it->sText.cx)
			nMaxTextWidth = it->sText.cx;
		if (nMaxDescWidth < it->sDesc.cx)
			nMaxDescWidth = it->sDesc.cx;
	}

	m_sMaxText.SetSize(0, 0);
	m_sMaxDesc.SetSize(0, 0);
	int nMaxWidth = 0;
	if (nMaxTextWidth >= nMaxDescWidth) {
		m_sMaxText.cx = nMaxTextWidth;
		m_sMaxText.cy = m_arrItem[0].sText.cy;
		m_sMaxDesc.cx = nMaxTextWidth;
		m_sMaxDesc.cy = m_arrItem[0].sDesc.cy;
		nMaxWidth = m_nBorderMargin * 4 + m_nInterMargin * 4 + m_sIcon.cx + nMaxTextWidth;
	}
	else if (nMaxDescWidth <= m_nMaxDescWidth) {
		m_sMaxText.cx = nMaxDescWidth;
		m_sMaxText.cy = m_arrItem[0].sText.cy;
		m_sMaxDesc.cx = nMaxDescWidth;
		m_sMaxDesc.cy = m_arrItem[0].sDesc.cy;
		nMaxWidth = m_nBorderMargin * 4 + m_nInterMargin * 4 + m_sIcon.cx + nMaxDescWidth;
	}
	else {
		int nMaxDescHeight = 0;
		for (CItemArray::iterator it = m_arrItem.begin();
			it != m_arrItem.end();
			it ++)
		{
			nMaxDescHeight = max(CElcSkinDoc::CetStringHeight(it->strDesc, m_nMaxDescWidth, &m_ftDesc), nMaxDescHeight);
		}
		
		m_sMaxText.cx = m_nMaxDescWidth;
		m_sMaxText.cy = m_arrItem[0].sText.cy;
		m_sMaxDesc.cx = m_nMaxDescWidth;
		m_sMaxDesc.cy = nMaxDescHeight;
		nMaxWidth = m_nBorderMargin * 4 + m_nInterMargin * 4 + m_sIcon.cx + m_nMaxDescWidth;
	}

	if (m_sMaxDesc.cy != 0) {
		m_nItemHeight = max(m_nBorderMargin * 2 + m_nInterMargin * 2 + m_sIcon.cy,
			m_nBorderMargin * 2 + m_nInterMargin * 3 + m_sMaxText.cy + m_sMaxDesc.cy);
	}
	else {
		m_nItemHeight = max(m_nBorderMargin * 2 + m_nInterMargin * 2 + m_sIcon.cy,
			m_nBorderMargin * 2 + m_sMaxText.cy);
	}

	m_rcBody.SetRect(0, 0, nMaxWidth, m_nItemHeight * (int)m_arrItem.size() + m_nBorderMargin * 2);
}

void CElcRichMenu::DrawMemDC()
{
	CDC * pDC = GetDC();
	if (!pDC)
		return;

	m_dcMem.CreateMe(pDC, m_rcBody);

	thePainter.DrawImageGDI(m_dcMem.GetSafeHdc(),
		&m_skin.backgnd,
		m_rcBody,
		0,
		0,
		1,
		0);

	CRect rcItem;
	rcItem.left = m_rcBody.left + m_nBorderMargin;
	rcItem.right = m_rcBody.right - m_nBorderMargin;
	for (int i=0; i < (int)m_arrItem.size(); i ++) {
		rcItem.top = m_rcBody.top + m_nBorderMargin + m_nItemHeight * i;
		rcItem.bottom = rcItem.top + m_nItemHeight;
		DrawItem(&m_dcMem, rcItem, i, FALSE);
	}

	ReleaseDC(pDC);
}

void CElcRichMenu::DrawItem(CDC* pDC, CRect& rcItem, int nIndex, BOOL bHotItem)
{
	PELC_RICHMENU_ITEM pItem = &m_arrItem[nIndex];

	BOOL bDisabled = (pItem->dwState & MF_DISABLED) != 0;
	ImageAttributes *pImageAtt = NULL;
	ImageAttributes imageAtt;
	if (bDisabled) {
		imageAtt.SetColorMatrix(&g_GrayColorMatrix);
		pImageAtt = &imageAtt;
	}

	CRect rect = rcItem;
	if (bHotItem && !bDisabled) {
		thePainter.DrawImageGDI(pDC->GetSafeHdc(),
			&m_skin.select,
			rect,
			0,
			0,
			1,
			0);
	}

	if (pItem->dwState & MF_CHECKED) {
		rect.left = rcItem.left + m_nBorderMargin;
		rect.right = rcItem.right - m_nInterMargin * 2 - m_sMaxText.cx - m_nBorderMargin;
		rect.top = rcItem.top + (rcItem.Height() - rect.Width()) / 2;
		rect.bottom = rect.top + rect.Width();

		thePainter.DrawImageGDI(pDC->GetSafeHdc(),
			&m_skin.check,
			rect,
			0,
			0,
			1,
			0,
			pImageAtt);
	}

	if (pItem->pIcon) {
		rect.left = rcItem.left + m_nBorderMargin + m_nInterMargin;
		rect.right = rect.left + m_sIcon.cx;
		rect.top = rcItem.top + (rcItem.Height() - m_sIcon.cy) / 2;
		rect.bottom = rect.top + m_sIcon.cy;
		thePainter.DrawImageGDI(pDC->GetSafeHdc(), pItem->pIcon, rect, pImageAtt);
	}

	if (bDisabled)
		pDC->SetTextColor(m_skin.crTextDisabled);
	else if (bHotItem)
		pDC->SetTextColor(m_skin.crTextHot);
	else
		pDC->SetTextColor(m_skin.crTextNormal);


	CFont* pOldFont = pDC->SelectObject(&m_ftText);

	rect.right = rcItem.right - m_nBorderMargin - m_nInterMargin;
	rect.left = rect.right - m_sMaxText.cx;

	if (pItem->strDesc.IsEmpty()) {
		rect.top = rcItem.top;
		rect.bottom = rcItem.bottom;
		pDC->DrawText(pItem->strText, rect, DT_VCENTER | DT_SINGLELINE);
	}
	else {
		rect.top = rcItem.top + m_nInterMargin;
		rect.bottom = rect.top + m_sMaxText.cy;
		pDC->DrawText(pItem->strText, rect, DT_VCENTER | DT_SINGLELINE);

		pDC->SelectObject(&m_ftDesc);
		rect.top = rect.bottom + m_nInterMargin;
		rect.bottom = rect.top + m_sMaxDesc.cy;
		if (rect.Width() >= pItem->sDesc.cx)
			pDC->DrawText(pItem->strDesc, rect, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		else
			pDC->DrawText(pItem->strDesc, rect, DT_WORDBREAK | DT_NOFULLWIDTHCHARBREAK | DT_NOPREFIX);
	}

	pDC->SelectObject(&pOldFont);
}

void CElcRichMenu::Destroy()
{
	ClearItems();
	PostMessage(WM_CLOSE);
}