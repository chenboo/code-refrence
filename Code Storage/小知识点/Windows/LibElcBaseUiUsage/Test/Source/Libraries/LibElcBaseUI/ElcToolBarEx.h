/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////



#pragma once

#include <AFXPRIV.H>
#include "ElcBaseBar.h"

class LIBELCBASEUI_API CElcToolBarEx : public CElcBaseBar
{
	DECLARE_DYNAMIC(CElcToolBarEx)

public:
	enum ELCTB_ITEM_TYPE {
		ELCIT_BUTTON,
		ELCIT_LABEL,
		ELCIT_UNION_BUTTON,
	};
	typedef struct ELCTB_ITEM {
		UINT			nID;
		int				type;
		CWnd*			pItem;
		DWORD_PTR		dwData;

		ELCTB_ITEM() {
			nID = 0;
			type = ELCIT_BUTTON;
			pItem = NULL;
			dwData = 0;
		}
	}*PELCTB_ITEM;

	typedef vector<ELCTB_ITEM> CETBItemArray;

public:
	CElcToolBarEx();
	virtual ~CElcToolBarEx();

	// create a dock bar(top-level), this style bar is used only for Mainframe.
	// if u wanna create a bar in others' window, pls call the following create function.
	BOOL CreateMainBar(CWnd* pParent, int nHeight, UINT nID = AFX_IDW_TOOLBAR);

	// create a general toolbar in a child window.
	virtual BOOL Create(
				CWnd* pParent, 
				const RECT& rect,
				DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
				UINT nID = 0 );

	// default : the button's clicked command is send to the bar's parent window
	// also you can specify a received window. 
	void SetCmdDrainWindow(HWND hWnd);

	BOOL AddButton(UINT nID, 
		UINT nResId, 
		LPCTSTR lpszType=RT_PNG, 
		LPCTSTR lpszText=NULL, 
		int nDropType = ELCBDT_NORMAL,
		UINT nAlign=ELCBAR_ALIGN_LEFT);

	BOOL AddButton(UINT nID, 
		LPCTSTR lpszText,
		UINT nResId, 
		LPCTSTR lpszType=RT_PNG, 
		int nDropType = ELCBDT_NORMAL,
		UINT nAlign=ELCBAR_ALIGN_LEFT);

	BOOL AddFourStateButton(UINT nID, 
		UINT nResId, 
		LPCTSTR lpszType=RT_PNG, 
		LPCTSTR lpszText=NULL, 
		int nDropType = ELCBDT_NORMAL,
		UINT nAlign=ELCBAR_ALIGN_LEFT);

	BOOL AddUnionButton(UINT nLeftId, 
		UINT nLeftResId,
		LPCTSTR lpszLeftText,
		UINT nRightId, 
		UINT nRightResId,
		LPCTSTR lpszRightText,
		UINT nAlign=ELCBAR_ALIGN_LEFT);

	BOOL DelButton(UINT nID);
	CBarButtonEx* GetButton(UINT nID);

	BOOL EnableButton(UINT nID, BOOL bEnable);
	BOOL IsButtonEnable(UINT nID);

	BOOL SetButtonTip(UINT nID, UINT nTipResId);
	BOOL SetButtonTip(UINT nID, LPCTSTR lpszText);

	BOOL SetButtonText(UINT nID, UINT nResId);
	BOOL SetButtonText(UINT nID, LPCTSTR lpszText);

	// see BTN_DROP_TYPE in the class CBarButtonEx
	// notice : this function will change the button's size(horizontal & vertical), 
	// make sure the bar's size is enough.
	BOOL SetButtonDropType(UINT nID, int nDropType);

	// set all buttons' layout type
	// see BTN_IMG_LAYOUT in the class CBarButtonEx
	// notice : this function will change the button's size(horizontal & vertical), 
	// make sure the bar's size is enough.
	// !!! CALL this function BEFORE add buttons best, if possible.
	BOOL SetButtonsLayout(int layout);

	BOOL SetButtonChecked(UINT nID, BOOL bChecked);
	BOOL IsButtonChecked(UINT nID);

	BOOL SetButtonImage(UINT nID, UINT nResId, LPCTSTR lpszType=RT_PNG, ELC_BUTTON_ICON_TYPE mode=ELCBIM_SINGLE);

	BOOL SetButtonVisible(UINT nID, BOOL bVisible);

	// label
	// note: the id must be not duplicate with others buttons and lables
	// if bAdaptation == 0, you need set label's size.
	BOOL AddLabel(UINT nID, LPCTSTR lpszText, UINT nAlign=ELCBAR_ALIGN_LEFT, BOOL bAdaptation=TRUE, CSize size=CSize(0, 0));
	BOOL AddLabel(UINT nID, LPCTSTR lpszText, COLORREF crText, UINT nAlign, BOOL bAdaptation, CSize size);
	BOOL SetLabelText(UINT nID, LPCTSTR lpszText);
	BOOL SetLabelColor(UINT nID, COLORREF crText);

protected:
	int FindButton(UINT nID);
	CBarButtonEx * FindUnionButton(UINT nID);

protected:
	CETBItemArray	m_arrTBItem;
	CETBItemArray	m_arrTBUnionBtnItem;
	HWND			m_hCmdDrainWindow;
	int				m_nButtonLayout;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnButtonClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLabelClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnIdleUpDateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
};


