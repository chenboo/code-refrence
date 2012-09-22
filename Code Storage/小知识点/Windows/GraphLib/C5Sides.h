class GRAPHLIBAPI C5Sides : public CGraph
{
public:
	C5Sides();
	~C5Sides(void);
	C5Sides(const C5Sides& gph);
	C5Sides(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	C5Sides & operator = (const C5Sides& gph);

protected:
	CArray<PointF,PointF&> 		m_ptarytemp;
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	
	void InitGraphData();
	void NormalizeTempData();
	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);
};