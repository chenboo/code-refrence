
class GRAPHLIBAPI CITriangle : public CGraph
{
public:
	CITriangle();
	CITriangle(const CITriangle& gph);
	CITriangle(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CITriangle & operator = (const CITriangle& gph);
	~CITriangle(void);
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	void DrawPreView(Graphics& graph, Pen& penDraw);//draw the prev triangle that do not change the data

	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);
};
