#include "stdafx.h"
#include "GraphLib.h"

CToolTriangleRuler::CToolTriangleRuler()
{
	InitDataMember();
	InitDefaultData();
}

CToolTriangleRuler::CToolTriangleRuler(const CToolTriangleRuler& gph)
{
	CopyTheBaseDataMember(&gph);

	m_pathMove.Reset();
	m_pathLeft.Reset();
	m_pathRight.Reset();

	m_pathMove.AddPath(&gph.m_pathMove, TRUE);
	m_pathLeft.AddPath(&gph.m_pathLeft, TRUE);
	m_pathRight.AddPath(&gph.m_pathRight, TRUE);

	m_rcInterTriangle = gph.m_rcInterTriangle;
}

CToolTriangleRuler& CToolTriangleRuler::operator = (const CToolTriangleRuler& gph)
{
	CopyTheBaseDataMember(&gph);

	m_pathMove.Reset();
	m_pathLeft.Reset();
	m_pathRight.Reset();

	m_pathMove.AddPath(&gph.m_pathMove, TRUE);
	m_pathLeft.AddPath(&gph.m_pathLeft, TRUE);
	m_pathRight.AddPath(&gph.m_pathRight, TRUE);

	m_rcInterTriangle = gph.m_rcInterTriangle;

	return *this;
}

CToolTriangleRuler::~CToolTriangleRuler(void)
{
}

void CToolTriangleRuler::InitDataMember()
{
	BYTE r = 0;
	BYTE g = 0;
	BYTE b = 200;
	ARGB argb = Color::MakeARGB(200, r, g, b);
	m_crColor.SetValue(argb);

	m_ptary.SetSize(2, 2);
	m_rcInterTriangle.SetRectEmpty();
}

void CToolTriangleRuler::InitDefaultData()
{
	int nCenterx = GetSystemMetrics(SM_CXSCREEN) / 2;
	int nCentery = GetSystemMetrics(SM_CYSCREEN) / 2;

	m_nPtCount = 2;
	m_ptary[0].X = nCenterx - 300.0f;
	m_ptary[0].Y = nCentery - 150.0f;
	m_ptary[1].X = nCenterx + 300.0f;
	m_ptary[1].Y = nCentery + 150.0f;

	m_bEdit = TRUE;
	m_bSelected = TRUE;
	InitHotData();
}

void CToolTriangleRuler::Draw(CDC* pDC, CRect& rcInvalid)
{
	if(m_nPtCount <= 1)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);
	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);
	Pen penDraw(m_crColor, 2);

	DrawTriangles(graph);
	DrawTriangleRulerMarks(graph);

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}
}

void CToolTriangleRuler::FinishGraph()
{
	if(m_nPtCount == 2)
	{
		m_bFinish = TRUE;
		m_bSelected = FALSE;
		m_bEdit = FALSE;
		InitHotData();
	}
	else
	{
		m_bFinish = FALSE;
	}

	return;
}

void CToolTriangleRuler::SetRegion()
{
	if(m_nPtCount < 2)
	{
		return;
	}

	m_Region.MakeEmpty();
	GraphicsPath path;

	PointF pt[3];
	pt[0].X = (float)m_ptCenter.X;
	pt[0].Y = (float)m_rcHot.top;
	pt[1].X = (float)m_rcHot.left;
	pt[1].Y = (float)m_rcHot.bottom;
	pt[2].X = (float)m_rcHot.right;
	pt[2].Y = (float)m_rcHot.bottom;
	path.AddPolygon(pt, 3);
	m_Region.Union(&path);

	path.Reset();
	pt[0].X = (float)m_ptCenter.X;
	pt[0].Y = (float)m_rcInterTriangle.top;
	pt[1].X = (float)m_rcInterTriangle.left;
	pt[1].Y = (float)m_rcInterTriangle.bottom;
	pt[2].X = (float)m_rcInterTriangle.right;
	pt[2].Y = (float)m_rcInterTriangle.bottom;
	path.AddPolygon(pt, 3);

	m_Region.Exclude(&path);
}

