#include "stdafx.h"
#include "CCircle.h"

//#pragma warning (disable : 4244)

CCircle::CCircle(int nCStyle,int nWidth,Color crColor)
{
	m_nCStyle = nCStyle;
	m_nWidth = nWidth;
	m_crColor = crColor;

	m_ptary.SetSize(2,2);
	m_nPtCount = 0;
	m_nMinCount = 2;
	m_fRadius = 0;
	m_ptCenter.X = 0.0;
	m_ptCenter.Y = 0.0;
}

CCircle::CCircle()
{

}

CCircle::CCircle(const CCircle& gph)
{
	CopyBase(&gph);

	m_ptary.RemoveAll();
	m_ptary.SetSize(2,2);
	m_ptary.Copy(gph.m_ptary);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
	
	m_nCStyle = gph.m_nCStyle;		
	m_fRadius = gph.m_fRadius;	
}

CCircle::~CCircle()
{
}

void CCircle::AddPoint(UINT nMouseState,CPoint point)
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

void CCircle::SetCircleStyle(int nCStyle)
{
	m_nCStyle = nCStyle;
}

void CCircle::Draw(CDC* pDC)
{
	if(m_nPtCount <= 0)
		return;

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	if(m_nPtCount == 1)
	{
		return;
	}

	CPoint ptLT;
	CPoint ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;

	CRect rc(ptLT,ptRB);
	rc.NormalizeRect();

	Pen penDraw(m_crColor,(float)m_nWidth);
	graph.DrawEllipse(&penDraw,rc.left,rc.top,rc.Width(),rc.Height());

	if(m_bSelected)
	{
		DrawHotShape(graph);
	}
}


void CCircle::FormGraphData(UINT nMouseState,CPoint point)
{
	PointF pt((float)point.x,(float)point.y);

	if(MS_LBTNDOWN	== nMouseState)
	{
		if(CS_CIRCLE == m_nCStyle)
		{
			m_ptCenter = pt;
		}
		else
		{
			m_ptary[0] = pt;
		}
		m_nPtCount = 1;
	}
	else
	{
		if(CS_CIRCLE == m_nCStyle)
		{
			m_fRadius = CalGapOfpoints(m_ptCenter,pt);
			m_ptary[0].X = m_ptCenter.X - m_fRadius;
			m_ptary[0].Y = m_ptCenter.Y - m_fRadius;
			m_ptary[1].X = m_ptCenter.X + m_fRadius;
			m_ptary[1].Y = m_ptCenter.Y + m_fRadius;
		}
		else
		{
			m_ptary[1] = pt;
		}
		if(MS_LBTNUP == nMouseState)
		{
			m_bFinish = TRUE;
			NormalizeData();
			InitHotData();
		}
		m_nPtCount = 2;
	}
}

void CCircle::FinishGraph()
{
	m_bFinish = TRUE;
	InitHotData();
	return;
}

void CCircle::NormalizeData()
{
	CPoint ptLT;
	CPoint ptRB;
	ptLT.x = (int)m_ptary[0].X;
	ptLT.y = (int)m_ptary[0].Y;
	ptRB.x = (int)m_ptary[1].X;
	ptRB.y = (int)m_ptary[1].Y;

	CRect rct(ptLT,ptRB);
	rct.NormalizeRect();

	m_ptary[0].X = (float)rct.left;
	m_ptary[0].Y = (float)rct.top;
	m_ptary[1].X = (float)rct.right;
	m_ptary[1].Y = (float)rct.bottom;
}

void CCircle::SetRegion()
{
	CRect rc((int)m_ptary[0].X,(int)m_ptary[0].Y,
		(int)m_ptary[1].X,(int)m_ptary[1].Y);
	rc.NormalizeRect();

	int nWidth = m_nWidth;
	if(m_nWidth < 6)
	{
		nWidth = 6;
	}

	Pen greenPen(Color(255, 0, 255,  0), (float)nWidth);
	GraphicsPath path;
	path.AddEllipse(rc.left,rc.top,rc.Width(),rc.Height());
	path.Widen(&greenPen);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}
