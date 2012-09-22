#include "StdAfx.h"
#include "DrawButton.h"

CDrawButton::CDrawButton(void)
{
	m_pImage = NULL;
	m_hParent = NULL;
	m_STATE = ST_NONE;
	m_nStyle = BT_NORMAL;

	m_bMenu = FALSE;
	m_bToggle = FALSE;
	m_bEnRadio = FALSE;
	m_bClickDown = FALSE;

	m_nUnitHeight = 0;
	m_nImageWidth = 0;
	m_nImageHeight = 0;
}

CDrawButton & CDrawButton::operator = (const CDrawButton& drawButton)
{
	m_rcBtn = drawButton.m_rcBtn;
	m_hParent = drawButton.m_hParent;

	m_nID = drawButton.m_nID;
	m_nImageID = drawButton.m_nImageID;
	SetPNGImage(m_nImageID);

	m_STATE = drawButton.m_STATE;
	m_nStyle = drawButton.m_nStyle;

	m_bClickDown = drawButton.m_bClickDown;
	m_bToggle = drawButton.m_bToggle;

	return *this;
}

CDrawButton::~CDrawButton(void)
{
	if(m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

void CDrawButton::Create(UINT nID, HWND hParent, CRect &rect)
{
	m_nID = nID;
	m_hParent = hParent;
	m_rcBtn = rect;
}

void CDrawButton::SetEnhanceRadio(BOOL bEnhance)
{
	m_bEnRadio = bEnhance;
}

void CDrawButton::SetMenuRadio(BOOL bMenu)
{
	m_bMenu = bMenu;
}

void CDrawButton::SetButtonID(UINT nID)
{
	m_nID = nID;
}

void CDrawButton::SetButtonRect(CRect& rect)
{
	m_rcBtn = rect;
}

void CDrawButton::SetParenthwnd(HWND hwnd)
{
	ASSERT(hwnd);
	m_hParent = hwnd;
}

void CDrawButton::SetButtonStyle(BT_STYLE nStyle)
{
	m_nStyle = nStyle;
}

int CDrawButton::GetButtonID()
{
	return m_nID;
}

CRect CDrawButton::GetButtonRect()
{
	return m_rcBtn;
}

ST_TATE CDrawButton::GetButtonState()
{
	return m_STATE;
}

void CDrawButton::SetPNGImage(UINT nImageID)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource (hInst, MAKEINTRESOURCE(nImageID), _T("PNG")); // type
	if (!hRsrc)
	{
		return;
	}

	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
	{
		return;
	}

	// Allocate global memory on which to create stream
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem, lpRsrc, len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem, FALSE, &pstm);
	
	// load from stream
	if(m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	m_nImageID = nImageID;
	m_pImage = Image::FromStream(pstm);

	m_nImageWidth = m_pImage->GetWidth();
	m_nImageHeight = m_pImage->GetHeight();
	m_nUnitHeight = m_nImageHeight / 4;

	// free/release stuff
	GlobalUnlock(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);
}

BOOL CDrawButton::IsptInButton(CPoint point)
{
	return m_rcBtn.PtInRect(point);
}

void CDrawButton::MouseState(UINT nFlags, CPoint point)
{
	if(ST_DISABLED == m_STATE)
	{
		return;
	}

	if(MU_LDCLK == nFlags)
	{
		nFlags = MU_LBTNDOWN;
	}

	if(MU_MOUSELEAVE == nFlags)
	{
		if(ST_SELECTED != m_STATE)
		{
			m_STATE = ST_NONE;
			::InvalidateRect(m_hParent, m_rcBtn, FALSE);
		}
	}
	else
	{
		if(BT_NORMAL == m_nStyle)
		{
			NoramalMouseSate(nFlags, point);
		}
		else if(BT_RADIO == m_nStyle)
		{
			RadioMouseSate(nFlags, point);
		}
		else if(BT_TOGGLE == m_nStyle)
		{
			ToggleMouseSate(nFlags, point);
		}
		else if(BT_NULL == m_nStyle)
		{
			return;
		}

		::InvalidateRect(m_hParent, m_rcBtn, FALSE);
	}
}

void CDrawButton::NoramalMouseSate(UINT nFlags, CPoint point)
{
	if(!m_rcBtn.PtInRect(point))
	{
		m_STATE = ST_NONE;
		m_bClickDown = FALSE;
	}
	else
	{
		switch(nFlags)
		{
		case MU_LBTNDOWN:
			m_bClickDown = TRUE;
			m_STATE = ST_LBTNIN;
			break;
		case MU_LBTNUP:
			if(m_bClickDown)
			{
				::PostMessage(m_hParent, WM_BTNCLICK, m_nID, TRUE);
			}

			m_STATE = ST_FOCUS;
			m_bClickDown = FALSE;
			break;
		case MU_MOVE:
		case MU_LBTNMOUSE:
			m_STATE = ST_FOCUS;
			break;
		default:
			m_STATE = ST_NONE;
			break;
		}
	}
}

void CDrawButton::RadioMouseSate(UINT nFlags, CPoint point)
{
	if(m_rcBtn.PtInRect(point))
	{
		switch(nFlags)
		{
		case MU_LBTNDOWN:
			m_bClickDown = TRUE;
			break;
		case MU_LBTNUP:
			if(m_bClickDown)
			{
				if(m_bEnRadio)
				{
					m_STATE = ST_SELECTED;
					::PostMessage(m_hParent, WM_BTNCLICK, m_nID, TRUE);
				}
				else if( m_STATE != ST_SELECTED)
				{
					m_STATE = ST_SELECTED;
					::PostMessage(m_hParent, WM_BTNCLICK, m_nID, TRUE);
				}
			}
			else
			{
				::PostMessage(m_hParent, WM_BTNCLICK, m_nID, FALSE);
			}
			
			m_bClickDown = FALSE;
			break;
		case MU_MOVE:
		case MU_LBTNMOUSE:
			if(ST_SELECTED != m_STATE)
			{
				m_STATE = ST_FOCUS;
			}
			break;
		default:
			break;
		}
	}
	else
	{
		if(ST_SELECTED == m_STATE)
		{
			if(MU_LBTNUP == nFlags)
			{
				m_STATE = ST_NONE;
			}
		}
		else
		{
			m_STATE = ST_NONE;
		}

		if(MU_LBTNDOWN == m_STATE)
		{
			m_bClickDown = FALSE;
		}
	}
}

void CDrawButton::ToggleMouseSate(UINT nFlags, CPoint point)
{
	if(m_rcBtn.PtInRect(point))
	{
		switch(nFlags)
		{
		case MU_LBTNDOWN:
			m_bClickDown = TRUE;
			break;
		case MU_LBTNUP:
			if(m_bClickDown)
			{
				m_STATE = ST_SELECTED;
				m_bToggle = !m_bToggle;
				::PostMessage(m_hParent, WM_BTNCLICK, m_nID, TRUE);
			}

			m_bClickDown = FALSE;
			break;
		case MU_MOVE:
		case MU_LBTNMOUSE:
			if(ST_SELECTED != m_STATE)
			{
				m_STATE = ST_FOCUS;
			}
			break;
		default:
			break;
		}
	}
	else
	{
		m_STATE = ST_NONE;
	}
}

void CDrawButton::SetToggle(BOOL bToggle)
{
	m_bToggle = bToggle;
	::InvalidateRect(m_hParent, m_rcBtn, FALSE);
}

void CDrawButton::DrawButton(CDC* pDC)
{
	if(!m_pImage)
	{
		return;
	}

	if(BT_NORMAL == m_nStyle)
	{
		DrawNormalButton(pDC);
	}
	else if(BT_RADIO == m_nStyle)
	{
		DrawRadioButton(pDC);
	}
	else if(BT_TOGGLE == m_nStyle)
	{
		DrawToggleButton(pDC);
	}
}

void CDrawButton::DrawNormalButton(CDC* pDC)
{
	int nscry = 0;

	if(ST_DISABLED == m_STATE)
	{
		nscry = m_nUnitHeight * 3;
	}
	else if(m_bClickDown)
	{
		nscry = m_nUnitHeight * 2;
	}
	else if(ST_FOCUS == m_STATE)
	{
		nscry = m_nUnitHeight;
	}
	else if(ST_SELECTED == m_STATE)
	{
		nscry = m_nUnitHeight * 2;
	}

	Graphics graph(pDC->m_hDC);
	Rect rcDest;
	rcDest.X = m_rcBtn.left;
	rcDest.Y = m_rcBtn.top;
	rcDest.Width = m_rcBtn.Width();
	rcDest.Height = m_rcBtn.Height();
	graph.DrawImage(m_pImage, rcDest, 0, nscry, m_nUnitHeight, m_nUnitHeight, UnitPixel);
}

void CDrawButton::DrawRadioButton(CDC* pDC)
{
	int nscry = 0;

	if(ST_DISABLED == m_STATE)
	{
		nscry = m_nUnitHeight * 3;
	}
	else if(m_bClickDown)
	{
		nscry = m_nUnitHeight * 2;
	}
	else if(ST_FOCUS == m_STATE)
	{
		nscry = m_nUnitHeight;
	}
	else if(ST_SELECTED == m_STATE)
	{
		nscry = m_nUnitHeight * 2;
	}

	Graphics graph(pDC->m_hDC);
	Rect rcDest;
	rcDest.X = m_rcBtn.left;
	rcDest.Y = m_rcBtn.top;
	rcDest.Width = m_rcBtn.Width();
	rcDest.Height = m_rcBtn.Height();
	graph.DrawImage(m_pImage, rcDest, 0, nscry, m_nUnitHeight, m_nUnitHeight, UnitPixel);

	if(m_bMenu)
	{
		PointF pt[3];
		pt[0].X = m_rcBtn.right - 2.0f;
		pt[0].Y = m_rcBtn.bottom - 9.0f;

		pt[1].X = m_rcBtn.right - 2.0f;
		pt[1].Y = m_rcBtn.bottom - 3.0f;

		pt[2].X = m_rcBtn.right - 9.0f;
		pt[2].Y = m_rcBtn.bottom - 3.0f;

		SolidBrush br(Color::Black);
		graph.FillPolygon(&br, pt, 3);
	}
}

void CDrawButton::DrawToggleButton(CDC* pDC)
{
	int nscry = 0;

	if(ST_DISABLED == m_STATE)
	{
		nscry = m_nUnitHeight * 3;
	}
	else
	{
		if(m_bToggle)
		{
			nscry = m_nUnitHeight * 2;
		}
		else if(ST_FOCUS == m_STATE)
		{
			nscry = m_nUnitHeight;
		}
	}

	Graphics graph(pDC->m_hDC);
	Rect rcDest;
	rcDest.X = m_rcBtn.left;
	rcDest.Y = m_rcBtn.top;
	rcDest.Width = m_rcBtn.Width();
	rcDest.Height = m_rcBtn.Height();
	graph.DrawImage(m_pImage, rcDest, 0, nscry, m_nUnitHeight, m_nUnitHeight, UnitPixel);
}

void CDrawButton::SetButtonState(ST_TATE nState)
{
	m_STATE = nState;
	::InvalidateRect(m_hParent, m_rcBtn, FALSE);
}

void CDrawButton::SetButtonToggle(BOOL bToggled)
{
	m_bToggle = bToggled;
	::InvalidateRect(m_hParent, m_rcBtn, FALSE);
}

void CDrawButton::EnableButtun(BOOL bEnable)
{
	if(!bEnable)
	{
		m_STATE = ST_DISABLED;
	}
	else
	{
		m_STATE = ST_NONE;
	}

	::InvalidateRect(m_hParent, m_rcBtn, FALSE);
}

void CDrawButton::SetClickDown(BOOL bClkdown)
{
	m_bClickDown = bClkdown;
}

