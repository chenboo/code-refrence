#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcNonWndButton.h"

#define ENWB_HMARGIN		2
#define ENWB_VMARGIN		2

/////////////////////////////////////////
// CElcNonWndButton

CElcNonWndButton::CElcNonWndButton(void)
{
	memset(&m_icon, 0, sizeof(m_icon));
	memset(&m_background, 0, sizeof(m_background));

	m_hParent = NULL;
	m_hNotify = NULL;
	m_nMsg = NULL;
	m_nId = 0;
	m_nIconResId = 0;

	m_rect.SetRectEmpty();

	GetMSShellDlgFont(m_ftText);

	m_state = ENWS_NORMAL;
	m_bMouseDown = FALSE;
	m_bReleaseIconRes = FALSE;
	m_nEnableRedraw = TRUE;
}

CElcNonWndButton::~CElcNonWndButton(void)
{
	if (m_bReleaseIconRes && m_icon.pImage)
		delete (Image *)m_icon.pImage;
}

BOOL CElcNonWndButton::Create(LPCTSTR lpszText, HWND hParent, CRect rect, UINT nId)
{
	m_strText = lpszText;
	m_hParent = hParent;
	m_nId = nId;
	m_rect = rect;

	return TRUE;
}

BOOL CElcNonWndButton::SetIcon(UINT nResId, LPCTSTR lpszType, int nCount, BOOL bVertical)
{
	if (m_nIconResId == nResId)
		return TRUE;

	if (m_bReleaseIconRes && m_icon.pImage) {
		delete (Image *)m_icon.pImage;
		memset(&m_icon, 0, sizeof(m_icon));
	}

	m_icon.nCount = nCount;
	m_icon.bVertical = bVertical;
	m_icon.stretch = EIST_FIXED;
	m_icon.nLeftSplit = -1;
	m_icon.nRightSplit = -1;
	m_icon.nTopSplit = -1;
	m_icon.nBottomSplit = -1;

	Image* pImage = NULL;
	BOOL bRet = ImageFromIDResource(nResId, lpszType, pImage);
	if (!bRet) {
		if (pImage)
			delete pImage;
		memset(&m_icon, 0, sizeof(m_icon));
		m_nIconResId = 0;
	}
	else {
		m_icon.pImage = pImage;
		m_nIconResId = nResId;
	}

	m_bReleaseIconRes = TRUE;

	Repaint(m_rect);

	return bRet;
}

void CElcNonWndButton::SetIcon(PELC_SKIN_IMAGE pIcon)
{
	if (m_bReleaseIconRes && m_icon.pImage) {
		delete (Image *)m_icon.pImage;
		memset(&m_icon, 0, sizeof(m_icon));
	}

	memcpy(&m_icon, pIcon, sizeof(m_icon));
	m_bReleaseIconRes = FALSE;

	Repaint(m_rect);
}

void CElcNonWndButton::LoadBackgroundSkin(LPCTSTR lpStyle)
{
	PELC_SKIN_BUTTON pSkin = theSkin.GetButtonSkin(lpStyle);
	if (pSkin) 
		memcpy(&m_background, pSkin, sizeof(m_background));
}

CSize CElcNonWndButton::GetIdealSize()
{
	CSize size(0,0);

	if (m_icon.pImage) {
		size.cx = ((Image*)m_icon.pImage)->GetWidth() + ENWB_HMARGIN * 2;
		size.cy = ((Image*)m_icon.pImage)->GetHeight() + ENWB_VMARGIN * 2 / m_icon.nCount;
	}

	if (!m_strText.IsEmpty()) {
		HDC hdc = ::GetDC(NULL);

		HGDIOBJ hFont = NULL;
		if (m_ftText.GetSafeHandle()) 
			hFont = ::SelectObject(hdc, m_ftText.GetSafeHandle());
		else
			hFont = ::SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

		CSize sizeText;
		::GetTextExtentExPoint(hdc, 
			m_strText,
			m_strText.GetLength(),
			0,
			NULL,
			NULL,
			&sizeText);

		if (hFont)
			::SelectObject(hdc, hFont);
		::ReleaseDC(NULL, hdc);

		if (!m_icon.pImage) {
			size.cx = sizeText.cx + ENWB_HMARGIN * 2;
			size.cy = sizeText.cy + ENWB_VMARGIN * 2;
		}
		else {
			size.cx += sizeText.cx + ENWB_HMARGIN;
			size.cy = max(size.cy, (sizeText.cy + ENWB_VMARGIN * 2));
		}
	}

	return size;
}

CSize CElcNonWndButton::GetBkImageSize()
{
	CSize size(0,0);
	Image *pImage = (Image *)m_background.background.pImage;
	if (pImage) {
		size.cx = pImage->GetWidth();
		size.cy = pImage->GetHeight();
	}

	return size;
}

