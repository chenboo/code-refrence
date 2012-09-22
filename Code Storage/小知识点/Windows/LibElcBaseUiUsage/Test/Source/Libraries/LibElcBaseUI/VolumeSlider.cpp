// VolumeSlider.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "VolumeSlider.h"
#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CVolumeSlider

IMPLEMENT_DYNAMIC(CVolumeSlider, CWnd)

CVolumeSlider::CVolumeSlider()
{
	m_crBorder = RGB(128,160,192);
	m_crBorderDisable = RGB(192,192,192);

	m_crBody = RGB(205,220,237);
	m_crBodyDisable = RGB(220,220,220);

	m_crLevel = RGB(0,204,0);
	m_crHightLevel = RGB(255,255,0);

	m_crBackgnd = RGB(205,220,237);

	m_crFill = RGB(0,100,150);
	m_crFillDisable = RGB(200,200,200);

	m_crScale = RGB(192,192,192);

	m_nBarState = BAR_NORMAL;
	m_nRange = 100;
	m_nLevel = 0;
	m_nPos = 0;
	m_nFillPos = 0;
	m_nSliderThick = 10;

	m_nCmdID = 0;
	m_bHorizontal = TRUE;
	m_hParent = NULL;
	m_nMsg = 0;
	m_hNotify = NULL;

	m_bLBDown = FALSE;

	m_nBarPos = 0;

	m_bRepaint = FALSE;

	m_nLevel = 0;
	m_nLevelTop = 0;
	m_nLevelMax = 100;
	m_nLevelElapse = 20;
	m_bTimer = FALSE;
	m_bEnableBKImage = FALSE;
	m_bMouseCheckTimer = FALSE;

	m_rcClient = CRect(0,0,0,0);
	m_rcThumb = CRect(0,0,0,0);
	m_rcBody = CRect(0,0,0,0);

	m_bDrawScale = TRUE;
	m_bNotifyMoving = TRUE;
	m_bAutoFill = FALSE;

	m_bEnableDrag = TRUE;

	memset(&m_bmBarInfo, 0, sizeof(BITMAP));
	memset(&m_bmInfoLevel, 0, sizeof(BITMAP));
}

CVolumeSlider::~CVolumeSlider()
{
}


BEGIN_MESSAGE_MAP(CVolumeSlider, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()



// CVolumeSlider message handlers

BOOL CVolumeSlider::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, UINT nCmdID, BOOL bHorizontal)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	m_nCmdID = nCmdID;
	m_bHorizontal = bHorizontal;
	m_hParent = pParentWnd->m_hWnd;
	m_nID = nID;

	m_rcClient = CRect(0,0,rect.right-rect.left,rect.bottom-rect.top);
	m_rcBody = m_rcClient;

	return CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID);
}

int CVolumeSlider::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CreateSpectrumDC();

	return 0;
}

void CVolumeSlider::SetPosChangedNotify(UINT nMessage)
{
	m_nMsg = nMessage;
}

void CVolumeSlider::SetPosChangedNotify(HWND hWnd, UINT nMessage)
{
	m_hNotify = hWnd;
	m_nMsg = nMessage;
}

void CVolumeSlider::EnableNotifyMoving(BOOL bEnable)
{
	m_bNotifyMoving = bEnable;
}

void CVolumeSlider::SetBorderColor(COLORREF cr)
{
	m_crBorder = cr;
	Repaint();
}

void CVolumeSlider::SetSliderBodyColor(COLORREF cr)
{
	m_crBody = cr;
	Repaint();
}

void CVolumeSlider::SetBkColor(COLORREF color)
{
	m_crBackgnd = color;
	Repaint();
}

COLORREF CVolumeSlider::GetBkColor()
{
	return m_crBackgnd;
}

void CVolumeSlider::OnSize(UINT nType, int cx, int cy)
{
	if (cx <= 0 ||
		cy <= 0 )
	{
		m_rcClient = CRect(0,0,0,0);
		return;
	}

	ReCalcSize();
}

