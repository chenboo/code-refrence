#include "stdafx.h"
#include "GraphLib.h"
#include "TextEditWnd.h"

CTextEditWnd* CText::m_pWndTextEdit = NULL;

CText::CText()
{
	InitDataMember();
}

CText::CText(CWnd* pParent, Color clr)
{
	VERIFY(pParent);
	InitDataMember();
	m_crColor = clr;
	m_pWndParent = pParent;
	VERIFY(CreateEditTextBox(pParent));
}

CText::CText(const CText& gph)
{
	CopyTheBaseDataMember(&gph);

	m_pFont = gph.m_pFont->Clone();
	m_strText = gph.m_strText;

	m_bFresh = FALSE;
	m_fsize = gph.m_fsize;
	m_bBold = gph.m_bBold;
	m_bItalic = gph.m_bItalic;
	m_bUnderLine = gph.m_bUnderLine;

	m_bDataOk = gph.m_bDataOk;
	m_pWndParent = gph.m_pWndParent;
	m_pWndTextEdit->InCreaseReference();
}

CText& CText::operator = (const CText& gph)
{
	CopyTheBaseDataMember(&gph);

	m_pFont = gph.m_pFont->Clone();
	m_strText = gph.m_strText;

	m_bFresh = FALSE;
	m_fsize = gph.m_fsize;
	m_bBold = gph.m_bBold;
	m_bItalic = gph.m_bItalic;
	m_bUnderLine = gph.m_bUnderLine;

	m_bDataOk = gph.m_bDataOk;
	m_pWndParent = gph.m_pWndParent;
	m_pWndTextEdit->InCreaseReference();

	return *this;
}

void CText::InitDataMember()
{
	m_bBold = FALSE;
	m_bFresh = FALSE;
	m_bItalic = FALSE;
	m_bDataOk = FALSE;
	m_bUnderLine = FALSE;
	m_strText = _T("");
	m_crColor = Color::Black;

	m_pFont = NULL;
	m_pWndParent = NULL;

	m_nMinCount = 2;
	m_ptary.SetSize(2, 2);
}

void CText::ShowEditBoxDefault()
{
	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYSCREEN);

	m_ptCenter.X = nWidth / 2.0f;
	m_ptCenter.Y = nHeight / 2.0f;

	m_rcHot.SetRect((int)m_ptCenter.X - 150,
		(int)m_ptCenter.Y - 50,
		(int)m_ptCenter.X + 150,
		(int)m_ptCenter.Y + 50);

	m_ptary[0].X = (float)m_rcHot.left;
	m_ptary[0].Y = (float)m_rcHot.top;
	m_ptary[1].X = (float)m_rcHot.right;
	m_ptary[1].Y = (float)m_rcHot.bottom;
	SetHotPoints();

	m_nPtCount = 2;
	m_bDataOk = TRUE;

	CRect rect = m_rcHot;
	rect.DeflateRect(7, 7);
	m_pWndTextEdit->MoveEditBox(rect);
	m_pWndTextEdit->ShowEditWindow(TRUE);
}

CText::~CText()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_pFont)
	{
		delete m_pFont;
		m_pFont = NULL;
	}

	if(m_pWndTextEdit)
	{
		m_pWndTextEdit->DeCreaseReference();
		if(m_pWndTextEdit->IsNoreference())
		{
			m_pWndTextEdit->DestroyWindow();
			delete m_pWndTextEdit;
			m_pWndTextEdit = NULL;
		}
	}
}

void CText::FinishGraph()
{
	if(m_nPtCount < 2 || m_strText.IsEmpty())
	{
		m_bFinish = FALSE;
		m_pWndTextEdit->ShowEditWindow(FALSE);
	}
	else
	{
		m_bFinish = TRUE;
		m_pWndTextEdit->ShowEditWindow(FALSE);
	}
}

void CText::HideEditBox()
{
	ASSERT(m_pWndTextEdit);
	m_pWndTextEdit->ShowEditWindow(FALSE);
}

