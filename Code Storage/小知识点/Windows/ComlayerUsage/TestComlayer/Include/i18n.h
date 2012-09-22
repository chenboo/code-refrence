#pragma once

#define ELC_PRODUCT_NAME				_T("Classroom Management by Mythware")
#define DEFAULT_PRODUCT_REGISTRY_KEY	_T("Topdomain\\e-Learning Class V6.0")

#define ELC_FOLDER_CONFIGURATION		_T("Configuration")
#define ELC_FOLDER_CLASS_MODEL			_T("Class Data")
#define ELC_FOLDER_CHAT					_T("Chat History")
#define ELC_FOLDER_LOG					_T("Message History")
#define ELC_FOLDER_QUIZ					_T("Quiz")
#define ELC_FOLDER_RECEIVED_FILES		_T("Received Files")
#define ELC_FOLDER_RECORDED_FILES		_T("Recorded Files")
#define ELC_FOLDER_SNAPSHOTS			_T("Snapshots")
#define ELC_FOLDER_SHARD_BOARD			_T("Share Board")

inline BOOL SetRegValueDwordLocalHost(LPCTSTR lpszKey, LPCTSTR lpszName, DWORD dwValue)
{
	HKEY hKey = NULL;

	do
	{
		LONG lRet = RegCreateKey(HKEY_LOCAL_MACHINE, lpszKey, &hKey);
		if (ERROR_SUCCESS != lRet) {
			break;
		}

		lRet = RegSetValueEx(hKey, lpszName, 0, REG_DWORD, (PBYTE)&dwValue, sizeof(dwValue));
		if (ERROR_SUCCESS != lRet) {
			break;
		}

		RegCloseKey(hKey);

		return TRUE;
	} while (FALSE);

	if (hKey) {
		RegCloseKey(hKey);
	}

	return FALSE;
}

inline BOOL SetRegValueStringLocalHost(LPCTSTR lpszKey, LPCTSTR lpszName, CString strValue)
{
	HKEY hKey = NULL;

	do
	{
		LONG lRet = RegCreateKey(HKEY_LOCAL_MACHINE, lpszKey, &hKey);
		if (ERROR_SUCCESS != lRet) {
			break;
		}

		lRet = RegSetValueEx(hKey, lpszName, 0, REG_SZ, (const BYTE*)(LPCTSTR)strValue, (strValue.GetLength()+1) * sizeof(TCHAR));
		if (ERROR_SUCCESS != lRet) {
			break;
		}

		RegCloseKey(hKey);

		return TRUE;
	} while (FALSE);

	if (hKey) {
		RegCloseKey(hKey);
	}

	return FALSE;
}

inline BOOL SetRegValueBinaryLocalHost(LPCTSTR lpszKey, LPCTSTR lpszName, const BYTE * pData, UINT cbData)
{
	HKEY hKey = NULL;

	do
	{
		LONG lRet = RegCreateKey(HKEY_LOCAL_MACHINE, lpszKey, &hKey);
		if (ERROR_SUCCESS != lRet) {
			break;
		}

		lRet = RegSetValueEx(hKey, lpszName, 0, REG_BINARY, pData, cbData);
		if (ERROR_SUCCESS != lRet) {
			break;
		}

		RegCloseKey(hKey);

		return TRUE;
	} while (FALSE);

	if (hKey) {
		RegCloseKey(hKey);
	}

	return FALSE;
}

inline DWORD GetRegValueDwordLocalHost(LPCTSTR lpszKey, LPCTSTR lpszName)
{
	DWORD dwValue = (DWORD)-1;

	HKEY hKey;
	LONG lRet = RegOpenKey(HKEY_LOCAL_MACHINE, lpszKey, &hKey);
	if (ERROR_SUCCESS != lRet) {
		return dwValue;
	}

	DWORD dwType = 0;
	DWORD cbData = sizeof(DWORD);
    RegQueryValueEx(hKey, lpszName, NULL, &dwType, (PBYTE)&dwValue, &cbData);
	RegCloseKey(hKey);

	return dwValue;
}

