#pragma once

enum ELC_THUMB_ITEM_STATE {
	ETIS_NONE					= 0x00000000,

	ETIS_SELECTED				= 0x00000001,
	ETIS_FOCUS					= 0x00000002,
	ETIS_TITLE					= 0x00000004,

	ETIS_SELBTN_VISIBLED		= 0x00010000,
	ETIS_SELBTN_CHECKED			= 0x00020000,
	ETIS_SELBTN_ENABLE			= 0x00040000,
	ETIS_SELBTN_MASK			= 0x000F0000,

	ETIS_FUNCBTN_VISIBLED		= 0x00100000,
	ETIS_FUNCBTN_CHECKED		= 0x00200000,
	ETIS_FUNCBTN_ENABLE			= 0x00400000,
	ETIS_FUNCBTN_MASK			= 0x00F00000,

	ETIS_BUTTON_MASK			= 0x00FF0000,

	ETIS_STATE_ICON_1			= 0x01000000,
	ETIS_STATE_ICON_2			= 0x02000000,
	ETIS_STATE_ICON_3			= 0x04000000,
	ETIS_STATE_ICON_4			= 0x08000000,

	ETIS_STATE_ICON_MASK		= 0x0F000000,
};

typedef struct ELC_THUMBNAIL_ITEM {
	CString			strName;
	CString			strTitle;

	CRect			rect;
	CRect			rcFuncBtn;
	CRect			rcCheckBtn;
	CRect			rcTitle;
	CRect			rcThumb;
	CRect			rcName;

	DWORD			dwState;
	DWORD			dwImageIndex; // status image index, each image index 8-bit (0-255)
	DWORD_PTR		dwData;

	LPBYTE			lpThumbnail; // if nThumbnailIndex == -1, show this image
	int				nThumbnailBits;

	int				nThumbnailIndex; // if nThumbnailIndex != -1, show this image

	ELC_THUMBNAIL_ITEM& operator= (ELC_THUMBNAIL_ITEM& item) {
		strName = item.strName;
		strTitle = item.strTitle;

		rect = item.rect;
		rcFuncBtn = item.rcFuncBtn;
		rcCheckBtn = item.rcCheckBtn;
		rcTitle = item.rcTitle;
		rcThumb = item.rcThumb;
		rcName = item.rcName;

		dwState = item.dwState;
		dwImageIndex = item.dwImageIndex;
		dwData = item.dwData;

		lpThumbnail = item.lpThumbnail;
		nThumbnailBits = item.nThumbnailBits;

		nThumbnailIndex = item.nThumbnailIndex;

		return *this;
	}

} * PELC_THUMBNAIL_ITEM;

enum ELC_THUMBNAIL_LIST_NOTIFY_TYPE {
	ETCN_SEL_BTN_CHECKED = 1,
	ETCN_FUNC_BTN_CLICKED,
	ETCN_ITEM_DBLCLK,
	ETCN_BEGINLABELEDIT,
	ETCN_ENDLABELEDIT,
	ETCN_SELECT_CHANGED,
};

typedef struct ELC_THUMBNAIL_LIST_NOTIFY {
	HWND						hwndFrom; // thumblist's window handle
	UINT						nCtrlId; // thumblist's id
	int							nItem;
	ELC_THUMBNAIL_ITEM			item;
} * LPELC_THUMBNAIL_LIST_NOTIFY;

enum ELC_HIT_TYPE {
	EHT_NONE = 0,
	EHT_FUNCBTN,
	EHT_SELECT_BTN,
	EHT_LABEL,
	EHT_ITEM,
};

enum ELC_THUMBNAIL_ITEM_MODIFY_TYPE {
	ETIMT_NONE = 0,
	ETIMT_HITTING,
	ETIMT_SELECTING,
	ETIMT_MOVEING,
	ETIMT_EDITLABEL,
};

enum ELC_THUMBNAIL_ITEM_NI {
	ETINI_ALL			= 0x0000,
	ETINI_FOCUSED		= 0x0001,
	ETINI_SELECTED		= 0x0002,
	ETINI_CHECKED		= 0x0004,
	ETINI_FUNC_CHECKED	= 0x0008,
};

