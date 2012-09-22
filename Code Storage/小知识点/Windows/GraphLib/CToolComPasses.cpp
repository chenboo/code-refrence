#include "stdafx.h"
#include "GraphLib.h"

CToolComPasses::CToolComPasses()
{
	InitDefaultData();
}

CToolComPasses::CToolComPasses(const CToolComPasses& gph)
{
	CopyTheBaseDataMember(&gph);

	m_fRadius = gph.m_fRadius;
	m_ptStart = gph.m_ptStart;
	m_ptRORLeg = gph.m_ptRORLeg;
	m_ptCircleCenter = gph.m_ptCircleCenter;
	m_fRLegOrg = gph.m_fRLegOrg;
	m_fRlegRted = gph.m_fRlegRted;
	m_fRDrawOrg = gph.m_fRDrawOrg;

	m_fRDrawted = 0.0f;
	m_fStartAngle = 0.0f;

	m_pathHead.Reset();
	m_pathLeftLeg.Reset();
	m_pathRightLeg.Reset();
	m_pathPencilEnd.Reset();
	m_pathPencilBody.Reset();
	m_pathPencilSharp.Reset();

	m_pathHead.AddPath(&gph.m_pathHead, TRUE);
	m_pathLeftLeg.AddPath(&gph.m_pathLeftLeg, TRUE);
	m_pathRightLeg.AddPath(&gph.m_pathRightLeg, TRUE);
	m_pathPencilEnd.AddPath(&gph.m_pathPencilEnd, TRUE);
	m_pathPencilBody.AddPath(&gph.m_pathPencilBody, TRUE);
	m_pathPencilSharp.AddPath(&gph.m_pathPencilSharp, TRUE);
}

CToolComPasses& CToolComPasses::operator = (const CToolComPasses& gph)
{
	CopyTheBaseDataMember(&gph);

	SAFE_DELETE(m_pSubGraph);
	m_fRadius = gph.m_fRadius;
	m_ptStart = gph.m_ptStart;
	m_ptRORLeg = gph.m_ptRORLeg;
	m_ptCircleCenter = gph.m_ptCircleCenter;
	m_fRLegOrg = gph.m_fRLegOrg;
	m_fRlegRted = gph.m_fRlegRted;
	m_fRDrawOrg = gph.m_fRDrawOrg;
	
	m_fRDrawted = 0.0f;
	m_fStartAngle = 0.0f;

	m_pathHead.Reset();
	m_pathLeftLeg.Reset();
	m_pathRightLeg.Reset();
	m_pathPencilEnd.Reset();
	m_pathPencilBody.Reset();
	m_pathPencilSharp.Reset();

	m_pathHead.AddPath(&gph.m_pathHead, TRUE);
	m_pathLeftLeg.AddPath(&gph.m_pathLeftLeg, TRUE);
	m_pathRightLeg.AddPath(&gph.m_pathRightLeg, TRUE);
	m_pathPencilEnd.AddPath(&gph.m_pathPencilEnd, TRUE);
	m_pathPencilBody.AddPath(&gph.m_pathPencilBody, TRUE);
	m_pathPencilSharp.AddPath(&gph.m_pathPencilSharp, TRUE);

	return *this;
}

CToolComPasses::~CToolComPasses(void)
{
	SAFE_DELETE(m_pSubGraph);
}

void CToolComPasses::InitDefaultData()
{
	InitHeadPath();
	InitLeftLegPath();
	InitRightLegPath();

	m_fRadius = 20.0f;
	m_fRlegRted = 0.0f;
	m_fRDrawted = 0.0f;
	m_fRLegOrg = 0.0f;
	m_fRDrawOrg = 0.0f;
	m_fStartAngle = 0.0f;

	float fCenterx = GetSystemMetrics(SM_CXSCREEN) / 2.0f;
	float fCentery = GetSystemMetrics(SM_CYSCREEN) / 2.0f;

	m_nPtCount = 2;
	m_ptary.SetSize(2, 2);
	m_ptary[0].X = fCenterx - 101.0f;
	m_ptary[0].Y = fCentery - 202.0f;
	m_ptary[1].X = fCenterx + 64.0f;
	m_ptary[1].Y = fCentery + 184.0f;

	m_ptRORLeg.X = fCenterx - 27.0f;
	m_ptRORLeg.Y = fCentery - 90.0f;
	m_ptCircleCenter.X = fCenterx - 28.0f;
	m_ptCircleCenter.Y = fCentery + 178.0f;

	m_ptStart.X = fCenterx - 8.0f;
	m_ptStart.Y = fCentery + 178.0f;

	m_bEdit = TRUE;
	m_bSelected = TRUE;
	m_bFinish = TRUE;
	MoveAllPath(fCenterx - 100.0f, fCentery - 200.0f);

	m_rcHot.left = (int)min(m_ptary[0].X, m_ptary[1].X);
	m_rcHot.right = (int)max(m_ptary[0].X, m_ptary[1].X);
	m_rcHot.top = (int)min(m_ptary[0].Y, m_ptary[1].Y);
	m_rcHot.bottom = (int)max(m_ptary[0].Y, m_ptary[1].Y);

	m_HotPts.ptRotate.X = (m_rcHot.left + m_rcHot.right) / 2.0f - 8.0f;
	m_HotPts.ptRotate.Y = m_rcHot.top - 3.0f * HOTINTERVAL;

	m_HotPts.ptTC.X = m_HotPts.ptRotate.X;
	m_HotPts.ptTC.Y = (float)m_rcHot.top;

	SetRegion();
}

