#pragma once
#include "afx.h"
#include "atltypes.h"


// CWavWnd

class CWavWnd : public CWnd
{
	DECLARE_DYNAMIC(CWavWnd)

public:
	CWavWnd();
	virtual ~CWavWnd();

public:
	void SixDouble();
	void EigDouble();
	void SixSingle();
	void EigSingle();


public:
	int sample_channel;
	int sample_BperSec;
	int sample_Hz;
	CFile wavFile;
	CString FileName;
	CString FilePath;
	double sample_time;
	int current_time;
	double wavSize;
	void* data_ptr;
	bool IsPlaying;
	int sample_bit;
	bool back;
	bool reOpen;
	int m_nDrawed;
	int Current_pos;
	void* m_pData;
	int current_x;
	short current_yL;
	short current_yR;
	bool P_Direction;


protected:
	CDC			m_dcMem;
	CBitmap*	m_pOldBmp;
	CBitmap		m_bitmap;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


