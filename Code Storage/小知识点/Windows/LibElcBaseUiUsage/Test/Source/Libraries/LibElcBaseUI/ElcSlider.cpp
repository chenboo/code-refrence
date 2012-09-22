// ElcSilder.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcSlider.h"


// CElcSlider

IMPLEMENT_DYNAMIC(CElcSlider, CWnd)

CElcSlider::CElcSlider()
{
	memset(&m_skin, 0, sizeof(m_skin));

	m_hNotifyWnd = NULL;
	m_nNotifyMsg = NULL;
	m_nRange = 100;
	m_nPos = 0;
	m_nLevelRange = 100;
	m_nLevel = 0;
	m_bShowLevel = FALSE;
	m_dwTimer = 0;

	m_nState = ESS_NORMAL;
	m_bEnabldDrag = TRUE;
	m_bHorizontal = TRUE;
	m_bNotifyMoving = TRUE;
}

CElcSlider::~CElcSlider()
{
}


BEGIN_MESSAGE_MAP(CElcSlider, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CElcSlider message handlers


BOOL CElcSlider::LoadSkin(LPCTSTR lpszStyle)
{
	PELC_SKIN_SLIDER pSkin = theSkin.GetSliderSkin(lpszStyle);
	if (!pSkin)
		return FALSE;

	memcpy(&m_skin, pSkin, sizeof(m_skin));

	if (::IsWindow(m_hWnd)) {
		ReLayout();
		Repaint();
	}

	return TRUE;
}

void CElcSlider::SetBackgroundColor(COLORREF cr)
{
	m_skin.crBackgnd = cr;
	Repaint();
}

BOOL CElcSlider::Create(DWORD dwStyle, 
						const RECT& rect, 
						CWnd* pParent, 
						UINT nId, 
						BOOL bHorizontal,
						LPCTSTR lpSkinStyle)
{
	m_bHorizontal = bHorizontal;
	if (lpSkinStyle)
		LoadSkin(lpSkinStyle);

	return CWnd::Create(NULL, NULL, dwStyle, rect, pParent, nId);
}

int CElcSlider::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (_tcslen(m_skin.szStyle) == 0) {
		LoadSkin(ELC_SKIN_STYLE_SLIDER_BIG);
	}

	SetTimer(EST_STATE_CHECK, 1000, NULL);
	m_dwTimer |= EST_STATE_CHECK;

	return 0;
}

BOOL CElcSlider::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CElcSlider::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	ReLayout();
}

void CElcSlider::ReLayout()
{
	GetClientRect(m_rcClient);
	if (m_rcClient.IsRectEmpty())
		return;

	if (!m_skin.thumb.pImage
		|| !m_skin.background.pImage)
		return;

	m_sizeThumb = CElcSkinDoc::GetImageSize(&m_skin.thumb);
	CSize sizeBk = CElcSkinDoc::GetImageSize(&m_skin.background);

	m_rcBody = m_rcClient;

	if (m_bHorizontal)
		m_rcBody.DeflateRect(m_sizeThumb.cx / 2, (m_rcClient.Height() - sizeBk.cy) / 2);
	else 
		m_rcBody.DeflateRect((m_rcClient.Width() - sizeBk.cx) / 2, m_sizeThumb.cy / 2);
}