void CText::SetTextFont(Font& fnt)
{
	delete m_pFont;
	m_pFont = fnt.Clone();
	InitFontData();
}

void CText::InitFontData()
{
	int nstyle = m_pFont->GetStyle();

	if(nstyle & FontStyleBold)
	{
		m_bBold = TRUE;
	}
	if(nstyle & FontStyleItalic)
	{
		m_bItalic = TRUE;
	}
	if(nstyle & FontStyleUnderline)
	{
		m_bUnderLine = TRUE;
	}

	m_fsize = m_pFont->GetSize();
}

void CText::SetFntFmy(FontFamily& FntFmy)
{
	int nstyle = m_pFont->GetStyle();
	m_fsize = m_pFont->GetSize();
	Font fnt(&FntFmy,m_fsize,nstyle);

	delete m_pFont;
	m_pFont = fnt.Clone();
}

void CText::SetFntSize(float fsize)
{
	m_fsize = fsize;
	ModifyFont();
}

void CText::SetBold(BOOL bBold)
{
	m_bBold = bBold;
	ModifyFont();
}

void CText::SetItalic(BOOL bItalic)
{
	m_bItalic = bItalic;
	ModifyFont();
}

void CText::SetUnderLine(BOOL bUnderLine)
{
	m_bUnderLine = bUnderLine;
	ModifyFont();
}

void CText::ModifyFont()
{
	int nstyle = 0;

	if(m_bBold)
	{
		nstyle |= FontStyleBold;
	}
	if(m_bItalic)
	{
		nstyle |= FontStyleItalic;
	}
	if(m_bUnderLine)
	{
		nstyle |= FontStyleUnderline;
	}
	
	FontFamily* pFontFamily = NULL;
	m_pFont->GetFamily(pFontFamily);

	Font fnt(pFontFamily, m_fsize, nstyle);
	SetTextFont(fnt);
}

CRect CText::FormUpdateRect()
{
	if(m_bFresh)
	{
		CRect rect;
		m_pWndTextEdit->GetWindowRect(&rect);
		rect.InflateRect(10, 10);
		m_bFresh = FALSE;
		return rect;
	}
	else
	{
		return __super::FormUpdateRect();
	}
}

void CText::AddPoint(UINT nMouseState,CPoint point, CRect& rcUpdate)
{
	if(!m_bDataOk)
	{
		FormGraphData(nMouseState, point, rcUpdate);
	}
	else
	{
		if(!m_bFinish)
		{
			DecideEditTextBoxType(nMouseState, point);
			ModifyGraph(nMouseState, point);

			if(MS_LBTNDOWN == nMouseState && !IsPtInModifyZone(point))
			{
				SetTextInfoFrmEditBox();
				return;
			}

			CRect rect = m_rcHot;
			rect.DeflateRect(5, 5);
			m_pWndTextEdit->MoveEditBox(rect);
		}
		else
		{
			if(MS_LBTNDOWN == nMouseState && !IsPtInModifyZone(point))
			{
				if(m_bEdit)
				{
					m_bFresh = TRUE;
				}

				AdjustGraph(m_fAngle);
				SetEditSate(FALSE);
				SetSelectState(FALSE);
				return;
			}

			DecideModiFyType(nMouseState, point);

			if(MT_MOVE == m_ModifyType || MT_ROTATE == m_ModifyType)
			{
				rcUpdate = FormUpdateRect();
			}

			ModifyGraph(nMouseState, point);

			if(m_bEdit)
			{
				CRect rect = m_rcHot;
				rect.DeflateRect(5, 5);
				m_pWndTextEdit->MoveEditBox(rect);
			}
		}

		if(MS_LBTNUP == nMouseState)
		{
			SetRegion();
		}
	}
}

