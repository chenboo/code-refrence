#pragma once
class CTextEditWnd;

class GRAPHLIBAPI CText : public CGraph
{
public:
	CText();
	~CText();
	CText(const CText& gph);
	CText(CWnd* pParent, Color clr = Color::Black);
	CText& operator = (const CText& gph);
	
public:
	virtual void SetRegion();
	virtual void SetColor(Color crColor);
	virtual void SetEditSate(BOOL bEdit);

	virtual void InitHotData();
	virtual void FinishGraph();
	virtual CRect FormUpdateRect();
	virtual void Draw(CDC* pDC, CRect& rcInvalid);
	virtual void AddPoint(UINT nMouseState, CPoint point, CRect& rcUpdate);
	virtual void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);

	void HideEditBox();
	void InitDataMember();
	void ShowEditBoxDefault();
	virtual ModifyType AroundHotPts(CPoint point);
	void DrawEditBoxHotPoints(Graphics& graph);
	void DecideEditTextBoxType(UINT nMouseState, CPoint point);

	void SetHotData(CRect rect);
	void SetTextInfo(CString strText, Font& fnt);//input the text string info

	CString GetTextString();
	Font* GetTextFont();

	//interface for outside calling
	//not used in this application
	void SetFntFmy(FontFamily& FntFmy);
	void SetFntSize(float fsize);
	void SetBold(BOOL bBold);
	void SetItalic(BOOL bItalic);
	void SetUnderLine(BOOL bUnderLine);
	void SetTextFont(Font& fnt);
	void InitFontData();
	void ModifyFont();//help function

	void SetTextInfoFrmEditBox();
	void UpdateTextBorder(CRect rect);
	BOOL CreateEditTextBox(CWnd* pParent);

protected:
	//interface variables
	float					m_fsize;
	BOOL					m_bBold;
	BOOL					m_bFresh;	//used to refresh the board when the edit box is hidden
	BOOL					m_bItalic;
	BOOL					m_bUnderLine;
	BOOL					m_bDataOk;//determine whethe the rect of the editbox has been finished

	Font*					m_pFont;
	CString					m_strText;
	CWnd*					m_pWndParent;
	static CTextEditWnd*	m_pWndTextEdit;
};