class GRAPHLIBAPI CErro : public CGraph
{
public:
	CErro();
	~CErro(void);
	CErro(const CErro& gph);
	CErro(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CErro & operator = (const CErro& gph);

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