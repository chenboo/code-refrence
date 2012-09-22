#pragma once


// CRollLabel

enum ELCRLABEL_TEXT_ALIGN {
	ELCRLABEL_TEXT_ALIGN_LEFT = 1,
	ELCRLABEL_TEXT_ALIGN_CENTER,
	ELCRLABEL_TEXT_ALIGN_RIGHT
};

class LIBELCBASEUI_API CRollLabel : public CWnd
{
	DECLARE_DYNAMIC(CRollLabel)

public:
	CRollLabel();
	virtual ~CRollLabel();

	static CSize GetTextSize(LPCTSTR lpszText, CFont* pFont);

	void SetText(LPCTSTR lpszText, COLORREF crText);
	void SetText(LPCTSTR lpszText);
	CString GetText();

	void SetTextColor(COLORREF crText);
	COLORREF GetTextColor();

	BOOL SetTextFont(CFont* pFont);
	CFont* GetTextFont();

	void SetBkColor(BOOL bTransparentBk, COLORREF crBk=RGB(255,255,255));
	COLORREF GetBkColor();
	BOOL IsTransparentBk();
	
	void SetRollElapse(int nMilliSecond); // set roll rate,
	int GetRollElapse();

	void SetTextAlign(int nAlign);
	int GetTextAlign();

protected:
	void UpdateTimer();
	void Repaint();

private:
	enum {
		ID_TIMER_ROLL = 0x70000001,
	};

	CString		m_strText;
	CFont		m_ftText;
	COLORREF	m_crText;
	COLORREF	m_crBk;
	BOOL		m_bRollTimer;
	int			m_nRollElapse;
	CRect		m_rcClient;
	CSize		m_sizeText;
	int			m_nCurXPos;
	int			m_nCurYPos;

	BOOL		m_bTransparentBk;

	int			m_nTextAlign;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


