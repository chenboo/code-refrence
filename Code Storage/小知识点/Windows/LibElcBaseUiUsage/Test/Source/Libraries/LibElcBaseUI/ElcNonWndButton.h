/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////



#pragma once 

#include "ElcNonWndCtrl.h"

enum ELC_NWBUTTON_STATE {
	ENWS_NORMAL,
	ENWS_HOT,
	ENWS_DOWN,
	ENWS_DISABLED,
};


class LIBELCBASEUI_API CElcNonWndButton : public CElcNWControl
{
public:
	CElcNonWndButton(void);
	~CElcNonWndButton(void);

	BOOL Create(LPCTSTR lpszText, HWND hParent, CRect rect, UINT nId);
	void SetNotifyWindow(HWND hWnd) {
		m_hNotify = hWnd;
	}
	void SetNotifyWindow(HWND hWnd, UINT nMsg) {
		m_hNotify = hWnd;
		m_nMsg = nMsg;
	}

	BOOL SetIcon(UINT nResId, LPCTSTR lpszType, int nCount, BOOL bVertical);
	void SetIcon(PELC_SKIN_IMAGE pIcon);

	void LoadBackgroundSkin(LPCTSTR lpStyle);

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

	void Enable(BOOL bEnable) {
		if ((bEnable && m_state == ENWS_DISABLED) || (!bEnable && m_state != ENWS_DISABLED)) {
			m_state = bEnable ? ENWS_NORMAL : ENWS_DISABLED;
			Repaint(m_rect);
		}
	}
	BOOL IsEnable() {
		return (m_state != ENWS_DISABLED);
	}

	void SetCheck(BOOL bChecked) {
		if (m_state != ENWS_DISABLED) {
			if ((bChecked && m_state != ENWS_DOWN) || (!bChecked && m_state == ENWS_DOWN)) {
				m_state = bChecked ? ENWS_DOWN : ENWS_NORMAL;
				Repaint(m_rect);
			}
		}
	}
	BOOL IsChecked() {
		return m_state == ENWS_DOWN;
	}

	CSize GetIdealSize();
	CSize GetBkImageSize();
	CSize GetIconSize();

protected:
	void Repaint(CRect rect);

protected:
	HWND				m_hParent;
	HWND				m_hNotify;
	UINT				m_nMsg;
	UINT				m_nId;
	BOOL				m_nEnableRedraw;

	UINT				m_nIconResId;
	ELC_SKIN_IMAGE		m_icon;
	BOOL				m_bReleaseIconRes;
	ELC_SKIN_BUTTON		m_background;

	CRect				m_rect;
	CString				m_strText;
	CFont				m_ftText;

	ELC_NWBUTTON_STATE	m_state;
	BOOL				m_bMouseDown;
};