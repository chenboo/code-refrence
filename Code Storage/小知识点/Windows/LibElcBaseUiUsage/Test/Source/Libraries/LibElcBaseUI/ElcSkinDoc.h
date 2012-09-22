/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////



#pragma once

#include "../LibTinyXML/tinyxml.h"
#include "i18nUtils.h"
#include "ElcUISnippets.h"
#include <vector>

#define ELC_SKIN_SUPPORT_MAJOR_VERSION		0x0001

typedef struct ELC_SKIN_GLOBAL {
	COLORREF	crMainBackgnd;
	COLORREF	crPaneBackgnd;
} *PELC_SKIN_GLOBAL;

typedef struct ELC_SKIN_FONT {
	TCHAR		szFaceName[LF_FACESIZE];
	int			nSize;
	BYTE		bBold;
	BYTE		bItalic;
	BYTE		bUnderline;
} *PELC_SKIN_FONT;

enum ELC_IMAGE_STRETCH_TYPE {
	EIST_FIXED = 0,

	EISR_HORIZONTAL = 0x0001,
	EISR_VERTICAL = 0x0002,

	EISR_BOTH = EISR_HORIZONTAL | EISR_VERTICAL,
};

typedef struct ELC_SKIN_POSITION {
	POINT			point;
	SIZE			size;
} *PELC_SKIN_POSITION;

typedef struct ELC_SKIN_IMAGE {
	int			nCount; // unit-image count, ex: button's background image has four states
	BOOL		bVertical; // Arranged in the direction of the unit-image 
	
	PVOID		pImage; // the following five variables specify how to resizing the image.
	int			stretch; // see ELC_IMAGE_STRETCH_TYPE
	int			nLeftSplit;
	int			nRightSplit;
	int			nTopSplit;
	int			nBottomSplit;
}*PELC_SKIN_IMAGE;

typedef struct ELC_SKIN_MASK {
	COLORREF			crKey;
	ELC_SKIN_IMAGE		mask;
} *PELC_SKIN_MASK;

typedef struct ELC_SKIN_NOTWND_BUTTON {
	ELC_SKIN_POSITION	pos;
	ELC_SKIN_IMAGE		image;
} *PELC_SKIN_NOTWND_BUTTON;

typedef struct ELC_SKIN_IMAGEPANE {
	ELC_SKIN_IMAGE		image;
	COLORREF			crText;
	COLORREF			crBackgnd;
	BOOL				bFillBackgnd;
} *PELC_SKIN_IMAGEPANE;

typedef struct ELC_SKIN_CLIENT {
	COLORREF				crText;
	COLORREF				crBackgnd;
	ELC_SKIN_IMAGEPANE		pane;
} *PELC_SKIN_CLIENT;

typedef struct ELC_SKIN_PICTURE
{
	TCHAR			szStyle[32];
	COLORREF		crBackgnd;
	ELC_SKIN_IMAGE	image;
} *PELC_SKIN_PICTURE;

typedef struct ELC_SKIN_BUTTON
{
	TCHAR			szStyle[32];

	COLORREF		crTextNormal;
	COLORREF		crTextHot;
	COLORREF		crTextDown;
	COLORREF		crTextDisabled;

	COLORREF		crBackgnd;
	COLORREF		crGroupBorder; // for group box

	ELC_SKIN_IMAGE	background;
	ELC_SKIN_IMAGE	dropdown;

	ELC_SKIN_IMAGE	check; // notice it'll be radio if the style is for radio button
	ELC_SKIN_IMAGE	Indeterminate;

	POINT			ptAdjust;
} *PELC_SKIN_BUTTON;

typedef struct ELC_SKIN_PNAE_NC {
	TCHAR			szStyle[32];
	COLORREF		crBackgnd;
	COLORREF		crClient;
	ELC_SKIN_IMAGE	ncimage;
} *PELC_SKIN_PNAE_NC;

typedef struct ELC_SKIN_PNAE_CLIENT {
	TCHAR			szStyle[32];
	COLORREF		crBackgnd;
	ELC_SKIN_IMAGE	background;
} *PELC_SKIN_PNAE_CLIENT;

