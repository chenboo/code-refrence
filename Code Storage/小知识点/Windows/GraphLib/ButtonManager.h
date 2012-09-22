#pragma once
#include "DrawButton.h"

class CButtonManager
{
public:
	CButtonManager(void);
	~CButtonManager(void);

public:
	void CreateButtonManager(int nIconId[],
		int nBtnID[],
		int nstyle[],
		CRect rects[],
		int nCount,
		HWND hparent);

	BT_STYLE IsPtInButtonsRect(CPoint point);
	void MouseState(UINT nFlags, CPoint point);

	void NormalMouseState(UINT nFlags, CPoint point);
	void RadioMouseState(UINT nFlags, CPoint point);
	void ToggleMouseState(UINT nFlags, CPoint point);

	void DrawButtons(CDC* pDC);

	BOOL IsHasButton();
	int GetActiveRadio();
	UINT GetActiveRadioID();
	CRect GetButtonRect(int nID);

	void ResetAllRadioButton();
	void RestoreActiveRadio();
	void SetActiveRadio(int nID);
	void SetButtonToggle(int nID, BOOL bToggled);

	void EnableRadioButton(int nbtnID, BOOL bEnable);
	void EnableNormalButton(int nbtnID, BOOL bEnable);

	void SetRadioButtonMenu(int nbtnID, BOOL bMenu);
	void SetRadioButtonEnhance(int nbtnID, BOOL bEnhance);

protected:
	CArray<CDrawButton, CDrawButton&>	m_NorbtnArray;
	CArray<CDrawButton, CDrawButton&>	m_RadbtnArray;
	CArray<CDrawButton, CDrawButton&>	m_TogbntArray;

	BOOL								m_bHasButton;
	int									m_nActiveRadbtn;
};
