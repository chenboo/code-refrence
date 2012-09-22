// FloatBar.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "FloatBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define HMARGIN 10
#define VMARGIN 5

#define ROUNDED_CX 3
#define ROUNDED_CY 3


#define FLOATBAR_HIDE_INCREASE			2
#define FLOATBAR_HIDE_REMAIN_HEIGHT		5

#define TIMER_ELAPSE_HIDE			10
#define TIMER_ELAPSE_TRACE_MOUSE	2000

// CFloatBar

IMPLEMENT_DYNAMIC(CFloatBar, CWnd)

CFloatBar::CFloatBar()
{
	m_hNotifyHwnd = NULL;
	m_uNotifyMsg = NULL;

	m_nDefaultbtnCx = 0;
	m_nDefaultbtnCy = 0;
	m_nSeparatorWidth = 16;
	m_nBtnSpace = 5;

	m_nFloatBarWidth = 0;
	m_nFloatBarHeight = 0;
	m_nLeftWidth = 0;
	m_nRightWidth = 0;
	m_nCenterWidth = 0;

	m_bExpandBar = TRUE;
	m_bLBtnDown = FALSE;
	m_bEnableDrag = TRUE;

	m_bMirror = FALSE;

	m_pbmTwinkling = NULL;
	m_nStateNums = 0;
	m_nCurState = 0;
	m_nFrequency = 500;
	m_bShowTwinkling = FALSE;
	m_bTimerOn = FALSE;
	m_strTime = _T("");

	m_dwEscapeTime = 0;
	m_dwOldTime = 0;

	m_bActive = TRUE;

	m_bEnableAutoHide = FALSE;
	m_bHideBar = FALSE;
	m_nHideTopPos = 0;
	m_dwHideState = AUTOHIDE_STATE_SHOW;
}

CFloatBar::~CFloatBar()
{
	ReleaseButtonInfo(&m_BtnShow);
	ReleaseButtonInfo(&m_BtnHide);

	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++) {
		ReleaseButtonInfo(&(BUTTON_INFO(*it)));
	}

	if (m_pbmTwinkling != NULL) {
		for (int i=0; i < m_nStateNums; i++) 
			m_pbmTwinkling[i].DeleteObject();

		delete []m_pbmTwinkling;
		m_pbmTwinkling = NULL;
	}
}


BEGIN_MESSAGE_MAP(CFloatBar, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()

	ON_MESSAGE(ID_FLOATBAR_BTN_CLICKED, OnFloatBarBtnCliked)
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChanged)
	ON_MESSAGE(ID_FLOATBAR_SLIDER_POS, OnFloatBarSliderPosChanged)
END_MESSAGE_MAP()



// CFloatBar message handlers

int CFloatBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndTooltip.Create(this, TTS_ALWAYSTIP);
	m_wndTooltip.Activate(TRUE);
	m_wndTooltip.SetDelayTime(500);

	return 0;
}

void CFloatBar::OnPaint()
{
	CPaintDC dc(this); 
	
	GetClientRect(&m_rcClient);
	if (m_rcClient.IsRectEmpty())
		return;

	DrawBk(&dc);
	DrawIcon(&dc);

	DrawTwinkling(&dc);
}

void CFloatBar::DrawIcon(CDC* pDC)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (ITEMTYPE_ICON == it->nType && 
			it->bEnable )
		{
			::DrawIconEx(pDC->GetSafeHdc(),
				it->rcBtn.left, it->rcBtn.top, it->hIcon,
				it->rcBtn.Width(), it->rcBtn.Height(), 0, NULL, DI_NORMAL);
		}
	}
}

void CFloatBar::SetBkBitmap(int nActiveID, int nUnactiveID, int nLeftWidth, int nRightWidth)
{
	CBitmap bmActive, bmUnactive;
	bmActive.Attach((HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nActiveID), 
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
	bmUnactive.Attach((HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nUnactiveID), 
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));

	BITMAP bm;
	bmActive.GetBitmap(&bm);

	m_nFloatBarHeight = bm.bmHeight + 1;

	CDC dcActive, dcNA;
	dcActive.CreateCompatibleDC(NULL);
	dcActive.SelectObject(&bmActive);
	dcNA.CreateCompatibleDC(NULL);
	dcNA.SelectObject(&bmUnactive);

	for (int i=0; i < 3; i++) {
		m_dcActive[i].DeleteDC();
		m_dcNA[i].DeleteDC();
		m_bmActive[i].DeleteObject();
		m_bmNA[i].DeleteObject();
	}

	m_nLeftWidth = nLeftWidth;
	m_nRightWidth = nRightWidth;
	m_nCenterWidth = bm.bmWidth - nLeftWidth - nRightWidth;

	m_bmActive[0].CreateCompatibleBitmap(&dcActive, nLeftWidth, m_nFloatBarHeight);
	m_bmActive[1].CreateCompatibleBitmap(&dcActive, m_nCenterWidth, m_nFloatBarHeight);
	m_bmActive[2].CreateCompatibleBitmap(&dcActive, nRightWidth, m_nFloatBarHeight);

	m_bmNA[0].CreateCompatibleBitmap(&dcNA, nLeftWidth, m_nFloatBarHeight);
	m_bmNA[1].CreateCompatibleBitmap(&dcNA, m_nCenterWidth, m_nFloatBarHeight);
	m_bmNA[2].CreateCompatibleBitmap(&dcNA, nRightWidth, m_nFloatBarHeight);

	for (int i=0; i < 3; i++) {
		m_dcActive[i].CreateCompatibleDC(NULL);
		m_dcNA[i].CreateCompatibleDC(NULL);
	}

	m_dcActive[0].SelectObject(&m_bmActive[0]);
	m_dcActive[0].BitBlt(0, 0, nLeftWidth, m_nFloatBarHeight,
		&dcActive, 0, 0, SRCCOPY);

	m_dcActive[1].SelectObject(&m_bmActive[1]);
	m_dcActive[1].BitBlt(0, 0, m_nCenterWidth, m_nFloatBarHeight,
		&dcActive, nLeftWidth, 0, SRCCOPY);

	m_dcActive[2].SelectObject(&m_bmActive[2]);
	m_dcActive[2].BitBlt(0, 0, nRightWidth, m_nFloatBarHeight,
		&dcActive, bm.bmWidth - nRightWidth, 0, SRCCOPY);

	m_dcNA[0].SelectObject(&m_bmNA[0]);
	m_dcNA[0].BitBlt(0, 0, nLeftWidth, m_nFloatBarHeight,
		&dcNA, 0, 0, SRCCOPY);

	m_dcNA[1].SelectObject(&m_bmNA[1]);
	m_dcNA[1].BitBlt(0, 0, m_nCenterWidth, m_nFloatBarHeight,
		&dcNA, nLeftWidth, 0, SRCCOPY);

	m_dcNA[2].SelectObject(&m_bmNA[2]);
	m_dcNA[2].BitBlt(0, 0, nRightWidth, m_nFloatBarHeight,
		&dcNA, bm.bmWidth - nRightWidth, 0, SRCCOPY);

	if (m_dcDrawBk.GetSafeHdc() != NULL) {
		m_dcDrawBk.DeleteDC();
		Invalidate();
	}
}

void CFloatBar::DrawBk(CDC* pDC)
{
	if (m_dcDrawBk.GetSafeHdc() == NULL)
		SetBkDC();

	if (m_dcDrawBk.GetSafeHdc() == NULL)
		pDC->FillSolidRect(&m_rcClient, RGB(255,255,255));
	else
		pDC->BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(),
			&m_dcDrawBk, 0, 0, SRCCOPY);

}

