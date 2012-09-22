#include "stdafx.h"
#include "GraphLib.h"
#include "FontDlg.h"

IMPLEMENT_DYNAMIC(CFontDlg, CDialog)

CFontDlg::CFontDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontDlg::IDD, pParent)
{
	m_hNotifyWnd = NULL;
	m_nMsg = 0;
	m_nFontSel = 0;
	m_bTracking = FALSE;

	m_bBold = FALSE;
	m_bItalic = FALSE;
	m_bUnderline = FALSE;
	InitFont();
}

void CFontDlg::InitFont()
{
	LOGFONT lf = {0};
	HDC hdc = ::GetDC(NULL);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	lf.lfWeight = FW_NORMAL;
	wcscpy_s(lf.lfFaceName,_T("ËÎÌå"));

	m_Font.CreateFontIndirect(&lf); 
	::ReleaseDC(NULL,hdc);
}

CFontDlg::~CFontDlg()
{
}

void CFontDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FONTNAME, m_ComFtname);
	DDX_Control(pDX, IDC_COMBO_FONTSIZE, m_ComFtsize);
}

BEGIN_MESSAGE_MAP(CFontDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_BTNCLICK, OnBtnclicked)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_CBN_SELCHANGE(IDC_COMBO_FONTNAME, &CFontDlg::OnCbnSelchangeComboFontname)
	ON_CBN_SELCHANGE(IDC_COMBO_FONTSIZE, &CFontDlg::OnCbnSelchangeComboFontsize)
END_MESSAGE_MAP()

BOOL CFontDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitComFtname();
	InitComFtsize();
	InitButton();

	m_ComFtname.SetCurSel(m_nFontSel);
	m_ComFtsize.SetCurSel(10);

	return TRUE;
}

void CFontDlg::InitButton()
{
	int nBtnID[3] = {IDC_BTN_BOLD, IDC_BTN_ITALIC, IDC_BTN_UNDERLINE};
	int nID[3] = {IDR_PNG_BOLD, IDR_PNG_ITALIC, IDR_PNG_UNDERLINE};
	int nstyle[] = {BT_TOGGLE, BT_TOGGLE, BT_TOGGLE};

	CRect rcUnit;
	rcUnit.left = 245;
	rcUnit.top = 1;
	rcUnit.right = rcUnit.left + 24;
	rcUnit.bottom = rcUnit.top + 24;

	CRect rc[3];
	for(int i = 0; i < 3; i++)
	{
		rc[i] = rcUnit;
		rcUnit.OffsetRect(24, 0);
	}

	m_btnManager.CreateButtonManager(nID, nBtnID, nstyle, rc, 3, m_hWnd);
}

void CFontDlg::SetNotiFyWindow(HWND hWnd, UINT nMsg)
{
	m_nMsg = nMsg;
	m_hNotifyWnd = hWnd;
}

void CFontDlg::InitComFtname()
{
	InstalledFontCollection ifc;
	int n = ifc.GetFamilyCount();
	FontFamily* ffs = new FontFamily[n];
	int nfound;
	ifc.GetFamilies(n, ffs, &nfound);
	
	wchar_t name[LF_FACESIZE];

	//add the fontname to the combox
	for (int i = 0; i < nfound; i++) 
	{
		ffs[i].GetFamilyName(name);
		m_ComFtname.AddString(name);
	}

	//sel the special font
	CString strCurFont;
	int count = m_ComFtname.GetCount();
	for(int i = 0; i < count; i++)
	{
		m_ComFtname.GetLBText(i,strCurFont);
		if(!strCurFont.Collate(_T("ËÎÌå")))
		{
			m_nFontSel = i;
			return;
		}
	}
}

void CFontDlg::InitComFtsize()
{
	UINT nFontSize[] = {8, 9, 10 ,11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};

	m_ComFtsize.ResetContent();

	CString strText;
	for (int i=0; i < _countof(nFontSize); i ++)
	{
		strText.Format(_T("%d"), nFontSize[i]);
		int nItem = m_ComFtsize.AddString(strText);
		m_ComFtsize.SetItemData(nItem, nFontSize[i]);
	}
}

void CFontDlg::DrawButons()
{
	CDC* pDC = GetDC();
	CRect rect;
	GetClientRect(&rect);
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());

	CBitmap* pOldbmp = dcMem.SelectObject(&bmp);
	dcMem.SetBkMode(TRANSPARENT);
	dcMem.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0,0,SRCCOPY);

	m_btnManager.DrawButtons(&dcMem);

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(pOldbmp);
	bmp.DeleteObject();
	dcMem.DeleteDC();

	ReleaseDC(pDC);
}

void CFontDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DrawButons();
}

void CFontDlg::OnBnClickedBtnBold()
{
	m_bBold = !m_bBold;
	ModifyFont(FDNT_BOLD);
}

void CFontDlg::OnBnClickedBtnItalic()
{
	m_bItalic = !m_bItalic;
	ModifyFont(FDNT_ITALIC);
}

void CFontDlg::OnBnClickedBtnUnderline()
{
	m_bUnderline = !m_bUnderline;
	ModifyFont(FDNT_UNDERLINE);
}

void CFontDlg::OnCbnSelchangeComboFontname()
{
	ModifyFont(FDNT_FONTNAME);
}

void CFontDlg::OnCbnSelchangeComboFontsize()
{
	ModifyFont(FDNT_FONTSIZE);
}

