// ElcStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcStatusBar.h"


struct AFX_STATUSPANE
{
	UINT    nID;        // IDC of indicator: 0 => normal text area
	int     cxText;     // width of string area in pixels
						//   on both sides there is a 3 pixel gap and
						//   a one pixel border, making a pane 6 pixels wider
	UINT    nStyle;     // style flags (SBPS_*)
	UINT    nFlags;     // state flags (SBPF_*)
	CString strText;    // text in the pane
};


// CElcStatusBar

IMPLEMENT_DYNAMIC(CElcStatusBar, CStatusBar)

CElcStatusBar::CElcStatusBar()
{
	memset(&m_skin, 0, sizeof(m_skin));

	GetMSShellDlgFont(m_ftText);
}

CElcStatusBar::~CElcStatusBar()
{
}

BOOL CElcStatusBar::LoadSkin(LPCTSTR lpszStyle)
{
	PELC_SKIN_STATUSBAR pSkin = theSkin.GetStatusBarSkin(lpszStyle);
	if (!pSkin)
		return TRUE;

	memcpy(&m_skin, pSkin, sizeof(m_skin));
	return TRUE;
}


BEGIN_MESSAGE_MAP(CElcStatusBar, CStatusBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CElcStatusBar message handlers
int CElcStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_skin.background.pImage
		&& !LoadSkin(ELC_SKIN_STYLE_STATUSBAR_MAINFRAME))
		return -1;

	GetStatusBarCtrl().SetMinHeight(CElcSkinDoc::GetImageSize(&m_skin.background).cy);

	return 0;
}

void CElcStatusBar::OnSize(UINT nType, int cx, int cy)
{
	CStatusBar::OnSize(nType, cx, cy);

	Relayout();
}

BOOL CElcStatusBar::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CElcStatusBar::OnPaint()
{
	CPaintDC dc(this);

	CElcMemDC memdc(&dc, m_rcClient);
	memdc.FillSolidRect(m_rcClient, m_skin.crBackgnd);

	// draw background image
	thePainter.DrawImageGDI(memdc.GetSafeHdc(),
		&m_skin.background,
		m_rcClient, 
		0,
		0,
		1,
		0);

	DrawItems(&memdc);

	memdc.DrawMe();
}

void CElcStatusBar::Relayout()
{
	GetClientRect(m_rcClient);
	if (m_rcClient.IsRectEmpty())
		return;

	CStatusBarCtrl& statusctrl = GetStatusBarCtrl();
	CRect rect;
	for (int i=0; (size_t)i < m_arrItem.size(); i++) {
		if (m_arrItem[i].nType == ELC_SBIT_BUTTON 
			|| m_arrItem[i].nType == ELC_SBIT_CTRL)
		{
			statusctrl.GetRect(i, &rect);
			rect.right = rect.left + m_arrItem[i].size.cx;
			rect.top = rect.top + (rect.Height() - m_arrItem[i].size.cy) / 2;
			rect.bottom = rect.top + m_arrItem[i].size.cy;
			((CWnd*)m_arrItem[i].dwCtrl)->MoveWindow(rect);
		}
	}
}

void CElcStatusBar::DrawItems(CDC* pDC)
{
	CFont* pOldFont = pDC->SelectObject(&m_ftText);

	CStatusBarCtrl& statusctrl = GetStatusBarCtrl();

	int pParts[255] = {0};
	int nCount = statusctrl.GetParts(_countof(pParts), pParts);
	int nFormat = DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_NOFULLWIDTHCHARBREAK|DT_WORDBREAK;

	CRect rcItem;
	CString strText;
	for (int i = 0; i < nCount; i++) {

		statusctrl.GetRect(i, &rcItem);
		if (i == 0)
			rcItem.left += 2;
		else if (i == nCount - 1) 
			rcItem.right = m_rcClient.right - (CElcSkinDoc::GetImageSize(&m_skin.background).cx - m_skin.background.nRightSplit);

		//pDC->Draw3dRect(rcItem, m_skin.crPaneBorder, m_skin.crPaneBorder);

		if (m_arrItem[i].nType == ELC_SBIT_IMAGE) {
			RECT rcImge;
			rcImge.left = rcItem.left;
			rcImge.right = rcImge.left + m_arrItem[i].size.cx;
			rcImge.top = rcItem.top + (rcItem.Height() - m_arrItem[i].size.cy) / 2;
			rcImge.bottom = rcImge.top + m_arrItem[i].size.cy;
			thePainter.DrawImageGDI(pDC->GetSafeHdc(), (Image*)m_arrItem[i].dwCtrl, rcImge);
		}

		GetPaneText(i, strText);
		if(strText.IsEmpty())
			continue;

		UINT uStyle = GetPaneStyle(i);
		if (uStyle & SBPS_DISABLED)
			pDC->SetTextColor(m_skin.crTextDisabled);
		else 
			pDC->SetTextColor(m_skin.crTextNormal);

		pDC->DrawText(strText, rcItem, nFormat);
	}

	pDC->SelectObject(pOldFont);
}

