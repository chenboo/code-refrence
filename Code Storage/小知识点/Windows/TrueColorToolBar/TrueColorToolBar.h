/***=========================================================================
====                                                                     ====
====                          D C U t i l i t y                          ====
====                                                                     ====
=============================================================================
====                                                                     ====
====    File name           :  TrueColorToolBar.h                        ====
====    Project name        :  Tester                                    ====
====    Project number      :  ---                                       ====
====    Creation date       :  13/1/2003                                 ====
====    Author(s)           :  Dany Cantin                               ====
====                                                                     ====
====                  Copyright ?DCUtility  2003                        ====
====                                                                     ====
=============================================================================
===========================================================================*/

#ifndef TRUECOLORTOOLBAR_H_
#define TRUECOLORTOOLBAR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>


class CTrueColorToolBar : public CToolBar
{
public:
	CTrueColorToolBar();
	virtual ~CTrueColorToolBar();

private:
	struct stDropDownInfo
	{
		UINT  uButtonID;
		UINT  uMenuID;
		CWnd* pParent;
	};

	BOOL	m_bDropDown;
	CArray <stDropDownInfo, stDropDownInfo&> m_lstDropDownButton;

// Operations
public:
	BOOL LoadTrueColorToolBar(int  nBtnWidth,
							  UINT uToolBar,
							  UINT uToolBarHot		= 0,
							  UINT uToolBarDisabled = 0);

	void AddDropDownButton(CWnd* pParent, UINT uButtonID, UINT uMenuID);
	BOOL SetTrueColorToolBar(UINT uToolBarType, UINT uToolBar, int nBtnWidth);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnToolbarDropDown(NMHDR* pnmhdr, LRESULT* plRes);
};

#endif // TRUECOLORTOOLBAR_H_
