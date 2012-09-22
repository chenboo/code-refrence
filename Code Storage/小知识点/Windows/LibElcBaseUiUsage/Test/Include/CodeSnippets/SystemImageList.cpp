#include "stdafx.h"
#include "SystemImageList.h"

/////////////////////////////////////////////////////////////////////////////
//CSystemImageList
int CSystemImageList::m_nSmallRefCount=0;
int CSystemImageList::m_nNormalRefCount=0;

CSystemImageList::CSystemImageList()
{
	if(m_nSmallRefCount == 0)
	{
		//Attach to the system image list
		SHFILEINFO sfi;
		HIMAGELIST hSystemImageList = (HIMAGELIST) SHGetFileInfo(
			_T("C:\\"), 0, &sfi, sizeof(SHFILEINFO),SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
		VERIFY(m_SmallImageList.Attach(hSystemImageList));
	}

	if(m_nNormalRefCount == 0)
	{
		//Attach to the system image list
		SHFILEINFO sfi;
		HIMAGELIST hSystemImageList = (HIMAGELIST) SHGetFileInfo(
			_T("C:\\"), 0, &sfi, sizeof(SHFILEINFO),SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		VERIFY(m_NormalImageList.Attach(hSystemImageList));
	}

	//Increment the reference count
	m_nSmallRefCount++;
	m_nNormalRefCount++;
}

CSystemImageList::~CSystemImageList()
{
	//Decrement the reference count
	m_nSmallRefCount--;
	m_nNormalRefCount--;

	if(m_nSmallRefCount == 0)
	{
		//Detach from the image list to prevent problems on 95/98 where
		//the system image list is shared across processes
		m_SmallImageList.Detach();
	}

	if(m_nNormalRefCount == 0)
	{
		m_NormalImageList.Detach();
	}
}

CImageList& CSystemImageList::GetSmallImageList()
{
	return m_SmallImageList;
}

CImageList& CSystemImageList::GetNormalImageList()
{
	return m_NormalImageList;
}