void CFloatBar::SetBkDC()
{
	if (m_dcDrawBk.GetSafeHdc() != NULL)
		m_dcDrawBk.DeleteDC();

	CBitmap bmp;
	m_dcDrawBk.CreateCompatibleDC(NULL);
	
	if (m_bActive) {
		bmp.CreateCompatibleBitmap(&m_dcActive[0], m_rcClient.Width(), m_rcClient.Height());
		m_dcDrawBk.SelectObject(&bmp);

		int nCenterWidth = m_rcClient.Width() - m_nLeftWidth - m_nRightWidth;

		m_dcDrawBk.BitBlt(0, 0, m_nLeftWidth, m_rcClient.Height(),
			&m_dcActive[0], 0, 0, SRCCOPY);
		m_dcDrawBk.StretchBlt(m_nLeftWidth, 0, nCenterWidth, m_rcClient.Height(),
			&m_dcActive[1], 0, 0, m_nCenterWidth, m_rcClient.Height(), SRCCOPY);
		m_dcDrawBk.BitBlt(m_nLeftWidth + nCenterWidth - 1, 0, m_nRightWidth, m_rcClient.Height(),
			&m_dcActive[2], 0, 0, SRCCOPY);
	}
	else {
		bmp.CreateCompatibleBitmap(&m_dcNA[0], m_rcClient.Width(), m_rcClient.Height());
		m_dcDrawBk.SelectObject(&bmp);

		int nCenterWidth = m_rcClient.Width() - m_nLeftWidth - m_nRightWidth;

		m_dcDrawBk.BitBlt(0, 0, m_nLeftWidth, m_rcClient.Height(),
			&m_dcNA[0], 0, 0, SRCCOPY);
		m_dcDrawBk.StretchBlt(m_nLeftWidth, 0, nCenterWidth, m_rcClient.Height(),
			&m_dcNA[1], 0, 0, m_nCenterWidth, m_rcClient.Height(), SRCCOPY);
		m_dcDrawBk.BitBlt(m_nLeftWidth + nCenterWidth - 1, 0, m_nRightWidth, m_rcClient.Height(),
			&m_dcNA[2], 0, 0, SRCCOPY);
	}
}

void CFloatBar::DrawTwinkling(CDC* pDC)
{
	if (m_pbmTwinkling == NULL || !m_bShowTwinkling)
		return;

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	dc.SelectObject(m_pbmTwinkling[m_nCurState]);

	pDC->TransparentBlt(
		m_rcTwinkling.left, 
		m_rcTwinkling.top, 
		m_rcTwinkling.Width(), 
		m_rcTwinkling.Height(),
		&dc, 
		0, 
		0, 
		m_rcTwinkling.Width(), 
		m_rcTwinkling.Height(),
		RGB(255,0,255));

	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(m_strTime, m_rcTime, DT_LEFT);
}

void CFloatBar::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	GetClientRect(&m_rcClient);

	if (m_rcClient.IsRectEmpty())
		return;

	SetButtonsPos();
	SetBkDC();
}

void CFloatBar::SetButtonsPos()
{
	GetClientRect(&m_rcClient);
	HideAllBtn();

	CRect rcBtn;

	if (!m_bExpandBar) {
		if (m_BtnShow.pButton->GetSafeHwnd() == NULL)
			return;

		rcBtn.left = HMARGIN;
		rcBtn.top = (m_rcClient.Height() - m_BtnHide.rcBtn.Height()) / 2;
		rcBtn.right = rcBtn.left + m_BtnHide.rcBtn.Width();
		rcBtn.bottom = rcBtn.top + m_BtnHide.rcBtn.Height();
		m_BtnShow.pButton->MoveWindow(&rcBtn);
		m_BtnShow.pButton->ShowWindow(SW_SHOW);

		int nSpace = m_nSeparatorWidth;

		CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
		for (; it != m_arrBarBtn.end(); it++)
		{
			if (!it->bSeparator && it->bNoHide) {
				rcBtn.left = rcBtn.right + nSpace;
				rcBtn.right = rcBtn.left + it->rcBtn.Width();
				rcBtn.top = (m_rcClient.Height() - it->rcBtn.Height()) / 2;
				rcBtn.bottom = rcBtn.top +  it->rcBtn.Height();
				nSpace = m_nBtnSpace;

				it->rcBtn = rcBtn;

				if (ITEMTYPE_BUTTON == it->nType)
				{
				it->pButton->MoveWindow(&rcBtn);
				it->pButton->ShowWindow(SW_SHOW);
				}
				else if (ITEMTYPE_SLIDER == it->nType)
				{
					it->pWnd->MoveWindow(&rcBtn);
					it->pWnd->ShowWindow(SW_SHOW);
				}
				else if (ITEMTYPE_ICON == it->nType)
				{
					it->bEnable = TRUE;
					it->rcBtn = rcBtn;
				}
				else if (ITEMTYPE_TEXT == it->nType)
				{
					it->pWnd->MoveWindow(&rcBtn);
					it->pWnd->ShowWindow(SW_SHOW);
				}
				else
					ASSERT(0);

				continue;
			}

			if (it->bSeparator)
				nSpace = m_nSeparatorWidth;
			else
				nSpace = m_nBtnSpace;
		}
	}
	else {
		if (m_BtnHide.pButton->GetSafeHwnd() == NULL)
			return;

		rcBtn.left = HMARGIN;
		rcBtn.top = (m_rcClient.Height() - m_BtnHide.rcBtn.Height()) / 2;
		rcBtn.right = rcBtn.left + m_BtnHide.rcBtn.Width();
		rcBtn.bottom = rcBtn.top + m_BtnHide.rcBtn.Height();
		m_BtnHide.pButton->MoveWindow(&rcBtn);
		m_BtnHide.pButton->ShowWindow(SW_SHOW);

		int nSpace = m_nSeparatorWidth;
		int nCount = 0;

		CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
		for (; it != m_arrBarBtn.end(); it++)
		{
			if (!it->bSeparator) {
				rcBtn.left = rcBtn.right + nSpace;
				rcBtn.right = rcBtn.left + it->rcBtn.Width();
				rcBtn.top = (m_rcClient.Height() - it->rcBtn.Height()) / 2;
				rcBtn.bottom = rcBtn.top + it->rcBtn.Height();


				it->rcBtn = rcBtn;

				if (ITEMTYPE_BUTTON == it->nType)
				{
				it->pButton->MoveWindow(&rcBtn);
				it->pButton->ShowWindow(SW_SHOW);
				}
				else if (ITEMTYPE_SLIDER == it->nType)
				{
					it->pWnd->MoveWindow(&rcBtn);
					it->pWnd->ShowWindow(SW_SHOW);
				}
				else if (ITEMTYPE_ICON == it->nType)
				{
					it->bEnable = TRUE;
					it->rcBtn = rcBtn;
				}
				else if (ITEMTYPE_TEXT == it->nType)
				{
					it->pWnd->MoveWindow(&rcBtn);
					it->pWnd->ShowWindow(SW_SHOW);
				}
				else
					ASSERT(0);

				nSpace = m_nBtnSpace;
				nCount = 0;
			}
			else {
				nCount ++;
				nSpace = m_nSeparatorWidth * nCount;
			}
		}
	}

	if (m_bShowTwinkling) {
		int nWidth = m_rcTwinkling.Width();
		int nHeight = m_rcTwinkling.Height();

		m_rcTwinkling.left = rcBtn.right + m_nSeparatorWidth;
		m_rcTwinkling.right = m_rcTwinkling.left + nWidth;
		m_rcTwinkling.top = (m_rcClient.Height() - nHeight) / 2;
		m_rcTwinkling.bottom = m_rcTwinkling.top + nHeight;

		nWidth = m_rcTime.Width();
		nHeight = m_rcTime.Height();
		m_rcTime.left = m_rcTwinkling.right + m_nBtnSpace;
		m_rcTime.right = m_rcTime.left + nWidth;
		m_rcTime.top = (m_rcClient.Height() - nHeight) / 2;
		m_rcTime.bottom = m_rcTime.top + nHeight;

		rcBtn = m_rcTime;
	}

//	m_nFloatBarHeight = m_BtnHide.rcBtn.Height() + VMARGIN * 2;
	m_nFloatBarWidth = rcBtn.right + HMARGIN + 1;
}

