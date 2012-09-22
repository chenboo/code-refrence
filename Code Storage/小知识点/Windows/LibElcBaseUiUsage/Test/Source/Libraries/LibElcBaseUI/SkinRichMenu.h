#pragma once

#include <map>
using std::map;

// CSkinRichMenu
typedef struct MENU_ITEM {
	UINT		nID;
	CString		strCap;
	CString		strDesc;
	Image*		pImage;
	BOOL		bEnable;
	BOOL		bCheck;

	MENU_ITEM() {
		nID = 0;
		pImage = NULL;
		bEnable = 0;
		bCheck = 0;
	}
	MENU_ITEM& operator = (MENU_ITEM& item) {
		nID = item.nID;
		strCap = item.strCap;
		strDesc = item.strDesc;
		pImage = item.pImage;
		bEnable = item.bEnable;
		bCheck = item.bCheck;
		return *this;
	}
}*PMENU_ITEM;

typedef map<DWORD, MENU_ITEM> CMenuItemMap;

enum {
	LEFT_TOP = 0x01,
	RIGHT_TOP,
	LEFT_BOTTOM,
	RIGHT_BOTTOM,
	TOP_CENTER,
	BOTTOM_CENTER
};

class LIBELCBASEUI_API CSkinRichMenu : public CWnd
{
	DECLARE_DYNAMIC(CSkinRichMenu)

public:
	CSkinRichMenu();
	virtual ~CSkinRichMenu();

	BOOL ShowPopMenu(int x, int y, int nFlag, CWnd* pParent = NULL);

public:
	void SetIconSize(SIZE sz);
	void SetDescRow(int nRow);

	void SetMenuWidth(int nWidth);
	int GetMenuWidth();

	void SetCapFont(CFont* pFont);
	void SetDescFont(CFont* pFont);

	void SetHoverBmp(CBitmap* pBmp);
	void SetBkColor(COLORREF clr);
	void SetTextColor(COLORREF clr);

	void SetNotifyWindow(HWND hWnd, UINT uMsg);

	void AddItem(UINT nId, 
		LPCTSTR lpszItem, 
		LPCTSTR lpszDesc, 
		UINT nIconResId, 
		LPCTSTR lpsType=RT_PNG, 
		BOOL bEnable=TRUE, 
		BOOL bChecked=FALSE);
	void ClearItem();

protected:
	// create & destroy
	BOOL Create(const RECT& rect, CWnd* pParentWnd);
	void DestroyMe();

	// position
	void CalculateRect();
	POINT GetAlignPos(int x, int y, int nFlag);

	// draw item
	void MemDrawItem();
	void DrawItem(CDC* pDC, PMENU_ITEM pItem, int nIndex);
	void DrawHoverItem(CDC* pDC);

private:
	HWND			m_hNotify;
	UINT			m_nMsg;

	COLORREF		m_clrBackground;
	COLORREF		m_clrText;
	COLORREF		m_clrDisable;

	CFont			m_ftCap;
	CFont			m_ftDesc;

	CDC				m_dcMemHover;
	CDC				m_dcMemItem;

	CBrush			m_brFrame;
	CBrush			m_brItem;
	CBrush			m_brCheck;

	CRect			m_rcClient;
	CRect			m_rcItem;
	CRect			m_rcHover;
	CRect			m_rcIcon;
	CRect			m_rcCap;
	CRect			m_rcDesc;

	CMenuItemMap	m_MenuItem;

	int				m_nItemWidth;

	int				m_cItemNum;
	int				m_nHoverItem;

	int				m_nCapHeight;
	int				m_nDescHeight;

	int				m_cDescRow;

	BOOL			m_bDrawMemItem;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
};


