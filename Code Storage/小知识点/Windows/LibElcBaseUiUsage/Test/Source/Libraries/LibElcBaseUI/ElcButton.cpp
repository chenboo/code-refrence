// ElcButton.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcButton.h"


// CElcButton

IMPLEMENT_DYNAMIC(CElcButton, CButton)

CElcButton::CElcButton()
{
	memset(&m_skin, 0, sizeof(m_skin));

	m_hNotifyWnd = NULL;
	m_nNotifyMsg = NULL;

	m_nLayout = ELCBL_HORIZONTAL;
	m_droptype = ELCBDT_NORMAL;
	m_textstyle = ELCBDT_NORMAL;
	m_nIconType = ELCBIM_SINGLE;
	m_pIcon = NULL;
	m_bIconExtern = FALSE;

	m_bMouseOnButton = FALSE;
	m_bLButtonDown = FALSE;
	m_bAnimateDown = TRUE;
	m_bEnableCtrlColor = FALSE;
	m_nChecked = FALSE;
	m_bMirror = FALSE;

	m_nHMargin = 4;
	m_nVMargin = 2;

	GetMSShellDlgFont(m_ftText);
}

CElcButton::~CElcButton()
{
	if (m_pIcon && !m_bIconExtern)
		delete m_pIcon;
}


BEGIN_MESSAGE_MAP(CElcButton, CButton)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()



// CElcButton message handlers

void CElcButton::PreSubclassWindow()
{
	m_nChecked = __super::GetCheck();
	m_bMirror = IsLayoutRTL();

	LoadDefSkin();

	CButton::PreSubclassWindow();
}

int CElcButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bMirror = IsLayoutRTL();

	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	LoadDefSkin();

	return 0;
}

BOOL CElcButton::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CButton::PreTranslateMessage(pMsg);
}

LRESULT CElcButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (BM_SETCHECK == message) {
		m_nChecked = (int)wParam;
	}
	else if (BM_GETCHECK == message) {
		return m_nChecked;
	}
	else if (WM_ENABLE == message
		|| WM_SETTEXT == message)
	{
		SetRedraw(FALSE);
		LRESULT lResult =  __super::WindowProc(message, wParam, lParam);
		SetRedraw(TRUE);
		Invalidate(FALSE);
		return lResult;
	}

	return __super::WindowProc(message, wParam, lParam);
}

void CElcButton::LoadDefSkin()
{
	if (_tcslen(m_skin.szStyle) > 0)
		return;

	CString strStyle;

	UINT uButtonStyle = GetButtonStyle();
	if (BS_CHECKBOX == uButtonStyle
		|| BS_AUTOCHECKBOX == uButtonStyle
		|| BS_3STATE == uButtonStyle
		|| BS_AUTO3STATE == uButtonStyle
		|| BS_RADIOBUTTON == uButtonStyle
		|| BS_AUTORADIOBUTTON == uButtonStyle) 
	{
		strStyle = ELC_SKIN_STYLE_BUTTON_CHECKBOX;
	}
	else if (BS_GROUPBOX == uButtonStyle) 
	{
		strStyle = ELC_SKIN_STYLE_BUTTON_GROUPBOX;
	}
	else
	{
		strStyle = ELC_SKIN_STYLE_BUTTON_MAIN_TOOLBAR;
	}

	LoadSkin(strStyle);
}

BOOL CElcButton::LoadSkin(LPCTSTR lpszStyle)
{
	memset(&m_skin, 0, sizeof(m_skin));

	PELC_SKIN_BUTTON pSkin = theSkin.GetButtonSkin(lpszStyle);
	if (!pSkin) {
		// not find the style, pls check it
		ASSERT(0);
		return FALSE;
	}

	memcpy(&m_skin, pSkin, sizeof(ELC_SKIN_BUTTON));

	m_sizeBackgnd = CElcSkinDoc::GetImageSize(&m_skin.background);

	return TRUE;
}

