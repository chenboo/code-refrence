// RollText.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "RollLabel.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CRollLabel

IMPLEMENT_DYNAMIC(CRollLabel, CWnd)

CRollLabel::CRollLabel()
{
	LOGFONT lf = {0};
	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	m_ftText.CreateFontIndirect(&lf);

	m_crText = RGB(0,0,0);
	m_rcClient = CRect(0,0,0,0);
	m_sizeText = CSize(0,0);
	m_nCurXPos = 0;
	m_nCurYPos = 0;

	m_crBk = RGB(255,255,255);
	m_bTransparentBk = FALSE;

	m_bRollTimer = FALSE;
	m_nRollElapse = 100;

	m_nTextAlign = ELCRLABEL_TEXT_ALIGN_LEFT;
}


CRollLabel::~CRollLabel()
{
}


BEGIN_MESSAGE_MAP(CRollLabel, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CRollLabel message handlers


void CRollLabel::SetText(LPCTSTR lpszText, COLORREF crText)
{
	m_strText = lpszText;
	m_crText = TRUE;

	UpdateTimer();
}

void CRollLabel::SetText(LPCTSTR lpszText)
{
	m_strText = lpszText;

	UpdateTimer();
}

void CRollLabel::SetTextColor(COLORREF crText)
{
	m_crText = crText;

	if (::IsWindow(m_hWnd))
		Invalidate();
}

BOOL CRollLabel::SetTextFont(CFont* pFont)
{
	if (!pFont || !pFont->GetSafeHandle())
		return FALSE;

	if (m_ftText.GetSafeHandle())
		m_ftText.DeleteObject();

	LOGFONT lf = {0};
	pFont->GetLogFont(&lf);

	if (!m_ftText.CreateFontIndirect(&lf))
	{
		return FALSE;
	}

	UpdateTimer();

	return TRUE;
}

CFont* CRollLabel::GetTextFont()
{
	return &m_ftText;
}

CSize CRollLabel::GetTextSize(LPCTSTR lpszText, CFont* pFont)
{
	CSize size = CSize(0,0);

	LOGFONT lf = {0};
	CFont font;

	if (!pFont || !pFont->GetSafeHandle())
	{
		::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	}
	else
	{
		pFont->GetLogFont(&lf);
	}

	font.CreateFontIndirect(&lf);

	HDC hDC = ::GetDC(NULL);
	if (hDC)
	{
		CDC* pDC = CDC::FromHandle(hDC);
		if (pDC)
			size = pDC->GetTextExtent(lpszText);

		::ReleaseDC(NULL, hDC);
	}

	return size;
}

void CRollLabel::UpdateTimer()
{
	if (!::IsWindow(GetSafeHwnd()))
	{
		ASSERT(0);
		return;
	}

	if(m_bRollTimer)
	{
		KillTimer(ID_TIMER_ROLL);
		m_bRollTimer = FALSE;
	}

	CDC* pDC = GetDC();
	ASSERT(pDC);
	if (!pDC)
		return;

	CFont* pOldFont = pDC->SelectObject(&m_ftText);
	m_sizeText = pDC->GetTextExtent(m_strText);
	pDC->SelectObject(pOldFont);
	
	ReleaseDC(pDC);

	if (m_rcClient.Width() < m_sizeText.cx) 
	{
		SetTimer(ID_TIMER_ROLL, m_nRollElapse, NULL);
		m_bRollTimer = TRUE;
	}
	else 
	{
		m_bRollTimer = FALSE;
	}

	m_nCurYPos = (m_rcClient.Height() - m_sizeText.cy) / 2;
	m_nCurXPos = 0;

	Repaint();
}

CString CRollLabel::GetText()
{
	return m_strText;
}

COLORREF CRollLabel::GetTextColor()
{
	return m_crText;
}

void CRollLabel::SetBkColor(BOOL bTransparentBk, COLORREF crBk)
{
	m_bTransparentBk = bTransparentBk;
	m_crBk = crBk;
}

COLORREF CRollLabel::GetBkColor()
{
	return m_crBk;
}

BOOL CRollLabel::IsTransparentBk()
{
	return m_bTransparentBk;
}

void CRollLabel::SetRollElapse(int nMilliSecond)
{
	m_nRollElapse = nMilliSecond;
	if (m_bRollTimer)
		SetTimer(ID_TIMER_ROLL, nMilliSecond, NULL);
}

int CRollLabel::GetRollElapse()
{
	return m_nRollElapse;
}

void CRollLabel::SetTextAlign(int nAlign)
{
	m_nTextAlign = nAlign;

	Repaint();
}

int CRollLabel::GetTextAlign()
{
	return m_nTextAlign;
}

void CRollLabel::OnPaint()
{
	CPaintDC dc(this);
	
	DWORD dwLayout;
	GetProcessDefaultLayout(&dwLayout);
	if (LAYOUT_RTL & dwLayout) 
		dc.SetLayout(LAYOUT_RTL);

	dc.SetBkMode(TRANSPARENT);
	CFont *pOldFont = dc.SelectObject(&m_ftText);
	dc.SetTextColor(m_crText);

	if (m_bRollTimer)
	{
		dc.TextOut(m_nCurXPos, m_nCurYPos, m_strText);
	}
	else
	{
		UINT uFormat = DT_VCENTER|DT_SINGLELINE;
		if (m_nTextAlign == ELCRLABEL_TEXT_ALIGN_LEFT)
			uFormat |= DT_LEFT;
		else if (m_nTextAlign == ELCRLABEL_TEXT_ALIGN_CENTER)
			uFormat |= DT_CENTER;
		else if (m_nTextAlign == ELCRLABEL_TEXT_ALIGN_RIGHT)
			uFormat |= DT_RIGHT;
		dc.DrawText(m_strText, m_rcClient, uFormat);
	}

	dc.SelectObject(pOldFont);
}

void CRollLabel::OnTimer(UINT_PTR nIDEvent)
{
	if (ID_TIMER_ROLL == nIDEvent)
	{
		if (m_sizeText.cx + m_nCurXPos <= 0)
			m_nCurXPos = m_rcClient.right;

		m_nCurXPos -= 1;

		Repaint();
	}

	CWnd::OnTimer(nIDEvent);
}

void CRollLabel::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	GetClientRect(&m_rcClient);

	UpdateTimer();
}

BOOL CRollLabel::OnEraseBkgnd(CDC* pDC)
{
	if (!m_bTransparentBk)
		pDC->FillSolidRect(m_rcClient, m_crBk);

	return TRUE;
}

void CRollLabel::Repaint()
{
	if (m_bTransparentBk)
	{
		CWnd* pParent = GetParent();
		if (pParent) 
		{
			CRect rcRepaint = m_rcClient;
			ClientToScreen(&rcRepaint);
			pParent->ScreenToClient(&rcRepaint);
			pParent->InvalidateRect(rcRepaint);
		}
		else
		{
			Invalidate();
		}
	}
	else
	{
		Invalidate();
	}
}
