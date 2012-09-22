#include "StdAfx.h"
#include "Skinner.h"

HHOOK		CSkinner::m_hhook = NULL;
WNDPROC		CSkinner::m_WndProc = NULL;
MouseState	CSkinner::m_nState = MS_NONE;

CSkinner::CSkinner(void)
{
	m_nState = MS_NONE;
	m_hhook = NULL;
	m_WndProc = NULL;
}

CSkinner::~CSkinner(void)
{
}

void CSkinner::InitSkin()
{
	m_hhook = SetWindowsHookEx(WH_CALLWNDPROC,
		HookProc,
		NULL,
		GetCurrentThreadId());
}

void CSkinner::ExitSkin()
{
	UnhookWindowsHookEx(m_hhook);
}

LRESULT WINAPI CSkinner::HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PCWPSTRUCT	pcs = (PCWPSTRUCT)lParam;
	HWND		hWnd = pcs->hwnd;

	if (nCode < 0 || hWnd == NULL)
	{
		return CallNextHookEx(m_hhook, nCode, wParam, lParam); 
	}

	TCHAR szClassName[201];
	GetClassName(hWnd, szClassName, 200);
	if(wcscmp(szClassName, _T("Button")) != 0 )
	{
		return CallNextHookEx(m_hhook, nCode, wParam, lParam);
	}

	CWnd	*pWnd = CWnd::FromHandle( hWnd );
	DWORD	dwStyle = pWnd->GetStyle();
	if(dwStyle&WS_CHILD && dwStyle&WS_VISIBLE)
	{
		WNDPROC	WndProc;
		WndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
		if(WndProc != (WNDPROC)SkinWindowProc)
		{
			m_WndProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)SkinWindowProc);
		}
	}
	
	return CallNextHookEx(m_hhook, nCode, wParam, lParam);
}


LRESULT CSkinner::SkinWindowProc(HWND hWnd,UINT message,
								WPARAM wParam,
								LPARAM lParam)
{
	CWnd	*pWnd = NULL;
	CPoint	point;
	pWnd = CWnd::FromHandle( hWnd );

	switch( message )
	{
	case WM_PAINT:
		return OnPaint( pWnd );
		break;

	case WM_LBUTTONDOWN:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		return OnLButtonDown( pWnd, 0, point );
		break;

	case WM_LBUTTONUP:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		return OnLButtonUp( pWnd, 0, point );
		break;

	case WM_LBUTTONDBLCLK:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		return OnLButtonDblClk( pWnd, 0, point );
		break;

	case WM_MOUSEMOVE:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		return OnMouseMove( pWnd, 0, point );
		break;

	default:
		break;
	}

	return CallWindowProc(m_WndProc, hWnd, message, wParam, lParam);
}

LRESULT CSkinner::OnLButtonDown(CWnd *pWnd, UINT nFlags, CPoint point)
{
	m_nState = MS_LBUTTONDOWN;
	pWnd->Invalidate();

	return 0;
}

LRESULT CSkinner::OnLButtonUp(CWnd *pWnd, UINT nFlags, CPoint point)
{
	if( m_nState != MS_LBUTTONUP )
	{
		m_nState = MS_LBUTTONUP;
		pWnd->Invalidate();
		SendMessage(pWnd->GetParent()->m_hWnd, WM_COMMAND, pWnd->GetDlgCtrlID(), (LPARAM)(pWnd->m_hWnd));
	}

	return 0;
}

LRESULT CSkinner::OnLButtonDblClk( CWnd *pWnd, UINT nFlags, CPoint point )
{
	return 0;
}

LRESULT CSkinner::OnMouseMove( CWnd *pWnd, UINT nFlags, CPoint point )
{
	HRGN hRgn = CreateRectRgn( 0, 0, 0, 0 );
	pWnd->GetWindowRgn( hRgn );

	BOOL bIn = PtInRegion( hRgn, point.x, point.y );
	if(bIn)
	{
		if(m_nState == MS_LBUTTONDOWN)
		{
			return 0;
		}
		if(m_nState != MS_MOUSEHOVER)
		{
			m_nState = MS_MOUSEHOVER;
			pWnd->Invalidate();
			pWnd->UpdateWindow();
			pWnd->SetCapture();
		}
	}
	else
	{
		if (m_nState == MS_MOUSEHOVER)
		{
			m_nState = MS_LBUTTONUP;
			pWnd->Invalidate();
			pWnd->UpdateWindow();
			ReleaseCapture();
		}
	}

	DeleteObject( hRgn );
	return 0;
}

LRESULT CSkinner::OnPaint( CWnd *pWnd )
{
	CPaintDC	dc(pWnd);
	CString		cs;
	RECT		rc;
	CFont		Font;
	CFont		*pOldFont;
	CBrush		Brush;
	CBrush		*pOldBrush;
	CPen		Pen;
	CPen		*pOldPen;
	POINT		pt;
	pt.x = 2;
	pt.y = 2;

	dc.SetBkMode( TRANSPARENT );
	Font.CreateFont( 12, 0, 0, 0, FW_HEAVY, 0, 0, 0, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, _T("MS Sans Serif" ));
	pOldFont = dc.SelectObject( &Font );

	if( m_nState == MS_LBUTTONDOWN )
	{
		Brush.CreateSolidBrush( RGB( 160, 160, 160 ) );
		Pen.CreatePen( PS_SOLID, 1, RGB( 100, 100, 100 ) );
		dc.SetTextColor( RGB( 50, 50, 250 ) );
	}
	else if( m_nState == MS_MOUSEHOVER )
	{
		Brush.CreateSolidBrush( RGB( 60, 60, 180 ) );
		Pen.CreatePen( PS_SOLID, 1, RGB( 0, 0, 0 ) );
		dc.SetTextColor( RGB( 250, 250, 50 ) );
	}
	else if( m_nState == MS_LBUTTONUP )
	{
		Brush.CreateSolidBrush( RGB( 240, 240, 240 ) );
		Pen.CreatePen( PS_SOLID, 1, RGB( 120, 120, 120 ) );
		dc.SetTextColor( RGB( 50, 50, 50 ) );
	}

	pOldBrush = dc.SelectObject( &Brush );
	pOldPen = dc.SelectObject( &Pen );
	pWnd->GetClientRect( &rc );
	dc.RoundRect( &rc, pt );

	HRGN hRgn = CreateRectRgn( rc.left, rc.top, rc.right, rc.bottom );
	pWnd->SetWindowRgn( hRgn, TRUE );
	DeleteObject( hRgn );

	pWnd->GetWindowText( cs );
	dc.DrawText( cs, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );

	dc.SelectObject( pOldFont );
	dc.SelectObject( pOldBrush );
	dc.SelectObject( pOldPen );

	return 0;
}


LRESULT CSkinner::OnEraseBkgnd( CWnd *pWnd, CDC *pDC )
{
	return TRUE;
}

