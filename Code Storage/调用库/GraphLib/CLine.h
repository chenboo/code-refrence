#include "CGraph.h"

class CLine : public CGraph
{
public:
	CLine(LINETYPE LineType);
	CLine(const CLine& gph);
	CLine();
	~CLine(void);
	
	virtual void Draw(CDC* pDC);
	virtual	void AddPoint(UINT nMouseState,CPoint point);
	virtual void FinishGraph();
	virtual void SetRegion();
	
public:	
	DashStyle 	m_nDashStyle;
	LineCap 	m_nStartCap;
	LineCap 	m_nEndCap;	
	
public:
	void SetDashStyle(DashStyle nDashStyle);
	void SetStartCap(LineCap nStartCap);
	void SetEndCap(LineCap nEndCap);
	void SetLineType(LINETYPE LineType);
	
	void FormGraphData(UINT nMouseState,CPoint point);
};
