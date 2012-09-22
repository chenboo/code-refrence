#include "stdafx.h"
#include "GraphLib.h"

CPicture::CPicture(CString strName)
{
	InitDataMember();
	SetImageFile(strName);
}

CPicture::CPicture()
{
	InitDataMember();
}

CPicture::CPicture(const CPicture& gph)
{
	CopyTheBaseDataMember(&gph);

	m_OrgTop = gph.m_OrgTop;
	m_OrgLeft = gph.m_OrgLeft;
	m_nOrgWidth = gph.m_nOrgWidth;
	m_nOrgHeight = gph.m_nOrgHeight;
	m_pImage = gph.m_pImage->Clone();
}

CPicture& CPicture::operator = (const CPicture& gph)
{
	CopyTheBaseDataMember(&gph);
	
	m_OrgTop = gph.m_OrgTop;
	m_OrgLeft = gph.m_OrgLeft;
	m_nOrgWidth = gph.m_nOrgWidth;
	m_nOrgHeight = gph.m_nOrgHeight;
	m_pImage = gph.m_pImage->Clone();

	return *this;
}

void CPicture::InitDataMember()
{
	m_OrgTop = 0;
	m_OrgLeft = 0;
	m_nMinCount = 2;
	m_nOrgWidth = 0;
	m_nOrgHeight = 0;
	m_ptary.SetSize(2,2);
	m_pImage = NULL;
	m_strFilename = _T("");
}

CPicture::~CPicture(void)
{
	delete m_pImage;
}

void CPicture::SetImageFile(CString strname)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(strname.IsEmpty())
	{
		return;
	}

	if(m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	m_strFilename = strname;
	m_pImage = Image::FromFile(strname);

	InitPictureData();
}

void CPicture::InitPictureData()
{
	//save the orgin size and position of the picture uesd for stretch and rotate
	m_nOrgWidth = m_pImage->GetWidth();
	m_nOrgHeight = m_pImage->GetHeight();
	m_OrgTop = (GetSystemMetrics(SM_CYSCREEN) - m_nOrgHeight) / 2;
	m_OrgLeft = (GetSystemMetrics(SM_CXSCREEN) - m_nOrgWidth) / 2;

	//if the format is emf then calculate the opacity area
	RectF rc((float)m_OrgLeft, (float)m_OrgTop, (float)m_nOrgWidth, (float)m_nOrgHeight);
	m_Region.MakeEmpty();
	m_Region.Union(rc);

	m_nPtCount = 2;
	m_ptary[0].X = (float)m_OrgLeft;
	m_ptary[0].Y = (float)m_OrgTop;
	m_ptary[1].X = (float)m_OrgLeft + m_nOrgWidth;
	m_ptary[1].Y = (float)m_OrgTop + m_nOrgHeight;

	m_bEdit = TRUE;
	m_bSelected = TRUE;
	InitHotData();
}

void CPicture::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount <= 1)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle, m_ptCenter);
	graph.SetTransform(&mat);
	graph.ExcludeClip(&m_RgnErase);

	if(m_pImage)
	{
		graph.DrawImage(m_pImage, m_rcHot.left, m_rcHot.top, m_rcHot.Width(), m_rcHot.Height());
	}

	graph.SetClip(&m_RgnErase, CombineModeUnion);

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}
}

void CPicture::FinishGraph()
{
	if(!m_strFilename.IsEmpty())
	{
		m_bFinish = TRUE;
		m_bEdit = FALSE;
		m_bSelected = FALSE;
		InitHotData();
	}

	return;
}

void CPicture::InitHotData()
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

	SetHotPoints();
	m_ptCenter.X = (m_rcHot.left + m_rcHot.right) / 2.0f;
	m_ptCenter.Y = (m_rcHot.top + m_rcHot.bottom) / 2.0f;

	SetRegion();
	m_nOrgWidth = m_rcHot.Width();
	m_nOrgHeight = m_rcHot.Height();
}

void CPicture::SetRegion()
{
	if(!m_bFinish)
	{
		return;
	}

	float scalex = (float)m_rcHot.Width() / m_nOrgWidth;
	float scaley = (float)m_rcHot.Height() / m_nOrgHeight;

	PointF ptLT((float)m_OrgLeft,(float)m_OrgTop);
	Matrix mat;
	mat.Scale(scalex,scaley);
	mat.TransformPoints(&ptLT,1);
	float offsx = m_rcHot.left - ptLT.X;
	float offsy = m_rcHot.top - ptLT.Y;

	mat.Translate(offsx,offsy,MatrixOrderAppend);
	m_Region.Transform(&mat);

	m_nOrgWidth = m_rcHot.Width();
	m_nOrgHeight = m_rcHot.Height();
	m_OrgLeft = m_rcHot.left;
	m_OrgTop = m_rcHot.top;
}

void CPicture::AddPoint(UINT nMouseState,CPoint point, CRect& rcUpdate)
{
	if(!m_bFinish)
	{
		if(MS_LBTNDOWN == nMouseState && !IsPtInModifyZone(point))
		{
			AdjustGraph(m_fAngle);
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

	DecideModiFyType(nMouseState,point);
	ModifyGraph(nMouseState,point);

	if(MS_LBTNUP == nMouseState)
	{
		SetRegion();
	}
}

ModifyType CPicture::AroundHotPts(CPoint point)
{
	PointF pt((float)point.x,(float)point.y);
	Matrix mat;
	mat.RotateAt(- m_fAngle,m_ptCenter);
	mat.TransformPoints(&pt,1);

	CRect rect = m_rcHot;
	rect.InflateRect(7,7);

	if(CalcGapOfpoints(pt,m_HotPts.ptLT) <= HOTINTERVAL)
	{
		return MT_LT;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptTC) <= HOTINTERVAL)
	{
		return MT_TC;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptRT) <= HOTINTERVAL)
	{
		return MT_RT;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptLC) <= HOTINTERVAL)
	{
		return MT_LC;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptRC) <= HOTINTERVAL)
	{
		return MT_RC;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptLB) <= HOTINTERVAL)
	{
		return MT_LB;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptBC) <= HOTINTERVAL)
	{
		return MT_BC;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptRB) <= HOTINTERVAL)
	{
		return MT_RB;
	}
	else if(CalcGapOfpoints(pt,m_HotPts.ptRotate) <= 2 * MININTERVAL)
	{
		return MT_ROTATE;
	}
	else if(rect.PtInRect(CPoint((int)pt.X,(int)pt.Y)))
	{
		return MT_MOVE;
	}
	else
	{
		return MT_NONE;
	}
}