typedef struct ELC_SKIN_TOOLBAR {
	TCHAR			szStyle[32];
	COLORREF		crText;
	COLORREF		crBackgnd;
	COLORREF		crBorder;
	COLORREF		crSeparator[2];

	ELC_SKIN_IMAGE	background;

	TCHAR			szButtonStyle[32];
	TCHAR			szUnionBtnStyle1[32];
	TCHAR			szUnionBtnStyle2[32];
} *PELC_SKIN_TOOLBAR;

typedef struct ELC_SKIN_BALLOON {
	TCHAR					szStyle[32];

	COLORREF				crCaption;
	COLORREF				crTitle;
	COLORREF				crItem;
	COLORREF				crHotItem;

	ELC_SKIN_FONT			ftCaption;
	ELC_SKIN_FONT			ftTitle;
	ELC_SKIN_FONT			ftItem;

	ELC_SKIN_IMAGE			background;
	ELC_SKIN_IMAGE			close;
	ELC_SKIN_IMAGE			icon;
} *PELC_SKIN_BALLOON;

typedef struct ELC_SKIN_FLOATBAR {
	TCHAR					szStyle[32];

	COLORREF				crTip;
	COLORREF				crTipShadow;
	COLORREF				crLabel;
	COLORREF				crTitle;

	int						nTitleHeight;
	int						nPeekHeight;

	ELC_SKIN_POSITION		drag;
	ELC_SKIN_NOTWND_BUTTON	fold;
	ELC_SKIN_NOTWND_BUTTON	pin;

	ELC_SKIN_IMAGE			background;
	ELC_SKIN_IMAGE			buttonbackgnd;
	ELC_SKIN_IMAGE			separator;
	ELC_SKIN_IMAGE			dropdown;
	ELC_SKIN_IMAGE			tipbackground;

} *PELC_SKIN_FLOATBAR;

typedef struct ELC_SKIN_HEADERCTRL {
	TCHAR			szStyle[32];

	ELC_SKIN_IMAGE	background;
	ELC_SKIN_IMAGE	separator;
	ELC_SKIN_IMAGE	sort;
	COLORREF		crText;
	COLORREF		crTextShadow;
} *PELC_SKIN_HEADERCTRL;

typedef struct ELC_SKIN_LIST {
	TCHAR			szStyle[32];

	// header ctrl
	TCHAR			szHeaderCtrlStyle[32];

	// list
	ELC_SKIN_IMAGE	checkbox;
	COLORREF		crText;
	COLORREF		crItemA;
	COLORREF		crItemB;
	COLORREF		crTextSelected;
	COLORREF		crSelectedBackgnd;
	COLORREF		crBorder;
	COLORREF		crBackgnd;
} *PELC_SKIN_LIST;

typedef struct ELC_SKIN_SPLITTER {
	TCHAR			szStyle[32];

	ELC_SKIN_IMAGE	drag;

	COLORREF		crBorder;
	COLORREF		crBackgnd;
}*PELC_SKIN_SPLITTER;

typedef struct ELC_SKIN_SLIDER {
	TCHAR			szStyle[32];

	ELC_SKIN_IMAGE	background;
	ELC_SKIN_IMAGE	thumb;

	COLORREF		crBackgnd;
} *PELC_SKIN_SLIDER;

typedef struct ELC_SKIN_STATUSBAR {
	TCHAR			szStyle[32];

	ELC_SKIN_IMAGE	background;

	COLORREF		crTextNormal;
	COLORREF		crTextDisabled;
	COLORREF		crPaneBorder;
	COLORREF		crBackgnd;
} *PELC_SKIN_STATUSBAR;

