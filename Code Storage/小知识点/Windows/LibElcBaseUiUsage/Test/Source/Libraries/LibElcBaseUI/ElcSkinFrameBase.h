#pragma once

class CElcButton;
class CElcStatic;

#define RESIZE_BRODER		4
#define MENU_ITEM_HMARGIN	6
#define MENU_ITEM_VMARGIN	3
#define FRAME_ICON_MARGIN	4
#define FRAME_ICON_SIZE		16



enum ELC_SKINFRAME_WND_TYPE {
	ELCSWT_FRAMEWND = 0,
	ELCSWT_DIALOG,
	ELCSWT_WND,
};

enum ELC_SKINFRAME_NWBUTTON_TYPE {
	ESNWT_NONE = 0,
	ESNWT_MINIMIZE = HTMINBUTTON,
	ESNWT_MAXIMIZE = HTMAXBUTTON,
	ESNWT_CLOSE = HTCLOSE,

	ESNWT_CUSTOM_BASE = 30,
};

typedef struct ELC_SKINFRAME_NWBUTTON {
	UINT						type; // ELC_SKINFRAME_NWBUTTON_TYPE
	PELC_SKIN_NOTWND_BUTTON		pButton;
	BOOL						bEnable;
} *PELC_SKINFRAME_NWBUTTON;

typedef struct ELC_SKINFRAME_MENUITEM {
	CString				strItem;
	CRect				rect;
} *PELC_SKINFRAME_MENUITEM;

typedef struct ELC_SKINFRAME_CLIENT_IMAGE_POS {
	UINT				nId;
	ELC_SKIN_POSITION	pos;
} *PELC_SKINFRAME_CLIENT_IMAGE_POS;

typedef std::vector<ELC_SKINFRAME_MENUITEM>				CMenuItemArray;
typedef std::vector<ELC_SKINFRAME_NWBUTTON>				CFrameButtonArray;
typedef std::vector<ELC_SKINFRAME_CLIENT_IMAGE_POS>		CClientImagePosArray;
typedef std::vector<CWnd *>								CSkinCtrlArray;


///////////////////////////////////////////
// CElcSkinFrameBase
class LIBELCBASEUI_API CElcSkinFrameBase
{
public:
	CElcSkinFrameBase();
	virtual ~CElcSkinFrameBase();

public:
	virtual BOOL LoadSkin(LPCTSTR lpStyle);

	BOOL Initialize(HWND hThis);
	void Uninitialize();

	BOOL AddCustomButton(UINT nId, UINT nResId, LPCTSTR lpszType=RT_PNG);
	void DelCustomButton(UINT nId);
	void DelAllCustomButton();
	void EnableCustomButton(UINT nId, BOOL bEnable);
	BOOL IsCustomButtonEnable(UINT nId);
	CRect GetCustomButtonRect(UINT nId);

	// call this function to refresh the ui when the sysmenu was changed.
	void RefreshFrameStatus();

	void EnableDrawClientBackgnd(BOOL bEnable);

	LRESULT NcHitTest(CPoint point);
	void NcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	void NcPaint();
	BOOL NcLButtonDblClk(UINT nHitTest, CPoint point);
	BOOL NcLButtonDown(UINT nHitTest, CPoint point);
	BOOL NcLButtonUp(UINT nHitTest, CPoint point);
	BOOL NcMouseMove(UINT nHitTest, CPoint point);
	BOOL NcRButtonDown(UINT nHitTest, CPoint point);
	BOOL NcActivate(BOOL bActive);
	HBRUSH CtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void SetWindowRegion();

	void DrawClientBackground(CDC* pDC);

	void EnterIdle(UINT nWhy, CWnd* pWho);
	void CheckStates();

	BOOL AdjustImagePanePos(UINT nID);
	BOOL AdjustImagePanePos(UINT* nIDs, int nCount);

	BOOL AddClientBackgndPos(UINT nId, const LPPOINT lpTopLeft, const PSIZE pSize);
	void DelClientBackgndPos(UINT nId);
	void ClearClientBackgndPos();

	int FindClientBackgndPos(UINT nId);
	void RelayoutClientBackgndPos();

	// Bind a button (pushbutton, checkbox or groupbox) to CElcButton
	void BindButton(UINT* nIDs, int nCount, LPCTSTR lpszStyle=ELC_SKIN_STYLE_BUTTON_DLG);
	void BindButton(UINT nID, LPCTSTR lpszStyle=ELC_SKIN_STYLE_BUTTON_DLG);