void CVolumeSlider::ReCalcSize()
{
	int nOldPos = GetPos();
	int nOldFill = GetFillPos();

	GetClientRect(&m_rcClient);
	m_rcBody = m_rcClient;

	if (m_bHorizontal) 
	{
		m_rcThumb.left = 0;
		m_rcThumb.right = m_bmBarInfo.bmWidth;
		m_rcThumb.top = (m_rcClient.Height() - m_bmBarInfo.bmHeight)/2;
		m_rcThumb.bottom = m_rcThumb.top + m_bmBarInfo.bmHeight;

		m_rcBody.left = m_bmBarInfo.bmWidth/2;
		m_rcBody.right -= m_bmBarInfo.bmWidth/2;
	}
	else
	{
		m_rcThumb.bottom = m_rcClient.bottom;
		m_rcThumb.top = m_rcThumb.bottom - m_bmBarInfo.bmHeight;
		m_rcThumb.left = (m_rcClient.Width() - m_bmBarInfo.bmWidth)/2;
		m_rcThumb.right = m_rcThumb.left + m_bmBarInfo.bmWidth;

		m_rcBody.top = m_bmBarInfo.bmHeight/2;
		m_rcBody.bottom -= m_bmBarInfo.bmHeight/2;
	}

	if (m_dcLevelSrc.GetSafeHdc())
	{
		if (m_dcLevel.GetSafeHdc())
			m_dcLevel.DeleteDC();

		m_dcLevel.CreateCompatibleDC(&m_dcLevelSrc);

		CBitmap bmDest;
		bmDest.CreateCompatibleBitmap(&m_dcLevelSrc, m_rcBody.Width(), m_rcBody.Height());
		m_dcLevel.SelectObject(&bmDest);
		
		m_dcLevel.StretchBlt(0, 0, m_rcBody.Width(), m_rcBody.Height(),
			&m_dcLevelSrc, 0, 0, m_bmInfoLevel.bmWidth, m_bmInfoLevel.bmHeight, SRCCOPY);
	}
	else if (m_dcLevel.GetSafeHdc())
	{
		CreateSpectrumDC();
	}

	if (nOldPos >= 0)
		SetPos(nOldPos);

	if (nOldFill >= 0)
		SetFillPos(nOldFill);
}

void CVolumeSlider::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DrawBody(&dc);
	DrawBar(&dc);
}

void CVolumeSlider::DrawBody(CDC* pDC)
{
	pDC->FillSolidRect(m_rcClient, m_crBackgnd);

	if (m_bEnableBKImage)
		DrawBkImage(pDC);
	else
		DrawNormalBk(pDC);

	DrawScale(pDC);
}

