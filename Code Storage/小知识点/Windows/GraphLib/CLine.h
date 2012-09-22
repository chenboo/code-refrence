
class GRAPHLIBAPI CLine : public CGraph
{
public:
	CLine();
	CLine(const CLine& gph);
	CLine(LINETYPE LineType);
	CLine & operator = (const CLine& gph);
	~CLine(void);
	
	virtual void FinishGraph();
	virtual void DrawHotShape(Graphics& graph);
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);

	virtual void StretchGraph(CPoint point);
	virtual void ModifyGraph(UINT nMouseState,CPoint point);
	virtual void DecideModiFyType(UINT nMouseState,CPoint point);

	virtual BOOL IsPtInModifyZone(CPoint point);
	virtual ModifyType AroundHotPts(CPoint point);

	virtual void SetRegion();
	void SetEndCap(LineCap nEndCap);
	void SetStartCap(LineCap nStartCap);
	void SetLineType(LINETYPE LineType);
	void SetDashStyle(DashStyle nDashStyle);
	
public:
	LineCap 	m_nEndCap;
	LineCap 	m_nStartCap;
	DashStyle 	m_nDashStyle;
};
