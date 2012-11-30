// ChildView.h : CChildView 类的接口
//
//-------------------------------------------
//author : pengsm
//scholl : njau
//email  : lilingshui@126.com
//-------------------------------------------

#pragma once

#include "gl/gl.h"
#include "gl/glu.h"

#include "ControlBoard.h"

typedef struct SRotation
{
	float xDelta;
	float yDelta;
}Rotation;

typedef struct SScale
{
	float xScale;
	float yScale;
}Scale;

typedef struct SClrRGB
{
	float clrR;
	float clrG;
	float clrB;
}ClrRGB;

class CChildView : public CWnd
{
// 构造
public:
	CChildView();
	virtual ~CChildView();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT DoControl(WPARAM , LPARAM);
	afx_msg LRESULT SetTheClr(WPARAM , LPARAM);

// 重写
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	public:
	CClientDC*		m_pDC;
	Rotation		m_Rotation;			//结构体保存旋转量
	ClrRGB			m_clrBk;			//结构体用于保存清除颜色
	ClrRGB			m_clrLight;			//结构体用于保存灯的颜色
	Scale			m_Scale;			//结构体用于保存各方向的拉伸量
	CPoint			m_ptDelPos;			//用于相应鼠标移动旋转图像的点
	UINT			m_nList;			//显示列表的标识号	
	CControlBoard	m_wndControl;		//控制面板窗口

	BOOL			m_bHint;
	BOOL			m_bLight;
	BOOL			m_bRotation;
	BOOL			m_bScaleAll;

	char*	m_pDibBits;
	BITMAPFILEHEADER m_bmfileheader;
	BITMAPINFOHEADER m_bmheader;

public:
	void InitHGLRC();					//用于初始化渲染描述表
	BOOL SetPixelFormatInfo();			//设置像素模式
	void DoDraw();						//绘制
	void DrawTextTure();				//绘制图片纹理
	void SetTheBmpMap();				//设置纹理映射方式
	void LoadBmp(CString strfilename);	//加载位图
	void MakeModelList();				//用显示列表的方式三维建模
};

