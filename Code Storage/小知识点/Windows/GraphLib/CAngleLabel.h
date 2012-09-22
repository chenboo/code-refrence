class GRAPHLIBAPI CAngleLabel : public CGraph
{
public:
	CAngleLabel(int nWidth, Color crColor, DashStyle dashstyle = DashStyleSolid);
	CAngleLabel();
	CAngleLabel(const CAngleLabel& gph);
	CAngleLabel& operator = (const CAngleLabel& gph);
	~CAngleLabel(void);

public:
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC, CRect& rcInvalid);
	
	virtual void InitHotData();
	virtual void StretchGraph(CPoint point);
	virtual void DrawHotShape(Graphics& graph);
	virtual BOOL IsPtInModifyZone(CPoint point);
	virtual ModifyType AroundHotPts(CPoint point);
	virtual void ModifyGraph(UINT nMouseState, CPoint point);
	virtual void DecideModiFyType(UINT nMouseState, CPoint point);

	int CalLinesAngle();
	void DrawAngleLabelMark(Graphics& graph);
	void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);
	
private:
	CString		m_strAngle;
	BOOL		m_bhas3temppts;
};