void CText::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);

	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[0] = pt;
		m_ptary[1] = pt;
		m_nPtCount = 1;
	}
	else
	{
		if(m_nPtCount < 1)
		{
			return;
		}
		m_ptary[1] = pt;
		m_nPtCount = 2;
	}

	if(MS_LBTNUP == nMouseState)
	{
		if(m_nPtCount < 1)
		{
			return;
		}

		if(CalcGapOfpoints(m_ptary[0], m_ptary[1]) <= 5.0f)
		{
			PointF ptCenter;
			ptCenter.X = (m_ptary[0].X + m_ptary[1].X) / 2;
			ptCenter.Y = (m_ptary[0].Y + m_ptary[1].Y) / 2;

			m_ptary[0].X = ptCenter.X - 150;
			m_ptary[0].Y = ptCenter.Y - 50;
			m_ptary[1].X = ptCenter.X + 150;
			m_ptary[1].Y = ptCenter.Y + 50;
		}

		m_bDataOk = TRUE;
		InitHotData();
		rcUpdate = m_rcHot;
		rcUpdate.InflateRect(5, 5);

		CRect rect = m_rcHot;
		rect.DeflateRect(5, 5);
		m_pWndTextEdit->MoveEditBox(rect);
		m_pWndTextEdit->ShowEditWindow(TRUE);
	}
}

void CText::InitHotData()
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	m_rcHot.left = (LONG)min(m_ptary[0].X, m_ptary[1].X);
	m_rcHot.top = (LONG)min(m_ptary[0].Y, m_ptary[1].Y);
	m_rcHot.right = (LONG)max(m_ptary[0].X, m_ptary[1].X);
	m_rcHot.bottom = (LONG)max(m_ptary[0].Y, m_ptary[1].Y);

	SetHotPoints();
	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	SetRegion();
}

void CText::Draw(CDC* pDC, CRect& rcInvalid)
{
	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	if(!m_bDataOk)
	{
		Pen penDraw(Color::Blue, 2);
		penDraw.SetDashStyle(DashStyleDash);

		CPoint ptLT,ptRB;
		ptLT.x = (int)m_ptary[0].X;
		ptLT.y = (int)m_ptary[0].Y;
		ptRB.x = (int)m_ptary[1].X;
		ptRB.y = (int)m_ptary[1].Y;
		CRect rc(ptLT,ptRB);
		rc.NormalizeRect();
		graph.DrawRectangle(&penDraw, rc.left, rc.top, rc.Width(), rc.Height());
	}
	else if(!m_bFinish)
	{
		CPoint ptLT, ptRB;
		ptLT.x = (int)m_ptary[0].X;
		ptLT.y = (int)m_ptary[0].Y;
		ptRB.x = (int)m_ptary[1].X;
		ptRB.y = (int)m_ptary[1].Y;
		Pen penDraw(Color::Green, 2);
		penDraw.SetDashStyle(DashStyleDash);
		CRect rc(ptLT,ptRB);
		rc.NormalizeRect();
		graph.DrawRectangle(&penDraw, rc.left, rc.top, rc.Width(), rc.Height());

		DrawEditBoxHotPoints(graph);
	}
	else
	{
		if(m_bEdit)
		{
			CPoint ptLT,ptRB;
			ptLT.x = (int)m_ptary[0].X;
			ptLT.y = (int)m_ptary[0].Y;
			ptRB.x = (int)m_ptary[1].X;
			ptRB.y = (int)m_ptary[1].Y;
			Pen penDraw(Color::Green, 2);
			penDraw.SetDashStyle(DashStyleDash);
			CRect rc(ptLT, ptRB);
			rc.NormalizeRect();
			RectF rcf = CRect2RectF(rc);
			graph.DrawRectangle(&penDraw, rcf);
			DrawEditBoxHotPoints(graph);
			return;
		}
		else
		{
			Matrix mat;
			mat.RotateAt(m_fAngle, m_ptCenter);
			graph.SetTransform(&mat);

			if(m_bSelected)
			{
				DrawHotShape(graph);
			}

			SolidBrush brush(m_crColor);
			StringFormat format;
			format.SetAlignment(StringAlignmentNear);

			graph.ExcludeClip(&m_RgnErase);
			graph.SetTextRenderingHint(TextRenderingHintAntiAlias);

			RectF rcf = CRect2RectF(m_rcHot);
			graph.DrawString(m_strText, m_strText.GetLength(), m_pFont, rcf, &format, &brush);
		}
	}
}

