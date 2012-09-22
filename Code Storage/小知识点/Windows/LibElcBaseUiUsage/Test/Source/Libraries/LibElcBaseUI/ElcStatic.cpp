// ElcStatic.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcStatic.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CElcStatic

IMPLEMENT_DYNAMIC(CElcStatic, CStatic)

CElcStatic::CElcStatic()
{
	m_crText = RGB(16,64,128);
	m_bUseTextColor = FALSE;

	m_crBackgnd = RGB(237,242,249);
	m_bUseBackgndColor = FALSE;

	m_crEtched1 = RGB(255,255,255);
	m_crEtched2 = RGB(204,204,204);

	m_pImage = 0;
}

CElcStatic::~CElcStatic()
{
	if (m_pImage)
		delete m_pImage;
	m_pImage = NULL;
}


BEGIN_MESSAGE_MAP(CElcStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



void CElcStatic::SetTextColor(COLORREF cr, BOOL bOn)
{
	m_crText = cr;
	m_bUseTextColor = bOn;
	Repaint();
}

void CElcStatic::SetBKColor(COLORREF cr, BOOL bOn)
{
	m_crBackgnd = cr;
	m_bUseBackgndColor = bOn;
	Repaint();
}

void CElcStatic::SetEtchedColor(COLORREF cr1, COLORREF cr2)
{
	m_crEtched1 = cr1;
	m_crEtched2 = cr2;
	Repaint();
}

BOOL CElcStatic::SetImage(UINT nResId, LPCTSTR lpszType, BOOL bRealSizeImage)
{
	if (!::IsWindow(m_hWnd)) {
		ASSERT(0);
		return FALSE;
	}

	if (m_pImage)
		delete m_pImage;
	m_pImage = NULL;

	if (!ImageFromIDResource(nResId, lpszType, m_pImage)) {
		if (m_pImage)
			delete m_pImage;
		return FALSE;
	}

	if (bRealSizeImage) {
		SetWindowPos(NULL, 0, 0, m_pImage->GetWidth(), m_pImage->GetHeight(), SWP_NOMOVE | SWP_NOACTIVATE);
	}

	Repaint();

	return TRUE;
}

LRESULT CElcStatic::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_ENABLE == message
		|| WM_SETTEXT == message)
	{
		SetRedraw(FALSE);
		LRESULT lResult = __super::WindowProc(message, wParam, lParam);
		SetRedraw(TRUE);
		Invalidate(FALSE);
		return lResult;
	}

	return __super::WindowProc(message, wParam, lParam);
}

