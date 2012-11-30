// ChildView.h : CChildView ��Ľӿ�
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
// ����
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

// ��д
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	public:
	CClientDC*		m_pDC;
	Rotation		m_Rotation;			//�ṹ�屣����ת��
	ClrRGB			m_clrBk;			//�ṹ�����ڱ��������ɫ
	ClrRGB			m_clrLight;			//�ṹ�����ڱ���Ƶ���ɫ
	Scale			m_Scale;			//�ṹ�����ڱ���������������
	CPoint			m_ptDelPos;			//������Ӧ����ƶ���תͼ��ĵ�
	UINT			m_nList;			//��ʾ�б�ı�ʶ��	
	CControlBoard	m_wndControl;		//������崰��

	BOOL			m_bHint;
	BOOL			m_bLight;
	BOOL			m_bRotation;
	BOOL			m_bScaleAll;

	char*	m_pDibBits;
	BITMAPFILEHEADER m_bmfileheader;
	BITMAPINFOHEADER m_bmheader;

public:
	void InitHGLRC();					//���ڳ�ʼ����Ⱦ������
	BOOL SetPixelFormatInfo();			//��������ģʽ
	void DoDraw();						//����
	void DrawTextTure();				//����ͼƬ����
	void SetTheBmpMap();				//��������ӳ�䷽ʽ
	void LoadBmp(CString strfilename);	//����λͼ
	void MakeModelList();				//����ʾ�б�ķ�ʽ��ά��ģ
};