typedef std::vector<PELC_THUMBNAIL_ITEM>			CThumbnailItemArray;
typedef std::map<DWORD_PTR, PELC_THUMBNAIL_ITEM>	CThumbnailItemMap;



enum ELC_THUMBLIST_EDIT_NOTIFY_TYPE {
	ETENT_OK,
	ETENT_CANCEL,
};


///////////////////////////////////////////////////
// CThumbListCtrlEdit
class LIBELCBASEUI_API CThumbListCtrlEdit : public CEdit
{
	DECLARE_DYNAMIC(CThumbListCtrlEdit)

public:
	CThumbListCtrlEdit();
	virtual ~CThumbListCtrlEdit();

public:
	void SetNotifyWindow(HWND hWnd, UINT nMsg) {
		m_hNotifyWnd = hWnd;
		m_nNotifyMsg = nMsg;
	}

	CSize GetIdeaSize();

	int				m_nItem;
	DWORD_PTR		m_dwData;

// Attribute
protected:
	HWND			m_hNotifyWnd;
	UINT			m_nNotifyMsg;
	UINT			m_nNotifyType;
	CFont			m_ftText;

protected:
	void AutoFitSize();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnChange();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};


///////////////////////////////////////////////////
// CThumbListCtrl
class LIBELCBASEUI_API CThumbListCtrl : public CWnd
{
	DECLARE_DYNAMIC(CThumbListCtrl)

public:
	CThumbListCtrl();
	virtual ~CThumbListCtrl();

	void SetNotifyWindow(HWND hWnd, UINT nMsg) {
		m_hNotifyWnd = hWnd;
		m_nNotifyMsg = nMsg;
	}

	// notice : here is setting thumbnail's size
	// the item's size is large than thumbnail's because of its border
	void SetThumbSize(CSize size);
	CSize GetThumbSize();
	CSize GetItemSize();

	int InsertItem(
		int nItem, 
		LPCTSTR lpszItem, 
		LPBYTE lpThumbnail, 
		int nThumbnailBits,
		POINT ptPosition = CPoint(-1,-1),
		DWORD dwState = 0);
	void DeleteItem(int nItem);
	void DeleteAllItems();
	int GetItemCount() {
		return (int)m_listItem.size();
	}

	BOOL SetThumbnailBackground(UINT nResId, LPCTSTR lpszType=RT_PNG);
	void SetThumbnailImageList(UINT nResId, LPCTSTR lpszType, int cxUnit);
	void SetItemThumbnail(int nItem, LPBYTE lpThumbnail, int nThumbnailBits);
	void SetItemThumbnail(int nItem, int nIndex);

	// item's position
	void SetItemPosition(int nItem, POINT pt);
	BOOL GetItemPosition(int nItem, LPPOINT lpPoint);

	BOOL SetItemData(int nItem, DWORD_PTR dwData);
	DWORD_PTR GetItemData(int nItem);

	// item's status icon
	BOOL SetStatusImageListRes(UINT nResId, LPCTSTR lpszType, int cxUnit);
	// dwStatusIndex : ETIS_STATE_ICON_X (x: 0 ~ 4)
	void SetItemStatusImage(int nItem, DWORD dwStatusIndex, int nImage, BOOL bRedraw=TRUE);
	DWORD GetItemStatusIcon(int nItem);
	void SetAllItemsStatusIcons(DWORD dwStatusIndex, int nImage, BOOL bRedraw=TRUE);

	// select button's properties
	BOOL SetCheckButton(UINT nResId, LPCTSTR lpszType=RT_PNG);
	void ShowCheckButton(BOOL bShow);
	BOOL IsCheckButtonVisible();
	void EnableCheckButton(BOOL bEnabled);
	BOOL IsCheckButtonEnabled();

	void SetItemCheck(int nItem, BOOL bChecked, BOOL bRedraw=TRUE);
	BOOL IsItemChecked(int nItem);
	void CheckAllItems(BOOL bChecked);
	int GetCheckedItemCount();