void CElcButton::SetIconImage(UINT nResId, LPCTSTR lpszType, ELC_BUTTON_ICON_TYPE type)
{
	if (m_pIcon && !m_bIconExtern)
		delete m_pIcon;
	m_pIcon = NULL;

	m_nIconType = type;

	if (!ImageFromIDResource(nResId, lpszType, m_pIcon)) {
		if (m_pIcon)
			delete m_pIcon;
	}

	m_bIconExtern = FALSE;
}

void CElcButton::SetIconImage(Image * pImage, ELC_BUTTON_ICON_TYPE type)
{
	if (m_pIcon && !m_bIconExtern)
		delete m_pIcon;

	m_pIcon = NULL;
	m_pIcon = pImage;
	m_nIconType = type;
	m_bIconExtern = TRUE;
}

CSize CElcButton::GetIdealSize()
{
	CSize size(0,0);
	UINT uButtonStyle = GetButtonStyle();
	if (BS_CHECKBOX == uButtonStyle
		|| BS_AUTOCHECKBOX == uButtonStyle
		|| BS_3STATE == uButtonStyle
		|| BS_AUTO3STATE == uButtonStyle
		|| BS_RADIOBUTTON == uButtonStyle
		|| BS_AUTORADIOBUTTON == uButtonStyle
		|| BS_GROUPBOX == uButtonStyle) 
	{
		// do not call this function when the button has these style
		ASSERT(0);
		return size;
	}

	if (m_nIconType == ELCBIM_FOUR_STATE) {
		size.cx = m_pIcon->GetWidth();
		size.cy = m_pIcon->GetHeight() / m_nIconType;
	}
	else {
		CSize sizeTest(0,0);
		CString strText;
		GetWindowText(strText);
		if (!strText.IsEmpty())
			sizeTest = CElcSkinDoc::GetStringSize(strText, &m_ftText);

		if (m_nLayout == ELCBL_HORIZONTAL) {
			if (m_pIcon) {
				size.cx = m_nHMargin + m_pIcon->GetWidth() + m_nHMargin;
				size.cy = m_nVMargin + m_pIcon->GetHeight() + m_nVMargin;
			}
			else {
				size.cx = m_nHMargin;
			}

			if (!strText.IsEmpty()) {
				size.cx += sizeTest.cx + m_nHMargin;
				size.cy = max(sizeTest.cy + m_nVMargin * 2 , size.cy);
			}

			if (m_droptype != ELCBDT_NORMAL && m_skin.dropdown.pImage) {
				size.cx += m_nHMargin + CElcSkinDoc::GetImageSize(&m_skin.dropdown).cx + m_nHMargin;
			}

			size.cy = max(m_sizeBackgnd.cy, size.cy);
		}
		else {
			if (m_pIcon) {
				size.cx = m_nHMargin + m_pIcon->GetWidth() + m_nHMargin;
				size.cy = m_nVMargin + m_pIcon->GetHeight() + m_nVMargin;
			}
			else {
				size.cy = m_nVMargin;
			}

			if (!strText.IsEmpty()) {
				size.cx = max(sizeTest.cx + m_nHMargin * 2, size.cx);
				size.cy += sizeTest.cx + m_nVMargin;
			}

			if (m_droptype != ELCBDT_NORMAL && m_skin.dropdown.pImage) {
				size.cy += m_nVMargin + CElcSkinDoc::GetImageSize(&m_skin.dropdown).cy + m_nVMargin;
			}
		}
	}

	return size;
}

BOOL CElcButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void CElcButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;

	CButton::OnLButtonDown(nFlags, point);
}

void CElcButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bLButtonDown 
		&& m_rcClient.PtInRect(point)
		&& m_hNotifyWnd
		&& m_nNotifyMsg) 
	{
		::PostMessage(m_hNotifyWnd, m_nNotifyMsg, GetDlgCtrlID(), 0);
	}

	m_bLButtonDown = FALSE;
	Invalidate();

	CButton::OnLButtonUp(nFlags, point);
}

void CElcButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CButton::OnMouseMove(nFlags, point);

	if ((nFlags & MK_LBUTTON) && m_bMouseOnButton == FALSE) 
		return;

	CWnd* pWnd = GetActiveWindow();
	if ((GetCapture() != this) && pWnd != NULL) {
		m_bMouseOnButton = TRUE;
		SetCapture();
		Invalidate();
	}
	else {
		POINT pt = point;
		ClientToScreen(&pt);
		CWnd* wndUnderMouse = WindowFromPoint(pt);
		if (wndUnderMouse && wndUnderMouse->m_hWnd != this->m_hWnd) {
			if (m_bMouseOnButton == TRUE) {
				m_bMouseOnButton = FALSE;
				Invalidate();
			}

			if (!(nFlags & MK_LBUTTON)) 
				ReleaseCapture();
		}
	}
}

void CElcButton::OnCaptureChanged(CWnd *pWnd) 
{
	if (pWnd == this) {
		return;
	}

	BOOL bRepaint = FALSE;
	if (m_bMouseOnButton) {
		m_bMouseOnButton = FALSE;
		bRepaint = TRUE;
	}

	if (m_bLButtonDown) {
		m_bLButtonDown = FALSE;
		bRepaint = TRUE;
	}

	if (bRepaint)
		Invalidate();

	CButton::OnCaptureChanged(pWnd);
}

BOOL CElcButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

int CElcButton::GetButtonState()
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	if (!IsWindowEnabled())
		return ELCBS_DISABLED;
	else if (m_bLButtonDown && m_rcClient.PtInRect(point)) 
		return ELCBS_DOWN;
	else if (m_bMouseOnButton)
		return ELCBS_HOT;
	else 
		return ELCBS_NORMAL;
}

void CElcButton::OnPaint()
{
	CPaintDC dc(this);

	GetClientRect(m_rcClient);
	CElcMemDC dcMem(&dc, m_rcClient);

	CFont * pOldFont = dcMem.SelectObject(&m_ftText);

	DWORD dwStyle = GetStyle();
	UINT uButtonStyle = GetButtonStyle();

	if (BS_CHECKBOX == uButtonStyle
		|| BS_AUTOCHECKBOX == uButtonStyle
		|| BS_3STATE == uButtonStyle
		|| BS_AUTO3STATE == uButtonStyle
		|| BS_RADIOBUTTON == uButtonStyle
		|| BS_AUTORADIOBUTTON == uButtonStyle) 
	{
		// draw check-box or radio-button
		DrawCheckBox(&dcMem);
	}
	else if (BS_GROUPBOX == uButtonStyle) 
	{
		// draw group box
		DrawGroupBox(&dcMem);
	}
	else
	{
		// draw push button
		DrawPushButton(&dcMem);
	}

	dcMem.SelectObject(pOldFont);
	dcMem.DrawMe();
}

