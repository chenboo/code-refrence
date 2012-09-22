#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "CodeSnippets/Protocol.h"

#define WM_LIST_CHANGED			(WM_USER + 101)

class IAvailableMemberDlg
{
public:
	virtual void OnAddItem() {}
	virtual void OnDelItem() {}
};

// CAvailableMemberDlg dialog

class CAvailableMemberDlg : 
	public CDialog,
	public ITeacherFrameNotify
{
	DECLARE_DYNAMIC(CAvailableMemberDlg)

public:
	CAvailableMemberDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAvailableMemberDlg();

// Dialog Data
	enum { IDD = IDD_MEMBER_AVAILABLE };

public:
	void EnableCheckItem(BOOL bCheck);
	void ShowSwitchButton(BOOL bShow);
	void EnableMultiSel(BOOL bOn);
	void EnableTeacher(BOOL bEnable);

	BOOL Initialize(PSTUDV_PARAMS pParams);
	void SetCallback(IAvailableMemberDlg* pCallback);
	void SetNotifyWindow(HWND hWnd, UINT nMsg);

	void EnableButtons(BOOL bSet, BOOL bClear);


	void AddItem(PMEMBERINFO pMemberInfo);
	void DelItem(PMEMBERINFO pMemberInfo);

	// ITeacherFrameNotify
	virtual void OnMemberConnected(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnMemberDisconnected(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnMemberNameChanged(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnMemberHandUp(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnMemberHandDown(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnFunctionChanged(int nFunction, UINT64 dwCommand, BOOL bStart);


	CListCtrl		m_wndMemberList;

protected:
	int FindItemByMac(BYTE* abMac);
	void UpdateListMembers();

	CList <DWORD, DWORD&> m_MemberList;
	CList <CString, CString> m_listGroup;

	CBrush		m_brush;
	CImageList	m_imgList;

	
	CButton			m_btnSet;
	CButton			m_btnClear;
	CComboBox		m_wndClassList;
	CEdit			m_wndKeyword;

	BOOL	m_bCheck;
	BOOL	m_bShowBtn;
	BOOL	m_bHasTeacher;

	IAvailableMemberDlg*	m_pCallback;
	ITeacherFrame*			m_pTeacherFrame;
	UINT64					m_dwFuncMask;

	HWND	m_hNofityWnd;
	UINT	m_nNotifyMsg;

	MEMBERINFO m_teacherInfo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCbnSelchangeFilterClass();
	afx_msg void OnDestroy();
	afx_msg void OnMemberListItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedSet();
	afx_msg void OnBnClickedClear();
	afx_msg void OnEnChangeFilterKeyword();
	afx_msg void OnNMDblclkMemberList(NMHDR *pNMHDR, LRESULT *pResult);
};
