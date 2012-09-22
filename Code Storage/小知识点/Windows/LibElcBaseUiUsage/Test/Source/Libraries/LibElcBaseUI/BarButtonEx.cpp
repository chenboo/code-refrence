// BarButtonEx.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "BarButtonEx.h"
#include "GDIPlusSnippets.h"

// CBarButtonEx

IMPLEMENT_DYNAMIC(CBarButtonEx, CButton)

CBarButtonEx::CBarButtonEx()
{
	memset(&m_btnskin, 0, sizeof(m_btnskin));

	m_btnskin.crTextNormal = RGB(14,87,163);
	m_btnskin.crTextHot = RGB(14,87,163);
	m_btnskin.crTextDown = RGB(14,87,163);
	m_btnskin.crTextDisabled = RGB(161,161,146);
	m_btnskin.crBackgnd = RGB(236,233,216);
	
	m_pIconImage = NULL;
	m_sizeIcon.cx = m_sizeIcon.cy = 0;
	m_sizeDrop.cx = m_sizeDrop.cy = 0;
	m_nIconResId = 0;

	m_bTextlinkerBtn = FALSE;
	m_nUnderlineStyle = ELCBTS_UNDERLINE_WHEN_HOT;

	GetMSShellDlgFont(m_ftText);

	m_hNotifyWnd = NULL;
	m_nNotifyMsg = NULL;

	m_bMouseOnButton = FALSE;
	m_bChecked = FALSE;
	m_bLButtonDown = FALSE;
	m_bAnimateDown = TRUE;

	m_layout = ELCBL_HORIZONTAL;
	m_droptype = ELCBDT_NORMAL;
	m_nIconImageMode = ELCBIM_SINGLE;

	m_nHMargin = 4;
	m_nVMargin = 2;

	m_ptAdjust.x = 0;
	m_ptAdjust.y = 0;

	memset(&m_skinOverlay, 0, sizeof(m_skinOverlay));
	m_sizeOverlay.cx = 0;
	m_sizeOverlay.cy = 0;
	m_nAlign = ALIGN_RIGHT | ALIGN_TOP;
	m_nFlickerTime = 0;
	m_nFlickerCount = 0;
	m_nFlickerCounter = 0;
	m_bShowOverlay = FALSE;
}

CBarButtonEx::~CBarButtonEx()
{
	ReleaseImages();
}

void CBarButtonEx::ReleaseImages()
{
	if (m_pIconImage)
		delete (Image *)m_pIconImage;
	m_pIconImage = NULL;

	m_sizeIcon.cx = m_sizeIcon.cy = 0;
	m_sizeDrop.cx = m_sizeDrop.cy = 0;
}


BEGIN_MESSAGE_MAP(CBarButtonEx, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_SETFOCUS()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CBarButtonEx message handlers

BOOL CBarButtonEx::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	m_strText = lpszText;

	DWORD dwOwnDrawST = BS_PUSHBUTTON | BS_OWNERDRAW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	dwStyle |= dwOwnDrawST;
	return CButton::Create(lpszText, dwStyle, rect, pParentWnd, nID);
}

int CBarButtonEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_btnskin.background.pImage
		&& !LoadSkin(ELC_SKIN_STYLE_BUTTON_MAIN_TOOLBAR))
		return -1;

	return 0;
}

BOOL CBarButtonEx::LoadSkin(LPCTSTR lpStyle, BOOL bRedraw)
{
	if (!theSkin.IsValid() || !lpStyle)
		return FALSE;

	PELC_SKIN_BUTTON pSkin = theSkin.GetButtonSkin(lpStyle);
	if (!pSkin)
		return FALSE;

	memcpy(&m_btnskin, pSkin, sizeof(ELC_SKIN_BUTTON));

	if (m_btnskin.dropdown.pImage) {
		m_sizeDrop.cx = ((Image*)m_btnskin.dropdown.pImage)->GetWidth() / m_btnskin.dropdown.nCount;
		m_sizeDrop.cy = ((Image*)m_btnskin.dropdown.pImage)->GetHeight();
	}

	m_ptAdjust = m_btnskin.ptAdjust;

	if (bRedraw)
		Repaint();

	return TRUE;
}