void CFontDlg::ModifyFont(int nType)
{
	LOGFONT lf = {0};
	m_Font.GetLogFont(&lf);

	if (FDNT_BOLD == nType) 
	{
		lf.lfWeight = m_bBold ? FW_BOLD : FW_NORMAL;
	}
	else if (FDNT_UNDERLINE == nType) 
	{
		lf.lfUnderline = m_bUnderline;
	}
	else if (FDNT_ITALIC == nType) 
	{
		lf.lfItalic = m_bItalic;
	}
	else if (FDNT_FONTNAME == nType) 
	{
		m_ComFtname.GetLBText(m_ComFtname.GetCurSel(), lf.lfFaceName);
	}
	else if (FDNT_FONTSIZE == nType) 
	{
		CDC * pDC = GetDC();
		DWORD dwData = (DWORD)m_ComFtsize.GetItemData(m_ComFtsize.GetCurSel());
		lf.lfHeight = -MulDiv(dwData, GetDeviceCaps(pDC->m_hDC, LOGPIXELSY), 72);
		ReleaseDC(pDC);
	}
	else 
	{
		ASSERT(0);
	}

	if(m_Font.m_hObject)
	{
		m_Font.DeleteObject();
	}
	m_Font.CreateFontIndirect(&lf);


	::PostMessage(m_hNotifyWnd, m_nMsg,(WPARAM)&m_Font, NULL);
}

void CFontDlg::SetTextFont(CFont& font)
{
	LOGFONT lf = {0};
	font.GetLogFont(&lf);

	//set the fontname combo
	CString str;
	int nCount = m_ComFtname.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		m_ComFtname.GetLBText(i,str);

		if(str.Collate(lf.lfFaceName) == 0)
		{
			m_ComFtname.SetCurSel(i);
			break;
		}
	}

	//set the fontsize combo
	CDC * pDC = GetDC();
	nCount = m_ComFtsize.GetCount();
	DWORD dwData = -MulDiv(lf.lfHeight, 72,GetDeviceCaps(pDC->m_hDC, LOGPIXELSY));

	for(int i = 0; i < nCount; i++)
	{
		if(dwData == m_ComFtsize.GetItemData(i))
		{
			m_ComFtsize.SetCurSel(i);
			break;
		}
	}

	//set the bold¡¢Italic¡¢Underline state
	m_bItalic = lf.lfItalic;
	m_bUnderline = lf.lfUnderline;
	m_bBold = lf.lfWeight == FW_BOLD ? TRUE : FALSE;
	
	m_btnManager.SetButtonToggle(IDC_BTN_BOLD, m_bBold);
	m_btnManager.SetButtonToggle(IDC_BTN_ITALIC, m_bItalic);
	m_btnManager.SetButtonToggle(IDC_BTN_UNDERLINE, m_bUnderline);

	if(m_Font.GetSafeHandle())
	{
		m_Font.DeleteObject();
		m_Font.CreateFontIndirect(&lf);
	}

	ReleaseDC(pDC);
}

void CFontDlg::ResetFontInfo()
{
	LOGFONT lf = {0};
	lf.lfCharSet = DEFAULT_CHARSET;
	HDC hdc = ::GetDC(NULL);
	lf.lfHeight = -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	lf.lfWeight = FW_NORMAL;
	_tcscpy(lf.lfFaceName,_T("ËÎÌå"));

	if(m_Font.m_hObject)
	{
		m_Font.DeleteObject();
	}

	m_Font.CreateFontIndirect(&lf);
	::ReleaseDC(NULL,hdc);

	m_ComFtsize.SetCurSel(10);
	m_ComFtname.SetCurSel(m_nFontSel);

	m_bBold = FALSE;
	m_bItalic = FALSE;
	m_bUnderline = FALSE;

	m_btnManager.SetButtonToggle(IDC_BTN_BOLD, m_bBold);
	m_btnManager.SetButtonToggle(IDC_BTN_ITALIC, m_bItalic);
	m_btnManager.SetButtonToggle(IDC_BTN_UNDERLINE, m_bUnderline);
}

void CFontDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_btnManager.MouseState(MU_LBTNUP, point);

	CDialog::OnLButtonUp(nFlags, point);
}

void CFontDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	UINT nMouseSate = MU_NONE;
	if(MK_LBUTTON == nFlags)
	{
		nMouseSate = MU_LBTNMOUSE;
	}
	else
	{
		nMouseSate = MU_MOVE;
	}

	m_btnManager.MouseState(nMouseSate, point);

	CDialog::OnMouseMove(nFlags, point);
}

void CFontDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_btnManager.MouseState(MU_LBTNDOWN, point);

	CDialog::OnLButtonDown(nFlags, point);
}

void CFontDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_btnManager.MouseState(MU_LDCLK, point);

	CDialog::OnLButtonDblClk(nFlags, point);
}

LRESULT CFontDlg::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = FALSE;
	m_btnManager.MouseState(MU_MOUSELEAVE, CPoint(-10, -10));
	return 0;
}

LRESULT CFontDlg::OnBtnclicked(WPARAM wParam, LPARAM lParam)
{
	if(lParam)
	{
		if(IDC_BTN_BOLD == wParam)
		{
			OnBnClickedBtnBold();
		}
		else if(IDC_BTN_ITALIC == wParam)
		{
			OnBnClickedBtnItalic();
		}
		else if(IDC_BTN_UNDERLINE == wParam)
		{
			OnBnClickedBtnUnderline();
		}
	}
	else
	{
		m_btnManager.RestoreActiveRadio();
	}

	return 0;
}
