class GRAPHLIBAPI CBalloon : public CGraph
{
public:
	CBalloon();
	~CBalloon(void);
	CBalloon(const CBalloon& gph);
	CBalloon(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CBalloon & operator = (const CBalloon& gph);
	
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