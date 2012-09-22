#include "CGraph.h"

class CRectangle : public CGraph
{
public:
	CRectangle(int nRStyle,int nWidth,Color crColor);
	CRectangle();
	CRectangle(const CRectangle& gph);
	~CRectangle(void);
	
	virtual void Draw(CDC* pDC);
	virtual	void AddPoint(UINT nMouseState,CPoint point);
	virtual void FinishGraph();
	virtual void SetRegion();
	
private:
	int			m_nRStyle;		//Sytle of Rectangle
	
public:
	void SetRCStyle(int nRStyle); 
	void FormGraphData(UINT nMouseState,CPoint point);
	void NormalizeData();

	void DrawRoundRect(Graphics& graph,CRect rc);
	void DrawSquare(Graphics& graph,CRect rc);
	void DrawRectangle(Graphics& graph,CRect rc);
};
