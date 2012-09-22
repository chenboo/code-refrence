#pragma once

#include <afxtempl.h>

// CRadioListBox

typedef struct RADIOLIST_ITEM
{
	RADIOLIST_ITEM()
	{
		nItem = -1;
		nHeight = 0;
		nItemImage = -1;
		nSubItemImage = -1;
		dwItemData = 0;
//		rcItem.SetRectEmpty();
		bChecked = FALSE;
	}

	int			nItem;

	CString		strItem;
	int			nItemImage;

	CString		strSubItem;
	int			nSubItemImage;

	int			nHeight;
	DWORD		dwItemData;
//	CRect		rcItem;
	BOOL		bChecked;
}RADIOLIST_ITEM, *PRADIOLIST_ITEM;

#define RLS_HAS_RADIO		0x00000001
#define RLS_HAS_GRID		0x00000002
#define RLS_MULTILINE		0x00000004

class LIBELCBASEUI_API CRadioListBox : public CWnd
{
	DECLARE_DYNAMIC(CRadioListBox)

public:
	CRadioListBox();
	virtual ~CRadioListBox();

public:
	BOOL Create(CRect rc, CWnd* pWndParent, UINT nID, DWORD dwStyle, DWORD dwStyleRLS);

	void SetNotifyMsg(HWND hNotifyWnd, DWORD dwNotifyMsg);

	CImageList* SetImageList(CImageList* pImageList);
	CImageList* GetImageList();

	int AddItem(LPCTSTR lpszItem, int nItemImage, LPCTSTR lpszSubItem = NULL, int nSubItemImage = -1);
	BOOL DeleteItem(int nItem);

	BOOL SetItemText(int nItem, int nSubItem, CString strText);
	CString GetItemText(int nItem, int nSubItem);

	BOOL SetItemImage(int nItem, int nSubItem, int nImage);
	int GetItemImage(int nItem, int nSubItem);

	BOOL SetItemData(int nItem, DWORD dwData);
	DWORD GetItemData(int nItem);

	BOOL SetCheck(int nItem, BOOL bCheck);
	BOOL GetCheck(int nItem);
	int GetCheckedItem();

	void SetFont(CFont* pFont);
	void SetRadioIcon(HICON hCheck, HICON hUnCheck);

	void SetTextColor(COLORREF cr);
	COLORREF GetTextColor();

	void SetBkColor(COLORREF cr);
	COLORREF GetBkColor();

	virtual void DrawItem(CDC* pDC, RECT* pRect, PRADIOLIST_ITEM pItem);

	int GetListHeight();

	void SetDrawFocusRect(BOOL bOn);

	int GetItemCount();

protected:
	PRADIOLIST_ITEM GetItem(int nItem);
	int CalculateItemHeight(CString strItemText, CString strSubItemText);
	int CalculateListHeight();
	void ResetIndex();

	int HitTest(CPoint point);

	void SetAllItemCheck(BOOL bCheck);
	void UpdateScrollInfo();

protected:
	DECLARE_MESSAGE_MAP()

private:
	DWORD m_dwStyleRLS;
	HWND m_hNotifyWnd;
	DWORD m_dwNotifyMsg;


	CImageList* m_pImgList;
	CSize	m_szImage;

	COLORREF	m_crText;
	COLORREF	m_crBk;

	DWORD m_dwWidth;
	DWORD m_dwHeight;

	UINT m_nControlID;
	DWORD m_nListHeight;

	CFont m_ftText;

	CList<PRADIOLIST_ITEM, PRADIOLIST_ITEM> m_lstItem;

	HICON m_hUnCheck;
	HICON m_hCheck;

	BOOL m_bDrawFocusRect;

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