BOOL CBarButtonEx::SetBackgndImage(UINT nResId, LPCTSTR lpszType, int nLeftSplit, int nRightSplit, int nTopSplit, int nBottomSplit)
{
	Image* pImage = NULL;
	if (nResId == 0
		|| !ImageFromIDResource(nResId, lpszType, pImage)) {
		memset(&m_btnskin.background, 0, sizeof(m_btnskin.background));
		return TRUE;
	}
	
	m_btnskin.background.nCount = 4;
	m_btnskin.background.bVertical = 1;

	m_btnskin.background.pImage = pImage;
	m_btnskin.background.nLeftSplit = nLeftSplit;
	m_btnskin.background.nRightSplit = nRightSplit;
	m_btnskin.background.nTopSplit= nTopSplit;
	m_btnskin.background.nBottomSplit = nBottomSplit;

	m_btnskin.background.stretch = 0;
	if (nLeftSplit > 0 && nRightSplit > 0)
		m_btnskin.background.stretch |= EISR_HORIZONTAL;
	if (nTopSplit > 0 && nBottomSplit > 0)
		m_btnskin.background.stretch |= EISR_VERTICAL;

	return TRUE;
}

BOOL CBarButtonEx::SetIconImage(UINT nResId, LPCTSTR lpszType, ELC_BUTTON_ICON_TYPE mode)
{
	if (nResId != 0
		&& m_nIconResId == nResId)
	{
		// image not changed!
		return TRUE;
	}

	m_nIconImageMode = mode;
	m_nIconResId = nResId;

	Image *pImage = (Image *)m_pIconImage;
	if (pImage)
		delete pImage;
	pImage = NULL;

	BOOL bRet = FALSE;
	if (nResId == 0) {
		m_pIconImage = NULL;
		m_sizeIcon.cx = m_sizeIcon.cy = 0;
		bRet = TRUE;
	}
	else if (!ImageFromIDResource(nResId, lpszType, pImage)) {
		if (pImage) 
			delete pImage;

		m_pIconImage = NULL;
		m_sizeIcon.cx = 0;
		m_sizeIcon.cy = 0;
		bRet = FALSE;
	}
	else {
		m_pIconImage = pImage;
		m_sizeIcon.cx = pImage->GetWidth();
		m_sizeIcon.cy = pImage->GetHeight() / mode;
		bRet = TRUE;
	}

	Repaint();

	return bRet;
}

BOOL CBarButtonEx::SetIconImage(UINT nBitmapResId, COLORREF crTransparent, ELC_BUTTON_ICON_TYPE mode)
{
	Image * pImage = (Image *)m_pIconImage;
	if (pImage)
		delete pImage;
	pImage = NULL;

	if (nBitmapResId == 0) {
		m_pIconImage = NULL;
		m_sizeIcon.cx = m_sizeIcon.cy = 0;
		return TRUE;
	}

	CBitmap bm;
	bm.LoadBitmap(nBitmapResId);
	BITMAP bminfo;
	bm.GetBitmap(&bminfo);
	CImageList imglist;
	imglist.Create(bminfo.bmWidth, bminfo.bmHeight, ILC_MASK|ILC_COLOR24, 0, 1);
	imglist.Add(&bm, crTransparent);
	imglist.ExtractIcon(0);
	pImage = Bitmap::FromHICON(imglist.ExtractIcon(0));
	m_pIconImage = pImage;

	if (m_pIconImage) {
		m_sizeIcon.cx = pImage->GetWidth();
		m_sizeIcon.cy = pImage->GetHeight() / mode;
	}

	m_nIconImageMode = mode;

	return (pImage != NULL);
}

inline void CBarButtonEx::SetText(LPCTSTR lpszText)
{
	m_strText = lpszText;
	Repaint();
}

inline void CBarButtonEx::SetText(UINT nResId)
{
	CString strText;
	strText.LoadString(nResId);
	SetText(strText);
}

void CBarButtonEx::SetTextColor(COLORREF color)
{
	m_btnskin.crTextNormal = color;
	m_btnskin.crTextHot = color;
	m_btnskin.crTextDown = color;
	Repaint();
}

void CBarButtonEx::SetBackgndColor(COLORREF color)
{
	m_btnskin.crBackgnd = color;
	Repaint();
}

