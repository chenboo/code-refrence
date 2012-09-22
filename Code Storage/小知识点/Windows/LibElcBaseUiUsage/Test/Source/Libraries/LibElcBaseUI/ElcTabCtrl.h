#pragma once

#include <vector>
using std::vector;

#define MIN_TABITEM_WIDTH	50
#define MAX_TABITEM_WIDTH	150

enum ELCTAB_LAYOUT
{
	ELCTAB_LAYOUT_DOWN = 0,
	ELCTAB_LAYOUT_UP,
};

typedef struct ELCTABITEM {
	UINT		nId;
	int			nImage;
	CString		strText;
	CSize		size;
	BOOL		bActive;
	CRect		rcItem;
	DWORD_PTR	dwData;

public:
	ELCTABITEM() {
		nImage = 0;
		bActive = 0;
		rcItem = CRect(0,0,0,0);
		dwData = 0;
	}

}*PELCTABITEM;

typedef std::vector<ELCTABITEM> CELCTabItemArray;

typedef struct ELCTABCTRL_BUTTON {
	UINT				nId;
	ELC_SKIN_IMAGE		image;
	UINT				state;
	BOOL				bShow;
	RECT				rect;
	CString*			pStrTip;
} *PELCTABCTRL_BUTTON;

enum ELCTABCTRL_EVENT {
	ELCTAB_NEWITEM = 1,
	ELCTAB_CLOSEITEM,
	ELCTAB_ITEMCHANGING,
	ELCTAB_ITEMCHANGED,
	ELCTAB_ITEM_DOUBLE_CLICK,
};

class LIBELCBASEUI_API CElcTabCtrl : public CWnd
{
	DECLARE_DYNAMIC(CElcTabCtrl)

public:
	CElcTabCtrl();
	virtual ~CElcTabCtrl();

	BOOL LoadSkin(LPCTSTR lpszStyle);

public:
	void SetNotifyWindow(HWND hWnd, UINT nMessage);

	// old image list function, 
	CImageList* SetImageList(CImageList* pImageList);
	CImageList* GetImageList( ) const;

	// new image list function, support png
	BOOL SetImageListEx(UINT nResId, LPCTSTR lpszType=RT_PNG, int cxUnit=16, int cyUnit=16);

	int AddItem(LPCTSTR pszText, int nImage=0);
	int InsertItem(int nItem, LPCTSTR pszText, int nImage=0);
	BOOL DelItem(int nItem);
	void DelAllItem();

	int GetItemCount();

	BOOL SetItemData(int nItem, DWORD_PTR dwData);
	DWORD_PTR GetItemData(int nItem);

	BOOL SetItemText(int nItem, LPCTSTR pszText);
	CString GetItemText(int nItem);
	BOOL SetItemImage(int nItem, int nImage);
	int GetItemImage(int nItem);

	void SetCurSel(int nItem);
	int GetCurSel();

	void SetBkColor(COLORREF cr);
	COLORREF GetBkColor();

	// set the enum ELCTAB_LAYOUT
	void SetTabLayout(ELCTAB_LAYOUT layout);

	void EnableToolTip();

	void ShowBorder(BOOL bShow);
	BOOL IsBorderVisible();


	void EnableCloseButton(BOOL bEnable);
	BOOL IsCloseButtonEnable();
	void SetCloseButtonTip(LPCTSTR lpszText);

	void ShowCloseButton(BOOL bShow);
	BOOL IsCloseButtonVisiable();

	void EnableNewButton(BOOL bEnable);
	BOOL IsNewButtonEnable();
	void SetNewButtonTip(LPCTSTR lpszText);
	void SetNewButtonTip(UINT nTextId);

	void ShowNewButton(BOOL bShow);
	BOOL IsNewButtonVisiable();

	void SetDropdownButtonTip(LPCTSTR lpszText);
	void SetDropdownButtonTip(UINT nTextId);

	void SetLeftMargin(int nMargin);
	int GetLeftMargin();
	void SetRightMargin(int nMargin);
	int GetRightMargin();

	void SetItemMinWidth(int nWidth);
	int GetItemMinWidth();
	void SetItemMaxWidth(int nWidth);
	int GetItemMaxWidth();


	// these functions are used for testing point in which one
	// point : in screen coordinates
	// return the index of the item which point in
	// return the count of items if point in the new button
	// return -1 if not in any items or new button
	int PtInItem(CPoint point);
	void ShowFocusRect(int nItem);
	void ShowFocusRect(CPoint point);

protected:
	void DrawTab(CDC* pDC);
	void DrawItem(CDC* pDC, PELCTABITEM pItem, int nImageIndex);
	void DrawButton(CDC* pDC, PELCTABCTRL_BUTTON pButton);

	void DrawFocusRect(CDC* pDC);

	void Repaint();
	void UpdateTooltips();

	void Resize();
	void CalcTabItemsPos();
	int CalcTabWidth(int nFirstItem, int nLastItem);
	void SetBtnsPos();

	CSize GetStringSize(LPCTSTR lpszText);

	int GetActiveItem(CPoint point);
	PELCTABCTRL_BUTTON TrackMouse(CPoint point);

	void CheckMouseInButton(CPoint point);
	BOOL CheckButtonState(CPoint point, PELCTABCTRL_BUTTON pButton);

	int FindButton(int nID);

	void ButtonPressed(PELCTABCTRL_BUTTON pButton);
	
protected:
	ELC_SKIN_TABCTRL		m_skin;
	BOOL					m_bShowBorder;

	CImageList*				m_pImageList;
	CSize					m_szImage;
	Image*					m_pImageListEx;

	CRect					m_rcClient;
	CRect					m_rcTabs;

	ELCTABCTRL_BUTTON		m_tbtnClose;
	ELCTABCTRL_BUTTON		m_tbtnNew;
	ELCTABCTRL_BUTTON		m_tbtnDrop;
	PELCTABCTRL_BUTTON		m_pHotButton;

	CELCTabItemArray		m_arrItem;
	int						m_nBtnsWidth;

	int						m_nItemMinWidth;
	int						m_nItemMaxWidth;

	int						m_nLeftMargin; // ctrl's left margin
	int						m_nRightMargin; // ctrl's right margin
	int						m_nItemsMargin; // The distance between tab item and button (such as new button, dropdown button)
	int						m_nTabsMargin; // The distance between adjacent tab items
	int						m_nItemInnerMargin; // The distance in a tab item

	int						m_nActiveItem;
	int						m_nHotItem;
	int						m_nFirstVisibleItem;
	int						m_nLastVisibleItem;
	int						m_nFocusRectItem;

	CFont					m_ftText;
	CFont					m_ftBoldText;

	HWND					m_hNotify;
	UINT					m_nNotify;

	CToolTipCtrl			m_wndToolTip;

	BOOL					m_bMirror;

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnActiveItem(UINT uIndex);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
};


