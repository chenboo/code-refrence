#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "ElcFloatBar.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SNAP_SIZE							6
#define ELCBAR_FOLD_INCREASE				2
#define ELCBAR_DROPDOWN_MARGIN				2
#define ELCBAR_TEXT_SIZE_REVISE				10
#define ELCBAR_TITLEITEM_MARGIN				5
#define ELCBAR_BUTTON_BK_MARGIN				2

#define	ID_ELC_FLOATBAR_ITEM_FLOD			100
#define	ID_ELC_FLOATBAR_ITEM_PIN			101


IMPLEMENT_DYNAMIC(CElcFloatBar, CWnd)

CElcFloatBar::CElcFloatBar()
{
	m_nHMargin = 10;
	m_nVMargin = 2;
	m_nButtonMargin = 0;
	m_pCurItem = 0;
	m_hNotifyWnd = NULL;
	m_nAutoHideCount = 0;
	m_bShowButtonText = FALSE;
	m_bMirror = FALSE;

	m_nTipHeight = 40;
	m_pTitleIcon = 0;
	m_nMaxItemHeight = 0;
	m_skin.nPeekHeight = 4;

	m_bEnableAutoHide = TRUE;
	m_bFoldBar = FALSE;
	m_dwFoldState = FOLD_STATE_EXPAND;

	//GetDefaultFont(m_lfTitle);
	GetMSShellDlgFont(m_lfTitle);

	memcpy(&m_lfText, &m_lfTitle, sizeof(m_lfText));

	memcpy(&m_lfLabel, &m_lfTitle, sizeof(m_lfTitle));
	m_lfLabel.lfHeight = 24;

	memcpy(&m_lfTip, &m_lfTitle, sizeof(m_lfTitle));
//	m_lfTip.lfHeight = 14;
//	m_lfTip.lfWeight = 700;

	LoadSkin(ELC_SKIN_STYLE_FLOATBAR_MAIN);
}

CElcFloatBar::~CElcFloatBar()
{
	DelAllItem();

	for (CElcFloatBarItem::iterator it = m_arrInnerItem.begin();
		it != m_arrInnerItem.end();
		it ++)
	{
		delete ((PELC_FLOATBAR_NWBUTTON)((*it)->pvCtrl));
		delete (PELC_FLOATBAR_ITEM)(*it);
	}
}

BOOL CElcFloatBar::LoadSkin(LPCTSTR lpszStyle)
{
	PELC_SKIN_FLOATBAR pSkin = theSkin.GetFloatBarSkin(lpszStyle);
	if (!pSkin)
		return FALSE;

	memcpy(&m_skin, pSkin, sizeof(m_skin));

	PELC_FLOATBAR_ITEM pItem = CreateFloatbarItem(ID_ELC_FLOATBAR_ITEM_FLOD, &m_skin.fold);
	m_arrInnerItem.push_back(pItem);

	pItem = CreateFloatbarItem(ID_ELC_FLOATBAR_ITEM_PIN, &m_skin.pin);
	m_arrInnerItem.push_back(pItem);

	m_sizeDropImage = CElcSkinDoc::GetImageSize(&m_skin.dropdown);

	return TRUE;
}

PELC_FLOATBAR_ITEM CElcFloatBar::CreateFloatbarItem(UINT nId, PELC_SKIN_NOTWND_BUTTON pSkinNMButton)
{
	PELC_FLOATBAR_ITEM pItem = new ELC_FLOATBAR_ITEM;
	memset(pItem, 0, sizeof(ELC_FLOATBAR_ITEM));

	pItem->type = ELCFIT_NWBUTTON;
	pItem->nId = nId;
	pItem->bShow = TRUE;
	pItem->rcItem.X = 0;
	pItem->rcItem.Y = 0;
	pItem->rcItem.Width = pSkinNMButton->pos.size.cx;
	pItem->rcItem.Height = pSkinNMButton->pos.size.cy;

	PELC_FLOATBAR_NWBUTTON pButton = new ELC_FLOATBAR_NWBUTTON;
	pButton->nState = ELCBS_NORMAL;
	pButton->bChecked = 0;
	memcpy(&pButton->button, pSkinNMButton, sizeof(ELC_SKIN_NOTWND_BUTTON));

	pItem->pvCtrl = (PVOID)pButton;

	return pItem;
}

BEGIN_MESSAGE_MAP(CElcFloatBar, CWnd)
	ON_WM_CREATE()
	ON_WM_NCHITTEST()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_SETCURSOR()
	ON_WM_SHOWWINDOW()

	ON_COMMAND(ID_ELC_FLOATBAR_ITEM_FLOD, OnFlodBar)
	ON_COMMAND(ID_ELC_FLOATBAR_ITEM_PIN, OnPinBar)
END_MESSAGE_MAP()


BOOL CElcFloatBar::CreateBar(CWnd* pParent)
{
	LPCTSTR pClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS);
	DWORD dwStyle = WS_POPUP;
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED;
	CRect rc(0,0,0,0);

	if (CreateEx(dwExStyle, pClassName, _T(""), dwStyle, rc, this,0)) {

		LONG lStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
		lStyle &= ~WS_EX_LAYOUTRTL;
		SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyle);

		if (!m_hNotifyWnd && pParent)
			m_hNotifyWnd = pParent->GetSafeHwnd();
		SetTimer(ID_TIMER_CHECK_STATE, 40, NULL);
		return TRUE;
	}

	return FALSE;
}

void CElcFloatBar::ShowBar(BOOL bShow)
{
	ShowBarDirect(bShow);

	if (bShow) {
		m_sizeBar = CalcItemsPos();
		int nScreenCx = GetSystemMetrics(SM_CXFULLSCREEN);
		int x = (nScreenCx - m_sizeBar.cx) / 2;
		SetWindowPos(NULL, x, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
		m_dwFoldState = FOLD_STATE_EXPAND;
		Repaint();
	}
	else {
		ShowWindow(SW_HIDE);
	}
}

int CElcFloatBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_bMirror = IsLayoutRTL();

	ModifyStyle(WS_CAPTION|WS_BORDER|WS_DLGFRAME|WS_THICKFRAME, 0);

	CalcItemsPos();

	Repaint();

	return 0;
}

void CElcFloatBar::OnClose()
{
}

void CElcFloatBar::Repaint()
{
	HDC hdc = ::GetDC(m_hWnd); 
	if (!hdc)
		return;

	GetWindowRect(m_rcWindow);

	BITMAPINFO bmInfo;
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biWidth = m_rcWindow.Width();
	bmInfo.bmiHeader.biHeight = m_rcWindow.Height();
	void* pBits = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(hdc, &bmInfo, DIB_RGB_COLORS, &pBits, NULL, 0);

	HDC hdcMem = ::CreateCompatibleDC(hdc);
	HGDIOBJ hOldBitmap = ::SelectObject(hdcMem, hBitmap);

	Graphics grph(hdcMem);
	DrawBar(&grph);
	//DrawItems(&grph);
	//DrawTip(&grph);

	BLENDFUNCTION blendFunc;
	blendFunc.AlphaFormat = AC_SRC_ALPHA;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = 255;
	blendFunc.BlendOp = AC_SRC_OVER;

	::UpdateLayeredWindow(m_hWnd,
		hdc,
		&m_rcWindow.TopLeft(),
		&m_rcWindow.Size(),
		hdcMem,
		&CPoint(0,0),
		RGB(0,0,0),
		&blendFunc,
		ULW_ALPHA);

	::SelectObject(hdcMem, hOldBitmap);
	::DeleteObject(hBitmap);
	::DeleteDC(hdcMem);
	::ReleaseDC(m_hWnd, hdc);
}

