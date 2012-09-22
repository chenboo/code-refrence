Image* LoadImage(UINT nID, LPCTSTR strType)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource(hInst,MAKEINTRESOURCE(nID),strType);
	if (hRsrc == NULL)
	{
		return FALSE;
	}

	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (lpRsrc == NULL)
	{
		return FALSE;
	}

	// Allocate global memory on which to create stream
	HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(hMem);
	memcpy(pmem,lpRsrc,len);
	IStream* pstm;
	CreateStreamOnHGlobal(hMem,FALSE,&pstm);

	// load from stream
	Image* pImage = Image::FromStream(pstm);

	// free/release stuff
	pstm->Release();
	FreeResource(lpRsrc);

	return pImage;
}

//调用这个函数时，HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, len);
//这句话会分配内存，如果不释放会产生内存泄漏。

//释放内存的调用为：
	GlobalUnlock(hMem);
	GlobalFree(hMem);
//但这两句话不应该在pImage有效时调用，即调用这两句话之后，pImage所对应的
//的图片就失效了，因此应该将hMem定义为一个成员变量m_hBuffer，在程序对pIamge
//不再使用时调用释放内存的操作。

	::GlobalUnlock(m_hBuffer);
	::GlobalFree(m_hBuffer);
	m_hBuffer = NULL;