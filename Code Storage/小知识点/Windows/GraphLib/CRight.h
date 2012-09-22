class GRAPHLIBAPI CRight : public CGraph
{
public:
	CRight();
	~CRight(void);
	CRight(const CRight& gph);
	CRight(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CRight& operator = (const CRight& gph);

protected:
	CArray<PointF,PointF&> 		m_ptarytemp;
	
public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);

	void InitGraphData();
	void NormalizeTempData();
	void NormalizeGraphData();
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);
};