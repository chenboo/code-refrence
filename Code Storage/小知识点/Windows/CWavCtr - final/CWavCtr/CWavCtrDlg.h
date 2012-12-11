// CWavCtrDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "wavwnd.h"
#include "MySlider.h"


// CCWavCtrDlg �Ի���
class CCWavCtrDlg : public CDialog
{
// ����
public:
	CCWavCtrDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CWAVCTR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	bool FileOpen;
	CMySlider CS_Progress;
	CString time;
	CString WavInfo;
	CWavWnd wavWnd;
	afx_msg void OnBnClickedOk();
	void AnalyWav(void);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedSop();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
