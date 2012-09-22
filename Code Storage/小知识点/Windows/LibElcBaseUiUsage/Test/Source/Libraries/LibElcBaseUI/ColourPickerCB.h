#pragma once

void DDX_ColourPickerCB( CDataExchange *pDX, int nIDC, COLORREF& prgbColor );

struct Colour
{
	Colour( COLORREF crColour, CString strName )
		: m_crColour( crColour )
		, m_strName( strName )
	{};

	COLORREF m_crColour;
	CString m_strName;
};


class LIBELCBASEUI_API CColourPickerCB : public CComboBox
{
public:
	CColourPickerCB();
	virtual	~CColourPickerCB();
	
public:
	void AddColour( CString strName, COLORREF crColour );
	COLORREF GetSelectedColourValue();
	void SetCustomName(LPCTSTR lpszName);
	void SetNotifyWindow(HWND hWnd, UINT nMsg);

protected:
	virtual void PreSubclassWindow();	
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	void Initialise();

protected:
	CString m_strColourName;
	CString m_strCustom;

	HWND m_hNotifyWnd;
	UINT m_hNotifyMsg;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchange();
};
