/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////



#pragma once 

#include "ElcNonWndButton.h"
#include "ElcNonWndlabel.h"
#include <vector>

typedef struct ELC_NW_ITEM {
	int				nType;
	CElcNWControl*	pItem;

	POINT			ptTopLeft;
} *PELC_NW_ITEM;

typedef std::vector<ELC_NW_ITEM>	CNWItemArray;

class LIBELCBASEUI_API CNWCtrlContainer
{
public:
	CNWCtrlContainer();
	~CNWCtrlContainer();

	void Initialize(HWND hParent, HWND hNotify);

	void MouseDown(CPoint point);
	void MouseUp(CPoint point);
	void MouseMove(CPoint point);
	void CaptureChanged();

	void Move(CRect rect);

	void Draw(PVOID pvGraphics);
	void Draw(HDC hDC, CRect rect);

public:
	void AddButton(
		UINT nId, 
		UINT nResId, 
		LPCTSTR lpszType, 
		LPCTSTR lpszText, 
		CRect rect, 
		int nCount, 
		BOOL bVertical);
	void AddButton(
		UINT nId, 
		UINT nResId, 
		LPCTSTR lpszType, 
		LPCTSTR lpszText, 
		CPoint point,
		CSize size,
		int nCount, 
		BOOL bVertical);

	void AddButton(
		UINT nId,
		PELC_SKIN_IMAGE pIcon, 
		CPoint ptTopLeft,
		HWND hNotifyWnd,
		UINT nNotifyMsg);

	void AddButton(
		UINT nId,
		LPCTSTR lpBtnBkStyle, 
		CPoint ptTopLeft,
		HWND hNotifyWnd,
		UINT nNotifyMsg);

	void AddTextButton(
		UINT nId, 
		LPCTSTR lpszText,
		LPCTSTR lpBtnBkStyle,
		CPoint ptTopLeft);
	void AddTextButton(
		UINT nId, 
		LPCTSTR lpszText,
		LPCTSTR lpBtnBkStyle,
		CPoint ptTopLeft,
		HWND hNotifyWnd,
		UINT nNotifyMsg);

	void EnableButton(UINT nId, BOOL bEnable);
	BOOL IsButtonEnable(UINT nId);

	void CheckButton(UINT nId, BOOL bChecked);
	BOOL ISButtonChecked(UINT nId);

	void SetButtonImage(UINT nId, UINT nResId, LPCTSTR lpszType, int nCount, BOOL bVertical);

	void AddLabel(
		UINT nId, 
		LPCTSTR lpszText,
		CRect rect,
		int align = ENWLA_CENTER,
		COLORREF crText = RGB(0,0,0));
	void SetLabel(UINT nId, LPCTSTR lpszText);

protected:
	PELC_NW_ITEM FindItem(UINT nId);

	CSize GetTextSize(LPCTSTR lpszText, CFont* pFont);

protected:
	HWND			m_hParent;
	HWND			m_hNotify;
	CNWItemArray	m_arrItem;
	CRect			m_rect;
};