void CFloatBar::CalcFloatBarSize(int& cx, int& cy)
{
	cx = m_BtnShow.rcBtn.Width() + m_nSeparatorWidth + HMARGIN * 2;	

	BOOL bPreBtn = FALSE;

	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (it->bSeparator) {
			if (bPreBtn) {
				cx += (m_nSeparatorWidth - m_nBtnSpace);
				bPreBtn = FALSE;
			}
			else
				cx += m_nSeparatorWidth;			
		}
		else {
			cx += (it->rcBtn.Width() + m_nBtnSpace);
			bPreBtn = TRUE;
		}
	}

	if (m_nFloatBarHeight == 0) 
		cy = m_BtnShow.rcBtn.Height() + VMARGIN * 2;
	else 
		cy = m_nFloatBarHeight;

	cx -= m_nBtnSpace;
	cx += 1;

	if (m_bShowTwinkling) 
		cx += (m_rcTwinkling.Width() + m_rcTime.Width() + m_nBtnSpace);
}

void CFloatBar::HideAllBtn()
{
	if (m_BtnShow.pButton->GetSafeHwnd() != NULL)
		m_BtnShow.pButton->ShowWindow(SW_HIDE);
	if (m_BtnHide.pButton->GetSafeHwnd() != NULL)
		m_BtnHide.pButton->ShowWindow(SW_HIDE);

	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (ITEMTYPE_BUTTON == it->nType) 
		{
			if (it->pButton && it->pButton->GetSafeHwnd() != NULL)
			it->pButton->ShowWindow(SW_HIDE);
	}
		else if (ITEMTYPE_SLIDER == it->nType)
		{
			if (it->pWnd && it->pWnd->GetSafeHwnd() != NULL)
				it->pWnd->ShowWindow(SW_HIDE);
		}
		else if (ITEMTYPE_ICON == it->nType)
		{
			it->bEnable = FALSE;
		}
	}
}

BOOL CFloatBar::Create(CWnd* pParentWnd)
{
	EnableToolTips(TRUE);
	LPCTSTR pClassName=AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW);

	DWORD dwExStyle = WS_EX_TOOLWINDOW;
	DWORD dwLayout = 0;
	GetProcessDefaultLayout(&dwLayout);
	if (dwLayout == LAYOUT_RTL) {
		dwExStyle = WS_EX_TOOLWINDOW | WS_EX_LAYOUTRTL;
		m_bMirror = TRUE;
	}

	CRect rect(0,0,0,0);
	return CWnd::CreateEx(dwExStyle , 
		pClassName, 
		_T(""),
		WS_POPUP, 
		rect, 
		pParentWnd,
		0);
}

void CFloatBar::OnClose()
{
}

void CFloatBar::ShowFloadBar(BOOL bShow, int x, int y, const CWnd* pWndInsertAfter)
{
	ASSERT(m_hWnd != NULL);

	CalcFloatBarSize(m_nFloatBarWidth, m_nFloatBarHeight);

	m_nAlignFlag = FLOATVAR_ALIGN_NONE;

	int nFlag = SWP_HIDEWINDOW;
	if (bShow)
		nFlag = SWP_SHOWWINDOW;

	SetWindowPos(pWndInsertAfter, 
		x, 
		y, 
		m_nFloatBarWidth, 
		m_nFloatBarHeight, 
		nFlag);
	SetWndRound();

	UpdateWindow();
}

void CFloatBar::ShowFloadBar(BOOL bShow, int nAlign, const CWnd* pWndInsertAfter)
{
	ASSERT(m_hWnd != NULL);
	ASSERT(nAlign != FLOATVAR_ALIGN_NONE);	

	CalcFloatBarSize(m_nFloatBarWidth, m_nFloatBarHeight);

	int nScreenCx = GetSystemMetrics(SM_CXFULLSCREEN);
	int nXPos = 0, nYPos = 0;

	if (m_bMirror) {
		if (nAlign == FLOATBAR_ALIGN_LEFT)
			nAlign = FLOATBAR_ALIGN_RIGHT;
		else if (nAlign == FLOATBAR_ALIGN_RIGHT)
			nAlign = FLOATBAR_ALIGN_LEFT;
	}

	switch (nAlign) {
		case FLOATBAR_ALIGN_LEFT:
			{
				nXPos = 0;
				nYPos = 0;
			}
			break;
		case FLOATBAR_ALIGN_RIGHT:
			{
				nXPos = (nScreenCx - m_nFloatBarWidth);
				nYPos = 0;
			}
			break;
		case FLOATBAR_ALIGN_CENTER:
			{
				nXPos = (nScreenCx - m_nFloatBarWidth) / 2;
				nYPos = 0;
			}
			break;
		default:
			ASSERT(0);
			break;
	}

	m_nAlignFlag = nAlign;

	int nFlag = SWP_HIDEWINDOW;
	if (bShow)
		nFlag = SWP_SHOWWINDOW;

	SetWindowPos(pWndInsertAfter, 
		nXPos, 
		nYPos, 
		m_nFloatBarWidth, 
		m_nFloatBarHeight, 
		nFlag);

	SetWndRound();

	UpdateWindow();
}

void CFloatBar::SetWndRound()
{
	CRgn CaptionRegion;
	GetClientRect(&m_rcClient);
	CaptionRegion.CreateRoundRectRgn(0, 
		0, 
		m_rcClient.Width(), 
		m_rcClient.Height(), 
		ROUNDED_CX, 
		ROUNDED_CY);
	SetWindowRgn((HRGN)CaptionRegion.m_hObject, TRUE);
}

void CFloatBar::SetNotifyWindow(HWND hWnd, UINT uMsg)
{
	m_hNotifyHwnd = hWnd;
	m_uNotifyMsg = uMsg;
}

BOOL CFloatBar::SetShowHideBtn(CBitmap* pbmpShow, 
							   CBitmap* pbmpHide, 
							   CString strShowTip, 
							   CString strHideTip)
{
	InitButtonInfo(&m_BtnShow);
	InitButtonInfo(&m_BtnHide);

	m_BtnShow.bEnable = TRUE;
	m_BtnShow.bNoHide = TRUE;
	m_BtnShow.nID = ID_FLOATBAR_BTN_SHOW;
	m_BtnShow.strTip = strShowTip;
	
	m_BtnHide.bEnable = TRUE;
	m_BtnHide.bNoHide = TRUE;
	m_BtnHide.nID = ID_FLOATBAR_BTN_HIDE;
	m_BtnHide.strTip = strHideTip;

	if (CreateButton(&m_BtnShow, pbmpShow) &&
		CreateButton(&m_BtnHide, pbmpHide))
		return TRUE;

	m_nDefaultbtnCx = m_BtnShow.rcBtn.Height();
	m_nDefaultbtnCy = m_BtnShow.rcBtn.Height();

	return FALSE;
}