typedef struct ELC_SKIN_TABCTRL {
	TCHAR			szStyle[32];

	BOOL			bUpward;

	COLORREF		crTextNormal;
	COLORREF		crTextHot;
	COLORREF		crTextActive;
	COLORREF		crBackgnd;

	ELC_SKIN_IMAGE	background;
	ELC_SKIN_IMAGE	item;
	ELC_SKIN_IMAGE	borderleft;
	ELC_SKIN_IMAGE	bordercenter;
	ELC_SKIN_IMAGE	borderright;
	ELC_SKIN_IMAGE	dropdown;
	ELC_SKIN_IMAGE	buttonclose;
	ELC_SKIN_IMAGE	buttonnew;
} *PELC_SKIN_TABCTRL;

typedef struct ELC_SKIN_MENU {
	TCHAR			szStyle[32];

	COLORREF		crSelected;
	COLORREF		crSelectedBorder;
	COLORREF		crTextNormal;
	COLORREF		crTextHot;
	COLORREF		crTextDisabled;
	COLORREF		crSeparator;
	COLORREF		crBorder;
	COLORREF		crBackgnd;

	ELC_SKIN_IMAGE	check;
	ELC_SKIN_IMAGE	radio;
	ELC_SKIN_IMAGE	arrow;

} *PELC_SKIN_MENU;

typedef struct ELC_SKIN_RICH_MENU {
	TCHAR			szStyle[32];

	COLORREF		crTextNormal;
	COLORREF		crTextHot;
	COLORREF		crTextDisabled;
	COLORREF		crSeparator;

	ELC_SKIN_IMAGE	check;
	ELC_SKIN_IMAGE	select;
	ELC_SKIN_IMAGE	backgnd;
} *PELC_SKIN_RICH_MENU;

typedef struct ELC_SKIN_MENUBAR {
	COLORREF		crTextNormal;
	COLORREF		crTextHot;
	COLORREF		crTextDown;

	ELC_SKIN_IMAGE	background;
	ELC_SKIN_IMAGE	itembk;

	TCHAR			szMenuStyle[32];
} *PELC_SKIN_MENUBAR;

typedef struct ELC_SKIN_WINDOW {
	TCHAR				szStyle[32]; 

	COLORREF			crBackgnd;
	COLORREF			crCaption;

	ELC_SKIN_IMAGE		caption_min;
	ELC_SKIN_IMAGE		caption;
	ELC_SKIN_IMAGE		left;
	ELC_SKIN_IMAGE		right;
	ELC_SKIN_IMAGE		bottom;

	ELC_SKIN_MENUBAR	menubar;

	ELC_SKIN_NOTWND_BUTTON		close;
	ELC_SKIN_NOTWND_BUTTON		minimize;
	ELC_SKIN_NOTWND_BUTTON		maximize;

	ELC_SKIN_NOTWND_BUTTON		singleclose;

	ELC_SKIN_CLIENT				client;

} *PELC_SKIN_WINDOW;


typedef std::vector<PELC_SKIN_PICTURE>				CPictureSkinArray;
typedef std::vector<PELC_SKIN_BUTTON>				CButtonSkinArray;
typedef std::vector<PELC_SKIN_TOOLBAR>				CToolbarSkinArray;
typedef std::vector<PELC_SKIN_FLOATBAR>				CFloatBarSkinArray;
typedef std::vector<PELC_SKIN_BALLOON>				CBalloonSkinArray;
typedef std::vector<PELC_SKIN_HEADERCTRL>			CHeaderCtrlSkinArray;
typedef std::vector<PELC_SKIN_LIST>					CListSkinArray;
typedef std::vector<PELC_SKIN_SPLITTER>				CSplitterSkinArray;
typedef std::vector<PELC_SKIN_SLIDER>				CSliderSkinArray;
typedef std::vector<PELC_SKIN_STATUSBAR>			CStatusBarSkinArray;
typedef std::vector<PELC_SKIN_TABCTRL>				CTabCtrlSkinArray;
typedef std::vector<PELC_SKIN_MENU>					CMenuSkinArray;
typedef std::vector<PELC_SKIN_RICH_MENU>			CRichMenuArray;
typedef std::vector<PELC_SKIN_PNAE_NC>				CPaneNCSkinArray;
typedef std::vector<PELC_SKIN_PNAE_CLIENT>			CPaneCSkinArray;
typedef std::vector<PELC_SKIN_WINDOW>				CWindowSkinArray;


