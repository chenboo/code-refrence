#pragma once
#include <atlimage.h>

class GRAPHLIBAPI CPicture : public CGraph
{
public:
	CPicture();
	~CPicture(void);
	CPicture(CString strName);
	CPicture(const CPicture& gph);
	CPicture & operator = (const CPicture& gph);
	
	virtual void SetRegion();
	virtual void FinishGraph();
	virtual void InitHotData();
	virtual void Draw(CDC* pDC,CRect& rcInvalid);
	virtual ModifyType AroundHotPts(CPoint point);
	virtual void AddPoint(UINT nMouseState,CPoint point, CRect& rcUpdate);
	virtual void FormGraphData(UINT nMouseState,CPoint point,CRect& rcUpdate){};
	
	void InitDataMember();
	void InitPictureData();
	void SetImageFile(CString strname);

public:
	int			m_OrgTop;
	int			m_OrgLeft;
	Image*		m_pImage;
	CString		m_strFilename;
};