void CVolumeSlider::DrawNormalBk(CDC* pDC)
{
	CRect rcSlider = m_rcBody;
	CRect rcLevel;

	int nLevelTopPos = 0;
	int nMargin = 0;
	if (m_bHorizontal) 
	{
		nMargin = (m_rcClient.Height() - m_nSliderThick) / 2;
		rcSlider.top += nMargin;
		rcSlider.bottom -= nMargin;

		rcLevel.top = rcSlider.top + 1;
		rcLevel.bottom = rcSlider.bottom - 1;
		rcLevel.left = rcSlider.left + 1;
		rcLevel.right = rcLevel.left + m_nLevel * (m_rcBody.Width()-2) / m_nLevelMax;

		nLevelTopPos = rcLevel.left + m_nLevelTop * (m_rcBody.Width()-2) / m_nLevelMax;
	}
	else 
	{
		nMargin = (m_rcClient.Width() - m_nSliderThick) / 2;
		rcSlider.left += nMargin;
		rcSlider.right -= nMargin;

		rcLevel.bottom = rcSlider.bottom - 1;
		rcLevel.left = rcSlider.left + 1;
		rcLevel.right = rcSlider.right - 1;
		rcLevel.top = rcLevel.bottom - m_nLevel * (m_rcBody.Height()-2) / m_nLevelMax;

		nLevelTopPos = rcLevel.bottom - m_nLevelTop * (m_rcBody.Height()-2) / m_nLevelMax;
	}

	CPen pen, *pOldPen;
	pen.CreatePen(PS_SOLID, 1, m_nBarState == BAR_DISABLE ? m_crBorderDisable : m_crBorder);

	CBrush brBackgnd, *pOldBrush;
	brBackgnd.CreateSolidBrush(m_nBarState == BAR_DISABLE ? m_crBodyDisable : m_crBody);
	
	pOldPen = pDC->SelectObject(&pen);
	pOldBrush = pDC->SelectObject(&brBackgnd);

	pDC->Rectangle(rcSlider);

	// draw fill pos
	if (m_nFillPos > 0)
	{
		CRect rcFill;
		if (m_bHorizontal)
			rcFill = CRect(rcSlider.left+1, rcSlider.top+1, rcSlider.left + m_nFillPos-1, rcSlider.bottom-1);
		else
			rcFill = CRect(rcSlider.left+1, rcSlider.bottom-m_nFillPos+1, rcSlider.right-1, rcSlider.bottom-1);

		pDC->FillSolidRect(rcFill, m_nBarState == BAR_DISABLE ? m_crFillDisable : m_crFill);
	}

	// draw VUMeter level
	if (m_bTimer &&
		m_nLevelMax > 0 && 
		m_nLevelMax > m_nLevel) 
	{
		if (m_nLevel > 0)
		{
			if (m_dcLevel.GetSafeHdc())
			{
				pDC->BitBlt(rcLevel.left, rcLevel.top, rcLevel.Width(), rcLevel.Height(),
					&m_dcLevel, rcLevel.left, rcLevel.top, SRCCOPY);
			}
			else
			{
				pDC->FillSolidRect(rcLevel, m_crLevel);
			}
		}

		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		pen.CreatePen(PS_SOLID, 1, m_crHightLevel);
		pOldPen = pDC->SelectObject(&pen);

		if (m_bHorizontal) 
		{
			pDC->MoveTo(nLevelTopPos, rcLevel.top);
			pDC->LineTo(nLevelTopPos, rcLevel.bottom);
		}
		else
		{
			pDC->MoveTo(rcLevel.left, nLevelTopPos);
			pDC->LineTo(rcLevel.right, nLevelTopPos);
		}
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void CVolumeSlider::DrawBkImage(CDC* pDC)
{
	int nYPos = (m_rcClient.Height() - m_sBack[0].cy) / 2;

	pDC->TransparentBlt(0, nYPos, m_sBack[0].cx, m_sBack[0].cy,
		&m_dcBack[0], 0, 0, m_sBack[0].cx, m_sBack[0].cy, RGB(255,0,255));

	pDC->TransparentBlt(m_sBack[0].cx, nYPos, m_rcClient.Width()-m_sBack[0].cx-m_sBack[2].cx, m_sBack[1].cy,
		&m_dcBack[1], 0, 0, m_sBack[1].cx, m_sBack[1].cy, RGB(255,0,255));

	pDC->TransparentBlt(m_rcClient.Width()-m_sBack[2].cx, nYPos, m_sBack[2].cx, m_sBack[2].cy,
		&m_dcBack[2], 0, 0, m_sBack[2].cx, m_sBack[2].cy, RGB(255,0,255));
}

void CVolumeSlider::DrawBar(CDC* pDC)
{
	CBitmap bmBar;
	CDC dcBar;
	dcBar.CreateCompatibleDC(pDC);
	bmBar.CreateCompatibleBitmap(&dcBar, m_rcThumb.Width(), m_rcThumb.Height());
	CBitmap* pOldBm = dcBar.SelectObject(&m_bmBar[m_nBarState]);

	pDC->TransparentBlt(m_rcThumb.left, m_rcThumb.top, m_rcThumb.Width(), m_rcThumb.Height(), 
		&dcBar, 0, 0, m_rcThumb.Width(), m_rcThumb.Height(), RGB(255,0,255));

	dcBar.SelectObject(pOldBm);
}

void CVolumeSlider::DrawScale(CDC* pDC)
{
	if (!m_bDrawScale)
		return;

	CRect rcSlider = m_rcBody;

	int nMargin = 0;
	if (m_bHorizontal) 
	{
		nMargin = (m_rcClient.Height() - m_nSliderThick) / 2;

		if (m_nRange * 2 > m_rcBody.Width() ||
			nMargin < 3)
		{
			return;
		}

		CPen pen, *pOldPen;
		pen.CreatePen(PS_SOLID, 1, m_crScale);
		pOldPen = pDC->SelectObject(&pen);
		int nBarThick = m_rcThumb.Width()/2;

		float fUintWidth = (float)(m_rcBody.Width()) / (float)m_nRange;
		for (int i=0; i <= m_nRange; i++)
		{
			int nPos = (int)(fUintWidth * (float)i) + nBarThick;
			pDC->MoveTo(nPos, nMargin-3);
			pDC->LineTo(nPos, nMargin-1);
			pDC->MoveTo(nPos, nMargin+m_nSliderThick+1);
			pDC->LineTo(nPos, nMargin+m_nSliderThick+3);
		}

		pDC->SelectObject(pOldPen);
	}
	else 
	{
		nMargin = (m_rcClient.Width() - m_nSliderThick) / 2;

		if (m_nRange*2 > m_rcBody.Height() ||
			nMargin < 3)
		{
			return;
		}

		CPen pen, *pOldPen;
		pen.CreatePen(PS_SOLID, 1, m_crScale);
		pOldPen = pDC->SelectObject(&pen);

		float fUintHeight = (float)(m_rcBody.Height()) / (float)m_nRange;
		for (int i=0; i <= m_nRange; i++)
		{
			int nPos = (int)(fUintHeight * (float)i)+ m_rcThumb.Height()/2;
			pDC->MoveTo(nMargin-3, nPos);
			pDC->LineTo(nMargin-1, nPos);
			pDC->MoveTo(nMargin+m_nSliderThick+1, nPos);
			pDC->LineTo(nMargin+m_nSliderThick+3, nPos);
		}

		pDC->SelectObject(pOldPen);
	}	
}

BOOL CVolumeSlider::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CVolumeSlider::OnMouseMove(UINT nFlags, CPoint point)
{
	if (BAR_DISABLE == m_nBarState)
		return;

	if (!m_bLBDown && !m_bMouseCheckTimer)
	{
		SetTimer(ID_TIMER_MOUSE_CHECK, 1000, NULL);
		m_bMouseCheckTimer = TRUE;
	}

	if (BAR_DOWN == m_nBarState) 
	{
		m_bRepaint = TRUE;

		int nOldPos = GetPos();

		CalcBarPos(point);		

		if (::IsWindow(m_hParent) && nOldPos != GetPos() && m_bNotifyMoving) 
		{
			if (m_hNotify && m_nMsg)
			{
				::PostMessage(m_hNotify, m_nMsg, m_nID, GetPos());
			}
			else if (m_nMsg)
			{
				::PostMessage(m_hParent, m_nMsg, m_nID, GetPos());
			}
			else if (m_nCmdID)
			{
				::PostMessage(m_hParent, WM_COMMAND, m_nCmdID, 0);
			}
		}
	}
	else if (m_rcThumb.PtInRect(point)) 
	{
		m_bRepaint = TRUE;
		m_nBarState = BAR_HOVER;
	}
	else {
		if (BAR_NORMAL != m_nBarState)
			m_bRepaint = TRUE;

		m_nBarState = BAR_NORMAL;
	}

	if (m_bRepaint) {
		m_bRepaint = FALSE;
		Repaint();
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CVolumeSlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (BAR_DISABLE == m_nBarState || !m_bEnableDrag)
		return;

	CPoint pt = point;
	
	CalcBarPos(pt);

	m_nBarState = BAR_DOWN;
	m_bLBDown = TRUE;
	SetCapture();

	Repaint();

	CWnd::OnLButtonDown(nFlags, point);
}

void CVolumeSlider::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bLBDown)
		return ;

	ReleaseCapture();
	m_bLBDown = FALSE;
	m_nBarState = BAR_NORMAL;

	Repaint();

	if (m_hNotify && m_nMsg)
	{
		::PostMessage(m_hNotify, m_nMsg, m_nID, GetPos());
	}
	else if (m_nMsg)
	{
		::PostMessage(m_hParent, m_nMsg, m_nID, GetPos());
	}
	else if (m_nCmdID)
	{
		::PostMessage(m_hParent, WM_COMMAND, m_nCmdID, 0);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CVolumeSlider::OnTimer(UINT_PTR nIDEvent)
{
	if (ID_TIMER_LEVEL == nIDEvent) 
	{
		RepaintLevel();

		m_nLevel -= 2;

		if (m_nLevel > 0)
		{
			if (m_nLevelTop > m_nLevel + m_nLevelMax/10)
				m_nLevelTop -= 1;
		}
		else
		{
			m_nLevelTop -= 1;
			m_nLevel = 0;
		}

		if (m_nLevelTop <=0 ) 
		{
			m_nLevelTop = 0;
			m_nLevel = 0;
			m_bTimer = FALSE;
			KillTimer(ID_TIMER_LEVEL);
		}
	}
	else if (ID_TIMER_MOUSE_CHECK == nIDEvent)
	{
		if (!m_bLBDown)
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			if (!m_rcClient.PtInRect(pt))
			{
				if (m_nBarState != BAR_DISABLE)
					m_nBarState = BAR_NORMAL;
				KillTimer(ID_TIMER_MOUSE_CHECK);
				m_bMouseCheckTimer = FALSE;
				Repaint();
			}
		}
	}

	CWnd::OnTimer(nIDEvent);
}

void CVolumeSlider::RepaintLevel()
{
	CRect rcSlider = m_rcBody;
	CRect rcLevel;

	int nMargin = 0;
	if (m_bHorizontal) 
	{
		nMargin = (m_rcClient.Height() - m_nSliderThick) / 2;
		rcSlider.top += nMargin;
		rcSlider.bottom -= nMargin;
	}
	else 
	{
		nMargin = (m_rcClient.Width() - m_nSliderThick) / 2;
		rcSlider.left += nMargin;
		rcSlider.right -= nMargin;
	}

	InvalidateRect(rcSlider);
}

BOOL CVolumeSlider::SetSliderBar(int nBarID, int nSliderThickness)
{
	ASSERT(::IsWindow(m_hWnd));

	CBitmap bmBar;
	if (!bmBar.Attach((HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nBarID), 
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)))
		return FALSE;

	for (int i=0; i < 4; i ++)
	{
		if (m_bmBar[i].GetSafeHandle())
			m_bmBar[i].DeleteObject();
	}

	if (!ParseBitmap(&bmBar, m_bmBar, 4))
		return FALSE;

	m_bmBar[0].GetBitmap(&m_bmBarInfo);

	GetClientRect(&m_rcClient);
	if (m_bHorizontal) 
	{
		m_rcThumb.left = 0;
		m_rcThumb.right = m_bmBarInfo.bmWidth;
		m_rcThumb.top = (m_rcClient.Height() - m_bmBarInfo.bmHeight)/2;
		m_rcThumb.bottom = m_rcThumb.top + m_bmBarInfo.bmHeight;

		m_rcBody = m_rcClient;
		m_rcBody.left = m_bmBarInfo.bmWidth/2;
		m_rcBody.right -= m_bmBarInfo.bmWidth/2;
	}
	else
	{
		m_rcThumb.bottom = m_rcClient.bottom;
		m_rcThumb.top = m_rcThumb.bottom - m_bmBarInfo.bmHeight;
		m_rcThumb.left = (m_rcClient.Width() - m_bmBarInfo.bmWidth)/2;
		m_rcThumb.right = m_rcThumb.left + m_bmBarInfo.bmWidth;

		m_rcBody = m_rcClient;
		m_rcBody.top = m_bmBarInfo.bmHeight/2;
		m_rcBody.bottom -= m_bmBarInfo.bmHeight/2;
	}

	m_nSliderThick = nSliderThickness;

	return TRUE;
}

void CVolumeSlider::SetSliderThickness(int nSliderThickness)
{
	m_nSliderThick = nSliderThickness;

	Repaint();
}

int CVolumeSlider::GetSliderThickness()
{
	return m_nSliderThick;
}

BOOL CVolumeSlider::ParseBitmap(CBitmap* pbmSrc, CBitmap* pbmDest, int nNums)
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

BOOL CVolumeSlider::ParseBitmap(CBitmap* pbmSrc, CBitmap* pbmDest, CPoint ptSrc, CSize sDest)
{
	ASSERT(pbmSrc && pbmDest);
	if (!pbmSrc || !pbmDest)
		return FALSE;

	if (pbmDest->GetSafeHandle())
		pbmDest->DeleteObject();

	CBitmap* pOldBmp = NULL;
	CDC dcSrc, dcDest;
	dcSrc.CreateCompatibleDC(NULL);
	dcSrc.SelectObject(pbmSrc);
	dcDest.CreateCompatibleDC(NULL);

	pbmDest->CreateCompatibleBitmap(&dcSrc, sDest.cx, sDest.cy);
	pOldBmp = dcDest.SelectObject(pbmDest);
	dcDest.BitBlt(0,0,sDest.cx, sDest.cy, &dcSrc, ptSrc.x, ptSrc.y, SRCCOPY);
	dcDest.SelectObject(pOldBmp);

	return TRUE;
}

void CVolumeSlider::Repaint()
{
	if (::IsWindow(m_hWnd))
		Invalidate();
}

void CVolumeSlider::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	if (m_bLBDown) 
	{
		ReleaseCapture();
		m_bLBDown = FALSE;
		m_nBarState = BAR_NORMAL;
	}
}

