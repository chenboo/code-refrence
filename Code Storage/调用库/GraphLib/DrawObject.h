#pragma once

using namespace Gdiplus;

#define	MININTERVAL	5

//ArtistPen Styles
#define AP_BRUSH			1
#define AP_3COLORS			2
#define AP_5STARS			3
#define AP_SMILE			4

//Circle Styles
#define CS_CIRCLE			1
#define CS_ELLIPSE			2

//Retangle Styles
#define RS_ROUND			1
#define RS_SQUARE			2
#define RS_RECT				3

//Mouse State
#define MS_NONE				0
#define MS_LBTNDOWN			1
#define MS_LBTNUP			2
#define MS_MOVE				3	
#define	MS_LDCLK			4
#define MS_LBTNMOUSE		5	//mousemove with lbutton down

//Line and NormalPen Pattern
typedef struct LinePattern
{
	Color		crColor;
	int 		nWidth;
	DashStyle	nDashStyle;
	LineCap		nStartCap;
	LineCap 	nEndCap;
}LINETYPE;

typedef struct Struct_HotsPot
{
	PointF ptTC;
	PointF ptBC;
	PointF ptLC;
	PointF ptRC;
	PointF ptLT;
	PointF ptLB;
	PointF ptRT;
	PointF ptRB;
	PointF ptRotate;
}HOTPOINTS;

#define HOTINTERVAL	10

enum ModifyType
{
	MT_NONE,		
	MT_MOVE,				
	MT_ROTATE,			
	MT_LT,				
	MT_TC,				
	MT_RT,				
	MT_LC,				
	MT_RC,				
	MT_LB,				
	MT_BC,				
	MT_RB				
};

enum DrawType
{
	DT_NONE = 1,
	DT_NORMALPEN,
	DT_ARTISTPEN,
	DT_LINE,
	DT_TRIANGLE,
	DT_CIRCLE,
	DT_POLYGON,
	DT_RECTANGLE,
	DT_EMF,
	DT_TEXT
};