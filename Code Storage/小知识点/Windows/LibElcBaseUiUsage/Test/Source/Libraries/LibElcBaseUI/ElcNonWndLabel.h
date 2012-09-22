/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////



#pragma once 

#include "ElcNonWndCtrl.h"


enum ELC_NWLABEL_ALIGN {
	ENWLA_LEFT = 0,
	ENWLA_CENTER,
	ENWLA_RIGHT,
};

class LIBELCBASEUI_API CElcNonWndLabel : public CElcNWControl
{
public:
	CElcNonWndLabel(void);
	~CElcNonWndLabel(void);

	BOOL Create(LPCTSTR lpszText, HWND hParent, CRect rect, UINT nId);

	void SetTextColor(COLORREF crText) {
		m_crText = crText;
		Repaint(m_rect);
	}
	COLORREF GetTextColor() {
		return m_crText;
	}

	void SetText(LPCTSTR lpszText, BOOL bRedraw=TRUE) {
		m_strText = lpszText;
		if (bRedraw)
			Repaint(m_rect);
	}
	CString GetText() {
		return m_strText;
	}

	void SetAlign(int nAlign, BOOL bRedraw=TRUE) {
		m_nAlign = nAlign;
		if (bRedraw)
			Repaint(m_rect);
	}
	int GetAlign() {
		return m_nAlign;
	}

	void SetFont(CFont *pFont);

	virtual UINT GetId();
	virtual BOOL PtInRect(CPoint point);
	virtual CRect GetRect();
	virtual void Move(CRect rcNew, BOOL bRedraw);
	virtual BOOL MouseDown(CPoint point);
	virtual BOOL MouseUp(CPoint point);
	virtual BOOL MouseMove(CPoint point);
	virtual void CaptureChanged();
	virtual void Draw(PVOID pvGraphics);
	virtual void SetRedraw(BOOL bEnable);

protected:
	void Repaint(CRect rect);

protected:
	HWND				m_hParent;
	UINT				m_nId;
	int					m_nAlign;
	BOOL				m_nEnableRedraw;

	CRect				m_rect;
	CString				m_strText;
	CFont				m_ftText;
	COLORREF			m_crText;
};

