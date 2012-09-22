
class GRAPHLIBAPI CDiamond : public CGraph
{
public:
	CDiamond();
	CDiamond(const CDiamond& gph);
	CDiamond(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CDiamond & operator = (const CDiamond& gph);
	~CDiamond(void);
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);

	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);
};
