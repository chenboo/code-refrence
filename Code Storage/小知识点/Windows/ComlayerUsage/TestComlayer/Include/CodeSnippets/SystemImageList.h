#ifndef __SYSTEMIMAGELIST_H__
#define __SYSTEMIMAGELIST_H__

class CSystemImageList  
{
public:
	CSystemImageList();
	virtual ~CSystemImageList();

//Methods
	CImageList& GetNormalImageList();
	CImageList& GetSmallImageList();

protected:
	CImageList m_SmallImageList;
	CImageList m_NormalImageList;

	static int m_nSmallRefCount;
	static int m_nNormalRefCount;
};

#endif //__SYSTEMIMAGELIST_H__