BOOL CElcStatusBar::SetIndicators(const UINT* lpIDArray, int nIDCount)
{
	if (!__super::SetIndicators(lpIDArray, nIDCount))
		return FALSE;

	ELC_STATUSBAR_ITEM item;
	item.nType = ELC_SBIT_NONE;
	item.dwCtrl = 0;
	item.size.cx = item.size.cy = 0;
	item.dwData = 0;

	for (int i=0; i < nIDCount; i ++) {
		m_arrItem.push_back(item);
	}

	return TRUE;
}

BOOL CElcStatusBar::InsertCtrl(int nIndex, PELC_STATUSBAR_ITEM pItem, LPCTSTR lpszText)
{
	void* pData = NULL;
	if ((pData = calloc(m_nCount + 1, sizeof(AFX_STATUSPANE))) == NULL)
		return FALSE;

	for (int i=0, j=0; i < m_nCount; i++, j++) {
		if (i == nIndex)
			j ++;
		memcpy(((AFX_STATUSPANE*)pData) + j, ((AFX_STATUSPANE*)m_pData) + i, sizeof(AFX_STATUSPANE));
	}

	if (nIndex >= m_nCount) {
		nIndex = m_nCount;
		m_arrItem.push_back(*pItem);
	}
	else {
		m_arrItem.insert(m_arrItem.begin() + nIndex, *pItem);
	}

	AFX_STATUSPANE pane;
	pane.nID = ID_SEPARATOR;
	pane.cxText = pItem->size.cx;
	pane.nStyle = SBPS_NOBORDERS | SBPS_NORMAL;
	pane.nFlags = 0;
	pane.strText = lpszText;
	memcpy(((AFX_STATUSPANE*)pData) + nIndex, &pane, sizeof(AFX_STATUSPANE));

	AllocElements(m_nCount + 1, sizeof(AFX_STATUSPANE));
	memcpy(m_pData, pData, sizeof(AFX_STATUSPANE) * (m_nCount + 1));
	free(pData);

	UpdateAllPanes(TRUE, FALSE);

	return TRUE;
}

int CElcStatusBar::InsertImage(int nIndex, UINT nImageResId, LPCTSTR lpszType)
{
	if (nIndex < 0)
		nIndex = 0;
	else if (nIndex > m_nCount)
		nIndex = m_nCount;

	Image* pImage = NULL;
	if (!ImageFromIDResource(nImageResId, lpszType, pImage))
		return -1;

	ELC_STATUSBAR_ITEM item;
	item.nType = ELC_SBIT_IMAGE;
	item.dwCtrl = (DWORD_PTR)pImage;
	item.size.cx = pImage->GetWidth();
	item.size.cy = pImage->GetHeight();
	item.dwData = 0;

	if (!InsertCtrl(nIndex, &item, NULL)) {
		delete pImage;
		nIndex = -1;
	}

	return nIndex;
}

int CElcStatusBar::InsertButton(int nIndex, UINT nId, LPCTSTR lpszCaption, UINT nResId, LPCTSTR lpszType, ELC_BUTTON_ICON_TYPE type)
{
	if (nIndex < 0)
		nIndex = 0;
	else if (nIndex > m_nCount)
		nIndex = m_nCount;

	CElcButton* pButton = new CElcButton;
	do {
		if (!pButton)
			break;
		if (!pButton->Create(lpszCaption, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, nId))
			break;

		pButton->SetIconImage(nResId, lpszType, type);

		ELC_STATUSBAR_ITEM item;
		item.nType = ELC_SBIT_BUTTON;
		item.dwCtrl = (DWORD_PTR)pButton;
		item.size = pButton->GetIdealSize();
		item.dwData = 0;

		if (!InsertCtrl(nIndex, &item, NULL))
			break;

		Relayout();

		return nIndex;

	} while (0);

	if (pButton) {
		if (pButton->GetSafeHwnd())
			pButton->DestroyWindow();
		delete pButton;
	}

	return -1;
}

