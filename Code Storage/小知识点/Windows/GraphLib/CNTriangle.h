
class GRAPHLIBAPI CNTriangle : public CGraph
{
public:
	CNTriangle();
	CNTriangle(const CNTriangle& gph);
	CNTriangle(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CNTriangle & operator = (const CNTriangle& gph);
	~CNTriangle(void);

public:
	BOOL	m_bhas3temppts;

public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);

	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);
};