inline CString GetRegValueStringLocalHost(LPCTSTR lpszKey, LPCTSTR lpszName)
{
	CString strValue;

	TCHAR szValue[1024] = {0};
	HKEY hKey;
	LONG lRet = RegOpenKey(HKEY_LOCAL_MACHINE, lpszKey, &hKey);
	if (ERROR_SUCCESS != lRet) {
		return strValue;
	}

	DWORD dwType = 0;
	DWORD cbData = sizeof(szValue);
    RegQueryValueEx(hKey, lpszName, NULL, &dwType, (PBYTE)szValue, &cbData);
	RegCloseKey(hKey);

	strValue = szValue;

	return strValue;
}

// GetRegValueBinaryLocalHost allocates a buffer and returns its address in *ppData. 
// The caller is responsible for freeing the buffer using delete [].
inline BOOL GetRegValueBinaryLocalHost(LPCTSTR lpszKey, LPCTSTR lpszName, LPBYTE& ppData, LPDWORD pBytes)
{
	HKEY hKey;
	LONG lRet = RegOpenKey(HKEY_LOCAL_MACHINE, lpszKey, &hKey);
	if (ERROR_SUCCESS != lRet) {
		return FALSE;
	}

	*pBytes = 0;
	ppData = NULL;
	DWORD dwType = 0;
	RegQueryValueEx(hKey, lpszName, NULL, &dwType, NULL, pBytes);
	if (*pBytes > 0) {
		ppData = new BYTE[*pBytes];
		RegQueryValueEx(hKey, lpszName, NULL, &dwType, ppData, pBytes);
	}
	RegCloseKey(hKey);

	return (*pBytes > 0);
}

inline BOOL CreateMultilayerDir(CString strPath)
{
	if (_taccess(strPath, 0) == 0)
		return TRUE;

	TCHAR szTemp[_MAX_PATH] = {0};
	int nPos = strPath.Find(_T('\\'));
	while (nPos != -1)
	{
		_tcsncpy(szTemp, strPath, nPos + 1);
		szTemp[nPos + 1] = _T('\0');
		if (_taccess(szTemp, 00) == -1) {
			if (!CreateDirectory(szTemp, NULL))
				return FALSE;
		}

		nPos = strPath.Find(_T('\\'), nPos + 1);
	}

	if (_taccess(strPath, 0) == -1) {
		if (!CreateDirectory(strPath, NULL))
			return FALSE;
	}

	return TRUE;
}

inline BOOL GetApplicationDirectory(CString& strPath)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);

	TCHAR szDrive[_MAX_DRIVE] = { 0 };
	TCHAR szDir[_MAX_DIR] = { 0 };
	_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
	_tmakepath(szPath, szDrive, szDir, NULL, NULL);

	strPath = szPath;

	return TRUE;
}

inline CString GetOEMStringByOemFile(CString strOemFile, LPCTSTR lpszKey)
{
	TCHAR szResult[512] = { 0 };

	if (_tcsicmp(lpszKey, _T("web")) == 0
		|| _tcsicmp(lpszKey, _T("cmdlinemode")) == 0
		|| _tcsicmp(lpszKey, _T("RegistryKey")) == 0
		|| _tcsicmp(lpszKey, _T("Mirror")) == 0
		|| _tcsicmp(lpszKey, _T("ProductGuid")) == 0
		|| _tcsicmp(lpszKey, _T("Title")) == 0
		|| _tcsicmp(lpszKey, _T("ShortTitle")) == 0)
	{
		GetPrivateProfileString(_T("Common"), lpszKey, _T(""), szResult, _countof(szResult), strOemFile);
	}
	else if (_tcsicmp(lpszKey, _T("Name")) == 0
		|| _tcsicmp(lpszKey, _T("Version")) == 0
		|| _tcsicmp(lpszKey, _T("Company")) == 0) 
	{
		TCHAR szLang[256] = { 0 };
		GetPrivateProfileString(_T("Common"), _T("language"), _T("unknow"), szLang, _countof(szLang), strOemFile);
		GetPrivateProfileString(szLang, lpszKey, _T(""), szResult, _countof(szResult), strOemFile);
	}

	return szResult;
}

