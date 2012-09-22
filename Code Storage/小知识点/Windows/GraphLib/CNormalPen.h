#pragma once

class GRAPHLIBAPI CNormalPen : public CGraph
{
public:
	CNormalPen();
	~CNormalPen();
	CNormalPen(LINETYPE LineType);
	CNormalPen(const CNormalPen& gph);
	CNormalPen & operator = (const CNormalPen& gph);

	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);

	virtual void SetRegion();
	void SetEndCap(LineCap nEndCap);
	DashStyle GetNormalPenDashStyle();
	void SetLineType(LINETYPE LineType);
	void SetStartCap(LineCap nStartCap);
	void SetDashStyle(DashStyle nDashStyle);

public:
	int			m_noffst;
	LineCap 	m_nEndCap;
	LineCap 	m_nStartCap;
};
	
