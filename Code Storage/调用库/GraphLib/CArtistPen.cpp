#include "stdafx.h"
#include "CArtistPen.h"

CArtistPen::CArtistPen(int nAPStyle)
{
	m_nWidth = 20;
	m_nAPstyle = nAPStyle;
	m_ptary.SetSize(0,10);
	m_noffst = 0;
	m_nMinCount = 2;

	m_pImg = NULL;
}

CArtistPen::CArtistPen()
{

}

CArtistPen::CArtistPen(const CArtistPen& gph)
{
	CopyBase(&gph);

	m_ptary.RemoveAll();
	m_ptary.SetSize(0,10);
	m_ptary.Copy(gph.m_ptary);

	m_Region.MakeEmpty();
	m_Region.Union(&gph.m_Region);

	m_nAPstyle = gph.m_nAPstyle;
	m_noffst = gph.m_noffst;
	m_pImg = NULL;

	if(AP_5STARS == m_nAPstyle
		|| AP_SMILE == m_nAPstyle)
	{
		ASSERT(gph.m_pImg);
		m_pImg = gph.m_pImg->Clone();
	}
}

CArtistPen::~CArtistPen()
{
}

void CArtistPen::AddPoint(UINT nMouseState,CPoint point)
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

void CArtistPen::Draw(CDC* pDC)
{
	if(m_nPtCount == 0)
		return;
	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	if(AP_BRUSH == m_nAPstyle)
	{
		DrawBrush(graph);
	}
	else if(AP_3COLORS == m_nAPstyle)
	{
		Draw3Clr(graph);
	}
	else
	{
		DrawImagelines(graph,m_pImg);	
	}

	if(m_bSelected)
	{
		DrawHotShape(graph);
	}
}

void CArtistPen::DrawBrush(Graphics& graph)
{
	Pen penDraw(m_crColor,(float)m_nWidth);
	penDraw.SetStartCap(LineCapRound);
	penDraw.SetEndCap(LineCapRound);
	penDraw.SetLineJoin(LineJoinRound);

	PointF* pPt = m_ptary.GetData();
	graph.DrawCurve(&penDraw,pPt,m_nPtCount);
}

void CArtistPen::Draw3Clr(Graphics& graph)
{
	Pen penDraw(m_crColor, (float)m_nWidth);

	SolidBrush bblue(Color::Blue);
	SolidBrush byellow(Color::Yellow);
	SolidBrush bred(Color::Red);

	Pen penB(&bblue,(float)m_nWidth);
	Pen penY(&byellow,m_nWidth * 0.7f);
	Pen penR(&bred,m_nWidth*0.4f);

	penB.SetStartCap(LineCapRound);
	penB.SetEndCap(LineCapRound);
	penB.SetLineJoin(LineJoinRound);

	penY.SetStartCap(LineCapRound);
	penY.SetEndCap(LineCapRound);
	penY.SetLineJoin(LineJoinRound);

	penR.SetStartCap(LineCapRound);
	penR.SetEndCap(LineCapRound);
	penR.SetLineJoin(LineJoinRound);

	PointF* pPt = m_ptary.GetData();
	graph.DrawCurve(&penB,pPt,m_nPtCount);
	graph.DrawCurve(&penY,pPt,m_nPtCount);
	graph.DrawCurve(&penR,pPt,m_nPtCount);
}

void CArtistPen::FinishGraph()
{
	m_bFinish = TRUE;
	InitHotData();
	return;
}

void CArtistPen::SetArtistPenStyle(int nAPstyle)
{
	m_nAPstyle = nAPstyle;
	if(m_pImg)
	{
		delete m_pImg;
		m_pImg = NULL;
	}
}

void CArtistPen::FormGraphData(UINT nMouseState,CPoint point)
{
	PointF pt((float)point.x,(float)point.y);

	if(AP_5STARS == m_nAPstyle || AP_SMILE == m_nAPstyle)
	{
		FormDarwImageData(pt,36.0);
	}
	else
	{
		if(m_noffst % 5 == 0)
		{
			m_ptary.Add(pt);
			m_nPtCount++;
		}
		m_noffst++;
	}
	if(MS_LBTNUP == nMouseState)
	{
		m_bFinish = TRUE;
		if(AP_5STARS == m_nAPstyle || AP_SMILE == m_nAPstyle)
		{
			InitHotData(TRUE);
		}
		else
		{
			InitHotData();
		}
	}
}

void CArtistPen::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

	if(AP_5STARS == m_nAPstyle || AP_SMILE == m_nAPstyle)
	{
		SetpngRegion();
		PointF pt;
		float offsetx,offsety;
		Region region;
		region.MakeEmpty();
		Matrix mat;

		Region* pRgn = m_Region.Clone();
		offsetx = m_ptary[0].X - 18.0f;
		offsety = m_ptary[0].Y - 18.0f;
		mat.Translate(offsetx,offsety,MatrixOrderAppend);
		pRgn->Transform(&mat);
		region.Union(pRgn);

		for(int i = 1;i < m_nPtCount;i++)
		{
			if(offsetx != m_ptary[i].X - 18.0f || 
				offsety != m_ptary[i].Y - 18.0f)
			{
				mat.Reset();
				pRgn = m_Region.Clone();
				offsetx = m_ptary[i].X - 18.0f;
				offsety = m_ptary[i].Y - 18.0f;
				mat.Translate(offsetx,offsety,MatrixOrderAppend);
				pRgn->Transform(&mat);
				region.Union(pRgn);
			}
		}
		m_Region.MakeEmpty();
		m_Region.Union(&region);
	}
	else
	{
		Pen	penDraw(Color::Blue,(float)m_nWidth);
		penDraw.SetStartCap(LineCapRound);
		penDraw.SetEndCap(LineCapRound);
		penDraw.SetLineJoin(LineJoinRound);

		if(AP_3COLORS == m_nAPstyle)
		{
			SolidBrush bblue(Color::Blue);
			penDraw.SetBrush(&bblue);
		}

		GraphicsPath path;
		path.AddCurve(pPt,m_nPtCount);
		path.Widen(&penDraw);
		path.Outline();

		m_Region.MakeEmpty();
		m_Region.Union(&path);
	}
}

