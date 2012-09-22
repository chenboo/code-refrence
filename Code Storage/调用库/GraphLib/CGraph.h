#pragma once

#include "DrawObject.h"
#include <afxtempl.h>
#include <math.h>

class CGraph
{
public:
	CGraph();
	CGraph(const CGraph& gph);
	virtual ~CGraph();
	
public:
	virtual void Draw(CDC* pDC) = 0;
	virtual	void AddPoint(UINT nMouseState,CPoint point) = 0;
	virtual void FinishGraph() = 0;
	virtual void SetRegion() = 0;
	virtual void InitHotData(BOOL bExtend = FALSE);

	//Functions
public:
	void CopyBase(const CGraph* pgph);//used for copy construction

	//set graph properties
	void SetWidth(int nWidth);
	void SetColor(Color crColor);
	void SetHotPoints();

	//Assist functions
	float CalGapOfpoints(PointF ptStart,PointF ptEnd);
	float CalGapOfpoints(CPoint ptStart,CPoint ptEnd);
	void AdjustGraph(float fAngle);//to avoid the change of the center point

	BOOL bPtInHotRegion(CPoint point);
	BOOL bPtInModifyZone(CPoint point);
	ModifyType AroundHotPts(CPoint point);

	//drawing hot state 
	void DrawHotShape(Graphics& graph);
	void DrawEightpoints(Graphics& graph);

	//Modify Graphics
	void DeceideModiFyType(UINT nMouseState,CPoint point);
	void ModifyGraph(UINT nMouseState,CPoint point);

	//draw from mouse input
	void MoveGraph(CPoint point);
	void StretchGraph(CPoint point);
	void RotateGraph(CPoint point);

	//interface functions draw from direct input  
	void InputGraphData(CPoint* pPt,int nCount);
	void MoveGraph(float offsetx,float offsety);
	void RotateGraph(float fAngle);
	void StretchGraph(float xStret, float yStret);

	static HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance = RGB(0,0,0));

public://Data's property
	int 		m_nWidth;
	int			m_nPtCount;		//the count of points
	int			m_nMinCount;	//the least count of points needed to draw
	Color		m_crColor;
	ModifyType	m_ModifyType;	//modify type

	//hot property
	HOTPOINTS	m_HotPts;		//hot points
	Region		m_Region;		//hot rgn
	CRect		m_rcHot;		//hot rect
		
	//drawing state
	BOOL		m_bFinish;		//if the graph has finished
	BOOL		m_bSelected;	//if the graph is selected

	//modify parameters
	PointF		m_ptCenter;		//center point for rotating
	PointF		m_ptModifyOrg;	//org point for moving
	float		m_fAngle;		//rotate angle

	//data
	CArray<PointF,PointF&> 		m_ptary;	//points for drawing

};
	