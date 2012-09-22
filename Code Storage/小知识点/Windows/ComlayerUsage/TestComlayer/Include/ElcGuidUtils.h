#ifndef __ELC_GUID_UTILS_H__
#define __ELC_GUID_UTILS_H__

inline BOOL ElcGuid2String(const GUID* pGuid, CString& string)
{
	ASSERT(pGuid);
	if (pGuid == NULL)
		return FALSE;

	string.Format(_T("{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		pGuid->Data1, 
		pGuid->Data2, 
		pGuid->Data3, 
		pGuid->Data4[0], pGuid->Data4[1], 
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4], pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]); 

	return TRUE;
}

#define NUM_W2A(w, n, number) { for (int i=0; i < n; i++) { number += _ttoi(&w[i]) << (i * 4); } }

inline BOOL ElcString2Guid(GUID& guid, LPCTSTR lpszString)
{
	ASSERT(lpszString);
	if (lpszString == NULL)
		return FALSE;

	if (_tcslen(lpszString) != 38)
		return FALSE;

	LPCTSTR pTemp = lpszString + 1;
	TCHAR szData[9] = { 0 };
	

	_tcsncpy(szData, pTemp, 8);
	if (_stscanf(szData, _T("%x"), &guid.Data1) == EOF)
		return FALSE;

	pTemp += 9;
	memset(szData, 0, sizeof(szData));
	_tcsncpy(szData, pTemp, 4);
	if (_stscanf(szData, _T("%x"), &guid.Data2) == EOF)
		return FALSE;

	pTemp += 5;
	memset(szData, 0, sizeof(szData));
	_tcsncpy(szData, pTemp, 4);
	if (_stscanf(szData, _T("%x"), &guid.Data3) == EOF)
		return FALSE;

	pTemp += 5;
	for (int i=0; i < 8; i++)
	{
		int nTemp = 0;
		TCHAR ccTemp[3] = { 0 };
		_tcsncpy(ccTemp, pTemp, 2);
		_stscanf(ccTemp, _T("%x"), &nTemp);
		
		guid.Data4[i] = (char)nTemp;

		pTemp += 2;
		if (i == 1)
			pTemp += 1;
	}

	return TRUE;
}


#endif //__ELC_GUID_UTILS_H__