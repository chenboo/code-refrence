#include "stdafx.h"
#include "CodeSnippets/SystemDirectory.h"

CSystemDirectoryNet::CSystemDirectoryNet()
{
}

CSystemDirectoryNet::~CSystemDirectoryNet()
{
}

WORD CSystemDirectoryNet::GetSysDirID(CString strName)
{
	if (strName == _T("<WINDIR>"))
		return 0x0001;
	else if (strName == _T("<WINSYSDIR>"))
		return 0x0002;
	else if (strName == _T("<WINTEMPDIR>"))
		return 0x0003;
	else if (strName == _T("<FOLDER_PROGRAMS>"))
		return 0x0004;
	else if (strName == _T("<FOLDER_COMMON_FILES>"))
		return 0x0005;
	else if (strName == _T("<FOLDER_INSTALL>"))
		return 0x0006;
	else if (strName == _T("<FOLDER_MYDOCUMNETS>"))
		return 0x0007;
	else if (strName == _T("<FOLDER_DESKTOP>"))
		return 0x0008;
	else if (strName == _T("<FLODER_STARTMEMU>"))
		return 0x0009;
	else if (strName == _T("<FOLDER_STARTUP>"))
		return 0x000a;

	return 0x0000;
}

CString CSystemDirectoryNet::GetSysDirName(WORD wSysDirID)
{
	if (wSysDirID == 0x0001)
		return _T("<WINDIR>");
	else if (wSysDirID == 0x0002)
		return _T("<WINSYSDIR>");
	else if (wSysDirID == 0x0003)
		return _T("<WINTEMPDIR>");
	else if (wSysDirID == 0x0004)
		return _T("<FOLDER_PROGRAMS>");
	else if (wSysDirID == 0x0005)
		return _T("<FOLDER_COMMON_FILES>");
	else if (wSysDirID == 0x0006)
		return _T("<FOLDER_INSTALL>");
	else if (wSysDirID == 0x0007)
		return _T("<FOLDER_MYDOCUMNETS>");
	else if (wSysDirID == 0x0008)
		return _T("<FOLDER_DESKTOP>");
	else if (wSysDirID == 0x0009)
		return _T("<FLODER_STARTMEMU>");
	else if (wSysDirID == 0x000a)
		return _T("<FOLDER_STARTUP>");

	return _T("");
}

CString CSystemDirectoryNet::GetSysDirValueByID(WORD wSysDirID)
{	
	CString strSysDirValue;
	
	switch (wSysDirID)
	{
	case 0x0001:
		strSysDirValue = Get_WinDir();
		break;
	case 0x0002:
		strSysDirValue = Get_WinSysDir();
		break;
	case 0x0003:
		strSysDirValue = Get_WinTempDir();
		break;
	case 0x0004:
		strSysDirValue = Get_ProgramsDir();
		break;
	case 0x0005:
		strSysDirValue = Get_CommonfilesDir();
		break;
	case 0x0006:
		strSysDirValue = Get_InstallDir();
		break;
	case 0x0007:
		strSysDirValue = Get_MydocumnetsDir();
		break;
	case 0x0008:
		strSysDirValue = Get_DesktopDir();
		break;
	case 0x0009:
		strSysDirValue = Get_StartmenuDir();
		break;
	case 0x000a:
		strSysDirValue = Get_Startup();
		break;
	}

	return strSysDirValue;
}

CString CSystemDirectoryNet::GetSysDirValueByName(CString strName)
{
	if (strName == _T("<WINDIR>"))
		return Get_WinDir();
	if (strName == _T("<WINSYSDIR>"))
		return Get_WinSysDir();
	if (strName == _T("<WINTEMPDIR>"))
		return Get_WinTempDir();
	if (strName == _T("<FOLDER_DESKTOP>"))
		return Get_DesktopDir();
	if (strName == _T("<FOLDER_PROGRAMS>"))
		return Get_ProgramsDir();
	if (strName == _T("<FOLDER_MYDOCUMNETS>"))
		return Get_MydocumnetsDir();
	if (strName == _T("<FLODER_STARTMEMU>"))
		return Get_StartmenuDir();
	if (strName == _T("<FOLDER_STARTUP>"))
		return Get_Startup();
	if (strName == _T("<FOLDER_COMMON_FILES>"))
		return Get_CommonfilesDir();
	if (strName == _T("<FOLDER_INSTALL>"))
		return Get_InstallDir();

	return _T("");
}

CString CSystemDirectoryNet::TranslateSystemPath(CString strPath)
{
	CString strPathName = strPath;

	if (strPathName.GetAt(0) == '<')
	{
		int iIndex = strPathName.Find('>', 0);
		if (iIndex != -1)
		{
			CString strMacroDir = strPathName.Left(iIndex + 1);
			if (IsSystemDefinedPath(strMacroDir))
			{
				CString strValue = GetSysDirValueByName(strMacroDir);
				if (strValue.Right(1) == _T("\\"))
					strValue.Delete(strValue.GetLength() - 1, 1);
				strPathName.Replace(strMacroDir, strValue);
			}
		}
	}

	return strPathName;
}

CString CSystemDirectoryNet::Get_WinDir()
{
	TCHAR szPathName[MAX_PATH] = {0};
	GetWindowsDirectory(szPathName, MAX_PATH);
	return szPathName;
}

