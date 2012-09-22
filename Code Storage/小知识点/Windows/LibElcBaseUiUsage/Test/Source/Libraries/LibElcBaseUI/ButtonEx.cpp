// ButtonEx.cpp : implementation file
//

#include "stdafx.h"
#include "ButtonEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_AUTO_REPEAT	100

#pragma warning(disable : 4244)

/////////////////////////////////////////////////////////////////////////////
// CButtonEx

CButtonEx::CButtonEx()
{
	m_bMouseOnButton = FALSE;

	FreeResources(FALSE);

	m_nAlign = ALIGN_HORIZ; 
	m_bIsFlat = TRUE; 
	m_bDrawBorder = TRUE; 
	m_bDrawFlatFocus = FALSE;

	m_bIsCheckBox = FALSE;
	m_nCheck = 0;

	m_hCursor = NULL;

	m_bAutoRepeat = FALSE;
	m_dwPeriodAutoRepeat = 100;

	m_uIdRepeatTimer = 0;

	SetDefaultColors(FALSE);
}

CButtonEx::~CButtonEx()
{
	FreeResources();

	if (m_hCursor != NULL) 
		::DestroyCursor(m_hCursor);
}

BEGIN_MESSAGE_MAP(CButtonEx, CButton)
	//{{AFX_MSG_MAP(CButtonEx)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CAPTURECHANGED()
	ON_WM_SETCURSOR()
	ON_WM_ENABLE()
	ON_WM_ACTIVATE()
	ON_WM_CANCELMODE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonEx Attributes

void CButtonEx::SetAlign(int nAlign)
{
	switch (nAlign) {    
		case ALIGN_HORIZ:
		case ALIGN_HORIZ_RIGHT:
		case ALIGN_VERT:
			m_nAlign = nAlign;
			break;
	}
	Invalidate();
}

int CButtonEx::GetAlign()
{
	return m_nAlign;
}

BOOL CButtonEx::GetColor(BYTE bColorIndex, COLORREF& clrColor)
{
	if (bColorIndex > COLOR_FG_OUT)
		return FALSE;

	clrColor = m_clrColors[bColorIndex];
	return TRUE;
}

BOOL CButtonEx::SetColor(BYTE bColorIndex, COLORREF clrColor, BOOL bRepaint)
{
	if (bColorIndex > COLOR_FG_OUT)	
		return FALSE;

	m_clrColors[bColorIndex] = clrColor;
	if (bRepaint == TRUE)
		Invalidate();

	return TRUE;
}

BOOL CButtonEx::SetDefaultColors(BOOL bRepaint)
{
	m_clrColors[COLOR_BK_IN]  = ::GetSysColor(COLOR_BTNFACE) + RGB(30, 30, 30);
	m_clrColors[COLOR_FG_IN]  = ::GetSysColor(COLOR_BTNTEXT);
	m_clrColors[COLOR_BK_OUT] = ::GetSysColor(COLOR_BTNFACE);
	m_clrColors[COLOR_FG_OUT] = ::GetSysColor(COLOR_BTNTEXT);

	if (bRepaint == TRUE)
		Invalidate();

	return TRUE;
}

BOOL CButtonEx::SetButtonCursor(int nCursorId)
{
	if (m_hCursor != NULL) 
		::DestroyCursor(m_hCursor);

	m_hCursor = NULL;

	if (nCursorId != NULL) {
		m_hCursor = (HCURSOR)::LoadImage(AfxGetResourceHandle(), 
			MAKEINTRESOURCE(nCursorId), IMAGE_CURSOR, 0, 0, 0);
		if (m_hCursor == NULL) 
			return FALSE;
	}

	return TRUE;
}

void CButtonEx::SetFlat(BOOL bState)
{
	m_bIsFlat = bState;
	Invalidate();
}

BOOL CButtonEx::GetFlat()
{
	return m_bIsFlat;
}

