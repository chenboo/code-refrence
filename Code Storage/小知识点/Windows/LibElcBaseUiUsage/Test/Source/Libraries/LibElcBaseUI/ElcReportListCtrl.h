#pragma once

#include <vector>
#include "ElcHeaderCtrl.h"

enum ELC_REPORT_LIST_SUBITEM_TYPE 
{
	ERLST_NORMAL,
	ERLST_IMAGE,
	ERLST_PROGRESS,
};

enum ELC_RL_IMAGE_TYPE
{
	ELCRLIT_IMAGELIST, // Use the image of CImageList from CListCtrl::SetImageList()
	ELCRLIT_IMAGELIST_EX, // Use the image of SetImageListEx();
	ELCRLIT_BITMAP_HANDLE,
};

typedef struct ELC_RL_IMAGEITEM 
{
	UINT		nId;
	UINT		nImageType; // ELC_RL_IMAGE_TYPE
	DWORD_PTR	dwImage; // imagelist index if nImageType == ELCRLIT_IMAGELIST; handle if nImageType == ELCRLIT_BITMAP_HANDLE
	DWORD_PTR	dwData;
	CString		strTip;
} *PELC_RL_IMAGEITEM;

typedef std::vector<ELC_RL_IMAGEITEM>	CImageIndexArray;

typedef struct ELC_REPORT_LIST_SUBITEM 
{
	int			nType;
	DWORD_PTR	dwData;
} *PELC_REPORT_LIST_SUBITEM;

typedef struct ELC_SUBITEM_IMAGE : public ELC_REPORT_LIST_SUBITEM
{
	CImageIndexArray arrImage;

	BOOL Add(int nImgId, DWORD_PTR dwImage, int nType, LPCTSTR lpszTip) 
	{
		if (Find(nImgId) != arrImage.end())
			return FALSE;

		ELC_RL_IMAGEITEM item;
		item.nId = nImgId;
		item.nImageType = nType;
		item.dwImage = dwImage;
		item.dwData = 0;
		item.strTip = lpszTip;
		arrImage.push_back(item);
		return TRUE;
	}
	void Delete(int nImgId) 
	{
		CImageIndexArray::iterator it = Find(nImgId);
		if (it != arrImage.end())
			arrImage.erase(it);
	}
	void DeleteAll()
	{
		arrImage.clear();
	}
	CImageIndexArray::iterator Find(int nImgId) 
	{
		for (CImageIndexArray::iterator it = arrImage.begin();
			it != arrImage.end();
			it ++)
		{
			if (it->nId == nImgId)
				return it;
		}
		return arrImage.end();
	} 
} *PELC_SUBITEM_IMAGE;

typedef struct ELC_SUBITEM_PROGRESS : public ELC_REPORT_LIST_SUBITEM
{
	int nPos;
} *PELC_SUBITEM_PROGRESS;

typedef std::vector<PELC_REPORT_LIST_SUBITEM>	CRLSubItemArray;

typedef struct ELC_REPORTLIST_ITEM_INFO {
	BOOL		bBold;
	COLORREF	crText;
} *PELC_REPORTLIST_ITEM_INFO;

typedef struct ELC_REPORTLIST_ITEM_DATA
{
	CRLSubItemArray				arrSubItem;
	ELC_REPORTLIST_ITEM_INFO	iteminfo;
	DWORD_PTR					dwData;
} *PELC_REPORTLIST_ITEM_DATA;

typedef struct ELC_REPORTLIST_HITTESTINFO
{
	int			nItem;
	int			nSubItem;
	int			nImageIndex; // a zero-based value that specifies the image of the subitem in the list
	UINT		nImageId; // return OxFFFFFFFF if point not in image
	DWORD_PTR	dwImage; // imagelist index if nImageType == ELCRLIT_IMAGELIST; handle if nImageType == ELCRLIT_BITMAP_HANDLE
	DWORD_PTR	dwData; // image's data
} *PELC_REPORTLIST_HITTESTINFO;