void CToolTriangleRuler::SetEraseRegion(Region* pRgn)
{
}

void CToolTriangleRuler::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
}

void CToolTriangleRuler::AddPoint(UINT nMouseState,CPoint point, CRect& rcUpdate)
{
	if(!m_bFinish)
	{
		if(MS_LBTNDOWN == nMouseState && !IsPtInModifyZone(point))
		{
			m_bSelected = FALSE;
			SetEditSate(FALSE);
			m_bFinish = TRUE;
			InitHotData();
		}
	}
	else
	{
		rcUpdate = FormUpdateRect();

		if(MS_LBTNDOWN == nMouseState && !IsPtInModifyZone(point))
		{
			m_bSelected = FALSE;
			SetEditSate(FALSE);
			return;
		}
	}

	DecideModiFyType(nMouseState, point);
	ModifyGraph(nMouseState, point);

	if(MS_LBTNUP == nMouseState)
	{
		InitModifyZone();
		SetRegion();
	}
}

void CToolTriangleRuler::InitHotData()
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	m_rcHot.left = (int)min(m_ptary[0].X, m_ptary[1].X);
	m_rcHot.right = (int)max(m_ptary[0].X, m_ptary[1].X);
	m_rcHot.top = (int)min(m_ptary[0].Y, m_ptary[1].Y);
	m_rcHot.bottom = (int)max(m_ptary[0].Y, m_ptary[1].Y);

	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	m_HotPts.ptRotate.X = m_ptCenter.X;
	m_HotPts.ptRotate.Y = m_rcHot.top - 3.0f * HOTINTERVAL;

	int nInterRectWidth = (int)(m_rcHot.Width() * 0.6f);
	int nInterRectHeight = (int)(m_rcHot.Height() * 0.6f);

	m_rcInterTriangle.SetRect((int)(m_ptCenter.X - nInterRectWidth / 2),
							  (int)(m_ptCenter.Y - nInterRectHeight / 2),
							  (int)(m_ptCenter.X + nInterRectWidth / 2),
							  (int)(m_ptCenter.Y + nInterRectHeight / 2));

	InitModifyZone();

	SetRegion();
}

ModifyType CToolTriangleRuler::AroundHotPts(CPoint point)
{
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		return MT_ROTATE;
	}
	else if(m_pathLeft.IsVisible(pt))
	{
		return MT_LC;
	}
	else if(m_pathRight.IsVisible(pt))
	{
		return MT_RC;
	}
	else if(m_pathMove.IsVisible(pt))
	{
		return MT_MOVE;
	}
	else
	{
		return MT_NONE;
	}
}

void CToolTriangleRuler::DrawHotShape(Graphics& graph)
{
	SolidBrush sbrush(Color::Green);
	Pen penDraw(Color::Blue, 2);
	penDraw.SetDashStyle(DashStyleDot);

	PointF pt((m_ptary[0].X + m_ptary[1].X) / 2, m_ptary[0].Y);

	graph.DrawLine(&penDraw, pt, m_HotPts.ptRotate);
	graph.FillEllipse(&sbrush, m_HotPts.ptRotate.X - HOTINTERVAL,
		m_HotPts.ptRotate.Y - HOTINTERVAL,
		2.0 * HOTINTERVAL, 2.0 * HOTINTERVAL);
}

void CToolTriangleRuler::DecideModiFyType(UINT nMouseState, CPoint point)
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

	if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		m_ModifyType = MT_ROTATE;
	}
	else if(m_pathLeft.IsVisible(pt))
	{
		m_ModifyType = MT_LC;
		m_ptModifyOrg = pt;
	}
	else if(m_pathRight.IsVisible(pt))
	{
		m_ModifyType = MT_RC;
		m_ptModifyOrg = pt;
	}
	else if(m_pathMove.IsVisible(pt))
	{
		m_ModifyType = MT_MOVE;
		m_ptModifyOrg.X = (float)point.x;
		m_ptModifyOrg.Y = (float)point.y;
	}
	else
	{
		m_ModifyType = MT_NONE;
	}

	AdjustGraph(m_fAngle);
}

