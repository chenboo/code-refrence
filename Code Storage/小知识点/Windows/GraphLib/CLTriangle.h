class GRAPHLIBAPI CLTriangle : public CGraph
{
public:
	CLTriangle();
	CLTriangle(const CLTriangle& gph);
	CLTriangle(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CLTriangle & operator = (const CLTriangle& gph);
	~CLTriangle(void);
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);

	void DrawPreView(Graphics& graph, Pen& penDraw);

	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);
};