CString CSystemDirectoryNet::Get_WinSysDir()
{
	TCHAR szPathName[MAX_PATH] = {0};
	GetSystemDirectory(szPathName, MAX_PATH);
	return szPathName;
}

CString CSystemDirectoryNet::Get_WinTempDir()
{
	TCHAR szPathName[MAX_PATH] = {0};
	GetTempPath(MAX_PATH, szPathName);
	return szPathName;
}

CString CSystemDirectoryNet::Get_DesktopDir()
{
	CString strDesktop = GetShellFolders(_T("Desktop"));

	if (strDesktop.IsEmpty()) {
		strDesktop = Get_WinDir();
		if (strDesktop.Right(1) != _T("\\"))
			strDesktop += _T("\\");
		strDesktop += _T("Desktop");
	}

	return strDesktop;
}

CString CSystemDirectoryNet::Get_ProgramsDir()
{
	CString strProgramFiles = GetCurrentVersonPath(_T("ProgramFilesDir"));
	if (strProgramFiles.IsEmpty()) {
		strProgramFiles = Get_WinDir();
		strProgramFiles = strProgramFiles.Left(2);
		strProgramFiles += _T("\\Program Files");
	}

	return strProgramFiles;
}

CString CSystemDirectoryNet::Get_MydocumnetsDir()
{
	CString strPersonal = GetShellFolders(_T("Personal"));
	
	if (strPersonal.IsEmpty()) {
		strPersonal = Get_WinDir();
		strPersonal = strPersonal.Left(2);
		strPersonal += _T("\\My Documents");
	}

	return strPersonal;
}

CString CSystemDirectoryNet::Get_StartmenuDir()
{
	CString strStartMenu = GetShellFolders(_T("Start Menu"));

	if (strStartMenu.IsEmpty())	{
		strStartMenu = Get_WinDir();
		if (strStartMenu.Right(1) != _T("\\"))
			strStartMenu += _T("\\");
		strStartMenu += _T("Start Menu");
	}

	return strStartMenu;
}

CString CSystemDirectoryNet::Get_Startup()
{
	CString strStartup = GetShellFolders(_T("Startup"));

	if (strStartup.IsEmpty()) {
		strStartup = Get_StartmenuDir();
		if (strStartup.Right(1) != _T("\\"))
			strStartup += _T("\\");
		strStartup += _T("Programs\\Startup");
	}

	return strStartup;
}

CString CSystemDirectoryNet::Get_CommonfilesDir()
{
	CString strCommonFiles = GetCurrentVersonPath(_T("CommonFilesDir"));
	if (strCommonFiles.IsEmpty()) {
		strCommonFiles = Get_WinDir();
		strCommonFiles = strCommonFiles.Left(2);
		strCommonFiles += _T("\\Program Files\\Common Files");
	}

	return strCommonFiles;
}

CString CSystemDirectoryNet::Get_InstallDir()
{
	CString strValue;

	TCHAR szTargetDir[MAX_PATH] = {0};
	GetModuleFileName(NULL, szTargetDir, MAX_PATH);
	TCHAR* ptr = _tcsrchr(szTargetDir, '\\') + 1;
	*ptr = '\0';
	strValue = szTargetDir;

	return strValue;
}

CString CSystemDirectoryNet::GetCurrentVersonPath(CString strKeyName)
{
	CString strValue;
	TCHAR szKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion");

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		DWORD nSize = MAX_PATH;
		DWORD dwType = REG_SZ;
		TCHAR szValue[MAX_PATH] = {0};
		if (RegQueryValueEx(hKey, strKeyName, NULL, &dwType, (LPBYTE)szValue, &nSize) == ERROR_SUCCESS)
			strValue = szValue;

		RegCloseKey(hKey);
	}
	
	return strValue;
}

CString CSystemDirectoryNet::GetShellFolders(CString strKeyName)
{
	CString strValue;
	TCHAR szKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders");

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, szKey, 0, 
		KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {

		DWORD nSize = MAX_PATH;
		DWORD dwType = REG_SZ;
		TCHAR szValue[MAX_PATH] = {0};
		if (RegQueryValueEx(hKey, strKeyName, NULL, &dwType, (LPBYTE)szValue, &nSize) == ERROR_SUCCESS)
			strValue = szValue;

		RegCloseKey(hKey);
	}
	
	return strValue;
}

BOOL CSystemDirectoryNet::IsSystemDefinedPath(CString strName)
{
	if ((strName == _T("<WINDIR>")) ||
		(strName == _T("<WINSYSDIR>")) ||
		(strName == _T("<WINTEMPDIR>")) ||
		(strName == _T("<FOLDER_PROGRAMS>")) ||
		(strName == _T("<FOLDER_COMMON_FILES>")) ||
		(strName == _T("<FOLDER_INSTALL>")) ||
		(strName == _T("<FOLDER_MYDOCUMNETS>")) ||
		(strName == _T("<FOLDER_DESKTOP>")) ||
		(strName == _T("<FLODER_STARTMEMU>")) ||
		(strName == _T("<FOLDER_STARTUP>")))
		return TRUE;

	return FALSE;
}
