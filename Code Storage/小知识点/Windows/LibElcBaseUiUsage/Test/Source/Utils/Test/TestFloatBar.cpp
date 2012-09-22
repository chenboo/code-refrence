#include "stdafx.h"
//#include "Test.h"
//#include "TestFloatBar.h"
//
//IMPLEMENT_DYNAMIC(CTestFloatBar, CWnd)
//
//CTestFloatBar::CTestFloatBar()
//{
//	m_hNotifyWnd = NULL;
//}
//
//CTestFloatBar::~CTestFloatBar()
//{
//}
//
//BEGIN_MESSAGE_MAP(CTestFloatBar, CWnd)
//	ON_WM_CREATE()
//	ON_WM_NCHITTEST()
//	ON_WM_NCMOUSEMOVE()
//	ON_WM_NCLBUTTONDOWN()
//	ON_WM_NCLBUTTONUP()
//	ON_WM_WINDOWPOSCHANGING()
//	ON_WM_TIMER()
//	ON_WM_CLOSE()
//	ON_WM_SETCURSOR()
//	ON_WM_SHOWWINDOW()
//END_MESSAGE_MAP()
//
//BOOL CTestFloatBar::CreateBar(CWnd* pParent)
//{
//	LPCTSTR pClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
//	DWORD dwStyle = WS_POPUP;
//	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED;
//	CRect rc(0, 0, 0, 0);
//
//	if (CreateEx(dwExStyle, pClassName, _T(""), dwStyle, rc, this, 0))
//	{
//		LONG lStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
//		lStyle &= ~WS_EX_LAYOUTRTL;
//		SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyle);
//
//		if (!m_hNotifyWnd && pParent)
//		{
//			m_hNotifyWnd = pParent->GetSafeHwnd();
//		}
//
//		return TRUE;
//	}
//
//	return FALSE;
//}
//
//int CTestFloatBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (__super::OnCreate(lpCreateStruct) == -1)
//	{
//		return -1;
//	}
//
//	ModifyStyle(WS_CAPTION | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME, 0);
//}
//
//LRESULT CTestFloatBar::OnNcHitTest(CPoint point)
//{
//	return CWnd::OnNcHitTest(point);
//}
//
//void CTestFloatBar::OnNcMouseMove(UINT nHitTest, CPoint point)
//{
//	CWnd::OnNcMouseMove(nHitTest, point);
//}
//
//void CTestFloatBar::OnNcLButtonDown(UINT nHitTest, CPoint point)
//{
//	CWnd::OnNcLButtonDown(nHitTest, point);
//}
//
//void CTestFloatBar::OnNcLButtonUp(UINT nHitTest, CPoint point)
//{
//	CWnd::OnNcLButtonUp(nHitTest, point);
//}
//
//void CTestFloatBar::OnWindowPosChanging(WINDOWPOS* lpwndpos)
//{
//	CWnd::OnWindowPosChanging(lpwndpos);
//}
//
//void CTestFloatBar::OnTimer(UINT_PTR nIDEvent)
//{
//	CWnd::OnTimer(nIDEvent);
//}
//
//void CTestFloatBar::OnClose()
//{
//	CWnd::OnClose();
//}
//
//BOOL CTestFloatBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
//{
//	return CWnd::OnSetCursor(pWnd, nHitTest, message);
//}
//
//void CTestFloatBar::OnShowWindow(BOOL bShow, UINT nStatus)
//{
//	CWnd::OnShowWindow(bShow, nStatus);
//}
//
//void CTestFloatBar::ShowBar(BOOL bShow)
//{
//	if(bShow)
//	{
//
//
//
//	}
//
//	if (bShow) {
//		m_sizeBar = CalcItemsPos();
//		int nScreenCx = GetSystemMetrics(SM_CXFULLSCREEN);
//		int x = (nScreenCx - m_sizeBar.cx) / 2;
//		SetWindowPos(NULL, x, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
//		m_dwFoldState = FOLD_STATE_EXPAND;
//		Repaint();
//	}
//	else {
//		ShowWindow(SW_HIDE);
//	}
//
//
//}