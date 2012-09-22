// ElcSpliter.cpp : implementation file
//

#include "stdafx.h"
#include "ElcSplitter.h"

#define UNLIMITED_ENDPOS		0x0FFFFFFF


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CElcSplitter

IMPLEMENT_DYNAMIC(CElcSplitter, CWnd)

CElcSplitter::CElcSplitter()
{
	m_pParent = NULL;
	m_bHorizontal = TRUE;
	m_bLBDown = FALSE;
	m_ptOld = CPoint(0,0);

	m_crBk = RGB(204,218,240);
	m_crBorder = RGB(127,157,185);

	m_hNotifyWnd = NULL;
	m_nNotifyMsg = 0;

	m_nStartPos = 0;
	m_nEndPos = UNLIMITED_ENDPOS;

	m_hCursor = NULL;
	m_bUseDesktopDC = TRUE;
}

CElcSplitter::~CElcSplitter()
{
}

BEGIN_MESSAGE_MAP(CElcSplitter, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()



// CElcSplitter message handlers

BOOL CElcSplitter::Create(CWnd* pParent, UINT nID, const RECT& rect, BOOL bHorizontal)
{
	ASSERT(pParent);

	m_pParent = pParent;
	m_bHorizontal = bHorizontal;

	if (m_bHorizontal)
	{
		m_hCursor = ::LoadCursor(NULL, IDC_SIZENS);
	}
	else
	{
		m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
	}

	return CWnd::Create(NULL, NULL, WS_CHILD|WS_VISIBLE, rect, pParent, nID);
}

void CElcSplitter::SetNotifyWindow(HWND hWnd, UINT nMessage)
{
	m_hNotifyWnd = hWnd;
	m_nNotifyMsg = nMessage;
}

void CElcSplitter::SetThickness(int nThickness)
{
	ASSERT(::IsWindow(m_hWnd));

	if (!::IsWindow(m_hWnd))
		return;

	CRect rc;
	GetRelativePos(rc);
	
	if (m_bHorizontal)
	{
		rc.bottom = rc.top + nThickness;
	}
	else
	{
		rc.right = rc.left + nThickness;
	}

	MoveWindow(rc);
}

int CElcSplitter::GetThickness()
{
	if (m_bHorizontal)
		return m_rcClient.Height();
	else
		return m_rcClient.Width();
}

void CElcSplitter::SetCustomCursor(HINSTANCE hInstance, UINT nResID)
{
	m_hCursor = ::LoadCursor(hInstance, MAKEINTRESOURCE(nResID));
	SetCursor(m_hCursor);
}

void CElcSplitter::OnPaint()
{
	CPaintDC dc(this);

	dc.FillSolidRect(m_rcClient, m_crBk);
	CPen pen, *pOldPen=NULL;
	pen.CreatePen(PS_SOLID, 1, m_crBorder);
	pOldPen = dc.SelectObject(&pen);

	if (m_bHorizontal)
	{
		dc.MoveTo(m_rcClient.left, m_rcClient.top);
		dc.LineTo(m_rcClient.right, m_rcClient.top);
		dc.MoveTo(m_rcClient.left, m_rcClient.bottom-1);
		dc.LineTo(m_rcClient.right, m_rcClient.bottom-1);
	}
	else
	{
		dc.MoveTo(m_rcClient.left, m_rcClient.top);
		dc.LineTo(m_rcClient.left, m_rcClient.bottom);
		dc.MoveTo(m_rcClient.right-1, m_rcClient.top);
		dc.LineTo(m_rcClient.right-1, m_rcClient.bottom);
	}

	dc.SelectObject(pOldPen);
}

BOOL CElcSplitter::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CElcSplitter::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nStartPos == m_nEndPos)
		return;

	SetCursor(m_hCursor);

	SetCapture();
	m_bLBDown = TRUE;	

	m_ptOld = point;
	m_ptOffset = -point;

	m_ptOld.Offset(m_ptOffset);

	InverTracker(m_ptOld);

	CWnd::OnLButtonDown(nFlags, point);
}

void CElcSplitter::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bLBDown)
		return;

	m_bLBDown = FALSE;

	SetCursor(m_hCursor);

	ReleaseCapture();

	InverTracker(m_ptOld);

	point.Offset(m_ptOffset);
	CheckRange(point);

	CRect rc;
	GetRelativePos(rc);

	if (m_bHorizontal)
	{
		::PostMessage(m_hNotifyWnd, m_nNotifyMsg, GetDlgCtrlID(), (LPARAM)(rc.top + point.y));
	}
	else
	{
		::PostMessage(m_hNotifyWnd, m_nNotifyMsg, GetDlgCtrlID(), (LPARAM)(rc.left + point.x));
	}
}

