#include "stdafx.h"
#include "GraphLib.h"

CAngleLabel::CAngleLabel(int nWidth, Color crColor, DashStyle dashstyle)
{
	m_nWidth = nWidth;
	m_crColor = crColor;
	m_ptary.SetSize(3,1);
	m_nMinCount = 3;
	m_bhas3temppts = FALSE;
	m_nDashStyle = dashstyle;
}

CAngleLabel::CAngleLabel()
{
	m_nWidth = 3;
	m_crColor = Color::Black;
	m_ptary.SetSize(3,1);
	m_nMinCount = 3;
	m_bhas3temppts = FALSE;
}

CAngleLabel::CAngleLabel(const CAngleLabel& gph)
{
	CopyTheBaseDataMember(&gph);
	m_strAngle = gph.m_strAngle;
	m_bhas3temppts = gph.m_bhas3temppts;
}

CAngleLabel& CAngleLabel::operator = (const CAngleLabel& gph)
{
	CopyTheBaseDataMember(&gph);
	m_strAngle = gph.m_strAngle;
	m_bhas3temppts = gph.m_bhas3temppts;
	
	return *this;
}

CAngleLabel::~CAngleLabel()
{
}

void CAngleLabel::Draw(CDC* pDC, CRect& rcInvalid)
{
	if(m_nPtCount <= 0)
	{
		return;
	}

	Pen penDraw(m_crColor, (float)m_nWidth);
	penDraw.SetLineJoin(LineJoinRound);
	penDraw.SetEndCap(LineCapRound);
	penDraw.SetStartCap(LineCapRound);
	penDraw.SetDashStyle(m_nDashStyle);

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle, m_ptCenter);
	graph.SetTransform(&mat);
	PointF* pPt = m_ptary.GetData();

	if(m_bhas3temppts || m_bFinish)
	{
		PointF ptd[3];
		ptd[0] = pPt[1];
		ptd[1] = pPt[0];
		ptd[2] = pPt[2];

		graph.ExcludeClip(&m_RgnErase);
		graph.DrawLines(&penDraw, ptd, 3);
		DrawAngleLabelMark(graph);
	}
	else
	{
		graph.DrawLine(&penDraw, pPt[0], pPt[1]);
	}

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}
}

void CAngleLabel::DrawAngleLabelMark(Graphics& graph)
{
	CPoint pt2((int)m_ptary[1].X, (int)m_ptary[1].Y);
	CPoint pt3((int)m_ptary[2].X, (int)m_ptary[2].Y);

	float fAngle3 = 0.0f;
	float fAngle1 = CalcRotateAngle(pt2, m_ptary[0]) - 90.0f;
	float fAngle2 = CalcRotateAngle(pt3, m_ptary[0]) - 90.0f;

	if(fAngle1 > - 90 && fAngle1 <= 0)
	{
		if(fAngle2 >= 180 && fAngle2 <= 270)
		{
			fAngle2 -= 360;
		}
		else if(fAngle2 - fAngle1 >= 180)
		{
			fAngle1 += 360;
		}
	}
	else if(fAngle1 >= 0 && fAngle1 <= 90)
	{
		if(fAngle2 >= 180)
		{
			if(fAngle2 - fAngle1 >= 180)
			{
				fAngle2 -= 360;
			}
		}
	}
	else if(fAngle1 >= 90 && fAngle1 <= 180)
	{
		if(fAngle2 >= -90 && fAngle2 <= 0)
		{
			if(fAngle1 - fAngle2 >= 180)
			{
				fAngle2 += 360;
			}
		}
	}
	else if(fAngle1 >= 180 && fAngle1 <= 270)
	{
		if(fAngle2 >= 0 && fAngle2 <= 90)
		{
			if(fAngle1 - fAngle2 >= 180)
			{
				fAngle2 += 360;
			}
		}
		else if(fAngle2 >= -90 && fAngle2 <= 0)
		{
			fAngle2 += 360;
		}
	}

	fAngle3 = (fAngle1 + fAngle2) / 2.0f;
	PointF ptDraw;
	ptDraw.Y = m_ptary[0].Y;

	if(fabs(fAngle1 - fAngle2) <= 20)
	{
		ptDraw.X = 100 + m_ptary[0].X;
	}
	else if(fabs(fAngle1 - fAngle2) <= 30)
	{
		ptDraw.X = 80 + m_ptary[0].X;
	}
	else
	{
		ptDraw.X = 60 + m_ptary[0].X;
	}

	Matrix mat;
	mat.RotateAt(fAngle3, m_ptary[0]);
	mat.TransformPoints(&ptDraw, 1);

	Pen penDraw(Color::Blue, 2);
	RectF rcf(m_ptary[0].X - 30, m_ptary[0].Y - 30, 60, 60);
	graph.DrawArc(&penDraw, rcf, fAngle1, fAngle2 - fAngle1);

	Font myFont(_T("Arial"), 12);
	SolidBrush brush(Color::Red);
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);

	int nAngle = CalLinesAngle();
	m_strAngle.Format(_T("%d¡ã"), nAngle);

	graph.SetTextRenderingHint(TextRenderingHintAntiAlias);
	graph.DrawString(m_strAngle, m_strAngle.GetLength(), &myFont, ptDraw, &format, &brush);
}