class LIBELCBASEUI_API CElcSkinDoc
{
public:
	CElcSkinDoc(void);
	~CElcSkinDoc(void);

public:
	BOOL LoadFile(LPCTSTR lpszFileName);

	BOOL IsValid() {
		return m_bValid;
	}
	DWORD GetVersion() {
		return m_dwVersion;
	}

	static CSize GetImageSize(const PELC_SKIN_IMAGE pSkin);
	static CSize GetImageSize(UINT nImageId, LPCTSTR lpszType);

	static CSize GetStringSize(LPCTSTR lpszText, CFont* pFont=NULL);
	static CSize GetStringSize(Graphics* pGraphics, LPCTSTR lpszText, LOGFONT& lf);
	static int CetStringHeight(LPCTSTR lpszText, int nMaxWidth, CFont* pFont=NULL);
	static CRect CetStringRect(LPCTSTR lpszText, int nMaxWidth, CFont* pFont=NULL);

	static CRect CalcRect(const LPRECT lprcDest, const PPOINT lpTopLeft, const PSIZE pSize);

	static BOOL CreateSkinFont(const PELC_SKIN_FONT pSkinFont, CFont & ftFont);
	static BOOL CreateSkinFont(const PELC_SKIN_FONT pSkinFont, LOGFONT & lf);

public:
	PELC_SKIN_GLOBAL		GetGlobalSkin();

	PELC_SKIN_PICTURE		GetPicture(LPCTSTR lpStyle);
	PELC_SKIN_BUTTON		GetButtonSkin(LPCTSTR lpStyle);
	PELC_SKIN_HEADERCTRL	GetHeaderCtrlSkin(LPCTSTR lpStyle);
	PELC_SKIN_LIST			GetListSkin(LPCTSTR lpStyle);
	PELC_SKIN_SPLITTER		GetSplitterSkin(LPCTSTR lpStyle);
	PELC_SKIN_SLIDER		GetSliderSkin(LPCTSTR lpStyle);
	PELC_SKIN_STATUSBAR		GetStatusBarSkin(LPCTSTR lpStyle);
	PELC_SKIN_MENU			GetMenuSkin(LPCTSTR lpStyle);
	PELC_SKIN_RICH_MENU		GetRichMenuSkin(LPCTSTR lpStyle);
	PELC_SKIN_TOOLBAR		GetToolbarSkin(LPCTSTR lpStyle);
	PELC_SKIN_FLOATBAR		GetFloatBarSkin(LPCTSTR lpStyle);
	PELC_SKIN_BALLOON		GetBalloonSkin(LPCTSTR lpStyle);
	PELC_SKIN_TABCTRL		GetTabCtrlSkin(LPCTSTR lpStyle);
	PELC_SKIN_PNAE_NC		GetNCPaneSkin(LPCTSTR lpStyle);
	PELC_SKIN_PNAE_CLIENT	GetClientPaneSkin(LPCTSTR lpStyle);
	PELC_SKIN_WINDOW		GetWindowSkin(LPCTSTR lpStyle);

protected:
	BOOL ParseDocument();
	BOOL ParseHead(TiXmlElement* pRoot);
	BOOL ParseSkins(TiXmlElement* pRoot);

	BOOL ParseGlobal(TiXmlElement* pElm);

	BOOL ParsePictures(TiXmlElement* pElm);
	BOOL ParsePicture(TiXmlElement* pElm, PELC_SKIN_PICTURE pSkin);

	BOOL ParsePanes(TiXmlElement* pElm);
	BOOL ParsePane(TiXmlElement* pElm, PELC_SKIN_PNAE_NC pSkin);
	BOOL ParseClientPane(TiXmlElement* pElm, PELC_SKIN_PNAE_CLIENT pSkin);

	BOOL ParseToolbars(TiXmlElement* pElm);
	BOOL ParseToolbar(TiXmlElement* pElm, PELC_SKIN_TOOLBAR pSkin);

	BOOL ParseFloatBars(TiXmlElement* pElm);
	BOOL ParseFloatBar(TiXmlElement* pElm, PELC_SKIN_FLOATBAR pSkin);

