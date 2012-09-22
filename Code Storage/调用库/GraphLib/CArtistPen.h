#include "CGraph.h"
#include <atlimage.h>

class CArtistPen : public CGraph
{
public:
	CArtistPen(int nAPStyle);
	CArtistPen();
	CArtistPen(const CArtistPen& gph);
	~CArtistPen(void);
	
	virtual void Draw(CDC* pDC);
	virtual	void AddPoint(UINT nMouseState,CPoint point);
	virtual void FinishGraph();
	virtual void SetRegion();

public:
	void SetArtistPenStyle(int nAPstyle);
	void FormGraphData(UINT nMouseState,CPoint point);

	void DrawBrush(Graphics& graph);
	void Draw3Clr(Graphics& graph);
	void DrawImagelines(Graphics& graph,Image*	pImg);

	void FormDarwImageData(PointF pt,float fspace);
	void InitImagepointer();
	void InputGraphData(CPoint* pPt,int nCount);
	void AdjustTheData();
	
	void LoadImage(UINT nID, CImage* pImage);

	void SetImage(Image* pImage);
	void SetpngRegion();
	
private:
	int		m_nAPstyle;	
	Image*	m_pImg;
	int		m_noffst;
};