void CToolComPasses::Draw(CDC* pDC, CRect& rcInvalid)
{
	if(m_nPtCount <= 1)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);
	Matrix mat;
	mat.RotateAt(m_fAngle, m_ptCircleCenter);
	graph.SetTransform(&mat);

	DrawLeftLeg(graph);
	DrawRightLeg(graph);
	DrawHead(graph);

	if(m_bSelected)
	{
		DrawHotShape(graph);
	}

	graph.ResetTransform();
	RectF rcf(m_ptCircleCenter.X - m_fRadius, m_ptCircleCenter.Y - m_fRadius,
		2 * m_fRadius, 2 * m_fRadius);
	
	if(m_fRDrawted)
	{
		Pen penDraw(Color::Green, 3);
		graph.DrawArc(&penDraw, rcf, m_fStartAngle, m_fRDrawted);
	}
}

void CToolComPasses::FinishGraph()
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

void CToolComPasses::SetRegion()
{
	if(m_nPtCount < 2)
	{
		return;
	}

	m_Region.MakeEmpty();
	m_Region.Union(&m_pathHead);
	m_Region.Union(&m_pathLeftLeg);
	m_Region.Union(&m_pathRightLeg);
	m_Region.Union(&m_pathPencilEnd);
	m_Region.Union(&m_pathPencilBody);
	m_Region.Union(&m_pathPencilSharp);
}

void CToolComPasses::SetEraseRegion(Region* pRgn)
{
}

void CToolComPasses::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
}

void CToolComPasses::AddPoint(UINT nMouseState, CPoint point, CRect& rcUpdate)
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
		if(m_fRDrawted >= 360.0f || m_fRDrawted <= -360.0f)
		{
			m_nSubType = DT_CIRCLE;
			m_pSubGraph = new CCircle(CS_CIRCLE, 3, Color::Black);

			POINTARRAY ptarray;
			ptarray.SetSize(2);
			ptarray[0].X = m_ptCircleCenter.X - m_fRadius;
			ptarray[0].Y = m_ptCircleCenter.Y - m_fRadius;
			ptarray[1].X = m_ptCircleCenter.X + m_fRadius;
			ptarray[1].Y = m_ptCircleCenter.Y + m_fRadius;
			m_pSubGraph->FormGraphDirect(ptarray);
		}

		m_fRDrawted = 0.0f;
		SetRegion();
	}
}

CRect CToolComPasses::FormUpdateRect()
{
	PointF ptTop;
	ptTop.X = m_HotPts.ptRotate.X;
	ptTop.Y = m_HotPts.ptRotate.Y - HOTINTERVAL - 2.0f;
	PointF ptBottom(m_HotPts.ptRotate.X, (float)m_rcHot.bottom);

	Matrix mat;
	mat.RotateAt(m_fAngle, m_ptCircleCenter);
	mat.TransformPoints(&ptTop, 1);
	int nWidth = (int)CalcGapOfpoints(ptTop, ptBottom);

	CRect rect((int)m_ptCircleCenter.X - nWidth,
			   (int)m_ptCircleCenter.Y - nWidth,
			   (int)m_ptCircleCenter.X + nWidth,
			   (int)m_ptCircleCenter.Y + nWidth);

	return rect;
}

void CToolComPasses::InitDefaultHotData()
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

	m_HotPts.ptRotate.X = m_ptCenter.X - 7.0f;
	m_HotPts.ptRotate.Y = m_rcHot.top - 3.0f * HOTINTERVAL;
}

