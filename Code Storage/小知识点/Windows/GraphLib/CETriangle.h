
class GRAPHLIBAPI CETriangle : public CGraph
{
public:
	CETriangle();
	CETriangle(const CETriangle& gph);
	CETriangle(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CETriangle & operator = (const CETriangle& gph);
	~CETriangle(void);
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	void DrawPreView(Graphics& graph, Pen& penDraw);
	
	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);
};