void CBarButtonEx::SetFont(CFont* pFont)
{
	if (m_ftText.GetSafeHandle())
		m_ftText.DeleteObject();

	LOGFONT lf = {0};
	pFont->GetLogFont(&lf);
	m_ftText.CreateFontIndirect(&lf);

	Repaint();
}

CSize CBarButtonEx::GetIdealSize()
{
	CSize size(0,0);
	ASSERT(m_hWnd);
	if (!::IsWindow(m_hWnd))
		return size;

	CSize sizeBackgnd = CElcSkinDoc::GetImageSize(&m_btnskin.background);
	CSize sizeText(0,0);

	HWND hWnd = NULL;
	if (::IsWindow(m_hWnd))
		hWnd = m_hWnd;

	HDC hdc = ::GetDC(hWnd);
	HGDIOBJ hOldFont = ::SelectObject(hdc, m_ftText.GetSafeHandle());
	::GetTextExtentExPoint(hdc, m_strText, m_strText.GetLength(), 0, 0, 0, &sizeText);
	::SelectObject(hdc, hOldFont);
	::ReleaseDC(hWnd, hdc);

	if (m_bTextlinkerBtn) {
		sizeText.cx += (m_nHMargin<<2);
		sizeText.cy += m_nVMargin;
		return sizeText;
	}

	if (ELCBL_HORIZONTAL == m_layout) {
		if (sizeText.cx > 0)
			size.cx = m_nHMargin + m_sizeIcon.cx + m_nHMargin + sizeText.cx + m_nHMargin + m_nHMargin + m_nHMargin;
		else 
			size.cx = m_nHMargin + m_sizeIcon.cx + m_nHMargin;

		size.cy = m_nVMargin + max(m_sizeIcon.cy, sizeText.cy) + m_nVMargin;
		size.cy = max(sizeBackgnd.cy, size.cy);

		if (ELCBDT_DROPDOWN == m_droptype)
			size.cx += m_sizeDrop.cx;

		//size.cx = max(sizeBackgnd.cx, size.cx);
	}
	else {
		size.cx = m_nHMargin + max(m_sizeIcon.cx, sizeText.cx) + m_nHMargin;
		size.cx = max(sizeBackgnd.cx, size.cx);

		if (sizeText.cy > 0)
			size.cy = m_nVMargin + m_sizeIcon.cy + m_nVMargin + sizeText.cy + m_nVMargin;
		else
			size.cy = m_nVMargin + m_sizeIcon.cy  + m_nVMargin;

		if (ELCBDT_DROPDOWN == m_droptype)
			size.cy += m_sizeDrop.cy;

		//size.cy = max(sizeBackgnd.cy, size.cy);
	}

	return size;
}

CSize CBarButtonEx::GetBackgndImageSize()
{
	CSize size(0,0);
	ASSERT(m_hWnd && m_btnskin.background.pImage);
	if (!::IsWindow(m_hWnd) || !m_btnskin.background.pImage)
		return size;

	if (m_btnskin.background.bVertical) {
		size.cx = ((Image *)m_btnskin.background.pImage)->GetWidth();
		size.cy = ((Image *)m_btnskin.background.pImage)->GetHeight() / m_btnskin.background.nCount;
	}
	else {
		size.cx = ((Image *)m_btnskin.background.pImage)->GetWidth() / m_btnskin.background.nCount;
		size.cy = ((Image *)m_btnskin.background.pImage)->GetHeight();
	}
	
	return size;
}

void CBarButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (NULL == lpDrawItemStruct->hDC)
		return;

	::SetLayout(lpDrawItemStruct->hDC, LAYOUT_LTR);

	if (m_bTextlinkerBtn)
		DrawTextlinkerButton(lpDrawItemStruct);
	else 
		DrawImageButton(lpDrawItemStruct);
}

