#pragma once
class GRAPHLIBAPI CLengthLabel : public CGraph
{
public:
	CLengthLabel();
	CLengthLabel(int nWidth, Color cr, DashStyle dashstyle = DashStyleSolid);
	CLengthLabel(const CLengthLabel& gph);
	CLengthLabel & operator = (const CLengthLabel& gph);
	~CLengthLabel(void);
	
	virtual void SetRegion();
	virtual void InitHotData();
	virtual void FinishGraph();
	virtual void DrawHotShape(Graphics& graph);
	virtual BOOL IsPtInModifyZone(CPoint point);
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	virtual ModifyType AroundHotPts(CPoint point);
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);

	virtual void StretchGraph(CPoint point);
	virtual void DecideModiFyType(UINT nMouseState,CPoint point);
	virtual void ModifyGraph(UINT nMouseState,CPoint point);

private:
	CString			m_strLength;
};