int CElcStatusBar::InsertLabel(int nIndex, LPCTSTR lpszText)
{
	if (nIndex < 0)
		nIndex = 0;
	else if (nIndex > m_nCount)
		nIndex = m_nCount;

	ELC_STATUSBAR_ITEM item;
	item.nType = ELC_SBIT_NONE;
	item.dwCtrl = NULL;
	item.size = CElcSkinDoc::GetStringSize(lpszText);
	item.dwData = 0;

	if (!InsertCtrl(nIndex, &item, lpszText))
		return -1;
	return nIndex;
}

int CElcStatusBar::InsertControl(int nIndex, CWnd* pWnd, CSize size)
{
	if (nIndex < 0)
		nIndex = 0;
	else if (nIndex > m_nCount)
		nIndex = m_nCount;

	if (!pWnd->GetSafeHwnd())
		return -1;
	pWnd->SetParent(this);

	ELC_STATUSBAR_ITEM item;
	item.nType = ELC_SBIT_CTRL;
	item.dwCtrl = (DWORD_PTR)pWnd;
	item.size = size;
	item.dwData = 0;

	if (!InsertCtrl(nIndex, &item, NULL))
		return -1;

	Relayout();

	return nIndex;
}

BOOL CElcStatusBar::SetImage(int nIndex, UINT nImageResId, LPCTSTR lpszType)
{
	if (nIndex < 0 
		|| nIndex >= m_nCount
		|| m_arrItem[nIndex].nType != ELC_SBIT_IMAGE)
		return FALSE;

	Image* pImage = NULL;
	if (!ImageFromIDResource(nImageResId, lpszType, pImage))
		return FALSE;

	delete ((Image*)m_arrItem[nIndex].dwCtrl);
	m_arrItem[nIndex].dwCtrl = (DWORD_PTR)pImage;
	m_arrItem[nIndex].size.cx = pImage->GetWidth();
	m_arrItem[nIndex].size.cy = pImage->GetHeight();

	(((AFX_STATUSPANE*)m_pData) + nIndex)->cxText = m_arrItem[nIndex].size.cx;

	CRect rcItem;
	CStatusBarCtrl& statusctrl = GetStatusBarCtrl();
	statusctrl.GetRect(nIndex, &rcItem);
	InvalidateRect(rcItem);

	return TRUE;
}

CElcButton* CElcStatusBar::GetButton(UINT nId)
{
	for (CItemArray::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (it->nType == ELC_SBIT_BUTTON 
			&& ((CElcButton *)it->dwCtrl)->GetDlgCtrlID() == nId)
			return ((CElcButton *)it->dwCtrl);
	}

	return NULL;
}

void CElcStatusBar::DelItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nCount)
		return;
	m_arrItem.erase(m_arrItem.begin() + nIndex);

	void* pData = NULL;
	if ((pData = calloc(m_nCount - 1, sizeof(AFX_STATUSPANE))) == NULL)
		return;

	for (int i=0, j=0; i < m_nCount; i++) {
		if (i == nIndex)
			continue;
		j ++;
		memcpy(((AFX_STATUSPANE*)pData) + j, ((AFX_STATUSPANE*)m_pData) + i, sizeof(AFX_STATUSPANE));
	}

	AllocElements(m_nCount - 1, sizeof(AFX_STATUSPANE));
	memcpy(m_pData, pData, sizeof(AFX_STATUSPANE) * (m_nCount - 1));
	free(pData);

	m_arrItem.erase(m_arrItem.begin() + nIndex);

	UpdateAllPanes(TRUE, FALSE);
}

UINT CElcStatusBar::GetItemType(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nCount)
		return ELC_SBIT_NONE;

	return m_arrItem[nIndex].nType;
}

void CElcStatusBar::SetItemData(int nIndex, DWORD_PTR dwData)
{
	if (nIndex >= 0 && nIndex < m_nCount)
		m_arrItem[nIndex].dwData = dwData;
}

DWORD_PTR CElcStatusBar::GetItemData(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nCount)
		return m_arrItem[nIndex].dwData;
	return NULL;
}
