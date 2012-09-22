#include "CGraph.h"

class CPolygon : public CGraph
{
public:
	CPolygon(int nWidth,Color crColor);
	CPolygon();
	CPolygon(const CPolygon& gph);
	~CPolygon(void);

	virtual void Draw(CDC* pDC);
	virtual	void AddPoint(UINT nMouseState,CPoint point);
	virtual void FinishGraph();
	virtual void SetRegion();

public:
	BOOL	m_bLbtnAdd;		//whether have add the first point
	PointF	m_ptNxt;

public:
	void FormGraphData(UINT nMouseState,CPoint point);
};
