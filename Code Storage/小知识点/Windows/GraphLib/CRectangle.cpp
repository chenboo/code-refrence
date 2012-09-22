#include "stdafx.h"
#include "GraphLib.h"

CRectangle::CRectangle(int nRStyle, int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nRStyle = (RectStyle)nRStyle;
	m_nWidth = nWidth;
	m_crColor = crColor;
	
	m_ptary.SetSize(2,2);
	m_nPtCount = 0;
	m_nMinCount = 2;
	
	m_nDashStyle = dashstyle;
}

CRectangle::CRectangle()
{

}

CRectangle::CRectangle(const CRectangle& gph)
{
	CopyTheBaseDataMember(&gph);
	m_nRStyle = gph.m_nRStyle;
}

CRectangle & CRectangle::operator = (const CRectangle& gph)
{
	CopyTheBaseDataMember(&gph);
	m_nRStyle = gph.m_nRStyle;
	return *this;
}

CRectangle::~CRectangle()
{
}

void CRectangle::SetRCStyle(int nRStyle)
{
	m_nRStyle = (RectStyle)nRStyle;
}

void CRectangle::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount <= 0)
	{
		return;
	}
		
	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	CPoint ptLT;
	CPoint ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;
	CRect rc(ptLT,ptRB);
	rc.NormalizeRect();

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}

	graph.ExcludeClip(&m_RgnErase);

	if(m_nPtCount == 1)
	{
		return;
	}
	if(RS_ROUND == m_nRStyle)
	{
		DrawRoundRect(graph,rc);
	}
	else if(RS_SQUARE == m_nRStyle)
	{
		DrawSquare(graph,rc);
	}
	else if(RS_RECT == m_nRStyle)
	{
		DrawRectangle(graph,rc);
	}
}

void CRectangle::DrawRoundRect(Graphics& graph,CRect rc)
{
	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	INT offsetX = (rc.right - rc.left) * 20/100;
	INT offsetY = (rc.bottom - rc.top) * 20/100;
	GraphicsPath pt;
	pt.AddArc(rc.right - offsetX, rc.top, offsetX, offsetY, 270, 90);
	pt.AddArc(rc.right - offsetX, rc.bottom - offsetY, offsetX, offsetY, 0, 90);
	pt.AddArc(rc.left, rc.bottom - offsetY, offsetX, offsetY, 90, 90);
	pt.AddArc(rc.left, rc.top, offsetX, offsetY, 180, 90);
	pt.AddLine(rc.left + offsetX, rc.top, rc.right - offsetX/2, rc.top);

	graph.DrawPath(&penDraw,&pt);
}

void CRectangle::DrawSquare(Graphics& graph,CRect rc)
{
	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	if(m_bFinish)
	{
		graph.DrawRectangle(&penDraw,rc.left,rc.top,rc.Width(),rc.Height());
	}
	else
	{
		float fWidth = (float)min(rc.Width(),rc.Height());
		float fleft = 0,ftop = 0;
		if(m_ptary[0].X <= m_ptary[1].X)
		{
			fleft = m_ptary[0].X;
		}
		else 
		{
			fleft = m_ptary[0].X - fWidth;
		}
		if(m_ptary[0].Y <= m_ptary[1].Y)
		{
			ftop = m_ptary[0].Y;
		}
		else
		{
			ftop = m_ptary[0].Y - fWidth;
		}

		graph.DrawRectangle(&penDraw,fleft,ftop,fWidth,fWidth);
	}
}

void CRectangle::DrawRectangle(Graphics& graph,CRect rc)
{
	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetDashStyle(m_nDashStyle);
	graph.DrawRectangle(&penDraw,rc.left,rc.top,rc.Width(),rc.Height());
}

void CRectangle::FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN == nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
	}
	else if(m_nPtCount >= 1)
	{
		m_ptary[1] = pt;
		m_nPtCount = 2;

		if(MS_LBTNUP == nMouseState)
		{
			m_bFinish = TRUE;
			NormalizeData();
			InitHotData();
			rcUpdate = m_rcHot;
		}
	}
	else
	{
		rcUpdate.SetRect(0, 0, 1, 1);
	}
}

void CRectangle::NormalizeData()
{
	CPoint ptLT, ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;
	CRect rct(ptLT,ptRB);
	rct.NormalizeRect();

	CPoint ptCenter = rct.CenterPoint();

	if((rct.Width() < 5 || rct.Height() < 5)
		&& RS_SQUARE == m_nRStyle)
	{
		m_ptary[0].X = (float)(ptCenter.x - 20);
		m_ptary[0].Y = (float)(ptCenter.y - 20);

		m_ptary[1].X = (float)(ptCenter.x + 20);
		m_ptary[1].Y = (float)(ptCenter.y + 20);

		return;
	}
	else if(rct.Width() < 5 || rct.Height() < 5)
	{
		m_ptary[0].X = (float)(ptCenter.x - 20);
		m_ptary[0].Y = (float)(ptCenter.y - 30);

		m_ptary[1].X = (float)(ptCenter.x + 20);
		m_ptary[1].Y = (float)(ptCenter.y + 30);

		return;
	}

	if(RS_SQUARE == m_nRStyle)
	{
		float fleft = 0, ftop = 0;
		float fWidth = (float)min(rct.Width(),rct.Height());
		
		if(m_ptary[0].X <= m_ptary[1].X)
		{
			fleft = m_ptary[0].X;
		}
		else 
		{
			fleft = m_ptary[0].X - fWidth;
		}

		if(m_ptary[0].Y <= m_ptary[1].Y)
		{
			ftop = m_ptary[0].Y;
		}
		else
		{
			ftop = m_ptary[0].Y - fWidth;
		}

		m_ptary[0].X = fleft;
		m_ptary[0].Y = ftop;
		m_ptary[1].X = m_ptary[0].X + fWidth;
		m_ptary[1].Y = m_ptary[0].Y + fWidth;
	}
	else
	{
		m_ptary[0].X = (float)rct.left;
		m_ptary[0].Y = (float)rct.top;
		m_ptary[1].X = (float)rct.right;
		m_ptary[1].Y = (float)rct.bottom;
	}
}

void CRectangle::SetRegion()
{
	float left = min(m_ptary[0].X,m_ptary[1].X);
	float top = min(m_ptary[0].Y,m_ptary[1].Y);
	float width = fabs(m_ptary[0].X - m_ptary[1].X);
	float height = fabs(m_ptary[0].Y - m_ptary[1].Y);

	Pen penDraw(Color(255, 0, 255, 0));
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);
	
	GraphicsPath path;
	if(RS_ROUND == m_nRStyle)
	{
		CRect rc((int)left,(int)top,(int)(left + width),(int)(top + height));
		INT offsetX = (rc.right - rc.left) * 20/100;  
		INT offsetY = (rc.bottom - rc.top) * 20/100;
		path.AddArc(rc.right - offsetX, rc.top, offsetX, offsetY, 270, 90);
		path.AddArc(rc.right - offsetX, rc.bottom - offsetY, offsetX, offsetY, 0, 90);
		path.AddArc(rc.left, rc.bottom - offsetY, offsetX, offsetY, 90, 90);
		path.AddArc(rc.left, rc.top, offsetX, offsetY, 180, 90);
		path.AddLine(rc.left + offsetX, rc.top, rc.right - offsetX/2, rc.top);
	}
	else
	{

		RectF rcf(left,top,width,height);
		path.AddRectangle(rcf);
	}

	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CRectangle::FinishGraph()
{
	if(2 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}
