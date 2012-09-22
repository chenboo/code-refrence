#include "stdafx.h"
#include "CEMFile.h"

CEMFile::CEMFile(CString strName)
{
	m_strFilename = _T("");
	m_ptary.SetSize(2,2);
	m_nMinCount = 2;
	m_pImage = NULL;
	m_nOrgWidth = 0;
	m_nOrgHeight = 0;
	m_OrgLeft = 0;
	m_OrgTop = 0;

	LoadMetaFile(strName);
}

CEMFile::CEMFile()
{

}

CEMFile::CEMFile(const CEMFile& gph)
{
	CopyBase(&gph);	

	m_ptary.RemoveAll();
	m_ptary.SetSize(2,2);
	m_ptary.Copy(gph.m_ptary);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);
	
	m_pImage = gph.m_pImage->Clone();	

	m_nOrgWidth = gph.m_nOrgWidth;
	m_nOrgHeight = gph.m_nOrgHeight;
	m_OrgLeft = gph.m_OrgLeft;
	m_OrgTop = gph.m_OrgTop;
}

CEMFile::~CEMFile(void)
{
	delete m_pImage;
}

void CEMFile::Draw(CDC* pDC)
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

	if(!m_bFinish)
	{
		Pen pent(Color::Blue,2);
		pent.SetDashStyle(DashStyleDot);

		CPoint ptLT;
		CPoint ptRB;
		ptLT.x = (int)m_ptary[0].X;
		ptLT.y = (int)m_ptary[0].Y;
		ptRB.x = (int)m_ptary[1].X;
		ptRB.y = (int)m_ptary[1].Y;

		CRect rc(ptLT,ptRB);
		rc.NormalizeRect();
		graph.DrawRectangle(&pent,rc.left,rc.top,rc.Width(),rc.Height());
	}
	else
	{
		graph.DrawImage(m_pImage,m_rcHot.left,m_rcHot.top,m_rcHot.Width(),m_rcHot.Height());

		if(m_bSelected)
		{
			DrawHotShape(graph);
		}
	}
}

void CEMFile::AddPoint(UINT nMouseState,CPoint point)
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

void CEMFile::FinishGraph()
{
	if(!m_strFilename.IsEmpty())
	{
		m_bFinish = TRUE;
		InitHotData();
	}
	return;
}

void CEMFile::FormGraphData(UINT nMouseState,CPoint point)
{
	if(m_strFilename.IsEmpty())
	{
		return;
	}

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
	}
	if(MS_LBTNUP == nMouseState)
	{
		m_bFinish = TRUE;
		InitHotData();
	}
}

void CEMFile::LoadMetaFile(CString strname)
{
	if(strname.IsEmpty())
	{
		delete m_pImage;
		return;
	}
	if(m_pImage)
	{
		delete m_pImage;
	}

	m_strFilename = strname;

	m_pImage = Image::FromFile(strname);

	InitRegion(strname);
}

void CEMFile::InitRegion(CString strname)
{
	CImage image;
	image.Load(strname);
	HRGN hrgn = BitmapToRegion((HBITMAP)image, RGB(0,0,0));

	Region region(hrgn);
	m_Region.MakeEmpty();
	m_Region.Union(&region);

	m_nOrgWidth = m_pImage->GetWidth();
	m_nOrgHeight = m_pImage->GetHeight();
}

void CEMFile::SetRegion()
{
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
