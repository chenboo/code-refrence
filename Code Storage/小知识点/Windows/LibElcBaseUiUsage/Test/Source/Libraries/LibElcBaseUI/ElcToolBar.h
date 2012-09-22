#pragma once

#include "ElcBaseBar.h"


class LIBELCBASEUI_API CElcToolBar : public CElcBaseBar
{
	DECLARE_DYNAMIC(CElcToolBar)

public:
	typedef struct ELCTB_ITEM {
		UINT nID;
		CBarButton* pItem;
	}*PELCTB_ITEM;

	typedef vector<ELCTB_ITEM> CETBItemArray;

public:
	CElcToolBar();
	virtual ~CElcToolBar();

	virtual BOOL Create(
				CWnd* pParentWnd, 
				const RECT& rect,
				DWORD dwStyle = WS_CHILD | WS_VISIBLE,
				UINT nID = 0 );

	// default : the button's clicked command is send to the bar's parent window
	// also you can specify a received window. 
	void SetCmdDrainWindow(HWND hWnd);

	BOOL AddButton(UINT nID, UINT nBitmap, UINT nAlign=ELCBAR_ALIGN_LEFT);
	BOOL DelButton(UINT nID);
	CBarButton* GetButton(UINT nID);

	BOOL EnableButton(UINT nID, BOOL bEnable);
	BOOL IsButtonEnable(UINT nID);

	BOOL SetButtonTip(UINT nID, UINT nTipResId);
	BOOL SetButtonTip(UINT nID, LPCTSTR lpszText);

	BOOL SetButtonChecked(UINT nID, BOOL bChecked);
	BOOL IsButtonChecked(UINT nID);

	BOOL SetButtonBitmap(UINT nID, UINT nBitmapResId);

	BOOL SetButtonVisible(UINT nID, BOOL bVisible);

	// label
	// note: the id must be not duplicate with others buttons and lables
	BOOL AddLable(UINT nID, LPCTSTR lpszText, UINT nAlign=ELCBAR_ALIGN_LEFT);
	BOOL AddLable(UINT nID, LPCTSTR lpszText, COLORREF crText, UINT nAlign=ELCBAR_ALIGN_LEFT);
	BOOL SetLableText(UINT nID, LPCTSTR lpszText);
	BOOL SetLableColor(UINT nID, COLORREF crText);

protected:
	int FindButton(UINT nID);

protected:
	CETBItemArray	m_arrTBItem;
	HWND			m_hCmdDrainWindow;

protected:
	DECLARE_MESSAGE_MAP()

	LRESULT OnButtonClicked(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