inline CString GetOEMString(LPCTSTR lpszKey)
{
	CString strAppPath;
	GetApplicationDirectory(strAppPath);
	if (strAppPath.Right(1) != _T('\\'))
		strAppPath += _T("\\");
	strAppPath += _T("TDUninst.dat");

	return GetOEMStringByOemFile(strAppPath, lpszKey);
}

inline CString GetOEMRegistryKey()
{
	CString strRegistryKey = GetOEMString(_T("RegistryKey"));
	if (strRegistryKey.IsEmpty())
		strRegistryKey = DEFAULT_PRODUCT_REGISTRY_KEY;

	return strRegistryKey;
}

inline BOOL Check_4_BytesCharacterSupport(BOOL &bSupport)
{
	CString strSubkey;
	strSubkey.Format(_T("Software\\%s"), GetOEMRegistryKey());

	HKEY hKey = NULL;
	LONG lRet = RegOpenKey(HKEY_LOCAL_MACHINE, strSubkey, &hKey);
	if (ERROR_SUCCESS != lRet)
		return FALSE;

	DWORD dwValue, dwType, dwLength = sizeof(DWORD);
	lRet = RegQueryValueEx(hKey, _T("Enable4BytesCharacterSupport"), NULL, &dwType, (LPBYTE)&dwValue, &dwLength);
	RegCloseKey(hKey);

	if (ERROR_SUCCESS != lRet)
		return FALSE;
	
	bSupport = dwValue;
	return TRUE;
}

inline DWORD GetLangID()
{
	HKEY hKey = NULL;
	LONG lRet = RegOpenKey(HKEY_LOCAL_MACHINE, _T("Software\\TopDomain\\e-Learning Class V6.0"), &hKey);
	if (ERROR_SUCCESS != lRet)
		return ~0;

	DWORD dwValue, dwType, dwLength = sizeof(DWORD);
	lRet = RegQueryValueEx(hKey, _T("LanguageID"), NULL, &dwType, (LPBYTE)&dwValue, &dwLength);
	RegCloseKey(hKey);

	if (ERROR_SUCCESS != lRet)
		return ~0;
	else
		return dwValue;
}

inline BOOL GetL10NDirectory(CString& strPath)
{
	CString strAppPath;
	GetApplicationDirectory(strAppPath);
	if (strAppPath.Right(1) != _T('\\'))
		strAppPath += _T("\\");

	LANGID LangID = (LANGID)GetLangID();
	if (LangID == (WORD)~0) {
		strPath = _T("");
		return FALSE;
		//LangID = GetSystemDefaultLangID();
	}
	
	strPath.Format(_T("%sLanguage\\%04d\\"), strAppPath, LangID);
	return TRUE;
}

inline BOOL GetL10NFilePath(LPCTSTR lpszFileName, CString& strPath)
{
	if (!GetL10NDirectory(strPath))
		return FALSE;

	strPath += (CString)lpszFileName;

	return TRUE;
}

inline HINSTANCE LoadResourceLibrary(LPCTSTR lpszModuleName)
{
	CString strLanguagePath;
	if (!GetL10NDirectory(strLanguagePath))
		return NULL;
	
	strLanguagePath += (CString)lpszModuleName;
	
	return AfxLoadLibrary(strLanguagePath);
}

inline BOOL GetMydocumentDirectory(CString &strPath)
{
	TCHAR szDocument[MAX_PATH] = { 0 };
	LPITEMIDLIST pidl=NULL;   
	if (FAILED(SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &pidl)))
		return FALSE;

	if (pidl && SHGetPathFromIDList(pidl, szDocument)) {
//		GetShortPathName(szDocument, strPath.GetBuffer(MAX_PATH), MAX_PATH);
//		strPath.ReleaseBuffer();
		strPath = szDocument;
		return TRUE;
	}
	
	return FALSE;
}