void CBarButtonEx::DrawImageButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc;
	GetClientRect(&rc);

	CElcMemDC dcMem(CDC::FromHandle(lpDrawItemStruct->hDC), rc);

	//BOOL bMirror = (dcMem.GetLayout() == LAYOUT_RTL);

	ELC_UI_DRAW predraw;
	predraw.hWnd = m_hWnd;
	predraw.type = ELC_UI_DT_PREDRAW;
	predraw.hdc = dcMem.GetSafeHdc();
	predraw.rect = rc;
	if (!GetParent()->SendMessage(WM_ELC_UI_DRAW_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&predraw))
		dcMem.FillSolidRect(rc, m_btnskin.crBackgnd);

	Rect rect(0, 0, rc.Width(), rc.Height());

	Color crBackgnd, crText;
	crBackgnd.SetFromCOLORREF(m_btnskin.crBackgnd);

	Graphics grph(dcMem.GetSafeHdc());

	BOOL bIsPressed = (lpDrawItemStruct->itemState & ODS_SELECTED);
	BOOL bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);

	int nImageIndex = 0;
	if (m_bChecked || bIsPressed) {
		nImageIndex = 2;
		crText.SetFromCOLORREF(m_btnskin.crTextDown);
	}
	else if (bIsDisabled) {
		nImageIndex = 3;
		crText.SetFromCOLORREF(m_btnskin.crTextDisabled);
	}
	else if (m_bMouseOnButton) {
		nImageIndex = 1;
		crText.SetFromCOLORREF(m_btnskin.crTextHot);
	}
	else {
		nImageIndex = 0;
		crText.SetFromCOLORREF(m_btnskin.crTextNormal);
	}

	Rect rcIconDest(0, 0, m_sizeIcon.cx, m_sizeIcon.cy);
	RectF rcText;
	Rect rcDrop(0,0,0,0);

	if (ELCBL_HORIZONTAL == m_layout) {
		rcIconDest.Offset(m_nHMargin, (rect.Height - m_sizeIcon.cy) >> 1);

		if (m_sizeIcon.cx != 0)
			rcText.X = (float)(rcIconDest.X + rcIconDest.Width) + 3;
		else
			rcText.X = 0.0f;

		if (ELCBDT_NORMAL == m_droptype) {
			rcText.Width = (float)(rect.Width - rcText.X);
		}
		else {
			rcDrop.X = rect.Width - m_nHMargin - m_sizeDrop.cx;
			rcDrop.Y = (rect.Height - m_sizeDrop.cy) >> 1;
			rcDrop.Width = m_sizeDrop.cx;
			rcDrop.Height = m_sizeDrop.cy;

			rcText.Width = (float)(rcDrop.X - rcText.X);
		}

		rcText.Y = 2.0f;
		rcText.Height = (float)rect.Height;
	}
	else {
		rcIconDest.Offset((rect.Width - m_sizeIcon.cx) >> 1, m_nVMargin);

		if (m_sizeIcon.cy != 0)
			rcText.Y = (float)(rcIconDest.Y + rcIconDest.Height);
		else 
			rcText.Y = 0.0f;

		if (ELCBDT_NORMAL == m_droptype)
			rcText.Height = (float)rect.Height - rcText.Y;
		else {
			rcDrop.X = (rect.Width - m_sizeDrop.cx) >> 1;
			rcDrop.Y = rect.Height - m_sizeDrop.cy - m_nVMargin;
			rcDrop.Width = m_sizeDrop.cx;
			rcDrop.Height = m_sizeDrop.cy;

			rcText.Height = (float)(rcDrop.Y - rcText.Y);
		}

		rcText.X = 0.0f;
		rcText.Width = (float)rect.Width;
	}

	if ((m_bChecked || bIsPressed) && m_bAnimateDown) {
		rcIconDest.Offset(1, 1);
		rcDrop.Offset(1, 1);
		rcText.Offset(1, 1);
	}

	// draw background image
	if (m_btnskin.background.pImage) {
		thePainter.DrawImage(&grph, &m_btnskin.background, &rect, nImageIndex);
	}

	if (m_pIconImage) {
		if (m_nIconImageMode == ELCBIM_SINGLE) {
			ImageAttributes *pImageAtt = NULL;
			ImageAttributes imageAtt;

			if (bIsDisabled) {
				ColorMatrix colorMatrix = {
					0.3f,  0.3f,  0.3f,  0.0f, 0.0f,
					0.59f, 0.59f, 0.59f, 0.0f, 0.0f,
					0.11f, 0.11f, 0.11f, 0.0f, 0.0f,
					0.0f,  0.0f,  0.0f,  0.5f, 0.0f,
					0.0f,  0.0f,  0.0f,  0.0f, 0.5f 
				};

				imageAtt.SetColorMatrix(&colorMatrix);
				pImageAtt = &imageAtt;
			}

			grph.DrawImage((Image *)m_pIconImage, 
				rcIconDest, 
				0, 
				0, 
				m_sizeIcon.cx, 
				m_sizeIcon.cy, 
				UnitPixel,
				pImageAtt);
		}
		else {
			thePainter.DrawFixedImage(&grph, m_pIconImage, nImageIndex, 4, TRUE, CPoint(rcIconDest.X, rcIconDest.Y));
		}
	}

	if (m_btnskin.dropdown.pImage && m_droptype != ELCBDT_NORMAL) {
		int nIndex = nImageIndex;
		if (m_btnskin.dropdown.nCount == 1)
			nIndex = 0;
		thePainter.DrawFixedImage(
			&grph, 
			m_btnskin.dropdown.pImage, 
			nIndex, 
			m_btnskin.dropdown.nCount, 
			m_btnskin.dropdown.bVertical, 
			CPoint(rcDrop.X, rcDrop.Y));
	}

	if (!m_strText.IsEmpty()) {

		rcText.Offset((REAL)m_ptAdjust.x, (REAL)m_ptAdjust.y);

		LOGFONT lf = {0};
		m_ftText.GetLogFont(&lf);
		Font ftText(lpDrawItemStruct->hDC, &lf);
		StringFormat stringfmt;

		if (m_pIconImage && ELCBL_HORIZONTAL == m_layout)
			stringfmt.SetAlignment(StringAlignmentNear);
		else
			stringfmt.SetAlignment(StringAlignmentCenter);

		stringfmt.SetLineAlignment(StringAlignmentCenter);
		stringfmt.SetFormatFlags(StringFormatFlagsNoWrap);
		stringfmt.SetHotkeyPrefix(HotkeyPrefixHide);

		if (!IsFontSmoothingClearType())
			grph.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
		else 
			grph.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

		grph.DrawString(m_strText, -1, &ftText, rcText, &stringfmt, &SolidBrush(crText));
	}

	if (m_bShowOverlay && m_skinOverlay.pImage) {
		if (bIsDisabled)
			nImageIndex = 3;
		else if (bIsPressed)
			nImageIndex = 2;
		else if (m_bMouseOnButton)
			nImageIndex = 1;
		else 
			nImageIndex = 0;

		int x=0, y=0;

		if ((m_nAlign & ALIGN_H_MASK) == ALIGN_LEFT)
			x = 0;
		else if ((m_nAlign & ALIGN_H_MASK) == ALIGN_RIGHT) 
			x = rc.right - m_sizeOverlay.cx;
		else
			x = (rc.right - m_sizeOverlay.cx) / 2;

		if ((m_nAlign & ALIGN_V_MASK) == ALIGN_TOP)
			y = 0;
		else if ((m_nAlign & ALIGN_V_MASK) == ALIGN_BOTTOM)
			y = rc.bottom - m_sizeOverlay.cy;
		else
			y = (rc.bottom - m_sizeOverlay.cy) / 2;

		Rect rcDest(x, y, m_sizeOverlay.cx, m_sizeOverlay.cy);

		thePainter.DrawImage(
			&grph, 
			&m_skinOverlay,
			&rcDest,
			nImageIndex, 
			1, 
			0); 
	}

	dcMem.DrawMe();
}

