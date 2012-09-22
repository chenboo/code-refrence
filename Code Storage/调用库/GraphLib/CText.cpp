#include "stdafx.h"
#include "CText.h"

CText::CText()
{
	m_pFont = NULL;
	m_pFntFmy = NULL;
	m_bBold = FALSE;
	m_bItalic = FALSE;
	m_bUnderLine = FALSE;
	m_blbtndown = FALSE;
	m_bComplete = FALSE;

	m_ptary.SetSize(2,2);
	m_nMinCount = 2;
}

CText::CText(Font* pfnt,CString strText)
{
	m_pFntFmy = new FontFamily;
	m_pFont = pfnt->Clone();
	InitFontData();
	SetTextString(strText);

	m_ptary.SetSize(2,2);
	m_nMinCount = 2;
}

CText::CText(const CText& gph)
{
	CopyBase(&gph);	

	m_ptary.SetSize(3,1);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
	
	m_pFont = gph.m_pFont->Clone();
	m_pFntFmy = gph.m_pFntFmy->Clone();
	m_strText = gph.m_strText;

	m_fsize = gph.m_fsize;
	m_bBold = gph.m_bBold;
	m_bItalic = gph.m_bItalic;
	m_bUnderLine = gph.m_bUnderLine;

	m_bComplete = gph.m_bComplete;
	m_blbtndown = gph.m_blbtndown;
	m_rclayout = gph.m_rclayout;
}

CText::~CText()
{
	delete m_pFont;
	delete m_pFntFmy;
}

void CText::InitFontData()
{
	m_pFont->GetFamily(m_pFntFmy);

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

void CText::SetTextFont(Font& fnt)
{
	delete m_pFont;
	m_pFont = fnt.Clone();
	InitFontData();
}

void CText::SetFntFmy(FontFamily& FntFmy)
{
	delete m_pFntFmy;
	m_pFntFmy = FntFmy.Clone();
	ModifyFont();
}

void CText::SetTextString(CString strText,RectF rclayout)
{
	if(strText.GetLength() <= 0)
	{
		return;
	}
	m_strText = strText;
	m_rclayout = rclayout;
	InitRegion();
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

	Font fnt(m_pFntFmy,m_fsize,nstyle);
	SetTextFont(fnt);
}

void CText::AddPoint(UINT nMouseState,CPoint point)
{
	if(!m_bComplete)
	{
		FormGraphData(nMouseState,point);
	}
	else
	{
		DeceideModiFyType(nMouseState,point);
		ModifyGraph(nMouseState,point);
	}
}

void CText::FinishGraph()
{
	if(m_strText.GetLength() <= 0 || m_nPtCount <= 0)
	{
		return;
	}
	m_bFinish = TRUE;
	InitHotData();
	InitRegion();
	return;
}

void CText::FormGraphData(UINT nMouseState,CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN == nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
		m_blbtndown = TRUE;
	}
	else if(m_blbtndown)
	{
		m_ptary[1] = pt;
		m_nPtCount = 2;
		if(MS_LBTNUP == nMouseState)
		{
			InitHotData();
			m_bComplete = TRUE;
		}
	}
}

void CText::Draw(CDC* pDC)
{
	if(m_nPtCount <= 1)
		return;
		
	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	CPoint ptLT;
	CPoint ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;
	CRect rc(ptLT,ptRB);
	rc.NormalizeRect();

	if(!m_bFinish)
	{
		SolidBrush sbrush(Color::Green);
		Pen penDraw(Color::Blue,2);

		if(!m_bComplete)
		{
			penDraw.SetDashStyle(DashStyleDot);
		}

		graph.DrawRectangle(&penDraw,rc.left,rc.top,rc.Width(),rc.Height());
	}
	else if(m_bFinish)
	{
		RectF rcf((float)m_rcHot.left,(float)m_rcHot.top,
			(float)m_rcHot.Width(),(float)m_rcHot.Height());

		Matrix mat;
		mat.RotateAt(m_fAngle,m_ptCenter);
		graph.SetTransform(&mat);

		SolidBrush brush(m_crColor);
		StringFormat format;
		format.SetAlignment(StringAlignmentNear);

		graph.SetTextRenderingHint(TextRenderingHintAntiAlias);
		graph.DrawString(m_strText,m_strText.GetLength(),m_pFont,rcf,&format,&brush);

		if(m_bSelected)
		{
			DrawHotShape(graph);
		}
	}
}

void CText::SetRegion()
{
	if(!m_bFinish)
	{
		return;
	}

	RectF rcf((float)m_rcHot.left,(float)m_rcHot.top,
		(float)m_rcHot.Width(),(float)m_rcHot.Height());

	Pen greenPen(Color(255, 0, 255,  0),(float)m_nWidth);
	GraphicsPath path;

	StringFormat format;
	format.SetAlignment(StringAlignmentNear);
	FontFamily fontFamily;
	m_pFont->GetFamily(&fontFamily);
	int		nstyle = m_pFont->GetStyle();
	float	fsize = m_pFont->GetSize();

	path.AddString(m_strText, m_strText.GetLength(),&fontFamily, 
		nstyle, fsize, rcf,&format);

	path.Widen(&greenPen);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CText::SetTextString(CString strText)
{

}

void CText::InitRegion()
{
	if(!m_bFinish)
	{
		return;
	}
	Pen greenPen(Color(255, 0, 255,  0), (float)m_nWidth);
	GraphicsPath path;

	StringFormat format;
	format.SetAlignment(StringAlignmentNear);
	FontFamily fontFamily;
	m_pFont->GetFamily(&fontFamily);
	int		nstyle = m_pFont->GetStyle();
	float	fsize = m_pFont->GetSize();

	path.AddString(m_strText, m_strText.GetLength(),&fontFamily, 
		nstyle, fsize, m_rclayout,&format);

	path.Widen(&greenPen);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CText::InitHotData(BOOL bExtend)
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	m_rcHot.left = (LONG)min(m_ptary[0].X,m_ptary[1].X);
	m_rcHot.top = (LONG)min(m_ptary[0].Y,m_ptary[1].Y);
	m_rcHot.right = (LONG)max(m_ptary[0].X,m_ptary[1].X);
	m_rcHot.bottom = (LONG)max(m_ptary[0].Y,m_ptary[1].Y);

	SetHotPoints();
	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
	SetRegion();
}