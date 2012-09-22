#pragma once

class GRAPHLIBAPI CErasePen : public CGraph
{
public:
	CErasePen(const CErasePen& gph);
	CErasePen & operator = (const CErasePen& gph);
	CErasePen(Color clr = Color::White, HBITMAP hbmp = NULL);
	~CErasePen();

	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	virtual void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);

protected:
	HBITMAP		m_hBmp;
};
	
