#include "stdafx.h"
#include "BarButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum TIMERID {
	ID_TIMER_TRACK_MOUSE = 1,
};

/////////////////////////////////////////////////////////////////////////////
// CBarButton

BOOL GetRegionfromBitmap(RECT r, CRgn *pRgn, CBitmap *pBitmap, COLORREF keyColor, COLORREF cTolerance)
{
	if (!pBitmap) 
		return FALSE;

	if (!pRgn) 
		return FALSE;

	BYTE lr = (BYTE)max(0, (int)GetRValue(keyColor) - (int)GetRValue(cTolerance));
	BYTE lg = (BYTE)max(0, (int)GetGValue(keyColor) - (int)GetGValue(cTolerance));
	BYTE lb = (BYTE)max(0, (int)GetBValue(keyColor) - (int)GetBValue(cTolerance));
	BYTE hr = (BYTE)min((int)0xff, (int)GetRValue(keyColor) + (int)GetRValue(cTolerance));
	BYTE hg = (BYTE)min((int)0xff, (int)GetGValue(keyColor) + (int)GetGValue(cTolerance));
	BYTE hb = (BYTE)min((int)0xff, (int)GetBValue(keyColor) + (int)GetBValue(cTolerance));

	CDC					memDC;
	CBitmap				*pOldMemBmp = NULL;
	CRect				cRect(r);
	int					x, y;
	int					n=0;		//number of transparent pixels
	CRgn				rgnTemp;

	memDC.CreateCompatibleDC(NULL);
	pOldMemBmp = memDC.SelectObject(pBitmap);
	pRgn->CreateRectRgn(0, 0, cRect.Width(), cRect.Height());

	//COLORREF m_Color = memDC.GetPixel(0, 0);
	for (x = 0; x <= r.right - r.left; x++)
	{
		for (y = 0; y <= r.bottom - r.top; y++)
		{
			COLORREF cr = memDC.GetPixel(x,y);
			BYTE b = GetRValue(cr);
			if (lr <= b && b <= hr) 
			{
				b = GetGValue(cr);
				if (lg <= b && b <= hg) 
				{
					b = GetBValue(cr);
					if (lb <= b && b <= hb) 
					{
						rgnTemp.CreateRectRgn(x, y, x + 1, y + 1);
						pRgn->CombineRgn(pRgn, &rgnTemp, RGN_XOR);
						rgnTemp.DeleteObject();	
						++n;
					}
				}
			}
		}
	}

	if (pOldMemBmp) 
		memDC.SelectObject(pOldMemBmp);
	memDC.DeleteDC();

	return n > 0;
}

BOOL ParseBitmap(CBitmap* pbmSrc, CBitmap* pbmDest, int nNums)
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


IMPLEMENT_DYNAMIC(CBarButton, CButton)

CBarButton::CBarButton()
{
	m_hNotify = NULL;
	m_uMsg = NULL;

	m_bNotifyMouseIn = FALSE;
	m_hNotifyMouse = NULL;
	m_uMouseMsg = NULL;

	m_bMouseOnButton = FALSE;
	m_bChecked = FALSE;
	m_bLButtonDown = FALSE;

	m_bShowCaption = FALSE;
	m_crCaption = RGB(0,0,0);
	GetDefaultFont(m_ftCaption);

	m_bStretchBitmap = FALSE;
	m_bTransparent = FALSE;
	m_crTransparent = RGB(255,0,255);
	m_crBkColor = ::GetSysColor(COLOR_BTNFACE);

	m_sizeBmp = CSize(0,0);
}

CBarButton::~CBarButton()
{
}

BEGIN_MESSAGE_MAP(CBarButton, CButton)
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarButton message handlers

BOOL CBarButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (lpszCaption && _tcslen(lpszCaption) > 0)
	{
		m_strCaption = lpszCaption;
		m_bShowCaption = TRUE;
	}
	else
	{
		m_bShowCaption = FALSE;
	}

	DWORD dwOwnDrawST = BS_PUSHBUTTON | BS_OWNERDRAW;
	dwStyle |= dwOwnDrawST;
	return CButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}