void CVolumeSlider::CalcBarPos(CPoint point)
{
	int nBarWidth = m_rcThumb.Width();
	int nBarHeight = m_rcThumb.Height();

	if (m_bHorizontal)
	{
		float fUintWidth = (float)(m_rcBody.Width()) / (float)m_nRange;

		if (m_nRange < m_rcBody.Width())
		{
			m_rcThumb.left = (int)(fUintWidth * (int)((float)point.x / fUintWidth + 0.5f) + 0.5f);
			m_rcThumb.right = m_rcThumb.left + nBarWidth;
		}
		else
		{
			m_rcThumb.left = point.x - nBarWidth/2;
			m_rcThumb.right = m_rcThumb.left + nBarWidth;
		}

		if ((float)m_rcThumb.left < fUintWidth/2) 
		{
			m_rcThumb.left = 0;
			m_rcThumb.right = nBarWidth;
		}
		else if ((float)m_rcThumb.right + fUintWidth/2 > (float)m_rcClient.Width()) 
		{
			m_rcThumb.right = m_rcClient.right;
			m_rcThumb.left = m_rcThumb.right - nBarWidth;
		}

		m_rcThumb.top = (m_rcClient.Height() - nBarHeight) / 2;
		m_rcThumb.bottom = m_rcThumb.top + nBarHeight;

		if (m_bAutoFill)
			m_nFillPos = m_rcThumb.left;
	}
	else
	{
		float fUintHeight = (float)(m_rcBody.Height()) / (float)m_nRange;

		if (m_nRange < m_rcBody.Height())
		{
			m_rcThumb.top = (int)(fUintHeight * (int)((float)point.y / fUintHeight + 0.5f) + 0.5f);
			m_rcThumb.bottom = m_rcThumb.top + nBarHeight;
		}
		else
		{
			m_rcThumb.top = point.y - nBarHeight/2;
			m_rcThumb.bottom = m_rcThumb.top + nBarHeight;
		}

		if ((float)m_rcThumb.top < fUintHeight/2) 
		{
			m_rcThumb.top = 0;
			m_rcThumb.bottom = nBarHeight;
		}
		else if ((float)m_rcThumb.bottom + fUintHeight/2 > (float)m_rcClient.Height()) 
		{
			m_rcThumb.bottom = m_rcClient.bottom;
			m_rcThumb.top = m_rcThumb.bottom - nBarHeight;
		}

		m_rcThumb.left = (m_rcClient.Width() - m_rcThumb.Width()) / 2;
		m_rcThumb.right = m_rcThumb.left + nBarWidth;

		if (m_bAutoFill)
			m_nFillPos = m_rcClient.bottom - m_rcThumb.bottom;
	}
}

