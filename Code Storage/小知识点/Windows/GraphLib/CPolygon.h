
class GRAPHLIBAPI CPolygon : public CGraph
{
public:
	CPolygon();
	~CPolygon(void);
	CPolygon(const CPolygon& gph);
	CPolygon & operator = (const CPolygon& gph);
	CPolygon(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);

	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC, CRect& rcInvalid);
	void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);

public:
	BOOL		m_bDbclk;
};