void CElcFloatBar::DrawBar(Graphics* pGraphics)
{
	Rect rcDest;
	rcDest.X = 0;
	rcDest.Y = 0;
	rcDest.Width = m_sizeBar.cx;
	rcDest.Height = m_sizeBar.cy;

	thePainter.DrawImage(
		pGraphics,
		&m_skin.background,
		&rcDest,
		0,
		1,
		0);

	if (m_pTitleIcon && m_skin.nTitleHeight > 0) {
		CPoint pt;
		pt.x = ELCBAR_TITLEITEM_MARGIN;
		pt.y = (m_skin.nTitleHeight - m_pTitleIcon->GetHeight()) / 2;
		thePainter.DrawFixedImage(pGraphics,
			m_pTitleIcon,
			pt,
			NULL,
			0);
	}

	if (!m_strTitle.IsEmpty() && m_skin.nTitleHeight > 0) {
		RectF rcTitle;
		rcTitle.Y = 2;
		rcTitle.X = ELCBAR_TITLEITEM_MARGIN;
		rcTitle.Height = (REAL)m_skin.nTitleHeight;
		rcTitle.Width = (REAL)m_sizeBar.cx;
		if (m_pTitleIcon) 
			rcTitle.X += m_pTitleIcon->GetWidth() + ELCBAR_TITLEITEM_MARGIN;

		Font ftText(&FontFamily(m_lfTitle.lfFaceName), (REAL)abs(m_lfTitle.lfHeight), FontStyleBold, UnitPixel);

		StringFormat stringfmt;
		stringfmt.SetAlignment(StringAlignmentNear);
		stringfmt.SetLineAlignment(StringAlignmentCenter);
		stringfmt.SetFormatFlags(StringFormatFlagsNoWrap);
		stringfmt.SetHotkeyPrefix(HotkeyPrefixHide);

		Color crText;
		crText.SetFromCOLORREF(m_skin.crTitle);

		pGraphics->DrawString(m_strTitle, -1, &ftText, rcTitle, &stringfmt, &SolidBrush(crText));
	}

	for (CElcFloatBarItem::iterator it = m_arrInnerItem.begin();
		it != m_arrInnerItem.end();
		it ++)
	{
		PELC_FLOATBAR_NWBUTTON pButton = (PELC_FLOATBAR_NWBUTTON)((*it)->pvCtrl);
		thePainter.DrawImage(pGraphics,
			&pButton->button.image,
			&(*it)->rcItem,
			pButton->nState,
			0,
			0);
	}
}

void CElcFloatBar::DrawItems(Graphics* pGraphics)
{
	for (CElcFloatBarItem::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (!(*it)->bShow)
			continue;

		if ((*it)->type == ELCFIT_NONE) {
			ASSERT(0);
			continue;
		}
		else if ((*it)->type == ELCFIT_SPACE) {
			// do nothing
		}
		else if ((*it)->type == ELCFIT_BUTTON) {
			DrawButtons(pGraphics, (*it));
		}
		else if ((*it)->type == ELCFIT_LABEL) {
			DrawLabels(pGraphics, (*it));
		}
		else if ((*it)->type == ELCFIT_IMAGE) {
			thePainter.DrawFixedImage(pGraphics,
				(*it)->pvCtrl,
				CPoint((*it)->rcItem.X, (*it)->rcItem.Y),
				NULL,
				0);
		}
		else if ((*it)->type == ELCFIT_SEPARATOR) {
			thePainter.DrawImage(pGraphics,
				&m_skin.separator,
				&(*it)->rcItem,
				0,
				1,
				0);
		}
	}
}

void CElcFloatBar::DrawButtons(Graphics* pGraphics, PELC_FLOATBAR_ITEM pItem)
{
	PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)pItem->pvCtrl;

	ImageAttributes *pImageAtt = NULL;
	ImageAttributes imageAtt;
	if (pButton->nState == ELCBS_DISABLED) {
		imageAtt.SetColorMatrix(&g_GrayColorMatrix);
		pImageAtt = &imageAtt;
	}

	Rect rcItem = pItem->rcItem;
	if (m_bShowButtonText) {
		rcItem.Height += pItem->sizeText.cy;
	}
	else {
		rcItem = pItem->rcItem;
	}

	if (pButton->bChecked || pButton->nState == ELCBS_DOWN) {
		Rect rect = rcItem;
		rect.Inflate(ELCBAR_BUTTON_BK_MARGIN, ELCBAR_BUTTON_BK_MARGIN);
		thePainter.DrawImage(pGraphics,
			&m_skin.buttonbackgnd,
			&rect,
			2,
			1,
			pImageAtt);
	}
	else if (pButton->nState == ELCBS_HOT) {
		Color crBorder(150, 128, 128, 128);
		Pen pen(crBorder, 1);

		GraphicsPath grphpath;
		CreateRoundRect(&grphpath, rcItem, 6);

		SmoothingMode smoothingmode = pGraphics->GetSmoothingMode();
		pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
		pGraphics->DrawPath(&pen, &grphpath);
		pGraphics->SetSmoothingMode(smoothingmode);
	}


	CPoint point;
	point.y = pItem->rcItem.Y + m_nButtonMargin;
	if (pButton->nDropType == ELCBDT_DROPDOWN) {
		point.x = rcItem.X + (rcItem.Width - pButton->pImage->GetWidth() - m_sizeDropImage.cx - m_nButtonMargin - ELCBAR_DROPDOWN_MARGIN) / 2;
	}
	else {
		point.x = rcItem.X + (rcItem.Width - pButton->pImage->GetWidth()) / 2;
	}

	thePainter.DrawFixedImage(pGraphics,
		pButton->pImage,
		point,
		NULL,
		pImageAtt);

	if (pButton->nDropType == ELCBDT_DROPDOWN) {
		Rect rect;
		rect.Width = m_sizeDropImage.cx;
		rect.Height = m_sizeDropImage.cy;
		rect.X = point.x + pButton->pImage->GetWidth() + ELCBAR_DROPDOWN_MARGIN;
		rect.Y = pItem->rcItem.Y + (pItem->rcItem.Height - m_sizeDropImage.cy) / 2;

		thePainter.DrawImage(pGraphics,
			&m_skin.dropdown,
			&rect,
			pButton->nState,
			1,
			pImageAtt);
	}

	if (pItem->lpszText && pItem->cchText > 0 && m_bShowButtonText) {
		RectF rect;
		rect.X = (REAL)rcItem.X;
		rect.Y = (REAL)(rcItem.Y + rcItem.Height - pItem->sizeText.cy);
		rect.Width = (REAL)rcItem.Width;
		rect.Height = (REAL)pItem->sizeText.cy;

		Font ftText(&FontFamily(m_lfText.lfFaceName), (REAL)(abs(m_lfText.lfHeight)), FontStyleRegular, UnitPixel);

		StringFormat stringfmt;
		stringfmt.SetAlignment(StringAlignmentCenter);
		stringfmt.SetLineAlignment(StringAlignmentCenter);
		stringfmt.SetFormatFlags(StringFormatFlagsNoWrap);
		stringfmt.SetHotkeyPrefix(HotkeyPrefixHide);

		Color crText(200, GetRValue(m_skin.crLabel), GetGValue(m_skin.crLabel), GetBValue(m_skin.crLabel));

		TextRenderingHint textrenderinghint = pGraphics->GetTextRenderingHint();

		if (!IsFontSmoothingClearType())
			pGraphics->SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
		else 
			pGraphics->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

		//pGraphics->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
		pGraphics->DrawString(pItem->lpszText, pItem->cchText, &ftText, rect, &stringfmt, &SolidBrush(crText));
		pGraphics->SetTextRenderingHint(textrenderinghint);
	}
}

