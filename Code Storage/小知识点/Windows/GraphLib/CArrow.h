class GRAPHLIBAPI CArrow : public CGraph
{
public:
	CArrow();
	~CArrow(void);
	CArrow(const CArrow& gph);
	CArrow(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CArrow & operator = (const CArrow& gph);
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC, CRect& rcInvalid);
	
	void InitGraphData();
	void NormalizeTempData();
	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);

protected:
	CArray<PointF,PointF&> 		m_ptarytemp;
};