void CButtonEx::SetFlatFocus(BOOL bDrawFlatFocus, BOOL bRepaint)
{
	m_bDrawFlatFocus = bDrawFlatFocus;
	
	if (bRepaint == TRUE) 
		Invalidate();
}

BOOL CButtonEx::SetAutoRepeat(BOOL bSet, DWORD dwMilliseconds)
{
	m_bAutoRepeat = bSet;
	m_dwPeriodAutoRepeat = dwMilliseconds;

	return TRUE;
}

void CButtonEx::SetCheck(int nCheck, BOOL bRepaint)
{
	if (!m_bIsCheckBox)
		return;

	if (nCheck == 0) 
		m_nCheck = 0;
	else 
		m_nCheck = 1;

	if (bRepaint)
		Invalidate();
}

int CButtonEx::GetCheck()
{
	return m_nCheck;
}

void CButtonEx::DrawBorder(BOOL bEnable)
{
	m_bDrawBorder = bEnable;
}

BOOL CButtonEx::SetIcon(int nIconInId, int nIconOutId)
{
	HICON		hIconIn;
	HICON		hIconOut;

	hIconIn = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nIconInId), IMAGE_ICON, 0, 0, 0);
	hIconOut = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nIconOutId), IMAGE_ICON, 0, 0, 0);

	return SetIcon(hIconIn, hIconOut);
}

BOOL CButtonEx::SetIcon(HICON hIconIn, HICON hIconOut)
{
	BOOL		bRetValue;
	ICONINFO	ii;

	FreeResources();

	if (hIconIn != NULL) {
		m_abiIcons[0].hIcon = hIconIn;

		ZeroMemory(&ii, sizeof(ICONINFO));
		bRetValue = ::GetIconInfo(hIconIn, &ii);
		if (bRetValue == FALSE) {
			FreeResources();
			return FALSE;
		}

		m_abiIcons[0].dwWidth	= (DWORD)(ii.xHotspot * 2);
		m_abiIcons[0].dwHeight	= (DWORD)(ii.yHotspot * 2);
		::DeleteObject(ii.hbmMask);
		::DeleteObject(ii.hbmColor);

		if (hIconOut != NULL) {
			m_abiIcons[1].hIcon = hIconOut;

			ZeroMemory(&ii, sizeof(ICONINFO));
			bRetValue = ::GetIconInfo(hIconOut, &ii);
			if (bRetValue == FALSE) {
				FreeResources();
				return FALSE;
			}

			m_abiIcons[1].dwWidth	= (DWORD)(ii.xHotspot * 2);
			m_abiIcons[1].dwHeight	= (DWORD)(ii.yHotspot * 2);
			::DeleteObject(ii.hbmMask);
			::DeleteObject(ii.hbmColor);
		}
	}

	RedrawWindow();
	return TRUE;
}

BOOL CButtonEx::SetBitmaps(int nBitmapIn, COLORREF crTransColorIn, int nBitmapOut, COLORREF crTransColorOut)
{
	HBITMAP		hBitmapIn	= NULL;
	HBITMAP		hBitmapOut	= NULL;
	
	hBitmapIn = (HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nBitmapIn), IMAGE_BITMAP, 0, 0, 0);
	hBitmapOut = (HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nBitmapOut), IMAGE_BITMAP, 0, 0, 0);

	return SetBitmaps(hBitmapIn, crTransColorIn, hBitmapOut, crTransColorOut);
}

