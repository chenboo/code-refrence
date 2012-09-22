// ElcSkinBackgndWnd.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcSkinBackgndWnd.h"


// CElcSkinBackgndWnd

IMPLEMENT_DYNAMIC(CElcSkinBackgndWnd, CWnd)

CElcSkinBackgndWnd::CElcSkinBackgndWnd()
{
	m_pNCSkin = NULL;
	m_pCSkin = NULL;
}

CElcSkinBackgndWnd::~CElcSkinBackgndWnd()
{
}


BEGIN_MESSAGE_MAP(CElcSkinBackgndWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CElcSkinBackgndWnd message handlers


BOOL CElcSkinBackgndWnd::SetNCSkin(LPCTSTR lpStyle)
{
	if (!m_pNCSkin || _tcsicmp(m_pNCSkin->szStyle, lpStyle) != 0) {
		m_pNCSkin = theSkin.GetNCPaneSkin(lpStyle);
	}

	if (m_pNCSkin && ::IsWindow(m_hWnd)) {
		return theNCBinder.BindWindow(m_hWnd, m_pNCSkin->szStyle);
	}

	return m_pNCSkin != NULL;
}

BOOL CElcSkinBackgndWnd::SetClientSkin(LPCTSTR lpStyle)
{
	if (!m_pCSkin || _tcsicmp(m_pCSkin->szStyle, lpStyle) != 0) {
		m_pCSkin = theSkin.GetClientPaneSkin(lpStyle);
	}

	return m_pCSkin != NULL;
}

int CElcSkinBackgndWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CElcSkinBackgndWnd::Draw(CDC* pDC)
{
	Graphics g(pDC->m_hDC);
	Draw(&g);
}

void CElcSkinBackgndWnd::Draw(PVOID pvGraphics)
{
	if (!m_pCSkin)
		return;

	CRect rcClient;
	GetClientRect(rcClient);

	Bitmap bitmap(rcClient.Width(), rcClient.Height());
	Graphics gMem(&bitmap);

	Rect rect;
	rect.X = rcClient.left;
	rect.Y = rcClient.top;
	rect.Width = rcClient.Width();
	rect.Height = rcClient.Height();

	if (!m_pCSkin->background.pImage) {
		Color crBackgnd;
		crBackgnd.SetFromCOLORREF(m_pCSkin->crBackgnd);
		gMem.FillRectangle(&SolidBrush(crBackgnd), rect);
	}
	else {
		thePainter.DrawImage(&gMem, 
		&m_pCSkin->background,
		&rect,
		m_pCSkin->crBackgnd,
		0, 
		TRUE,
		NULL);
	}

	((Graphics *)pvGraphics)->DrawImage(&bitmap, rect.X, rect.Y);
}

BOOL CElcSkinBackgndWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