void CAngleLabel::FinishGraph()
{
	if(3 == m_nPtCount)
	{
		m_bFinish = TRUE;
		InitHotData();
	}

	return;
}

void CAngleLabel::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

	PointF ptd[3];
	ptd[0] = pPt[1];
	ptd[1] = pPt[0];
	ptd[2] = pPt[2];

	Pen penDraw(Color(255, 0, 255, 0));
	penDraw.SetDashStyle(m_nDashStyle);
	penDraw.SetWidth((float)m_nWidth);
	
	GraphicsPath path;
	path.AddLines(ptd, m_nPtCount);

	path.Widen(&penDraw);
	path.Outline();

	m_Region.MakeEmpty();
	m_Region.Union(&path);
}

void CAngleLabel::FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x, (float)point.y);
	if(MS_LBTNDOWN == nMouseState)
	{
		m_ptary[m_nPtCount] = pt;
		if(m_nPtCount == 0)
		{
			m_nPtCount++;
			m_ptary[m_nPtCount] = pt;
		}
	}
	if(m_nPtCount >= 1)
	{
		if(MS_LBTNMOUSE == nMouseState)
		{
			m_ptary[m_nPtCount] = pt;
			if(m_nPtCount == 2)
			{
				m_bhas3temppts = TRUE;
			}
		}
		else if(MS_LBTNUP == nMouseState)
		{
			if(m_nPtCount == 1)
			{
				if(pt.Equals(m_ptary[0]))
				{
					return;
				}
			}

			m_ptary[m_nPtCount] = pt;
			m_nPtCount++;

			if(m_nPtCount == 3)
			{
				m_bFinish = TRUE;
				m_bhas3temppts = FALSE;
				InitHotData();
				rcUpdate = m_rcHot;
			}
			else
			{
				m_ptary[m_nPtCount] = pt;
			}
		}
	}
	else
	{
		rcUpdate.SetRect(0, 0, 1, 1);
	}
}


int CAngleLabel::CalLinesAngle()
{
	ASSERT(m_bhas3temppts || m_bFinish);

	float a = CalcGapOfpoints(m_ptary[0], m_ptary[1]);
	float b = CalcGapOfpoints(m_ptary[1], m_ptary[2]);
	float c = CalcGapOfpoints(m_ptary[2], m_ptary[0]);

	if(a <= 1 || c <= 1)
	{
		return 0;
	}
	else
	{
		float fvalue = (a * a + c * c - b * b) / (2 * a * c);
		float PI = 3.1415926f;
		float fangle = (float)(acos(fvalue) * 180 / PI);

		if(fabs(fangle - 180) < 0.1f)
		{
			fangle = 180.0f;
		}
		else if(fabs(fangle) < 0.1f)
		{
			fangle = 0.0f;
		}

		if(fangle < 0 || fangle > 180)
		{
			return 0;
		}
		else
		{
			return (int)(fangle + 0.5f);
		}
	}
}

void CAngleLabel::InitHotData()
{
	int nleft = (int)min(min(m_ptary[0].X, m_ptary[1].X), m_ptary[2].X);
	int ntop = (int)min(min(m_ptary[0].Y, m_ptary[1].Y), m_ptary[2].Y);
	int nright = (int)max(max(m_ptary[0].X, m_ptary[1].X), m_ptary[2].X);
	int nbottom = (int)max(max(m_ptary[0].Y, m_ptary[1].Y), m_ptary[2].Y);
	m_rcHot.SetRect(nleft, ntop, nright, nbottom);

	float fside1 = CalcGapOfpoints(m_ptary[0], m_ptary[1]);
	float fside2 = CalcGapOfpoints(m_ptary[0], m_ptary[2]);

	if(fside1 >= 100 && fside2 >= 100)
	{
		m_rcHot.InflateRect(HOTINTERVAL, HOTINTERVAL);
	}
	else if(fside1 >= 50 && fside2 >= 50)
	{
		m_rcHot.InflateRect(5 * HOTINTERVAL, 5 * HOTINTERVAL);
	}
	else if(fside1 >= 30 && fside2 >= 30)
	{
		m_rcHot.InflateRect(8 * HOTINTERVAL, 8 * HOTINTERVAL);
	}
	else if(fside1 >= 20 && fside2 >= 20)
	{
		m_rcHot.InflateRect(10 * HOTINTERVAL, 10 * HOTINTERVAL);
	}
	else
	{
		m_rcHot.InflateRect(15 * HOTINTERVAL, 15 * HOTINTERVAL);
	}

	if(m_rcHot.Width() <= 200)
	{
		m_rcHot.InflateRect(8 * HOTINTERVAL, 0);
	}
	else if(m_rcHot.Height() <= 200)
	{
		m_rcHot.InflateRect(0, 8 * HOTINTERVAL);
	}

	m_ptCenter.X = (float)m_rcHot.CenterPoint().x;
	m_ptCenter.Y = (float)m_rcHot.CenterPoint().y;

	SetRegion();
}

