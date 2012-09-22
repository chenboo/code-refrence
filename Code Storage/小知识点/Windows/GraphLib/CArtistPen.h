#include <atlimage.h>

class GRAPHLIBAPI CArtistPen : public CGraph
{
public:
	CArtistPen();
	~CArtistPen(void);
	CArtistPen(const CArtistPen& gph);
	CArtistPen(int nAPStyle, int nWidth);
	CArtistPen& operator = (const CArtistPen& gph);
	
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	virtual void PrePareBufferDC(CDC* pDCMem, CDC* pDCOrg, CRect& rect);

public:
	void SetDrawingBmp();
	void SetArtistPenStyle(int nAPstyle);
	void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate);

	void Draw3Clr(Graphics& graph, POINTARRAY* ptArray);
	void DrawImagelines(Graphics& graph, POINTARRAY* ptArray);

	void AdjustTheData();
	int  InputGraphDataIndirect(CPoint* pPt,int nCount);
	void FormDarwImageData(PointF pt, float fspace, CRect& rcUpdate);//to form the Image point data

	void DrawSmileImage(Graphics& graph,PointF pt);
	void Draw5StarsImage(Graphics& graph,PointF pt);

	void FormSmileRegion();
	void Form5StarsRegion();

	void Add5Starpath(GraphicsPath& path,PointF& pt);

	int GetAPStyle()
	{
		return m_nAPstyle;
	}
	
private:
	int			m_noffst;
	int			m_nDCount;
	APenStyle	m_nAPstyle;
	Bitmap*		m_pBitmap;
};