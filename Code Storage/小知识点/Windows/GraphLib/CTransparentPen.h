#pragma once

class GRAPHLIBAPI CTransparentPen : public CGraph
{
public:
	CTransparentPen();
	CTransparentPen(Color cr, int nWidth);
	CTransparentPen(const CTransparentPen& gph);
	CTransparentPen& operator = (const CTransparentPen& gph);
	~CTransparentPen(void);

	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void InitHotData();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	virtual void SetColor(Color crColor);

	BYTE GetAlpha();
	void SetAlpha(BYTE nalpha);
	void FormGraphData(UINT nMouseState, CPoint point, CRect& rcUpdate);

public:
	int			m_noffst;
	int			m_nDCount;
	BYTE		m_nAlpha;
};