	// you can also call the BindButton() to bind checkbox or groupbox
	// these two functions just specify the Default Skin Style for checkbox(groupbox)
	// it's not different with BindButton() which specify checkbox(groupbox) Skin Style
	void BindCheckBox(UINT nID, LPCTSTR lpszStyle=ELC_SKIN_STYLE_BUTTON_CHECKBOX);
	void BindGroupBox(UINT nID, LPCTSTR lpszStyle=ELC_SKIN_STYLE_BUTTON_GROUPBOX);

	// Bind a picture ctrl to CElcStatic
	void BindPictureCtrl(UINT nCtrlId, UINT nImageId, LPCTSTR lpszType=RT_PNG, BOOL bRealImageSize=TRUE);

	// Unbind ctrls, do not call these two functions unless you know what you want
	void UnBindCtrl(UINT nID);
	void UnBindCtrls();


	CElcButton* GetSkinButton(UINT nID);
	CElcStatic* GetPictureCtrl(UINT nCtrlId);
	CWnd* GetBindCtrl(UINT nCtrlId);

	COLORREF GetClientBkColor();
	void SetClientBkColor(COLORREF cr);

	COLORREF GetImagePaneColor();

	void SetTitleIcon(UINT nResId, LPCTSTR lpszType=RT_PNG);

	int GetLeftWidth();
	int GetRightWidth();
	int GetCaptionHeight();
	int GetMenuBarHeight();
	int GetBottomHeight();

	PELC_SKIN_WINDOW GetSkinStyle();

	void RedrawMenuBar();
	void RedrawButton(int type); // type : ELC_SKINFRAME_NWBUTTON_TYPE
	void RedrawButton(PELC_SKIN_NOTWND_BUTTON pButton);
	void RedrawCaption();
	void RedrawFrame();

	void EnableSkin(BOOL bEnable) {
		m_bEnableSkin = bEnable;
	}
	BOOL IsEnableSkin() {
		return m_bEnableSkin;
	}

protected:
	void InitButtons();
	void InitMenuItem();

	int FindButton(int type);

	void PopupMenu(int nIndex);
	void PopupSysMenu();
	int PtInMenuItem(CPoint point);

	void DrawBorder(CDC* pDC, HICON hIcon);
	void DrawSkinMenuBar(CDC* pDC);
	void DrawCaptionMin(CDC* pDC, HICON hIcon);

	LRESULT TrackBorderHitTest(CPoint point);

	void PressFrameButton(UINT nHitTest);

	CRect CalcPos(const PELC_SKIN_NOTWND_BUTTON pButton);
	CRect CalcRect(const LPRECT lprcDest, const PPOINT lpTopLeft, const PSIZE pSize);
	BOOL PtInIcon(CPoint point);
	void GetOppositePoint(CPoint &point);
	void GetOppositeRect(CRect &rect);
	void GetScreenPoint(CPoint &point);

	BOOL IsCtrlInPane(UINT nID);
	BOOL IsCtrlInPane(CWnd* pWnd);
	COLORREF GetCtrlBkColor(UINT nID);
	COLORREF GetCtrlBkColor(CWnd* pWnd);

protected:
	HWND					m_hDerive;
	int						m_nWndType;

	ELC_SKIN_WINDOW			m_skin;
	CRect					m_rect;
	HICON					m_hIcon;

	int						m_nLeft;
	int						m_nRight;
	int						m_nCaption;
	int						m_nMenuBar;
	int						m_nBottom;

	CClientImagePosArray	m_arrClientImagePos;

	CMenuItemArray			m_arrMenu;
	int						m_nActiveItem;
	BOOL					m_bMenuActivated;
	BOOL					m_bSelfCancelMenu;
	
	CFrameButtonArray		m_arrButton;
	int						m_nActiveButton;
	int						m_nLBDownItem;

	HMENU					m_hMenu;
	CFont					m_ftMenu;
	CFont					m_ftCaption;

	BOOL					m_bEnableSkin;
	BOOL					m_bDrawClientBackgnd;
	BOOL					m_bHasIcon;
	BOOL					m_bActivated;

	CBrush					m_brBackgnd;
	CBrush					m_brPane;
	CSkinCtrlArray			m_arrSkinCtrl;
};


