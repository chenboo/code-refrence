#include "stdafx.h"
#include "GraphLib.h"

CToolProtractor::CToolProtractor()
{
	InitDataMember();
	InitDefaultData();
}

CToolProtractor::CToolProtractor(const CToolProtractor& gph)
{
	CopyTheBaseDataMember(&gph);
	
	m_fRadio[0] = gph.m_fRadio[0];
	m_fRadio[1] = gph.m_fRadio[1];
	m_fRadio[2] = gph.m_fRadio[2];
	m_fRadio[3] = gph.m_fRadio[3];

	m_rcCircle[0] = gph.m_rcCircle[0];
	m_rcCircle[1] = gph.m_rcCircle[1];
	m_rcCircle[2] = gph.m_rcCircle[2];
}

CToolProtractor& CToolProtractor::operator = (const CToolProtractor& gph)
{
	CopyTheBaseDataMember(&gph);

	m_fRadio[0] = gph.m_fRadio[0];
	m_fRadio[1] = gph.m_fRadio[1];
	m_fRadio[2] = gph.m_fRadio[2];
	m_fRadio[3] = gph.m_fRadio[3];

	m_rcCircle[0] = gph.m_rcCircle[0];
	m_rcCircle[1] = gph.m_rcCircle[1];
	m_rcCircle[2] = gph.m_rcCircle[2];

	return *this;
}

CToolProtractor::~CToolProtractor(void)
{
}

void CToolProtractor::InitDataMember()
{
	BYTE r = 0;
	BYTE g = 0;
	BYTE b = 200;
	ARGB argb = Color::MakeARGB(200, r, g, b);
	m_crColor.SetValue(argb);

	for(int i = 0; i < 3; i++)
	{
		m_rcCircle[i].SetRectEmpty();
		m_fRadio[i] = 0.0f;
	}

	m_fRadio[3] = 0.0f;
}

void CToolProtractor::InitDefaultData()
{
	int nCenterx = GetSystemMetrics(SM_CXSCREEN) / 2;
	int nCentery = GetSystemMetrics(SM_CYSCREEN) / 2;

	m_nPtCount = 2;
	m_ptary.SetSize(2, 2);
	m_ptary[0].X = nCenterx - 250.0f;
	m_ptary[0].Y = nCentery - 250.0f;
	m_ptary[1].X = nCenterx + 250.0f;
	m_ptary[1].Y = nCentery + 250.0f;

	m_bEdit = TRUE;
	m_bSelected = TRUE;
	InitHotData();
}

void CToolProtractor::Draw(CDC* pDC, CRect& rcInvalid)
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
	graph.DrawArc(&penDraw, CRect2RectF(m_rcHot), - 182, 184);

	for(int i = 0; i < 3; i++)
	{
		graph.DrawArc(&penDraw, CRect2RectF(m_rcCircle[i]), - 180, 180);
	}

	DrawProtractorMarks(graph);

	mat.Reset();
	PointF ptTopCenter(m_ptCenter.X, (float)m_rcHot.top);
	mat.RotateAt(- 92, m_ptCenter, MatrixOrderAppend);
	mat.TransformPoints(&ptTopCenter, 1);
	PointF ptLeft = ptTopCenter;
	mat.Reset();
	mat.RotateAt(184, m_ptCenter, MatrixOrderAppend);
	mat.TransformPoints(&ptTopCenter, 1);
	PointF ptRight = ptTopCenter;

	graph.DrawLine(&penDraw, ptLeft, ptRight);

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}
}

void CToolProtractor::FinishGraph()
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

void CToolProtractor::SetRegion()
{
	if(m_nPtCount < 2)
	{
		return;
	}

	GraphicsPath path;
	float left = min(m_ptary[0].X, m_ptary[1].X);
	float top = min(m_ptary[0].Y, m_ptary[1].Y);
	float width = fabs(m_ptary[0].X - m_ptary[1].X);
	float height = fabs(m_ptary[0].Y - m_ptary[1].Y);

	RectF rcf(left, top, width, height);
	path.AddPie(rcf, -180, 180);

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CToolProtractor::SetEraseRegion(Region* pRgn)
{
}

void CToolProtractor::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
}

void CToolProtractor::AddPoint(UINT nMouseState,CPoint point, CRect& rcUpdate)
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
		SetRegion();
	}
}

void CToolProtractor::InitHotData()
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

	m_fRadio[0] = m_rcHot.Width() * 0.4f;
	m_fRadio[1] = m_fRadio[0] * 0.8f;
	m_fRadio[2] = m_fRadio[1] * 0.2f;
	m_fRadio[3] = m_rcHot.Width() / 2.0f;

	for(int i = 0; i < 3; i++)
	{
		m_rcCircle[i].SetRect((int)(m_ptCenter.X - m_fRadio[i]),
			(int)(m_ptCenter.Y - m_fRadio[i]),
			(int)(m_ptCenter.X + m_fRadio[i]),
			(int)(m_ptCenter.Y + m_fRadio[i]));
	}

	SetRegion();
}

