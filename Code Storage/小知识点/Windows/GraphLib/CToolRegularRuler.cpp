#include "stdafx.h"
#include "GraphLib.h"

CToolRegularRuler::CToolRegularRuler()
{
	InitDataMember();
	InitDefaultData();
}

CToolRegularRuler::CToolRegularRuler(const CToolRegularRuler& gph)
{
	CopyTheBaseDataMember(&gph);
	m_rcGrip = gph.m_rcGrip;
}

CToolRegularRuler& CToolRegularRuler::operator = (const CToolRegularRuler& gph)
{
	CopyTheBaseDataMember(&gph);
	m_rcGrip = gph.m_rcGrip;

	return *this;
}

CToolRegularRuler::~CToolRegularRuler(void)
{
}

void CToolRegularRuler::InitDataMember()
{
	BYTE r = 0;
	BYTE g = 0;
	BYTE b = 200;
	ARGB argb = Color::MakeARGB(200, r, g, b);
	m_crColor.SetValue(argb);

	m_rcGrip.SetRectEmpty();
	m_ptary.SetSize(2, 2);
}

void CToolRegularRuler::InitDefaultData()
{
	int nCenterx = GetSystemMetrics(SM_CXSCREEN) / 2;
	int nCentery = GetSystemMetrics(SM_CYSCREEN) / 2;

	m_nPtCount = 2;
	m_ptary[0].X = nCenterx - 400.0f;
	m_ptary[0].Y = nCentery - 35.0f;
	m_ptary[1].X = nCenterx + 400.0f;
	m_ptary[1].Y = nCentery + 35.0f;
	
	m_bEdit = TRUE;
	m_bSelected = TRUE;
	InitHotData();
}

void CToolRegularRuler::Draw(CDC* pDC, CRect& rcInvalid)
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

	RectF rcf(m_ptary[0].X, m_ptary[0].Y,
		m_ptary[1].X - m_ptary[0].X,
		m_ptary[1].Y - m_ptary[0].Y);

	DrawRulerMarks(graph);
	graph.DrawRectangle(&penDraw, rcf);

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}
}

void CToolRegularRuler::FinishGraph()
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

void CToolRegularRuler::SetRegion()
{
	if(!m_bFinish)
	{
		return;
	}

	float left = min(m_ptary[0].X, m_ptary[1].X);
	float top = min(m_ptary[0].Y, m_ptary[1].Y);
	float width = fabs(m_ptary[0].X - m_ptary[1].X);
	float height = fabs(m_ptary[0].Y - m_ptary[1].Y);

	RectF rcf(left, top, width, height);

	m_Region.MakeEmpty();
	m_Region.Union(rcf);
}

void CToolRegularRuler::SetEraseRegion(Region* pRgn)
{

}

void CToolRegularRuler::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x, (float)point.y);
	if(MS_LBTNDOWN	== nMouseState)
	{
		m_ptary[0] = pt;
		m_nPtCount = 1;
	}
	else
	{
		m_ptary[1] = pt;
		m_nPtCount = 2;
	}

	if(MS_LBTNUP == nMouseState)
	{
		if(CalcGapOfpoints(m_ptary[0], m_ptary[1]) <= 3)
		{
			m_bDataInvalid = TRUE;
		}
		else
		{
			m_bFinish = TRUE;
			InitHotData();
			rcUpdate = m_rcHot;
		}
	}
}

void CToolRegularRuler::AddPoint(UINT nMouseState,CPoint point, CRect& rcUpdate)
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

void CToolRegularRuler::InitHotData()
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

	m_rcGrip.SetRect(m_rcHot.right - 30, m_rcHot.top,
					 m_rcHot.right, m_rcHot.bottom);

	m_HotPts.ptRotate.X = m_ptCenter.X;
	m_HotPts.ptRotate.Y = m_rcHot.top - 3.0f * HOTINTERVAL;

	SetRegion();
}

