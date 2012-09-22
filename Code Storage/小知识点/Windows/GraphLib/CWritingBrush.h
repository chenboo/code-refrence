#pragma once

class GRAPHLIBAPI CWritingBrush : public CGraph
{
public:
	CWritingBrush();
	~CWritingBrush(void);
	CWritingBrush(Color cr, int nWidth);
	CWritingBrush(const CWritingBrush& gph);
	CWritingBrush& operator = (const CWritingBrush& gph);

public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);

	void DrawWritingBrush(Graphics& graph);
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);

private:
	GraphicsPath	m_PathBrush;
};