void CElcFloatBar::DrawLabels(Graphics* pGraphics, PELC_FLOATBAR_ITEM pItem)
{
	if (!pItem->lpszTip || pItem->cchTip <= 0)
		return;

	RectF rect;
	rect.X = (REAL)pItem->rcItem.X;
	rect.Y = (REAL)pItem->rcItem.Y;
	rect.Width = (REAL)pItem->rcItem.Width;
	rect.Height = (REAL)pItem->rcItem.Height;

	Font ftText(&FontFamily(m_lfLabel.lfFaceName), (REAL)(abs(m_lfLabel.lfHeight)), FontStyleRegular, UnitPixel);

	StringFormat stringfmt;
	stringfmt.SetAlignment(StringAlignmentCenter);
	stringfmt.SetLineAlignment(StringAlignmentCenter);
	stringfmt.SetFormatFlags(StringFormatFlagsNoWrap);
	stringfmt.SetHotkeyPrefix(HotkeyPrefixHide);

	Color crText(254, GetRValue(m_skin.crLabel), GetGValue(m_skin.crLabel), GetBValue(m_skin.crLabel));

	TextRenderingHint textrenderinghint = pGraphics->GetTextRenderingHint();
	//pGraphics->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	if (!IsFontSmoothingClearType())
		pGraphics->SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
	else 
		pGraphics->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	pGraphics->DrawString(pItem->lpszTip, pItem->cchTip, &ftText, rect, &stringfmt, &SolidBrush(crText));
	pGraphics->SetTextRenderingHint(textrenderinghint);

}

void CElcFloatBar::DrawTip(Graphics* pGraphics)
{
	if (!m_pCurItem 
		|| !m_pCurItem->lpszTip
		|| m_pCurItem->cchTip <= 0
		|| m_pCurItem->type != ELCFIT_BUTTON)
		return;

	Rect rect;

	rect.Y = m_sizeBar.cy + 4;
	rect.Height = max(m_pCurItem->sizeTip.cy, CElcSkinDoc::GetImageSize(&m_skin.tipbackground).cy);

	rect.X = m_pCurItem->rcItem.X + m_pCurItem->rcItem.Width / 2 - m_pCurItem->sizeTip.cx / 2;
	if (rect.X + m_pCurItem->sizeTip.cx >= m_sizeBar.cx - 1)
		rect.X = m_sizeBar.cx - m_pCurItem->sizeTip.cx - 1;
	if (rect.X < 1)
		rect.X = 1;

	rect.Width = m_pCurItem->sizeTip.cx;
	if (rect.Width >= m_sizeBar.cx)
		rect.Width = m_sizeBar.cx - 2;

	thePainter.DrawImage(pGraphics,
		&m_skin.tipbackground,
		&rect,
		0,
		1,
		0);

	// draw tip text
	FontStyle fontstyle = FontStyleRegular;
	if (m_lfTip.lfWeight > 400)
		fontstyle = FontStyleBold;

	Font ftText(&FontFamily(m_lfTip.lfFaceName), (REAL)abs(m_lfTip.lfHeight), fontstyle, UnitPixel);

	StringFormat stringfmt;
	stringfmt.SetAlignment(StringAlignmentCenter);
	stringfmt.SetLineAlignment(StringAlignmentCenter);
	stringfmt.SetFormatFlags(StringFormatFlagsNoWrap);
	stringfmt.SetHotkeyPrefix(HotkeyPrefixHide);

	SmoothingMode smoothingmode = pGraphics->GetSmoothingMode();
	TextRenderingHint textrenderinghint = pGraphics->GetTextRenderingHint();

	pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
	//pGraphics->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	if (!IsFontSmoothingClearType())
		pGraphics->SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
	else 
		pGraphics->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	Color crText = COLORREF2Color(m_skin.crTip, 254);
	RectF rcText((REAL)rect.X, (REAL)rect.Y, (REAL)rect.Width, (REAL)rect.Height);
	rcText.Offset(2, 2);
	pGraphics->DrawString(m_pCurItem->lpszTip, m_pCurItem->cchTip, &ftText, rcText, &stringfmt, &SolidBrush(crText));

	pGraphics->SetTextRenderingHint(textrenderinghint);
	pGraphics->SetSmoothingMode(smoothingmode);
}

LRESULT CElcFloatBar::OnNcHitTest(CPoint point)
{
	CRect rect;
	GetWindowRect(rect);
	point.Offset(-rect.left, -rect.top);

	if (m_bMirror)
		point.x = rect.Width() - point.x;

	PELC_FLOATBAR_ITEM pItem = GetHitItem(point);
	if (m_pCurItem != pItem) {
		if (m_pCurItem) {
			if (m_pCurItem->type == ELCFIT_BUTTON) {
				PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)m_pCurItem->pvCtrl;
				if (pButton->nState != ELCBS_DISABLED 
					&& !pButton->bChecked
					&& pButton->nState != ELCBS_NORMAL) 
				{
					pButton->nState = ELCBS_NORMAL;
				}
			}
			else if (m_pCurItem->type == ELCFIT_NWBUTTON) {
				PELC_FLOATBAR_NWBUTTON pButton = (PELC_FLOATBAR_NWBUTTON)m_pCurItem->pvCtrl;
				if (pButton->nState != ELCBS_DISABLED 
					&& !pButton->bChecked
					&& pButton->nState != ELCBS_NORMAL) 
				{
					pButton->nState = ELCBS_NORMAL;
				}
			}
		}
		m_pCurItem = pItem;

		Repaint();
	}
	
	if (m_pCurItem == NULL) {
		if (m_rcDrag.PtInRect(point))
			return HTCAPTION;
		else
			return HTNOWHERE;
	}

	return HTMENU;
}