BOOL CFloatBar::AddButton(CBitmap* pBmp,
				   UINT nID,
				   CString strTip,
				   BOOL bSeparator,
				   BOOL bNoHide)
{
	BUTTON_INFO btnInfo;
	InitButtonInfo(&btnInfo);

	btnInfo.nID = nID;
	btnInfo.bSeparator = bSeparator;
	if (bSeparator) {
		m_arrBarBtn.push_back(btnInfo);
		return TRUE;
	}

	btnInfo.nType = ITEMTYPE_BUTTON;
	btnInfo.bEnable = TRUE;
	btnInfo.bNoHide = bNoHide;
	btnInfo.strTip = strTip;

	if (!CreateButton(&btnInfo, pBmp)) {
		ReleaseButtonInfo(&btnInfo);
		return FALSE;
	}

	m_arrBarBtn.push_back(btnInfo);

	return TRUE;
}

BOOL CFloatBar::AddButton(UINT nBitmapID,
				   UINT nID,
				   LPCTSTR lpszTip,
				   BOOL bSeparator,
				   BOOL bNoHide)
{
	BUTTON_INFO btnInfo;
	InitButtonInfo(&btnInfo);

	btnInfo.nID = nID;
	btnInfo.bSeparator = bSeparator;
	if (bSeparator) {
		m_arrBarBtn.push_back(btnInfo);
		return TRUE;
	}

	btnInfo.nType = ITEMTYPE_BUTTON;
	btnInfo.bEnable = TRUE;
	btnInfo.bNoHide = bNoHide;
	btnInfo.strTip = lpszTip;

	CBitmap bmBtn;
	if (!bmBtn.Attach((HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nBitmapID), 
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)))
		return FALSE;
	
	if (!CreateButton(&btnInfo, &bmBtn)) {
		ReleaseButtonInfo(&btnInfo);
		bmBtn.Detach();
		return FALSE;
	}

	bmBtn.Detach();

	m_arrBarBtn.push_back(btnInfo);

	return TRUE;
}

BOOL CFloatBar::RemoveButton(UINT nID)
{
	return FALSE;
}

void CFloatBar::InitButtonInfo(PBUTTON_INFO pBtnInfo)
{
	ReleaseButtonInfo(pBtnInfo);

	pBtnInfo->Bitmap[0] = new CBitmap;
	pBtnInfo->Bitmap[1] = new CBitmap;
	pBtnInfo->Bitmap[2] = new CBitmap;
	pBtnInfo->Bitmap[3] = new CBitmap;

	pBtnInfo->pButton = new CBarButton;

	pBtnInfo->nID = 0;
	pBtnInfo->bEnable = TRUE;
	pBtnInfo->bSeparator = FALSE;
	pBtnInfo->bNoHide = FALSE;
	pBtnInfo->rcBtn = CRect(0,0,0,0);

	pBtnInfo->strTip = _T("");
}

void CFloatBar::ReleaseButtonInfo(PBUTTON_INFO pBtnInfo)
{
	for (int i=0; i < 4; i++) 
	{
		if (pBtnInfo->Bitmap[i] != NULL) {
			pBtnInfo->Bitmap[i]->DeleteObject();
			delete pBtnInfo->Bitmap[i];
			pBtnInfo->Bitmap[i] = NULL;
		}
	}

	if (pBtnInfo->pButton != NULL) {
		delete pBtnInfo->pButton;
		pBtnInfo->pButton = NULL;
	}

	if (pBtnInfo->pWnd != NULL) {
		delete pBtnInfo->pWnd;
		pBtnInfo->pWnd = NULL;
	}
}

BOOL CFloatBar::CreateButton(PBUTTON_INFO pBtnInfo, CBitmap* pBmpSrc)
{
	if (pBmpSrc == NULL) {
		ASSERT(pBmpSrc);
		return FALSE;
	}

	BITMAP bmpInfo;
	pBmpSrc->GetBitmap(&bmpInfo);
			
	pBtnInfo->rcBtn.left = 0;
	pBtnInfo->rcBtn.top = 0;
	pBtnInfo->rcBtn.right = bmpInfo.bmWidth;
	pBtnInfo->rcBtn.bottom = bmpInfo.bmHeight / 4;

	CDC dcSrc, dcDest;
	dcSrc.CreateCompatibleDC(NULL);
	dcSrc.SelectObject(pBmpSrc);
	dcDest.CreateCompatibleDC(NULL);

	for (int i=0; i<4; i++) {
		CBitmap* pOldBmp = NULL;
		pBtnInfo->Bitmap[i]->CreateCompatibleBitmap(&dcSrc, 
												pBtnInfo->rcBtn.right, 
												pBtnInfo->rcBtn.bottom);
		pOldBmp = dcDest.SelectObject(pBtnInfo->Bitmap[i]);
		dcDest.BitBlt(0, 
					0, 
					pBtnInfo->rcBtn.right, 
					pBtnInfo->rcBtn.bottom, 
					&dcSrc, 
					0, 
					pBtnInfo->rcBtn.bottom * i, 
					SRCCOPY);
		dcDest.SelectObject(pOldBmp);
	}

	if (!pBtnInfo->pButton->Create(_T(""), WS_CHILD, pBtnInfo->rcBtn, this, pBtnInfo->nID))
		return FALSE;

	pBtnInfo->pButton->SetNotifyWindow(GetSafeHwnd(), ID_FLOATBAR_BTN_CLICKED);
	pBtnInfo->pButton->SetBitmap(*pBtnInfo->Bitmap[0],
								*pBtnInfo->Bitmap[1],
								*pBtnInfo->Bitmap[2],
								*pBtnInfo->Bitmap[3]);
	m_wndTooltip.AddTool(pBtnInfo->pButton, pBtnInfo->strTip);

	return TRUE;
}

BOOL CFloatBar::AddVolumeBar(
		UINT nBarBitmapID, 
		UINT nID, 
		int nRange, 
		int nLevelMax, 
		int nElapse,
		LPCTSTR lpszTip,
		int nSliderLength,
		BOOL bHorizontal,
		BOOL bNoHide)
{
	BUTTON_INFO btnInfo;

	btnInfo.nType = ITEMTYPE_SLIDER;
	btnInfo.nID = nID;
	btnInfo.bEnable = TRUE;
	btnInfo.bSeparator = FALSE;
	btnInfo.bNoHide = bNoHide;
	btnInfo.strTip = lpszTip;

	btnInfo.pWnd = new CVolumeSlider;
	ASSERT(btnInfo.pWnd);
	if (!btnInfo.pWnd)
		return FALSE;

	CBitmap bmBar;
	if (!bmBar.Attach((HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nBarBitmapID), 
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)))
		return FALSE;
	BITMAP bm;
	bmBar.GetBitmap(&bm);
	bmBar.Detach();

	if (bHorizontal)
	{
		int nHeight = bm.bmHeight / 4;
		if (nHeight < 10)
			nHeight = 10;
		CRect rc(0,0,nSliderLength,nHeight);
		btnInfo.rcBtn = rc;
	}
	else
	{
		int nWidth = bm.bmWidth;
		if (nWidth < 10)
			nWidth = 10;

		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rc(0,0,nWidth,nSliderLength);
		btnInfo.rcBtn = rc;
	}

	CVolumeSlider* pSlider = (CVolumeSlider*)btnInfo.pWnd;
	pSlider->Create(btnInfo.rcBtn, this, nID, 0, bHorizontal);
	pSlider->SetSliderBar(nBarBitmapID, 10);
	pSlider->SetPosChangedNotify(ID_FLOATBAR_SLIDER_POS);
	pSlider->SetRange(nRange);
	pSlider->SetLevelMax(nLevelMax);
	pSlider->SetLevelElapse(nElapse);
	pSlider->SetPos(0);

	m_arrBarBtn.push_back(btnInfo);

	m_wndTooltip.AddTool(btnInfo.pWnd, btnInfo.strTip);

	return TRUE;
}