class LIBELCBASEUI_API CElcReportListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CElcReportListCtrl)

public:
	CElcReportListCtrl();
	virtual ~CElcReportListCtrl();

public:

	BOOL LoadSkin(LPCTSTR lpszStyle);

	int InsertColumn(int nCol, const LVCOLUMN* pColumn);
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);

	BOOL SetColumnWidth(int nCol, int cx);

	int InsertItem(int nItem, LPCTSTR lpszItem);
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);
	int InsertItem(const LVITEM* pItem);
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam);

	BOOL SetItem(const LVITEM* pItem);
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem, int nImage, UINT nState, UINT nStateMask, LPARAM lParam);
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem, int nImage, UINT nState, UINT nStateMask, LPARAM lParam, int nIndent);

	// nImageType : see ELC_RL_IMAGE_TYPE
	BOOL AddImage(int nItem, int nSubItem, UINT nImgId, DWORD_PTR dwImage, int nImageType=ELCRLIT_IMAGELIST, LPCTSTR lpszTip=NULL);
	void DelImage(int nItem, int nSubItem, UINT nImgId);
	void DelAllImages(int nItem, int nSubItem);
	int GetImageCount(int nItem, int nSubItem);

	void SetImage(int nItem, int nSubItem, UINT nImgId, DWORD_PTR dwImage, int nImageType=ELCRLIT_IMAGELIST);
	BOOL GetImage(int nItem, int nSubItem, UINT nImgId, DWORD_PTR& dwImage, int& nImageType);
	void SetImageTip(int nItem, int nSubItem, UINT nImgId, LPCTSTR lpszTip);

	void SetImageData(int nItem, int nSubItem, UINT nImgId, DWORD_PTR dwData);
	DWORD_PTR GetImageData(int nItem, int nSubItem, UINT nImgId);

	// this function is used only for ELCRLIT_BITMAP_HANDLE
	void SetImageTransparentColor(COLORREF color);
	
	// do not add progress ctrl in the first subitem (nSubItem = 0)
	BOOL AddProgress(int nItem, int nSubItem, int nPos);
	void DelProgress(int nItem, int nSubItem);
	BOOL SetProgressPos(int nItem, int nSubItem, int nPos);
	int GetProgressPos(int nItem, int nSubItem);

	int GetSubItemType(int nItem, int nSubItem);

	void SetItemBold(int nItem, BOOL bBold);
	BOOL GetItemBold(int nItem);

	void SetItemTextColor(int nItem, COLORREF crText);
	COLORREF GetItemTextColor(int nItem);

	BOOL SetItemData(int nItem, DWORD_PTR dwData);
	DWORD_PTR GetItemData(int nItem) const;

	void SetImageList(CImageList* pImageList, int nImageListType);
	CImageList* GetImageList();

	// this image list is used for AddImage()
	BOOL SetImageListEx(UINT nResId, LPCTSTR lpszType, int cxUnit, int cyUnit);

	BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData);

	CElcHeaderCtrl* GetHeaderCtrl() {
		return &m_SkinHeaderCtrl;
	}

	void EnableCheckItems(BOOL bEnabled) {
		m_bEnableCheckBtn = bEnabled;
		if (m_hWnd) 
			Invalidate();
	}
	BOOL IsEnableCheckItems() {
		return m_bEnableCheckBtn;
	}

	// point : screen coordinates
	void GetRLHitTest(CPoint point, PELC_REPORTLIST_HITTESTINFO pInfo);

