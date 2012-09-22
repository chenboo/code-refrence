#ifndef __ELC_UI_COMMON_H__
#define __ELC_UI_COMMON_H__

#pragma once

/******************************************************
 Predefined Resource Types
*/
#define RT_PNG									_T("RT_PNG") // Custom defined Resource Type
/*
* The following Resource Type were defined by MS OS in "winuser.h"
*
#define RT_CURSOR           MAKEINTRESOURCE(1)
#define RT_BITMAP           MAKEINTRESOURCE(2)
#define RT_ICON             MAKEINTRESOURCE(3)
#define RT_MENU             MAKEINTRESOURCE(4)
#define RT_DIALOG           MAKEINTRESOURCE(5)
#define RT_STRING           MAKEINTRESOURCE(6)
#define RT_FONTDIR          MAKEINTRESOURCE(7)
#define RT_FONT             MAKEINTRESOURCE(8)
#define RT_ACCELERATOR      MAKEINTRESOURCE(9)
#define RT_RCDATA           MAKEINTRESOURCE(10)
#define RT_MESSAGETABLE     MAKEINTRESOURCE(11)
*/

enum ELC_RESOURCE_TYPE {
	ELCRT_PNG = 0,
	ELCRT_CURSOR,
	ELCRT_BITMAP,
	ELCRT_ICON,
};

/******************************************************
 The CElcToolBarEx post this message when it's position has changed
*/
#define WM_ELC_SKINFRAME_REPOSITIONBARS			(WM_APP + 0x0FFF)

#define	WM_MENUBAR_NOTIFY						(WM_APP + 0x0FFE)

/******************************************************
 Re-Initialize Frame Window when Window's Style (StyleEx) has been changed.
*/
#define WM_ELC_SKINFRAME_REINITIALIZE			(WM_APP + 0x0FFD)

/******************************************************
 The ELCUI-Control send this message to it's parent window when drawing.
 Return non-zero if an application processes this message.
 wParam : Control's ID.
 lParam : ELC_UI_DRAW struct pointer of ELCUI-Control.
*/
enum ELC_UI_DRAW_TYPE {
	ELC_UI_DT_PREDRAW,
	ELC_UI_DT_DRAWING, // not use yet
	ELC_UI_DT_DRAWED,
};

typedef struct ELC_UI_DRAW
{
	HWND				hWnd;
	ELC_UI_DRAW_TYPE	type;
	HDC					hdc;
	RECT				rect;
} *PELC_UI_DRAW;

#define WM_ELC_UI_DRAW_NOTIFY				(WM_APP + 0x0FFC)


/******************************************************
 When the Rich-Menu Control is been destroyed, it send this message to its parent window.
*/
#define WM_ELC_RICH_MENU_NOTIFY					(WM_APP + 0x0FFB)

/******************************************************
 The ELCUI-Control send this message to it's parent window when mouse state changed in the control.
*/
#define WM_ELC_MOUSE_NOTIFY					(WM_APP + 0x0FFA)

enum ELC_MOUSE_NOTIFY_TYPE
{
	ELC_MOUSE_MOVE,
	ELC_MOUSE_LBUTTON_DOWN,
	ELC_MOUSE_LBUTTON_UP,
	ELC_MOUSE_RBUTTON_DOWN,
	ELC_MOUSE_RBUTTON_UP,
};

typedef struct ELC_MOUSE_NOTIFY
{
	HWND					hWnd; // control's window handle
	ELC_MOUSE_NOTIFY_TYPE	type;
	UINT					nFlags;
	POINT					point; // in screen coordinates
} *PELC_MOUSE_NOTIFY;


/******************************************************
 ELCBaseUI Predefine Control Ids
*/
#define ELC_IDW_MAINTOOLBAR_FIRST				(AFX_IDW_TOOLBAR + 0x0020)
#define ELC_ID_STATIC_ITEM_FIRST				0x0000FFAA


/******************************************************
 ELCBaseUI Predefine Timer Ids
*/ 
#define ELC_SKIN_FRAME_STATE_TIMER				0x00007FFF



/******************************************************
 Elc Button's enum
*/
enum ELC_BUTTON_STATE {
	ELCBS_NORMAL,
	ELCBS_HOT,
	ELCBS_DOWN,
	ELCBS_DISABLED,
};

enum ELC_BUTTON_ICON_TYPE {
	ELCBIM_SINGLE = 1,
	ELCBIM_FOUR_STATE = 4,
};

enum ELC_BUTTON_DROP_TYPE {
	ELCBDT_NORMAL = 0,
	ELCBDT_DROPDOWN,
	ELCBDT_PULLUP,
};

enum ELC_BUTTON_TEXT_STYLE {
	ELCBTS_NORMAL = 0,
	ELCBTS_UNDERLINE_WHEN_HOT,
	ELCBTS_UNDERLINE_WHEN_ANYTIME,
};

enum ELC_BUTTON_LAYOUT {
	ELCBL_HORIZONTAL = 0,
	ELCBL_VERTICAL,
};


/******************************************************
 Elc Timer ID
*/
typedef struct ELC_TIMER_ID {
	DWORD		bStart : 1;
	DWORD		dwId : 11;
	DWORD		dwTimeMS : 20;
} *PELC_TIMER_ID;

#define ELC_TIMER_START(x) { SetTimer(x.dwId, x.dwTimeMS, NULL); x.bStart = TRUE; }
#define ELC_TIMER_STOP(x) { if (x.bStart) { KillTimer(x.dwId); x.bStart = FALSE; } }


#endif // __ELC_UI_COMMON_H__