BOOL CFloatBar::SetVolumeBarPos(UINT nID, int nPos)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (it->nID == nID)
		{
			if (ITEMTYPE_SLIDER == it->nType && it->pWnd)
			{
				((CVolumeSlider*)(it->pWnd))->SetPos(nPos);
				return TRUE;
			}
		}
	}

	ASSERT(0);
	return FALSE;
}

int CFloatBar::GetVulumeBarPos(UINT nID)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (it->nID == nID)
		{
			if (ITEMTYPE_SLIDER == it->nType && it->pWnd)
			{
				return ((CVolumeSlider*)(it->pWnd))->GetPos();
			}
		}
	}

	ASSERT(0);
	return 0;
}

BOOL CFloatBar::SetVolumeBarLevel(UINT nID, int nLevel)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (it->nID == nID)
		{
			if (ITEMTYPE_SLIDER == it->nType && it->pWnd)
			{
				((CVolumeSlider*)it->pWnd)->SetLevel(nLevel);
				return TRUE;
			}
		}
	}

	ASSERT(0);
	return FALSE;
}

BOOL CFloatBar::OnSliderPosChanged(UINT nID, int nPos)
{
	return FALSE;
}

LRESULT CFloatBar::OnFloatBarSliderPosChanged(WPARAM wParam, LPARAM lParam)
{
	if (!OnSliderPosChanged((UINT)wParam, (int)lParam))
		::PostMessage(m_hNotifyHwnd, m_uNotifyMsg, wParam, lParam);

	return 0;
}

LRESULT CFloatBar::OnFloatBarBtnCliked(WPARAM wParam, LPARAM lParam)
{
	if (ID_FLOATBAR_BTN_SHOW == wParam) {
		m_bExpandBar = TRUE;
		SetButtonsPos();
		AnimateExpand(m_nFloatBarWidth, m_bExpandBar);
		SetWndRound();
	}
	else if (ID_FLOATBAR_BTN_HIDE == wParam){
		m_bExpandBar = FALSE;
		SetButtonsPos();
		AnimateExpand(m_nFloatBarWidth, m_bExpandBar);
		SetWndRound();
	}
	else {
		::PostMessage(m_hNotifyHwnd, m_uNotifyMsg, wParam, lParam);
	}

	return 0;
}

BOOL CFloatBar::AddIcon(UINT nID, HICON hIcon, SIZE size, BOOL bNoHide)
{
	ASSERT(hIcon && size.cx && size.cy);
	if (hIcon == 0 || size.cx == 0 || size.cy == 0)
		return FALSE;

	BUTTON_INFO btnInfo;

	btnInfo.nID = nID;
	btnInfo.hIcon = hIcon;
	btnInfo.rcBtn = CRect(0,0,size.cx,size.cy);
	btnInfo.bNoHide = bNoHide;
	btnInfo.bSeparator = FALSE;
	btnInfo.nType = ITEMTYPE_ICON;
	btnInfo.bEnable = TRUE;

	m_arrBarBtn.push_back(btnInfo);

	return TRUE;
}

BOOL CFloatBar::SetIcon(UINT nID, HICON hNewIcon, SIZE size)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it ++)
	{
		if (it->nID == nID)
		{
			if (it->nType != ITEMTYPE_ICON)
				return FALSE;

			it->hIcon = hNewIcon;

			if (it->rcBtn.Width() != size.cx || it->rcBtn.Height() != size.cy)
			{
				it->rcBtn = CRect(0,0,size.cx,size.cy);
				SetButtonsPos();
				Invalidate();
			}
			else
			{
				InvalidateRect(it->rcBtn);
			}
			
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFloatBar::AddLabel(UINT nID, LPCTSTR lpszText, int nWidth, CFont* pFont, COLORREF crText, BOOL bNoHide)
{
	if (lpszText == NULL || nWidth <= 0)
	{
		ASSERT(0);
		return FALSE;
	}

	CSize size = CRollLabel::GetTextSize(lpszText, pFont);

	BUTTON_INFO btnInfo;
	btnInfo.nID = nID;
	btnInfo.rcBtn = CRect(0,0,nWidth,size.cy);
	btnInfo.bNoHide = bNoHide;
	btnInfo.bSeparator = FALSE;
	btnInfo.nType = ITEMTYPE_TEXT;
	btnInfo.bEnable = TRUE;

	CRollLabel* pLabel = new CRollLabel;
	pLabel->Create(NULL, NULL, WS_CHILD, btnInfo.rcBtn, this, nID);
	pLabel->SetTextFont(pFont);
	pLabel->SetText(lpszText, crText);
	pLabel->SetBkColor(TRUE);

	btnInfo.pWnd = pLabel;

	m_arrBarBtn.push_back(btnInfo);

	return TRUE;
}

BOOL CFloatBar::SetLabelText(UINT nID, LPCTSTR lpszText)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it ++)
	{
		if (it->nID == nID)
		{
			if (it->nType == ITEMTYPE_TEXT && it->pWnd)
			{
				((CRollLabel*)it->pWnd)->SetText(lpszText);

				return TRUE;
			}

			return FALSE;
		}
	}

	return FALSE;
}

CString CFloatBar::GetLabelText(UINT nID)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it ++)
	{
		if (it->nID == nID)
		{
			if (it->nType == ITEMTYPE_TEXT && it->pWnd)
			{
				return ((CRollLabel*)it->pWnd)->GetText();
			}

			ASSERT(0);
			return _T("");
		}
	}

	ASSERT(0);
	return _T("");
}

BOOL CFloatBar::SetLabelColor(UINT nID, COLORREF crText)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it ++)
	{
		if (it->nID == nID)
		{
			if (it->nType == ITEMTYPE_TEXT && it->pWnd)
			{
				((CRollLabel*)it->pWnd)->SetTextColor(crText);

				return TRUE;
			}

			ASSERT(0);
			return FALSE;
		}
	}

	ASSERT(0);
	return FALSE;
}

COLORREF CFloatBar::GetLabelColor(UINT nID)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it ++)
	{
		if (it->nID == nID)
		{
			if (it->nType == ITEMTYPE_TEXT && it->pWnd)
			{
				return ((CRollLabel*)it->pWnd)->GetTextColor();
			}

			ASSERT(0);
			return 0;
		}
	}

	ASSERT(0);
	return 0;
}

BOOL CFloatBar::SetLabelFont(UINT nID, CFont* pFont)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it ++)
	{
		if (it->nID == nID)
		{
			if (it->nType == ITEMTYPE_TEXT && it->pWnd)
			{
				return ((CRollLabel*)it->pWnd)->SetTextFont(pFont);
			}

			ASSERT(0);
			return FALSE;
		}
	}

	ASSERT(0);
	return FALSE;
}

CFont* CFloatBar::GetLabelTextFont(UINT nID)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it ++)
	{
		if (it->nID == nID)
		{
			if (it->nType == ITEMTYPE_TEXT && it->pWnd)
			{
				return ((CRollLabel*)it->pWnd)->GetTextFont();
			}

			ASSERT(0);
			return NULL;
		}
	}

	ASSERT(0);
	return NULL;
}

