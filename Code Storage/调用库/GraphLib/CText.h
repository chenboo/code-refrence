#include "CGraph.h"

class CText : public CGraph
{
public:
	CText();

	CText(Font* pfnt,CString strText);//interface left
	CText(const CText& gph);
	~CText();
	
public:	
	virtual void Draw(CDC* pDC);
	virtual	void AddPoint(UINT nMouseState,CPoint point);
	virtual void FinishGraph();
	virtual void SetRegion();
	virtual void InitHotData(BOOL bExtend = FALSE);

public:
	void FormGraphData(UINT nMouseState,CPoint point);
	void InitRegion();

	void SetTextString(CString strText,RectF rclayout);
	void SetTextString(CString strText);
	void SetTextFont(Font& fnt);

	void InitFontData();
	
private:
	Font*			m_pFont;
	FontFamily*		m_pFntFmy;//font family
	CString			m_strText;

public:
	BOOL			m_bComplete;//determine whethe the rect of the editbox has been finished
	BOOL			m_blbtndown;
	RectF			m_rclayout;

//Interface left
private://interface variables
	float			m_fsize;
	BOOL			m_bBold;
	BOOL			m_bItalic;
	BOOL			m_bUnderLine;

public://interface functions
	void ModifyFont();//help function called by the functions below

	//interface for out calling
	void SetFntFmy(FontFamily& FntFmy);
	void SetFntSize(float fsize);
	void SetBold(BOOL bBold);
	void SetItalic(BOOL bItalic);
	void SetUnderLine(BOOL bUnderLine);
};