void CToolTriangleRuler::MoveGraph(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	float offsetx = pt.X - m_ptModifyOrg.X;
	float offsety = pt.Y - m_ptModifyOrg.Y;
	m_ptModifyOrg.X = pt.X;
	m_ptModifyOrg.Y = pt.Y;

	MoveGraph(offsetx, offsety);
}

int CToolTriangleRuler::MoveGraph(float offsetx,float offsety)
{
	m_ptary[0].X += offsetx;
	m_ptary[0].Y += offsety;
	m_ptary[1].X += offsetx;
	m_ptary[1].Y += offsety;

	m_rcHot.OffsetRect((int)offsetx, (int)offsety);
	m_rcInterTriangle.OffsetRect((int)offsetx, (int)offsety);

	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	m_HotPts.ptRotate.X = m_ptCenter.X;
	m_HotPts.ptRotate.Y = m_rcHot.top - 3.0f * HOTINTERVAL;

	return ER_NO_ERRO;
}

void CToolTriangleRuler::StretchGraph(CPoint point)
{
	//translate the point to the rotate point
	//for the graph has rotated and the hot points
	//has changed
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	float offsetx = pt.X - m_ptModifyOrg.X;
	float offsety = pt.Y - m_ptModifyOrg.Y;
	m_ptModifyOrg.X = pt.X;
	m_ptModifyOrg.Y = pt.Y;

	if(MT_LC == m_ModifyType)
	{
		if(m_rcHot.left + offsetx <= m_rcHot.right - 200)
		{
			m_rcHot.left += (int)offsetx;
		}
		else
		{
			return;
		}
	}
	else if(MT_RC == m_ModifyType)
	{
		if(m_rcHot.right + offsetx >= m_rcHot.left + 200)
		{
			m_rcHot.right += (int)offsetx;
		}
		else
		{
			return;
		}
	}

	int nWidth = m_rcHot.Width();
	m_rcHot.top = m_rcHot.bottom - nWidth / 2;

	m_ptary[0].X = (float)m_rcHot.left;
	m_ptary[0].Y = (float)m_rcHot.top;
	m_ptary[1].X = (float)m_rcHot.right;
	m_ptary[1].Y = (float)m_rcHot.bottom;

	PointF ptCenter;
	ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	m_ptCenter.X = ptCenter.X;
	m_ptCenter.Y = ptCenter.Y;

	m_HotPts.ptRotate.X = ptCenter.X;
	m_HotPts.ptRotate.Y = m_rcHot.top - 3.0f * HOTINTERVAL;

	int nInterRectWidth = (int)(m_rcHot.Width() * 0.6f);
	int nInterRectHeight = (int)(m_rcHot.Height() * 0.6f);

	m_rcInterTriangle.SetRect((int)(ptCenter.X - nInterRectWidth / 2),
		(int)(ptCenter.Y - nInterRectHeight / 2),
		(int)(ptCenter.X + nInterRectWidth / 2),
		(int)(ptCenter.Y + nInterRectHeight / 2));
}

void CToolTriangleRuler::DrawTriangles(Graphics& graph)
{
	Pen penDraw(m_crColor, 2);
	PointF ptCenter = CPoint2PointF(m_rcHot.CenterPoint());

	PointF pt[3];
	pt[0].X = ptCenter.X;
	pt[0].Y = (float)m_rcHot.top;
	pt[1].X = (float)m_rcHot.left;
	pt[1].Y = (float)m_rcHot.bottom;
	pt[2].X = (float)m_rcHot.right;
	pt[2].Y = (float)m_rcHot.bottom;
	graph.DrawPolygon(&penDraw, pt, 3);

	pt[0].X = (float)ptCenter.X;
	pt[0].Y = (float)m_rcInterTriangle.top;
	pt[1].X = (float)m_rcInterTriangle.left;
	pt[1].Y = (float)m_rcInterTriangle.bottom;
	pt[2].X = (float)m_rcInterTriangle.right;
	pt[2].Y = (float)m_rcInterTriangle.bottom;
	graph.DrawPolygon(&penDraw, pt, 3);
}

