#pragma once

class GRAPHLIBAPI CRectangle : public CGraph
{
public:
	CRectangle();
	~CRectangle(void);
	CRectangle(const CRectangle& gph);
	CRectangle & operator = (const CRectangle& gph);
	CRectangle(int nRStyle, int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	
	void NormalizeData();
	virtual void FinishGraph();

	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	void DrawSquare(Graphics& graph,CRect rc);
	void DrawRoundRect(Graphics& graph,CRect rc);
	void DrawRectangle(Graphics& graph,CRect rc);

	virtual void SetRegion();
	void SetRCStyle(int nRStyle);
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);

	int GetRCStyle()
	{
		return m_nRStyle;
	}
	
private:
	RectStyle			m_nRStyle;	//Sytle of Rectangle
};