void CText::SetRegion()
{
	if(!m_bFinish)
	{
		return;
	}

	RectF rcf((float)m_rcHot.left, (float)m_rcHot.top,
		(float)m_rcHot.Width(), (float)m_rcHot.Height());

	m_Region.MakeEmpty();
	m_Region.Union(rcf);
}

void CText::SetHotData(CRect rect)
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	m_rcHot = rect;

	m_ptary[0].X = (float)m_rcHot.left;
	m_ptary[0].Y = (float)m_rcHot.top;
	m_ptary[1].X = (float)m_rcHot.right;
	m_ptary[1].Y = (float)m_rcHot.bottom;

	SetHotPoints();
	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	SetRegion();
}

void CText::SetTextInfo(CString strText, Font& fnt)
{
	if(strText.GetLength() <= 0)
	{
		return;
	}

	m_bFinish = TRUE;
	m_strText = strText;

	delete m_pFont;
	m_pFont = fnt.Clone();
	InitFontData();
}

ModifyType CText::AroundHotPts(CPoint point)
{
	CRect rect = m_rcHot;
	rect.InflateRect(7, 7);

	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	if(CalcGapOfpoints(pt, m_HotPts.ptLT) <= HOTINTERVAL)
	{
		return MT_LT;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptTC) <= HOTINTERVAL)
	{
		return MT_TC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRT) <= HOTINTERVAL)
	{
		return MT_RT;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptLC) <= HOTINTERVAL)
	{
		return MT_LC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRC) <= HOTINTERVAL)
	{
		return MT_RC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptLB) <= HOTINTERVAL)
	{
		return MT_LB;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptBC) <= HOTINTERVAL)
	{
		return MT_BC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRB) <= HOTINTERVAL)
	{
		return MT_RB;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		if(m_bFinish && !m_bEdit)
		{
			return MT_ROTATE;
		}
	}
	else if(rect.PtInRect(CPoint((int)pt.X, (int)pt.Y)))
	{
		return MT_MOVE;
	}

	return MT_NONE;
}

void CText::DrawEditBoxHotPoints(Graphics& graph)
{
	Pen penDraw(Color::Green, 2.0f);
	graph.DrawRectangle(&penDraw, m_HotPts.ptBC.X - 3.0f, m_HotPts.ptBC.Y - 3.0f, 6.0f, 6.0f);
	graph.DrawRectangle(&penDraw, m_HotPts.ptLB.X - 3.0f, m_HotPts.ptLB.Y - 3.0f, 6.0f, 6.0f);
	graph.DrawRectangle(&penDraw, m_HotPts.ptLC.X - 3.0f, m_HotPts.ptLC.Y - 3.0f, 6.0f, 6.0f);
	graph.DrawRectangle(&penDraw, m_HotPts.ptLT.X - 3.0f, m_HotPts.ptLT.Y - 3.0f, 6.0f, 6.0f);
	graph.DrawRectangle(&penDraw, m_HotPts.ptRB.X - 3.0f, m_HotPts.ptRB.Y - 3.0f, 6.0f, 6.0f);
	graph.DrawRectangle(&penDraw, m_HotPts.ptRC.X - 3.0f, m_HotPts.ptRC.Y - 3.0f, 6.0f, 6.0f);
	graph.DrawRectangle(&penDraw, m_HotPts.ptRT.X - 3.0f, m_HotPts.ptRT.Y - 3.0f, 6.0f, 6.0f);
	graph.DrawRectangle(&penDraw, m_HotPts.ptTC.X - 3.0f, m_HotPts.ptTC.Y - 3.0f, 6.0f, 6.0f);
}

CString CText::GetTextString()
{
	return m_strText;
}