inline CString GetLoginUser()
{
	return GetRegValueStringLocalHost(_T("Software\\TopDomain\\e-Learning Class\\Teacher"), _T("LoginUser"));
}

inline CString GetLoginClassModel()
{
	return GetRegValueStringLocalHost(_T("Software\\TopDomain\\e-Learning Class\\Teacher"), _T("LoginClassModel"));
}

inline CString GetUsersRootPath()
{
	CString strPath;
	if (GetMydocumentDirectory(strPath)) {
		strPath += _T("\\");
		strPath += ELC_PRODUCT_NAME;
	}

	return strPath;
}

inline CString GetUserFilePath(LPCTSTR lpszFileName, BOOL bTeacherSide = TRUE)
{
	CString strPath = GetUsersRootPath();

	if (bTeacherSide) {
		CString strUserName = GetLoginUser();
		if (!strUserName.IsEmpty()) {
			strPath += _T("\\");
			strPath += strUserName;
		}
	}

	if (lpszFileName) {
		if (L'\\' != lpszFileName[0])
			strPath += _T("\\");
		strPath += (CString)lpszFileName;
	}

	return strPath;
}

inline BOOL GetUserFilePath(LPCTSTR lpszFileName, CString& strPath, BOOL bTeacherSide = TRUE)
{
	strPath = GetUserFilePath(lpszFileName, bTeacherSide);
	return TRUE;
}

inline BOOL OpenIndexHelp(CString strPath, int nIndex)
{
	strPath.MakeLower();
	if (strPath.Find(_T(".chm")) != -1) {
		if (nIndex == 0)
			::HtmlHelp(NULL, strPath, HH_DISPLAY_TOPIC, nIndex);
		else
			::HtmlHelp(NULL, strPath, HH_HELP_CONTEXT, nIndex);
	}
	return TRUE;
}

inline BOOL ShellOpenFile(LPCTSTR lpszPath)
{
	HINSTANCE hInstance = ShellExecute(NULL, _T("open"), lpszPath, NULL, NULL, SW_SHOWNORMAL);
	if (hInstance <= (HINSTANCE)0x00000020) {
		CString strCmdLine;
		strCmdLine.Format(_T("shell32,OpenAs_RunDLL \"%s\""), lpszPath);
		ShellExecute(NULL, _T("open"), _T("rundll32"), strCmdLine, NULL, SW_SHOWNORMAL);
	}
	return TRUE;
}

inline BOOL OpenTeacherHelp(int nIndex)
{
	CString strPath;
	GetL10NFilePath(_T("ELCStand.chm"), strPath);
	if (_taccess(strPath, 0) == 0)
		return OpenIndexHelp(strPath, nIndex);
	
	GetL10NFilePath(_T("ELCStand.pdf"), strPath);
	if (_taccess(strPath, 0) == 0) 
		return ShellOpenFile(strPath);
	
	return FALSE;
}

inline BOOL OpenStudentHelp(int nIndex)
{
	CString strPath;
	GetL10NFilePath(_T("eLCStandStud.chm"), strPath);
	if (_taccess(strPath, 0) == 0) 
		return OpenIndexHelp(strPath, nIndex);
	
	GetL10NFilePath(_T("eLCStandStud.pdf"), strPath);
	if (_taccess(strPath, 0) == 0) 
		return ShellOpenFile(strPath);

	return FALSE;
}

inline BOOL IsWindowsVistaLater()
{
	OSVERSIONINFO osVerInfo;
	
	ZeroMemory(&osVerInfo, sizeof(osVerInfo));
	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	GetVersionEx(&osVerInfo);

	return (osVerInfo.dwMajorVersion >= 6);
}

inline BOOL IsWindowsVista()
{
	OSVERSIONINFO osVerInfo;
	
	ZeroMemory(&osVerInfo, sizeof(osVerInfo));
	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	GetVersionEx(&osVerInfo);

	return (osVerInfo.dwMajorVersion == 6 && osVerInfo.dwMinorVersion == 0);
}

