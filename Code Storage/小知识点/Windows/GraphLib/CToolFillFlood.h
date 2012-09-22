#pragma once

typedef struct Bitmap_ST
{
	Bitmap*	m_pBmp;
	int		m_cRefrencce;

	Bitmap_ST()
	{
		m_pBmp = NULL;
		m_cRefrencce = 0;
	}

	Bitmap_ST(int nWidth, int nHeight)
	{
		m_cRefrencce = 1;
		m_pBmp = new Bitmap(nWidth, nHeight);
	}

	void DecreaseRefrence()
	{
		m_cRefrencce--;
	}

	void IncreaseRefrence()
	{
		m_cRefrencce++;
	}

	~Bitmap_ST()
	{
		m_cRefrencce = 0;

		if(m_pBmp)
		{
			delete m_pBmp;
			m_pBmp = NULL;
		}
	}
}*PBitmap_ST;

class GRAPHLIBAPI CToolFillFlood : public CGraph
{
public:
	~CToolFillFlood(void);
	CToolFillFlood(Color clr);
	CToolFillFlood(const CToolFillFlood& gph);
	CToolFillFlood & operator = (const CToolFillFlood& gph);

	virtual void SetRegion();
	virtual void InitHotData();
	virtual void FinishGraph();
	virtual void DrawHotShape(Graphics& graph);

	virtual ModifyType AroundHotPts(CPoint point);
	virtual void Draw(CDC* pDC, CRect& rcInvalid);
	virtual int MoveGraph(float offsetx, float offsety);
	virtual void DecideModiFyType(UINT nMouseState, CPoint point);
	virtual void AddPoint(UINT nMouseState, CPoint point, CRect& rcUpdate);
	virtual void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);

	void FormRegion(CDC* pDC);
	COLORREF GetPixel(int x, int y, int nbmWidthBytes, BYTE* pBmpBit);
	void SetPixel(int x, int y, int nbmWidthBytes, COLORREF clr, BYTE* pBmpBit);

	inline BOOL ColorEqual(COLORREF clr1, COLORREF clr2)
	{
		if(GetRValue(clr1) == GetRValue(clr2)
			&& GetGValue(clr1) == GetGValue(clr2)
			&& GetBValue(clr1) == GetBValue(clr2))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

protected:
	PBitmap_ST	m_pBmpST;
	CRect		m_rcBound;
	float		m_foffsetX;
	float		m_foffsetY;
	BOOL		m_bRegionFormed;
};
