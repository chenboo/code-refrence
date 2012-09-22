#pragma once

class GRAPHLIBAPI CCameralAll : public CGraph
{
public:
	CCameralAll();
	~CCameralAll(void);
	CCameralAll(const CCameralAll& gph);
	CCameralAll & operator = (const CCameralAll& gph);

	void InitPhotoData();
	void InitBitmap(CDC* pDC);
	HBITMAP CopyBitmap(HBITMAP hSourceHbitmap);

	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void InitHotData();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	virtual ModifyType AroundHotPts(CPoint point);
	virtual void AddPoint(UINT nMouseState,CPoint point, CRect& rcUpdate);
	virtual void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate){};

public:
	CBitmap		m_bitmap;
};