	// function button's properties
	BOOL SetFuncButton(UINT nResId, LPCTSTR lpszType=RT_PNG);
	void ShowFuncButton(BOOL bShow);
	BOOL IsFuncButtonVisible();
	void EnableFuncButton(BOOL bEnabled);
	BOOL IsFuncButtonEnabled();

	void SetItemFuncBtnCheck(int nItem, BOOL bChecked, BOOL bRedraw=TRUE);
	BOOL IsItemFuncBtnChecked(int nItem);
	void CheckAllItemsFuncBtn(BOOL bChecked);
	int GetFuncBtnCheckedItemCount();

	// item's text
	void SetItemText(int nItem, LPCTSTR lpszText, BOOL bRedraw=TRUE);
	CString GetItemText(int nItem);

	void SetItemTitle(int nItem, LPCTSTR lpszText, BOOL bRedraw=TRUE);
	CString GetItemTitle(int nItem);

	// item's state
	// see ELC_THUMB_ITEM_STATE for detail
	void SetItemState(int nItem, DWORD dwState, DWORD dwMask, BOOL bRedraw=TRUE);
	DWORD GetItemState(int nItem, DWORD dwMask);
	void SetAllItemsState(DWORD dwState, DWORD dwMask, BOOL bRedraw=TRUE);

	// force update item
	void UpdateItem(int nItem);
	void UpdateItemThumbnail(int nItem);
	void UpdateItemThumbnail(int nItem, LPRECT lpRect);
	void UpdateAllItem();

	// sort
	void AutoArrange();
	void AlignSelItems(BOOL bHorizontal);
	void AlignToGrid();
	void AlignSelItemToGrid();

	BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData);

	void EnableDragging(BOOL bEnabled) {
		m_bEnabeDragging = bEnabled;
	}
	BOOL IsDragEnabled() {
		return m_bEnabeDragging;
	}

	// 
	int GetFocusItem();

	// nFlags : see ELC_THUMBNAIL_ITEM_NI
	int GetNextItem(int nItem, int nFlags);
	int GetFirstSelectItem();
	int GetNextSelectedItem(int nItem);
	int GetSelectedCount();

	void SetItemSpacing(int nHorzSpacing, int nVertSpacing);
	void GetItemSpacing(int* pnHorzSpacing, int* pnVertSpacing);

	BOOL EnsureVisible(int nItem);
	BOOL IsItemVisible(int nItem);


	// list properties
	void SetBackgndColor(COLORREF color);
	COLORREF GetBackgndColor();
	void SetItemBodyColor(COLORREF color);
	COLORREF GetItemBodyColor();
	void SetTextColor(COLORREF color);
	COLORREF GetTextColor();
	
	void SetTextFont(CFont *pFont);
	CFont* GetTextFont();

	// load logo from resource id
	void SetLogoImage(UINT nId, LPCTSTR lpszType=RT_PNG);
	// load logo from skin config file
	void SetLogoImage(LPCTSTR lpszStyle);

	// edit label
	void EnableEditLabel(BOOL bEnable);
	CEdit* EditLabel(int nItem);
	CEdit* GetEditControl();

