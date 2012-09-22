#pragma once

#include <vector>
#include "BarButton.h"

#define ELCRLS_EXPAND	0x00000001
#define ELCRLS_SELECT	0x00000002
#define ELCRLS_BOLD		0x00000004


#define ELCRLS_ITEM_IMAGE		0x00000001
#define ELCRLS_SUBITEM_IMAGE	0x00000002
#define ELCRLS_EXPAND_BTN		0x00000004

enum ELCRL_NOTIFY
{
	ELCRLN_DBLCLK	= 1,
	ELCRLN_SELECT,
	ELCRLN_BTN_CLICKED,
};


typedef struct ELCRICHLIST
{
	int nItem;
	int nSubItem;
	ELCRL_NOTIFY code;
	UINT nID;
}*LPELCRICHLIST;

typedef struct ELCRICHLIST_BUTTON_NOTIFY : public ELCRICHLIST
{
	UINT nBtnID;
}*LPELCRICHLIST_BUTTON_NOTIFY;

class LIBELCBASEUI_API CElcRichList : public CWnd
{
	DECLARE_DYNAMIC(CElcRichList)

public:
	typedef struct ELCLIST_BUTTON
	{
		UINT			nID;
		CBarButtonEx*	pBtn;
		CRect			rcBtn;
		BOOL			bShow;
	}*PELCLIST_BUTTON;

	typedef std::vector<ELCLIST_BUTTON> CButtonArray;

	typedef struct LIST_SUBITEM
	{
		CString		strText;
		int			nImage;

		COLORREF	crText;
	}*PLIST_SUBITEM;

	typedef std::vector<LIST_SUBITEM> CSubItemArray;

	typedef struct ELCRLIST_ITEM
	{
		CString			strItem;
		int				nImage;

		DWORD			dwState;

		int				nExpandHeight;

		COLORREF		crText;

		CSubItemArray	arrSubItem;

		WORD			wBtnsHiId;
		CButtonArray	arrButton;

		DWORD_PTR		dwData;
	}*PELCRLIST_ITEM;

	typedef std::vector<ELCRLIST_ITEM> CItemArray;

	
	enum EXPAND_IMAGE_INDEX {
		ELCRLS_EI_EXPAND = 0,
		ELCRLS_EI_FOLD,
	};

public:
	CElcRichList();
	virtual ~CElcRichList();

public:

	void SetNotifyWindow(HWND hWnd, UINT nMsg);