inline BOOL IsWindowsWin7()
{
	OSVERSIONINFO osVerInfo;
	
	ZeroMemory(&osVerInfo, sizeof(osVerInfo));
	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	GetVersionEx(&osVerInfo);

	return (osVerInfo.dwMajorVersion == 6 && osVerInfo.dwMinorVersion == 1);
}

inline DWORD GetWinMajorVersion()
{
	OSVERSIONINFO osVerInfo;
	ZeroMemory(&osVerInfo, sizeof(osVerInfo));
	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	GetVersionEx(&osVerInfo);

	return osVerInfo.dwMajorVersion;
}

inline void GetWindowsVersion(DWORD& dwMajorVer, DWORD& dwMinorVer)
{
	OSVERSIONINFO osVerInfo;
	ZeroMemory(&osVerInfo, sizeof(osVerInfo));
	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	GetVersionEx(&osVerInfo);

	dwMajorVer = osVerInfo.dwMajorVersion;
	dwMinorVer = osVerInfo.dwMinorVersion;
}

inline BOOL GetProductVersion(TCHAR* pszVer, int* pnVersion)
{
	if (pszVer == NULL)
		return FALSE;

	TCHAR* pHead = pszVer;
	TCHAR* pFind = pszVer;

	int nIndex = 0;
	int nCount = 0;

	while (nIndex < 4) 
	{
		if (*pFind == _T('.'))
		{
			if (nCount == 0)
				pnVersion[nIndex] = 0;
			else {
				TCHAR* pTmp = new TCHAR[nCount + 1];
				memset(pTmp, 0, (nCount + 1) * sizeof(TCHAR));
				_tcsncpy(pTmp, pHead, nCount);
				pnVersion[nIndex] = _ttoi(pTmp);
				delete []pTmp;
			}

			nCount = 0;
			nIndex ++;
			pFind ++;
			pHead = pFind;

			continue;
		}
		if (*pFind == _T('\0')) {
			if (nIndex != 3)
				return FALSE;

			if (nCount == 0)
				pnVersion[nIndex] = 0;
			else {
				TCHAR* pTmp = new TCHAR[nCount + 1];
				memset(pTmp, 0, (nCount + 1) * sizeof(TCHAR));
				_tcsncpy(pTmp, pHead, nCount);
				pnVersion[nIndex] = _ttoi(pTmp);
				delete []pTmp;
			}

			break;
		}

		pFind ++;
		nCount ++;
	}

	return TRUE;
}

inline void ParseDWordVersion(DWORD dwVersion, int* pnVersion)
{
	pnVersion[0] = dwVersion >> 28;
	pnVersion[1] = (dwVersion & 0x0F000000) >> 24;
	pnVersion[2] = (dwVersion & 0x00FFC000) >> 14;
	pnVersion[3] = (dwVersion & 0x00003FFF);
}

inline BOOL GetElcVersionString(CString& strVersion)
{
	CString strAppPath;
	GetApplicationDirectory(strAppPath);
	if (strAppPath.Right(1) != _T('\\'))
		strAppPath += _T("\\");
	strAppPath += _T("TDUninst.dat");

	if (_taccess(strAppPath, 0) != 0)
		return FALSE;

	TCHAR szVer[32] = {_T('\0')};
	int cchVer = sizeof(szVer);
	GetPrivateProfileString(_T("Common"), _T("Build"), NULL, szVer, cchVer, strAppPath);

	strVersion = szVer;
	return TRUE;
}

/********************************************************************************
e-Learning Classs Software version:

Hight								(32-bits)    							Low
----------------   ----------------   ------------------   ---------------------
| Major (4-bits) |  Minor (4-bits)  |  Release (10-bits) |  build No.(14-bits) |
----------------   ----------------   ------------------   ---------------------

********************************************************************************/