void CArtistPen::DrawImagelines(Graphics& graph,Image*	pImg)
{
	ASSERT(m_pImg);
	PointF pt;
	for(int i = 0;i < m_nPtCount;i++)
	{
		pt.X = m_ptary[i].X - 18;
		pt.Y = m_ptary[i].Y - 18;
		graph.DrawImage(pImg,pt);
	}
}

void CArtistPen::FormDarwImageData(PointF pt,float fspace)
{
	if(m_nPtCount == 0)
	{
		m_ptary.Add(pt);
		m_nPtCount++;
	}
	else
	{
		int nbound = (int)m_ptary.GetUpperBound();
		float offset = CalGapOfpoints(m_ptary[nbound],pt);
		if(offset == fspace)
		{
			m_ptary.Add(pt);
			m_nPtCount++;
		}
		else if(offset > fspace)
		{
			float nCount = offset / fspace;
			float offx = pt.X - m_ptary[nbound].X;
			float offy = pt.Y - m_ptary[nbound].Y;
			offx /= nCount;
			offy /= nCount;

			for(int i = 0;i < nCount; i++)
			{
				pt.X = m_ptary[nbound].X + offx;
				pt.Y = m_ptary[nbound].Y + offy;

				m_ptary.Add(pt);
				m_nPtCount++;
			}
		}
	}
}

void CArtistPen::SetpngRegion()
{
	Status status = m_pImg->GetLastStatus();
	if(Ok == status)
	{
		CDC *pDC = CDC::FromHandle(::GetDC(NULL));
		int nWidth = m_pImg->GetWidth();
		int nHeight = m_pImg->GetHeight();

		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(pDC,nWidth, nHeight);
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CBitmap* pbmpOld = dc.SelectObject(&bitmap);
		dc.FillSolidRect(0,0,nWidth,nHeight,RGB(255,255,255));

		Graphics graphics(dc.m_hDC);
		status = graphics.GetLastStatus();
		if(Ok == status)
		{
			graphics.DrawImage(m_pImg, 0, 0);
		}

		dc.SelectObject(pbmpOld);
		dc.DeleteDC();

		HRGN hrgn = BitmapToRegion((HBITMAP)bitmap, RGB(255,255,255));
		bitmap.DeleteObject();

		Region region(hrgn);
		m_Region.MakeEmpty();
		m_Region.Union(&region);
	}
}

void CArtistPen::LoadImage(UINT nID, CImage* pImage)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource(hInst,MAKEINTRESOURCE(nID),_T("PNG"));
	if (hRsrc == NULL)
		return ;

	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (lpRsrc == NULL)
		return ;

	// Allocate global memory on which to create stream
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);

	// load from stream
	pImage->Load(pstm);

	// free/release stuff
	GlobalUnlock(m_hMem);
	GlobalFree(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);
}


void CArtistPen::InputGraphData(CPoint* pPt,int nCount)
{
	CGraph::InputGraphData(pPt,nCount);

	if(AP_5STARS == m_nAPstyle || AP_SMILE == m_nAPstyle)
	{
		AdjustTheData();
		InitHotData(TRUE);
	}
}

void CArtistPen::AdjustTheData()
{
	CArray<PointF,PointF&> 	ptary;	
	ptary.SetSize(0,10);
	ptary.Add(m_ptary[0]);
	float offset = 0.0f;
	PointF pt1;

	for(int i = 1;i < m_nPtCount;i++)
	{
		int nbound = (int)ptary.GetUpperBound();
		offset = CalGapOfpoints(m_ptary[i],ptary[nbound]);
		if(offset == 36.0f)
		{
			ptary.Add(m_ptary[i]);
		}
		else if(offset > 36.0f)
		{
			float nCount = offset / 36.0f;
			float offx = m_ptary[i].X - ptary[nbound].X;
			float offy = m_ptary[i].Y - ptary[nbound].Y;
			offx /= nCount;
			offy /= nCount;

			for(int i = 0;i < nCount; i++)
			{
				pt1.X = ptary[nbound].X + offx;
				pt1.Y = ptary[nbound].Y + offy;
				ptary.Add(pt1);
			}
		}
	}

	ptary.FreeExtra();
	m_ptary.Copy(ptary);
	m_ptary.FreeExtra();
}

void CArtistPen::SetImage(Image* pImage)
{
	ASSERT(pImage);

	if(m_pImg)
	{
		delete m_pImg;
		m_pImg = NULL;
	}

	m_pImg = pImage;
}