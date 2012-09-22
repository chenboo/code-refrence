#ifndef __ELC_SKIN_STYLE_H__
#define __ELC_SKIN_STYLE_H__

#pragma once

/******************************************************
 Picture Style
*/
#define ELC_SKIN_STYLE_PICTURE_MAIN			_T("style1")
#define ELC_SKIN_STYLE_PICTURE_MAINUI_LOGO	_T("mainui_logo")

/******************************************************
 Button Style
*/
#define ELC_SKIN_STYLE_BUTTON_MAIN_TOOLBAR	_T("style1")
#define ELC_SKIN_STYLE_BUTTON_SUB_TOOLBAR	_T("style2")
#define ELC_SKIN_STYLE_BUTTON_DLG			_T("style3")
#define ELC_SKIN_STYLE_BUTTON_4				_T("style4")
#define ELC_SKIN_STYLE_BUTTON_CHECKBOX		_T("checkbox1")
#define ELC_SKIN_STYLE_BUTTON_CHECKBOX_2	_T("checkbox2")
#define ELC_SKIN_STYLE_BUTTON_GROUPBOX		_T("groupbox1")
#define ELC_SKIN_STYLE_BUTTON_GROUPBOX_2	_T("groupbox2")
#define ELC_SKIN_STYLE_UNION_LEFT_BUTTON_1	_T("union_button_left_1")
#define ELC_SKIN_STYLE_UNION_RIGHT_BUTTON_1	_T("union_button_right_1")


/******************************************************
 Toolbar Style
*/
#define ELC_SKIN_STYLE_TOOLBAR_MAIN			_T("style1")
#define ELC_SKIN_STYLE_TOOLBAR_SUB_1		_T("style2")
#define ELC_SKIN_STYLE_TOOLBAR_SUB_2		_T("style3")


/******************************************************
 FloatBar Style
*/
#define ELC_SKIN_STYLE_FLOATBAR_MAIN		_T("main")


/******************************************************
 Splitter Style
*/
#define ELC_SKIN_STYLE_SPLITTER_MAIN		_T("style1")
#define ELC_SKIN_STYLE_SPLITTER_SUB_1		_T("style2")


/******************************************************
 Slider Style
*/
#define ELC_SKIN_STYLE_SLIDER_BIG			_T("style1")
#define ELC_SKIN_STYLE_SLIDER_SMALL			_T("style2")
#define ELC_SKIN_STYLE_SLIDER_ORANGE		_T("slider_orange")


/******************************************************
 HeaderCtrl Style
*/
#define ELC_SKIN_STYLE_HEADERCTRL_MAIN		_T("style1")


/******************************************************
 Report-List Style
*/
#define ELC_SKIN_STYLE_LIST_MAIN			_T("style1")


/******************************************************
 Menu Style
*/
#define ELC_SKIN_STYLE_MENU_MAIN			_T("style1")


/******************************************************
 Rich-Menu Style
*/
#define ELC_SKIN_STYLE_RICHMENU_MAIN		_T("style1")

/******************************************************
 StatusBar Style
*/
#define ELC_SKIN_STYLE_STATUSBAR_MAINFRAME	_T("style1")


/******************************************************
 TabCtrl Style
*/
#define ELC_SKIN_STYLE_TABCTRL_DOWNWARD			_T("style1")
#define ELC_SKIN_STYLE_TABCTRL_UPWARD			_T("style2")
#define ELC_SKIN_STYLE_TABCTRL_INPANE_DOWNWARD	_T("style3")
#define ELC_SKIN_STYLE_TABCTRL_INPANE_UPWARD	_T("style4")


/******************************************************
 Client Pane Style
 This style using for: Skin a window's client-region.
*/
#define ELC_SKIN_STYLE_CLIENTPANE_1			_T("style1") // the vr player control pane


/******************************************************
 Pane Style
 Notice : These styles are designed only for CNCSkinBinding class to skin a child window's non-client region.
 DO NOT use these styles in others class.
*/
#define ELC_SKIN_STYLE_PANE_CHILD			_T("style1") // child is a main pane
#define ELC_SKIN_STYLE_PANE_CONTROL_INNER	_T("style2") // controls, such as edit, tree, list...
#define ELC_SKIN_STYLE_PANE_VRPLAYER		_T("style3") // vr player
#define ELC_SKIN_STYLE_PANE_VRWFCTRL		_T("style4") // vr waveform control
#define ELC_SKIN_STYLE_PANE_NRWFCTRL		_T("style5") // net-repeat waveform control
#define ELC_SKIN_STYLE_PANE_NRPLAYBAR		_T("style6") // net-repeat playbar control
#define ELC_SKIN_STYLE_PANE_VIDEOPANE		_T("style7")


/******************************************************
 Window Style
 Notice : These styles are designed for CElcSkinFrame, CElcSkinDialog, CElcSkinWnd and CSkinWndBinder.
 DO NOT use these styles in others class
*/
#define ELC_SKIN_STYLE_WINDOW_FRAME			_T("mainframe")
#define ELC_SKIN_STYLE_WINDOW_DLG			_T("dialog")
#define ELC_SKIN_STYLE_WINDOW_NCSKIN_WND	_T("window_ncskin_pane")
#define ELC_SKIN_STYLE_WINDOW_INNER_PANE	_T("window_inner_pane")
#define ELC_SKIN_STYLE_WINDOW_IMAGE_WND		_T("window_clientskin_pane")
#define ELC_SKIN_STYLE_WINDOW_COLORBK_WND	_T("window_clientbkcolor_pane")
#define ELC_SKIN_STYLE_WINDOW_COLORBK_MAIN	_T("window_clientbkcolor_pane")




#endif // __ELC_SKIN_STYLE_H__