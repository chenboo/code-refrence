#pragma once

class GRAPHLIBAPI CToolProtractor : public CGraph
{
public:
	CToolProtractor();
	~CToolProtractor(void);
	CToolProtractor(const CToolProtractor& gph);
	CToolProtractor & operator = (const CToolProtractor& gph);
	
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void InitHotData();
	virtual void SetEraseRegion(Region* pRgn);
	virtual void DrawHotShape(Graphics& graph);
	virtual BOOL IsPtInModifyZone(CPoint point);
	virtual void Draw(CDC* pDC, CRect& rcInvalid);
	virtual ModifyType AroundHotPts(CPoint point);
	virtual void DecideModiFyType(UINT nMouseState, CPoint point);
	virtual void AddPoint(UINT nMouseState, CPoint point, CRect& rcUpdate);
	virtual void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);

	virtual void MoveGraph(CPoint point);
	virtual void StretchGraph(CPoint point);
	virtual int MoveGraph(float offsetx, float offsety);

	void InitDataMember();
	void InitDefaultData();
	void DrawProtractorMarks(Graphics& graph);

protected:
	float	m_fRadio[4];
	CRect	m_rcCircle[3];

};
