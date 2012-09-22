#pragma once


// CElcSkinBackgndWnd

class LIBELCBASEUI_API CElcSkinBackgndWnd : public CWnd
{
	DECLARE_DYNAMIC(CElcSkinBackgndWnd)

public:
	CElcSkinBackgndWnd();
	virtual ~CElcSkinBackgndWnd();

	// load nc skin
	// call this function after window been created;
	BOOL SetNCSkin(LPCTSTR lpStyle);
	// load client skin
	BOOL SetClientSkin(LPCTSTR lpStyle);

	PELC_SKIN_PNAE_NC GetNCSkin() {
		return m_pNCSkin;
	}
	PELC_SKIN_PNAE_CLIENT GetClientSkin() {
		return m_pCSkin;
	}

	void Draw(CDC* pDC);
	void Draw(PVOID pvGraphics);

protected:
	PELC_SKIN_PNAE_NC		m_pNCSkin;
	PELC_SKIN_PNAE_CLIENT	m_pCSkin;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


