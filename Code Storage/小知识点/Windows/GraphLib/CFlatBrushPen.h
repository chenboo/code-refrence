#pragma once
class GRAPHLIBAPI CFlatBrushPen : public CGraph
{
public:
	CFlatBrushPen();
	CFlatBrushPen(const CFlatBrushPen& gph);
	CFlatBrushPen& operator = (const CFlatBrushPen& gph);
	CFlatBrushPen(Color cr, int nWidth, float fTransAngle = 45.0f);
	~CFlatBrushPen(void);

	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void InitHotData();
	void SetTransAngle(float fTransAngle);
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);
	
public:
	int			m_OrgTop;
	int			m_OrgLeft;
	int			m_nOrgWidth;
	int			m_nOrgHeight;
	float		m_ftransAngle;
};