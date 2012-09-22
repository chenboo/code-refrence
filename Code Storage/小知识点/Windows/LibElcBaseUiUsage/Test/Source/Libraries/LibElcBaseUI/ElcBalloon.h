#pragma once

#include "BarButton.h"
#include <vector>

// CElcBalloon

static HWND g_hLastWnd = NULL;

class CElcBalloon;

enum {
	ELC_POPUP_LEFT = 0x01,
	ELC_POPUP_RIGHT,
	ELC_POPUP_CENTER
};

enum ELC_BALLOON_BASE_ID {
	ELC_BALLOON_CHECKBOX = 0x10000000,
};

typedef struct ELC_BALLOON_CALLBACK {
	CElcBalloon *	pThis;
	UINT			nTrackId;
	DWORD			dwUser;
} *PELC_BALLOON_CALLBACK;

typedef void (*PMOUSECLICKED)(PELC_BALLOON_CALLBACK pCallback);

typedef struct tagBALLOON_INFO {
	UINT				nId; // Balloon's id

	CString				strCaption;
	CString				strTitle;
	int					nTimeout;

	BOOL				bShowCaptonIcon;
	UINT				nTitleIcon;
	ELC_RESOURCE_TYPE	titleIconType; // ELC_RESOURCE_TYPE, define in "ElcUICommon.h"

	CString				strCheckBox; // if strCheckBox is not empty, the balloon will show check box
	BOOL				bChecked;

	PMOUSECLICKED		pMouseClicked;
	DWORD				dwUser;

public:
	tagBALLOON_INFO() {
		bShowCaptonIcon = TRUE;
		nTitleIcon = 0;
		titleIconType = ELCRT_PNG;

		bChecked = FALSE;
		nTimeout = 5000;
		pMouseClicked = NULL;
		dwUser = 0;
	}

	tagBALLOON_INFO& operator= (tagBALLOON_INFO& balloon) {
		strCaption = balloon.strCaption;
		strTitle = balloon.strTitle;

		bShowCaptonIcon = balloon.bShowCaptonIcon;
		nTitleIcon = balloon.nTitleIcon;
		titleIconType = balloon.titleIconType;

		strCheckBox = balloon.strCheckBox;
		bChecked = balloon.bChecked;

		nTimeout = balloon.nTimeout;
		pMouseClicked = balloon.pMouseClicked;
		dwUser = balloon.dwUser;

		return *this;
	}

}BALLOON_INFO, *PBALLOON_INFO;


class LIBELCBASEUI_API CElcBalloon : public CWnd
{
	DECLARE_DYNAMIC(CElcBalloon)

public:
	typedef struct TRACKITEM {
		UINT		nID;
		CString		strText;
		CRect		rect;
		BOOL		bHot;
	}*PTRACKITEM;

	enum TRACKITEM_ALIGN {
		TRACKITEM_ALIGN_LEFT = 1,
		TRACKITEM_ALIGN_CENTER,
		TRACKITEM_ALIGN_RIGHT
	};

	typedef std::vector<TRACKITEM> CTextArray;

public:
	BOOL LoadSkin(LPCTSTR lpszStyle);

	static CElcBalloon* CreateBalloon(BALLOON_INFO& balloon);
	BOOL Popup(CWnd* pParent = NULL, int nPopupPos = ELC_POPUP_RIGHT);

	// get balloon's id
	UINT GetId();

	// set text's under line
	void ShowTrackFlag(BOOL bShow);

	void AddTrackItem(UINT nID, LPCTSTR lpszText);
	void SetTrackItemAlign(TRACKITEM_ALIGN align);

	void SetTitleFont(CFont* pFont);
	void SetDescrFont(CFont* pFont);

	// need : bShowCheckBox == TRUE
	void SetCheck(BOOL bCheck);
	BOOL GetCheck();

protected:
	void ShowBalloon(BOOL bShow);
	void RecalcLayout();
	void DrawBalloon(CDC* pDC);
	void DestroyMe();
	
protected:
	BALLOON_INFO			m_ballInfo;
	ELC_SKIN_BALLOON		m_skin;
	CElcButton				m_btnClose;
	CElcButton				m_btnCheck;
	Bitmap *				m_pTitleIcon;

	CElcMemDC				m_dcMemBk;

	CTextArray				m_arrItem;
	int						m_nTrackItemAlign;
	int						m_nHotItem;

	CRgn					m_rgnWindow;

	CFont					m_ftCaption;
	CFont 					m_ftTitle;
	CFont 					m_ftItem;
	CFont 					m_ftHotItem;

	CRect					m_rcIcon;
	CRect					m_rcCaption;
	CRect					m_rcClose;
	CRect 					m_rcTitle;
	CRect					m_rcTitleIcon;
	CRect 					m_rcWindow;
	CRect					m_rcCheckBox;

	BOOL 					m_bShowBall;
	BOOL 					m_bLButtonDown;
	BOOL 					m_bMouseIn;
	BOOL 					m_bBlock;
	BOOL					m_bMirror;
	BOOL					m_bShowTrack;

	int 					m_nFactHeight;
	int						m_nTimeroutCount;

private:
	CElcBalloon(BALLOON_INFO balloon);
	~CElcBalloon();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnCheckBox();
	afx_msg void OnClickClose();
	afx_msg LRESULT OnElcUIDrawNotify(WPARAM wParam, LPARAM lParam);
};