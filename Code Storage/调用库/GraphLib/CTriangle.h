#include "CGraph.h"

class CTriangle : public CGraph
{
public:
	CTriangle(int nWidth,Color crColor);
	CTriangle();
	CTriangle(const CTriangle& gph);
	~CTriangle(void);
	
public:	
	virtual void Draw(CDC* pDC);
	virtual	void AddPoint(UINT nMouseState,CPoint point);
	virtual void FinishGraph();
	virtual void SetRegion();

public:
	void FormGraphData(UINT nMouseState,CPoint point);
	void CalGraphData();
};
