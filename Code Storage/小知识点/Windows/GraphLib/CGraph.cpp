#include "stdafx.h"
#include "GraphLib.h"
#include <math.h>

CGraph::CGraph()
{
	m_nPtCount = 0;
	m_fAngle = 0.0f;
	m_fScale = 1.0f;
	m_nOrgWidth = 0;
	m_nOrgHeight = 0;

	m_fOldAngle = 0.0f;
	m_ptCenter.X = 0.0f;
	m_ptCenter.Y = 0.0f;
	m_ModifyType = MT_NONE;
	m_rcHot.SetRectEmpty();

	m_pSubGraph = NULL;
	m_nSubType = DT_NONE;

	m_bEdit = FALSE;
	m_bErased = FALSE;
	m_bFinish = FALSE;
	m_bSelected = FALSE;
	m_bDataInvalid = FALSE;
	m_nDashStyle = DashStyleSolid;

	m_Region.MakeEmpty();
	m_RgnErase.MakeEmpty();
}

CGraph& CGraph::operator = (const CGraph& gph)
{
	SAFE_DELETE(m_pSubGraph);
	CopyTheBaseDataMember(&gph);
	return *this;
}

CGraph::CGraph(const CGraph& gph)
{
	CopyTheBaseDataMember(&gph);
}

void CGraph::FormGraphDirect(POINTARRAY& ptarray)
{
	m_ptary.RemoveAll();
	m_ptary.Copy(ptarray);
	m_nPtCount = (int)m_ptary.GetCount();

	m_bFinish = TRUE;
	InitHotData();
}

void CGraph::ResetSubGraph()
{
	SAFE_DELETE(m_pSubGraph);
	m_nSubType = DT_NONE;
}

void CGraph::CopyTheBaseDataMember(const CGraph* pgph)
{
	m_pSubGraph = NULL;
	m_nSubType = DT_NONE;

	m_nWidth = pgph->m_nWidth;
	m_crColor = pgph->m_crColor;
	m_nPtCount = pgph->m_nPtCount;
	m_nMinCount = pgph->m_nMinCount;
	m_ModifyType = pgph->m_ModifyType;

	m_rcHot = pgph->m_rcHot;
	m_HotPts = pgph->m_HotPts;
	
	m_fAngle = pgph->m_fAngle;
	m_ptCenter = pgph->m_ptCenter;
	m_fOldAngle = pgph->m_fOldAngle;
	m_ptModifyOrg = pgph->m_ptModifyOrg;

	m_Region.MakeEmpty();
	m_Region.Union(&(pgph->m_Region));

	m_RgnErase.MakeEmpty();
	m_RgnErase.Union(&(pgph->m_RgnErase));

	m_bErased = pgph->m_bErased;
	m_bFinish = pgph->m_bFinish;
	m_bSelected = pgph->m_bSelected;

	m_bEdit = pgph->m_bEdit;
	m_bDataInvalid = pgph->m_bDataInvalid;

	m_ptary.RemoveAll();
	m_ptary.Copy(pgph->m_ptary);

	m_fScale = pgph->m_fScale;
	m_nOrgWidth = pgph->m_nOrgWidth;
	m_nOrgHeight = pgph->m_nOrgHeight;
	m_nDashStyle = pgph->m_nDashStyle;
}

CGraph::~CGraph()
{
	SAFE_DELETE(m_pSubGraph);
}

