#include "stdafx.h"
#include "IconButton.h"

HMODULE			CIconButton::m_hXTheme = NULL;
HTHEME			CIconButton::m_hTheme = NULL;
pFnOpenTheme	CIconButton::m_pFnOpenTheme = NULL;
pFnDrawPBk		CIconButton::m_pFnDrawPBk = NULL;
pFnDrawBk		CIconButton::m_pFnDrawBk = NULL;
BOOL			CIconButton::m_bThemeLoaded = FALSE;

// CIconButton
IMPLEMENT_DYNAMIC(CIconButton, CButton)

CIconButton::CIconButton()
{
	m_bTracking = FALSE;
	m_bhover = FALSE;
	m_bEnableToggle = FALSE;
	m_bToggle = FALSE;

	m_hIcon = NULL;
	m_nImageWidth = 0;
	m_nImageHeight = 0;

	m_strTipText = _T("");
	m_CtlToolTip.m_hWnd = NULL;

	if(!m_bThemeLoaded)
	{
		InitXTheme();
	}
}

CIconButton::~CIconButton()
{
	if (m_hXTheme && m_bThemeLoaded)
	{
		FreeLibrary(m_hXTheme);
		m_hXTheme = NULL;
		m_bThemeLoaded = FALSE;
	}

	if(m_hIcon)
	{
		::DestroyIcon(m_hIcon);
	}
	if(m_bitmap.GetSafeHandle())
	{
		m_bitmap.DeleteObject();
	}
}

BEGIN_MESSAGE_MAP(CIconButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CIconButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW,SWP_FRAMECHANGED);
	CButton::PreSubclassWindow();
}

void CIconButton::InitXTheme()
{
	m_hXTheme = LoadLibrary(_T("UXTHEME.DLL"));

	if (m_hXTheme)
	{
		m_pFnOpenTheme = (pFnOpenTheme)GetProcAddress(m_hXTheme,"OpenThemeData");
		m_pFnDrawPBk = (pFnDrawPBk)GetProcAddress(m_hXTheme,"DrawThemeParentBackground");
		m_pFnDrawBk = (pFnDrawBk)GetProcAddress(m_hXTheme,"DrawThemeBackground");

		m_bThemeLoaded = TRUE;
	}
	else
	{
		TRACE(_T("ERROR - UXTHEME.DLL not accessible\n"));
		ASSERT(FALSE);
	}
}

void CIconButton::SetIcon(UINT nIcon)
{
	if(m_hIcon)
	{
		::DestroyIcon(m_hIcon);
	}
		
	HINSTANCE	hInstResource	= NULL;
	// Find correct resource handle
	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nIcon), RT_GROUP_ICON);
	
	if(hInstResource)
	{
		m_hIcon = (HICON)::LoadImage(hInstResource,
					MAKEINTRESOURCE(nIcon),
					IMAGE_ICON,
					0,
					0,
					0);

		if(m_hIcon)
		{
			ICONINFO iconinfo;
			::GetIconInfo(m_hIcon, &iconinfo);
			m_nImageWidth = iconinfo.xHotspot * 2;
			m_nImageHeight = iconinfo.yHotspot * 2;
			if (iconinfo.hbmColor)
			{
				::DeleteObject(iconinfo.hbmColor);
			}
			if (iconinfo.hbmMask)
			{
				::DeleteObject(iconinfo.hbmMask);
			}
			SizeToContent();
		}
	}
}

void CIconButton::SetBitmap(UINT nBmID)
{
	if(m_bitmap.GetSafeHandle())
	{
		m_bitmap.DeleteObject();
	}

	m_bitmap.LoadBitmap(nBmID);

	if(m_bitmap.GetSafeHandle())
	{
		BITMAP bm;
		m_bitmap.GetBitmap(&bm);
		m_nImageWidth = bm.bmWidth;
		m_nImageHeight = bm.bmHeight;
		SizeToContent();
	}
}

void CIconButton::SizeToContent()
{
	int nWidth = (int)(6 * m_nImageWidth / 5.0f);
	int nHeight = (int)(6 * m_nImageHeight / 5.0f);
	SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_NOMOVE);
}

void CIconButton::SetTooltipText(int nTextID, BOOL bActivate)
{
	CString sText;
	sText.LoadString(nTextID);
	
	if(!sText.IsEmpty()) 
	{
		SetTooltipText((LPCTSTR)sText, bActivate);
	}
}

void CIconButton::SetTooltipText(LPCTSTR lpszText, BOOL bActivate)
{
	if (lpszText == NULL) 
	{
		return;
	}

	if (m_CtlToolTip.m_hWnd == NULL)
	{
		m_CtlToolTip.Create(this);
		m_CtlToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 400);

		if(m_CtlToolTip.GetToolCount() == 0)
		{
			m_CtlToolTip.AddTool(this, lpszText);
		}
	}

	m_CtlToolTip.UpdateTipText(lpszText, this);
	m_CtlToolTip.Activate(bActivate);
	m_strTipText = lpszText;
}

