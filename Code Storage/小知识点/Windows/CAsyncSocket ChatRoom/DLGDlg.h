#pragma once

#include "InterSock.h"
#include "afxwin.h"

// CDLGDlg dialog
class CDLGDlg : public CDialog
{
// Construction
public:
	CDLGDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DLG_DIALOG };

public:
	CInterSock		m_InterSock;
	CInterSock		m_InterAccept;
	CListBox		m_ListMsg;
	CString			m_EditMsg;
	BOOL			m_bServer;

// Implementation
protected:
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedRadioSever();
	afx_msg void OnBnClickedRadioClient();
	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnSend();

	afx_msg LRESULT OnAccept(WPARAM,LPARAM);
	afx_msg LRESULT OnReceive(WPARAM,LPARAM);
};