void CToolComPasses::InitHotData()
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	SetRegion();
}

ModifyType CToolComPasses::AroundHotPts(CPoint point)
{
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCircleCenter);
	mat.TransformPoints(&pt, 1);

	if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		return MT_ROTATE;
	}
	else if(m_pathHead.IsVisible(pt))
	{
		return MT_MOVE;
	}
	else if(m_pathRightLeg.IsVisible(pt))
	{
		return MT_RC;
	}
	else if(m_pathPencilBody.IsVisible(pt))
	{
		return MT_TC;
	}
	else if(m_Region.IsVisible(pt))
	{
		return MT_MOVE;
	}
	else
	{
		return MT_NONE;
	}
}

void CToolComPasses::DrawHotShape(Graphics& graph)
{
	Pen penDraw(Color::Blue, 2);
	SolidBrush sbrush(Color::Green);
	penDraw.SetDashStyle(DashStyleDot);

	graph.DrawLine(&penDraw, m_HotPts.ptRotate, m_HotPts.ptTC);
	graph.FillEllipse(&sbrush, m_HotPts.ptRotate.X - HOTINTERVAL,
					  m_HotPts.ptRotate.Y - HOTINTERVAL,
					  2.0 * HOTINTERVAL, 2.0 * HOTINTERVAL);
}

void CToolComPasses::DecideModiFyType(UINT nMouseState, CPoint point)
{
	if(nMouseState != MS_LBTNDOWN)
	{
		return;
	}

	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCircleCenter);
	mat.TransformPoints(&pt, 1);

	if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		m_ModifyType = MT_ROTATE;
	}
	else if(m_pathHead.IsVisible(pt))
	{
		m_ModifyType = MT_MOVE;
		m_ptModifyOrg.X = (float)point.x;
		m_ptModifyOrg.Y = (float)point.y;
	}
	else if(m_pathRightLeg.IsVisible(pt))
	{
		m_ModifyType = MT_RC;
		
		mat.Reset();
		mat.RotateAt(m_fAngle, m_ptCircleCenter);
		PointF pt1 = m_ptRORLeg;
		mat.TransformPoints(&pt1, 1);
		m_fRLegOrg = CalcRotateAngle(point, pt1);
	}
	else if(m_pathPencilBody.IsVisible(pt))
	{
		m_ModifyType = MT_TC;
		m_fRDrawted = 0.0f;
		m_fRDrawOrg = CalcRotateAngle(point, m_ptCircleCenter);

		PointF pt1 = m_ptStart;

		mat.Reset();
		mat.RotateAt(m_fAngle, m_ptCircleCenter);
		mat.TransformPoints(&pt1, 1);

		point.x = (int)pt1.X;
		point.y = (int)pt1.Y;
		m_fStartAngle = CalcRotateAngle(point, m_ptCircleCenter) - 90.0f;
	}
	else if(m_Region.IsVisible(pt))
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

void CToolComPasses::ModifyGraph(UINT nMouseState, CPoint point)
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
		m_fAngle = CalcRotateAngle(point, m_ptCircleCenter);
	}
	else if(MT_NONE != m_ModifyType)
	{
		StretchGraph(point);
	}
}

void CToolComPasses::ResetSubGraph()
{
	__super::ResetSubGraph();

	m_fRDrawted = 0.0f;
}

void CToolComPasses::MoveGraph(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	float offsetx = pt.X - m_ptModifyOrg.X;
	float offsety = pt.Y - m_ptModifyOrg.Y;

	m_ptModifyOrg.X = pt.X;
	m_ptModifyOrg.Y = pt.Y;

	MoveGraph(offsetx, offsety);
}

int CToolComPasses::MoveGraph(float offsetx, float offsety)
{
	m_ptStart.X += offsetx;
	m_ptStart.Y += offsety;
	m_ptRORLeg.X += offsetx;
	m_ptRORLeg.Y += offsety;
	m_ptCircleCenter.X += offsetx;
	m_ptCircleCenter.Y += offsety;

	m_HotPts.ptTC.X += offsetx;
	m_HotPts.ptTC.Y += offsety;

	m_HotPts.ptRotate.X += offsetx;
	m_HotPts.ptRotate.Y += offsety;

	MoveAllPath(offsetx, offsety);
	m_rcHot.OffsetRect((int)offsetx, (int)offsety);

	return ER_NO_ERRO;
}

BOOL CToolComPasses::IsPtInHotRegion(CPoint point)
{
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCircleCenter);
	mat.TransformPoints(&pt, 1);

	return m_Region.IsVisible(pt);
}

