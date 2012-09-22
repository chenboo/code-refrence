class GRAPHLIBAPI C5Stars : public CGraph
{
public:
	C5Stars();
	~C5Stars(void);
	C5Stars(const C5Stars& gph);
	C5Stars(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	C5Stars & operator = (const C5Stars& gph);

protected:
	CArray<PointF,PointF&> 		m_ptarytemp;
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC, CRect& rcInvalid);
	
	void InitGraphData();
	void NormalizeTempData();
	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);
};