#pragma once

class LIBELCBASEUI_API CSplashWnd : public CWnd
{
protected:
	CSplashWnd();
	virtual ~CSplashWnd();

public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(UINT nBitmapId, UINT nElapseTime, CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);


protected:
	BOOL Create(UINT nBitmapId, UINT nElapseTime, CWnd* pParentWnd = NULL);
	void HideSplashScreen();

protected:
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;

	CBitmap m_bitmap;
	UINT	m_nElapseTime;

protected:
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
};