void CToolComPasses::MoveAllPath(float offsetx, float offsety)
{
	Matrix mat;
	mat.Translate(offsetx, offsety);
	m_pathHead.Transform(&mat);
	m_pathLeftLeg.Transform(&mat);
	m_pathRightLeg.Transform(&mat);
	m_pathPencilEnd.Transform(&mat);
	m_pathPencilBody.Transform(&mat);
	m_pathPencilSharp.Transform(&mat);
}

float CToolComPasses::GetRotateAngle()
{
	return 0.0f;
}

void CToolComPasses::StretchGraph(CPoint point)
{
	//translate the point to the rotate point
	//for the graph has rotated and the hot points
	//has changed
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCircleCenter);
	mat.TransformPoints(&pt, 1);

	if(MT_RC == m_ModifyType)
	{
		PointF pt1 = m_ptRORLeg;
		mat.Reset();
		mat.RotateAt(m_fAngle, m_ptCircleCenter);
		mat.TransformPoints(&pt1, 1);
		float fAngle = CalcRotateAngle(point, pt1);

		if(fAngle >= 270 && fAngle <= 360 && m_fRLegOrg < 180)
		{
			fAngle -= 360;
		}

		float fROffset = fAngle - m_fRLegOrg;
		if(m_fRlegRted + fROffset <= -90.0f)
		{
			fROffset = - 90.0f - m_fRlegRted;
			m_fRlegRted = -90.0f;
		}
		else if(m_fRlegRted + fROffset >= 0.0f)
		{
			fROffset = - m_fRlegRted;
			m_fRlegRted = 0.0f;
		}
		else
		{
			m_fRlegRted += fROffset;
		}

		m_fRLegOrg = fAngle;
		RotateRightLeg(fROffset);
	}
	else if(MT_TC == m_ModifyType)
	{
		float fROffset = 0.0f;
		float fAngle = CalcRotateAngle(point, m_ptCircleCenter);

		if(fAngle <= 90.0f && fAngle >= 0
			&& m_fRDrawOrg >= 270)
		{
			fROffset = 360.0f + fAngle - m_fRDrawOrg;
		}
		else if(fAngle >= 270
			&& m_fRDrawOrg >= 0
			&& m_fRDrawOrg <= 90)
		{
			fROffset = fAngle - 360.0f - m_fRDrawOrg;
		}
		else
		{
			fROffset = fAngle - m_fRDrawOrg;
		}

		m_fAngle += fROffset;
		m_fRDrawOrg = fAngle;
		m_fRDrawted += fROffset;
	}
}

void CToolComPasses::RotateRightLeg(float fAngle)
{
	Matrix mat;
	mat.RotateAt(fAngle, m_ptRORLeg, MatrixOrderAppend);

	m_pathRightLeg.Transform(&mat);
	m_pathPencilEnd.Transform(&mat);
	m_pathPencilBody.Transform(&mat);
	m_pathPencilSharp.Transform(&mat);

	mat.TransformPoints(&m_ptStart, 1);
	m_fRadius = CalcGapOfpoints(m_ptCircleCenter, m_ptStart);
}

BOOL CToolComPasses::IsPtInModifyZone(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCircleCenter);
	mat.TransformPoints(&pt, 1);

	if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		return TRUE;
	}
	else if(IsPtInHotRegion(point))
	{
		return TRUE;
	}

	return FALSE;
}

void CToolComPasses::DrawHead(Graphics& graph)
{
	SolidBrush brush(Color::Silver);
	graph.FillPath(&brush, &m_pathHead);
}

void CToolComPasses::DrawLeftLeg(Graphics& graph)
{
	SolidBrush brush(Color::Blue);
	graph.FillPath(&brush, &m_pathLeftLeg);
}

void CToolComPasses::DrawRightLeg(Graphics& graph)
{
	Pen penDraw(Color::Red, 1);
	SolidBrush brush(Color::Green);

	//Draw pencil body
	graph.FillPath(&brush, &m_pathPencilBody);

	//draw pencil sharp
	brush.SetColor(Color::BurlyWood);
	graph.FillPath(&brush, &m_pathPencilSharp);

	//draw pencil point
	penDraw.SetWidth(3.0f);
	penDraw.SetColor(Color::Red);
	graph.DrawPath(&penDraw, &m_pathPencilEnd);

	//draw right leg
	brush.SetColor(Color::Blue);
	graph.FillPath(&brush, &m_pathRightLeg);
}