	// add or delete item
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage=0);
	BOOL DeleteItem(int nItem);
	void DeleteAllItem();
	int GetItemCount();

	// add or delete subitem
	BOOL AddSubItem(int nItem, LPCTSTR lpszText, int nImage=0);
	BOOL DeleteSubItem(int nItem, int nSubItem);
	BOOL DeleteAllSubItem(int nItem);
	int GetSubItemCount(int nItem);

	BOOL SetItemData(int nItem, DWORD_PTR dwData);
	DWORD_PTR GetItemData(int nItem);

	void ExpandItem(int nItem, BOOL bExpand);
	void ExpandAll(BOOL bExpand);

	CImageList* SetImageList(CImageList* pImageList);
	CImageList* GetImageList() const;

	void SetItemText(int nItem, LPCTSTR lpszText);
	CString GetItemText(int nItem);

	void SetSubItemText(int nItem, int nSubItem, LPCTSTR lpszText);
	CString GetItemText(int nItem, int nSubItem);

	void SetItemState(int nItem, DWORD dwState, DWORD dwMask);
	DWORD GetItemState(int nItem, DWORD dwMask);

	BOOL SetItemImage(int nItem, int nImage);
	int GetItemImage(int nItem);

	BOOL SetSubItemImage(int nItem, int nSubItem, int nImage);
	int GetSubItemImage(int nItem, int nSubItem);

	BOOL SetItemTextColor(int nItem, COLORREF color, BOOL bReDraw = TRUE);
	BOOL SetSubItemTextColor(int nItem, int nSubItem, COLORREF color, BOOL bReDraw=TRUE);


	void SetRedraw(BOOL bRedraw = TRUE);


	// add a 24-bit bitmap button
	// old interface, disuse.
	// need to removed
	BOOL AddButton(
		int nItem, 
		UINT nBtnID, 
		UINT nBitmapID, 
		LPCTSTR lpszTip = NULL, 
		BOOL bShow = TRUE);
	// add a png, jpg, gif, bmp button. 
	// specify by "lpszType", see CBarButtonEx, RT_BITMAP, RT_PNG, RT_ICON...
	BOOL AddButton(
		int nItem, 
		UINT nBtnID, 
		UINT nResId, 
		LPCTSTR lpszType, 
		LPCTSTR lpszBtnText, 
		LPCTSTR lpszTip = NULL,
		BOOL bShow = TRUE);
	// add a text-linker style button
	// see CBarButtonEx for more detail
	BOOL AddTextlinkerButton(
		int nItem, UINT nBtnID, LPCTSTR lpszText, 
		int nUnderlineStyle = ELCBTS_UNDERLINE_WHEN_HOT, 
		COLORREF crNormal = RGB(14,87,163), 
		COLORREF crHot = RGB(204,0,0),
		BOOL bAnimateDown = TRUE);

	BOOL DelButton(int nItem, UINT nBtnID);

	BOOL ShowButton(int nItem, UINT nBtnID, BOOL bShow);
	BOOL EnableButton(int nItem, UINT nBtnID, BOOL bEnable);

	BOOL SetButtonText(int nItem, UINT nBtnID, LPCTSTR lpszText);
	CString GetButtonText(int nItem, UINT nBtnID);

	void EnableToolTip(BOOL bEnable=TRUE);


	//////////////////////////////////////
	// list property
	void SetBkColor(COLORREF color);
	COLORREF GetBkColor();

	void SetItemTextColor(COLORREF color);
	COLORREF GetItemTextColor();

	void SetSubItemTextColor(COLORREF color);
	COLORREF GetSubItemTextColor();

	void SetSelectedItemBkColor(COLORREF color);
	COLORREF GetSelectedItemBkColor();

	void SetItemFont(CFont* pFont);
	CFont* GetItemFont();

	void SetHMargin(int nMargin);
	int GetHMargin();

	void SetSubItemIndent(int nIndent);
	int GetSubItemIndent();

	void SetItemImageShow(BOOL bShow);
	BOOL IsItemImageShow();

	void SetSubItemImageShow(BOOL bShow);
	BOOL IsSubItemImageShow();

	void SetExpandButtonShow(BOOL bShow);
	BOOL IsExpandButtonShow();

	void SetItemABColor(COLORREF crItemA, COLORREF crItemB, BOOL bReDraw=TRUE);
	void GetItemABColor(COLORREF& crItemA, COLORREF& crItemB);

	void SetItemHeight(int nHeight);
	int GetItemHeight();

protected:

	void Repaint();

	void DrawItem(CDC* pDC);
	int DrawSubItem(CDC* pDC, CSubItemArray* pSubItem, int nTop);
	void DrawSubItem(CDC* pDC, PLIST_SUBITEM pItem, CRect rect);

	void UpdateScrollInfo();
	int GetListHeight();
	void GetHitItem(CPoint point, int& nItem, int& nSubItem, BOOL& bHitExpand);

	void HideAllButtons();
	void HideButtons(CButtonArray* parrButton);
	void RemoveButtons(CButtonArray* parrButton);

	int FindButton(int nItem, UINT nBtnID);
	// find button in which item (m_arrItem)
	BOOL FindButtonInItem(UINT nBtnID, int& nItem);

	WORD GetButtonsHiId();

protected:
	CItemArray		m_arrItem;

	CImageList*		m_pImageList;
	CSize			m_szImage;

	CImageList		m_ExpandImage;
	CSize			m_szExpandImage;

	COLORREF		m_crListBk;
	COLORREF		m_crSelectItemBk;
	COLORREF		m_crItemText;
	COLORREF		m_crSubItemText;
	COLORREF		m_crItemA;
	COLORREF		m_crItemB;

	CFont			m_ftItem;
	CFont			m_ftBoldItem;

	CRect			m_rcClient;

	int				m_nItemHeight;
	int				m_nHMargin;
	int				m_nSubItemIndent;

	HWND			m_hNotifyWnd;
	UINT			m_nNotifyMsg;

	BOOL			m_bEnableRedraw;

	CToolTipCtrl	m_wndToolTip;
	BOOL			m_bEnableTip;

	DWORD			m_dwListState;

	int				m_nVMargin;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg LRESULT OnButtonClicked(WPARAM wParam, LPARAM lParam);
};


