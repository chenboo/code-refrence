/////////////////////////////////////////////////
// Author : tomlie
// if any question, pls let me know, thx.
// call me for more details if necessary.
/////////////////////////////////////////////////



#pragma once 


enum ELC_NW_CTRL_TYPE {
	ENWCT_BUTTON = 1,
	ENWCT_LABEL,
};


class CElcNWControl
{
public:
	virtual UINT GetId() = NULL;

	virtual BOOL PtInRect(CPoint point) = NULL;
	virtual CRect GetRect() = NULL;
	virtual void Move(CRect rcNew, BOOL bRedraw) = NULL;

	virtual BOOL MouseDown(CPoint point) = NULL;
	virtual BOOL MouseUp(CPoint point) = NULL;
	virtual BOOL MouseMove(CPoint point) = NULL;
	virtual void CaptureChanged() = NULL;

	virtual void Draw(PVOID pvGraphics) = NULL;
	virtual void SetRedraw(BOOL bEnable) = NULL;
};