BOOL CFloatBar::SetLabelRollRate(UINT nID, int nMilleSec)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it ++)
	{
		if (it->nID == nID)
		{
			if (it->nType == ITEMTYPE_TEXT && it->pWnd)
			{
				((CRollLabel*)it->pWnd)->SetRollElapse(nMilleSec);
				return TRUE;
			}

			ASSERT(0);
			return FALSE;
		}
	}

	ASSERT(0);
	return FALSE;
}

int CFloatBar::GetLabelRollRate(UINT nID)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it ++)
	{
		if (it->nID == nID)
		{
			if (it->nType == ITEMTYPE_TEXT && it->pWnd)
			{
				return ((CRollLabel*)it->pWnd)->GetRollElapse();
			}

			ASSERT(0);
			return -1;
		}
	}

	ASSERT(0);
	return -1;
}

void CFloatBar::AnimateExpand(int nCx, BOOL bExpand)
{
	MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(MonitorFromWindow(GetSafeHwnd(), MONITOR_DEFAULTTONEAREST), &mi);

	CRect rect;
	GetWindowRect(&rect);

	int nPatch = 10;
	int nXPos = 0, nYPos = 0;
	if (m_bMirror) {
		nXPos = rect.right - nCx;
		nYPos = rect.top;
	}
	else {
		nXPos = rect.left;
		nYPos = rect.top;
	}

	if (bExpand) {
		if (nCx > rect.Width()) 
		{
			int nOffset = (nCx - rect.Width()) / nPatch;
			for (int i=0; i<nPatch; i++) {
				if (m_bMirror)
					rect.left -= nOffset;
				else
					rect.right += nOffset;

				SetWindowPos(NULL, rect.left, rect.top, 
					rect.Width(), rect.Height(), SWP_SHOWWINDOW);

				Sleep(5);
			}

			int nScreenCx = GetSystemMetrics(SM_CXFULLSCREEN) + mi.rcMonitor.left;
			if (m_bMirror) {
				if (rect.left < 0)
					nXPos = 0;
			}
			else {
				if (nXPos + nCx > nScreenCx)
					nXPos = nScreenCx - nCx;
			}

			SetWindowPos(NULL, nXPos, nYPos, nCx, rect.Height(), SWP_SHOWWINDOW);
		}
	}
	else {
		if (nCx < rect.Width()) 
		{
			int nOffset = (rect.Width() - nCx) / nPatch;
			for (int i=0; i<nPatch; i++) {
				if (m_bMirror)
					rect.left += nOffset;
				else
					rect.right -= nOffset;

				SetWindowPos(NULL, rect.left, rect.top, 
					rect.Width(), rect.Height(), SWP_SHOWWINDOW);

				Sleep(5);
			}
			SetWindowPos(NULL, nXPos, nYPos, nCx, rect.Height(), SWP_SHOWWINDOW);
		}
	}
}

void CFloatBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bEnableDrag) {
		m_ptOldPoint = point;
		m_ptDown = point;
		ClientToScreen(&m_ptOldPoint);
		m_bLBtnDown = TRUE;
		SetCapture();
	}

	m_bActive = TRUE;
	SetBkDC();
	Invalidate();

	CWnd::OnLButtonDown(nFlags, point);
}

void CFloatBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bLBtnDown) {
		ReleaseCapture();
		m_bLBtnDown =FALSE;
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CFloatBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bLBtnDown) 
	{
		if (m_bEnableAutoHide)
		{
			if (m_dwHideState != AUTOHIDE_STATE_SHOW ||
				m_dwHideState != AUTOHIDE_STATE_SHOWING)
			{
				AutoHideBar(FALSE);
			}
		}
	}
	else
	{
		DragMoving(point);
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CFloatBar::DragMoving(CPoint point)
{
	MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(MonitorFromWindow(GetParent()->m_hWnd, MONITOR_DEFAULTTONEAREST), &mi);

	CRect rcDesktop;
    ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);

	int nScreenCx = rcDesktop.Width() + mi.rcMonitor.left;
	int nSrceenCy = rcDesktop.Height() + mi.rcMonitor.top;

	ClientToScreen(&point);
	int cx = point.x - m_ptOldPoint.x;
	int cy = point.y - m_ptOldPoint.y;

	CRect rect;
	GetWindowRect(&rect);

	int nWidth = rect.Width();
	int nHeight = rect.Height();

	rect.left += cx;
	rect.top += cy;

	if (rect.left < 0)
		rect.left = 0;
	if (rect.top < 0)
		rect.top = 0;
	if (rect.right + cx > nScreenCx)
		rect.left = nScreenCx - nWidth;
	if (rect.bottom + cy > nSrceenCy)
		rect.top = nSrceenCy - nHeight;

	SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE);
	
	GetWindowRect(&rect);

	if (m_bMirror) 
		m_ptOldPoint.x = rect.left + rect.Width() - m_ptDown.x;
	else
		m_ptOldPoint.x = rect.left + m_ptDown.x;
		

	m_ptOldPoint.y = rect.top + m_ptDown.y;
}

void CFloatBar::OnCaptureChanged(CWnd *pWnd)
{
	if(m_bLBtnDown) {
		ReleaseCapture();
		m_bLBtnDown = FALSE;
	}

	CWnd::OnCaptureChanged(pWnd);
}

BOOL CFloatBar::IsEnableDrag()
{
	return m_bEnableDrag;
}

void CFloatBar::EnableDrag(BOOL bEnable)
{
	m_bEnableDrag = bEnable;
}

BOOL CFloatBar::IsButtonCheck(UINT nID)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (it->nID == nID)
			return it->pButton->GetCheck();
	}

	ASSERT(0);
	return FALSE;
}

void CFloatBar::SetButtonCheck(UINT nID, BOOL bCheck)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (it->nID == nID) {
			it->pButton->SetCheck(bCheck);
			break;
		}
	}
}

BOOL CFloatBar::IsButtonEnable(UINT nID)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (it->nID == nID)
			return it->pButton->IsWindowEnabled();
	}

	ASSERT(0);
	return FALSE;
}

void CFloatBar::EnableButton(UINT nID, BOOL bEnable)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (it->nID == nID) {
			it->pButton->EnableWindow(bEnable);
			it->pButton->Invalidate();
			return;
		}
	}
}

