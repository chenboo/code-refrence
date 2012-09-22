#pragma once
#include "ElcHeaderCtrl.h"

class LIBELCBASEUI_API CElcListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CElcListCtrl)

public:
	CElcListCtrl();
	virtual ~CElcListCtrl();

public:

	BOOL LoadSkin(LPCTSTR lpszStyle);

	BOOL SetSkinHeader(BOOL bEnable);
	CElcHeaderCtrl* GetHeaderCtrl() {
		return &m_SkinHeaderCtrl;
	}

protected:
	CElcHeaderCtrl			m_SkinHeaderCtrl;
	BOOL					m_bSkinedHeaderCtrl;
	ELC_SKIN_LIST			m_skin;

protected:
	virtual void PreSubclassWindow();
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDownNotify(NMHDR* pNMHDR, LRESULT* pResult);
};