void CText::DecideEditTextBoxType(UINT nMouseState, CPoint point)
{
	if(nMouseState != MS_LBTNDOWN)
	{
		return;
	}

	PointF pt((float)point.x, (float)point.y);
	CRect rect = m_rcHot;
	rect.InflateRect(7, 7);

	if(CalcGapOfpoints(pt, m_HotPts.ptLT) <= HOTINTERVAL)
	{
		m_ModifyType = MT_LT;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptTC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_TC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRT) <= HOTINTERVAL)
	{
		m_ModifyType = MT_RT;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptLC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_LC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_RC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptLB) <= HOTINTERVAL)
	{
		m_ModifyType = MT_LB;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptBC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_BC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRB) <= HOTINTERVAL)
	{
		m_ModifyType = MT_RB;
	}
	else if(rect.PtInRect(CPoint((int)pt.X, (int)pt.Y)))
	{
		m_ModifyType = MT_MOVE;
		m_ptModifyOrg.X = (float)point.x;
		m_ptModifyOrg.Y = (float)point.y;
	}
	else
	{
		m_ModifyType = MT_NONE;
	}
}

Font* CText::GetTextFont()
{
	return m_pFont;
}


BOOL CText::CreateEditTextBox(CWnd* pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(!m_pWndTextEdit)
	{
		m_pWndTextEdit = new CTextEditWnd;

		if(!m_pWndTextEdit->Create(ES_MULTILINE | ES_AUTOVSCROLL | WS_CHILD,
			CRect(0, 0, 0, 0), pParent, 0))
		{
			return FALSE;
		}
	}

	m_pWndTextEdit->SetTextGraph(this);
	m_pWndTextEdit->SetParentWnd(pParent);
	m_pWndTextEdit->InCreaseReference();

	return TRUE;
}

void CText::SetEditSate(BOOL bEdit)
{
	ASSERT(m_pWndTextEdit);
	ASSERT(m_pWndParent);
	m_pWndTextEdit->SetParentWnd(m_pWndParent);

	if(bEdit)
	{
		SaveTheRotateAngle();
		//get the text infomation
		Font* pFont = GetTextFont();
		COLORREF clr = m_crColor.ToCOLORREF();

		HDC hdc = ::GetDC(NULL);
		Graphics graph(hdc);
		LOGFONT lf = {0};
		pFont->GetLogFontW(&graph, &lf);
		::ReleaseDC(NULL, hdc);
		
		CFont font;
		font.CreateFontIndirect(&lf);

		//move editbox
		CRect rect = m_rcHot;
		rect.DeflateRect(5, 5);

		m_pWndTextEdit->SetTextGraph(this);
		m_pWndTextEdit->SetParentWnd(m_pWndParent);
		m_pWndTextEdit->MoveEditBox(rect);
		m_pWndTextEdit->ShowEditWindow(TRUE);
		
		//set the edit box parameters
		m_pWndTextEdit->SetTextFont(font);
		m_pWndTextEdit->SetWindowText(m_strText);
	}
	else if(m_bEdit)
	{
		RecoverTheRotateAngle();
		SetTextInfoFrmEditBox();
	}

	m_bEdit = bEdit;
}

void CText::SetTextInfoFrmEditBox()
{
	LOGFONT lf = {0};
	m_pWndTextEdit->m_Font.GetLogFont(&lf);

	HDC hdc = ::GetDC(NULL);
	Font font(hdc, &lf);
	::ReleaseDC(NULL, hdc);

	delete m_pFont;
	m_pFont = font.Clone();
	InitFontData();

	CSize size = m_pWndTextEdit->GetTextSize();
	m_pWndTextEdit->GetWindowText(m_strText);

	if(m_strText.GetLength() == 0)
	{
		m_bDataInvalid = TRUE;
	}
	else
	{
		m_bFinish = TRUE;
	}

	SetHotData(m_rcHot);
	m_pWndTextEdit->ShowEditWindow(FALSE);
}

void CText::UpdateTextBorder(CRect rect)
{
	rect.InflateRect(5, 5);
	SetHotData(rect);
	m_pWndParent->Invalidate();
}

void CText::SetColor(Color crColor)
{
	__super::SetColor(crColor);
	ASSERT(m_pWndTextEdit && m_pWndTextEdit->m_hWnd);
	m_pWndTextEdit->Invalidate();
}