void CElcFloatBar::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (m_bMirror)
		point.x = m_rcWindow.Width() - point.x;

	if (m_pCurItem) {
		if (m_pCurItem->type == ELCFIT_BUTTON) {
			PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)m_pCurItem->pvCtrl;
			if (pButton->nState != ELCBS_DISABLED) {
				pButton->nState = ELCBS_DOWN;
				Repaint();
			}
		}
		else if (m_pCurItem->type == ELCFIT_NWBUTTON) {
			PELC_FLOATBAR_NWBUTTON pButton = (PELC_FLOATBAR_NWBUTTON)m_pCurItem->pvCtrl;
			if (pButton->nState != ELCBS_DISABLED) {
				pButton->nState = ELCBS_DOWN;
				Repaint();
			}
		}
	}

	CWnd::OnNcLButtonDown(nHitTest, point);
}

void CElcFloatBar::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	if (m_bMirror)
		point.x = m_rcWindow.Width() - point.x;

	if (nHitTest == HTCAPTION) {
		CWnd::OnNcLButtonUp(nHitTest, point);
	}
	else if (m_pCurItem) {
		if (m_pCurItem->type == ELCFIT_BUTTON) {
			PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)m_pCurItem->pvCtrl;
			if (pButton->nState == ELCBS_DOWN) {
				pButton->nState = ELCBS_HOT;
				::PostMessage(m_hNotifyWnd, WM_COMMAND, m_pCurItem->nId, 0);
			}
		}
		else if (m_pCurItem->type == ELCFIT_NWBUTTON) {
			PELC_FLOATBAR_NWBUTTON pButton = (PELC_FLOATBAR_NWBUTTON)m_pCurItem->pvCtrl;
			if (pButton->nState == ELCBS_DOWN) {
				//if (!pButton->bChecked)
				//	pButton->nState = ELCBS_HOT;
				PostMessage(WM_COMMAND, m_pCurItem->nId);
				Repaint();
			}
		}
	}
}

void CElcFloatBar::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (m_bMirror)
		point.x = m_rcWindow.Width() - point.x;

	if (m_pCurItem) {
		if (m_pCurItem->type == ELCFIT_BUTTON) {
			PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)m_pCurItem->pvCtrl;
			if (pButton->nState == ELCBS_NORMAL) {
				pButton->nState = ELCBS_HOT;
			}
		}
		else if (m_pCurItem->type == ELCFIT_NWBUTTON) {
			PELC_FLOATBAR_NWBUTTON pButton = (PELC_FLOATBAR_NWBUTTON)m_pCurItem->pvCtrl;
			if (pButton->nState == ELCBS_NORMAL) {
				pButton->nState = ELCBS_HOT;
				Repaint();
			}
		}
	}

	CWnd::OnNcMouseMove(nHitTest, point);
}

void CElcFloatBar::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{	
	CWnd::OnWindowPosChanging(lpwndpos);

	CRect rcWork;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);

	if (abs(lpwndpos->x) <= (rcWork.left + SNAP_SIZE))
	{
		lpwndpos->x = rcWork.left;
	}
	else if ((lpwndpos->x + lpwndpos->cx) >= (rcWork.right - SNAP_SIZE) &&
				(lpwndpos->x + lpwndpos->cx) <= (rcWork.right + SNAP_SIZE))
	{
		lpwndpos->x = rcWork.right - lpwndpos->cx;
	}

	if (abs( lpwndpos->y) <= (rcWork.top + SNAP_SIZE ))
	{
		lpwndpos->y = rcWork.top;
	}
	else if ((lpwndpos->y + lpwndpos->cy) >= (rcWork.bottom - SNAP_SIZE) &&
				(lpwndpos->y + lpwndpos->cy) <= (rcWork.bottom + SNAP_SIZE))
	{
		lpwndpos->y = rcWork.bottom-lpwndpos->cy;
	}

	EnablePinButton(lpwndpos->y == 0);
}

void CElcFloatBar::SetBarAutoHide(BOOL bEnable)
{
	if (m_bEnableAutoHide == bEnable)
		return;

	m_bEnableAutoHide = bEnable;

	PELC_FLOATBAR_ITEM pItem = FindInnerItemPtr(ID_ELC_FLOATBAR_ITEM_PIN);
	if (pItem) {
		((PELC_FLOATBAR_NWBUTTON)(pItem->pvCtrl))->bChecked = !m_bEnableAutoHide;
		((PELC_FLOATBAR_NWBUTTON)(pItem->pvCtrl))->nState = m_bEnableAutoHide ? ELCBS_NORMAL : ELCBS_DOWN;
		Repaint();
	}

	if (!m_bEnableAutoHide)
		ShowBarDirect();
}

BOOL CElcFloatBar::IsBarAutoHide()
{
	return m_bEnableAutoHide;
}