void CGraph::SetWidth(int nWidth)
{
	if(m_bFinish)
	{
		float fscale = (float)nWidth / m_nWidth;

		int nflate = 0;

		if(fscale >= 1)
		{
			nflate = (int)(HOTINTERVAL * fscale - HOTINTERVAL);
		}
		else
		{
			fscale = 1.0f / fscale;
			nflate = (int)(HOTINTERVAL - HOTINTERVAL * fscale);
		}

		int nleft, ntop, nright, nbottom;
		nleft = ntop = nright = nbottom = 0;

		for(int i = 0;i < m_nPtCount;i++)
		{
			if(nleft > (int)m_ptary[i].X)
			{
				nleft = (int)m_ptary[i].X;
			}
			else if(nright < (int)m_ptary[i].X)
			{
				nright = (int)m_ptary[i].X;
			}
			if(ntop > (int)m_ptary[i].Y)
			{
				ntop = (int)m_ptary[i].Y;
			}
			else if(nbottom < (int)m_ptary[i].Y)
			{
				nbottom = (int)m_ptary[i].Y;
			}
		}

		if(m_rcHot.left - nflate > nleft - HOTINTERVAL)
		{
			nflate = 0;
		}
		if(m_rcHot.left + nflate < nright + HOTINTERVAL)
		{
			nflate = 0;
		}
		if(m_rcHot.top - nflate > ntop - HOTINTERVAL)
		{
			nflate = 0;
		}
		if(m_rcHot.bottom + nflate < nbottom + HOTINTERVAL)
		{
			nflate = 0;
		}

		m_rcHot.InflateRect(nflate, nflate);
		m_nWidth = nWidth;

		SetRegion();
		SetHotPoints();
	}
	else
	{
		m_nWidth = nWidth;
	}
}

void CGraph::SetColor(Color crColor)
{
	m_crColor = crColor;
}

void CGraph::SetLineDashStyle(DashStyle dashStyle)
{
	m_nDashStyle = dashStyle;

	if(m_bFinish)
	{
		SetRegion();
	}
}

void CGraph::SetEditSate(BOOL bEdit)
{
	m_bEdit = bEdit;
}

void CGraph::AddPoint(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	if(!m_bFinish)
	{
		FormGraphData(nMouseState, point, rcUpdate);
	}
	else
	{
		if(MS_LBTNDOWN == nMouseState && !IsPtInModifyZone(point))
		{
			AdjustGraph(m_fAngle);
			SetRegion();
			SetSelectState(FALSE);
			SetEditSate(FALSE);
			return;
		}

		DecideModiFyType(nMouseState, point);
		if(MT_MOVE == m_ModifyType || MT_ROTATE == m_ModifyType)
		{
			rcUpdate = FormUpdateRect();
		}

		ModifyGraph(nMouseState, point);

		if(MS_LBTNUP == nMouseState)
		{
			SetRegion();
		}
	}
}

void CGraph::PrePareBufferDC(CDC* pDCMem, CDC* pDCOrg, CRect& rect)
{
	pDCMem->BitBlt(0, 0, rect.Width(), rect.Height(), pDCOrg, 0, 0, SRCCOPY);
}

void CGraph::TransformHotDataToWnd(int nleft, int ntop)
{
	m_rcHot.OffsetRect(- nleft, - ntop);
	SetHotPoints();

	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	SetRegion();
}

void CGraph::TransformHotDataToScreen(int nleft, int ntop)
{
	m_rcHot.OffsetRect(nleft, ntop);
	SetHotPoints();

	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	SetRegion();
}

void CGraph::InitHotData()
{
	if(m_nPtCount <= 0)
	{
		return;
	}
	m_rcHot.left = m_rcHot.right = (int)m_ptary[0].X;
	m_rcHot.top = m_rcHot.bottom = (int)m_ptary[0].Y;
	for(int i = 0;i < m_nPtCount;i++)
	{
		if(m_rcHot.left > (int)m_ptary[i].X)
		{
			m_rcHot.left = (int)m_ptary[i].X;
		}
		else if(m_rcHot.right < (int)m_ptary[i].X)
		{
			m_rcHot.right = (int)m_ptary[i].X;
		}
		if(m_rcHot.top > (int)m_ptary[i].Y)
		{
			m_rcHot.top = (int)m_ptary[i].Y;
		}
		else if(m_rcHot.bottom < (int)m_ptary[i].Y)
		{
			m_rcHot.bottom = (int)m_ptary[i].Y;
		}
	}

	m_rcHot.InflateRect(2 * HOTINTERVAL, 2 * HOTINTERVAL);

	SetHotPoints();
	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	SetRegion();
	m_nOrgWidth = m_rcHot.Width();
	m_nOrgHeight = m_rcHot.Height();
}