void CBarButtonEx::DrawTextlinkerButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc;
	GetClientRect(&rc);

	RectF rect(0.0f, 0.0f, (float)rc.Width(), (float)rc.Height());
	Bitmap bmmem((INT)rc.Width(), (INT)rc.Height(), PixelFormat32bppARGB);

	Color crBackgnd, crText;
	crBackgnd.SetFromCOLORREF(m_btnskin.crBackgnd);

	Graphics grph(&bmmem);
	RectF rcTemp = rect;
	rcTemp.Height += 2;
	rcTemp.Width += 2;
	grph.FillRectangle(&SolidBrush(crBackgnd), rcTemp);
	
	BOOL bIsPressed = (lpDrawItemStruct->itemState & ODS_SELECTED);
	BOOL bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);

	if (!m_strText.IsEmpty()) {
		LOGFONT lf = {0};
		m_ftText.GetLogFont(&lf);

		StringFormat stringfmt;

		if (m_pIconImage)
			stringfmt.SetAlignment(StringAlignmentNear);
		else 
			stringfmt.SetAlignment(StringAlignmentCenter);

		stringfmt.SetLineAlignment(StringAlignmentCenter);
		stringfmt.SetFormatFlags(StringFormatFlagsNoWrap);

		if (m_bChecked || bIsPressed) {
			crText.SetFromCOLORREF(m_btnskin.crTextDown);
			if (m_bAnimateDown)
				rect.Offset(1, 1);
			if (m_nUnderlineStyle == ELCBTS_UNDERLINE_WHEN_HOT || m_nUnderlineStyle == ELCBTS_UNDERLINE_WHEN_ANYTIME)
				lf.lfUnderline = TRUE;
		}
		else if (m_bMouseOnButton) {
			crText.SetFromCOLORREF(m_btnskin.crTextHot);
			if (m_nUnderlineStyle == ELCBTS_UNDERLINE_WHEN_HOT || m_nUnderlineStyle == ELCBTS_UNDERLINE_WHEN_ANYTIME)
				lf.lfUnderline = TRUE;
		}
		else if (bIsDisabled) {
			crText.SetFromCOLORREF(m_btnskin.crTextDisabled);
		}
		else {
			crText.SetFromCOLORREF(m_btnskin.crTextNormal);
			if (m_nUnderlineStyle == ELCBTS_UNDERLINE_WHEN_ANYTIME)
				lf.lfUnderline = TRUE;
		}

		//grph.SetSmoothingMode(SmoothingModeAntiAlias);
		//grph.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

		if (!IsFontSmoothingClearType())
			grph.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
		else 
			grph.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

		grph.DrawString(m_strText, -1, &Font(lpDrawItemStruct->hDC, &lf), rect, &stringfmt, &SolidBrush(crText));
	}

	Graphics gout(lpDrawItemStruct->hDC);
	gout.DrawImage(&bmmem, 0, 0);
}

void CBarButtonEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;
	CButton::OnLButtonDown(nFlags, point);
}

void CBarButtonEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	GetClientRect(&rect);

	if (m_bLButtonDown 
		&& rect.PtInRect(point)
		&& m_hNotifyWnd
		&& m_nNotifyMsg) 
	{
		::PostMessage(m_hNotifyWnd, m_nNotifyMsg, GetDlgCtrlID(), 0);
	}

	m_bLButtonDown = FALSE;
	Invalidate();

	CButton::OnLButtonUp(nFlags, point);
}

void CBarButtonEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	CButton::OnMouseMove(nFlags, point);

	if ((nFlags & MK_LBUTTON) && m_bMouseOnButton == FALSE) {
		return;
	}

	CWnd* pWnd = GetActiveWindow();
	if ((GetCapture() != this) && pWnd != NULL) 
	{
		m_bMouseOnButton = TRUE;
		SetCapture();
		Invalidate();
	}
	else
	{
		POINT pt = point;
		ClientToScreen(&pt);
		CWnd* wndUnderMouse = WindowFromPoint(pt);
		if (wndUnderMouse && wndUnderMouse->m_hWnd != this->m_hWnd)
		{
			if (m_bMouseOnButton == TRUE)
			{
				m_bMouseOnButton = FALSE;
				Invalidate();
			}

			if (!(nFlags & MK_LBUTTON)) 
				ReleaseCapture();
		} else
		{
		}
	}
}

void CBarButtonEx::OnCaptureChanged(CWnd *pWnd) 
{
	if (m_bMouseOnButton == TRUE) {
		m_bMouseOnButton = FALSE;
		ReleaseCapture();
		Invalidate();
	}

	if (pWnd == this) {
		m_bMouseOnButton = TRUE;
		Invalidate();
	}

	CButton::OnCaptureChanged(pWnd);
}

void CBarButtonEx::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);

	ReleaseCapture();
	m_bMouseOnButton = FALSE;
	Invalidate();
}

BOOL CBarButtonEx::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CButton::PreTranslateMessage(pMsg);
}

LRESULT CBarButtonEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (BM_SETCHECK == message) {
		SetCheck((BOOL)wParam);
		return 0;
	}
	else if (BM_GETCHECK == message) {
		return m_bChecked;
	}
	else if (WM_SETTEXT == message) {
		SetText((LPCTSTR)lParam);
	}

	return __super::WindowProc(message, wParam, lParam);
}

BOOL CBarButtonEx::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE; 
}

