#pragma once
#include "afxwin.h"
#include "ButtonManager.h"

enum FONTDLG_NOTIFY_TYPE {
	FDNT_BOLD = 1,
	FDNT_UNDERLINE,
	FDNT_ITALIC,
	FDNT_FONTNAME,
	FDNT_FONTSIZE,
};

class CFontDlg : public CDialog
{
	DECLARE_DYNAMIC(CFontDlg)

public:
	CFontDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFontDlg();
	enum { IDD = IDD_DLG_FONT};
public:
	void InitComFtname();
	void InitComFtsize();
	void InitFont();
	void InitButton();

	void SetTextFont(CFont& font);
	void SetNotiFyWindow(HWND hWnd, UINT nMsg);
	void ModifyFont(int nType);
	void ResetFontInfo();

	void DrawButons();

	void OnBnClickedBtnBold();
	void OnBnClickedBtnItalic();
	void OnBnClickedBtnUnderline();

public:
	CComboBox		m_ComFtname;
	CComboBox		m_ComFtsize;

	UINT			m_nMsg;
	HWND			m_hNotifyWnd;

	CFont			m_Font;
	BOOL			m_bBold;
	BOOL			m_bItalic;
	BOOL			m_bUnderline;

	int				m_nFontSel;
	BOOL			m_bTracking;
	CButtonManager	m_btnManager;
	
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	afx_msg void OnPaint();
	afx_msg void OnCbnSelchangeComboFontname();
	afx_msg void OnCbnSelchangeComboFontsize();

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg LRESULT OnBtnclicked(WPARAM, LPARAM);
};