BOOL CFloatBar::SetButtonBitmap(UINT nID, CBitmap* pBmp)
{
	if (pBmp == NULL)
		return FALSE;

	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (it->nID == nID) 
		{
			ParseBitmap(pBmp,
				it->Bitmap[0],
				it->Bitmap[1],
				it->Bitmap[2],
				it->Bitmap[3]);
			it->pButton->SetBitmap(*it->Bitmap[0],
									*it->Bitmap[1],
									*it->Bitmap[2],
									*it->Bitmap[3]);

			if (GetSafeHwnd() != NULL)
				Invalidate();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFloatBar::ParseBitmap(CBitmap* pbmSrc, CBitmap* pbmDest, int nNums)
{
	ASSERT(pbmSrc != NULL && nNums > 0 && pbmDest != NULL);
	if (pbmSrc == NULL || nNums <= 0 || pbmDest == NULL)
		return FALSE;

	BITMAP bitmap;
	pbmSrc->GetBitmap(&bitmap);

	int nCx = bitmap.bmWidth;
	int nCy = bitmap.bmHeight / nNums;

	CDC dcSrc, dcDest;
	dcSrc.CreateCompatibleDC(NULL);
	dcSrc.SelectObject(pbmSrc);
	dcDest.CreateCompatibleDC(NULL);

	for (int i=0; i < nNums; i++) {
		CBitmap* pOldBmp = NULL;
		pbmDest[i].CreateCompatibleBitmap(&dcSrc, nCx, nCy);
		pOldBmp = dcDest.SelectObject(&pbmDest[i]);
		dcDest.BitBlt(0, 0, nCx, nCy, &dcSrc, 0, nCy * i, SRCCOPY);
		dcDest.SelectObject(pOldBmp);
	}

	return TRUE;
}

BOOL CFloatBar::ParseBitmap(CBitmap* pbmSrc, 
		CBitmap*& pbmNormal,
		CBitmap*& pbmHover,
		CBitmap*& pbmDown,
		CBitmap*& pbmDisable)
{
	ASSERT(pbmSrc != NULL);
	if (pbmSrc == NULL)
		return FALSE;

	BITMAP bitmap;
	pbmSrc->GetBitmap(&bitmap);

	int nCx = bitmap.bmWidth;
	int nCy = bitmap.bmHeight / 4;

	CBitmap* pOldBmp = NULL;
	CDC dcSrc, dcDest;
	dcSrc.CreateCompatibleDC(NULL);
	dcSrc.SelectObject(pbmSrc);
	dcDest.CreateCompatibleDC(NULL);
	
	pbmNormal->DeleteObject();
	pbmHover->DeleteObject();
	pbmDown->DeleteObject();
	pbmDisable->DeleteObject();

	pbmNormal->CreateCompatibleBitmap(&dcSrc, nCx, nCy);
	pbmHover->CreateCompatibleBitmap(&dcSrc, nCx, nCy);
	pbmDown->CreateCompatibleBitmap(&dcSrc, nCx, nCy);
	pbmDisable->CreateCompatibleBitmap(&dcSrc, nCx, nCy);
	
	pOldBmp = dcDest.SelectObject(pbmNormal);
	dcDest.BitBlt(0, 0, nCx, nCy, &dcSrc, 0, 0,	SRCCOPY);
	dcDest.SelectObject(pOldBmp);

	dcDest.SelectObject(pbmHover);
	dcDest.BitBlt(0, 0, nCx, nCy, &dcSrc, 0, nCy,	SRCCOPY);
	dcDest.SelectObject(pOldBmp);

	dcDest.SelectObject(pbmDown);
	dcDest.BitBlt(0, 0, nCx, nCy, &dcSrc, 0, nCy * 2, SRCCOPY);
	dcDest.SelectObject(pOldBmp);

	dcDest.SelectObject(pbmDisable);
	dcDest.BitBlt(0, 0, nCx, nCy, &dcSrc, 0, nCy * 3, SRCCOPY);
	dcDest.SelectObject(pOldBmp);

	return TRUE;
}

BOOL CFloatBar::ParseBitmap(CBitmap* pbmSrc, CBitmap* pbmDest[4])
{
	return ParseBitmap(pbmSrc, pbmDest[0], pbmDest[1], pbmDest[2], pbmDest[3]);
}

#define SNAP_SIZE 6

void CFloatBar::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanging(lpwndpos);

	if (m_dwHideState == AUTOHIDE_STATE_SHOWING ||
		m_dwHideState == AUTOHIDE_STATE_HIDING)
	{
		return;
	}

	CRect rcWork;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);

	if (abs(lpwndpos->x) <= (rcWork.left + SNAP_SIZE))
	{
		lpwndpos->x = rcWork.left;
	}
	else if ((lpwndpos->x + lpwndpos->cx) >= (rcWork.right - SNAP_SIZE) &&
				(lpwndpos->x + lpwndpos->cx) <= (rcWork.right + SNAP_SIZE))
	{
		lpwndpos->x = rcWork.right - lpwndpos->cx;
	}

	if (abs( lpwndpos->y) <= (rcWork.top + SNAP_SIZE ))
	{
		lpwndpos->y = rcWork.top;
	}
	else if ((lpwndpos->y + lpwndpos->cy) >= (rcWork.bottom - SNAP_SIZE) &&
				(lpwndpos->y + lpwndpos->cy) <= (rcWork.bottom + SNAP_SIZE))
	{
		lpwndpos->y = rcWork.bottom-lpwndpos->cy;
	}
}

LRESULT CFloatBar::OnDisplayChanged(WPARAM wParam, LPARAM lParam)
{
	MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(MonitorFromWindow(GetParent()->GetSafeHwnd(), MONITOR_DEFAULTTONEAREST), &mi);

	int nScreenCx = GetSystemMetrics(SM_CXFULLSCREEN) + mi.rcMonitor.left;
	int nSrceenCy = GetSystemMetrics(SM_CYFULLSCREEN) + mi.rcMonitor.top;

	CRect rect;
	GetWindowRect(&rect);

	int nWidth = rect.Width();
	int nHeight = rect.Height();

	switch (m_nAlignFlag) 
	{
		case FLOATBAR_ALIGN_LEFT:
			{
				rect.left = 0;
				rect.top = 0;
			}
			break;
		case FLOATBAR_ALIGN_RIGHT:
			{
				rect.left = nScreenCx - nWidth;
				rect.top = 0;
			}
			break;
		case FLOATBAR_ALIGN_CENTER:
			{
				rect.left = (nScreenCx - nWidth) / 2;
				rect.top = 0;
			}
			break;
		default:
			{
				if (rect.right > nScreenCx)
					rect.left = nScreenCx - nWidth;
				if (rect.bottom > nSrceenCy)
					rect.top = nSrceenCy - nHeight;
			}
			break;
	}

	SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	ShowBarDirect();

	return 0;
}

void CFloatBar::ResetBarPosition(HMONITOR hMonitor)
{
	MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);

	CRect rc(0,0,0,0);
	GetWindowRect(&rc);	

	int nLeft = 0;

	if (m_nAlignFlag == FLOATVAR_ALIGN_NONE) {
		CRect rect;
		GetWindowRect(&rect);
		nLeft = rect.left;
	}
	if (m_nAlignFlag == FLOATBAR_ALIGN_LEFT)
		nLeft = 0;
	else if (m_nAlignFlag == FLOATBAR_ALIGN_RIGHT)
		nLeft = mi.rcMonitor.right - mi.rcMonitor.left - rc.Width();
	else if (m_nAlignFlag == FLOATBAR_ALIGN_CENTER)
		nLeft = (mi.rcMonitor.right - mi.rcMonitor.left - rc.Width()) / 2;
	
	if (nLeft < 0)
		nLeft = 0;

	nLeft += mi.rcMonitor.left;

	SetWindowPos(NULL, nLeft, mi.rcMonitor.top, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
}

void CFloatBar::SetBarPosition(HMONITOR hMonitor)
{
	MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);

	CRect rc(0,0,0,0);
	GetWindowRect(&rc);	

	rc.left += mi.rcMonitor.left;
	rc.top += mi.rcMonitor.top;

	SetWindowPos(NULL, rc.left, rc.top, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
}

void CFloatBar::GetButtonRect(UINT nID, RECT* pRect)
{
	CFloatBarBtnArray::iterator it = m_arrBarBtn.begin();
	for (; it != m_arrBarBtn.end(); it++)
	{
		if (it->nID == nID) {
			pRect->left = it->rcBtn.left;
			pRect->top = it->rcBtn.top;
			pRect->right = it->rcBtn.right;
			pRect->bottom = it->rcBtn.bottom;

			ClientToScreen(pRect);
			
			return;
		}
	}
}