void CElcFloatBar::ShowBarDirect(BOOL bShow)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.bottom -= m_nTipHeight;

	if (bShow) {
		if (m_dwFoldState == FOLD_STATE_EXPAND)
			return;

		KillTimer(ID_TIMER_FOLD);
		m_dwFoldState = FOLD_STATE_EXPAND;
		SetWindowPos(NULL, rcWindow.left, 0, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
	else {
		if (m_dwFoldState == FOLD_STATE_FOLDED)
			return;

		KillTimer(ID_TIMER_FOLD);
		int nTop = m_skin.nPeekHeight - rcWindow.Height();
		m_dwFoldState = FOLD_STATE_FOLDED;
		SetWindowPos(NULL, rcWindow.left, nTop, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}

void CElcFloatBar::FoldBar(BOOL bFold)
{
	m_bFoldBar = bFold;
	if (m_bFoldBar)
	{
		if (m_dwFoldState != FOLD_STATE_EXPAND)
			return;

		m_dwFoldState = FOLD_STATE_FOLDING;
		m_nFactHeight = 0;
	}
	else
	{
		if (m_dwFoldState == FOLD_STATE_EXPAND ||
			m_dwFoldState != FOLD_STATE_FOLDED)
			return;

		m_nFactHeight = -(m_sizeBar.cy - m_skin.nPeekHeight);
	}

	SetTimer(ID_TIMER_FOLD, 10, NULL);
}

void CElcFloatBar::OnTimer(UINT_PTR nIDEvent)
{
	if (ID_TIMER_CHECK_STATE == nIDEvent) {

		CRect rcWindow;
		GetWindowRect(&rcWindow);
		rcWindow.bottom -= m_nTipHeight;

		CPoint point;
		GetCursorPos(&point);

		BOOL bRepaint = FALSE;

		if (IsWindowVisible()) {
			if (m_bEnableAutoHide) {
				if (!rcWindow.PtInRect(point)) {
					if (rcWindow.top == 0) {
						m_nAutoHideCount ++;
						if (m_nAutoHideCount >= 50) {
							m_nAutoHideCount = 0;
							if (m_dwFoldState == FOLD_STATE_EXPAND) 
								FoldBar(TRUE);
						}
					}
					else
						m_nAutoHideCount = 0;

					if (m_pCurItem) {
						m_pCurItem = NULL;
						bRepaint = TRUE;
					}
				}
				else {
					if (m_dwFoldState != FOLD_STATE_EXPAND ||
						m_dwFoldState != FOLD_STATE_EXPANDING)
					{
						FoldBar(FALSE);
					}

					m_nAutoHideCount = 0;
				}
			}
			else {
				if (m_dwFoldState != FOLD_STATE_EXPAND ||
					m_dwFoldState != FOLD_STATE_EXPANDING)
				{
					FoldBar(FALSE);
				}
			}
		}

		if (m_dwFoldState == FOLD_STATE_FOLDING
			|| m_dwFoldState == FOLD_STATE_EXPANDING
			|| m_dwFoldState == FOLD_STATE_FOLDED)
			return;

		point.Offset(-rcWindow.left, -rcWindow.top);
		if (m_bMirror)
			point.x = m_rcWindow.Width() - point.x;

		// function buttons
		for (CElcFloatBarItem::iterator it = m_arrItem.begin();
			it != m_arrItem.end();
			it ++)
		{
			if ((*it)->type != ELCFIT_BUTTON)
				continue;

			int nYPos = m_skin.nTitleHeight + (m_sizeBar.cy - m_skin.nTitleHeight - (*it)->rcItem.Height) / 2;

			PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)(*it)->pvCtrl;

			if (pButton->nState == ELCBS_HOT && !pButton->bChecked) {

				if (!(*it)->rcItem.Contains(point.x, point.y)) {
					pButton->nState = ELCBS_NORMAL;
					pButton->nAmState = ELCFBS_AM_NONE;
					(*it)->rcItem.Y = nYPos;
				}
				else if (pButton->nAmState == ELCFBS_AM_UP) {
					(*it)->rcItem.Y -= 2;
					if (nYPos - (*it)->rcItem.Y >= 6)
						pButton->nAmState = ELCFBS_AM_DOWN;
				}
				else if (pButton->nAmState == ELCFBS_AM_DOWN) {
					(*it)->rcItem.Y += 2;
					if (nYPos - (*it)->rcItem.Y <= -4)
						pButton->nAmState = ELCFBS_AM_BACK;
				}
				else if (pButton->nAmState == ELCFBS_AM_BACK) {
					(*it)->rcItem.Y -= 1;
					if (nYPos - (*it)->rcItem.Y >= 2) {
						(*it)->rcItem.Y = nYPos;
						pButton->nAmState = ELCFBS_AM_WAIT;
					}
				}
				else if (pButton->nAmState >= ELCFBS_AM_WAIT && pButton->nAmState <= ELCFBS_AM_MAX_WAIT) {
					pButton->nAmState ++;
					if (pButton->nAmState >= ELCFBS_AM_MAX_WAIT)
						pButton->nAmState = ELCFBS_AM_NONE;
					else
						continue;
				}
				else if (pButton->nAmState == ELCFBS_AM_NONE) {
					(*it)->rcItem.Y += 2;
					pButton->nAmState = ELCFBS_AM_UP;
				}

				bRepaint = TRUE;
			}
			else if ((*it)->rcItem.Y != nYPos) {
				(*it)->rcItem.Y = nYPos;
				pButton->nAmState = ELCFBS_AM_NONE;
				bRepaint = TRUE;
			}
		}

		// bar-inner buttons
		for (CElcFloatBarItem::iterator it = m_arrInnerItem.begin();
			it != m_arrInnerItem.end();
			it ++)
		{
			if ((*it)->type != ELCFIT_NWBUTTON)
				continue;

			PELC_FLOATBAR_NWBUTTON pButton = (PELC_FLOATBAR_NWBUTTON)(*it)->pvCtrl;

			if (pButton->nState != ELCBS_DISABLED 
				&& !pButton->bChecked)
			{
				if ((*it)->rcItem.Contains(point.x, point.y)) {
					if (pButton->nState != ELCBS_DOWN
						&& pButton->nState != ELCBS_HOT) 
					{
						pButton->nState = ELCBS_HOT;
						bRepaint = TRUE;
					}
				}
				else if (pButton->nState != ELCBS_NORMAL) {
					pButton->nState = ELCBS_NORMAL;
					bRepaint = TRUE;
				}
			}
		}

		if (bRepaint) {
			Repaint();
		}
	}
	else if (ID_TIMER_FOLD == nIDEvent)
	{
		CRect rcWindow;
		GetWindowRect(&rcWindow);
		rcWindow.bottom -= m_nTipHeight;

		if (m_bFoldBar)
		{
			m_nFactHeight -= ELCBAR_FOLD_INCREASE;

			if (rcWindow.Height() - abs(m_nFactHeight) <= m_skin.nPeekHeight)
			{				
				m_dwFoldState = FOLD_STATE_FOLDED;
				KillTimer(ID_TIMER_FOLD);
			}
			else
			{
				m_dwFoldState = FOLD_STATE_FOLDING;
			}
		}
		else
		{
			m_nFactHeight += ELCBAR_FOLD_INCREASE;

			if (m_nFactHeight >= 0)
			{
				m_nFactHeight = 0;
				KillTimer(ID_TIMER_FOLD);
				m_dwFoldState = FOLD_STATE_EXPAND;
			}
			else
			{
				m_dwFoldState = FOLD_STATE_EXPANDING;
			}
		}

		SetWindowPos(NULL, rcWindow.left, m_nFactHeight, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}

	CWnd::OnTimer(nIDEvent);
}

PELC_FLOATBAR_ITEM CElcFloatBar::GetHitItem(CPoint point)
{
	for (CElcFloatBarItem::iterator it = m_arrInnerItem.begin();
		it != m_arrInnerItem.end();
		it ++)
	{
		if ((*it)->rcItem.Contains(point.x, point.y)) {
			return (*it);
		}
	}

	for (CElcFloatBarItem::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (!(*it)->bShow)
			continue;
		
		if ((*it)->rcItem.Contains(point.x, point.y)) {
			return (*it);
		}
	}

	return NULL;
}

int CElcFloatBar::FindItem(UINT nId)
{
	for (size_t i = 0; i < m_arrItem.size(); i ++)
	{
		if (m_arrItem[i]->nId == nId) 
			return (int)i;
	}

	return -1;
}

PELC_FLOATBAR_ITEM CElcFloatBar::FindItemPtr(UINT nId)
{
	for (CElcFloatBarItem::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if ((*it)->nId == nId)
			return (*it);
	}

	return NULL;
}

PELC_FLOATBAR_ITEM CElcFloatBar::FindInnerItemPtr(UINT nId)
{
	for (CElcFloatBarItem::iterator it = m_arrInnerItem.begin();
		it != m_arrInnerItem.end();
		it ++)
	{
		if ((*it)->nId == nId)
			return (*it);
	}

	return NULL;
}

void CElcFloatBar::Relayout()
{
	m_sizeBar = CalcItemsPos();
	SetWindowPos(NULL, 0, 0, m_sizeBar.cx, m_sizeBar.cy + m_nTipHeight, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);

	if (m_dwFoldState == FOLD_STATE_FOLDED 
		|| m_dwFoldState == FOLD_STATE_FOLDING)
		ShowBarDirect(TRUE);
	Repaint();
}

CSize CElcFloatBar::CalcItemsPos()
{
	CSize size = CElcSkinDoc::GetImageSize(&m_skin.background);
	CSize sizeDropdown = CElcSkinDoc::GetImageSize(&m_skin.dropdown);

	if (m_skin.background.stretch & EISR_VERTICAL) {
		size.cy = m_nMaxItemHeight + size.cy - (m_skin.background.nBottomSplit - m_skin.background.nTopSplit);
	}

	int nXPos = m_nHMargin;
	if (m_skin.background.stretch & EISR_HORIZONTAL) {
		nXPos = m_nHMargin + m_skin.background.nLeftSplit;
	}

	for (CElcFloatBarItem::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if (!(*it)->bShow) 
			continue;

		(*it)->rcItem.X = nXPos;

		if ((*it)->type == ELCFIT_SEPARATOR) {
			(*it)->rcItem.Height = min(m_nMaxItemHeight, size.cy - m_skin.nTitleHeight - 10);
			(*it)->rcItem.Y = m_skin.nTitleHeight + (size.cy - m_skin.nTitleHeight - (*it)->rcItem.Height) / 2;
			nXPos += (*it)->rcItem.Width + m_nHMargin;
		}
		else if ((*it)->type == ELCFIT_BUTTON) {
			if (m_bShowButtonText) {
				(*it)->rcItem.Y = m_skin.nTitleHeight + (size.cy - m_skin.nTitleHeight - (*it)->rcItem.Height - (*it)->sizeText.cy) / 2;
				nXPos += (*it)->rcItem.Width + m_nHMargin + (*it)->sizeText.cy;
			}
			else {
				(*it)->rcItem.Y = m_skin.nTitleHeight + (size.cy - m_skin.nTitleHeight - (*it)->rcItem.Height) / 2;
				nXPos += (*it)->rcItem.Width + m_nHMargin;
			}
		}
		else {
			(*it)->rcItem.Y = m_skin.nTitleHeight + (size.cy - m_skin.nTitleHeight - (*it)->rcItem.Height) / 2;
			nXPos += (*it)->rcItem.Width + m_nHMargin;
		}

		//nXPos += (*it)->rcItem.Width + m_nHMargin;
	}

	if (m_skin.background.stretch & EISR_HORIZONTAL) 
		nXPos += size.cx - m_skin.background.nRightSplit;

	size.cx = max(nXPos, size.cx);

	CRect rect(0, 0, size.cx, size.cy);

	for (CElcFloatBarItem::iterator it = m_arrInnerItem.begin();
		it != m_arrInnerItem.end();
		it ++)
	{
		PELC_FLOATBAR_NWBUTTON pButton = (PELC_FLOATBAR_NWBUTTON)((*it)->pvCtrl);
		CRect rcButton = CElcSkinDoc::CalcRect(&rect, &pButton->button.pos.point, &pButton->button.pos.size);
		(*it)->rcItem.X = rcButton.left;
		(*it)->rcItem.Y = rcButton.top;
		(*it)->rcItem.Width = rcButton.Width();
		(*it)->rcItem.Height = rcButton.Height();
	}

	m_rcDrag = CElcSkinDoc::CalcRect(&rect, &m_skin.drag.point, &m_skin.drag.size);

	return size;
}

CRect CElcFloatBar::GetItemRect(UINT nId)
{
	CRect rect(0,0,0,0);
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (pItem) {
		rect.left = pItem->rcItem.X;
		rect.top = pItem->rcItem.Y;
		rect.right = rect.left + pItem->rcItem.Width;
		rect.bottom = rect.top + pItem->rcItem.Height;

		ClientToScreen(&rect);
	}

	return rect;
}

void CElcFloatBar::DelItem(UINT nId)
{
	int nItem = FindItem(nId);
	if (nItem != -1) {
		if (m_arrItem[nItem]->type == ELCFIT_BUTTON) {
			PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)m_arrItem[nItem]->pvCtrl;
			delete pButton->pImage;
			delete pButton;
		}
		else if (m_arrItem[nItem]->type == ELCFIT_IMAGE) {
			delete (Image *)m_arrItem[nItem]->pvCtrl;
		}

		if (m_arrItem[nItem]->lpszTip)
			delete []m_arrItem[nItem]->lpszTip;

		delete m_arrItem[nItem];

		m_arrItem.erase(m_arrItem.begin() + nItem);
	}
}

void CElcFloatBar::DelAllItem()
{
	for (CElcFloatBarItem::iterator it = m_arrItem.begin();
		it != m_arrItem.end();
		it ++)
	{
		if ((*it)->type == ELCFIT_BUTTON) {
			PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)((*it)->pvCtrl);
			delete pButton->pImage;
			delete pButton;
		}
		else if ((*it)->type == ELCFIT_IMAGE) {
			delete (Image *)((*it)->pvCtrl);
		}

		if ((*it)->lpszText)
			delete [](*it)->lpszText;
		if ((*it)->lpszTip)
			delete [](*it)->lpszTip;

		delete (*it);
	}

	m_arrItem.clear();
}

void CElcFloatBar::SetTitle(LPCTSTR lpszTitle)
{
	m_strTitle = lpszTitle;
	Repaint();
}

void CElcFloatBar::SetTitleIcon(UINT nResId, LPCTSTR lpszType)
{
	if (m_pTitleIcon)
		delete m_pTitleIcon;
	
	if (!ImageFromIDResource(nResId, lpszType, m_pTitleIcon)) {
		if (m_pTitleIcon)
			delete m_pTitleIcon;
	}

	Repaint();
}

BOOL CElcFloatBar::AddButton(UINT nId, UINT nResId, LPCTSTR lpszType, LPCTSTR lpszTip, ELC_BUTTON_DROP_TYPE type)
{
	return AddButton(nId, NULL, nResId, lpszType, lpszTip, type);
}

BOOL CElcFloatBar::AddButton(UINT nId, LPCTSTR lpszText, UINT nResId, LPCTSTR lpszType, LPCTSTR lpszTip, ELC_BUTTON_DROP_TYPE type)
{
	if (nId <= 0 || nResId <= 0 || FindItem(nId) != -1)
		return FALSE;

	Image* pImage = NULL;
	if (!ImageFromIDResource(nResId, lpszType, pImage)) {
		if (pImage)
			delete pImage;
		return FALSE;
	}

	PELC_FLOATBAR_BUTTON pButton = new ELC_FLOATBAR_BUTTON;
	memset(pButton, 0, sizeof(ELC_FLOATBAR_BUTTON));
	pButton->bChecked = FALSE;
	pButton->nState = ELCBS_NORMAL;
	pButton->nDropType = type;
	pButton->pImage = pImage;

	PELC_FLOATBAR_ITEM pItem = new ELC_FLOATBAR_ITEM;
	memset(pItem, 0, sizeof(ELC_FLOATBAR_ITEM));
	pItem->nId = nId;
	pItem->type = ELCFIT_BUTTON;
	pItem->bShow = TRUE;
	pItem->pvCtrl = pButton;

	pItem->rcItem.X = 0;
	pItem->rcItem.Y = 0;
	pItem->rcItem.Width = pImage->GetWidth() + m_nButtonMargin * 2;
	pItem->rcItem.Height = pImage->GetHeight() + m_nButtonMargin * 2;

	if (type == ELCBDT_DROPDOWN) {
		pItem->rcItem.Width += CElcSkinDoc::GetImageSize(&m_skin.dropdown).cx + ELCBAR_DROPDOWN_MARGIN;
	}

	if (lpszText) {
		pItem->cchText = (int)(_tcslen(lpszText) + 1);
		pItem->lpszText = new TCHAR[pItem->cchText];
		_tcscpy(pItem->lpszText, lpszText);

		pItem->sizeText = CElcSkinDoc::GetStringSize(NULL, pItem->lpszText, m_lfText);
		pItem->sizeText.cx += ELCBAR_TEXT_SIZE_REVISE;

		//pItem->rcItem.Width = max(pItem->sizeText.cx, pItem->rcItem.Width);
		//pItem->rcItem.Height = pItem->sizeText.cy + pItem->rcItem.Height;
	}

	if (lpszTip) {
		pItem->cchTip = (int)(_tcslen(lpszTip) + 1);
		pItem->lpszTip = new TCHAR[pItem->cchTip];
		_tcscpy(pItem->lpszTip, lpszTip);

		pItem->sizeTip = CElcSkinDoc::GetStringSize(NULL, pItem->lpszTip, m_lfTip);
		pItem->sizeTip.cx += ELCBAR_TEXT_SIZE_REVISE;
	}

	m_arrItem.push_back(pItem);

	m_nMaxItemHeight = pItem->rcItem.Height;

	Relayout();

	return TRUE;
}

BOOL CElcFloatBar::AddSeparator(UINT nId)
{
	if (nId <= 0 || FindItem(nId) != -1)
		return FALSE;

	PELC_FLOATBAR_ITEM pItem = new ELC_FLOATBAR_ITEM;
	memset(pItem, 0, sizeof(ELC_FLOATBAR_ITEM));

	pItem->nId = nId;
	pItem->type = ELCFIT_SEPARATOR;
	pItem->bShow = TRUE;

	pItem->rcItem.X = 0;
	pItem->rcItem.Y = 0;
	pItem->rcItem.Width = CElcSkinDoc::GetImageSize(&m_skin.separator).cx;
	pItem->rcItem.Height = CElcSkinDoc::GetImageSize(&m_skin.separator).cy;

	m_arrItem.push_back(pItem);

	Relayout();

	return TRUE;
}

BOOL CElcFloatBar::AddLabel(UINT nId, LPCTSTR lpszText)
{
	if (nId <= 0 || FindItem(nId) != -1)
		return FALSE;

	PELC_FLOATBAR_ITEM pItem = new ELC_FLOATBAR_ITEM;
	memset(pItem, 0, sizeof(ELC_FLOATBAR_ITEM));

	pItem->nId = nId;
	pItem->type = ELCFIT_LABEL;
	pItem->bShow = TRUE;
	pItem->pvCtrl = NULL;

	CSize size = CElcSkinDoc::GetStringSize(NULL, lpszText, m_lfLabel);
	size.cx += ELCBAR_TEXT_SIZE_REVISE;

	if (lpszText) {
		pItem->cchTip = (int)(_tcslen(lpszText) + 1);
		pItem->lpszTip = new TCHAR[pItem->cchTip];
		_tcscpy(pItem->lpszTip, lpszText);
	}

	pItem->rcItem.X = 0;
	pItem->rcItem.Y = 0;
	pItem->rcItem.Width = size.cx;
	pItem->rcItem.Height = size.cy;

	m_arrItem.push_back(pItem);

	Relayout();

	return TRUE;
}

BOOL CElcFloatBar::AddImage(UINT nId, UINT nResId, LPCTSTR lpszType)
{
	if (nId <= 0 || nResId <= 0 || FindItem(nId) != -1)
		return FALSE;

	Image* pImage = NULL;
	if (!ImageFromIDResource(nResId, lpszType, pImage)) {
		if (pImage)
			delete pImage;
		return FALSE;
	}

	PELC_FLOATBAR_ITEM pItem = new ELC_FLOATBAR_ITEM;
	memset(pItem, 0, sizeof(ELC_FLOATBAR_ITEM));

	pItem->nId = nId;
	pItem->type = ELCFIT_IMAGE;
	pItem->bShow = TRUE;
	pItem->pvCtrl = pImage;

	pItem->rcItem.X = 0;
	pItem->rcItem.Y = 0;
	pItem->rcItem.Width = pImage->GetWidth();
	pItem->rcItem.Height = pImage->GetHeight();

	m_arrItem.push_back(pItem);

	Relayout();

	return TRUE;
}

BOOL CElcFloatBar::AddSpace(UINT nId, int nSpaceWidth)
{
	if (nId <= 0 || FindItem(nId) != -1)
		return FALSE;

	PELC_FLOATBAR_ITEM pItem = new ELC_FLOATBAR_ITEM;
	memset(pItem, 0, sizeof(ELC_FLOATBAR_ITEM));

	pItem->nId = nId;
	pItem->type = ELCFIT_SPACE;
	pItem->bShow = TRUE;
	pItem->pvCtrl = NULL;

	pItem->rcItem.X = 0;
	pItem->rcItem.Y = 0;
	pItem->rcItem.Width = nSpaceWidth;
	pItem->rcItem.Height = 0;

	m_arrItem.push_back(pItem);

	Relayout();

	return TRUE;
}

void CElcFloatBar::SetButtonCheck(UINT nId, BOOL bChecked)
{
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (!pItem || pItem->type != ELCFIT_BUTTON)
		return;

	PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)pItem->pvCtrl;
	if (pButton->bChecked != bChecked) {
		pButton->bChecked = bChecked;
		Repaint();
	}
}