ModifyType CToolRegularRuler::AroundHotPts(CPoint point)
{
	PointF pt((float)point.x, (float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle, m_ptCenter);
	mat.TransformPoints(&pt, 1);

	if(m_rcGrip.PtInRect(CPoint((int)pt.X, (int)pt.Y))/*CalcGapOfpoints(pt,m_HotPts.ptRC) <= HOTINTERVAL*/)
	{
		return MT_RC;
	}
	else if(CalcGapOfpoints(pt, m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		return MT_ROTATE;
	}
	else if(m_rcHot.PtInRect(CPoint((int)pt.X, (int)pt.Y)))
	{
		return MT_MOVE;
	}
	else
	{
		return MT_NONE;
	}
}

void CToolRegularRuler::DrawHotShape(Graphics& graph)
{
	SolidBrush sbrush(Color::Green);
	Pen penDraw(Color::Blue, 2);
	penDraw.SetDashStyle(DashStyleDot);

	PointF pt((m_ptary[0].X + m_ptary[1].X) / 2, m_ptary[0].Y);
	
	graph.DrawLine(&penDraw, pt, m_HotPts.ptRotate);
	graph.FillEllipse(&sbrush, m_HotPts.ptRotate.X - HOTINTERVAL,
					  m_HotPts.ptRotate.Y - HOTINTERVAL,
					  2.0 * HOTINTERVAL, 2.0 * HOTINTERVAL);

	penDraw.SetDashStyle(DashStyleDash);
	penDraw.SetColor(Color::Red);

	graph.DrawRectangle(&penDraw, m_rcGrip.left, m_rcGrip.top, m_rcGrip.Width(), m_rcGrip.Height());
}

void CToolRegularRuler::DecideModiFyType(UINT nMouseState,CPoint point)
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

	if(m_rcGrip.PtInRect(CPoint((int)pt.X,(int)pt.Y)))
	{
		m_ModifyType = MT_RC;
		m_ptModifyOrg = pt;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		m_ModifyType = MT_ROTATE;
	}
	else if(m_rcHot.PtInRect(CPoint((int)pt.X,(int)pt.Y)))
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

void CToolRegularRuler::MoveGraph(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	float offsetx = pt.X - m_ptModifyOrg.X;
	float offsety = pt.Y - m_ptModifyOrg.Y;
	m_ptModifyOrg.X = pt.X;
	m_ptModifyOrg.Y = pt.Y;

	MoveGraph(offsetx, offsety);
}

int CToolRegularRuler::MoveGraph(float offsetx,float offsety)
{
	m_ptary[0].X += offsetx;
	m_ptary[0].Y += offsety;
	m_ptary[1].X += offsetx;
	m_ptary[1].Y += offsety;

	m_rcHot.OffsetRect((int)offsetx, (int)offsety);

	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	m_rcGrip.SetRect(m_rcHot.right - 30, m_rcHot.top,
		m_rcHot.right, m_rcHot.bottom);

	m_HotPts.ptRotate.X = m_ptCenter.X;
	m_HotPts.ptRotate.Y = m_rcHot.top - 3.0f * HOTINTERVAL;

	return ER_NO_ERRO;
}

void CToolRegularRuler::StretchGraph(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle,m_ptCenter);
	mat.TransformPoints(&pt,1);

	float offsetx = pt.X - m_ptModifyOrg.X;
	float offsety = pt.Y - m_ptModifyOrg.Y;
	m_ptModifyOrg.X = pt.X;
	m_ptModifyOrg.Y = pt.Y;

	if(m_ptary[1].X + offsetx <= m_ptary[0].X + 130)
	{
		return;
	}

	m_ptary[1].X += offsetx;

	m_rcHot.left = (int)min(m_ptary[0].X, m_ptary[1].X);
	m_rcHot.right = (int)max(m_ptary[0].X, m_ptary[1].X);
	m_rcHot.top = (int)min(m_ptary[0].Y, m_ptary[1].Y);
	m_rcHot.bottom = (int)max(m_ptary[0].Y, m_ptary[1].Y);

	m_rcGrip.SetRect(m_rcHot.right - 30, m_rcHot.top,
		m_rcHot.right, m_rcHot.bottom);

	m_HotPts.ptRotate.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_HotPts.ptRotate.Y = m_rcHot.top - 3.0f * HOTINTERVAL;
}

void CToolRegularRuler::DrawRulerMarks(Graphics& graph)
{
	int nNumber = 0;
	int nbegin = m_rcHot.left + 10;
	int nend = m_rcHot.right - 10;

	CString strNumber;
	Font fontNumber(L"Arial", 10);
	SolidBrush brush(Color::Black);
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);

	Pen penDraw(Color::Black, 1);
	PointF ptBegin((float)nbegin, m_rcHot.top + 1.0f);
	PointF ptEnd = ptBegin;
	graph.SetTextRenderingHint(TextRenderingHintAntiAlias);

	for(int i = nbegin; i < nend; i+= 10)
	{
		ptEnd.X = (float)i;
		ptBegin.X = (float)i;
		if((i - nbegin) % 100 == 0)
		{
			ptEnd.Y = ptBegin.Y + 30;
			graph.DrawLine(&penDraw, ptBegin, ptEnd);
			nNumber = (i - nbegin) / 100;
			strNumber.Format(_T("%d"), nNumber);
			
			graph.DrawString(strNumber, strNumber.GetLength(), &fontNumber, ptEnd, &format, &brush);
		}
		else if((i - nbegin) % 50 == 0)
		{
			ptEnd.Y = ptBegin.Y + 20;
			graph.DrawLine(&penDraw, ptBegin, ptEnd);
		}
		else
		{
			ptEnd.Y = ptBegin.Y + 10;
			graph.DrawLine(&penDraw, ptBegin, ptEnd);
		}
	}
}