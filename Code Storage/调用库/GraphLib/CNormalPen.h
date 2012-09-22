#include "CGraph.h"
class CNormalPen : public CGraph
{
public:
	CNormalPen(LINETYPE LineType);
	CNormalPen();
	CNormalPen(const CNormalPen& gph);
	~CNormalPen();
	
	virtual void Draw(CDC* pDC);
	virtual	void AddPoint(UINT nMouseState,CPoint point);
	virtual void FinishGraph();
	virtual void SetRegion();
	
public:
	DashStyle 	m_nDashStyle;
	LineCap 	m_nStartCap;
	LineCap 	m_nEndCap;

	int			m_noffst;
	
public:
	void SetDashStyle(DashStyle nDashStyle);
	void SetStartCap(LineCap nStartCap);
	void SetEndCap(LineCap nEndCap);	
	void SetLineType(LINETYPE LineType);
	
	void FormGraphData(UINT nMouseState,CPoint point);
};
	