void CVolumeSlider::SetLevel(int nLevel)
{
	ASSERT(m_nLevelMax>0);

	nLevel = nLevel * GetPos() / m_nRange;

	if (nLevel <= m_nLevel)
		return;
	m_nLevel = nLevel;
	
	if (m_nLevel < 0)
		m_nLevel = 0;
	if (m_nLevel > m_nLevelMax)
		m_nLevel = m_nLevelMax;

	if (m_nLevelTop < m_nLevel)
		m_nLevelTop = m_nLevel;

	if (!m_bTimer) 
	{
		m_bTimer = TRUE;
		SetTimer(ID_TIMER_LEVEL, m_nLevelElapse, NULL);
	}
}

void CVolumeSlider::SetLevelMax(int nMax)
{
	m_nLevelMax = nMax;
}

int CVolumeSlider::GetLevelRange()
{
	return m_nLevelMax;
}

void CVolumeSlider::SetLevelElapse(int nElapse)
{
	m_nLevelElapse = nElapse;
}

void CVolumeSlider::SetRange(int nRange)
{
	ASSERT(nRange>=0);

	if (nRange <= 0)
		return;

	m_nRange = nRange;
}

int CVolumeSlider::GetRange()
{
	return m_nRange;
}

void CVolumeSlider::SetPos(int nPos)
{
	ASSERT(::IsWindow(m_hWnd) && nPos>=0 && nPos<=m_nRange);

	if (nPos < 0)
		return;

	if (nPos > m_nRange)
		nPos = m_nRange;

	int nBarPos = 0;
	if (m_bHorizontal)
		nBarPos = nPos*m_rcBody.Width()/m_nRange + m_rcThumb.Width()/2;
	else
		nBarPos = (m_nRange-nPos)*m_rcBody.Height()/m_nRange + m_rcThumb.Height()/2;

	CalcBarPos(CPoint(nBarPos, nBarPos));

	Repaint();
}