void CGraph::DecideModiFyType(UINT nMouseState,CPoint point)
{
	if(nMouseState != MS_LBTNDOWN)
	{
		return;
	}

	//Translate the point for the point has been considered rotated so rotated back
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	CRect rect = m_rcHot;
	rect.InflateRect(7, 7);

	if(CalcGapOfpoints(pt, m_HotPts.ptLT) <= HOTINTERVAL)
	{
		m_ModifyType = MT_LT;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptTC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_TC;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptRT) <= HOTINTERVAL)
	{
		m_ModifyType = MT_RT;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptLC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_LC;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptRC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_RC;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptLB) <= HOTINTERVAL)
	{
		m_ModifyType = MT_LB;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptBC) <= HOTINTERVAL)
	{
		m_ModifyType = MT_BC;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptRB) <= HOTINTERVAL)
	{
		m_ModifyType = MT_RB;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		if(m_ModifyType != MT_MOVE &&
			m_ModifyType != MT_NONE)
		{
			AdjustGraph(m_fAngle);
		}

		m_ModifyType = MT_ROTATE;
	}
	else if(rect.PtInRect(CPoint((int)pt.X,(int)pt.Y)))
	{
		if(m_ModifyType != MT_MOVE &&
			m_ModifyType != MT_NONE)
		{
			AdjustGraph(m_fAngle);
		}

		m_ModifyType = MT_MOVE;
		m_ptModifyOrg.X = (float)point.x;
		m_ptModifyOrg.Y = (float)point.y;
	}
	else
	{
		m_ModifyType = MT_NONE;
	}
}

void CGraph::ModifyGraph(UINT nMouseState,CPoint point)
{
	if(MS_LBTNMOUSE != nMouseState)
	{
		return;
	}

	if(MT_MOVE == m_ModifyType)
	{
		MoveGraph(point);
	}
	else if(MT_ROTATE == m_ModifyType)
	{
		m_fAngle = CalcRotateAngle(point, m_ptCenter);
	}
	else if(MT_NONE != m_ModifyType)
	{
		StretchGraph(point);
	}
}

float CGraph::CalcGapOfpoints(PointF ptStart,PointF ptEnd)
{
	float x = ptEnd.X - ptStart.X;
	float y = ptEnd.Y - ptStart.Y;
	float l = sqrt((float)(x * x + y * y));
	return l;
}

float CGraph::CalcGapOfpoints(CPoint ptStart,CPoint ptEnd)
{
	PointF pts((float)ptStart.x,(float)ptStart.y);
	PointF pte((float)ptEnd.x,(float)ptEnd.y);
	return CalcGapOfpoints(pts,pte);
}

BOOL CGraph::IsPtInHotRegion(CPoint point)
{
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	return m_Region.IsVisible(pt);
}

void CGraph::DrawHotShape(Graphics& graph)
{
	SolidBrush sbrush(Color::Green);
	Pen penDraw(Color::Blue, 2);
	penDraw.SetDashStyle(DashStyleDot);

	graph.DrawRectangle(&penDraw, m_rcHot.left, m_rcHot.top, m_rcHot.Width(), m_rcHot.Height());
	graph.DrawLine(&penDraw, m_HotPts.ptTC, m_HotPts.ptRotate);
	graph.FillEllipse(&sbrush, m_HotPts.ptRotate.X - HOTINTERVAL,
		m_HotPts.ptRotate.Y - HOTINTERVAL, 2.0 * HOTINTERVAL, 2.0 * HOTINTERVAL);

	DrawEightHotPoints(graph);
}

void CGraph::MoveGraph(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	float offsetx = pt.X - m_ptModifyOrg.X;
	float offsety = pt.Y - m_ptModifyOrg.Y;
	m_ptModifyOrg.X = pt.X;
	m_ptModifyOrg.Y = pt.Y;

	MoveGraph(offsetx,offsety);
}