void CElcButton::DrawCheckBox(CDC* pDC)
{
	DWORD dwLayout = ::GetLayout(pDC->GetSafeHdc());
	if (m_bMirror)
		::SetLayout(pDC->m_hDC, LAYOUT_LTR);

	Graphics graphics(pDC->m_hDC);
	::SetLayout(pDC->m_hDC, dwLayout);

	ELC_UI_DRAW predraw;
	predraw.hWnd = m_hWnd;
	predraw.type = ELC_UI_DT_PREDRAW;
	predraw.hdc = pDC->GetSafeHdc();
	predraw.rect = m_rcClient;
	if (!GetParent()->SendMessage(WM_ELC_UI_DRAW_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&predraw)) {
		if (m_bEnableCtrlColor) {
			HBRUSH hBrush = (HBRUSH)GetParent()->SendMessage(WM_CTLCOLORBTN, (WPARAM)pDC->GetSafeHdc(), (LPARAM)m_hWnd);
			if (hBrush)
				::FillRect(pDC->GetSafeHdc(), m_rcClient, hBrush);
		}
		else {
			Color crBackgnd;
			crBackgnd.SetFromCOLORREF(m_skin.crBackgnd);
			graphics.Clear(crBackgnd);
		}
	}
	
	int nState = GetButtonState();

	Rect rcDest;
	rcDest.X = m_rcClient.left;
	rcDest.Y = m_rcClient.top + (m_rcClient.Height() - m_sizeBackgnd.cy) / 2;
	rcDest.Width = m_sizeBackgnd.cx;
	rcDest.Height = m_sizeBackgnd.cy;

	// draw background
	thePainter.DrawImage(&graphics,
		&m_skin.background,
		&rcDest,
		nState,
		1,
		0);

	// draw check or indeterminate
	PELC_SKIN_IMAGE pSkinImage = NULL;
	if (m_nChecked == BST_CHECKED)
		pSkinImage = &m_skin.check;
	else if (m_nChecked == BST_INDETERMINATE)
		pSkinImage = &m_skin.Indeterminate;

	if (pSkinImage) {
		thePainter.DrawImage(&graphics,
			pSkinImage,
			&rcDest,
			nState,
			1,
			0);
	}

	// draw text
	CRect rcText = m_rcClient;
	rcText.left = rcDest.X + rcDest.Width + 3;

	DWORD dwStyle = GetStyle();

	UINT nFormat = 0;
	if (dwStyle & BS_LEFT)
		nFormat |= DT_LEFT;
	else if (dwStyle & BS_RIGHT)
		nFormat |= DT_RIGHT;
	else if (dwStyle & BS_CENTER)
		nFormat |= DT_CENTER;

	if (dwStyle & BS_VCENTER)
		nFormat |= DT_VCENTER | DT_SINGLELINE;
	else if (dwStyle & BS_TOP)
		nFormat |= DT_TOP;
	else if (dwStyle & BS_BOTTOM)
		nFormat |= DT_BOTTOM;
	else
		nFormat |= DT_VCENTER | DT_SINGLELINE;

	if ((dwStyle & BS_MULTILINE) == 0)
		nFormat |= DT_SINGLELINE;
	else
		nFormat &= ~DT_SINGLELINE;

	if (!m_bEnableCtrlColor) {
		if (nState == ELCBS_NORMAL)
			pDC->SetTextColor(m_skin.crTextNormal);
		else if (nState == ELCBS_HOT)
			pDC->SetTextColor(m_skin.crTextHot);
		else if (nState == ELCBS_DOWN)
			pDC->SetTextColor(m_skin.crTextDown);
		else if (nState == ELCBS_DISABLED)
			pDC->SetTextColor(m_skin.crTextDisabled);
	}

	CString strText;
	GetWindowText(strText);
	if (!strText.IsEmpty())
		pDC->DrawText(strText, rcText, nFormat);
}

