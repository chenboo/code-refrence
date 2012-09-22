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

//�����������ʱ��HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, len);
//��仰������ڴ棬������ͷŻ�����ڴ�й©��

//�ͷ��ڴ�ĵ���Ϊ��
	GlobalUnlock(hMem);
	GlobalFree(hMem);
//�������仰��Ӧ����pImage��Чʱ���ã������������仰֮��pImage����Ӧ��
//��ͼƬ��ʧЧ�ˣ����Ӧ�ý�hMem����Ϊһ����Ա����m_hBuffer���ڳ����pIamge
//����ʹ��ʱ�����ͷ��ڴ�Ĳ�����

	::GlobalUnlock(m_hBuffer);
	::GlobalFree(m_hBuffer);
	m_hBuffer = NULL;