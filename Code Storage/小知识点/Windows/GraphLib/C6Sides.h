class GRAPHLIBAPI C6Sides : public CGraph
{
public:
	C6Sides();
	~C6Sides(void);
	C6Sides(const C6Sides& gph);
	C6Sides(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	C6Sides & operator = (const C6Sides& gph);

protected:
	CArray<PointF,PointF&> 		m_ptarytemp;
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	
	void InitGraphData();
	void NormalizeTempData();
	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState, CPoint point,CRect& rcUpdate);
};