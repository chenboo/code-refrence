#pragma once
#include "afxwin.h"
#include <Winsock2.h>


// CChatRoomClientDlg �Ի���
class CChatRoomClientDlg : public CDialog
{
// ����
public:
	CChatRoomClientDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CChatRoomClientDlg();

	enum { IDD = IDD_CHATROOMCLIENT_DIALOG };

public:
	BOOL InitWinsock();

public:
	CListBox	m_ListMsg;
	HANDLE		m_hThread;
	CString		m_strMsg;

	SOCKET		m_socket;

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnLink();
	afx_msg void OnBnClickedBtnSend();
};

DWORD WINAPI CLIThreadProc(LPVOID pParam);