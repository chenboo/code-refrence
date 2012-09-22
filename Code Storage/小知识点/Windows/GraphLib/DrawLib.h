#pragma once

#ifdef GRAPHLIB
#define GRAPHLIBAPI __declspec(dllexport)
#else
#define GRAPHLIBAPI __declspec(dllimport)
#endif

//ERRO NO.
#define ER_NO_ERRO					100
#define ER_DATA_LESS				101
#define ER_NOT_DRAW_FINISHED		102
#define ER_NEGATIVE_ERRO			103

#define	MININTERVAL					5
#define HOTINTERVAL					10

//Mouse State
#define MS_NONE						0
#define MS_LBTNDOWN					1
#define MS_LBTNUP					2
#define MS_MOVE						3
#define	MS_LDCLK					4
#define MS_LBTNMOUSE				5	//mousemove with lbutton down

enum APenStyle
{
	AP_NONE = 1,
	AP_3COLORS,
	AP_5STARS,
	AP_SMILE
};

enum CircleStyle
{
	CS_NONE = 1,
	CS_CIRCLE,
	CS_ELLIPSE
};

enum RectStyle
{
	RS_NONE = 1,
	RS_ROUND,
	RS_SQUARE,
	RS_RECT
};

//Line and NormalPen Pattern
typedef struct LinePattern
{
	Color		crColor;
	int 		nWidth;
	DashStyle	nDashStyle;
	LineCap		nStartCap;
	LineCap 	nEndCap;
}LINETYPE;

typedef struct APenPattern
{
	int			nWidth;
	APenStyle	nStyle;
}APENTYPE;

typedef struct FlatBrushPen
{
	int			nWidth;
	float		fTranAngle;
}FLATBRUSHTYPE;

typedef struct CirclePattern
{
	int			nWidth;
	DashStyle	nDashStyle;
	CircleStyle nCSStyle;
}CIRCLETYPE;

typedef struct QuadrilateralPattern
{
	int			nWidth;
	DashStyle	nDashStyle;
	RectStyle	nRcStyle;
}QUADTYPE;

typedef struct PolygonPattern
{
	int			nWidth;
	DashStyle	nDashStyle;
}POLYGONTYPE;

typedef struct TrianglePattern
{
	int			nWidth;
	DashStyle	nDashStyle;
}TRIANGLETYPE;

typedef struct LabelPattern
{
	int			nWidth;
	DashStyle	nDashStyle;
}LABELTYPE;

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

enum ModifyType
{
	MT_LT,
	MT_TC,
	MT_RT,
	MT_LC,
	MT_RC,
	MT_LB,
	MT_BC,
	MT_RB,
	MT_NONE,
	MT_MOVE,
	MT_ROTATE
};

enum DrawType
{
	DT_NONE = 1,
	DT_LINE,
	DT_TEXT,
	DT_ARROW,
	DT_BALLON,
	DT_6SIDES,
	DT_5SIDES,
	DT_CIRCLE,
	DT_5STARS,
	DT_PICTURE,
	DT_DIAMOND,
	DT_POLYGON,
	DT_ERASEANY,
	DT_ERASERGN,
	DT_COMPASSES,
	DT_RECTANGLE,
	DT_ARTISTPEN,
	DT_NORMALPEN,
	DT_ITRIANGLE,
	DT_LTRIANGLE,
	DT_ETRIANGLE,
	DT_NTRIANGLE,
	DT_TRAPEZOID,
	DT_FLOODFILL,
	DT_CAMERALALL,
	DT_CAMERALRGN,
	DT_ANGLELABEL,
	DT_ERROSYMBOL,
	DT_PROTRACTOR,
	DT_RIGHTSYMBOL,
	DT_LENGTHLABEL,
	DT_FLATBRUSHPEN,
	DT_REGULARRULER,
	DT_PARALLELOGRAM,
	DT_TRIANGLERULER,
	DT_TRANSPARENTPEN,
	DT_WRITINGBRUSHPEN
};

typedef CArray<PointF, PointF&>	POINTARRAY;

#include "CGraph.h"
#include "CLine.h"
#include "CErro.h"
#include "CText.h"
#include "CArrow.h"
#include "CRight.h"
#include "CBallon.h"
#include "C5Sides.h"
#include "C6Sides.h"
#include "C5Stars.h"
#include "CCircle.h"
#include "CPolygon.h"
#include "CDiamond.h"
#include "CPicture.h"
#include "CErasePen.h"
#include "CEraseRect.h"
#include "CNormalPen.h"
#include "CArtistPen.h"
#include "CRectangle.h"
#include "CITriangle.h"
#include "CLTriangle.h"
#include "CNTriangle.h"
#include "CETriangle.h"
#include "CTrapezoid.h"
#include "CCameralAll.h"
#include "CCameralRgn.h"
#include "CAngleLabel.h"
#include "CLengthLabel.h"
#include "CFlatBrushPen.h"
#include "CWritingBrush.h"
#include "CParallelogram.h"
#include "CTransparentPen.h"
#include "CToolFillFlood.h"
#include "CToolComPasses.h"
#include "CToolProtractor.h"
#include "CToolRegularRuler.h"
#include "CToolTriangleRuler.h"

BOOL GRAPHLIBAPI GraphlibPreTranslateMessage(MSG *pMsg);