void CElcSlider::OnPaint()
{
	CPaintDC dc(this);

	CElcMemDC memdc(&dc, m_rcClient);

	BOOL bMirror = IsLayoutRTL();

	ELC_UI_DRAW predraw;
	predraw.hWnd = m_hWnd;
	predraw.type = ELC_UI_DT_PREDRAW;
	predraw.hdc = memdc.GetSafeHdc();
	predraw.rect = m_rcClient;
	if (!GetParent()->SendMessage(WM_ELC_UI_DRAW_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&predraw)) {
		memdc.FillSolidRect(m_rcClient, m_skin.crBackgnd);
	}

	CRect rect = m_rcBody;
	// backgound

	if (bMirror)
		rect = MirrorRect(m_rcClient.Width(), rect);

	int nImageIndex = m_nState == ESS_DISABLE ? 1 : 0;
	thePainter.DrawImageGDI(memdc.GetSafeHdc(), 
		&m_skin.background,
		rect,
		nImageIndex,
		0,
		1,
		0);

	if (m_bShowLevel) {
		// fill left
		ColorMatrix alphaColorMatrix = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.4f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f 
		};

		ImageAttributes imageattributes;
		imageattributes.SetColorMatrix(&alphaColorMatrix);

		if (m_bHorizontal)
			rect.right = rect.left + (int)((float)m_rcBody.Width() / (float)m_nRange * (float)m_nPos);
		else
			rect.top = rect.bottom - (int)((float)m_rcBody.Height() / (float)m_nRange * (float)m_nPos);

		nImageIndex = m_nState == ESS_DISABLE ? 3 : 2;
		thePainter.DrawImageGDI(memdc.GetSafeHdc(), 
			&m_skin.background,
			rect,
			nImageIndex,
			0,
			1,
			0,
			&imageattributes);

		// level
		rect = m_rcBody;
		if (m_bHorizontal)
			rect.right = rect.left + m_rcBody.Width() * m_nLevel / m_nLevelRange;
		else
			rect.top = rect.bottom - m_rcBody.Height() * m_nLevel / m_nLevelRange;

		if (!rect.IsRectEmpty()) {
			Bitmap bmLevel(m_rcBody.Width(), m_rcBody.Height());
			Graphics graphics(&bmLevel);

			CSize size = CElcSkinDoc::GetImageSize(&m_skin.background);
			Rect rcLevel;
			rcLevel.X = 0;
			rcLevel.Y = 0;
			rcLevel.Width = m_rcBody.Width();
			rcLevel.Height = size.cy;

			nImageIndex = (m_nState == ESS_DISABLE) ? 3 : 2;
			thePainter.DrawImage(&graphics, &m_skin.background, &rcLevel, nImageIndex, 1, NULL);

			rcLevel.X = m_rcBody.left;
			rcLevel.Y = m_rcBody.top + (m_rcBody.Height() - size.cy) / 2;
			rcLevel.Width = rect.Width();
			rcLevel.Height = size.cy;
			Graphics gout(memdc.GetSafeHdc());
			gout.DrawImage(&bmLevel, rcLevel, 0, 0, rcLevel.Width, size.cy, UnitPixel);
		}
	}
	else {
		// fill left
		if (m_bHorizontal)
			rect.right = rect.left + (int)((float)m_rcBody.Width() / (float)m_nRange * (float)m_nPos);
		else
			rect.top = rect.bottom - (int)((float)m_rcBody.Height() / (float)m_nRange * (float)m_nPos);

		if (bMirror) {
			rect = MirrorRect(m_rcClient.Width(), rect);
		}

		nImageIndex = m_nState == ESS_DISABLE ? 3 : 2;
		thePainter.DrawImageGDI(memdc.GetSafeHdc(), 
			&m_skin.background,
			rect,
			nImageIndex,
			0,
			1,
			0);
	}

	// thumb
	if (m_bEnabldDrag) {
		rect = GetThumbRect();

		if (bMirror) {
			rect = MirrorRect(m_rcClient.Width(), rect);
		}

		thePainter.DrawImageGDI(memdc.GetSafeHdc(), 
			&m_skin.thumb,
			rect,
			m_nState,
			0,
			1,
			0);
	}

	memdc.DrawMe();
}

void CElcSlider::Repaint()
{
	if (::IsWindow(m_hWnd))
		Invalidate();
}

void CElcSlider::Repaint(LPCRECT lpRect)
{
	if (::IsWindow(m_hWnd)) {
		InvalidateRect(lpRect, FALSE);
	}
}

inline CRect CElcSlider::GetThumbRect()
{
	CRect rect;

	if (m_bHorizontal) {
		rect.left = m_rcBody.left + (int)((float)m_rcBody.Width() / (float)m_nRange * (float)m_nPos) - m_sizeThumb.cx / 2;
		rect.top = m_rcClient.top + (m_rcClient.Height() - m_sizeThumb.cy) / 2;
		rect.right = rect.left + m_sizeThumb.cx;
		rect.bottom = rect.top + m_sizeThumb.cy;
	}
	else {
		rect.left = m_rcClient.left + (m_rcClient.Width() - m_sizeThumb.cx) / 2;
		rect.right = rect.left + m_sizeThumb.cx;
		rect.bottom = m_rcClient.bottom - (int)((float)m_rcBody.Height() / (float)m_nRange * (float)m_nPos) - m_sizeThumb.cy / 2;
		rect.top = rect.bottom - m_sizeThumb.cy;
	}

	return rect;
}

void CElcSlider::SetLevel(int nLevel)
{
	if (nLevel < 0)
		nLevel = 0;
	else if (nLevel > m_nLevelRange)
		nLevel = m_nLevelRange;

	if (nLevel < m_nLevel)
		return;

	m_nLevel = nLevel;
	if ((m_dwTimer & EST_LEVEL) == 0) {
		SetTimer(EST_LEVEL, 30, NULL);
		m_dwTimer |= EST_LEVEL;
	}

	m_bShowLevel = TRUE;
}

void CElcSlider::SetLevelRange(int nRange)
{
	m_nLevelRange = nRange;
}

void CElcSlider::SetRange(int nRange)
{
	m_nRange = nRange;
	if (m_nRange <= 0)
		m_nRange = 100;
	Repaint();
}

void CElcSlider::SetPos(int nPos)
{
	if (m_nState == ESS_DOWN)
		return;

	if (nPos < 0)
		nPos = 0;
	if (nPos > m_nRange)
		nPos = m_nRange;

	m_nPos = nPos;
	Repaint();
}

