// TestPopupWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TestBaseUI.h"
#include "TestPopupWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



// CTestPopupWnd

IMPLEMENT_DYNAMIC(CTestPopupWnd, CWnd)

CTestPopupWnd::CTestPopupWnd()
{

}

CTestPopupWnd::~CTestPopupWnd()
{
}


BEGIN_MESSAGE_MAP(CTestPopupWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()

	ON_COMMAND(100, OnCustomButton1)

	ON_MESSAGE(WM_ELC_UI_DRAW_NOTIFY, OnElcUIDrawNotify)
END_MESSAGE_MAP()



// CTestPopupWnd message handlers



void CTestPopupWnd::OnPaint()
{
	CPaintDC dc(this); 

	CRect rect;
	GetClientRect(rect);
	rect.bottom = rect.bottom / 2;

	CElcMemDC dcMem(&dc, rect);

	dcMem.FillSolidRect(rect, RGB(220,240,255));

	m_ImagePaintMgr.SetImagePos(2, CPoint(rect.Width() / 2, 20));
	m_ImagePaintMgr.Draw(&dcMem, rect);

	dcMem.DrawMe();
}

LRESULT CTestPopupWnd::OnElcUIDrawNotify(WPARAM wParam, LPARAM lParam)
{
	PELC_UI_DRAW pDraw = (PELC_UI_DRAW)lParam;
	if (pDraw) {
		CString strText = _T("this is a test string.");
		CRect rect = pDraw->rect;
		::DrawText(pDraw->hdc, strText, strText.GetLength(), &rect, DT_CENTER | DT_SINGLELINE);
		return 1;
	}
	return 0;
}

int CTestPopupWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0, WS_CLIPCHILDREN);

	//ModifyStyleEx(0, WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 123, LWA_ALPHA);

	DWORD dwStyle = WS_CHILD
		| WS_VISIBLE
		| LVS_REPORT 
		| LVS_SHAREIMAGELISTS
		| LVS_SHOWSELALWAYS
		| LVS_OWNERDRAWFIXED
		| LVS_EDITLABELS;
			  
	DWORD dwExStyle = LVS_EX_INFOTIP | 
				LVS_EX_FULLROWSELECT |  
				LVS_EX_DOUBLEBUFFER | 
				LVS_EX_HEADERDRAGDROP |
				LVS_EX_LABELTIP;

	m_wndList.Create(
		dwStyle,
		CRect(0,0,0,0),
		this,
		0);

	m_wndList.SetExtendedStyle(dwExStyle);

	m_wndList.InsertColumn(0, _T("Column 1"), 0, 100);
	m_wndList.InsertColumn(1, _T("Column 2"), 0, 100);
	m_wndList.InsertColumn(2, _T("Column 3"), 0, 100);

	for (int i=0; i < 15; i ++) {
		CString strText;
		strText.Format(_T("item %d"), i);
		int nItem = m_wndList.InsertItem(m_wndList.GetItemCount(), strText, 0);

		strText.Format(_T("File name 1; \nFile name 2; \nFile name 3; \n"), i);
		m_wndList.SetItemText(nItem, 1, strText);
	}

	m_wndList.SetItemBold(1, TRUE);
	m_wndList.SetItemTextColor(2, RGB(200,100,150));

	CBitmap bmpBase;
	m_imgList.Create(16, 16, ILC_MASK | ILC_COLOR24, 0, 1);
	bmpBase.Attach(::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_TEST), 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
	m_imgList.Add(&bmpBase, RGB(255,0,255));

	m_wndList.SetImageList(&m_imgList, LVSIL_STATE);

	m_wndList.AddImage(0, 0, 100, 1, ELCRLIT_IMAGELIST, _T("Image List \nIcon"));
	m_wndList.AddImage(2, 0, 100, 2, ELCRLIT_IMAGELIST, _T("Image List Icon"));

	m_wndList.SetImageData(0, 0, 100, 1);
	m_wndList.SetImageData(2, 0, 100, 2);

	m_bitmap.LoadBitmap(IDB_BITMAP6);
	m_wndList.AddImage(3, 0, 100, (DWORD_PTR)m_bitmap.GetSafeHandle(), ELCRLIT_BITMAP_HANDLE, _T("Bitmap Icon"));
	m_wndList.SetImageData(3, 0, 100, 3);

	m_ImagePaintMgr.LoadSkin(_T("style1"));

	m_ImagePaintMgr.AddImage(1, IDR_RT_PNG1);
	m_ImagePaintMgr.AddImage(2, IDR_RT_PNG1);
	
	m_ImagePaintMgr.SetImagePos(1, CRect(10, 10, 100, 100));
	m_ImagePaintMgr.SetImagePos(2, CPoint(150, 20));

	POINT ptTopLeft = {30, 5};

	//AddCustomButton(100, IDR_RT_PNG2, _T("RT_PNG")); 

	m_wndList.ModifyStyle(0, WS_BORDER);
	//m_wndList.ModifyStyle(0, WS_THICKFRAME);
	//GetNCSkinBinder()->BindWindow(m_wndList.GetSafeHwnd(), ELC_SKIN_STYLE_PANE_CHILD);
	//GetSkinWndBinder()->BindWindow(m_wndList.GetSafeHwnd(), ELC_SKIN_STYLE_WINDOW_NCSKIN_WND);
	GetSkinWndBinder()->BindWindow(this, ELC_SKIN_STYLE_WINDOW_FRAME);

	return 0;
}

void CTestPopupWnd::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (m_wndList.GetSafeHwnd()) {
		m_wndList.MoveWindow(0, cy/2, cx, cy/2);
	}
}

BOOL CTestPopupWnd::OnEraseBkgnd(CDC* pDC)
{

	return TRUE;
}

void CTestPopupWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd == &m_wndList)
	{
		int nItem = m_wndList.GetNextItem(-1, LVNI_SELECTED);
		if (nItem == -1)
			return;

		ELC_REPORTLIST_HITTESTINFO info;
		m_wndList.GetRLHitTest(point, &info);
		
		CString strText;
		strText.Format(_T("nItem=%d, nSubItem=%d, nImageIndex=%d, nImageId=%d, dwData=%d"), info.nItem, info.nSubItem, info.nImageIndex, info.nImageId, info.dwData);
		AfxMessageBox(strText);
	}
}

void CTestPopupWnd::OnCustomButton1()
{
	AfxMessageBox(_T("Button 1"));
}