inline DWORD GetElcVersion()
{
	DWORD dwVersion = 0;

	CString strAppPath;
	GetApplicationDirectory(strAppPath);
	if (strAppPath.Right(1) != _T('\\'))
		strAppPath += _T("\\");
	strAppPath += _T("TDUninst.dat");

	if (_taccess(strAppPath, 0) != 0)
		return dwVersion;

	TCHAR szVer[32] = {_T('\0')};
	int cchVer = sizeof(szVer);
	GetPrivateProfileString(_T("Common"), _T("Build"), NULL, szVer, cchVer, strAppPath);

	int nVersion[4];
	GetProductVersion(szVer, nVersion);

	dwVersion = (nVersion[0] << 28) + (nVersion[1] << 24) + ((nVersion[2] << 14)) + nVersion[3];

	return dwVersion;
}

inline DWORD GetMajorVersion(DWORD dwVersion)
{
	return (DWORD)((dwVersion & 0xF0000000) >> 28);
}

inline DWORD GetMinorVersion(DWORD dwVersion)
{
	return (DWORD)((dwVersion & 0x0F000000) >> 24);
}

inline DWORD GetReleaseNumber(DWORD dwVersion)
{
	return (DWORD)((dwVersion & 0x00FFC000) >> 14);
}

inline DWORD GetBuildNumber(DWORD dwVersion)
{
	return (DWORD)(dwVersion & 0x00003FFF);
}

/************************************************************
* Compare elc version function
* if dwVer1 > dwVer2, return more than 0
* if dwVer1 < dwVer2, return less than 0
* if dwVer1 = dwVer2, return 0
************************************************************/
inline int CompareElcVersion(DWORD dwVer1, DWORD dwVer2)
{
	if (dwVer1 == dwVer2)
		return 0;
	else if (dwVer1 > dwVer2)
		return 1;
	else if (dwVer1 < dwVer2)
		return -1;
}

inline void SetWindowToMonitor(HWND hWnd, HMONITOR hMonitor)
{
	if (!hMonitor)
		return;

	if (!::IsWindow(hWnd))
		return;

    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(hMonitor, &mi))
		return;

	CRect rcMonitor(mi.rcMonitor);

	CRect rc;
	::GetWindowRect(hWnd, rc);

	int nLeft = rc.left + rcMonitor.left;
	int nTop = rc.top;

	::SetWindowPos(hWnd, NULL, nLeft, nTop, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
}

/****************************************************
*
* Check OS if is windows 64 bits platform
*
****************************************************/
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

inline BOOL IsWindows64Bits()
{
	OSVERSIONINFO osVerInfo;
	ZeroMemory(&osVerInfo, sizeof(osVerInfo));
	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	GetVersionEx(&osVerInfo);

	if (osVerInfo.dwMajorVersion == 5 && osVerInfo.dwMinorVersion == 0)
		return FALSE;
	
	BOOL bIsWow64 = FALSE;

	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandle(_T("kernel32")),"IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            // handle error
			return FALSE;
        }		
	}

	return bIsWow64;
}

inline UINT ElcMessageBox(UINT uIdText, UINT nType = MB_OK, HWND hWnd = NULL)
{
	CString strText;
	strText.LoadString(uIdText);

	DWORD dwLayout = 0;
	GetProcessDefaultLayout(&dwLayout);
	if (dwLayout == LAYOUT_RTL) {
		nType |= MB_RTLREADING | MB_RIGHT;
	}

	return MessageBox(hWnd, strText, GetOEMString(_T("Title")), nType);
}

inline UINT ElcMessageBox(CString strText, UINT nType = MB_OK, HWND hWnd = NULL)
{
	DWORD dwLayout = 0;
	GetProcessDefaultLayout(&dwLayout);
	if (dwLayout == LAYOUT_RTL) {
		nType |= MB_RTLREADING | MB_RIGHT;
	}

	return MessageBox(hWnd, strText, GetOEMString(_T("Title")), nType);
}

inline void ElcFormatMessage(DWORD dwErrorCode, HWND hWnd = NULL)
{
	TCHAR* buffer = NULL;
	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwErrorCode,
		0,
		(LPTSTR)&buffer,
		0,
		NULL );

	ElcMessageBox(buffer, MB_OK|MB_ICONERROR, hWnd);
	LocalFree(buffer);
}