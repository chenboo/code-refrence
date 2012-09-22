#pragma once
#include "FontDlg.h"
#include "CText.h"

#define WM_FONTCHANGED		(WM_USER + 20)

class CTextEditWnd : public CEdit
{
public:
	CTextEditWnd();
	virtual ~CTextEditWnd();

public:
	void SetTextFont(CFont& font);
	void SetTextGraph(CText* pText);
	void SetParentWnd(CWnd* pWndParent);
	void SetNotifyWindow(HWND hWnd, UINT nMsg);
	
	void InitFont();
	void ResetEditInfo();

	CSize GetTextSize();
	BOOL IsNoreference();
	void InCreaseReference();
	void DeCreaseReference();

	void ChangeEditSize();
	void MoveEditBox(CRect rect);
	void ShowEditWindow(BOOL bShow);

public:
	CText*			m_pText;
	CFont			m_Font;
	CRect			m_rcEdit;
	CFontDlg		m_DlgFont;

	UINT			m_nMsg;
	HWND			m_hNotifyWnd;
	int				m_nTextHeight;
	int				m_nReference;

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CTextEditWnd)

	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnFontChanged(WPARAM, LPARAM);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


