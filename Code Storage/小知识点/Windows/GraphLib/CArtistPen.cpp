#include "stdafx.h"
#include "GraphLib.h"

CArtistPen::CArtistPen(int nAPStyle, int nWidth)
{
	m_noffst = 0;
	m_nDCount = 0;
	m_pBitmap = NULL;
	m_nWidth = nWidth;
	m_nMinCount = 2;
	m_nAPstyle = (APenStyle)nAPStyle;
	m_ptary.SetSize(0,10);
	SetDrawingBmp();
}

CArtistPen::CArtistPen()
{
	m_noffst = 0;
	m_nDCount = 0;
	m_nWidth = 10;
	m_nMinCount = 2;
	m_pBitmap = NULL;
	m_ptary.SetSize(0,10);
	m_nAPstyle = AP_NONE;
}

CArtistPen::CArtistPen(const CArtistPen& gph)
{
	CopyTheBaseDataMember(&gph);

	m_noffst = gph.m_noffst;
	m_nAPstyle = gph.m_nAPstyle;

	m_nDCount = 0;
	m_nMinCount = 2;

	if(gph.m_pBitmap)
	{
		Unit nit;
		RectF rcf;
		gph.m_pBitmap->GetBounds(&rcf,&nit);
		m_pBitmap = gph.m_pBitmap->Clone(rcf,PixelFormatDontCare);
	}
}

CArtistPen& CArtistPen::operator = (const CArtistPen& gph)
{
	CopyTheBaseDataMember(&gph);

	m_noffst = gph.m_noffst;
	m_nAPstyle = gph.m_nAPstyle;

	m_nDCount = 0;
	m_nMinCount = 2;

	if(gph.m_pBitmap)
	{
		Unit nit;
		RectF rcf;
		gph.m_pBitmap->GetBounds(&rcf,&nit);
		m_pBitmap = gph.m_pBitmap->Clone(rcf,PixelFormatDontCare);
	}

	return *this;
}