inline int CElcSlider::GetPos()
{
	return m_nPos;
}

void CElcSlider::CalcPos(CPoint point)
{
	int nPos = 0;
	if (m_bHorizontal)
	{
		point.x -= m_rcBody.left;
		if (point.x < 0)
			point.x = 0;
		else if (point.x > m_rcBody.Width())
			point.x = m_rcBody.Width();

		nPos = (int)((float)m_nRange / (float)m_rcBody.Width() * (float)point.x);
	}
	else 
	{
		point.y -= m_rcBody.top;
		if (point.y < 0)
			point.y = 0;
		else if (point.y > m_rcBody.Height())
			point.y = m_rcBody.Height();

		nPos = (int)((float)m_nRange / (float)m_rcBody.Height() * (float)point.y);
	}

	if (nPos != m_nPos) {
		m_nPos = nPos;
		Repaint();
	}
}

void CElcSlider::NotifyPos()
{
	if (m_hNotifyWnd && m_nNotifyMsg)
		::PostMessage(m_hNotifyWnd, m_nNotifyMsg, GetDlgCtrlID(), m_nPos);
}

void CElcSlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (ESS_DISABLE == m_nState
		|| !m_bEnabldDrag)
		return;

	m_nState = ESS_DOWN;
	CalcPos(point);
	Repaint();

	SetCapture();

	CWnd::OnLButtonDown(nFlags, point);
}

void CElcSlider::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (ESS_DOWN != m_nState
		|| !m_bEnabldDrag)
		return;

	ReleaseCapture();

	CalcPos(point);

	NotifyPos();

	CWnd::OnLButtonUp(nFlags, point);
}

void CElcSlider::OnMouseMove(UINT nFlags, CPoint point)
{
	if (ESS_DISABLE == m_nState
		|| !m_bEnabldDrag)
		return;

	if (m_nState == ESS_DOWN) {
		CalcPos(point);

		if (m_bNotifyMoving)
			NotifyPos();
	}
	else {
		CRect rect;
		rect.left = m_rcBody.left + (int)((float)m_rcBody.Width() / (float)m_nRange * (float)m_nPos) - m_sizeThumb.cx / 2;
		rect.top = m_rcClient.top + (m_rcClient.Height() - m_sizeThumb.cy) / 2;
		rect.right = rect.left + m_sizeThumb.cx;
		rect.bottom = rect.top + m_sizeThumb.cy;

		if (rect.PtInRect(point)) {
			if (ESS_HOVER != m_nState) {
				m_nState = ESS_HOVER;
				Repaint(GetThumbRect());
			}
		} 
		else {
			if (ESS_NORMAL != m_nState) {
				m_nState = ESS_NORMAL;
				Repaint(GetThumbRect());
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CElcSlider::OnCaptureChanged(CWnd *pWnd)
{
	if (ESS_DISABLE == m_nState)
		return;

	m_nState = ESS_NORMAL;
	Repaint();

	CWnd::OnCaptureChanged(pWnd);
}

void CElcSlider::OnTimer(UINT_PTR nIDEvent)
{
	if (EST_STATE_CHECK == nIDEvent) {
		if (ESS_HOVER == m_nState) {
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			if (!m_rcClient.PtInRect(pt))
			{
				m_nState = ESS_NORMAL;
				Repaint(GetThumbRect());
			}
		}
	}
	else if (EST_LEVEL == nIDEvent) {
		m_nLevel -= 2;
		if (m_nLevel < 0) {
			m_nLevel = 0;
			KillTimer(EST_LEVEL);
			m_dwTimer &= ~EST_LEVEL;
		}

		Repaint(m_rcBody);
	}

	CWnd::OnTimer(nIDEvent);
}

void CElcSlider::EnableDraging(BOOL bEnable)
{
	m_bEnabldDrag = bEnable;
}

BOOL CElcSlider::IsDraging()
{
	return (ESS_DOWN == m_nState);
}

void CElcSlider::EnableMovingNotify(BOOL bEnable)
{
	m_bNotifyMoving = bEnable;
}

BOOL CElcSlider::EnableWindow(BOOL bEnable)
{
	if (IsWindowEnabled() == bEnable)
		return TRUE;

	if (m_nState == ESS_DOWN)
		ReleaseCapture();

	m_nState = bEnable ? ESS_NORMAL : ESS_DISABLE;

	BOOL bRet = CWnd::EnableWindow(bEnable);

	Repaint();

	return bRet;
}

BOOL CElcSlider::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (ESS_DOWN == m_nState || WM_LBUTTONDOWN == message) {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_DRAG));
	}
	else if (ESS_HOVER == m_nState) {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_HAND));
	}
	else {
		return CWnd::OnSetCursor(pWnd, nHitTest, message);
	}

	return TRUE;
}