int CVolumeSlider::GetPos()
{
	if (m_rcClient.IsRectEmpty())
		return 0;

	if (m_bHorizontal)
	{
		return (int)((float)m_nRange * (float)m_rcThumb.left / (float)m_rcBody.Width() + 0.5f);
	}
	else
	{
		return (int)(m_nRange * (float)(m_rcClient.bottom - m_rcThumb.bottom) / (float)m_rcBody.Height() + 0.5);
	}
}

void CVolumeSlider::SetFillPos(int nPos)
{
	ASSERT(::IsWindow(m_hWnd) && nPos>=0 && nPos<=m_nRange);

	if (m_bHorizontal)
		m_nFillPos = nPos * m_rcBody.Width() / m_nRange;
	else
		m_nFillPos = nPos * m_rcBody.Height() / m_nRange;

	Repaint();
}
	
int CVolumeSlider::GetFillPos()
{
	if (m_rcBody.IsRectEmpty())
		return 0;

	return m_nFillPos * m_nRange / m_rcBody.Width();
}

void CVolumeSlider::SetAutoFill(BOOL bAuto)
{
	m_bAutoFill = bAuto;
	Repaint();
}

BOOL CVolumeSlider::IsAutoFill()
{
	return m_bAutoFill;
}

void CVolumeSlider::SetSliderImage(UINT nImageID, int nHeadWidth, int nTailWidth)
{
	if (nImageID == 0 || !::IsWindow(m_hWnd))
	{
		ASSERT(0);
		return;
	}

	for (int i=0; i < 3; i++)
	{
		if (m_bmBack[i].GetSafeHandle())
			m_bmBack[i].DeleteObject();
		if (m_dcBack[i].GetSafeHdc())
			m_dcBack[i].DeleteDC();
	}

	CBitmap bm;
	if (!bm.Attach((HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nImageID), 
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)))
		return ;

	BITMAP bmInfo;
	bm.GetBitmap(&bmInfo);

	m_sBack[0].cx = nHeadWidth;
	m_sBack[0].cy = bmInfo.bmHeight;
	m_sBack[1].cx = bmInfo.bmWidth - nHeadWidth - nTailWidth;
	m_sBack[1].cy = bmInfo.bmHeight;
	m_sBack[2].cx = nTailWidth;
	m_sBack[2].cy = bmInfo.bmHeight;

	ParseBitmap(&bm, &m_bmBack[0], CPoint(0,0), 
		CSize(nHeadWidth,bmInfo.bmHeight));
	ParseBitmap(&bm, &m_bmBack[1], CPoint(nHeadWidth,0), 
		CSize(bmInfo.bmWidth-nHeadWidth-nTailWidth,bmInfo.bmHeight));
	ParseBitmap(&bm, &m_bmBack[2], CPoint(bmInfo.bmWidth-nTailWidth,0), 
		CSize(nTailWidth,bmInfo.bmHeight));

	bm.Detach();

	CDC* pDC = GetWindowDC();
	for (int i=0; i < 3; i++)
	{
		m_dcBack[i].CreateCompatibleDC(pDC);
		m_dcBack[i].SelectObject(&m_bmBack[i]);
	}

	ReleaseDC(pDC);

	m_nSliderThick = bmInfo.bmHeight;

	Invalidate();
}

