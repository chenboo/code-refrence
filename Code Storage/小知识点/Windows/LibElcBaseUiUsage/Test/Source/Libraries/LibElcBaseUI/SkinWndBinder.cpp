#include "StdAfx.h"
#include "LibElcBaseUI.h"
#include "SkinWndBinder.h"


CSkinWndBinder::CSkinWndBinder(void)
{
}

CSkinWndBinder::~CSkinWndBinder(void)
{
}

BOOL CSkinWndBinder::BindWindow(CWnd* pWnd, LPCTSTR lpStyle)
{
	if (!pWnd || !pWnd->GetSafeHwnd()) {
		_TRACE(_T("The window is not created!\n"));
		ASSERT(0);
		return FALSE;
	}

	return BindWindow(pWnd->GetSafeHwnd(), lpStyle);
}

BOOL CSkinWndBinder::BindWindow(HWND hWnd, LPCTSTR lpStyle)
{
	if (!::IsWindow(hWnd)) {
		ASSERT(0);
		return FALSE;
	}

	PELC_SKINWND_PARAMS pSkinParams = GetSkinParams(hWnd);
	if (pSkinParams) {
		if (!pSkinParams->pSkinWnd->LoadSkin(lpStyle)) {
			_TRACE(_T("Can not find the skin style : %s !\n"), lpStyle);
			return FALSE;
		}
	}
	else {
		pSkinParams = new ELC_SKINWND_PARAMS;
		pSkinParams->pSkinWnd = new CElcSkinFrameBase;

		if (!pSkinParams->pSkinWnd->LoadSkin(lpStyle)) {
			_TRACE(_T("Can not find the skin style : %s !\n"), lpStyle);
			delete pSkinParams->pSkinWnd;
			delete pSkinParams;
			return FALSE;
		}

		pSkinParams->pSkinWnd->Initialize(hWnd);
		pSkinParams->pOldWndProc = (WNDPROC)(LONG_PTR)::GetWindowLong(hWnd, GWL_WNDPROC);
		pSkinParams->pvUser = this;

		::SetWindowLong(hWnd, GWL_USERDATA, (LONG)(LONG_PTR)pSkinParams);
		::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(LONG_PTR)HookWndProc);

		m_mapBindingWnd.insert(CSkinWndParamMap::value_type(hWnd, pSkinParams));
	}

	// make sure we can get the WM_NCCALCSIZE message after hook windowprocess function
	CRect rect;
	::GetWindowRect(hWnd, rect);
	if (rect.Width() > 0 && rect.Height() > 0) {
		::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, 
			SWP_NOACTIVATE|SWP_NOZORDER|SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE);
	}

	return TRUE;
}

void CSkinWndBinder::UnBindWindow(HWND hWnd)
{
	CSkinWndParamMap::iterator it = m_mapBindingWnd.find(hWnd);
	if (it != m_mapBindingWnd.end()) {
		::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(LONG_PTR)it->second->pOldWndProc);
		delete it->second->pSkinWnd;
		delete it->second;
		m_mapBindingWnd.erase(it);
	}
}

PELC_SKINWND_PARAMS CSkinWndBinder::GetSkinParams(HWND hBindingWnd)
{
	CSkinWndParamMap::iterator it = m_mapBindingWnd.find(hBindingWnd);
	if (it == m_mapBindingWnd.end())
		return NULL;
	return it->second;
}

CElcSkinFrameBase* CSkinWndBinder::GetSkinFrame(HWND hBindingWnd)
{
	CSkinWndParamMap::iterator it = m_mapBindingWnd.find(hBindingWnd);
	if (it == m_mapBindingWnd.end())
		return NULL;
	return it->second->pSkinWnd;
}

