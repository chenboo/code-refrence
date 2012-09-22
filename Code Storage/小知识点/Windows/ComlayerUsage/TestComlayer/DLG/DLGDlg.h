#pragma once

class CDLGDlg : public CDialog,
	public IEventManagerNotify,
	public IFrameAssembler::ICallback
{
public:
	CDLGDlg(CWnd* pParent = NULL);	// standard constructor
	enum { IDD = IDD_DLG_DIALOG };

protected:
	BOOL Startup();
	void Shutdown();

	//IFrameAssembler::ICallback Interface
	virtual void OnFrame(LPCVOID pvFrame, int cbFrame, DWORD dwParam);
	virtual void OnNewSession(DWORD dwSessionID, DWORD dwParam){};
	virtual void OnIncompleteFrame(LPCVOID pvFrame, int cbFrame, DWORD dwParam){};

	//IEventManagerNotify interface
	virtual void OnEventArrived(LPVOID lpvUserValue){};

protected:
	HICON							m_hIcon;

	IEventItemManager*				m_pEventMgr;
	ICommunicationLayer*			m_pComLayer;
	IReliableMulticastSender *		m_pSBDSender;
	IReliableMulticastReceiver *	m_pSBDReceiver;
	IFrameAssembler *				m_pSBDAssembler;

	typedef struct tagSHAREBOARDPARAMS
	{
		DWORD		dwCmdIpAddress;
		WORD		wCmdPort;
		DWORD		dwDataIpAddress;
		WORD		wDataPort;
		float		fBitrateKbps;
		float		fWindowDurationSeconds;
		float		fRedundancyRate;
		WORD		wMaxPayloadSize; 
		WORD		wNumDataPacketsInGroup;
	}SHAREBOARDPARAMS, *PSHAREBOARDPARAMS;

	typedef struct SBSERVER_PARAM
	{
		HWND		hWndNotify;
		UINT		nNotifyMsg;
		TCHAR		szName[32];
		DWORD		dwLocalInterface;
		SHAREBOARDPARAMS	params;
	}*PSBSERVER_PARAM;

	SBSERVER_PARAM					m_param;

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