void CVolumeSlider::EnableBkImage(BOOL bOn)
{
	m_bEnableBKImage = bOn;
	Invalidate();
}

BOOL CVolumeSlider::SetLevelImage(UINT nBitmap)
{
	if (nBitmap == 0 || !::IsWindow(m_hWnd))
	{
		ASSERT(0);
		return FALSE;
	}

	CBitmap bm, bmLevel;
	if (!bm.Attach((HBITMAP)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nBitmap), 
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)))
	{
		ASSERT(0);
		return FALSE;
	}

	bm.GetBitmap(&m_bmInfoLevel);

	if (m_dcLevelSrc.GetSafeHdc())
		m_dcLevelSrc.DeleteDC();
	if (m_dcLevel.GetSafeHdc())
		m_dcLevel.DeleteDC();
	
	m_dcLevelSrc.CreateCompatibleDC(NULL);
	m_dcLevelSrc.SelectObject(&bm);
	
	bmLevel.CreateCompatibleBitmap(&m_dcLevelSrc, m_rcBody.Width(), m_rcBody.Height());
	m_dcLevel.CreateCompatibleDC(NULL);
	m_dcLevel.SelectObject(&bmLevel);

	m_dcLevel.StretchBlt(0, 0, m_rcBody.Width(), m_rcBody.Height(), 
		&m_dcLevelSrc, 0, 0, m_bmInfoLevel.bmWidth, m_bmInfoLevel.bmHeight, SRCCOPY);

	return TRUE;
}