void CElcStatic::OnPaint()
{
	CPaintDC dc(this);

	DWORD dwStyle = GetStyle();
	DWORD dwStaticType = (dwStyle & SS_TYPEMASK);

	CRect rect;
	GetClientRect(rect);

	CElcMemDC dcMem(&dc, rect);

	if (m_bUseBackgndColor) {
		dcMem.FillSolidRect(rect, m_crBackgnd);
	}
	else if (GetParent()) {
		HBRUSH hBrush = (HBRUSH)GetParent()->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)dcMem.GetSafeHdc(), (LPARAM)m_hWnd);
		if (hBrush)
			::FillRect(dcMem.GetSafeHdc(), rect, hBrush);
	}

	if (m_bUseTextColor) {
		dcMem.SetTextColor(m_crText);
	}

	if (m_pImage) {
		Graphics g(dcMem.GetSafeHdc());

		Rect rcImage(0, 0, m_pImage->GetWidth(), m_pImage->GetHeight());
		Rect rcDest;
		if (dwStyle & SS_CENTERIMAGE) {
			rcDest.X = rect.left + (rect.Width() - rcImage.Width) / 2;
			rcDest.Y = rect.top + (rect.Height() - rcImage.Height) / 2;
			rcDest.Width = rcImage.Width;
			rcDest.Height = rcImage.Height;
		}
		else {
			rcDest.X = rect.left;
			rcDest.Y = rect.top;
			rcDest.Width = rect.Width();
			rcDest.Height = rect.Height();
		}

		thePainter.DrawStretchImage(&g, m_pImage, &rcDest, &rcImage, 0);
	}
	else if (dwStaticType == SS_ICON
		|| dwStaticType == SS_BITMAP
		|| dwStaticType == SS_OWNERDRAW
		|| dwStaticType == SS_USERITEM
		|| dwStaticType == SS_ENHMETAFILE) 
	{
		// not support type
		// pls use SetImage() to replace
		ASSERT(0);
		return;
	}
	else if (dwStaticType == SS_BLACKRECT
		|| dwStaticType == SS_GRAYRECT
		|| dwStaticType == SS_WHITERECT
		|| dwStaticType == SS_BLACKFRAME
		|| dwStaticType == SS_GRAYFRAME
		|| dwStaticType == SS_WHITEFRAME)
	{
		// not support
		// pls use Standard MFC class : CStatic
		ASSERT(0);
		return;
	}
	else { 
		if (dwStaticType == SS_ETCHEDHORZ) {
			CPen pen1(PS_SOLID, 1, m_crEtched1);
			CPen * pOld = dcMem.SelectObject(&pen1);
			dcMem.MoveTo(rect.left, rect.top);
			dcMem.LineTo(rect.right - 2, rect.top);

			CPen pen2(PS_SOLID, 1, m_crEtched2);
			dcMem.SelectObject(&pen2);
			dcMem.MoveTo(rect.left + 2, rect.top+1);
			dcMem.LineTo(rect.right, rect.top+1);

			dcMem.SelectObject(&pOld);
		}
		else if (dwStaticType == SS_ETCHEDVERT) {
			CPen pen1(PS_SOLID, 1, m_crEtched1);
			CPen * pOld = dcMem.SelectObject(&pen1);
			dcMem.MoveTo(rect.left, rect.top);
			dcMem.LineTo(rect.left, rect.bottom - 2);

			CPen pen2(PS_SOLID, 1, m_crEtched2);
			dcMem.SelectObject(&pen2);
			dcMem.MoveTo(rect.left + 1, rect.top + 2);
			dcMem.LineTo(rect.left + 1, rect.bottom);

			dcMem.SelectObject(&pOld);
		}
		else if (dwStaticType == SS_ETCHEDFRAME) {
			// not implement
			// support later...
			ASSERT(0);
		}
		else {
			CFont* pOldFont = dcMem.SelectObject(GetFont());
			UINT nFormat = 0;
			
			if (dwStaticType == SS_LEFT) {
				nFormat = DT_LEFT | DT_NOFULLWIDTHCHARBREAK | DT_WORDBREAK;
			}
			else if (dwStaticType == SS_CENTER) {
				nFormat = DT_CENTER | DT_NOFULLWIDTHCHARBREAK | DT_WORDBREAK;
			}
			else if (dwStaticType == SS_RIGHT) {
				nFormat = DT_RIGHT | DT_NOFULLWIDTHCHARBREAK | DT_WORDBREAK;
			}
			else if (dwStaticType == SS_SIMPLE) {
				nFormat = DT_SINGLELINE | DT_VCENTER;
			}
			else if (dwStaticType == SS_LEFTNOWORDWRAP) {
				nFormat = DT_SINGLELINE;
			}

			/*
			// these codes are been commented because something errors are not been fiexd yet
			if (dwStyle & SS_ENDELLIPSIS) {
				nFormat |= DT_MODIFYSTRING | DT_END_ELLIPSIS | DT_WORD_ELLIPSIS;
			}
			if (dwStyle & SS_PATHELLIPSIS) {
				nFormat |= DT_MODIFYSTRING | DT_PATH_ELLIPSIS | DT_WORD_ELLIPSIS;
			}*/

			if (dwStyle & SS_NOPREFIX) {
				nFormat |= DT_NOPREFIX;
			}

			if (dwStyle & SS_EDITCONTROL) {
				nFormat |= DT_EDITCONTROL;
			}

			CString strText;
			GetWindowText(strText);
			dcMem.DrawText(strText, rect, nFormat);

			dcMem.SelectObject(pOldFont);
		}
	}

	dcMem.DrawMe();
}


