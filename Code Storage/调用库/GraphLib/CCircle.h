#include "CGraph.h"

class CCircle : public CGraph
{
public:
	CCircle(int nCStyle,int nWidth,Color crColor);
	CCircle();
	CCircle(const CCircle& gph);
	~CCircle();
	
	virtual void Draw(CDC* pDC);
	virtual	void AddPoint(UINT nMouseState,CPoint point);
	virtual void FinishGraph();
	virtual void SetRegion();
	
private:
	int 	m_nCStyle;		//Style of the circle
	float	m_fRadius;		//the circle's radius
	
public:
	void SetCircleStyle(int nCStyle);
	
	void FormGraphData(UINT nMouseState,CPoint point);
	void NormalizeData();
};