void CBarButton::SetNotifyWindow(HWND hWnd, UINT uMsg)
{
	m_hNotify = hWnd;
	m_uMsg = uMsg;
}

void CBarButton::SetNotifyMouseIn(BOOL bNotify, HWND hWnd, UINT uMsg)
{
	m_bNotifyMouseIn = bNotify;
	m_hNotifyMouse = hWnd;
	m_uMouseMsg = uMsg;
}

void CBarButton::SetCaption(LPCTSTR lpszCaption, BOOL bShowCaption)
{
	m_strCaption = lpszCaption;
	m_bShowCaption = bShowCaption;
}

void CBarButton::SetFont(CFont* pFont)
{
	LOGFONT lf = { 0 };
	pFont->GetLogFont(&lf);
	if (m_ftCaption.GetSafeHandle())
		m_ftCaption.DeleteObject();
	m_ftCaption.CreateFontIndirect(&lf);

	Repaint();
}

void CBarButton::SetFontColor(COLORREF color)
{
	m_crCaption = color;
	Repaint();
}

void CBarButton::SetBitmap(CBitmap& bmpNormal, CBitmap& bmpOver, CBitmap& bmpDown, CBitmap& bmpDisabled)
{
	for (int i=0; i < 4; i++) 
	{
		if (m_bmBtn[i].GetSafeHandle())
			m_bmBtn[i].DeleteObject();
	}

	m_bmBtn[0].Attach(bmpNormal);
	m_bmBtn[1].Attach(bmpOver);
	m_bmBtn[2].Attach(bmpDown);
	m_bmBtn[3].Attach(bmpDisabled);

	BITMAP bm;
	m_bmBtn[0].GetBitmap(&bm);
	m_sizeBmp.cx = bm.bmWidth;
	m_sizeBmp.cy = bm.bmHeight;
}

void CBarButton::SetBitmap(UINT nBimapID)
{
	ASSERT(nBimapID > 0);

	CBitmap bmSrc;
	if (!bmSrc.Attach((HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nBimapID), 
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)))
		return;

	for (int i=0; i < 4; i++) {
		if (m_bmBtn[i].GetSafeHandle())
			m_bmBtn[i].DeleteObject();
	}

	ParseBitmap(&bmSrc, m_bmBtn, 4);

	BITMAP bm;
	m_bmBtn[0].GetBitmap(&bm);
	m_sizeBmp.cx = bm.bmWidth;
	m_sizeBmp.cy = bm.bmHeight;

	if (::IsWindow(m_hWnd))
		Invalidate();
}

CSize CBarButton::GetBitmapSize()
{
	return m_sizeBmp;
}

void CBarButton::SetStretchBitmap(BOOL bOn)
{
	m_bStretchBitmap = bOn;
}

void CBarButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{	
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (NULL == pDC)
		return;
	
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	if (NULL == dcMem.m_hDC)
		return;

	BOOL bIsPressed  = (lpDrawItemStruct->itemState & ODS_SELECTED);
	BOOL bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);

	CBitmap *pOldBmp = NULL;
	if (m_bChecked)
		pOldBmp = dcMem.SelectObject(&m_bmBtn[2]);
	else if (bIsDisabled)
		pOldBmp = dcMem.SelectObject(&m_bmBtn[3]);
	else if (bIsPressed)
		pOldBmp = dcMem.SelectObject(&m_bmBtn[2]);
	else if (m_bMouseOnButton)
		pOldBmp = dcMem.SelectObject(&m_bmBtn[1]);
	else 
		pOldBmp = dcMem.SelectObject(&m_bmBtn[0]);

	CRect rcClient;
	GetClientRect(&rcClient);

	if (!m_bTransparent)
		pDC->FillSolidRect(rcClient, m_crBkColor);

	if (m_bStretchBitmap)
	{
		pDC->TransparentBlt(0, 0, rcClient.Width(), rcClient.Height(), 
			&dcMem, 0, 0, m_sizeBmp.cx, m_sizeBmp.cy, m_crTransparent);
	}
	else
	{
		pDC->TransparentBlt(0, 0, m_sizeBmp.cx, m_sizeBmp.cy, 
			&dcMem, 0, 0, m_sizeBmp.cx, m_sizeBmp.cy, m_crTransparent);
	}

	dcMem.SelectObject(pOldBmp);
	dcMem.DeleteDC();

	// draw caption
	if (!m_bShowCaption || m_strCaption.IsEmpty())
		return;

	CFont *pOldFont=NULL;
	if (m_ftCaption.GetSafeHandle())
		pOldFont = pDC->SelectObject(&m_ftCaption);
	pDC->SetTextColor(m_crCaption);
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(m_strCaption, rcClient, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
	if (pOldFont)
		pDC->SelectObject(pOldFont);
}

void CBarButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);	
}

void CBarButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;
	CButton::OnLButtonDown(nFlags, point);
}

void CBarButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	GetClientRect(&rect);

	if (m_bLButtonDown 
		&& rect.PtInRect(point)
		&& m_hNotify
		&& m_uMsg)
		::PostMessage(m_hNotify, m_uMsg, GetDlgCtrlID(), 0);

	m_bLButtonDown = FALSE;

	Repaint();

	CButton::OnLButtonUp(nFlags, point);
}

void CBarButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	CButton::OnMouseMove(nFlags, point);

	CRect rect;
	GetClientRect(rect);
	if (rect.PtInRect(point))
	{
		if (CWnd::GetCapture() != this)
			SetCapture();

		if (!m_bMouseOnButton)
		{
			m_bMouseOnButton = TRUE;
			Repaint();
			if (m_bNotifyMouseIn 
				&& m_hNotifyMouse
				&& m_uMouseMsg) 
				::PostMessage(m_hNotifyMouse, m_uMouseMsg, GetDlgCtrlID(), 0);
		}
	}
	else
	{
		if (!m_bLButtonDown)
			ReleaseCapture();

		if (m_bMouseOnButton)
		{
			m_bMouseOnButton = FALSE;
			Repaint();
		}
	}
}

void CBarButton::OnCaptureChanged(CWnd *pWnd) 
{
	if (pWnd == this)
		return;

	m_bLButtonDown = FALSE;
	if (m_bMouseOnButton == TRUE)
	{
		m_bMouseOnButton = FALSE;
		Repaint();
	}

	CButton::OnCaptureChanged(pWnd);
}

BOOL CBarButton::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CButton::PreTranslateMessage(pMsg);
}


BOOL CBarButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE; 
}

void CBarButton::SetCheck(BOOL bNewValue)
{
	if(bNewValue != m_bChecked)
	{
		m_bChecked = bNewValue;
		Repaint();
	}
}

int CBarButton::GetCheck()
{
	return m_bChecked;
}

void CBarButton::Repaint()
{
	if (!m_hWnd)
		return;

	CWnd* pWnd = GetParent();
	if (pWnd != NULL && m_bTransparent) 
	{
		CRect rcClient;
		GetWindowRect(&rcClient);

		pWnd->ScreenToClient(&rcClient);
		pWnd->InvalidateRect(rcClient);
	}
	else
		Invalidate();
}

void CBarButton::SetTransparent(BOOL bTransparent, COLORREF crTransparent)
{
	m_bTransparent = bTransparent;
	m_crTransparent = crTransparent;
}

BOOL CBarButton::IsTransparent()
{
	return m_bTransparent;
}

void CBarButton::SetBkColor(COLORREF crBk)
{
	m_crBkColor = crBk;
	m_bTransparent = FALSE;
}

COLORREF CBarButton::GetBkColor()
{
	return m_crBkColor;
}

void CBarButton::OnTimer(UINT_PTR nIDEvent)
{
	CButton::OnTimer(nIDEvent);
}
