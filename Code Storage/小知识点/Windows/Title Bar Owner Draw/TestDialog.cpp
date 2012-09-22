#include "stdafx.h"
#include "Dialog.h"
#include "TestDialog.h"

// CTestDialog dialog

IMPLEMENT_DYNAMIC(CTestDialog, CDialog)

CTestDialog::CTestDialog(CWnd* pParent /*=NULL*/)
: CDialog(CTestDialog::IDD, pParent)
{
	m_bFocus = FALSE;
}

CTestDialog::~CTestDialog()
{
}

void CTestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCLBUTTONUP()
END_MESSAGE_MAP()

void CTestDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void CTestDialog::OnNcPaint()
{
	DrawTitleBar();
	DrawSysMenu();
}

int CTestDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	int nxfthick = GetSystemMetrics(SM_CXFRAME);
	int nyfthick = GetSystemMetrics(SM_CYFRAME);

	CRect rcWindow;
	GetWindowRect(&rcWindow);
	m_rcTilte.left = 0;
	m_rcTilte.top = 0;
	m_rcTilte.right = rcWindow.Width();
	m_rcTilte.bottom = nyfthick + GetSystemMetrics(SM_CYSIZE);

	m_bmpExitNormal.LoadBitmap(IDB_EXIT_NORMAL);
	m_bmpExitFocus.LoadBitmap(IDB_EXIT_FOCUS);

	BITMAP bm;
	m_bmpExitNormal.GetBitmap(&bm);

	int nright = m_rcTilte.right - nxfthick;
	int nleft = nright - bm.bmWidth;
	int ntop = nyfthick;
	int nbottom = ntop + bm.bmHeight;

	m_rcExitButton.SetRect(nleft, ntop, nright, nbottom);

	return 0;
}

void CTestDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	DrawTitleBar();
	DrawSysMenu();
}

void CTestDialog::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	CRect rcWnd,rect;
	GetWindowRect(&rcWnd);

	rect = m_rcExitButton;
	rect.OffsetRect(rcWnd.left,rcWnd.top);

	if(rect.PtInRect(point))
	{
		SendMessage(WM_CLOSE);
		return;
	}

	CDialog::OnNcLButtonDown(nHitTest, point);
}

void CTestDialog::DrawTitleBar()
{
	if(!m_hWnd)
	{
		return;
	}

	CDC* pDC = GetWindowDC();

	CBitmap bmp;
	bmp.LoadBitmap(IDB_TITLE);
	CBrush brush;
	brush.CreatePatternBrush(&bmp);
	
	CBrush Brush(RGB(200,205,225));
	CBrush* pOldBrush = pDC->SelectObject(&Brush);

	CRect rcWindow;
	GetWindowRect(&rcWindow);

	int nxfthick = GetSystemMetrics(SM_CXFRAME);
	int nyfthick = GetSystemMetrics(SM_CYFRAME);

	//file the tilte
	pDC->FillRect(&m_rcTilte, &brush);

	//fill the left frame
	int nleft = 0;
	int ntop = m_rcTilte.bottom;
	int nwidth = nxfthick;
	int nheight = rcWindow.Height() - ntop - nyfthick;
	pDC->PatBlt(nleft, ntop, nwidth, nheight, PATCOPY);

	//fill the bottom frame
	nleft = 0;
	ntop += nheight;
	nwidth = rcWindow.Width();
	nheight = nyfthick;
	pDC->PatBlt(nleft, ntop, nwidth, nheight, PATCOPY);

	//fill the right frame
	nleft = rcWindow.Width() - nxfthick;
	ntop = m_rcTilte.bottom;
	nwidth = nxfthick;
	nheight = rcWindow.Height() - ntop;
	pDC->PatBlt(nleft, ntop, nwidth, nheight, PATCOPY);

	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
	bmp.DeleteObject();

	ReleaseDC(pDC);
}

void CTestDialog::DrawSysMenu()
{
	CDC* pDC = GetWindowDC();

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap* pOldbm = NULL;
	
	if(m_bFocus)
	{
		pOldbm = dcMem.SelectObject(&m_bmpExitFocus);
	}
	else
	{
		pOldbm = dcMem.SelectObject(&m_bmpExitNormal);
	}

	pDC->BitBlt(m_rcExitButton.left,
		m_rcExitButton.top,
		m_rcExitButton.Width(),
		m_rcExitButton.Height(),
		&dcMem,
		0,
		0,
		SRCCOPY);

	dcMem.SelectObject(pOldbm);
	dcMem.DeleteDC();

	ReleaseDC(pDC);
}

void CTestDialog::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	CRect rcWnd,rect;
	GetWindowRect(&rcWnd);

	rect = m_rcExitButton;
	rect.OffsetRect(rcWnd.left,rcWnd.top);

	if(rect.PtInRect(point))
	{
		m_bFocus = TRUE;
	}
	else
	{
		m_bFocus = FALSE;
	}

	DrawSysMenu();

	CDialog::OnNcMouseMove(nHitTest, point);
}

void CTestDialog::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	CDialog::OnNcLButtonUp(nHitTest, point);
}

