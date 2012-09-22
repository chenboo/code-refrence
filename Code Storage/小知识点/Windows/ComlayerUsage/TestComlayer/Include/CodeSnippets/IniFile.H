#ifndef __INIFILE_H__
#define __INIFILE_H__

class CIniFile
{
public:
	CIniFile(LPCTSTR lpszFileName)
	{
		m_strFileName = lpszFileName;
	}

	CString ReadString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszDefault);
	BOOL WriteString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszValue);
	BOOL ReadSections(CStringArray& m_strArraySecs);
	BOOL ReadSection(LPCTSTR lpszSection, CStringArray& m_strArrayKeys);

protected:
	CString m_strFileName;
};

#endif //__INIFILE_H__