
class GRAPHLIBAPI CEraseRect : public CGraph
{
public:
	~CEraseRect(void);
	CEraseRect(const CEraseRect& gph);
	CEraseRect & operator = (const CEraseRect& gph);
	CEraseRect(Color clr = Color::White, HBITMAP hbmp = NULL);
	
	void NormalizeData();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC, CRect& rcInvalid);
	virtual ModifyType AroundHotPts(CPoint point);

	virtual void SetRegion();
	void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);

protected:
	HBITMAP		m_hBmp;
};
