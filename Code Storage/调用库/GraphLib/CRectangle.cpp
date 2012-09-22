#include "stdafx.h"
#include "CRectangle.h"

CRectangle::CRectangle(int nRStyle,int nWidth,Color crColor)
{
	m_nRStyle = nRStyle;
	m_nWidth = nWidth;
	m_crColor = crColor;
	
	m_ptary.SetSize(2,2);
	m_nPtCount = 0;
	m_nMinCount = 2;
}

CRectangle::CRectangle()
{

}

CRectangle::CRectangle(const CRectangle& gph)
{
	CopyBase(&gph);

	m_ptary.SetSize(0,4);
	m_ptary.RemoveAll();
	m_ptary.Copy(gph.m_ptary);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
	
	m_nRStyle = gph.m_nRStyle;
}

CRectangle::~CRectangle()
{
}

void CRectangle::AddPoint(UINT nMouseState,CPoint point)
{
	if(!m_bFinish)
	{
		FormGraphData(nMouseState,point);
	}
	else
	{
		DeceideModiFyType(nMouseState,point);
		ModifyGraph(nMouseState,point);
	}
}

void CRectangle::SetRCStyle(int nRStyle)
{
	m_nRStyle = nRStyle;
}

void CRectangle::Draw(CDC* pDC)
{
	if(m_nPtCount <= 0)
		return;
		
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

	if(m_bSelected)
	{
		DrawHotShape(graph);
	}
}

void CRectangle::DrawRoundRect(Graphics& graph,CRect rc)
{
	Pen penDraw(m_crColor,(float)m_nWidth);
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
	graph.DrawRectangle(&penDraw,rc.left,rc.top,rc.Width(),rc.Height());
}

void CRectangle::FormGraphData(UINT nMouseState,CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
	}
	else
	{
		m_ptary[1] = pt;
		m_nPtCount = 2;
		if(MS_LBTNUP == nMouseState)
		{
			m_bFinish = TRUE;
			NormalizeData();
			InitHotData();
		}
	}
}

void CRectangle::SetRegion()
{
	float left = min(m_ptary[0].X,m_ptary[1].X);
	float top = min(m_ptary[0].Y,m_ptary[1].Y);
	float width = fabs(m_ptary[0].X - m_ptary[1].X);
	float height = fabs(m_ptary[0].Y - m_ptary[1].Y);
	/*PointF* pPt = m_ptary.GetData();*/

	int nWidth = m_nWidth;
	if(m_nWidth < 6)
	{
		nWidth = 6;
	}

	Pen greenPen(Color(255, 0, 255,  0), (float)nWidth);
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

	path.Widen(&greenPen);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CRectangle::FinishGraph()
{
	m_bFinish = TRUE;
	InitHotData();
	return;
}

void CRectangle::NormalizeData()
{
	CPoint ptLT;
	CPoint ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;
	CRect rct(ptLT,ptRB);
	rct.NormalizeRect();

	if(RS_SQUARE == m_nRStyle)
	{
		float fWidth = (float)min(rct.Width(),rct.Height());
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