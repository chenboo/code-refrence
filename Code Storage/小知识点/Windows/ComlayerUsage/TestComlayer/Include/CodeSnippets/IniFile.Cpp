#include "stdafx.h"
#include "IniFile.h"

CString CIniFile::ReadString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszDefault)
{
	TCHAR szBuffer[2048];

	szBuffer[0] = '\0';
	GetPrivateProfileString(lpszSectionName, lpszKeyName, lpszDefault, szBuffer, sizeof(szBuffer) / sizeof(TCHAR), m_strFileName);
	return szBuffer;
}

BOOL CIniFile::WriteString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszValue)
{
	return WritePrivateProfileString(lpszSectionName, lpszKeyName, lpszValue, m_strFileName);
}

BOOL CIniFile::ReadSections(CStringArray& m_strArraySecs)
{
	LPVOID pvData = NULL;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, 32770); //16385 * 2

	pvData = GlobalLock(hGlobal);

	m_strArraySecs.RemoveAll();
	
	if (GetPrivateProfileString(NULL, NULL, NULL, (TCHAR*)pvData, 16384, m_strFileName))
	{
        TCHAR *pSection = (TCHAR*)pvData;
        while (*pSection != 0)
		{
			m_strArraySecs.Add(pSection);
			pSection += _tcslen(pSection) + 1;
		}
	}

	GlobalUnlock(hGlobal);
	GlobalFree(hGlobal);

	return (m_strArraySecs.GetSize() > 0);
}

BOOL CIniFile::ReadSection(LPCTSTR lpszSection, CStringArray& m_strArrayKeys)
{
	LPVOID pvData = NULL;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, 16385);

	pvData = GlobalLock(hGlobal);

	m_strArrayKeys.RemoveAll();
	
	if (GetPrivateProfileString(lpszSection, NULL, NULL, (TCHAR*)pvData, 16384, m_strFileName))
	{
        TCHAR *pKey = (TCHAR*)pvData;
        while (*pKey != 0)
		{
			m_strArrayKeys.Add(pKey);
			pKey += _tcslen(pKey) + 1;
		}
	}

	GlobalUnlock(hGlobal);
	GlobalFree(hGlobal);

	return m_strArrayKeys.GetSize() > 0;
}