void CElcButton::DrawGroupBox(CDC* pDC)
{
	CString strText;
	GetWindowText(strText);
	CSize size = pDC->GetTextExtent(strText);

	Graphics graphics(pDC->m_hDC);

	HBRUSH hBrush = NULL;
	if (m_bEnableCtrlColor) {
		hBrush = (HBRUSH)GetParent()->SendMessage(WM_CTLCOLORBTN, (WPARAM)pDC->GetSafeHdc(), (LPARAM)m_hWnd);
		if (hBrush)
			::FillRect(pDC->GetSafeHdc(), m_rcClient, hBrush);

		m_skin.crTextNormal = pDC->GetTextColor();
	}
	else {
		Color crBackgnd;
		crBackgnd.SetFromCOLORREF(m_skin.crBackgnd);
		graphics.Clear(crBackgnd);
	}

	Rect rect(m_rcClient.left, m_rcClient.top + size.cy / 2, m_rcClient.Width(), m_rcClient.Height() - size.cy / 2);

	int nRoundDiameter = 6;

	Rect rc1(rect.X, rect.Y, nRoundDiameter, nRoundDiameter); // left-top
	Rect rc2(rect.X + rect.Width - nRoundDiameter - 1, rect.Y, nRoundDiameter, nRoundDiameter); // right-top
	Rect rc3(rect.X + rect.Width - nRoundDiameter - 1, rect.Y + rect.Height - nRoundDiameter - 1, nRoundDiameter, nRoundDiameter); // right-bottom
	Rect rc4(rect.X, rect.Y + rect.Height - nRoundDiameter - 1, nRoundDiameter, nRoundDiameter); // left-bottom

	GraphicsPath grph;
	
	grph.AddArc(rc1, 180, 90);
	grph.AddLine(rc1.X + rc1.Width/2, rc1.Y, rc2.X + rc2.Width/2, rc2.Y);

	grph.AddArc(rc2, 270, 90);
	grph.AddLine(rc2.X + rc2.Width, rc2.Y + rc2.Height/2, rc3.X + rc3.Width, rc3.Y + rc3.Height/2);

	grph.AddArc(rc3, 0, 90);
	grph.AddLine(rc3.X + rc3.Width/2, rc3.Y + rc3.Height, rc4.X + rc4.Width/2, rc4.Y + rc4.Height);

	grph.AddArc(rc4, 90, 90);
	grph.AddLine(rc4.X, rc4.Y + rc4.Height/2, rc1.X, rc1.Y + rc1.Height/2);

	Color crBorder;
	crBorder.SetFromCOLORREF(m_skin.crGroupBorder);
	Pen pen(crBorder, 1);

	SmoothingMode smoothingmode = graphics.GetSmoothingMode();
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.DrawPath(&pen, &grph);

	CRect rcText;
	rcText.left = m_rcClient.left + 5;
	rcText.right = rcText.left + size.cx + 10;
	rcText.top = m_rcClient.top;
	rcText.bottom = rcText.top + size.cy;

	if (!m_bEnableCtrlColor) {
		pDC->FillSolidRect(rcText, m_skin.crBackgnd);
		pDC->SetTextColor(m_skin.crTextNormal);
	}
	else {
		::FillRect(pDC->GetSafeHdc(), rcText, hBrush);
	}

	pDC->DrawText(strText, rcText, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

void CElcButton::DrawPushButton(CDC* pDC)
{
	ELC_UI_DRAW predraw;
	predraw.hWnd = m_hWnd;
	predraw.type = ELC_UI_DT_PREDRAW;
	predraw.hdc = pDC->GetSafeHdc();
	predraw.rect = m_rcClient;
	if (!GetParent()->SendMessage(WM_ELC_UI_DRAW_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&predraw)) {
		if (m_bEnableCtrlColor) {
			HBRUSH hBrush = (HBRUSH)GetParent()->SendMessage(WM_CTLCOLORBTN, (WPARAM)pDC->GetSafeHdc(), (LPARAM)m_hWnd);
			if (hBrush)
				::FillRect(pDC->GetSafeHdc(), m_rcClient, hBrush);

			m_skin.crTextNormal = pDC->GetTextColor();
		}
		else {
			pDC->FillSolidRect(m_rcClient, m_skin.crBackgnd);
		}
	}

	Graphics graphics(pDC->m_hDC);

	int nState = GetButtonState();
	if (m_nChecked && nState != ELCBS_DISABLED)
		nState = ELCBS_DOWN;

	Rect rect(0, 0, m_rcClient.Width(), m_rcClient.Height());

	if (m_nIconType == ELCBIM_SINGLE) {
		thePainter.DrawImage(&graphics,
			&m_skin.background,
			&rect,
			nState,
			1,
			0);
	}

	CRect rcText(0,0,0,0);
	Rect rcIconDest(0,0,0,0);

	if (m_pIcon) {
		if (m_nIconType == ELCBIM_FOUR_STATE) {
			thePainter.DrawFixedImage(&graphics,
				m_pIcon,
				nState,
				4,
				1,
				CPoint(0,0),
				0);
		}
		else {
			ImageAttributes *pImageAtt = NULL;
			ImageAttributes imageAtt;

			if (nState == ELCBS_DISABLED) {
				imageAtt.SetColorMatrix(&g_GrayColorMatrix);
				pImageAtt = &imageAtt;
			}

			rcIconDest.Width = m_pIcon->GetWidth();
			rcIconDest.Height = m_pIcon->GetHeight();
			if (m_nLayout == ELCBL_HORIZONTAL) {
				rcIconDest.X = m_nHMargin;
				rcIconDest.Y = m_nVMargin;
				rcText.left = rcIconDest.X + rcIconDest.Width + m_nHMargin;
				rcText.top = m_rcClient.top;
			}
			else {
				rcIconDest.X = (m_rcClient.Width() - rcIconDest.Width) / 2;
				rcIconDest.Y = m_nVMargin;
				rcText.left = m_rcClient.left;
				rcText.top = rcIconDest.Y + rcIconDest.Width + m_nVMargin;
			}

			if (m_bAnimateDown && nState == ELCBS_DOWN) {
				rcIconDest.Offset(1, 1);
			}

			graphics.DrawImage((Image *)m_pIcon, 
				rcIconDest, 
				0, 
				0, 
				rcIconDest.Width, 
				rcIconDest.Height, 
				UnitPixel,
				pImageAtt);
		}
	}
	else {
		rcText.left = m_rcClient.left;
		rcText.top = m_rcClient.top;
	}

	Rect rcDrop(0,0,0,0);
	if (m_droptype != ELCBDT_NORMAL) {
		CSize size = CElcSkinDoc::GetImageSize(&m_skin.dropdown);
		rcDrop.Width = size.cx;
		rcDrop.Height = size.cy;
		if (m_nLayout == ELCBL_HORIZONTAL) {
			rcDrop.X = m_rcClient.right - m_nHMargin - size.cx;
			rcDrop.Y = (m_rcClient.Height() - size.cy) / 2;
			rcText.right = rcDrop.X - m_nHMargin;
			rcText.bottom = m_rcClient.bottom;
		}
		else {
			rcDrop.X = (m_rcClient.Width() - size.cx) / 2;
			rcDrop.Y = m_rcClient.bottom - size.cy - m_nVMargin;
			rcText.right = m_rcClient.right;
			rcText.bottom = rcDrop.Y - m_nVMargin;
		}

		if (m_bAnimateDown && nState == ELCBS_DOWN) {
			rcDrop.Offset(1, 1);
		}

		thePainter.DrawImage(&graphics,
			&m_skin.dropdown,
			&rcDrop,
			nState,
			1,
			0);
	}
	else {
		rcText.right = m_rcClient.right;
		rcText.bottom = m_rcClient.bottom;
	}

	CString strText;
	GetWindowText(strText);
	if (!strText.IsEmpty())  {

		if (nState == ELCBS_NORMAL)
			pDC->SetTextColor(m_skin.crTextNormal);
		else if (nState == ELCBS_HOT)
			pDC->SetTextColor(m_skin.crTextHot);
		else if (nState == ELCBS_DOWN)
			pDC->SetTextColor(m_skin.crTextDown);
		else if (nState == ELCBS_DISABLED)
			pDC->SetTextColor(m_skin.crTextDisabled);

		UINT nFormat = DT_VCENTER | DT_SINGLELINE;
		if (m_pIcon)
			nFormat |= DT_LEFT;
		else if (m_droptype != ELCBDT_NORMAL)
			nFormat |= DT_RIGHT;
		else
			nFormat |= DT_CENTER;

		if (m_bAnimateDown && nState == ELCBS_DOWN) {
			rcText.OffsetRect(1, 1);
		}

		pDC->DrawText(strText, rcText, nFormat);
	}
}

