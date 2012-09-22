#pragma once

class GRAPHLIBAPI CCameralRgn : public CGraph
{
public:
	CCameralRgn();
	~CCameralRgn(void);
	CCameralRgn(const CCameralRgn& gph);
	CCameralRgn & operator = (const CCameralRgn& gph);
	
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void InitHotData();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	virtual ModifyType AroundHotPts(CPoint point);
	virtual void AddPoint(UINT nMouseState, CPoint point, CRect& rcUpdate);

	void NormalizeData();
	void InitBitmap(CDC* pDC);
	HBITMAP CopyBitmap(HBITMAP hSourceHbitmap);
	void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);

public:
	CBitmap		m_bitmap;
	BOOL		m_bFormed;
};