void CToolComPasses::InitHeadPath()
{
	PointF pt[5];
	pt[0].X = 60.0f;
	pt[0].Y = 0.0f;

	RectF rcf(pt[0].X, pt[0].Y, 25, 50);
	m_pathHead.AddRectangle(rcf);

	pt[1].X = - 79.0f;
	pt[1].Y = 49.6f;

	rcf.X = pt[1].X;
	rcf.Y = pt[1].Y;
	rcf.Width = 300.0f;
	rcf.Height = 300.0f;
	m_pathHead.AddArc(rcf, -118, 56);

	pt[1].X = 141.0f;
	pt[1].Y = 68.0f;
	pt[2].X = 117.0f;
	pt[2].Y = 140.0f;
	m_pathHead.AddLine(pt[1], pt[2]);

	pt[3].X = 31.0f;
	pt[3].Y = pt[2].Y;
	m_pathHead.AddLine(pt[2], pt[3]);

	pt[4].X = 1.0f;
	pt[4].Y = 68.0f;
	m_pathHead.AddLine(pt[3], pt[4]);
}

void CToolComPasses::InitLeftLegPath()
{
	PointF pt[5];
	pt[0].X = 35.0f;
	pt[0].Y = 120.0f;
	pt[1].X = 35.0f;
	pt[1].Y = 328.0f;
	pt[2].X = 53.5f;
	pt[2].Y = 358.0f;
	pt[3].X = 72.0f;
	pt[3].Y = 358.0f;
	pt[4].X = 72.0f;
	pt[4].Y = 120.0f;
	m_pathLeftLeg.AddPolygon(pt, 5);

	pt[0].X = 62.75f;
	pt[0].Y = 358.0f;
	pt[1].X = 72.0f;
	pt[1].Y = 358.0f;
	pt[2].X = 72.0f;
	pt[2].Y = 379.636f;
	m_pathLeftLeg.AddPolygon(pt, 3);
}

void CToolComPasses::InitRightLegPath()
{
	PointF pt[5];
	//init pencil body path
	pt[0].X = 126.0f;
	pt[0].Y = 225.0f;
	pt[1].X = 87.5f;
	pt[1].Y = 340.0f;
	pt[2].X = 121.5f;
	pt[2].Y = 354.0f;
	pt[3].X = 161.7f;
	pt[3].Y = 237.0f;
	m_pathPencilBody.AddPolygon(pt, 4);

	//init pencil sharp path
	pt[0].X = 87.5f;
	pt[0].Y = 366.0f;
	pt[3].X = 102.5f;
	pt[3].Y = 372.0f;
	m_pathPencilSharp.AddPolygon(pt, 4);

	//init pencil end path
	pt[0].X = 95.0f;
	pt[0].Y = 368.0f;
	pt[1].X =91.0f;
	pt[1].Y = 380.0f;
	m_pathPencilEnd.AddLine(pt[0], pt[1]);
	
	//init right leg path
	pt[0].X = 75.0f;
	pt[0].Y = 120.0f;
	pt[1].X = 75.0f;
	pt[1].Y = 270.0f;
	pt[2].X = 95.0f;
	pt[2].Y = 320.0f;
	m_pathRightLeg.AddLine(pt[0], pt[1]);
	m_pathRightLeg.AddLine(pt[1], pt[2]);

	pt[3].X = 80.0f;
	pt[3].Y = 316.0f;
	RectF rcf(pt[3].X, pt[3].Y, 60, 60);
	m_pathRightLeg.AddArc(rcf, - 120, 90);

	pt[3].X = 136.5f;
	pt[3].Y = 330.0f;
	pt[4].X = 147.0f;
	pt[4].Y = 300.0f;
	m_pathRightLeg.AddLine(pt[3], pt[4]);

	pt[4].X = 112.0f;
	pt[4].Y = 240.0f;
	rcf.X = pt[4].X;
	rcf.Y = pt[4].Y;
	rcf.Width = 60;
	rcf.Height = 60;
	m_pathRightLeg.AddArc(rcf, 80, 108);

	pt[3].X = 112.25f;
	pt[3].Y = 165.5f;
	pt[4].X = 113.095f;
	pt[4].Y = 120.0f;
	m_pathRightLeg.AddLine(pt[3], pt[4]);

	pt[3].X = 112.25f;
	pt[3].Y = 120.0f;
	pt[4].X = 75.0f;
	pt[4].Y = 120.0f;
	m_pathRightLeg.AddLine(pt[3], pt[4]);
}
