void CDialogDlg::GetLogicalDrives(HTREEITEM hParent)
{
	//get the length of drive strings not include the last terminating null character
	int nBufferLength = GetLogicalDriveStrings(0, NULL);

	//the size must plus the last terminating null character
	TCHAR* pBuffer = new TCHAR[nBufferLength + 1];
	
	//Get the actually DriveString
	GetLogicalDriveStrings(nBufferLength, pBuffer);

	TCHAR* pBegin = pBuffer;
	size_t nLength = _tcslen(pBuffer);

	while(nLength > 0)
	{
		TRACE(_T("%s\n"), pBuffer);
		pBuffer += nLength + 1;
		nLength = _tcslen(pBuffer);
	}
		
	delete [] pBegin;
	pBegin = NULL;
}

//Out put:
//C:\
//D:\
//E:\
//F:\
//G:\
//H:\