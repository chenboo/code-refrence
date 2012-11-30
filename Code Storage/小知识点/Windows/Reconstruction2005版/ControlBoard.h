#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CControlBoard 对话框

class CControlBoard : public CDialog
{
	DECLARE_DYNAMIC(CControlBoard)

public:
	CControlBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CControlBoard();

// 对话框数据
	enum { IDD = IDD_DLG_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnControl(UINT nID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
public:
	int			m_nViewHeight;				//控制面板的高度
	int			m_nVPageSize;				//滑动条每页的高度
	int			m_nVScrollPos;				//当前滑动条的位置
	UINT		m_nMsg[2];					//向父窗口发送的消息,控制和颜色两个消息
	CStatic		m_ControlBkclr;				//静态图片控件背景颜色
	CStatic		m_ControlLightclr;			//静态图片控件灯光颜色
	COLORREF	m_ClrBk;					//背景颜色
	COLORREF	m_ClrLight;					//灯光的颜色
	CBrush		m_brush;					//用于绘制图片控件的画刷
	BOOL		m_bScaleAll;				//是否同时放大缩小
	CSliderCtrl m_xSlider;
	CSliderCtrl m_ySlider;
						
public:	
	void ScWndToOrg();								//让窗口滑回到原始位置
	void GetMessageID(UINT nMsg,UINT nMsgClr);		//取得向父窗口发送消息的消息ID
};
