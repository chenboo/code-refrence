#pragma once
#include <afxtempl.h>
#include <math.h>

class GRAPHLIBAPI CGraph
{
public:
	CGraph();
	virtual ~CGraph();
	CGraph(const CGraph& gph);
	CGraph & operator = (const CGraph& gph);
	void CopyTheBaseDataMember(const CGraph* pgph);//used for copy construction

	virtual void ResetSubGraph();
	void FormGraphDirect(POINTARRAY& ptarray);
	
public:
	virtual void InitHotData();
	virtual void SetRegion() = 0;
	virtual void FinishGraph() = 0;
	virtual void SetEraseRegion(Region* pRgn);
	virtual void Draw(CDC* pDC, CRect& rcInvalid) = 0;
	virtual void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate) = 0;

	//set the graph's properties
	void SetHotPoints();
	virtual void SetWidth(int nWidth);
	void SetSelectState(BOOL bSelect);
	virtual void SetColor(Color crColor);
	virtual void SetEditSate(BOOL bEdit);
	void SetLineDashStyle(DashStyle dashStyle);
	virtual void PrePareBufferDC(CDC* pDCMem, CDC* pDCOrg, CRect& rect);
	virtual void AddPoint(UINT nMouseState, CPoint point, CRect& rcUpdate);

	//Modify Graphics
	virtual void MoveGraph(CPoint point);
	virtual void StretchGraph(CPoint point);
	virtual void ModifyGraph(UINT nMouseState, CPoint point);
	virtual void DecideModiFyType(UINT nMouseState, CPoint point);
	virtual float CalcRotateAngle(CPoint point, PointF ptRotateCenter);

	//decide whether the mouse is in hot region
	virtual BOOL IsPtInHotRegion(CPoint point);
	virtual BOOL IsPtInModifyZone(CPoint point);
	virtual ModifyType AroundHotPts(CPoint point);

	virtual float GetRotateAngle();
	virtual CRect FormUpdateRect();
	virtual void DrawHotShape(Graphics& graph);
	virtual int MoveGraph(float offsetx, float offsety);

	//drawing hot state 
	void DrawEightHotPoints(Graphics& graph);
	
	void TransformHotDataToWnd(int nleft, int ntop);
	void TransformHotDataToScreen(int nleft, int ntop);

	void ResetEraseState();
	void SaveTheRotateAngle();
	void RecoverTheRotateAngle();
	
	//Assist functions
	void AdjustGraph(float fAngle);//to avoid the change of the center point
	float CalcGapOfpoints(PointF ptStart, PointF ptEnd);
	float CalcGapOfpoints(CPoint ptStart, CPoint ptEnd);
	
	//interface functions draw from direct input
	int RotateGraph(float fAngle);
	int StretchGraph(float xStret, float yStret);
	int InputGraphDataIndirect(CPoint* pPt, int nCount);
	
	BOOL GetEraseState();
	BOOL GetSelectState();
	
	Region* GetGraphRegion();
	PointF GetRotateCenterPoint();

	inline RectF CRect2RectF(CRect& rect)
	{
		RectF rcf((float)rect.left,
				  (float)rect.top,
				  (float)rect.Width(),
				  (float)rect.Height());

		return rcf;
	}

	inline CRect Rect2CRect(Rect& rc)
	{
		CRect rect(rc.X,
				   rc.Y,
				   rc.X + rc.Width,
				   rc.Y + rc.Height
				   );

		return rect;
	}

	inline PointF CPoint2PointF(CPoint& point)
	{
		PointF ptf((float)point.x, (float)point.y);

		return ptf;
	}

public:
	int 		m_nWidth;
	Color		m_crColor;
	int			m_nPtCount;
	DashStyle 	m_nDashStyle;
	
	CRect		m_rcHot;

	BOOL		m_bEdit;		//if the graph is being edit used in screen board and CText Graph
	BOOL		m_bFinish;
	BOOL		m_bDataInvalid;	//if the input is valid

	POINTARRAY	m_ptary;

	DrawType	m_nSubType;
	CGraph*		m_pSubGraph;

protected:
	HOTPOINTS	m_HotPts;		//hot points
	ModifyType	m_ModifyType;	//modify type
	int			m_nMinCount;	//the least count of points needed to draw
	
	float		m_fAngle;		//rotate angle
	float		m_fOldAngle;	//old angle used to save
	PointF		m_ptCenter;		//center point for rotating
	PointF		m_ptModifyOrg;	//org point for moving

	float		m_fScale;		//use to scale the graph
	int			m_nOrgWidth;
	int			m_nOrgHeight;

	Region		m_Region;		//hot rgn
	Region		m_RgnErase;		//Erase rgn

	BOOL		m_bErased;		//if the graph has been erased by the last option
	BOOL		m_bSelected;
};
	