#pragma once


// CElcRichMenu

enum ELC_RICH_MENU_ALIGN {
	ELCRMA_LEFT_TOP = 1,
	ELCRMA_RIGHT_TOP,
	ELCRMA_LEFT_BOTTOM,
	ELCRMA_RIGHT_BOTTOM,
	ELCRMA_TOP_CENTER,
	ELCRMA_BOTTOM_CENTER
};

typedef struct ELC_RICHMENU_ITEM {
	UINT			nId;
	DWORD			dwState;

	CString			strText;
	CString			strDesc;

	CSize			sText;
	CSize			sDesc;

	Bitmap*			pIcon;
} *PELC_RICHMENU_ITEM;

class LIBELCBASEUI_API CElcRichMenu : public CWnd
{
	DECLARE_DYNAMIC(CElcRichMenu)

public:
	CElcRichMenu();
	virtual ~CElcRichMenu();

	BOOL LoadSkin(LPCTSTR lpszStyle);

public:
	// show menu after additem finished
	// the control will destroy by itself
	BOOL Popup(int x, int y, UINT nFlag, CWnd* pParent, UINT nId=0);

	// Add Item
	BOOL AddItem(UINT nId, 
		UINT nTextResId, 
		UINT nDescResId, 
		UINT nIconResId, 
		LPCTSTR lpszIconType=RT_PNG);
	BOOL AddItem(UINT nId, 
		LPCTSTR lpszText, 
		LPCTSTR lpszDesc, 
		UINT nIconResId, 
		LPCTSTR lpszIconType=RT_PNG);

	void ClearItems();

	// check item
	void CheckItem(UINT nId, BOOL bChecked);
	BOOL IsItemChecked(UINT nId);

	// enable item
	void EnableItem(UINT nId, BOOL bEnabled);
	BOOL IsItemEnabled(UINT nId);

	// set icon size, default : 32 x 32
	void SetIconSize(int cx, int cy) {
		m_sIcon.SetSize(cx, cy);
		m_nInterMargin = cx / 8;
	}
	void SetIconSize(SIZE size) {
		SetIconSize(size.cx, size.cy);
	}

	void SetMaxDescWidth(int nWidth) {
		m_nMaxDescWidth = nWidth;
	}

protected:
	void Relayout();
	POINT GetAlignPos(int x, int y, int nFlag);

	void DrawMemDC();
	void DrawItem(CDC* pDC, CRect& rcItem, int nIndex, BOOL bHotItem);

	void Destroy();

	int FindItem(UINT nId);

protected:
	typedef std::vector<ELC_RICHMENU_ITEM>	CItemArray;
	CItemArray								m_arrItem;

	ELC_SKIN_RICH_MENU		m_skin;
	CFont					m_ftText;
	CFont					m_ftDesc;
	CWnd*					m_pParent;
	int						m_nId;

	CElcMemDC				m_dcMem;

	CRect					m_rcBody;
	CSize					m_sMaxText;
	CSize					m_sMaxDesc;
	CSize					m_sIcon;
	int						m_nInterMargin;
	int						m_nBorderMargin;
	int						m_nMaxDescWidth;
	int						m_nItemHeight;
	int						m_nHotItem;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
};


