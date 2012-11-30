#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CControlBoard �Ի���

class CControlBoard : public CDialog
{
	DECLARE_DYNAMIC(CControlBoard)

public:
	CControlBoard(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CControlBoard();

// �Ի�������
	enum { IDD = IDD_DLG_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	int			m_nViewHeight;				//�������ĸ߶�
	int			m_nVPageSize;				//������ÿҳ�ĸ߶�
	int			m_nVScrollPos;				//��ǰ��������λ��
	UINT		m_nMsg[2];					//�򸸴��ڷ��͵���Ϣ,���ƺ���ɫ������Ϣ
	CStatic		m_ControlBkclr;				//��̬ͼƬ�ؼ�������ɫ
	CStatic		m_ControlLightclr;			//��̬ͼƬ�ؼ��ƹ���ɫ
	COLORREF	m_ClrBk;					//������ɫ
	COLORREF	m_ClrLight;					//�ƹ����ɫ
	CBrush		m_brush;					//���ڻ���ͼƬ�ؼ��Ļ�ˢ
	BOOL		m_bScaleAll;				//�Ƿ�ͬʱ�Ŵ���С
	CSliderCtrl m_xSlider;
	CSliderCtrl m_ySlider;
						
public:	
	void ScWndToOrg();								//�ô��ڻ��ص�ԭʼλ��
	void GetMessageID(UINT nMsg,UINT nMsgClr);		//ȡ���򸸴��ڷ�����Ϣ����ϢID
};