ModifyType CToolProtractor::AroundHotPts(CPoint point)
{
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	float fgapCenter = CalcGapOfpoints(pt, m_ptCenter);
	float fgapRotate = CalcGapOfpoints(pt, m_HotPts.ptRotate);

	if(fgapRotate <= 2 * MININTERVAL)
	{
		return MT_ROTATE;
	}
	else if(m_Region.IsVisible(pt))
	{
		if(fgapCenter >= m_fRadio[1] && fgapCenter <= m_fRadio[0])
		{
			return MT_TC;
		}
		else
		{
			return MT_MOVE;
		}
	}
	else
	{
		return MT_NONE;
	}
}

void CToolProtractor::DrawHotShape(Graphics& graph)
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

void CToolProtractor::DecideModiFyType(UINT nMouseState,CPoint point)
{
	if(nMouseState != MS_LBTNDOWN)
	{
		return;
	}

	//Translate the point for the point has been considered rotated so rotated back
	PointF pt((float)point.x,(float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle,m_ptCenter);
	mat.TransformPoints(&pt,1);

	float fgapRotate = CalcGapOfpoints(pt, m_HotPts.ptRotate);
	float fgapCenter = CalcGapOfpoints(pt, m_ptCenter);

	if(fgapRotate <= 2 * MININTERVAL)
	{
		m_ModifyType = MT_ROTATE;
	}
	else if(m_Region.IsVisible(pt))
	{
		if(fgapCenter >= m_fRadio[1] && fgapCenter <= m_fRadio[0])
		{
			m_ModifyType = MT_TC;
			m_ptModifyOrg = pt;
		}
		else
		{
			m_ModifyType = MT_MOVE;
			m_ptModifyOrg.X = (float)point.x;
			m_ptModifyOrg.Y = (float)point.y;
		}
	}
	else
	{
		m_ModifyType = MT_NONE;
	}

	AdjustGraph(m_fAngle);
}

void CToolProtractor::MoveGraph(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	float offsetx = pt.X - m_ptModifyOrg.X;
	float offsety = pt.Y - m_ptModifyOrg.Y;
	m_ptModifyOrg.X = pt.X;
	m_ptModifyOrg.Y = pt.Y;

	MoveGraph(offsetx, offsety);
}

int CToolProtractor::MoveGraph(float offsetx,float offsety)
{
	m_ptary[0].X += offsetx;
	m_ptary[0].Y += offsety;
	m_ptary[1].X += offsetx;
	m_ptary[1].Y += offsety;

	m_rcHot.OffsetRect((int)offsetx, (int)offsety);
	m_rcCircle[0].OffsetRect((int)offsetx, (int)offsety);
	m_rcCircle[1].OffsetRect((int)offsetx, (int)offsety);
	m_rcCircle[2].OffsetRect((int)offsetx, (int)offsety);

	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	m_HotPts.ptRotate.X = m_ptCenter.X;
	m_HotPts.ptRotate.Y = m_rcHot.top - 3.0f * HOTINTERVAL;

	return ER_NO_ERRO;
}

void CToolProtractor::StretchGraph(CPoint point)
{
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	float fnowLength = CalcGapOfpoints(pt, m_ptCenter);
	float fpreLength = CalcGapOfpoints(m_ptModifyOrg, m_ptCenter);
	float fratio = fnowLength / fpreLength;
	m_ptModifyOrg = pt;

	m_fRadio[0] *= fratio;
	m_fRadio[1] *= fratio;
	m_fRadio[2] *= fratio;
	m_fRadio[3] *= fratio;

	m_ptary[0].X = m_ptCenter.X - m_fRadio[3];
	m_ptary[0].Y = m_ptCenter.Y - m_fRadio[3];
	m_ptary[1].X = m_ptCenter.X + m_fRadio[3];
	m_ptary[1].Y = m_ptCenter.Y + m_fRadio[3];

	m_rcHot.left = (int)min(m_ptary[0].X, m_ptary[1].X);
	m_rcHot.right = (int)max(m_ptary[0].X, m_ptary[1].X);
	m_rcHot.top = (int)min(m_ptary[0].Y, m_ptary[1].Y);
	m_rcHot.bottom = (int)max(m_ptary[0].Y, m_ptary[1].Y);

	m_HotPts.ptRotate.X = m_ptCenter.X;
	m_HotPts.ptRotate.Y = m_rcHot.top - 3.0f * HOTINTERVAL;

	for(int i = 0; i < 3; i++)
	{
		m_rcCircle[i].SetRect((int)(m_ptCenter.X - m_fRadio[i]),
			(int)(m_ptCenter.Y - m_fRadio[i]),
			(int)(m_ptCenter.X + m_fRadio[i]),
			(int)(m_ptCenter.Y + m_fRadio[i]));
	}
	
	SetRegion();
}

void CToolProtractor::DrawProtractorMarks(Graphics& graph)
{
	float pi = 3.1415926535f;
	Pen penDraw(Color::Black, 1);

	CString strNumber;
	Font fontNumber(L"Arial", m_fRadio[2] * 0.25f);
	SolidBrush brush(Color::Black);
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	graph.SetTextRenderingHint(TextRenderingHintAntiAlias);

	float fcos = 1.0f;
	float fsin = 0.0f;
	float fAngle = 0;
	float fdegree = pi / 180.0f;
	float fdeltadegree = fdegree * 1.4f;
	float fRadio1 = m_fRadio[0] * 1.2f;
	float fRadio2 = m_fRadio[0] * 1.145f;
	float fRadio3 = m_fRadio[0] * 1.17f;

	PointF ptEnd1(fRadio1, 0);
	PointF ptEnd2(m_fRadio[3], 0);
	PointF ptTemp1 = ptEnd1;
	PointF ptTemp2 = ptEnd2;

	for(int i = 0; i <= 180; i ++)
	{
		if(0 == i % 10)
		{
			ptTemp1.X = fRadio2;
		}
		else if(0 == i % 5)
		{
			ptTemp1.X = fRadio3;
		}

		fcos = cos(fAngle);
		fsin = sin(fAngle);

		ptEnd1.X = ptTemp1.X * fcos + ptTemp1.Y * fsin + m_ptCenter.X;
		ptEnd1.Y = ptTemp1.Y * fcos - ptTemp1.X * fsin + m_ptCenter.Y;

		ptEnd2.X = ptTemp2.X * fcos + ptTemp2.Y * fsin + m_ptCenter.X;
		ptEnd2.Y = ptTemp2.Y * fcos - ptTemp2.X * fsin + m_ptCenter.Y;

		graph.DrawLine(&penDraw, ptEnd1, ptEnd2);

		if(0 == i % 10)
		{
			ptTemp1.X = m_fRadio[2];
			ptEnd1.X = ptTemp1.X * fcos + ptTemp1.Y * fsin + m_ptCenter.X;
			ptEnd1.Y = ptTemp1.Y * fcos - ptTemp1.X * fsin + m_ptCenter.Y;

			ptTemp2.X = m_fRadio[1];
			ptEnd2.X = ptTemp2.X * fcos + ptTemp2.Y * fsin + m_ptCenter.X;
			ptEnd2.Y = ptTemp2.Y * fcos - ptTemp2.X * fsin + m_ptCenter.Y;
			graph.DrawLine(&penDraw, ptEnd1, ptEnd2);

			ptTemp2.X = m_fRadio[3];
		}

		fAngle += fdegree;
		ptTemp1.X = fRadio1;
	}

	fAngle = 0.0f;
	fRadio1 = m_fRadio[0] * 1.08f;
	fRadio2 = m_fRadio[0] * 0.91f;
	fRadio3 = 10 * fdegree;

	for(int i = 0; i <= 180; i += 10)
	{
		ptTemp1.X = fRadio1;
		if( i < 80)
		{
			fcos = cos(fAngle + fdeltadegree);
			fsin = sin(fAngle + fdeltadegree);
		}
		else if( i > 100)
		{
			fcos = cos(fAngle - fdeltadegree);
			fsin = sin(fAngle - fdeltadegree);
		}
		else
		{
			fcos = cos(fAngle);
			fsin = sin(fAngle);
		}

		ptEnd1.X = ptTemp1.X * fcos + ptTemp1.Y * fsin + m_ptCenter.X;
		ptEnd1.Y = ptTemp1.Y * fcos - ptTemp1.X * fsin + m_ptCenter.Y;

		ptTemp1.X = fRadio2;
		ptEnd2.X = ptTemp1.X * fcos + ptTemp1.Y * fsin + m_ptCenter.X;
		ptEnd2.Y = ptTemp1.Y * fcos - ptTemp1.X * fsin + m_ptCenter.Y;
	
		strNumber.Format(_T("%d"), i);
		graph.DrawString(strNumber, strNumber.GetLength(), &fontNumber, ptEnd1, &format, &brush);

		strNumber.Format(_T("%d"), 180 - i);
		graph.DrawString(strNumber, strNumber.GetLength(), &fontNumber, ptEnd2, &format, &brush);

		fAngle += fRadio3;
	}

	ptEnd1.X = (float)m_rcCircle[0].left;
	ptEnd1.Y = (float)m_ptCenter.Y;
	ptEnd2.X = (float)m_rcCircle[0].right;
	ptEnd2.Y = (float)m_ptCenter.Y;

	penDraw.SetWidth(2);
	graph.DrawLine(&penDraw, ptEnd1, ptEnd2);

	penDraw.SetWidth(1);
	ptEnd1.X = m_ptCenter.X;
	ptEnd1.Y = (float)m_rcCircle[2].top;
	graph.DrawLine(&penDraw, ptEnd1, m_ptCenter);
}

BOOL CToolProtractor::IsPtInModifyZone(CPoint point)
{
	BOOL bret = TRUE;
	PointF pt((float)point.x,(float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle,m_ptCenter);
	mat.TransformPoints(&pt,1);

	if(CalcGapOfpoints(pt,m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
	}
	else if(IsPtInHotRegion(point))
	{
	}
	else
	{
		bret = FALSE;
	}

	return bret;
}