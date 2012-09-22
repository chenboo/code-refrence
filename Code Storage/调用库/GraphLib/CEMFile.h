#include "CGraph.h"
#include <atlimage.h>


class CEMFile : public CGraph
{
public:
	CEMFile(CString strName);
	CEMFile(const CEMFile& gph);
	CEMFile();
	
	~CEMFile(void);
	
	virtual void Draw(CDC* pDC);
	virtual	void AddPoint(UINT nMouseState,CPoint point);
	virtual void FinishGraph();
	virtual void SetRegion();

public:
	Image*		m_pImage;	
	CString		m_strFilename;

	int			m_nOrgWidth;
	int			m_nOrgHeight;
	int			m_OrgLeft;
	int			m_OrgTop;

public:
	void FormGraphData(UINT nMouseState,CPoint point);
	void LoadMetaFile(CString strname);
	void InitRegion(CString strname);
};





