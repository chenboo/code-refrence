#pragma once

class GRAPHLIBAPI CToolComPasses : public CGraph
{
public:
	CToolComPasses();
	~CToolComPasses(void);
	CToolComPasses(const CToolComPasses& gph);
	CToolComPasses & operator = (const CToolComPasses& gph);

	void InitHeadPath();
	void InitLeftLegPath();
	void InitDefaultData();
	void InitRightLegPath();
	void InitDefaultHotData();

	void DrawHead(Graphics& graph);
	void DrawLeftLeg(Graphics& graph);
	void DrawRightLeg(Graphics& graph);

	void RotateRightLeg(float fAngle);
	void MoveAllPath(float offsetx, float offsety);

	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void InitHotData();
	virtual CRect FormUpdateRect();
	virtual float GetRotateAngle();
	virtual void SetEraseRegion(Region* pRgn);
	virtual void DrawHotShape(Graphics& graph);
	virtual BOOL IsPtInModifyZone(CPoint point);
	virtual void Draw(CDC* pDC, CRect& rcInvalid);
	virtual ModifyType AroundHotPts(CPoint point);
	virtual void DecideModiFyType(UINT nMouseState, CPoint point);
	virtual void AddPoint(UINT nMouseState, CPoint point, CRect& rcUpdate);
	virtual void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);

	virtual void MoveGraph(CPoint point);
	virtual void StretchGraph(CPoint point);
	virtual BOOL IsPtInHotRegion(CPoint point);
	virtual int MoveGraph(float offsetx, float offsety);
	virtual void ModifyGraph(UINT nMouseState, CPoint point);

	virtual void ResetSubGraph();

protected:
	PointF			m_ptStart;
	PointF			m_ptRORLeg;	//rotate point for the right leg
	PointF			m_ptCircleCenter;	//center point for the draw circle

	float			m_fRadius;
	float			m_fRLegOrg;
	float			m_fRDrawOrg;

	float			m_fRlegRted;//angle has rotated by right leg
	float			m_fRDrawted;//Angle has roteted by draw circle
	float			m_fStartAngle;
	
	//graph path for draw the compass
	GraphicsPath	m_pathHead;
	GraphicsPath	m_pathLeftLeg;
	GraphicsPath	m_pathRightLeg;
	GraphicsPath	m_pathPencilEnd;
	GraphicsPath	m_pathPencilBody;
	GraphicsPath	m_pathPencilSharp;
};