	BOOL ParseBalloons(TiXmlElement* pElm);
	BOOL ParseBalloon(TiXmlElement* pElm, PELC_SKIN_BALLOON pSkin);

	BOOL ParseButtons(TiXmlElement* pElm);
	BOOL ParseButton(TiXmlElement* pElm, PELC_SKIN_BUTTON pSkin);

	BOOL ParseHeaderCtrls(TiXmlElement* pElm);
	BOOL ParseHeaderCtrl(TiXmlElement* pElm, PELC_SKIN_HEADERCTRL pSkin);

	BOOL ParseLists(TiXmlElement* pElm);
	BOOL ParseList(TiXmlElement* pElm, PELC_SKIN_LIST pSkin);

	BOOL ParseSplitters(TiXmlElement* pElm);
	BOOL ParseSplitter(TiXmlElement* pElm, PELC_SKIN_SPLITTER pSplitter);

	BOOL ParseSliders(TiXmlElement* pElm);
	BOOL ParseSlider(TiXmlElement* pElm, PELC_SKIN_SLIDER pSlider);

	BOOL ParseStatusBars(TiXmlElement* pElm);
	BOOL ParseStatusBar(TiXmlElement* pElm, PELC_SKIN_STATUSBAR pStatusBar);

	BOOL ParseTabCtrls(TiXmlElement* pElm);
	BOOL ParseTabCtrl(TiXmlElement* pElm, PELC_SKIN_TABCTRL pTabCtrl);

	BOOL ParseWindows(TiXmlElement* pElm);
	BOOL ParseWindow(TiXmlElement* pElm, PELC_SKIN_WINDOW pWindow);
	BOOL ParseWindowControls(TiXmlElement* pElm, PELC_SKIN_WINDOW pWindow);
	BOOL ParseNWButtons(TiXmlElement* pElm, PELC_SKIN_NOTWND_BUTTON pNWButton);
	BOOL ParseMenuBar(TiXmlElement* pElm, PELC_SKIN_MENUBAR pMenuBar);
	BOOL ParseWindowClient(TiXmlElement* pElm, PELC_SKIN_CLIENT pClient);

	BOOL ParseMenus(TiXmlElement* pElm);
	BOOL ParseMenu(TiXmlElement* pElm, PELC_SKIN_MENU pMenu);

	BOOL ParseRichMenus(TiXmlElement* pElm);
	BOOL ParseRichMenu(TiXmlElement* pElm, PELC_SKIN_RICH_MENU pRichMenu);

	BOOL ParseFont(TiXmlElement* pElmFont, PELC_SKIN_FONT pFont);

	BOOL ParseImage(TiXmlElement* pElmSkin, PELC_SKIN_IMAGE pImage);

	void ReleaseDoc();


private:
	TiXmlDocument*				m_pXMLDoc;
	DWORD						m_dwVersion;
	BOOL						m_bValid;
	CString						m_strPath;

	ELC_SKIN_GLOBAL				m_global;

	CPictureSkinArray			m_arrPicture;
	CPaneNCSkinArray			m_arrPaneSkin;
	CPaneCSkinArray				m_arrPaneClientSkin;
	CToolbarSkinArray			m_arrToolbarSkin;
	CFloatBarSkinArray			m_arrFloatBarSkin;
	CBalloonSkinArray			m_arrBalloonSkin;
	CButtonSkinArray			m_arrButtonSkin;
	CHeaderCtrlSkinArray		m_arrHeaderCtrlSkin;
	CListSkinArray				m_arrListSkin;
	CSplitterSkinArray			m_arrSplitterSkin;
	CSliderSkinArray			m_arrSliderSkin;
	CStatusBarSkinArray 		m_arrStatusBarSkin;
	CTabCtrlSkinArray			m_arrTabCtrlSkin;
	CWindowSkinArray			m_arrWindowSkin;
	CMenuSkinArray				m_arrMenuSkin;
	CRichMenuArray				m_arrRichMenuSkin;
};