BOOL CFloatBar::AddTwinkling(CBitmap* pBmp, int nStateNums)
{
	if (m_pbmTwinkling) {
		delete []m_pbmTwinkling;
		m_pbmTwinkling = NULL;
	}

	m_nStateNums = nStateNums;
	m_pbmTwinkling = new CBitmap[nStateNums];

	BOOL bRet = ParseBitmap(pBmp, m_pbmTwinkling, nStateNums);
	if (bRet) {
		BITMAP bmInfo;
		m_pbmTwinkling[0].GetBitmap(&bmInfo);
		m_rcTwinkling.left = 0;
		m_rcTwinkling.top = 0;
		m_rcTwinkling.right = bmInfo.bmWidth;
		m_rcTwinkling.bottom = bmInfo.bmHeight;
	}

	m_strTime = _T("00:00:00");
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	CSize size = dc.GetTextExtent(m_strTime);
	m_rcTime.left = 0;
	m_rcTime.top = 0;
	m_rcTime.right = size.cx;
	m_rcTime.bottom = size.cy;

	return bRet;
}

void CFloatBar::StartTwinkling(int nFreq)
{
	ASSERT(nFreq > 0);

	m_nFrequency = nFreq;
	if (m_nFrequency < 200)
		m_nFrequency = 200;

	SetTimer(ID_TIMER_TWINKLING, m_nFrequency, NULL);
	m_bTimerOn = TRUE;
	m_dwOldTime = GetTickCount();
	m_dwEscapeTime = 0;

	m_bShowTwinkling = TRUE;
	SetButtonsPos();
	SetWindowPos(
		NULL, 
		-1, 
		-1,
		m_nFloatBarWidth, 
		m_nFloatBarHeight, 
		SWP_NOMOVE);
	SetWndRound();
}

void CFloatBar::PauseTwinkling(BOOL bPause)
{
	if (!bPause && !m_bTimerOn) {
		m_dwOldTime = ::GetTickCount();
		SetTimer(ID_TIMER_TWINKLING, m_nFrequency, NULL);
		m_bTimerOn = TRUE;
	}
	else if (m_bTimerOn) {
		KillTimer(ID_TIMER_TWINKLING);
		m_bTimerOn = FALSE;
	}
	
	m_nCurState = 0;
	Invalidate();
}

void CFloatBar::StopTwinkling()
{
	if (m_bTimerOn) {
		KillTimer(ID_TIMER_TWINKLING);
		m_bTimerOn = FALSE;
	}

	m_dwEscapeTime = 0;
	m_dwOldTime = 0;
	m_bShowTwinkling = FALSE;
	SetButtonsPos();
	SetWindowPos(
		NULL, 
		-1, 
		-1,
		m_nFloatBarWidth, 
		m_nFloatBarHeight, 
		SWP_NOMOVE);
	SetWndRound();
}

void CFloatBar::OnTimer(UINT_PTR nIDEvent)
{
	if (ID_TIMER_TWINKLING == nIDEvent) 
	{
		m_nCurState ++;
		if (m_nCurState >= m_nStateNums)
			m_nCurState = 0;

		DWORD dwTime = GetTickCount();
		m_dwEscapeTime += (dwTime - m_dwOldTime);
		m_dwOldTime = dwTime;
		
		WORD wHour=0, wMinute=0, wSecond=0;
		wSecond = (WORD)(m_dwEscapeTime / 1000) % 60;
		wMinute = (WORD)((m_dwEscapeTime / 1000) / 60) % 60;
		wHour = (WORD)(m_dwEscapeTime / 1000) / 3600;

		m_strTime.Format(_T("%02d:%02d:%02d"), wHour, wMinute, wSecond);

		CRect rect;
		rect = m_rcClient;
		rect.left = m_rcTwinkling.left;
		rect.right = m_rcTime.right;

		InvalidateRect(rect);
	}
	else if (ID_TIMER_HIDE == nIDEvent)
	{
		CRect rc;
		GetWindowRect(rc);

		if (m_bHideBar)
		{
			m_nHideTopPos -= FLOATBAR_HIDE_INCREASE;

			if (rc.Height() - abs(m_nHideTopPos) <= FLOATBAR_HIDE_REMAIN_HEIGHT)
			{				
				m_dwHideState = AUTOHIDE_STATE_HIDE;
				KillTimer(ID_TIMER_HIDE);
			}
			else
			{
				m_dwHideState = AUTOHIDE_STATE_HIDING;
			}
		}
		else
		{
			m_nHideTopPos += FLOATBAR_HIDE_INCREASE;

			if (m_nHideTopPos >= 0)
			{
				m_nHideTopPos = 0;
				KillTimer(ID_TIMER_HIDE);
				m_dwHideState = AUTOHIDE_STATE_SHOW;
			}
			else
			{
				m_dwHideState = AUTOHIDE_STATE_SHOWING;
			}
		}

		SetWindowPos(NULL, rc.left, m_nHideTopPos, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
	else if (ID_TIMER_TRACE_MOUSE == nIDEvent)
	{
		if (!IsWindowVisible())
			return;

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		if (!m_rcClient.PtInRect(pt))
		{		
			if (m_dwHideState == AUTOHIDE_STATE_SHOW)
			{
				if (IsBarDocked())
					AutoHideBar(TRUE);
			}
		}
	}

	CWnd::OnTimer(nIDEvent);
}

void CFloatBar::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	m_bActive = FALSE;
	SetBkDC();
	Invalidate();
}

BOOL CFloatBar::PreTranslateMessage(MSG* pMsg)
{
	m_wndTooltip.RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}

void CFloatBar::SetBarAutoHide(BOOL bEnable)
{
	m_bEnableAutoHide = bEnable;

	if (m_bEnableAutoHide)
	{
		SetTimer(ID_TIMER_TRACE_MOUSE, TIMER_ELAPSE_TRACE_MOUSE, NULL);
	}
	else 
	{
		KillTimer(ID_TIMER_TRACE_MOUSE);
	}
	
	ShowBarDirect();
}

BOOL CFloatBar::IsBarAutoHide()
{
	return m_bEnableAutoHide;
}

void CFloatBar::AutoHideBar(BOOL bHide)
{
	m_bHideBar = bHide;

	if (m_bHideBar)
	{
		if (m_dwHideState != AUTOHIDE_STATE_SHOW)
			return;

		m_dwHideState = AUTOHIDE_STATE_HIDING;
		m_nHideTopPos = 0;	
	}
	else
	{
		if (m_dwHideState == AUTOHIDE_STATE_SHOW ||
			m_dwHideState != AUTOHIDE_STATE_HIDE)
			return;

		m_nHideTopPos = -(m_rcClient.Height() - FLOATBAR_HIDE_REMAIN_HEIGHT);
	}
	
	SetTimer(ID_TIMER_HIDE, TIMER_ELAPSE_HIDE, NULL);
}

void CFloatBar::ShowBarDirect()
{
	if (m_dwHideState == AUTOHIDE_STATE_SHOW)
		return;

	CRect rc;
	GetWindowRect(rc);

	KillTimer(ID_TIMER_HIDE);
	m_dwHideState = AUTOHIDE_STATE_SHOW;
	SetWindowPos(NULL, rc.left, 0, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
}

BOOL CFloatBar::IsBarDocked()
{
	CRect rc;
	GetWindowRect(rc);
	
	return (rc.top == 0);
}

void CFloatBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		ShowBarDirect();
	}

	CWnd::OnShowWindow(bShow, nStatus);
}