BOOL CElcFloatBar::IsButtonChecked(UINT nId)
{
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (!pItem || pItem->type != ELCFIT_BUTTON)
		return FALSE;

	return ((PELC_FLOATBAR_BUTTON)pItem->pvCtrl)->bChecked;
}

void CElcFloatBar::SetButtonImage(UINT nId, UINT nResId, LPCTSTR lpszType)
{
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (!pItem || pItem->type != ELCFIT_BUTTON)
		return;

	Image* pImage = NULL;
	if (!ImageFromIDResource(nResId, lpszType, pImage)) {
		if (pImage)
			delete pImage;
		return;
	}

	PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)pItem->pvCtrl;
	delete pButton->pImage;
	pButton->pImage = pImage;

	Repaint();
}

void CElcFloatBar::ShowButtonText(BOOL bShow)
{
	if (m_bShowButtonText == bShow)
		return;

	m_bShowButtonText = bShow;
	Relayout();
}

BOOL CElcFloatBar::IsShowButtonText()
{
	return m_bShowButtonText;
}

void CElcFloatBar::SetButtonText(UINT nId, LPCTSTR lpszText)
{
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (!pItem || pItem->type != ELCFIT_BUTTON)
		return;

	if (pItem->lpszText)
		delete []pItem->lpszText;
	pItem->lpszText = NULL;
	pItem->cchText = 0;

	if (!lpszText)
		return;

	pItem->cchText = (int)_tcslen(lpszText);
	if (lpszText && pItem->cchText > 0) {
		pItem->lpszText = new TCHAR[pItem->cchText + 1];
		_tcscpy(pItem->lpszText, lpszText);

		pItem->sizeText = CElcSkinDoc::GetStringSize(NULL, pItem->lpszText, m_lfText);
		pItem->sizeText.cx += ELCBAR_TEXT_SIZE_REVISE;
	}
}