void CAngleLabel::DrawHotShape(Graphics& graph)
{
	SolidBrush sbrush(Color::White);
	Pen penDraw(Color::Blue, 3);

	PointF ptLT;
	ptLT.X = m_ptary[0].X - m_nWidth;
	ptLT.Y = m_ptary[0].Y - m_nWidth;

	Rect rect((int)ptLT.X, (int)ptLT.Y, m_nWidth * 2, m_nWidth * 2);
	graph.DrawEllipse(&penDraw, rect);
	graph.FillEllipse(&sbrush, rect);

	ptLT.X = m_ptary[1].X - m_nWidth;
	ptLT.Y = m_ptary[1].Y - m_nWidth;

	rect.X = (int)ptLT.X;
	rect.Y = (int)ptLT.Y;
	rect.Width = m_nWidth * 2;
	rect.Height = m_nWidth * 2;

	graph.DrawEllipse(&penDraw, rect);
	graph.FillEllipse(&sbrush, rect);

	ptLT.X = m_ptary[2].X - m_nWidth;
	ptLT.Y = m_ptary[2].Y - m_nWidth;

	rect.X = (int)ptLT.X;
	rect.Y = (int)ptLT.Y;
	rect.Width = m_nWidth * 2;
	rect.Height = m_nWidth * 2;

	graph.DrawEllipse(&penDraw, rect);
	graph.FillEllipse(&sbrush, rect);
}

void CAngleLabel::DecideModiFyType(UINT nMouseState,CPoint point)
{
	if(nMouseState != MS_LBTNDOWN)
		return;

	PointF pt((float)point.x, (float)point.y);

	if(CalcGapOfpoints(pt, m_ptary[0]) <= m_nWidth)
	{
		m_ModifyType = MT_LT;
	}
	else if(CalcGapOfpoints(pt, m_ptary[1]) <= m_nWidth)
	{
		m_ModifyType = MT_RB;
	}
	else if(CalcGapOfpoints(pt, m_ptary[2]) <= m_nWidth)
	{
		m_ModifyType = MT_RC;
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

void CAngleLabel::ModifyGraph(UINT nMouseState, CPoint point)
{
	if(MS_LBTNMOUSE != nMouseState)
	{
		return;
	}

	if(MT_MOVE == m_ModifyType)
	{
		MoveGraph(point);
	}
	else if(MT_NONE != m_ModifyType)
	{
		StretchGraph(point);
	}
}

void CAngleLabel::StretchGraph(CPoint point)
{
	if(MT_LT == m_ModifyType)
	{
		m_ptary[0].X = (float)point.x;
		m_ptary[0].Y = (float)point.y;
	}
	else if(MT_RB == m_ModifyType)
	{
		m_ptary[1].X = (float)point.x;
		m_ptary[1].Y = (float)point.y;
	}
	else if(MT_RC == m_ModifyType)
	{
		m_ptary[2].X = (float)point.x;
		m_ptary[2].Y = (float)point.y;
	}

	InitHotData();
}

ModifyType CAngleLabel::AroundHotPts(CPoint point)
{
	if(!m_bFinish)
	{
		return MT_NONE;
	}

	PointF pt((float)point.x,(float)point.y);

	if(CalcGapOfpoints(pt,m_ptary[0]) <= m_nWidth)
	{
		return MT_TC;
	}
	else if(CalcGapOfpoints(pt,m_ptary[1]) <= m_nWidth)
	{
		return MT_TC;
	}
	else if(CalcGapOfpoints(pt,m_ptary[2]) <= m_nWidth)
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

BOOL CAngleLabel::IsPtInModifyZone(CPoint point)
{
	BOOL bret = TRUE;
	PointF pt((float)point.x,(float)point.y);

	if(CalcGapOfpoints(pt,m_ptary[0]) <= m_nWidth)
	{
	}
	else if(CalcGapOfpoints(pt,m_ptary[1]) <= m_nWidth)
	{
	}
	else if(CalcGapOfpoints(pt,m_ptary[2]) <= m_nWidth)
	{
	}
	else if(m_Region.IsVisible(pt))
	{
	}
	else
	{
		bret = FALSE;
	}

	return bret;
}