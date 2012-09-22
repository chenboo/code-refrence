#pragma once


// CElcStatusBar

enum ELC_STATUSBAR_ITEM_TYPE {
	ELC_SBIT_NONE,
	ELC_SBIT_IMAGE,
	ELC_SBIT_BUTTON,
	ELC_SBIT_CTRL,
};

typedef struct ELC_STATUSBAR_ITEM {
	UINT			nType;
	DWORD_PTR		dwCtrl;
	SIZE			size;
	DWORD_PTR		dwData; // user data
} *PELC_STATUSBAR_ITEM;

class LIBELCBASEUI_API CElcStatusBar : public CStatusBar
{
	DECLARE_DYNAMIC(CElcStatusBar)

public:
	CElcStatusBar();
	virtual ~CElcStatusBar();

	BOOL LoadSkin(LPCTSTR lpszStyle);

	BOOL SetIndicators(const UINT* lpIDArray, int nIDCount);

public:
	// Insert specified control, such as image, button, children window and so on.
	// Insert image
	int InsertImage(int nIndex, UINT nImageResId, LPCTSTR lpszType=RT_PNG);
	// Insert button
	int InsertButton(int nIndex, UINT nId, LPCTSTR lpszCaption, UINT nResId, LPCTSTR lpszType=RT_PNG, ELC_BUTTON_ICON_TYPE type=ELCBIM_SINGLE);
	// Insert label
	int InsertLabel(int nIndex, LPCTSTR lpszText);
	// Insert Control. The control's parent window will be change to CElcStatusBar in this function.
	int InsertControl(int nIndex, CWnd* pWnd, CSize size);

	BOOL SetImage(int nIndex, UINT nImageResId, LPCTSTR lpszType=RT_PNG);
	CElcButton* GetButton(UINT nId);

	void DelItem(int nIndex);
	UINT GetItemType(int nIndex);

	void SetItemData(int nIndex, DWORD_PTR dwData);
	DWORD_PTR GetItemData(int nIndex);

protected:
	BOOL InsertCtrl(int nIndex, PELC_STATUSBAR_ITEM pItem, LPCTSTR lpszText);

	void Relayout();
	void DrawItems(CDC* pDC);

protected:
	typedef std::vector<ELC_STATUSBAR_ITEM>		CItemArray;
	CItemArray									m_arrItem;

	ELC_SKIN_STATUSBAR		m_skin;
	CRect					m_rcClient;
	CFont					m_ftText;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};