void CElcFloatBar::SetButtonText(UINT nId, UINT nTextResId)
{
	CString strText;
	strText.LoadString(nTextResId);
	SetButtonTip(nId, strText);
}

void CElcFloatBar::SetButtonTip(UINT nId, LPCTSTR lpszTip)
{
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (!pItem || pItem->type != ELCFIT_BUTTON)
		return;

	if (pItem->lpszTip)
		delete []pItem->lpszTip;
	pItem->lpszTip = NULL;

	pItem->cchTip = (int)_tcslen(lpszTip);
	if (lpszTip && pItem->cchTip > 0) {
		pItem->lpszTip = new TCHAR[pItem->cchTip + 1];
		_tcscpy(pItem->lpszTip, lpszTip);

		pItem->sizeTip = CElcSkinDoc::GetStringSize(NULL, pItem->lpszTip, m_lfTip);
		pItem->sizeTip.cx += ELCBAR_TEXT_SIZE_REVISE;
	}
}

void CElcFloatBar::SetButtonTip(UINT nId, UINT nTipResId)
{
	CString strTip;
	strTip.LoadString(nTipResId);
	SetButtonTip(nId, strTip);
}

void CElcFloatBar::EnableButton(UINT nId, BOOL bEnable)
{
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (!pItem || pItem->type != ELCFIT_BUTTON)
		return;

	PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)pItem->pvCtrl;
	if (!bEnable) {
		pButton->nState = ELCBS_DISABLED;
		Repaint();
	}
	else if (pButton->nState == ELCBS_DISABLED) {
		pButton->nState = ELCBS_NORMAL;
		Repaint();
	}
}