void CToolTriangleRuler::DrawTriangleRulerMarks(Graphics& graph)
{
	int nNumber = 0;
	int nbegin = m_rcHot.left + 40;
	int nend = m_rcHot.right - 33;

	CString strNumber;
	Font fontNumber(L"Arial", 10);
	SolidBrush brush(Color::Black);
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);

	Pen penDraw(Color::Black, 1);
	PointF ptBegin((float)nbegin, (float)m_rcHot.bottom);
	PointF ptEnd = ptBegin;
	graph.SetTextRenderingHint(TextRenderingHintAntiAlias);

	for(int i = nbegin; i < nend; i+= 10)
	{
		ptEnd.X = (float)i;
		ptBegin.X = (float)i;
		if((i - nbegin) % 100 == 0)
		{
			ptEnd.Y = ptBegin.Y - 20;
			graph.DrawLine(&penDraw, ptBegin, ptEnd);
			nNumber = (i - nbegin) / 100;
			strNumber.Format(_T("%d"), nNumber);

			ptEnd.Y = ptBegin.Y - 35;
			graph.DrawString(strNumber, strNumber.GetLength(), &fontNumber, ptEnd, &format, &brush);
		}
		else if((i - nbegin) % 50 == 0)
		{
			ptEnd.Y = ptBegin.Y - 15;
			graph.DrawLine(&penDraw, ptBegin, ptEnd);
		}
		else
		{
			ptEnd.Y = ptBegin.Y - 10;
			graph.DrawLine(&penDraw, ptBegin, ptEnd);
		}
	}
}

void CToolTriangleRuler::InitModifyZone()
{
	PointF pt[4];
	PointF ptCenter = CPoint2PointF(m_rcHot.CenterPoint());

	pt[0].X = (float)ptCenter.X;
	pt[0].Y = (float)m_rcHot.top;
	pt[1].X = (float)ptCenter.X;
	pt[1].Y = (float)m_rcInterTriangle.top;
	pt[2].X = (float)m_rcInterTriangle.left;
	pt[2].Y = (float)m_rcInterTriangle.bottom;
	pt[3].X = (float)m_rcHot.left;
	pt[3].Y = (float)m_rcHot.bottom;

	m_pathLeft.Reset();
	m_pathLeft.AddPolygon(pt, 4);

	pt[2].X = (float)m_rcInterTriangle.right;
	pt[2].Y = (float)m_rcInterTriangle.bottom;
	pt[3].X = (float)m_rcHot.right;
	pt[3].Y = (float)m_rcHot.bottom;

	m_pathRight.Reset();
	m_pathRight.AddPolygon(pt, 4);

	pt[0].X = (float)m_rcHot.left;
	pt[0].Y = (float)m_rcHot.bottom;
	pt[1].X = (float)m_rcInterTriangle.left;
	pt[1].Y = (float)m_rcInterTriangle.bottom;

	m_pathMove.Reset();
	m_pathMove.AddPolygon(pt, 4);
}

BOOL CToolTriangleRuler::IsPtInModifyZone(CPoint point)
{
	BOOL bret = TRUE;
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
	}
	else if(m_pathLeft.IsVisible(pt))
	{
	}
	else if(m_pathRight.IsVisible(pt))
	{
	}
	else if(m_pathMove.IsVisible(pt))
	{
	}
	else
	{
		bret = FALSE;
	}

	return bret;
}