void CGraph::StretchGraph(CPoint point)
{
	//translate the point to the rotate point
	//for the graph has rotated and the hot points
	//has changed

	if(m_rcHot.Width() <= 15 || m_rcHot.Height() <= 15)
	{
		return;
	}

	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	PointF pts;
	float fsx = 1.0f, fsy = 1.0f;
	CPoint ptLT = m_rcHot.TopLeft();
	CPoint ptRB = m_rcHot.BottomRight();

	if(MT_LT == m_ModifyType)
	{
		ptLT.x = (int)pt.X;
		ptLT.y = (int)pt.Y;
		pts = m_HotPts.ptRB;
	}
	else if(MT_LB == m_ModifyType)
	{
		ptLT.x = (int)pt.X;
		ptLT.y = (int)m_rcHot.top;
		ptRB.x = (int)m_rcHot.right;
		ptRB.y = (int)pt.Y;
		pts = m_HotPts.ptRT;
	}
	else if(MT_RT == m_ModifyType)
	{
		ptLT.x = m_rcHot.left;
		ptLT.y = (int)pt.Y;
		ptRB.x = (int)pt.X;
		ptRB.y = m_rcHot.bottom;
		pts = m_HotPts.ptLB;
	}
	else if(MT_RB == m_ModifyType)
	{
		ptRB.x = (int)pt.X;
		ptRB.y = (int)pt.Y;
		pts = m_HotPts.ptLT;
	}
	else if(MT_TC == m_ModifyType)
	{
		ptLT.x = m_rcHot.left;
		ptLT.y = (int)pt.Y;
		pts = m_HotPts.ptBC;
	}
	else if(MT_LC == m_ModifyType)
	{
		ptLT.x = (int)pt.X;
		ptLT.y = m_rcHot.top;
		pts = m_HotPts.ptRC;
	}
	else if(MT_BC == m_ModifyType)
	{
		ptRB.x = m_rcHot.right;
		ptRB.y = (int)pt.Y;
		pts = m_HotPts.ptTC;
	}
	else if(MT_RC == m_ModifyType)
	{
		ptRB.x = (int)pt.X;
		ptRB.y = m_rcHot.bottom;
		pts = m_HotPts.ptLC;
	}

	fsx = (float)abs(ptRB.x - ptLT.x) / m_rcHot.Width();
	fsy = (float)abs(ptRB.y - ptLT.y) / m_rcHot.Height();

	if(ptRB.x - ptLT.x <= 15 || ptRB.y - ptLT.y <= 15)
	{
		return;
	}

	//set the graph data
	PointF* pPt = m_ptary.GetData();
	for(int i = 0; i < m_nPtCount; i++)
	{
		pPt[i].X = pts.X + (pPt[i].X - pts.X) * fsx;
		pPt[i].Y = pts.Y + (pPt[i].Y - pts.Y) * fsy;
	}

	m_rcHot.SetRect(ptLT, ptRB);
	m_rcHot.NormalizeRect();
	SetHotPoints();

	float fscalex = (float)m_rcHot.Width() / m_nOrgWidth;
	float fscaley = (float)m_rcHot.Height() / m_nOrgHeight;
	m_fScale = min(fscalex, fscaley);
}

//adjust the points of the graph 
//for the points should be changed
//if the center points changed
void CGraph::AdjustGraph(float fAngle)
{
	//set the matrix based on the old center points
	Matrix mat;
	mat.RotateAt(fAngle, m_ptCenter);

	//calculate the center point base on the old graph points
	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
	PointF pt = m_ptCenter;

	//get the new center points based on the matrix
	mat.TransformPoints(&m_ptCenter, 1);

	//calculate the offset of the graph
	float offx = m_ptCenter.X - pt.X;
	float offy = m_ptCenter.Y - pt.Y;

	//move graph
	if(offx != 0 && offy != 0)
	{
		MoveGraph(offx, offy);
	}

	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
}

void CGraph::SetHotPoints()
{
	m_HotPts.ptLT.X = (float)m_rcHot.left;
	m_HotPts.ptLT.Y = (float)m_rcHot.top;
	m_HotPts.ptRB.X = (float)m_rcHot.right;
	m_HotPts.ptRB.Y = (float)m_rcHot.bottom;
	m_HotPts.ptLB.X = (float)m_rcHot.left;
	m_HotPts.ptLB.Y = (float)m_rcHot.bottom;
	m_HotPts.ptRT.X = (float)m_rcHot.right;
	m_HotPts.ptRT.Y = (float)m_rcHot.top;
	m_HotPts.ptTC.Y = (float)m_rcHot.top;
	m_HotPts.ptTC.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_HotPts.ptLC.X = (float)m_rcHot.left;
	m_HotPts.ptLC.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
	m_HotPts.ptRC.X = (float)m_rcHot.right;
	m_HotPts.ptRC.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;
	m_HotPts.ptBC.Y = (float)m_rcHot.bottom;
	m_HotPts.ptBC.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	//rotate point
	m_HotPts.ptRotate.X = m_HotPts.ptTC.X;
	m_HotPts.ptRotate.Y = m_HotPts.ptTC.Y - 3.0f * HOTINTERVAL;
}

