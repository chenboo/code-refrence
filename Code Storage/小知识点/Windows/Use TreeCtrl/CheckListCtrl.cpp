// CheckListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Dialog.h"
#include "CheckListCtrl.h"


// CCheckListCtrl

IMPLEMENT_DYNAMIC(CCheckListCtrl, CWnd)

CCheckListCtrl::CCheckListCtrl()
{
	m_nMsg = 0;
	m_hWndNotify = NULL;
	m_clrText = RGB(0, 0, 0);
}

CCheckListCtrl::~CCheckListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCheckListCtrl, CWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCheckListCtrl message handlers

void CCheckListCtrl::SetNotifyHwnd(HWND hWndNotify, UINT nMsg)
{
	m_nMsg = nMsg;
	m_hWndNotify = hWndNotify;
}


void CCheckListCtrl::OnPaint()
{



}

void CCheckListCtrl::OnClose()
{


}

void CCheckListCtrl::OnDestroy()
{


}

BOOL CCheckListCtrl::OnEraseBkgnd(CDC* pDC)
{


}

void CCheckListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{


}

int CCheckListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{


}

void CCheckListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{



}

void CCheckListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{




}

void CCheckListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{


}

void CCheckListCtrl::OnSize(UINT nType, int cx, int cy)
{




}


CImageList* CCheckListCtrl::GetImageList()
{


}

CImageList* CCheckListCtrl::SetImageList(CImageList* pImageList)
{


}


int CCheckListCtrl::AddItem(LPCTSTR lpszItem, int nItemImage)
{



}

BOOL CCheckListCtrl::DeleteItem(int nItem)
{


}


BOOL CCheckListCtrl::SetItemData(int nItem, DWORD dwData)
{


}

DWORD CCheckListCtrl::GetItemData(int nItem)
{


}


BOOL CCheckListCtrl::SetCheck(int nItem, BOOL bCheck)
{


}

BOOL CCheckListCtrl::GetCheck(int nItem)
{
	m_ItemArray


}


void CCheckListCtrl::SetFont(CFont* pFont)
{
	if (pFont == NULL)
	{
		return;
	}

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	pFont->GetLogFont(&lf);
	m_ftText.DeleteObject();
	m_ftText.CreateFontIndirect(&lf);
}

void CCheckListCtrl::SetTextColor(COLORREF cr)
{
	m_clrText = cr;
}

COLORREF CCheckListCtrl::GetTextColor()
{
	return m_clrText;
}

void CCheckListCtrl::SetBkColor(COLORREF cr)
{
	m_clrBk = cr;
}

COLORREF CCheckListCtrl::GetBkColor()
{
	return m_clrBk;
}

int CCheckListCtrl::GetItemCount()
{
	return m_ItemArray.GetCount();
}