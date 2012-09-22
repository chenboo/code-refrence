#pragma once
#include "afxcmn.h"
#include "afxwin.h"



class CSelectMemberDlg : 
	public CElcSkinDialog,
	public ITeacherFrameNotify
{
	DECLARE_DYNAMIC(CSelectMemberDlg)

public:
	CSelectMemberDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectMemberDlg();

	enum { IDD = IDD_SELECT_MEMBER };

	void SetParams(PSTUDC_PARAMS pParams, int nCount, PMEMBERINFO* ppMemberInfo);

	// ITeacherFrameNotify
	virtual void OnMemberConnected(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnMemberDisconnected(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnMemberAdded(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnMemberRemoved(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnMemberNameChanged(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnMemberHandUp(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnMemberHandDown(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData);
	virtual void OnFunctionChanged(int nFunction, UINT64 dwCommand, BOOL bStart);


protected:
	void InitList();
	int GetItemImage(PMEMBERINFO pMemberInfo);

	int FindItemByMac(BYTE* abMac);

	BOOL HasMember(PMEMBERINFO pMemberInfo);

private:
	CImageList						m_imgList;
	CListCtrl						m_wndMemberList;
	CButton							m_btnSelectAll;
	CElcStatic		m_wndIcon;

	ITeacherFrame*					m_pTeacherFrame;
	UINT64							m_dwFuncMask;
	IStudentCheckDialogCallback*	m_pCallback;
	PMEMBERINFO						m_pExcludeMember;

	int								m_nCount;
	PMEMBERINFO						m_ppMemberInfo[ELC_MAX_MEMBER_COUNT];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnBnClickedBtnHelp();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedCheckSelectAll();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};