void CVolumeSlider::CreateSpectrumDC()
{
	if (m_rcClient.IsRectEmpty())
		return ;

	if (m_dcLevel.GetSafeHdc())
		m_dcLevel.DeleteDC();

	CDC* pDC = GetWindowDC();
	m_dcLevel.CreateCompatibleDC(pDC);
	CBitmap bm;
	bm.CreateCompatibleBitmap(pDC, m_rcBody.Width(), m_rcBody.Height());
	m_dcLevel.SelectObject(&bm);
	ReleaseDC(pDC);

	COLORREF crA = RGB(0,230,0);
	COLORREF crB = RGB(255,0,0);

	m_dcLevel.FillSolidRect(m_rcBody, crA);

	CPen pen, *pOldPen=NULL;
	
	int nPart = 0; 
	if (m_bHorizontal)
		nPart = m_rcBody.Width() / 3;
	else
		nPart = m_rcBody.Height() / 3;

	int nMiddle = nPart;
	for (int i=0; i < nMiddle; i++)
	{
		int nRed = (int)GetRValue(crB) * i / nMiddle;
		int nGreen = (int)GetGValue(crA);
		
		pen.CreatePen(PS_SOLID, 1, RGB(nRed, nGreen, 0));
		pOldPen = m_dcLevel.SelectObject(&pen);

		if (m_bHorizontal)
		{
			m_dcLevel.MoveTo(nPart + i, m_rcBody.top);
			m_dcLevel.LineTo(nPart + i, m_rcBody.bottom);
		}
		else
		{
			m_dcLevel.MoveTo(m_rcBody.left, m_rcBody.bottom - nPart - i);
			m_dcLevel.LineTo(m_rcBody.right, m_rcBody.bottom - nPart - i);
		}

		m_dcLevel.SelectObject(pOldPen);
		pen.DeleteObject();
	}

	for (int i=0; i < nMiddle; i++)
	{
		BYTE byRed = GetRValue(crB);
		BYTE byGreen = GetGValue(crA) - GetGValue(crA) * i / nMiddle;

		pen.CreatePen(PS_SOLID, 1, RGB(byRed, byGreen, 0));
		pOldPen = m_dcLevel.SelectObject(&pen);

		if (m_bHorizontal)
		{
			m_dcLevel.MoveTo(nPart + nMiddle + i, m_rcBody.top);
			m_dcLevel.LineTo(nPart + nMiddle + i, m_rcBody.bottom);
		}
		else
		{
			m_dcLevel.MoveTo(m_rcBody.left, m_rcBody.bottom - nMiddle - nPart - i);
			m_dcLevel.LineTo(m_rcBody.right, m_rcBody.bottom - nMiddle - nPart - i);
		}

		m_dcLevel.SelectObject(pOldPen);
		pen.DeleteObject();
	}
}


void CVolumeSlider::OnCaptureChanged(CWnd *pWnd)
{
	m_bLBDown = FALSE;
	m_nBarState = BAR_NORMAL;

	CWnd::OnCaptureChanged(pWnd);
}

double CVolumeSlider::Round(double _x, int decimalnum)
{   
	// round(0.499,2)=0.50
	// round(0.4949,2)=0.49
	double num, dec, factor;
	dec = modf(_x,&num);
	factor = pow((double)10,decimalnum);
	dec *=factor;
	dec += _x >= 0 ? 0.5 : -0.5;
	modf(dec, &dec);
	dec /= factor;
	return num + dec;
};  

void CVolumeSlider::SetDrawScale(BOOL bDraw)
{
	m_bDrawScale = bDraw;
}

BOOL CVolumeSlider::IsDrawScale()
{
	return m_bDrawScale;
}

void CVolumeSlider::SetScaleColor(COLORREF color)
{
	m_crScale = color;

	Repaint();
}

COLORREF CVolumeSlider::GetScaleColor()
{
	return m_crScale;
}

void CVolumeSlider::SetFillPosColor(COLORREF cr)
{
	m_crFill = cr;

	Repaint();
}

COLORREF CVolumeSlider::GetFillPosColor()
{
	return m_crFill;
}

void CVolumeSlider::EnableDrag(BOOL bEnable)
{
	m_bEnableDrag = bEnable;

	if (m_bEnableDrag == FALSE && BAR_DOWN == m_nBarState)
	{
		ReleaseCapture();
		m_bLBDown = FALSE;
		m_nBarState = BAR_NORMAL;
	}

	Repaint();
}

BOOL CVolumeSlider::IsDraging()
{
	return (BAR_DOWN == m_nBarState);
}

BOOL CVolumeSlider::EnableWindow(BOOL bEnable)
{
	m_nBarState = bEnable ? BAR_NORMAL : BAR_DISABLE;

	BOOL bRet = CWnd::EnableWindow(bEnable);

	Repaint();

	return bRet;
}