CArtistPen::~CArtistPen()
{
	if(m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CArtistPen::Draw(CDC* pDC,CRect& rcInvalid)
{
	if(m_nPtCount == 0)
	{
		return;
	}

	Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(SmoothingModeHighQuality);

	Matrix mat;
	mat.RotateAt(m_fAngle,m_ptCenter);
	graph.SetTransform(&mat);

	if(m_bSelected || m_bEdit)
	{
		DrawHotShape(graph);
	}

	graph.ExcludeClip(&m_RgnErase);

	if(AP_3COLORS == m_nAPstyle)
	{
		if(!m_bFinish)
		{
			Rect rc(rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height());
			graph.SetClip(rc);
		}

		Draw3Clr(graph, &m_ptary);

	}
	else
	{
		if(!m_bFinish)
		{
			Rect rc(rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height());
			graph.SetClip(rc);
		}

		graph.SetPixelOffsetMode(PixelOffsetModeHalf);
		DrawImagelines(graph, &m_ptary);
	}
}

void CArtistPen::Draw3Clr(Graphics& graph, POINTARRAY* ptArray)
{
	float fWidth = (float)m_nWidth;

	if(m_bFinish)
	{
		fWidth *= m_fScale;
	}

	PointF* pPt = ptArray->GetData();

	Pen penDraw(Color::Blue, fWidth);
	penDraw.SetEndCap(LineCapRound);
	penDraw.SetStartCap(LineCapRound);
	penDraw.SetLineJoin(LineJoinRound);
	graph.DrawLines(&penDraw, pPt, m_nPtCount);
	
	penDraw.SetWidth(fWidth * 0.7f);
	penDraw.SetColor(Color::Yellow);
	graph.DrawLines(&penDraw, pPt, m_nPtCount);

	penDraw.SetWidth(fWidth * 0.4f);
	penDraw.SetColor(Color::Red);
	graph.DrawLines(&penDraw, pPt, m_nPtCount);
}

void CArtistPen::FinishGraph()
{
	m_bFinish = TRUE;
	InitHotData();
	return;
}

void CArtistPen::SetArtistPenStyle(int nAPstyle)
{
	m_nAPstyle = (APenStyle)nAPstyle;
	SetDrawingBmp();
}

void CArtistPen::FormGraphData(UINT nMouseState,CPoint point, CRect& rcUpdate)
{
	PointF pt((float)point.x,(float)point.y);

	if(AP_5STARS == m_nAPstyle || AP_SMILE == m_nAPstyle)
	{
		FormDarwImageData(pt, 2.0f * m_nWidth, rcUpdate);
	}
	else
	{
		if(m_nPtCount == 0)
		{
			m_ptary.Add(pt);
			m_nPtCount++;
		}
		else
		{
			PointF ptf = m_ptary[m_nPtCount - 1];
			if(CalcGapOfpoints(pt, ptf) >= 3)
			{
				m_ptary.Add(pt);
				m_nPtCount++;
			}
		}

		if(m_nPtCount >= 2 && !m_bFinish)
		{
			CPoint ptLT((int)m_ptary[m_nPtCount - 1].X, (int)m_ptary[m_nPtCount - 1].Y);
			CPoint ptRB((int)m_ptary[m_nPtCount - 2].X, (int)m_ptary[m_nPtCount - 2].Y);

			rcUpdate.SetRect(ptLT, ptRB);
			rcUpdate.NormalizeRect();
			rcUpdate.InflateRect(2 * m_nWidth, 2 * m_nWidth);
		}
	}
	if(MS_LBTNUP == nMouseState)
	{
		if(AP_5STARS == m_nAPstyle || AP_SMILE == m_nAPstyle)
		{
			m_bFinish = TRUE;
			InitHotData();
			rcUpdate = m_rcHot;
		}
		else
		{
			if((m_nPtCount <= 1) ||
				(m_nPtCount <= 2 && 
				CalcGapOfpoints(m_ptary[0],m_ptary[1]) <= 3 ))
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
}

void CArtistPen::SetRegion()
{
	PointF* pPt = m_ptary.GetData();

	if(AP_5STARS == m_nAPstyle)
	{
		Form5StarsRegion();
	}
	else if(AP_SMILE == m_nAPstyle)
	{
		FormSmileRegion();
	}
	else
	{
		Pen	penDraw(Color::Blue);
		penDraw.SetStartCap(LineCapRound);
		penDraw.SetEndCap(LineCapRound);
		penDraw.SetLineJoin(LineJoinRound);
		penDraw.SetWidth(m_nWidth * m_fScale);

		GraphicsPath path;
		path.AddCurve(pPt,m_nPtCount);
		path.Widen(&penDraw);
		path.Outline();

		m_Region.MakeEmpty();
		m_Region.Union(&path);
	}
}

void CArtistPen::FormSmileRegion()
{
	ASSERT(m_nPtCount >= 1);
	m_Region.MakeEmpty();
	Pen penWiden(Color::Blue, 1);
	//penWiden.SetAlignment(PenAlignmentInset);

	GraphicsPath path;
	float fWidth = m_nWidth * m_fScale;
	RectF rcf(0.0f, 0.0f, fWidth * 2, fWidth * 2);

	for(int i = 0;i < m_nPtCount;i++)
	{
		rcf.X = m_ptary[i].X - fWidth;
		rcf.Y = m_ptary[i].Y - fWidth;
		path.AddEllipse(rcf);
		
		m_Region.Union(&path);
		path.Widen(&penWiden);
		m_Region.Union(&path);
		path.Reset();
	}
}

void CArtistPen::Form5StarsRegion()
{
	ASSERT(m_nPtCount >= 1);
	PointF pt;
	m_Region.MakeEmpty();
	Pen penWiden(Color::Blue, 3);
	
	for(int i = 0;i < m_nPtCount;i++)
	{
		GraphicsPath path;

		float fWidth = m_nWidth * m_fScale;
		pt.X = m_ptary[i].X - fWidth;
		pt.Y = m_ptary[i].Y - fWidth;
		Add5Starpath(path,pt);

		m_Region.Union(&path);
		path.Widen(&penWiden);
		m_Region.Union(&path);
	}
}

void CArtistPen::Add5Starpath(GraphicsPath& path,PointF& pt)
{
	PointF ptData[10];
	ptData[0].X = 17.9f;
	ptData[0].Y = 0.1f;
	ptData[1].X = 13.5f;
	ptData[1].Y = 13.5f;
	ptData[2].X = 0.1f;
	ptData[2].Y = 13.7f;
	ptData[3].X = 10.8f;
	ptData[3].Y = 22.4f;
	ptData[4].X = 7.3f;
	ptData[4].Y = 35.7f;
	ptData[5].X = 17.9f;
	ptData[5].Y = 27.9f;
	ptData[6].X = 29.3f;
	ptData[6].Y = 35.7f;
	ptData[7].X = 25.4f;
	ptData[7].Y = 22.3f;
	ptData[8].X = 35.9f;
	ptData[8].Y = 13.7f;
	ptData[9].X = 22.6f;
	ptData[9].Y = 13.3f;

	float fWidth = m_nWidth * m_fScale;
	float fscale = fWidth / 18.0f;

	Matrix mat;
	mat.Scale(fscale, fscale, MatrixOrderAppend);
	mat.Translate(pt.X, pt.Y, MatrixOrderAppend);
	
	mat.TransformPoints(ptData, 10);

	path.AddPolygon(ptData,10);
}

void CArtistPen::DrawImagelines(Graphics& graph, POINTARRAY* ptArray)
{
	int nCount = 0;

	if(!m_bFinish)
	{
		nCount = m_nDCount;
	}

	for(int i = nCount; i < m_nPtCount; i++)
	{
		if(AP_SMILE == m_nAPstyle)
		{
			DrawSmileImage(graph, (*ptArray)[i]);
		}
		else if(AP_5STARS == m_nAPstyle)
		{
			Draw5StarsImage(graph, (*ptArray)[i]);
		}
	}

	if(!m_bFinish)
	{
		if(m_nPtCount == 1)
		{
			m_nDCount = m_nPtCount - 1;
		}
		else
		{
			m_nDCount = m_nPtCount;
		}
	}
}

void CArtistPen::DrawSmileImage(Graphics& graph, PointF pt)
{
	float fWidth = (float)m_nWidth;

	if(m_bFinish)
	{
		fWidth = m_nWidth * m_fScale;
	}

	pt.X -= fWidth;
	pt.Y -= fWidth;

	RectF rcf(pt.X, pt.Y, 2 * fWidth, 2 * fWidth);
	graph.DrawImage(m_pBitmap, rcf);
}

void CArtistPen::Draw5StarsImage(Graphics& graph,PointF pt)
{
	float fWidth = (float)m_nWidth;

	if(m_bFinish)
	{
		fWidth = m_nWidth * m_fScale;
	}

	pt.X -= fWidth;
	pt.Y -= fWidth;

	RectF rcf(pt.X, pt.Y, 2 * fWidth, 2 * fWidth);
	graph.DrawImage(m_pBitmap, rcf);
}

void CArtistPen::FormDarwImageData(PointF pt, float fspace, CRect& rcUpdate)
{
	if(m_nPtCount == 0)
	{
		m_ptary.Add(pt);
		m_nPtCount++;

		rcUpdate.SetRect(
			(int)pt.X - 2 * m_nWidth,
			(int)pt.Y - 2 * m_nWidth,
			(int)pt.X + 2 * m_nWidth,
			(int)pt.Y + 2 * m_nWidth);
	}
	else
	{
		int nbound = (int)m_ptary.GetUpperBound();
		float offset = CalcGapOfpoints(m_ptary[nbound],pt);
		if(offset == fspace)
		{
			m_ptary.Add(pt);
			m_nPtCount++;
			rcUpdate.SetRect(
				(int)pt.X - 2 * m_nWidth,
				(int)pt.Y - 2 * m_nWidth,
				(int)pt.X + 2 * m_nWidth,
				(int)pt.Y + 2 * m_nWidth);
		}
		else if(offset > fspace)
		{
			float fsum = offset / fspace;
			int nCount = (int)fsum;
			float offx = pt.X - m_ptary[nbound].X;
			float offy = pt.Y - m_ptary[nbound].Y;
			offx /= fsum;
			offy /= fsum;
			
			for(int i = 1; i <= nCount; i++)
			{
				pt.X = m_ptary[nbound].X + offx * i;
				pt.Y = m_ptary[nbound].Y + offy * i;

				m_ptary.Add(pt);
				m_nPtCount++;
			}
		}
	}
}

int CArtistPen::InputGraphDataIndirect(CPoint* pPt,int nCount)
{
	int nResult = CGraph::InputGraphDataIndirect(pPt,nCount);
	if(ER_NO_ERRO != nResult)
	{
		return nResult;
	}

	if(AP_5STARS == m_nAPstyle || AP_SMILE == m_nAPstyle)
	{
		AdjustTheData();
		InitHotData();
	}
	return ER_NO_ERRO;
}

void CArtistPen::AdjustTheData()
{
	CArray<PointF,PointF&>	ptary;
	ptary.SetSize(0,10);
	ptary.Add(m_ptary[0]);
	float offset = 0.0f;
	PointF pt1;

	for(int i = 1;i < m_nPtCount;i++)
	{
		int nbound = (int)ptary.GetUpperBound();
		offset = CalcGapOfpoints(m_ptary[i],ptary[nbound]);
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

void CArtistPen::SetDrawingBmp()
{
	if(m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}

	m_pBitmap = new Bitmap(36, 36);
	ASSERT(m_pBitmap);
	Graphics graph(m_pBitmap);
	graph.SetSmoothingMode(SmoothingModeHighQuality);

	if(AP_SMILE == m_nAPstyle)
	{
		Pen penOutLine(Color::Black, 1.5f);
		penOutLine.SetAlignment(PenAlignmentInset);
		SolidBrush brhead(Color::Yellow);
		SolidBrush breye(Color::Black);

		RectF rcf(1.0f, 1.0f, 33.0f, 33.0f);

		graph.FillEllipse(&brhead, rcf);
		graph.DrawEllipse(&penOutLine, rcf);

		rcf.X = 7.2f;
		rcf.Y = 7.2f;
		rcf.Width = 7.2f;
		rcf.Height = 14.4f;
		graph.FillEllipse(&breye, rcf);

		rcf.X = 21.6f;
		rcf.Y = 7.2f;
		rcf.Width = 7.2f;
		rcf.Height = 14.4f;
		graph.FillEllipse(&breye, rcf);

		rcf.X = 7.2f;
		rcf.Y = 9.6f;
		rcf.Width = 21.6f;
		rcf.Height = 21.6f;
		graph.DrawArc(&penOutLine, rcf, 20.0f, 140.0f);
	}
	else if(AP_5STARS == m_nAPstyle)
	{
		PointF ptData[10];
		ptData[0].X = 17.9f;
		ptData[0].Y = 0.1f;
		ptData[1].X = 13.5f;
		ptData[1].Y = 13.5f;
		ptData[2].X = 0.1f;
		ptData[2].Y = 13.7f;
		ptData[3].X = 10.8f;
		ptData[3].Y = 22.4f;
		ptData[4].X = 7.3f;
		ptData[4].Y = 35.7f;
		ptData[5].X = 17.9f;
		ptData[5].Y = 27.9f;
		ptData[6].X = 29.3f;
		ptData[6].Y = 35.7f;
		ptData[7].X = 25.4f;
		ptData[7].Y = 22.3f;
		ptData[8].X = 35.9f;
		ptData[8].Y = 13.7f;
		ptData[9].X = 22.6f;
		ptData[9].Y = 13.3f;

		SolidBrush fillbr(Color::Yellow);
		graph.FillPolygon(&fillbr, ptData, 10);

		Pen penOutLine(Color::Blue, 2.0f);
		penOutLine.SetAlignment(PenAlignmentInset);
		graph.DrawPolygon(&penOutLine, ptData, 10);
	}
}

void CArtistPen::PrePareBufferDC(CDC* pDCMem, CDC* pDCOrg, CRect& rect)
{
	if((AP_5STARS != m_nAPstyle
		&& AP_SMILE != m_nAPstyle)
		|| m_nPtCount <= 1
		|| m_bSelected
		|| m_bEdit)
	{
		pDCMem->BitBlt(0, 0, rect.Width(), rect.Height(), pDCOrg, 0, 0, SRCCOPY);
	}
}