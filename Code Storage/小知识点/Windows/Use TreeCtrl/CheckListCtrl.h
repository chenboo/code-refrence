#pragma once

typedef struct CHECKLIST_ITEM
{
	int			nItemImage;
	BOOL		bChecked;
	CString		strItem;
	DWORD		dwItemData;
	CRect		rcItem;

	RADIOLIST_ITEM()
	{
		nItemImage = -1;
		nSubItemImage = -1;
		dwItemData = 0;
		bChecked = FALSE;
		rcItem.SetRectEmpty();
	}

}CHECKLIST_ITEM, *PCHECKLIST_ITEM;


// CCheckListCtrl

class CCheckListCtrl : public CWnd
{
public:
	CCheckListCtrl();
	virtual ~CCheckListCtrl();
	typedef vector<PCHECKLIST_ITEM> CItemArray;

public:
	void SetNotifyHwnd(HWND hWndNotify, UINT nMsg);

	CImageList* GetImageList();
	CImageList* SetImageList(CImageList* pImageList);

	int AddItem(LPCTSTR lpszItem, int nItemImage);
	BOOL DeleteItem(int nItem);

	BOOL SetItemData(int nItem, DWORD dwData);
	DWORD GetItemData(int nItem);

	BOOL SetCheck(int nItem, BOOL bCheck);
	BOOL GetCheck(int nItem);

	void SetFont(CFont* pFont);

	void SetTextColor(COLORREF cr);
	COLORREF GetTextColor();

	void SetBkColor(COLORREF cr);
	COLORREF GetBkColor();

	int GetItemCount();


protected:
	UINT			m_nMsg;
	HWND			m_hWndNotify;

	CFont			m_ftText;
	CItemArray		m_ItemArray;
	COLORREF		m_clrText;
	COLORREF		m_clrBk;


protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CCheckListCtrl)

	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