void CGraph::SetEraseRegion(Region* pRgn)
{
	ASSERT(pRgn);
	ASSERT(m_bFinish);
	RectF rcf = CRect2RectF(m_rcHot);

	HDC hdc = ::GetDC(NULL);
	Graphics graph(hdc);

	if(!m_Region.IsEmpty(&graph))
	{
		Region rgn;
		rgn.MakeEmpty();
		rgn.Union(rcf);

		Matrix mat;
		mat.RotateAt(m_fAngle, m_ptCenter);
		rgn.Transform(&mat);
		rgn.Intersect(pRgn);

		if(!rgn.IsEmpty(&graph))
		{
			mat.Reset();
			mat.RotateAt(- m_fAngle, m_ptCenter);
			rgn.Transform(&mat);

			m_bErased = TRUE;
			m_RgnErase.Union(&rgn);
			m_Region.Exclude(&rgn);
		}
	}

	::ReleaseDC(NULL, hdc);
}

void CGraph::ResetEraseState()
{
	m_bErased = FALSE;
}

void CGraph::DrawEightHotPoints(Graphics& graph)
{
	Pen penDraw(Color::CadetBlue, 3);
	graph.DrawRectangle(&penDraw, m_HotPts.ptBC.X - 5, m_HotPts.ptBC.Y - 5, 10.0, 10.0);
	graph.DrawRectangle(&penDraw, m_HotPts.ptLB.X - 5, m_HotPts.ptLB.Y - 5, 10.0, 10.0);
	graph.DrawRectangle(&penDraw, m_HotPts.ptLC.X - 5, m_HotPts.ptLC.Y - 5, 10.0, 10.0);
	graph.DrawRectangle(&penDraw, m_HotPts.ptLT.X - 5, m_HotPts.ptLT.Y - 5, 10.0, 10.0);
	graph.DrawRectangle(&penDraw, m_HotPts.ptRB.X - 5, m_HotPts.ptRB.Y - 5, 10.0, 10.0);
	graph.DrawRectangle(&penDraw, m_HotPts.ptRC.X - 5, m_HotPts.ptRC.Y - 5, 10.0, 10.0);
	graph.DrawRectangle(&penDraw, m_HotPts.ptRT.X - 5, m_HotPts.ptRT.Y - 5, 10.0, 10.0);
	graph.DrawRectangle(&penDraw, m_HotPts.ptTC.X - 5, m_HotPts.ptTC.Y - 5, 10.0, 10.0);
}

ModifyType CGraph::AroundHotPts(CPoint point)
{
	if(!m_bFinish)
	{
		return MT_NONE;
	}

	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	CRect rect = m_rcHot;
	rect.InflateRect(7, 7);

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
		return MT_ROTATE;
	}
	else if(rect.PtInRect(CPoint((int)pt.X, (int)pt.Y)))
	{
		return MT_MOVE;
	}
	else
	{
		return MT_NONE;
	}
}

BOOL CGraph::IsPtInModifyZone(CPoint point)
{
	BOOL bret = TRUE;
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	CRect rect = m_rcHot;
	rect.InflateRect(7, 7);

	if(CalcGapOfpoints(pt, m_HotPts.ptLT) <= HOTINTERVAL)
	{
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptTC) <= HOTINTERVAL)
	{
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRT) <= HOTINTERVAL)
	{
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptLC) <= HOTINTERVAL)
	{
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRC) <= HOTINTERVAL)
	{
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptLB) <= HOTINTERVAL)
	{
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptBC) <= HOTINTERVAL)
	{
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRB) <= HOTINTERVAL)
	{
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
	}
	else if(rect.PtInRect(CPoint((int)pt.X, (int)pt.Y)))
	{
	}
	else
	{
		bret = FALSE;
	}

	return bret;
}

