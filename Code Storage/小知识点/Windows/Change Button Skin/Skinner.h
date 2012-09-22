#pragma once

enum MouseState
{
	MS_NONE,
	MS_LBUTTONDOWN,
	MS_LBUTTONUP,
	MS_MOUSEHOVER
};

class CSkinner
{
public:
	CSkinner(void);
	~CSkinner(void);

public:
	void InitSkin();
	void ExitSkin();

	//Set Hook to the Thread to change the defualt WindowProc of Button
	static LRESULT WINAPI HookProc(int nCode, WPARAM wParam, LPARAM lParam);
	//New button WindowProc
	static LRESULT SkinWindowProc(HWND hWnd,UINT message,
							WPARAM wParam,
							LPARAM lParam);
	//Messages entries Functions
	static LRESULT OnPaint(CWnd *pWnd);
	static LRESULT OnLButtonDown(CWnd *pWnd, UINT nFlags, CPoint point);
	static LRESULT OnLButtonUp(CWnd *pWnd, UINT nFlags, CPoint point);
	static LRESULT OnLButtonDblClk(CWnd *pWnd, UINT nFlags, CPoint point);
	static LRESULT OnMouseMove(CWnd *pWnd, UINT nFlags, CPoint point);
	static LRESULT OnEraseBkgnd(CWnd *pWnd, CDC *pDC);

public:
	static HHOOK		m_hhook;
	static WNDPROC		m_WndProc;
	static MouseState	m_nState;
};
