#include "stdafx.h"
#include "GraphLib.h"
#include "TextEditWnd.h"

IMPLEMENT_DYNAMIC(CTextEditWnd, CEdit)

CTextEditWnd::CTextEditWnd()
{
	InitFont();
	m_nMsg = 0;
	m_nReference = 0;
	m_nTextHeight = 0;
	m_hNotifyWnd = NULL;
	m_pText = NULL;
}

void CTextEditWnd::InitFont()
{
	LOGFONT lf = {0};
	lf.lfCharSet = DEFAULT_CHARSET;
	HDC hdc = ::GetDC(NULL);
	lf.lfHeight = -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	lf.lfWeight = FW_NORMAL;
	_tcscpy(lf.lfFaceName, _T("ËÎÌו"));
	m_Font.CreateFontIndirect(&lf);

	::ReleaseDC(NULL,hdc);
}

CTextEditWnd::~CTextEditWnd()
{
	m_DlgFont.DestroyWindow();
}

BEGIN_MESSAGE_MAP(CTextEditWnd, CEdit)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_FONTCHANGED,OnFontChanged)
END_MESSAGE_MAP()

int CTextEditWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if(!m_DlgFont.Create(CFontDlg::IDD, this))
	{
		return -1;
	}

	SetParent(NULL);
	SetFont(&m_Font);
	m_DlgFont.SetNotiFyWindow(GetSafeHwnd(), WM_FONTCHANGED);
	
	return 0;
}

void CTextEditWnd::SetNotifyWindow(HWND hWnd, UINT nMsg)
{
	m_hNotifyWnd = hWnd;
	m_nMsg = nMsg;
}

LRESULT CTextEditWnd::OnFontChanged(WPARAM wParam, LPARAM lParam)
{
	CFont* pFont = (CFont*)wParam;

	LOGFONT lf = {0};
	pFont->GetLogFont(&lf);

	if(m_Font.m_hObject)
	{
		m_Font.DeleteObject();
	}
	m_Font.CreateFontIndirect(&lf);
	SetFont(&m_Font);
	return 1;
}

void CTextEditWnd::OnDestroy()
{
	CEdit::OnDestroy();
}

void CTextEditWnd::ShowEditWindow(BOOL bShow)
{
	if(bShow)
	{
		CRect rcEdit;
		GetWindowRect(&rcEdit);
		CRect rcFont;
		m_DlgFont.GetWindowRect(&rcFont);

		CPoint ptCenter = rcEdit.CenterPoint();
		int nleft = ptCenter.x - rcFont.Width() / 2;
		int ntop = rcEdit.top - 50 - rcFont.Height();

		if(nleft <= 0)
		{
			nleft = 0;
		}

		if(ntop <= 0)
		{
			ntop = GetSystemMetrics(SM_CYSCREEN) / 2;
		}
		
		m_DlgFont.SetWindowPos(&CWnd::wndTopMost, 
			nleft,
			ntop,
			0,
			0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

		ShowWindow(SW_SHOW);
		m_DlgFont.ShowWindow(SW_SHOW);
		SetFocus();
	}
	else
	{
		ResetEditInfo();
		ShowWindow(SW_HIDE);
		SetWindowText(_T(""));
		m_DlgFont.ResetFontInfo();
		m_DlgFont.ShowWindow(SW_HIDE);
	}
}

BOOL CTextEditWnd::OnEraseBkgnd(CDC* pDC)
{
	return CEdit::OnEraseBkgnd(pDC);
}

void CTextEditWnd::MoveEditBox(CRect rect)
{
	m_rcEdit = rect;
	MoveWindow(&rect);
}

void CTextEditWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ChangeEditSize();
	Invalidate();

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CTextEditWnd::ChangeEditSize()
{
	LOGFONT lf;
	m_Font.GetLogFont(&lf);
	m_nTextHeight = -lf.lfHeight;

	int nLineCount = GetLineCount();
	int nHeight = nLineCount * m_nTextHeight;

	if(m_rcEdit.Height() - nHeight < m_nTextHeight)
	{
		m_rcEdit.InflateRect(0,0,0,m_nTextHeight);
		MoveWindow(&m_rcEdit);
		ASSERT(m_pText);
		m_pText->UpdateTextBorder(m_rcEdit);
	}
}

BOOL CTextEditWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	return TRUE;
}

CSize CTextEditWnd::GetTextSize()
{
	int nlen = 0;
	CString strMaxWidth;
	int nLineCount = GetLineCount();

	//get the largest line int the edit box
	for(int i = 0; i < nLineCount; i++)
	{
		int ncx = LineLength(LineIndex(i));
		if(nlen < ncx)
		{
			nlen = ncx;
			GetLine(i, strMaxWidth.GetBuffer(nlen), nlen);
			strMaxWidth.ReleaseBuffer(nlen);
		}
	}

	LOGFONT lf;
	m_Font.GetLogFont(&lf);

	CDC* pDC = GetDC();
	pDC->SelectObject(&m_Font);

	Graphics graph(pDC->m_hDC);
	Font font(pDC->m_hDC,&lf);
	RectF boundRect;
	RectF layoutRect(0, 0, 
		(float)GetSystemMetrics(SM_CXSCREEN),
		(float)GetSystemMetrics(SM_CYSCREEN));
	
	// Measure the string.
	graph.MeasureString(strMaxWidth,-1,&font,layoutRect, &boundRect);

	int nWidth = (int)boundRect.Width;
	int nHeight =  (int)boundRect.Height * nLineCount;

	ReleaseDC(pDC);
	return CSize(nWidth, nHeight);
}

void CTextEditWnd::SetTextFont(CFont& font)
{
	LOGFONT lf = {0};
	font.GetLogFont(&lf);

	if(m_Font.m_hObject)
	{
		m_Font.DeleteObject();
	}

	m_Font.CreateFontIndirect(&lf);
	SetFont(&m_Font);
	m_DlgFont.SetTextFont(font);
}

void CTextEditWnd::ResetEditInfo()
{
	LOGFONT lf = {0};
	lf.lfCharSet = DEFAULT_CHARSET;
	HDC hdc = ::GetDC(NULL);
	lf.lfHeight = -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	lf.lfWeight = FW_NORMAL;
	_tcscpy(lf.lfFaceName,_T("ËÎÌו"));

	if(m_Font.m_hObject)
	{
		m_Font.DeleteObject();
	}

	m_Font.CreateFontIndirect(&lf);
	SetFont(&m_Font);
}

void CTextEditWnd::InCreaseReference()
{
	m_nReference++;
}

void CTextEditWnd::DeCreaseReference()
{
	m_nReference--;
}

BOOL CTextEditWnd::IsNoreference()
{
	if(m_nReference == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CTextEditWnd::SetParentWnd(CWnd* pWndParent)
{
	VERIFY(pWndParent);
	SetParent(pWndParent);
	m_DlgFont.SetParent(pWndParent);
}

void CTextEditWnd::SetTextGraph(CText* pText)
{
	ASSERT(pText);
	m_pText = pText;
}