float CGraph::CalcRotateAngle(CPoint point, PointF ptRotateCenter)
{
	float b= 0.0f;
	float alpha= 0.0f;
	float PI = 3.1415926f;
	float dx = point.x - ptRotateCenter.X;
	float dy = point.y - ptRotateCenter.Y;

	if ((dx > 0) && (dy > 0))//the first quartile
	{
		b = 90;
		alpha = (float)abs((atan((double)(dy / dx)) * (180 / PI)));
	}
	else if ((dx <=0 ) && (dy >= 0))//the forth quartile
	{
		b = 180;
		if (dy > 0)
		{
			alpha = (float)abs((atan((double)(dx / dy)) * (180 / PI)));
		}
		else if (dy == 0)
		{
			b = 270;
		}
	}
	else if ((dx < 0) && (dy < 0))//the third quartile
	{
		b = 270;
		alpha= (float)abs((atan((double)(dy / dx)) * (180 / PI)));
	}
	else//the second quartile
	{
		b = 0;
		alpha = (float)abs((atan((double)(dx / dy)) * (180 / PI)));
	}

	return (b + alpha);
}

int CGraph::InputGraphDataIndirect(CPoint* pPt, int nCount)
{
	if(nCount < m_nMinCount)
	{
		return ER_DATA_LESS;
	}
	m_ptary.RemoveAll();
	m_nPtCount = nCount;
	PointF	pt;
	for(int i = 0; i < m_nPtCount; i++)
	{
		pt.X = (float)pPt[i].x;
		pt.Y = (float)pPt[i].y;
		m_ptary.Add(pt);
	}
	m_bFinish = TRUE;
	InitHotData();

	return ER_NO_ERRO;
}

int CGraph::RotateGraph(float nAngle)
{
	if(!m_bFinish)
	{
		return ER_NOT_DRAW_FINISHED;
	}
	m_fAngle += nAngle;

	return ER_NO_ERRO;
}

int CGraph::StretchGraph(float xStret, float yStret)
{
	if(xStret < 0 || yStret < 0)
	{
		return ER_NEGATIVE_ERRO;
	}

	PointF* pPt = m_ptary.GetData();
	for(int i = 0;i < m_nPtCount;i++)
	{
		pPt[i].X = m_ptCenter.X + (pPt[i].X - m_ptCenter.X) * xStret;
		pPt[i].Y = m_ptCenter.Y + (pPt[i].Y - m_ptCenter.Y) * yStret;
	}

	InitHotData();

	return ER_NO_ERRO;
}

int CGraph::MoveGraph(float offsetx, float offsety)
{
	PointF* pPt = m_ptary.GetData();
	for(int i = 0; i < m_nPtCount; i++)
	{
		pPt[i].X += offsetx;
		pPt[i].Y += offsety;
	}

	m_rcHot.OffsetRect((int)offsetx, (int)offsety);
	SetHotPoints();

	m_ptCenter.X += offsetx;
	m_ptCenter.Y += offsety;
	m_RgnErase.Translate(offsetx, offsety);

	return ER_NO_ERRO;
}

void CGraph::SetSelectState(BOOL bSelect)
{
	m_bSelected = bSelect;
}

BOOL CGraph::GetSelectState()
{
	return m_bSelected;
}

BOOL CGraph::GetEraseState()
{
	return m_bErased;
}

Region* CGraph::GetGraphRegion()
{
	ASSERT(m_bFinish);
	return &m_Region;
}

void CGraph::SaveTheRotateAngle()
{
	m_fOldAngle = m_fAngle;
	m_fAngle = 0.0f;
}

void CGraph::RecoverTheRotateAngle()
{
	m_fAngle = m_fOldAngle;
	m_fOldAngle = 0.0f;
}

float CGraph::GetRotateAngle()
{
	return m_fAngle;
}

PointF CGraph::GetRotateCenterPoint()
{
	return m_ptCenter;
}

CRect CGraph::FormUpdateRect()
{
	CPoint ptCenter = m_rcHot.CenterPoint();
	int nWidth = max(m_rcHot.Width(), m_rcHot.Height() + 4 * HOTINTERVAL);

	CRect rc(ptCenter.x - nWidth,
			 ptCenter.y - nWidth,
			 ptCenter.x + nWidth,
			 ptCenter.y + nWidth);

	return rc;
}
