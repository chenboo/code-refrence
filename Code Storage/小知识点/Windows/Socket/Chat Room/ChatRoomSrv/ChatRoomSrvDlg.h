#pragma once
#include <Winsock2.h>
#include "afxwin.h"

// CChatRoomSrvDlg dialog
class CChatRoomSrvDlg : public CDialog
{
// Construction
public:
	CChatRoomSrvDlg(CWnd* pParent = NULL);	// standard constructor
	~CChatRoomSrvDlg();
	enum { IDD = IDD_CHATROOMSRV_DIALOG };

public:
	BOOL InitWinsock();

public:
	SOCKET		m_soketSrv;
	SOCKET		m_soketCon;
	HANDLE		m_hThread;
	CListBox	m_ListMsg;
	CString		m_strInput;

// Implementation
protected:
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnSend();
};

DWORD WINAPI SRVThreadProc(LPVOID pParam);

BOOL CreateSrvSocket(CChatRoomSrvDlg* pDlg);
BOOL GetPortNumber(CChatRoomSrvDlg* pDlg,int& nPort);
BOOL BindAddress(SOCKET& socketSrv,int nPort);
BOOL ListenSocket(SOCKET& socketSrv,SOCKET& socketCon);
BOOL AcceptSocket(SOCKET& socketSrv,SOCKET& socketCon,sockaddr_in& addrclt);
CString ReceiveMessage(SOCKET& socketSrv,SOCKET& socketCon,sockaddr_in& addrclt);
