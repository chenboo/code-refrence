// Convert the char* to CString or TCHAR;
//CP_ACP can be replaced by CP_UTF8 or other codepage depend on the request;
//--------------------------------------------------------------------------
	char* pbuffer = "peng shui ming";

	DWORD dwCount = MultiByteToWideChar(CP_ACP, 0, pbuffer, -1, 0, 0 );
	TCHAR* pch = new TCHAR[dwCount];
	memset(pch, 0, dwCount * sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, 0, pbuffer, -1, pch, nCount);

	CString str = pch;
	
	//OR
	//filename is type of char*
	
	int cchfilename = MultiByteToWideChar( CP_UTF8, 0, filename, -1, 0, 0 );
	wchar_t* pfilename = new wchar_t[cchfilename];
	memset( pfilename, 0, cchfilename * sizeof(wchar_t) );
	MultiByteToWideChar( CP_UTF8, 0, filename, -1, pfilename, cchfilename );
	
	
//Convert the CString to char*
//----------------------------------------------------------------------------
	CString str = _T("peng shui ming");
	DWORD dwNum = WideCharToMultiByte(CP_UTF8,NULL,str,-1,NULL,0,NULL,NULL);
	char* pch = new char[dwNum];
	WideCharToMultiByte(CP_UTF8,NULL,str,-1,pch,dwNum,NULL,NULL);