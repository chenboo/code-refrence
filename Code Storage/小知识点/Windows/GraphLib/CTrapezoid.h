
class GRAPHLIBAPI CTrapezoid : public CGraph
{
public:
	CTrapezoid(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CTrapezoid();
	CTrapezoid(const CTrapezoid& gph);
	CTrapezoid & operator = (const CTrapezoid& gph);
	~CTrapezoid(void);
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);

	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);
};
