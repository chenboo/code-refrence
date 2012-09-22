
class GRAPHLIBAPI CCircle : public CGraph
{
public:
	CCircle();
	CCircle(const CCircle& gph);
	CCircle & operator = (const CCircle& gph);
	CCircle(int nCStyle, int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	~CCircle();
	
	void NormalizeData();
	virtual void FinishGraph();
	void SetCircleStyle(int nCStyle);
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	
	virtual void SetRegion();
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);

	int GetCircleStyle()
	{
		return m_nCStyle;
	}
	
private:
	float			m_fRadius;		//the circle's radius
	CircleStyle 	m_nCStyle;		//Style of the circle
};