BOOL CElcFloatBar::IsButtonEnable(UINT nId)
{
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (!pItem || pItem->type != ELCFIT_BUTTON)
		return FALSE;

	PELC_FLOATBAR_BUTTON pButton = (PELC_FLOATBAR_BUTTON)pItem->pvCtrl;
	return pButton->nState != ELCBS_DISABLED;
}

void CElcFloatBar::SetLabel(UINT nId, LPCTSTR lpszText, BOOL bAutoAdjustSize)
{
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (!pItem || pItem->type != ELCFIT_LABEL)
		return;

	if (pItem->lpszTip)
		delete []pItem->lpszTip;
	pItem->lpszTip = NULL;

	pItem->cchTip = (int)_tcslen(lpszText);
	if (lpszText && pItem->cchTip > 0) {
		pItem->lpszTip = new TCHAR[pItem->cchTip + 1];
		_tcscpy(pItem->lpszTip, lpszText);
	}

	if (bAutoAdjustSize) {
		CSize size = CElcSkinDoc::GetStringSize(NULL, lpszText, m_lfLabel);
		size.cx += ELCBAR_TEXT_SIZE_REVISE;

		pItem->rcItem.X = 0;
		pItem->rcItem.Y = 0;
		pItem->rcItem.Width = size.cx;
		pItem->rcItem.Height = size.cy;
		Relayout();
	}
	else {
		Repaint();
	}
}

CString CElcFloatBar::GetLabel(UINT nId)
{
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (!pItem || pItem->type != ELCFIT_LABEL)
		return (CString)_T("");

	return (CString)pItem->lpszTip;
}

void CElcFloatBar::SetLabelColor(COLORREF cr)
{
	m_skin.crLabel = cr;
	Repaint();
}

COLORREF CElcFloatBar::GetLabelColor()
{
	return m_skin.crLabel;
}

void CElcFloatBar::SetImage(UINT nId, UINT nResId, LPCTSTR lpszType)
{
	PELC_FLOATBAR_ITEM pItem = FindItemPtr(nId);
	if (!pItem || pItem->type != ELCFIT_IMAGE)
		return;

	Image* pImage = NULL;
	if (!ImageFromIDResource(nResId, lpszType, pImage)) {
		if (pImage)
			delete pImage;
		return;
	}

	delete (Image *)pItem->pvCtrl;
	pItem->pvCtrl = pImage;

	Repaint();
}

void CElcFloatBar::ResetBarPosition(HMONITOR hMonitor)
{
	SetWindowToMonitor(GetSafeHwnd(), hMonitor);
}

BOOL CElcFloatBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCAPTION) {
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		return TRUE;
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CElcFloatBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) 
	{
		ShowBarDirect(bShow);
	}

	CWnd::OnShowWindow(bShow, nStatus);
}

void CElcFloatBar::OnFlodBar()
{
	m_bEnableAutoHide = TRUE;
	PELC_FLOATBAR_ITEM pItem = FindInnerItemPtr(ID_ELC_FLOATBAR_ITEM_PIN);
	if (pItem) {
		((PELC_FLOATBAR_NWBUTTON)(pItem->pvCtrl))->bChecked = !m_bEnableAutoHide;
		((PELC_FLOATBAR_NWBUTTON)(pItem->pvCtrl))->nState = m_bEnableAutoHide ? ELCBS_NORMAL : ELCBS_DOWN;
	}

	int nScreenCx = GetSystemMetrics(SM_CXFULLSCREEN);
	int x = (nScreenCx - m_sizeBar.cx) / 2;
	SetWindowPos(NULL, x, 0, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOZORDER);
	ShowBarDirect(FALSE);
}

void CElcFloatBar::OnPinBar()
{
	m_bEnableAutoHide = !m_bEnableAutoHide;
	PELC_FLOATBAR_ITEM pItem = FindInnerItemPtr(ID_ELC_FLOATBAR_ITEM_PIN);
	if (pItem) {
		((PELC_FLOATBAR_NWBUTTON)(pItem->pvCtrl))->bChecked = !m_bEnableAutoHide;
		((PELC_FLOATBAR_NWBUTTON)(pItem->pvCtrl))->nState = m_bEnableAutoHide ? ELCBS_NORMAL : ELCBS_DOWN;
		Repaint();
	}
}

void CElcFloatBar::EnablePinButton(BOOL bEnable)
{
	PELC_FLOATBAR_ITEM pItem = FindInnerItemPtr(ID_ELC_FLOATBAR_ITEM_PIN);
	if (!pItem)
		return;

	PELC_FLOATBAR_NWBUTTON pButton = (PELC_FLOATBAR_NWBUTTON)pItem->pvCtrl;
	pButton->nState = bEnable ? (pButton->bChecked ? ELCBS_DOWN : ELCBS_NORMAL) : ELCBS_DISABLED;

	Repaint();
}
