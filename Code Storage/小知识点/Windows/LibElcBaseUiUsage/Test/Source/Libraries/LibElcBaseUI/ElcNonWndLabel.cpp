#include "StdAfx.h"
#include "LibElcBaseUI.h"
#include "ElcNonWndLabel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////
// CElcNonWndLabel

CElcNonWndLabel::CElcNonWndLabel(void)
{
	m_hParent = NULL;
	m_nId = 0;
	m_nAlign = ENWLA_LEFT;
	m_nEnableRedraw = TRUE;

	m_crText = RGB(0,0,0);
	m_rect.SetRectEmpty();

	GetMSShellDlgFont(m_ftText);
}

CElcNonWndLabel::~CElcNonWndLabel(void)
{
}

void CElcNonWndLabel::Repaint(CRect rect)
{
	if (!::IsWindow(m_hParent))
		return;

	::RedrawWindow(m_hParent, &rect, NULL, RDW_UPDATENOW|RDW_INVALIDATE);
}

BOOL CElcNonWndLabel::Create(LPCTSTR lpszText, HWND hParent, CRect rect, UINT nId)
{
	m_strText = lpszText;
	m_rect = rect;
	m_nId = nId;
	m_hParent = hParent;

	return TRUE;
}

void CElcNonWndLabel::SetFont(CFont *pFont)
{
	LOGFONT lf = {0};
	if (!pFont || !pFont->GetLogFont(&lf))
		return;

	m_ftText.DeleteObject();
	m_ftText.CreateFontIndirect(&lf);
}

inline UINT CElcNonWndLabel::GetId()
{
	return m_nId;
}

inline BOOL CElcNonWndLabel::PtInRect(CPoint point)
{
	return m_rect.PtInRect(point);
}

inline CRect CElcNonWndLabel::GetRect()
{
	return m_rect;
}

void CElcNonWndLabel::Move(CRect rcNew, BOOL bRedraw)
{
	m_rect = rcNew;
	if (bRedraw)
		Repaint(m_rect);
}

BOOL CElcNonWndLabel::MouseDown(CPoint point)
{
	return m_rect.PtInRect(point);
}

BOOL CElcNonWndLabel::MouseUp(CPoint point)
{
	return m_rect.PtInRect(point);
}

BOOL CElcNonWndLabel::MouseMove(CPoint point)
{
	return m_rect.PtInRect(point);
}

void CElcNonWndLabel::CaptureChanged()
{
}

void CElcNonWndLabel::SetRedraw(BOOL bEnable)
{
	m_nEnableRedraw = bEnable;
}

void CElcNonWndLabel::Draw(PVOID pvGraphics)
{
	if (m_strText.IsEmpty() || !pvGraphics)
		return;

	Graphics * pGrph = (Graphics *)pvGraphics;

	RectF rect;
	rect.X = (REAL)m_rect.left;
	rect.Y = (REAL)m_rect.top;
	rect.Width = (REAL)m_rect.Width();
	rect.Height = (REAL)m_rect.Height();

	Color crText;
	crText.SetFromCOLORREF(m_crText);

	LOGFONT lf = {0};
	m_ftText.GetLogFont(&lf);

	StringFormat stringfmt;
	if (ENWLA_LEFT == m_nAlign)
		stringfmt.SetAlignment(StringAlignmentNear);
	else if (ENWLA_CENTER == m_nAlign)
		stringfmt.SetAlignment(StringAlignmentCenter);
	else if (ENWLA_RIGHT == m_nAlign)
		stringfmt.SetAlignment(StringAlignmentFar);

	stringfmt.SetLineAlignment(StringAlignmentCenter);
	stringfmt.SetFormatFlags(StringFormatFlagsNoWrap);

	FontFamily fontFamily(lf.lfFaceName);
	Font font(&fontFamily, 14, FontStyleBold, UnitPixel);

	TextRenderingHint trh = pGrph->GetTextRenderingHint();
	pGrph->SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	pGrph->DrawString(
		m_strText, 
		-1, 
		&font, 
		rect, 
		&stringfmt, 
		&SolidBrush(crText));
	pGrph->SetTextRenderingHint(trh);
}