LRESULT CALLBACK CSkinWndBinder::HookWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PELC_SKINWND_PARAMS pSkinParams = (PELC_SKINWND_PARAMS)(LONG_PTR)GetWindowLong(hWnd, GWL_USERDATA);
	CSkinWndBinder * pBinder = (CSkinWndBinder *)pSkinParams->pvUser;

	if (!pSkinParams->pSkinWnd->IsEnableSkin())
		return ::CallWindowProc(pSkinParams->pOldWndProc, hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		{
			WNDPROC pOldWndProc = pSkinParams->pOldWndProc;
			pBinder->UnBindWindow(hWnd);
			::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(LONG_PTR)pOldWndProc);
			return ::CallWindowProc(pOldWndProc, hWnd, msg, wParam, lParam);
		}
		break;
	case WM_NCCALCSIZE:
		{
			pSkinParams->pSkinWnd->NcCalcSize((BOOL)wParam, (NCCALCSIZE_PARAMS *)lParam);
		}
		break;
	case WM_NCPAINT:
		{
			pSkinParams->pSkinWnd->NcPaint();
			return 0;
		}
		break;
	case WM_ERASEBKGND:
		{
			CDC * pDC = CDC::FromHandle((HDC)wParam);
			pSkinParams->pSkinWnd->DrawClientBackground(pDC);
			return 1;
		}
		break;
	case WM_STYLECHANGED:
		{
			BOOL bRefresh = FALSE;
			DWORD dwRejectStyle = 0;
			if (GWL_STYLE == wParam) 
				dwRejectStyle = WS_CAPTION | WS_BORDER;
			else 
				dwRejectStyle = WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;
			
			LPSTYLESTRUCT pStyleStruct = (LPSTYLESTRUCT)lParam;
			if (pStyleStruct->styleNew & dwRejectStyle)
				::PostMessage(hWnd, WM_ELC_SKINFRAME_REINITIALIZE, 0, 0);
		}
		break;
	case WM_ELC_SKINFRAME_REINITIALIZE:
		{
			pSkinParams->pSkinWnd->Initialize(hWnd);
		}
		break;
	case WM_CTLCOLORBTN:
		{
			CDC * pDC = CDC::FromHandle((HDC)wParam);
			CWnd* pWnd = CWnd::FromHandle((HWND)lParam);
			return (LRESULT)pSkinParams->pSkinWnd->CtlColor(pDC, pWnd, CTLCOLOR_BTN);
		}
		break;
	case WM_CTLCOLORDLG:
		{
			CDC * pDC = CDC::FromHandle((HDC)wParam);
			CWnd* pWnd = CWnd::FromHandle((HWND)lParam);
			return (LRESULT)pSkinParams->pSkinWnd->CtlColor(pDC, pWnd, CTLCOLOR_DLG);
		}
		break;
	case WM_CTLCOLORSTATIC:
		{
			CDC * pDC = CDC::FromHandle((HDC)wParam);
			CWnd* pWnd = CWnd::FromHandle((HWND)lParam);
			return (LRESULT)pSkinParams->pSkinWnd->CtlColor(pDC, pWnd, CTLCOLOR_STATIC);
		}
		break;
	case WM_CTLCOLOREDIT:
		{
			//CDC * pDC = CDC::FromHandle((HDC)wParam);
			//CWnd* pWnd = CWnd::FromHandle((HWND)lParam);
			//return (LRESULT)pSkinParams->pSkinWnd->CtlColor(pDC, pWnd, CTLCOLOR_EDIT);
		}
		break;
	case WM_CTLCOLORLISTBOX:
		{
			//CDC * pDC = CDC::FromHandle((HDC)wParam);
			//CWnd* pWnd = CWnd::FromHandle((HWND)lParam);
			//return (LRESULT)pSkinParams->pSkinWnd->CtlColor(pDC, pWnd, CTLCOLOR_LISTBOX);
		}
		break;
	case WM_CTLCOLORSCROLLBAR:
		{
			//CDC * pDC = CDC::FromHandle((HDC)wParam);
			//CWnd* pWnd = CWnd::FromHandle((HWND)lParam);
			//return (LRESULT)pSkinParams->pSkinWnd->CtlColor(pDC, pWnd, CTLCOLOR_SCROLLBAR);
		}
		break;
	case WM_NCACTIVATE:
		{
			return pSkinParams->pSkinWnd->NcActivate((BOOL)wParam);
		}
		break;
	case WM_NCHITTEST:
		{
			return pSkinParams->pSkinWnd->NcHitTest(CPoint(LOWORD(lParam), HIWORD(lParam)));
		}
		break;
	case WM_NCLBUTTONDBLCLK:
		{
			if (pSkinParams->pSkinWnd->NcLButtonDblClk((UINT)wParam, CPoint(LOWORD(lParam), HIWORD(lParam))))
				return 0;
		}
		break;
	case WM_NCLBUTTONDOWN:
		{
			if (pSkinParams->pSkinWnd->NcLButtonDown((UINT)wParam, CPoint(LOWORD(lParam), HIWORD(lParam))))
				return 0;
		}
		break;
	case WM_NCLBUTTONUP:
		{
			pSkinParams->pSkinWnd->NcLButtonUp((UINT)wParam, CPoint(LOWORD(lParam), HIWORD(lParam)));
			return 0;
		}
		break;
	case WM_NCMOUSEMOVE:
		{
			pSkinParams->pSkinWnd->NcMouseMove((UINT)wParam, CPoint(LOWORD(lParam), HIWORD(lParam)));
		}
		break;
	case WM_NCRBUTTONDOWN:
		{
			pSkinParams->pSkinWnd->NcRButtonDown((UINT)wParam, CPoint(LOWORD(lParam), HIWORD(lParam)));
			return 0;
		}
		break;
	case WM_TIMER:
		{
			if (ELC_SKIN_FRAME_STATE_TIMER == wParam) {
				pSkinParams->pSkinWnd->CheckStates();
				return 0;
			}
		}
		break;
	case WM_ENTERIDLE:
		{
		}
		break;
	case WM_SETTEXT:
		{
			pSkinParams->pSkinWnd->RedrawCaption();
		}
		break;
	case WM_GETMINMAXINFO:
		{
			if ((GetWindowLong(hWnd, GWL_STYLE) & WS_MAXIMIZEBOX) != 0) {
				CRect rcWork;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
				MINMAXINFO * lpMMI = (MINMAXINFO *)lParam;
				lpMMI->ptMaxSize.y = rcWork.bottom;
			}
		}
		break;
	default:
		break;
	}

	return ::CallWindowProc(pSkinParams->pOldWndProc, hWnd, msg, wParam, lParam);
}