protected:
	void DrawItems(CDC * pDC);
	void DrawButtons(LPVOID lpGraphics, PELC_THUMBNAIL_ITEM pItem);
	void DrawButton(LPVOID lpGraphics, LPVOID lpImage, int nIndex, CRect rcBtn);
	void DrawThumbnail(LPVOID lpGraphics, PELC_THUMBNAIL_ITEM pItem);
	void DrawStatusIcons(LPVOID lpGraphics, PELC_THUMBNAIL_ITEM pItem);
	void DrawMovingItems(LPVOID lpGraphics, CThumbnailItemMap *pmapItem);

	void CreateItemFrameBitmap();

	void CalcItemSize(PELC_THUMBNAIL_ITEM pItem, CRect rect);
	void ResizeItems();

	PELC_THUMBNAIL_ITEM FindItem(int nItem);
	int FindItem(PELC_THUMBNAIL_ITEM pItem);

	CSize GetMaxSize();
	void ScrollBarPos(int nBar, int nPos);
	void ResetScrollInfo();

	void MouseDown(UINT nFlags, CPoint point);
	void MouseUp(UINT nFlags, CPoint point);

	BOOL GetHitTest(CPoint point, int& nItem, int& nHitType);
	void CalcSelItems(CRect rect);
	void ClearState(DWORD dwState);

	void ClickButton(int nItem, DWORD dwType, DWORD dwState);

	void GetSelectedItems(int *pItem, int &nCount);

	void AddSelItem(PELC_THUMBNAIL_ITEM pItem);
	void RemoveSelItem(PELC_THUMBNAIL_ITEM pItem);
	void ClearSelItem(CRgn& rgnPaint, PELC_THUMBNAIL_ITEM pExceptItem=NULL);

	void SetFocusItem(PELC_THUMBNAIL_ITEM pItem);

	void PostSelectChangedNotify();

	// make item show at topmost.
	void MoveItemToTail(int nItem);

	void OffsetSelItems(CPoint ptOffset, CRgn& rgnPaint, CPoint *lpLeftTopmost, CPoint *lpRightBottommost);
	void OffsetItem(PELC_THUMBNAIL_ITEM pItem, CPoint ptOffset);
	void OffsetRects(CRect * pRects, int nCount, CPoint ptOffset);
	void OffsetAllItems(CPoint ptOffset);

	CPoint FindIdealInsertPosition();

	void CreatePaintRegion(CRgn &rgn);

	void GetSelItemsRect();

	void RepaintRect(LPRECT lpRect);
	void RepaintRgn(CRgn *rgn);
	
	void SelectNext(UINT nFlag);
	int FindLeftItem(PELC_THUMBNAIL_ITEM pItem);
	int FindRightItem(PELC_THUMBNAIL_ITEM pItem);
	int FindAboveItem(PELC_THUMBNAIL_ITEM pItem);
	int FindBelowItem(PELC_THUMBNAIL_ITEM pItem);

	BOOL Sort(PELC_THUMBNAIL_ITEM *pItems, int nCount, PFNLVCOMPARE pfnCompare, DWORD_PTR dwData);

	LRESULT MouseNotify(ELC_MOUSE_NOTIFY_TYPE type, UINT nFlags);

protected:
	CThumbnailItemArray			m_listItem;
	CThumbnailItemMap			m_mapSelItem;
	CSize						m_sizeItem;
	CSize						m_sizeThumb;

	int							m_nHorzSpacing;
	int							m_nVertSpacing;
	int							m_nBorderWidth;
	int							m_nTitleHeight;
	int							m_nLabelHeight;

	HWND						m_hNotifyWnd;
	UINT						m_nNotifyMsg;

	LPVOID						m_pbmItemFrame;
	LPVOID						m_pCheckBtn;
	LPVOID						m_pFuncBtn;
	LPVOID						m_pStatusIcons;
	
	int							m_cxStatusIcon; // unit cx
	int							m_cyStatusIcon; // unit cy

	Image*						m_pImageBackgnd;
	LPVOID						m_pImageList;
	int							m_cxImageUnit;

	DWORD						m_dwButtonState;

	CRect						m_rcClient;

	CFont						m_ftText;
	COLORREF					m_crSelect;
	COLORREF					m_crText;
	COLORREF					m_crItemBk;
	COLORREF					m_crItemFrameBorder;
	COLORREF					m_crItemThumbBorder;
	COLORREF					m_crCtrlBk;
	COLORREF					m_crFocus;

	//ELC_THUMBNAIL_ITEM_MODIFY_TYPE
	int							m_modifytype;
	CPoint						m_ptDownOld;
	CPoint						m_ptMoving;
	BOOL						m_bEnabeDragging;

	CRect *						m_prcItems;
	int							m_nrcItemsBuffer;
	int							m_nrcSelItems;

	PELC_THUMBNAIL_ITEM			m_pFocusItem;

	Image*						m_pLogoImage;

	CThumbListCtrlEdit			m_wndEdit;
	BOOL						m_bEnableEditLabel;

	BOOL						m_bMirror;

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnEditLabelNotify(WPARAM wParam, LPARAM lParam);
};


