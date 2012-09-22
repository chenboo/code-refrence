#pragma once
#include "afxwin.h"
#include "CISBitmap.h"

// CDialogDlg dialog
class CDialogDlg : public CDialog
{
public:
	CDialogDlg(CWnd* pParent = NULL);// standard constructor
	enum { IDD = IDD_DIALOG_DIALOG };

public:
	void AddSubDir(HTREEITEM hParent);
	void GetDriveDir(HTREEITEM hParent);
	void GetLogicalDrives(HTREEITEM hParen);
	CString GetFullPath(HTREEITEM hCurrent);

protected:
	HTREEITEM		m_hRoot;
	CTreeCtrl		m_TreeCtrl;
	CListCtrl		m_ListCtrl;
	CImageList		m_ImageList;

// Implementation
protected:
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnTvnItemexpandedTreeDir(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeDir(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclickListDir(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListDir(NMHDR *pNMHDR, LRESULT *pResult);
};
