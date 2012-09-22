#pragma once

class GRAPHLIBAPI CToolRegularRuler : public CGraph
{
public:
	CToolRegularRuler();
	~CToolRegularRuler(void);
	CToolRegularRuler(const CToolRegularRuler& gph);
	CToolRegularRuler & operator = (const CToolRegularRuler& gph);
	
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void InitHotData();
	virtual void SetEraseRegion(Region* pRgn);
	virtual void DrawHotShape(Graphics& graph);
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	virtual ModifyType AroundHotPts(CPoint point);
	virtual void DecideModiFyType(UINT nMouseState,CPoint point);
	virtual void AddPoint(UINT nMouseState,CPoint point, CRect& rcUpdate);
	virtual void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);

	virtual void MoveGraph(CPoint point);
	virtual void StretchGraph(CPoint point);
	virtual int MoveGraph(float offsetx,float offsety);

	void InitDataMember();
	void InitDefaultData();
	void DrawRulerMarks(Graphics& graph);

protected:
	CRect	m_rcGrip;
};
