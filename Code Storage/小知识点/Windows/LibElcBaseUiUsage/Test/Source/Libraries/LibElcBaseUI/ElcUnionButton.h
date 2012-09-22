#pragma once


// CElcUnionButton
typedef struct ELC_UNION_BUTTON {
		UINT					nId;
		UINT					nResId;
		CString					strType;
		CString					strCaption;
		CString					strSkinStyle;
		ELC_BUTTON_ICON_TYPE	mode;
	} *PELC_UNION_BUTTON;

class LIBELCBASEUI_API CElcUnionButton : public CWnd
{
	DECLARE_DYNAMIC(CElcUnionButton)

public:
	CElcUnionButton();
	virtual ~CElcUnionButton();

	enum LAYOUT {
		LAYOUT_HORIZONTAL	= 1,
		LAYOUT_VERTICAL		= 2,
	};

public:
	BOOL Create(const PELC_UNION_BUTTON pLeftbutton, 
		const PELC_UNION_BUTTON pRightbutton,
		DWORD dwStyle,
		CWnd * pWndParent,
		UINT nId);

	void SetCmdDrainWindow(HWND hWnd) {
		m_hNotifyWnd = hWnd;
	}

	CBarButtonEx * GetButton(UINT nId);

	CBarButtonEx * GetLeftButton() {
		return &m_btnLeft;
	}
	CBarButtonEx * GetRightButton() {
		return &m_btnRight;
	}

	CSize GetIdeaSize();
	void SetButtonSize(int nLeft);

	// create button external
	BOOL CreateButton(const PELC_UNION_BUTTON pButtonStruct, BOOL bLeft);

protected:
	BOOL CreateButton(const PELC_UNION_BUTTON pButtonStruct, CBarButtonEx * pButton, CSize * pSize);
	void Relayout();

protected:
	CBarButtonEx	m_btnLeft;
	CBarButtonEx	m_btnRight;

	CSize			m_sizeLeft;
	CSize			m_sizeRight;

	LAYOUT			m_layout;
	BOOL			m_bMirror;

	HWND			m_hNotifyWnd;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnElcUIDrawNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnButtonClicked(WPARAM wParam, LPARAM lParam);
};


