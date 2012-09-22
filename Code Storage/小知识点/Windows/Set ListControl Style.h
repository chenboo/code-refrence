//Change the style of the List Control

//Method 1:
//SetCtrlStyle is self definition function
void SetCtrlStyle(HWND hWnd, DWORD dwNewStyle)
{
	DWORD dwOldStyle;
	dwOldStyle = GetWindowLong(hWnd, GWL_STYLE);

	if((dwOldStyle & LVS_TYPEMASK) != dwNewStyle)
	{
		//reset the style
		dwOldStyle &= ~LVS_TYPEMASK;

		//set to the new style
		dwNewStyle |= dwOldStyle;

		SetWindowLong(hWnd, GWL_STYLE, dwNewStyle);
	}
}

//LVS_TYPEMASK (Determines the control's current window style.)
//You can use the LVS_TYPEMASK mask to isolate the window styles 
//that correspond to the current view: LVS_ICON, LVS_LIST, LVS_REPORT, and LVS_SMALLICON.

//Method 2: more fast

ModifyStyle(LVS_TYPEMASK, LVS_ICON);//Large Icon Style
ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);//Small Icon Style
ModifyStyle(LVS_TYPEMASK, LVS_LIST);//Report Style


//systerm definition
//#define LVS_ICON                0x0000
//#define LVS_REPORT              0x0001
//#define LVS_SMALLICON           0x0002
//#define LVS_LIST                0x0003
//#define LVS_TYPEMASK            0x0003