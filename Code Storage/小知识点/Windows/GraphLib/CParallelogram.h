
class GRAPHLIBAPI CParallelogram : public CGraph
{
public:
	CParallelogram();
	CParallelogram(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CParallelogram(const CParallelogram& gph);
	CParallelogram & operator = (const CParallelogram& gph);
	~CParallelogram(void);
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);

	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);
};
