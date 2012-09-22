#include "StdAfx.h"
#include "LibElcBaseUI.h"
#include "NCSkinBinding.h"


CNCSkinBinding::CNCSkinBinding(void)
{
}

CNCSkinBinding::~CNCSkinBinding(void)
{
}

BOOL CNCSkinBinding::BindWindow(CWnd* pWnd, LPCTSTR lpStyle, BOOL bFillBackground)
{
	return BindWindow(pWnd->GetSafeHwnd(), lpStyle, bFillBackground);
}

BOOL CNCSkinBinding::BindWindow(HWND hWnd, LPCTSTR lpStyle, BOOL bFillBackground)
{
	if (!::IsWindow(hWnd))
		return FALSE;

	if (!theSkin.IsValid())
		return FALSE;

	PELC_SKIN_PNAE_NC pSkin = theSkin.GetNCPaneSkin(lpStyle);
	if (!pSkin)
		return FALSE;

	PELC_NC_SKIN_PARAMS pSkinParams = GetSkinParams(hWnd);
	if (pSkinParams) {
		if (_tcsicmp(pSkinParams->pSkin->szStyle, pSkin->szStyle) == 0)
			return TRUE;

		pSkinParams->pSkin = pSkin;
		if (bFillBackground) {
			if (pSkinParams->hBrBackgnd)
				::DeleteObject(pSkinParams->hBrBackgnd);
			pSkinParams->hBrBackgnd = ::CreateSolidBrush(pSkin->crClient);
		}
		::RedrawWindow(hWnd, NULL, NULL, RDW_FRAME);
	}
	else {
		pSkinParams = new ELC_NC_SKIN_PARAMS;

		pSkinParams->pOldWndProc = (WNDPROC)(LONG_PTR)::GetWindowLong(hWnd, GWL_WNDPROC);
		pSkinParams->pSkin = pSkin;
		pSkinParams->bFillBackground = bFillBackground;

		if (bFillBackground)
			pSkinParams->hBrBackgnd = ::CreateSolidBrush(pSkin->crClient);

		m_mapBindingWnd.insert(CSkinBindingWndMap::value_type(hWnd, pSkinParams));

		::SetWindowLong(hWnd, GWL_USERDATA, (LONG)(LONG_PTR)this);
		::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(LONG_PTR)HookWndProc);
	}

	CRect rect;
	::GetWindowRect(hWnd, rect);
	if (rect.Width() > 0 && rect.Height() > 0) {
		// make sure we can get the WM_NCCALCSIZE message after hook windowprocess function
		::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOZORDER|SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE);
	}

	return TRUE;
}

void CNCSkinBinding::UnBindWindow(HWND hWnd)
{
	CSkinBindingWndMap::iterator it = m_mapBindingWnd.find(hWnd);
	if (it != m_mapBindingWnd.end()) {
		::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(LONG_PTR)it->second->pOldWndProc);
		if (it->second->hBrBackgnd)
			::DeleteObject(it->second->hBrBackgnd);
		delete it->second;
		m_mapBindingWnd.erase(it);
	}
}

PELC_NC_SKIN_PARAMS CNCSkinBinding::GetSkinParams(HWND hBindingWnd)
{
	CSkinBindingWndMap::iterator it = m_mapBindingWnd.find(hBindingWnd);
	if (it == m_mapBindingWnd.end())
		return NULL;
	return it->second;
}

LRESULT CALLBACK CNCSkinBinding::HookWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CNCSkinBinding *pSkin = (CNCSkinBinding*)(LONG_PTR)GetWindowLong(hWnd, GWL_USERDATA);
	PELC_NC_SKIN_PARAMS pSkinParams = pSkin->GetSkinParams(hWnd);
	Image * pImage = (Image *)pSkinParams->pSkin->ncimage.pImage;
	int cx = pImage->GetWidth();
	int cy = pImage->GetHeight();

	int nLeftW = pSkinParams->pSkin->ncimage.nLeftSplit;
	int nRightW = cx - pSkinParams->pSkin->ncimage.nRightSplit;
	int nTopW = pSkinParams->pSkin->ncimage.nTopSplit;
	int nBottomW = cy - pSkinParams->pSkin->ncimage.nBottomSplit;

	switch (msg)
	{
	case WM_DESTROY:
		{
			WNDPROC pOldWndProc = pSkinParams->pOldWndProc;
			pSkin->UnBindWindow(hWnd);
			return ::CallWindowProc(pOldWndProc, hWnd, msg, wParam, lParam);
		}
		break;
	case WM_NCCALCSIZE:
		{
			if (wParam) {
				NCCALCSIZE_PARAMS * lpncsp = (NCCALCSIZE_PARAMS *)lParam;
				lpncsp->rgrc[0].left += nLeftW;
				lpncsp->rgrc[0].right -= nRightW;
				lpncsp->rgrc[0].top += nTopW;
				lpncsp->rgrc[0].bottom -= nBottomW;
			}
		}
		break;
	case WM_NCPAINT:
		{
			LRESULT lRet = ::CallWindowProc(pSkinParams->pOldWndProc, hWnd, msg, wParam, lParam);

			CRect rc;
			::GetWindowRect(hWnd, &rc); 

			Rect rect(0, 0, rc.Width(), rc.Height());
			Bitmap bitmap(rc.Width(), rc.Height());
			Graphics grph(&bitmap);

			HDC hDC = ::GetWindowDC(hWnd);
			DWORD dwLayout = ::GetLayout(hDC);
			if (dwLayout == LAYOUT_RTL) {
				::SetLayout(hDC, LAYOUT_LTR);
				rect.X = rc.Width() - (rect.X + rect.Width);
			}

			thePainter.DrawImage(&grph, 
				&pSkinParams->pSkin->ncimage,
				&rect,
				pSkinParams->pSkin->crBackgnd, 
				0,
				0,
				0);

			Graphics g(hDC);
			g.DrawImage(&bitmap, 0, 0);

			if (dwLayout == LAYOUT_RTL) 
				::SetLayout(hDC, dwLayout);

			::ReleaseDC(hWnd, hDC);

			return lRet;
		}
		break;
	case WM_ERASEBKGND:
		{
			LRESULT lRet = ::CallWindowProc(pSkinParams->pOldWndProc, hWnd, msg, wParam, lParam);
			if (lRet == 0) {
				if (pSkinParams->bFillBackground) {
					RECT rc;
					::GetClientRect(hWnd, &rc);
					HBRUSH hbr = CreateSolidBrush(pSkinParams->pSkin->crClient);
					::FillRect((HDC)wParam, &rc, hbr);
					::DeleteObject(hbr);
					return 1;
				}
			}
			return lRet;
		}
		break;
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC:
		{
			if (pSkinParams->bFillBackground)
				return (LRESULT)pSkinParams->hBrBackgnd;
		}
		break;
	default:
		break;
	}

	return ::CallWindowProc(pSkinParams->pOldWndProc, hWnd, msg, wParam, lParam);
}