CSize CElcNonWndButton::GetIconSize()
{
	CSize size(0,0);
	Image *pImage = (Image *)m_icon.pImage;
	if (pImage) {
		size.cx = pImage->GetWidth();
		size.cy = pImage->GetHeight();
	}

	return size;
}

inline UINT CElcNonWndButton::GetId()
{
	return m_nId;
}

inline BOOL CElcNonWndButton::PtInRect(CPoint point)
{
	return m_rect.PtInRect(point);
}

inline CRect CElcNonWndButton::GetRect()
{
	return m_rect;
}

void CElcNonWndButton::Move(CRect rcNew, BOOL bRedraw)
{
	m_rect = rcNew;
	if (bRedraw)
		Repaint(rcNew);
}

void CElcNonWndButton::SetRedraw(BOOL bEnable)
{
	m_nEnableRedraw = bEnable;
}

void CElcNonWndButton::Draw(PVOID pvGraphics)
{
	if (!pvGraphics)
		return;

	Rect rect(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height());

	// draw background image 
	if (m_background.background.pImage) {
		thePainter.DrawImage(pvGraphics,
			&m_background.background,
			&rect,
			m_state,
			1,
			NULL);
	}

	// draw icon
	if (m_icon.pImage) {
		thePainter.DrawImage(pvGraphics,
			&m_icon,
			&rect,
			m_state,
			1,
			NULL);
	}

	if (!m_strText.IsEmpty()) {
		LOGFONT lf = {0};
		m_ftText.GetLogFont(&lf);

		FontFamily family(lf.lfFaceName);
		Font ftText(&family, 9);

		StringFormat stringfmt;
		stringfmt.SetAlignment(StringAlignmentCenter);
		stringfmt.SetLineAlignment(StringAlignmentCenter);
		stringfmt.SetHotkeyPrefix(HotkeyPrefixHide);

		Color crText;
		crText.SetFromCOLORREF(m_background.crTextNormal);

		RectF rcText;
		rcText.X = (REAL)m_rect.left;
		rcText.Y = (REAL)m_rect.top + 4;
		rcText.Width = (REAL)m_rect.Width();
		rcText.Height = (REAL)m_rect.Height();

		((Graphics *)pvGraphics)->DrawString(m_strText, 
			-1, 
			&ftText, 
			rcText, 
			&stringfmt, 
			&SolidBrush(crText));
	}
}

void CElcNonWndButton::Repaint(CRect rect)
{
	if (!m_nEnableRedraw || !::IsWindow(m_hParent))
		return;

	::RedrawWindow(m_hParent, &rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

BOOL CElcNonWndButton::MouseDown(CPoint point)
{
	if (!m_rect.PtInRect(point))
		return FALSE;

	if (m_state == ENWS_DISABLED)
		return TRUE;

	m_bMouseDown = TRUE;
	m_state = ENWS_DOWN;

	Repaint(m_rect);

	return TRUE;
}

BOOL CElcNonWndButton::MouseUp(CPoint point)
{
	if (!m_rect.PtInRect(point))
		return FALSE;

	if (m_state == ENWS_DISABLED)
		return TRUE;

	if (m_state != ENWS_DISABLED && m_bMouseDown) {
		if (m_hNotify && m_nMsg)
			::PostMessage(m_hNotify, m_nMsg, m_nId, 0);
		else if (m_hNotify)
			::PostMessage(m_hNotify, WM_COMMAND, m_nId, 0);
		else
			::PostMessage(m_hParent, WM_COMMAND, m_nId, 0);
	}

	m_bMouseDown = FALSE;
	m_state = ENWS_NORMAL;

	Repaint(m_rect);

	return TRUE;
}

BOOL CElcNonWndButton::MouseMove(CPoint point)
{
	if (!m_rect.PtInRect(point)) {
		if (m_state != ENWS_DISABLED && m_state != ENWS_NORMAL) {
			m_state = ENWS_NORMAL;
			Repaint(m_rect);
		}
		return FALSE;
	}

	if (m_state == ENWS_DISABLED)
		return TRUE;

	if (m_bMouseDown) {
		if (m_state != ENWS_DOWN) {
			m_state = ENWS_DOWN;
			Repaint(m_rect);
		}
	}
	else if (m_state != ENWS_HOT) {
		m_state = ENWS_HOT;
		Repaint(m_rect);
	}

	return TRUE;
}

void CElcNonWndButton::CaptureChanged()
{
	if (m_state == ENWS_DISABLED)
		return;

	m_bMouseDown = FALSE;
	m_state = ENWS_NORMAL;

	Repaint(m_rect);
}