BOOL CButtonEx::SetBitmaps(HBITMAP hBitmapIn, COLORREF crTransColorIn, HBITMAP hBitmapOut, COLORREF crTransColorOut)
{
	int		nRetValue;
	BITMAP	csBitmapSize;

	FreeResources();

	if (hBitmapIn != NULL) {
		m_abmBitmaps[0].hBitmap = hBitmapIn;
		m_abmBitmaps[0].crTransparent = crTransColorIn;
		nRetValue = ::GetObject(hBitmapIn, sizeof(csBitmapSize), &csBitmapSize);

		if (nRetValue == 0) {
			FreeResources();
			return FALSE;
		}

		m_abmBitmaps[0].dwWidth = (DWORD)csBitmapSize.bmWidth;
		m_abmBitmaps[0].dwHeight = (DWORD)csBitmapSize.bmHeight;

		m_abmBitmaps[0].hMask = CreateBitmapMask(hBitmapIn, m_abmBitmaps[0].dwWidth, m_abmBitmaps[0].dwHeight, crTransColorIn);
		if (m_abmBitmaps[0].hMask == NULL) {
			FreeResources();
			return FALSE;
		}

		if (hBitmapOut != NULL) {
			m_abmBitmaps[1].hBitmap = hBitmapOut;
			m_abmBitmaps[1].crTransparent = crTransColorOut;
			nRetValue = ::GetObject(hBitmapOut, sizeof(csBitmapSize), &csBitmapSize);
			if (nRetValue == 0) {
				FreeResources();
				return FALSE;
			}
			m_abmBitmaps[1].dwWidth = (DWORD)csBitmapSize.bmWidth;
			m_abmBitmaps[1].dwHeight = (DWORD)csBitmapSize.bmHeight;

			m_abmBitmaps[1].hMask = CreateBitmapMask(hBitmapOut, m_abmBitmaps[1].dwWidth, m_abmBitmaps[1].dwHeight, crTransColorOut);
			if (m_abmBitmaps[1].hMask == NULL) {
				FreeResources();
				return FALSE;
			}
		}
	}

	RedrawWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CButtonEx Operations

/////////////////////////////////////////////////////////////////////////////
// CButtonEx Implementation

BOOL CButtonEx::IsDefault()
{
	return (GetButtonStyle() & BS_DEFPUSHBUTTON) ? TRUE : FALSE;
}

void CButtonEx::CancelHover()
{
	if (m_bIsFlat == FALSE) 
		return;

	if (m_bMouseOnButton == TRUE) {
		m_bMouseOnButton = FALSE;
		Invalidate();
	}
}

void CButtonEx::PrepareImageRect(BOOL bHasTitle, RECT* prcItem, CRect* prcTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, CRect* prcImage)
{
	CRect rcButton;

	prcImage->CopyRect(prcItem);
	switch (m_nAlign) {
		case ALIGN_HORIZ:
			if (!bHasTitle) {
				prcImage->left += ((prcImage->Width() - dwWidth) / 2);
			}
			else {
				prcImage->left += 3;  
				prcTitle->left += dwWidth + 3;
			}

			prcImage->top += ((prcImage->Height() - dwHeight) / 2);
			break;

		case ALIGN_HORIZ_RIGHT:
			GetClientRect(&rcButton);
			if (!bHasTitle) {
				prcImage->left += ((prcImage->Width() - dwWidth) / 2);
			}
			else {
				prcTitle->right = prcTitle->Width() - dwWidth - 3;
				prcTitle->left = 3;
				prcImage->left = rcButton.right - dwWidth - 3;
				prcImage->top += ((prcImage->Height() - dwHeight)/2);
			}
			break;
		
		case ALIGN_VERT:
			prcImage->left += ((prcImage->Width() - dwWidth) / 2);
			if (!bHasTitle) {
				prcImage->top += ((prcImage->Height() - dwHeight)/2);
			}
			else {
				prcImage->top = 3;
				prcTitle->top += dwHeight;
			}
			break;
	}
    
	if (bIsPressed == TRUE && m_bIsCheckBox == FALSE)
		prcImage->OffsetRect(1, 1);
}

HBITMAP CButtonEx::CreateBitmapMask(HBITMAP hSourceBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF clrTransColor)
{
	HBITMAP		hMask		= NULL;
	HDC			hdcSrc		= NULL;
	HDC			hdcDest		= NULL;
	HBITMAP		hbmSrcT		= NULL;
	HBITMAP		hbmDestT	= NULL;
	COLORREF	clrSaveBk;
	COLORREF	clrSaveDestText;

	hMask = ::CreateBitmap(dwWidth, dwHeight, 1, 1, NULL);
	if (hMask == NULL)
		return NULL;

	hdcSrc	= ::CreateCompatibleDC(NULL);
	hdcDest	= ::CreateCompatibleDC(NULL);

	hbmSrcT = (HBITMAP)::SelectObject(hdcSrc, hSourceBitmap);
	hbmDestT = (HBITMAP)::SelectObject(hdcDest, hMask);

	clrSaveBk = ::SetBkColor(hdcSrc, clrTransColor);

	::BitBlt(hdcDest, 0, 0, dwWidth, dwHeight, hdcSrc, 0, 0, SRCCOPY);

	clrSaveDestText = ::SetTextColor(hdcSrc, RGB(255, 255, 255));
	::SetBkColor(hdcSrc,RGB(0, 0, 0));

	::BitBlt(hdcSrc, 0, 0, dwWidth, dwHeight, hdcDest, 0, 0, SRCAND);

	SetTextColor(hdcDest, clrSaveDestText);

	::SetBkColor(hdcSrc, clrSaveBk);
	::SelectObject(hdcSrc, hbmSrcT);
	::SelectObject(hdcDest, hbmDestT);

	::DeleteDC(hdcSrc);
	::DeleteDC(hdcDest);

	return hMask;
}

void CButtonEx::DrawTheBitmap(CDC* pDC, BOOL bHasTitle, RECT *prcItem, CRect *prcCaption, BOOL bIsPressed, BOOL bIsDisabled)
{
	HDC		hdcBmpMem	= NULL;
	HBITMAP	hbmOldBmp	= NULL;
	HDC		hdcMem		= NULL;
	HBITMAP	hbmTemp		= NULL;

	BYTE	bIndex		= 0;

	if (m_bIsCheckBox == TRUE) {
		if (bIsPressed == TRUE)
			bIndex = 0;
		else
			bIndex = (m_abmBitmaps[1].hBitmap == NULL ? 0 : 1);
	}
	else {
		if (m_bMouseOnButton == TRUE || bIsPressed == TRUE)
			bIndex = 0;
		else
			bIndex = (m_abmBitmaps[1].hBitmap == NULL ? 0 : 1);
	}

	CRect rcImage;
	PrepareImageRect(bHasTitle, prcItem, prcCaption, bIsPressed, m_abmBitmaps[bIndex].dwWidth, m_abmBitmaps[bIndex].dwHeight, &rcImage);

	hdcBmpMem = ::CreateCompatibleDC(pDC->m_hDC);
	hbmOldBmp = (HBITMAP)::SelectObject(hdcBmpMem, m_abmBitmaps[bIndex].hBitmap);
	hdcMem = ::CreateCompatibleDC(NULL);

	hbmTemp = (HBITMAP)::SelectObject(hdcMem, m_abmBitmaps[bIndex].hMask);

	::BitBlt(pDC->m_hDC, rcImage.left, rcImage.top, m_abmBitmaps[bIndex].dwWidth, m_abmBitmaps[bIndex].dwHeight, hdcMem, 0, 0, SRCAND);
	::BitBlt(pDC->m_hDC, rcImage.left, rcImage.top, m_abmBitmaps[bIndex].dwWidth, m_abmBitmaps[bIndex].dwHeight, hdcBmpMem, 0, 0, SRCPAINT);

	::SelectObject(hdcMem, hbmTemp);
	::DeleteDC(hdcMem);

	::SelectObject(hdcBmpMem, hbmOldBmp);
	::DeleteDC(hdcBmpMem);
}

void CButtonEx::FreeResources(BOOL bCheckForNULL)
{
	if (bCheckForNULL) {
		if (m_abiIcons[0].hIcon != NULL)	
			::DeleteObject(m_abiIcons[0].hIcon);
		if (m_abiIcons[1].hIcon != NULL)	
			::DeleteObject(m_abiIcons[1].hIcon);

		if (m_abmBitmaps[0].hBitmap != NULL)	
			::DeleteObject(m_abmBitmaps[0].hBitmap);
		if (m_abmBitmaps[1].hBitmap != NULL)	
			::DeleteObject(m_abmBitmaps[1].hBitmap);

		if (m_abmBitmaps[0].hMask != NULL)	
			::DeleteObject(m_abmBitmaps[0].hMask);
		if (m_abmBitmaps[1].hMask != NULL)	
			::DeleteObject(m_abmBitmaps[1].hMask);
	}

	::ZeroMemory(&m_abiIcons, sizeof(m_abiIcons));
	::ZeroMemory(&m_abmBitmaps, sizeof(m_abmBitmaps));
}

void CButtonEx::DrawTheIcon(CDC* pDC, BOOL bHasTitle, RECT* prcItem, CRect* prcTitle, BOOL bIsPressed, BOOL bIsDisabled)
{
	BYTE bIndex	= 0;

	if (m_bIsCheckBox == TRUE) {
		if (bIsPressed == TRUE)
			bIndex = 0;
		else
			bIndex = (m_abiIcons[1].hIcon == NULL ? 0 : 1);
	}
	else {
		if (m_bMouseOnButton == TRUE || bIsPressed == TRUE)
			bIndex = 0;
		else
			bIndex = (m_abiIcons[1].hIcon == NULL ? 0 : 1);
	}

	CRect rcImage;
	PrepareImageRect(bHasTitle, prcItem, prcTitle, bIsPressed, m_abiIcons[bIndex].dwWidth, 
					 m_abiIcons[bIndex].dwHeight, &rcImage);

	pDC->DrawState(rcImage.TopLeft(), rcImage.Size(), m_abiIcons[bIndex].hIcon,
				   (bIsDisabled ? DSS_DISABLED : DSS_NORMAL), (CBrush*)NULL);
}

BOOL CButtonEx::OnDrawBackground(CDC* pDC, LPCRECT pRect, BOOL bIsPressed)
{
	COLORREF clrColor;

	if (m_bMouseOnButton || bIsPressed)
		clrColor = m_clrColors[COLOR_BK_IN];
	else
		clrColor = m_clrColors[COLOR_BK_OUT];

	CBrush brBackground(clrColor);

	pDC->FillRect(pRect, &brBackground);

	return TRUE;
}

BOOL CButtonEx::OnDrawBorder(CDC* pDC, LPCRECT pRect, BOOL bIsPressed)
{
	if (bIsPressed)
		pDC->Draw3dRect(pRect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT));
	else
		pDC->Draw3dRect(pRect, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CButtonEx message handlers

void CButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	CDC	 *pDC = CDC::FromHandle(lpDIS->hDC);
	if (NULL == pDC)
		return;

	CPen *pOldPen;
	BOOL  bIsPressed, bIsFocused, bIsDisabled;

	if (m_bIsCheckBox) {
		bIsPressed  =  (lpDIS->itemState & ODS_SELECTED) || (m_nCheck != 0);
	}
	else {
		bIsPressed = (lpDIS->itemState & ODS_SELECTED);
	}

	bIsFocused  = (lpDIS->itemState & ODS_FOCUS);
	bIsDisabled = (lpDIS->itemState & ODS_DISABLED);

	CRect rectItem = lpDIS->rcItem;

	pDC->SetBkMode(TRANSPARENT);

	if (m_bIsFlat == FALSE) {
		if (bIsFocused || (IsDefault() == TRUE)) {
			CBrush br(RGB(0,0,0));  
			pDC->FrameRect(&rectItem, &br);
			rectItem.DeflateRect(1, 1);
		}
	}

	OnDrawBackground(pDC, &rectItem, bIsPressed);

	if (bIsPressed) {
		if (m_bIsFlat == TRUE) {
			if (m_bDrawBorder)
				OnDrawBorder(pDC, &rectItem, bIsPressed);
		}
		else {
			CBrush brButtonShadow(GetSysColor(COLOR_BTNSHADOW));
			pDC->FrameRect(&rectItem, &brButtonShadow);
		}
	}
	else {
		CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // White
		CPen pen3DLight(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));       // Light gray
		CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray
		CPen pen3DDKShadow(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW)); // Black

		if (m_bIsFlat == TRUE) {
			if (m_bMouseOnButton && m_bDrawBorder)
				OnDrawBorder(pDC, &rectItem, bIsPressed);
		}
		else {
			// Draw top-left borders
			// White line
			pOldPen = pDC->SelectObject(&penBtnHiLight);
			pDC->MoveTo(rectItem.left, rectItem.bottom - 1);
			pDC->LineTo(rectItem.left, rectItem.top);
			pDC->LineTo(rectItem.right, rectItem.top);

			// Light gray line
			pDC->SelectObject(pen3DLight);
			pDC->MoveTo(rectItem.left + 1, rectItem.bottom - 1);
			pDC->LineTo(rectItem.left + 1, rectItem.top + 1);
			pDC->LineTo(rectItem.right, rectItem.top + 1);

			// Draw bottom-right borders
			// Black line
			pDC->SelectObject(pen3DDKShadow);
			pDC->MoveTo(rectItem.left, rectItem.bottom - 1);
			pDC->LineTo(rectItem.right - 1, rectItem.bottom - 1);
			pDC->LineTo(rectItem.right - 1, rectItem.top - 1);

			// Dark gray line
			pDC->SelectObject(penBtnShadow);
			pDC->MoveTo(rectItem.left + 1, rectItem.bottom - 2);
			pDC->LineTo(rectItem.right - 2, rectItem.bottom - 2);
			pDC->LineTo(rectItem.right - 2, rectItem.top);

			pDC->SelectObject(pOldPen);
		}
	}

	// Read the button's title
	CString strTitle;
	GetWindowText(strTitle);

	CRect rectCaption = lpDIS->rcItem;

	// Draw the icon
	if (m_abiIcons[0].hIcon != NULL) {
		DrawTheIcon(pDC, !strTitle.IsEmpty(), &lpDIS->rcItem, &rectCaption, bIsPressed, bIsDisabled);
	}

	if (m_abmBitmaps[0].hBitmap != NULL) {
		pDC->SetBkColor(RGB(255,255,255));
		DrawTheBitmap(pDC, !strTitle.IsEmpty(), &lpDIS->rcItem, &rectCaption, bIsPressed, bIsDisabled);
	}

	if (!strTitle.IsEmpty()) {
		if (bIsPressed && m_bIsCheckBox == FALSE)
			rectCaption.OffsetRect(1, 1);
    
		CRect rectCenter = rectCaption;
		pDC->DrawText(strTitle, -1, rectCaption, DT_WORDBREAK | DT_CENTER | DT_CALCRECT);
		rectCaption.OffsetRect((rectCenter.Width() - rectCaption.Width())/2, (rectCenter.Height() - rectCaption.Height()) / 2);

		pDC->SetBkMode(TRANSPARENT);
		if (bIsDisabled) {
			rectCaption.OffsetRect(1, 1);
			pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
			pDC->DrawText(strTitle, -1, rectCaption, DT_WORDBREAK | DT_CENTER);

			rectCaption.OffsetRect(-1, -1);
			pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
			pDC->DrawText(strTitle, -1, rectCaption, DT_WORDBREAK | DT_CENTER);
		}
		else {
			if (m_bMouseOnButton || bIsPressed) {
				pDC->SetTextColor(m_clrColors[COLOR_FG_IN]);
				pDC->SetBkColor(m_clrColors[COLOR_BK_IN]);
			}
			else {
				pDC->SetTextColor(m_clrColors[COLOR_FG_OUT]);
				pDC->SetBkColor(m_clrColors[COLOR_BK_OUT]);
			}
			pDC->DrawText(strTitle, -1, rectCaption, DT_WORDBREAK | DT_CENTER);
		}
	}

	if ((!m_bIsFlat) || (m_bIsFlat && m_bDrawFlatFocus)) {
		if (bIsFocused) {
			CRect rectFocus = rectItem;
			rectFocus.DeflateRect(3, 3);
			pDC->DrawFocusRect(&rectFocus);
		}
	}
}