public:
	// call this function before list been created best
	void SetItemHeight(int nHeight);
	int GetItemHeight() {
		return m_nItemHeight;
	}

	void SetListItemTxtColor(COLORREF cr) {
		m_skin.crText = cr;
	}
	COLORREF GetListItemTxtColor() {
		return m_skin.crText;
	}

	void SetSelectBkColor(COLORREF cr) {
		m_skin.crSelectedBackgnd = cr;
	}
	COLORREF GetSelectBkColor() {
		return m_skin.crSelectedBackgnd;
	}
	void SetSelectTextColor(COLORREF cr) {
		m_skin.crTextSelected = cr;
	}
	COLORREF GetSelectTextColor() {
		return m_skin.crTextSelected;
	}

	void SetItemBkColorI(COLORREF crBk) {
		m_skin.crItemA = crBk;
	}
	COLORREF GetItemBkColorI() {
		return 	m_skin.crItemA;
	}
	void SetItemBkColorII(COLORREF crBk) {
		m_skin.crItemB = crBk;
	}
	COLORREF GetItemBkColorII() {
		return 	m_skin.crItemB;
	}

	// Progress bar
	void SetProgressHeight(int nHeight) {
		m_nProgressHeight = nHeight;
	}
	int GetProgressHeight() {
		return m_nProgressHeight;
	}
	void SetProgressBk(COLORREF cr) {
		m_crProgressBk = cr;
	}
	COLORREF GetProgressBk() {
		return 	m_crProgressBk;
	}
	void SetProgressFillColor(COLORREF cr) {
		m_crProgressFill = cr;
	}
	COLORREF GetProgressFillColor() {
		return 	m_crProgressFill;
	}
	void SetProgressBorder(COLORREF cr) {
		m_crProgressBorder = cr;
	}
	COLORREF GetProgressBorder() {
		return m_crProgressBorder;
	}

protected:
	CElcHeaderCtrl			m_SkinHeaderCtrl;
	CImageList *			m_pImageList;

	int						m_nMinColumnWidth;
	int						m_nItemHeight;
	int						m_cxImage;
	int						m_cyImage;
	int						m_nHMargin;
	int						m_nProgressHeight;

	ELC_SKIN_LIST			m_skin;

	COLORREF 				m_crProgressBk;
	COLORREF 				m_crProgressFill;
	COLORREF 				m_crProgressBorder;

	COLORREF 				m_crImageTransparentBk;

	CFont					m_ftText;
	CFont					m_ftTextBold;

	CSize					m_sizeCheckBtn;
	BOOL					m_bEnableCheckBtn;

	Image *					m_pImageListEx;
	int						m_cxUnitImagelistEx;
	int						m_cyUnitImagelistEx;

	PWCHAR					m_pszTip;
	int						m_cchTip;

	BOOL					m_bMirror;
	CRect					m_rcItem;

protected:

	void DrawCheckBtn(CDC *pDC, CRect rect, BOOL bChecked);
	void DrawSubItem(CDC *pDC, CRect rect, PELC_REPORT_LIST_SUBITEM pSubItem, LPCTSTR lpszText);
	int DrawImages(CDC *pDC, CRect rect, PELC_SUBITEM_IMAGE pImagesItem);
	int DrawProgress(CDC *pDC, CRect rect, PELC_SUBITEM_PROGRESS pProgress, LPCTSTR lpszText);

	CRLSubItemArray* GetItemInfo(int nItem);
	BOOL FindSubItem(int nItem, int nSubItem, PELC_REPORTLIST_ITEM_DATA &pItem,	PELC_REPORT_LIST_SUBITEM &pSubItem);
	void DeleteCustomData(int nItem);

	CRect GetSubItemTextDrawRect(CRect rect, PELC_REPORT_LIST_SUBITEM pSubItem, LPCTSTR lpszText, BOOL bBoldFont);
	int GetImageItemWidth(CRect rect, PELC_SUBITEM_IMAGE pImagesItem);
	int GetProgressItemWidth(CRect rect, PELC_SUBITEM_PROGRESS pProgress, LPCTSTR lpszText, BOOL bBoldFont);

	static int CALLBACK InternalSortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3);

	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct );
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};


