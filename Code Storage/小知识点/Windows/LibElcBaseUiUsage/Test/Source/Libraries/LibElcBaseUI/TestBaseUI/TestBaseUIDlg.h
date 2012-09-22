// TestBaseUIDlg.h : header file
//

#pragma once

#include "SkinDlgTest.h"
#include "afxwin.h"

// CTestBaseUIDlg dialog
class CTestBaseUIDlg : public CElcSkinDialog
{
// Construction
public:
	CTestBaseUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTBASEUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CElcSlider		m_wndSkinSlider;
	CVolumeSlider	m_wndHSlider;
	CVolumeSlider	m_wndHSlider2;
	CVolumeSlider	m_wndVSlider;

	int m_nHVolume;
	int m_nVVolume;

	CRadioListBox m_wndList1;
	CRadioListBox m_wndList2;
	CRadioListBox m_wndList3;
	CRadioListBox m_wndList4;

	CImageList imgList;

	CBarButton	m_btn1;
	CBarButton	m_btn2;
	CBarButton	m_btn3;
	CBarButtonEx m_btn4;

	CElcTabCtrl		m_wndTab;
	CElcToolBarEx	m_wndBar;
	CElcToolBarEx	m_wndTransparentBar;

	CSkinDlgTest	m_wndSkinDlg;

	CElcFloatBar	m_wndFloatBar;

	CElcStatic		m_wndPngStatic;
	CElcStatic		m_wndText;

	CFont			m_ftText;
	CFont			m_ftBold;

	CElcUnionButton		m_wndUnionButton;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHSliderChanged();
	afx_msg void OnVSliderChanged();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedStudVoice();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton5000();

	afx_msg LRESULT OnSliderNotify(WPARAM wParam, LPARAM lParam);
};