void CElcSplitter::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nStartPos == m_nEndPos)
		return;

	if (!m_bLBDown)
	{
		if (m_rcClient.PtInRect(point))
		{
			SetCursor(m_hCursor);
		}

		return;
	}

	point.Offset(m_ptOffset);

	CheckRange(point);

	InverTracker(m_ptOld);
	InverTracker(point);

	m_ptOld = point;
}

void CElcSplitter::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	GetClientRect(&m_rcClient);
}

inline void CElcSplitter::GetRelativePos(CRect& rc)
{
	ASSERT(m_pParent);

	GetWindowRect(&rc);
	m_pParent->ScreenToClient(&rc);
}

void CElcSplitter::CheckRange(CPoint& point)
{
	CRect rc;
	GetRelativePos(rc);

	if (m_bHorizontal)
	{	
		if (rc.top + point.y < m_nStartPos)
		{
			point.y = m_nStartPos - rc.top;
		}
		else if (rc.top + point.y > m_nEndPos)
		{
			point.y = m_nEndPos - rc.top;
		}
	}
	else
	{	
		if (rc.left + point.x < m_nStartPos)
		{
			point.x = m_nStartPos - rc.left;
		}
		else if (rc.left + point.x > m_nEndPos)
		{
			point.x = m_nEndPos - rc.left;
		}
	}
}

void CElcSplitter::UseDesktopDC(BOOL bUse)
{
	m_bUseDesktopDC = bUse;
}

void CElcSplitter::InverTracker(const POINT& point)
{
	HDC hDC = NULL;
	CDC* pDC = NULL;
	if (m_bUseDesktopDC)
	{
		hDC = ::GetDC(NULL);
		if (!hDC)
			return;
		pDC = CDC::FromHandle(hDC);
	}
	else
	{
		pDC = m_pParent->GetDC();
	}

	if (pDC == NULL)
		return ;

	CBrush* pBrush = CDC::GetHalftoneBrush();
	HBRUSH hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);


	CRect rc;
	if (m_bUseDesktopDC)
		GetWindowRect(&rc);
	else
		GetRelativePos(rc);

	if (m_bHorizontal)
	{
		rc.top += point.y;
		rc.bottom += point.y;
	}
	else
	{
		rc.left += point.x;
		rc.right += point.x;
	}

	pDC->PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATINVERT);

	if (hOldBrush != NULL)
		SelectObject(pDC->m_hDC, hOldBrush);

	if (m_bUseDesktopDC)
		::ReleaseDC(NULL, hDC);
	else
		ReleaseDC(pDC);
}

void CElcSplitter::SetBkColor(COLORREF cr)
{
	m_crBk = cr;
	
	if (::IsWindow(m_hWnd))
		Invalidate();
}

COLORREF CElcSplitter::GetBkColor()
{
	return m_crBk;
}

void CElcSplitter::SetBorderColor(COLORREF cr)
{
	m_crBorder = cr;

	if (::IsWindow(m_hWnd))
		Invalidate();
}

COLORREF CElcSplitter::GetBorderColor()
{
	return m_crBorder;
}

void CElcSplitter::SetRange(int nStart, int nEnd)
{
	ASSERT(nStart>=0 && nEnd>=0 && nStart <= nEnd);

	if (nStart<=0 && nEnd<=0 && nStart > nEnd)
		return;
	
	m_nStartPos = nStart;
	m_nEndPos = nEnd;
}

void CElcSplitter::GetRange(int& nStart, int& nEnd)
{
	nStart = m_nStartPos;
	nEnd = m_nEndPos;
}

void CElcSplitter::SetPos(int nPos)
{
	ASSERT(m_nStartPos <= nPos && m_nEndPos >= nPos);
	if (m_nStartPos > nPos || m_nEndPos < nPos)
		return;

	CRect rc;
	GetRelativePos(rc);
	BOOL bNeedMove = FALSE;

	if (m_bHorizontal)
	{
		if (rc.top != nPos) {
			bNeedMove = TRUE;
			int nHeight = rc.Height();
			rc.top = nPos;
			rc.bottom = nHeight + rc.top;
		}
	}
	else
	{
		if (rc.left != nPos) {
			bNeedMove = TRUE;
			int nWidth = rc.Width();
			rc.left = nPos;
			rc.right = rc.left + nWidth;
		}
	}

	if (bNeedMove)
		MoveWindow(rc);
}

int CElcSplitter::GetPos()
{
	CRect rc;
	GetRelativePos(rc);

	if (m_bHorizontal)
	{
		return rc.top;
	}
	else
	{
		return rc.left;
	}
}

void CElcSplitter::OnCaptureChanged(CWnd *pWnd)
{
	if (m_bLBDown)
	{
		m_bLBDown = FALSE;
		InverTracker(m_ptOld);
	}
	
	CWnd::OnCaptureChanged(pWnd);
}