void CIconButton::ActivateTooltip(BOOL bEnable)
{
	if (m_CtlToolTip.GetToolCount() == 0) 
	{
		return;
	}
	m_CtlToolTip.Activate(bEnable);
}

BOOL CIconButton::PreTranslateMessage(MSG* pMsg)
{
	if(m_CtlToolTip.m_hWnd)
	{
		m_CtlToolTip.RelayEvent(pMsg);
	}

	return CButton::PreTranslateMessage(pMsg);
}

void CIconButton::SetToggle(BOOL bEnble)
{
	m_bEnableToggle = bEnble;
	m_bToggle = FALSE;

	Invalidate();
}

LRESULT CIconButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_LBUTTONDBLCLK)
	{
		message = WM_LBUTTONDOWN;
	}
	return CButton::DefWindowProc(message, wParam, lParam);
}

void CIconButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rectItem = lpDrawItemStruct->rcItem;

	// set up for double buffering
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rectItem.Width(), rectItem.Height());
	CBitmap *pOldBitmap = dcMem.SelectObject(&bmp);
	dcMem.SetBkMode(TRANSPARENT);

	DrawBackGround(&dcMem);

	// button state
	BOOL bDrawFocusRect = !(lpDrawItemStruct->itemState & ODS_NOFOCUSRECT);
	BOOL bIsFocused     = lpDrawItemStruct->itemState & ODS_FOCUS;
	BOOL bIsPressed		= lpDrawItemStruct->itemState & ODS_SELECTED;

	if(m_bEnableToggle)
	{
		bIsPressed = m_bToggle;
	}

	DWORD state = bIsPressed ? PBS_PRESSED : PBS_NORMAL;

	if (state == PBS_NORMAL)
	{
		if (m_bhover)
		{
			state = PBS_HOT;
		}
	}

	if(m_hTheme == NULL)
	{
		m_hTheme = m_pFnOpenTheme(m_hWnd, _T("Toolbar"));
	}

	m_pFnDrawPBk(::GetParent(m_hWnd), dcMem.m_hDC, &rectItem);
	m_pFnDrawBk(m_hTheme, dcMem.m_hDC, BP_PUSHBUTTON, state, &rectItem, NULL);

	if(m_hIcon)
	{
		DrawIcon(&dcMem,rectItem);
	}
	else if(m_bitmap.GetSafeHandle())
	{
		DrawBitmap(&dcMem,rectItem);
	}

	// end double buffering
	pDC->BitBlt(0, 0, rectItem.Width(), rectItem.Height(),
		&dcMem, 0, 0, SRCCOPY);			

	// swap back the original bitmap
	dcMem.SelectObject(pOldBitmap);
	bmp.DeleteObject();
	dcMem.DeleteDC();
}

void CIconButton::DrawIcon(CDC* pDC,CRect& rect)
{
	CPoint ptCenter = rect.CenterPoint();
	UINT nFlags = DSS_NORMAL | DST_ICON;
	ptCenter.Offset(- m_nImageWidth / 2,- m_nImageHeight / 2);
	CPoint ptLT = ptCenter;
	pDC->DrawState(ptLT,CSize(m_nImageWidth,m_nImageHeight),m_hIcon,nFlags,(CBrush *)NULL);
}

void CIconButton::DrawBitmap(CDC* pDC,CRect& rect)
{
	CPoint ptCenter = rect.CenterPoint();
	UINT nFlags = DSS_NORMAL | DST_BITMAP;
	ptCenter.Offset(- m_nImageWidth / 2,- m_nImageHeight / 2);
	CPoint ptLT = ptCenter;
	pDC->DrawState(ptLT, CSize(m_nImageWidth, m_nImageHeight), &m_bitmap, nFlags, (CBrush *)NULL);
}

void CIconButton::DrawBackGround(CDC* pDC)
{
	CRect rect;
	GetWindowRect(&rect);
	GetParent()->ScreenToClient(&rect);

	// repaint rect for button on parent's window without button,
	// to get the original background of the parent
	BOOL bVisible = GetStyle() & WS_VISIBLE;
	ModifyStyle(WS_VISIBLE, 0);
	GetParent()->InvalidateRect(&rect);
	GetParent()->UpdateWindow();

	// get dc for parent's window
	CDC *pDCParent = GetParent()->GetDC();
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(),pDCParent, rect.left, rect.top, SRCCOPY);

	// restore visibility
	if (bVisible)
	{
		ModifyStyle(0, WS_VISIBLE);
		GetParent()->UpdateWindow();
	}

	ReleaseDC(pDCParent);
}

void CIconButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags, point);
}

LRESULT CIconButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if(m_CtlToolTip.m_hWnd)
	{
		m_CtlToolTip.UpdateTipText(m_strTipText, this);
	}
	m_bhover = FALSE;
	m_bTracking = FALSE;
	Invalidate();
	return 0;
}

LRESULT CIconButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bhover = TRUE;
	Invalidate();
	return 0;
}
void CIconButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bEnableToggle)
	{
		m_bToggle = !m_bToggle;
		Invalidate();
	}

	CButton::OnLButtonUp(nFlags, point);
}