void CBarButtonEx::SetCheck(BOOL bChecked)
{
	if(bChecked != m_bChecked)
	{
		m_bChecked = bChecked;
		Repaint();
	}
}

void CBarButtonEx::Repaint()
{
	if (!m_hWnd)
		return;

	Invalidate();
}

void CBarButtonEx::SetButtonDropType(int nType)
{
	m_droptype = nType;
	Repaint();
}

BOOL CBarButtonEx::SetDropImage(UINT nResId, LPCTSTR lpszType, ELC_BUTTON_ICON_TYPE mode)
{
	if (nResId == 0) {
		ASSERT(0);
		m_sizeDrop.cx = m_sizeDrop.cy = 0;
		return FALSE;
	}
	m_btnskin.dropdown.bVertical = TRUE;
	m_btnskin.dropdown.stretch = EIST_FIXED;
	m_btnskin.dropdown.nLeftSplit = -1;
	m_btnskin.dropdown.nRightSplit = -1;
	m_btnskin.dropdown.nTopSplit = -1;
	m_btnskin.dropdown.nBottomSplit = -1;

	if (ELCBIM_SINGLE == mode)
		m_btnskin.dropdown.nCount = 1;
	else 
		m_btnskin.dropdown.nCount = 4;

	Image * pImage = NULL;
	if (lpszType == NULL)
		ImageFromIDResource(nResId, RT_PNG, pImage);
	else
		ImageFromIDResource(nResId, lpszType, pImage);

	if (pImage) {
		m_sizeDrop.cx = pImage->GetWidth();
		m_sizeDrop.cy = pImage->GetHeight();
		m_btnskin.dropdown.pImage = pImage;
	}

	return (pImage != NULL);
}

CSize CBarButtonEx::SetTextLinkerButton(LPCTSTR lpszText, int nUnderlineStyle, COLORREF crNormal, COLORREF crHot)
{
	m_strText = lpszText;
	m_bTextlinkerBtn = TRUE;
	m_nUnderlineStyle = nUnderlineStyle;
	m_btnskin.crTextNormal = crNormal;
	m_btnskin.crTextHot = crHot;
	m_btnskin.crTextDown = crHot;

	return GetIdealSize();
}

BOOL CBarButtonEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!m_bTextlinkerBtn) {
		return __super::OnSetCursor(pWnd, nHitTest, message);
	}
	else {
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
		return TRUE;
	}
}

void CBarButtonEx::SetOverlayIcon(LPCTSTR lpszStyle, UINT nAlign, int nFlickerTime, int nFlickerCount)
{
	KillTimer(ID_TIMER_OVERLAY_FLICKER);
	memset(&m_skinOverlay, 0, sizeof(m_skinOverlay));

	if (!lpszStyle) {
		memset(&m_skinOverlay, 0, sizeof(m_skinOverlay));
		m_bShowOverlay = FALSE;
		Invalidate();
		return;
	}

	CElcSkinDoc* pSkinDoc = GetSkinDoc();
	if (!pSkinDoc) {
		return;
	}

	PELC_SKIN_PICTURE pPicture = pSkinDoc->GetPicture(lpszStyle);
	if (!pPicture) {
		return;
	}

	memcpy(&m_skinOverlay, &pPicture->image, sizeof(m_skinOverlay));

	m_sizeOverlay = CElcSkinDoc::GetImageSize(&m_skinOverlay);
	m_nAlign = nAlign;
	m_nFlickerTime = nFlickerTime;
	m_nFlickerCount = nFlickerCount;

	m_bShowOverlay = TRUE;

	m_nFlickerCounter = 0;

	if (m_nFlickerTime > 0)
		SetTimer(ID_TIMER_OVERLAY_FLICKER, m_nFlickerTime, NULL);

	Invalidate();
}

void CBarButtonEx::OnTimer(UINT_PTR nIDEvent)
{
	if (ID_TIMER_OVERLAY_FLICKER == nIDEvent) {
		m_bShowOverlay = !m_bShowOverlay;
		Invalidate();

		m_nFlickerCounter ++; 
		if ((m_nFlickerCount > 0) && (m_nFlickerCounter > m_nFlickerCount)) {
			m_bShowOverlay = TRUE;
			Invalidate();

			KillTimer(ID_TIMER_OVERLAY_FLICKER);
		}
	}

	__super::OnTimer(nIDEvent);
}