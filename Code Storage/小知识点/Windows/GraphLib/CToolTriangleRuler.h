#pragma once

class GRAPHLIBAPI CToolTriangleRuler : public CGraph
{
public:
	CToolTriangleRuler();
	~CToolTriangleRuler(void);
	CToolTriangleRuler(const CToolTriangleRuler& gph);
	CToolTriangleRuler & operator = (const CToolTriangleRuler& gph);
	
	virtual void SetRegion();
	virtual void FinishGraph();
	
	virtual void SetEraseRegion(Region* pRgn);
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	virtual ModifyType AroundHotPts(CPoint point);
	virtual void DecideModiFyType(UINT nMouseState,CPoint point);
	virtual void AddPoint(UINT nMouseState,CPoint point, CRect& rcUpdate);
	virtual void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);

	void InitDataMember();
	void InitModifyZone();
	void InitDefaultData();
	virtual void InitHotData();

	void DrawTriangles(Graphics& graph);
	virtual void DrawHotShape(Graphics& graph);
	void DrawTriangleRulerMarks(Graphics& graph);

	virtual void MoveGraph(CPoint point);
	virtual void StretchGraph(CPoint point);
	virtual int MoveGraph(float offsetx,float offsety);

	virtual BOOL IsPtInModifyZone(CPoint point);

protected:
	GraphicsPath	m_pathLeft;
	GraphicsPath	m_pathRight;
	GraphicsPath	m_pathMove;
	
	CRect			m_rcInterTriangle;
};
