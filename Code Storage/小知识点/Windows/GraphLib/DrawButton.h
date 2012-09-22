#pragma once
#define WM_BTNCLICK			(WM_USER + 160)

//Mouse State
#define MU_NONE				0
#define MU_LBTNDOWN			1
#define MU_LBTNUP			2
#define MU_MOVE				3	
#define	MU_LDCLK			4
#define MU_LBTNMOUSE		5	//mousemove with lbutton down
#define MU_MOUSELEAVE		6

enum ST_TATE {
	ST_NONE = 1,
	ST_SELECTED,
	ST_FOCUS,
	ST_LBTNIN,
	ST_DISABLED
};

enum BT_STYLE
{
	BT_NORMAL = 1,
	BT_RADIO,
	BT_TOGGLE,
	BT_NULL
};

class CDrawButton
{
public:
	CDrawButton(void);
	~CDrawButton(void);

	CDrawButton & operator = (const CDrawButton& drawPage);

public:
	void SetButtonID(UINT nID);
	void SetToggle(BOOL bToggle);
	void SetParenthwnd(HWND hwnd);
	void SetPNGImage(UINT nImageID);
	void SetButtonRect(CRect& rect);
	void SetClickDown(BOOL bClkdown);
	void SetButtonState(ST_TATE nState);
	void SetButtonToggle(BOOL bToggled);
	void SetButtonStyle(BT_STYLE nStyle);

	void SetMenuRadio(BOOL bMenu);
	void SetEnhanceRadio(BOOL bEnhance);

	int GetButtonID();
	CRect GetButtonRect();
	ST_TATE GetButtonState();
	void EnableButtun(BOOL bEnable);
	BOOL IsptInButton(CPoint point);
	
	void Create(UINT nID, HWND hParent, CRect &rect);

	void DrawButton(CDC* pDC);
	void DrawRadioButton(CDC* pDC);
	void DrawToggleButton(CDC* pDC);
	void DrawNormalButton(CDC* pDC);
	void MouseState(UINT nFlags, CPoint point);

	void RadioMouseSate(UINT nFlags, CPoint point);
	void ToggleMouseSate(UINT nFlags, CPoint point);
	void NoramalMouseSate(UINT nFlags, CPoint point);

protected:
	CRect		m_rcBtn;
	HWND		m_hParent;

	UINT		m_nID;
	int			m_nImageID;
	Image*		m_pImage;

	ST_TATE		m_STATE;
	BT_STYLE	m_nStyle;

	int			m_nUnitHeight;
	int			m_nImageWidth;
	int			m_nImageHeight;

	BOOL		m_bMenu;
	BOOL		m_bToggle;
	BOOL		m_bClickDown;
	BOOL		m_bEnRadio;//if the radio is the enhence radio
};