void CButtonEx::PreSubclassWindow() 
{
	UINT nButtonStyle = GetButtonStyle();
	if (nButtonStyle & BS_CHECKBOX) 
		m_bIsCheckBox = TRUE;

	SetButtonStyle(nButtonStyle | BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}

LRESULT CButtonEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_LBUTTONDBLCLK) {
		message = WM_LBUTTONDOWN;
	}

	return CButton::DefWindowProc(message, wParam, lParam);
}

void CButtonEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd*	pParent;

	CButton::OnMouseMove(nFlags, point);

	if ((nFlags & MK_LBUTTON) && (!m_bMouseOnButton)) 
		return;

	if (m_bIsFlat == FALSE) 
		return;

	pParent = GetOwner();

	if ((GetCapture() != this) && (pParent != NULL)) {
		m_bMouseOnButton = TRUE;
		SetCapture();
		Invalidate();
	}
	else {
		POINT p2 = point;
		ClientToScreen(&p2);
		CWnd* pWndUnderMouse = WindowFromPoint(p2);
		if (pWndUnderMouse && pWndUnderMouse->m_hWnd != m_hWnd) {
			if (m_bMouseOnButton == TRUE) {
				m_bMouseOnButton = FALSE;
				Invalidate();
			}
			if (!(nFlags & MK_LBUTTON)) 
				ReleaseCapture();
		}
	}
}

void CButtonEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonDown(nFlags, point);

	if (m_bAutoRepeat == TRUE) {
		m_uIdRepeatTimer = SetTimer(ID_AUTO_REPEAT, m_dwPeriodAutoRepeat, NULL);
	}
}

void CButtonEx::OnCaptureChanged(CWnd *pWnd) 
{
	if (pWnd != this) {
		ReleaseCapture();
		CancelHover();
	}

	CButton::OnCaptureChanged(pWnd);
}

BOOL CButtonEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor != NULL) {
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void CButtonEx::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);
	if (!bEnable)
		CancelHover();
}

void CButtonEx::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CButton::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_INACTIVE)
		CancelHover();
}

void CButtonEx::OnCancelMode() 
{
	CButton::OnCancelMode();
	CancelHover();
}

void CButtonEx::OnSysColorChange() 
{
	CButton::OnSysColorChange();	
}

HBRUSH CButtonEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return (HBRUSH)::GetStockObject(NULL_BRUSH); 
}

void CButtonEx::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);

	CancelHover();
}

void CButtonEx::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_uIdRepeatTimer) {
		BOOL bMouseDown = GetAsyncKeyState(
			GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON) & 0x8000;
		if (!bMouseDown) {
			KillTimer(m_uIdRepeatTimer);
			m_uIdRepeatTimer = 0;
		}
		else {
			POINT pt;
			GetCursorPos(&pt);
			CWnd* pWndUnderMouse = WindowFromPoint(pt);
			if (pWndUnderMouse == (CWnd *)this)
				GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM((WORD)GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
		}
	}

	CButton::OnTimer(nIDEvent);
}

void CButtonEx::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_uIdRepeatTimer) {
		KillTimer(m_uIdRepeatTimer);
		m_uIdRepeatTimer = 0;
	}

	POINT p2 = point;
	ClientToScreen(&p2);
	CWnd* pWndUnderMouse = WindowFromPoint(p2);

	if ((m_bIsCheckBox) && (pWndUnderMouse == this)) {
		m_nCheck = !m_nCheck;
		Invalidate();
	}

	CButton::OnLButtonUp(nFlags, point);
}
