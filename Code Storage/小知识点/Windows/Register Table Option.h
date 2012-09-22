1．读注册表

// 读取注册表
CString ReadRegisteTable(CString root,CString path, CString key)
{
  HKEY hAppKey;    
  LPCTSTR WINDS_SERVICE_REGISTRY_KEY=path;
  LPCTSTR DATA_FILE_SUB_KEY=key;
  char szDataFile[80]; 
 if(root=="HKEY_LOCAL_MACHINE")
 {
	//首先打开注册表
  if (ERROR_SUCCESS == RegOpenKeyEx (
    HKEY_LOCAL_MACHINE,
    WINDS_SERVICE_REGISTRY_KEY,
    0,
    KEY_READ,
    &hAppKey))
  {
    ULONG cbSize = MAX_PATH*sizeof(TCHAR);
	//读取注册表的值
    DWORD dwFlag = RegQueryValueEx (
     hAppKey,
     DATA_FILE_SUB_KEY,
     NULL,
     NULL,
     (LPBYTE)szDataFile,&cbSize);
    RegCloseKey (hAppKey);    
    if (ERROR_SUCCESS == dwFlag)   
   {
     CString strDate=szDataFile; //MessageBox(strDate);
     if(strDate.GetLength()>0)
	 {
		return strDate;
	}
     else
	{
		return "";
	}
	
   }
  }

   return "";
 }
 
 if(root=="HKEY_CURRENT_USER")
 {
	//打开注册表
  if (ERROR_SUCCESS == RegOpenKeyEx (
    HKEY_CURRENT_USER,
    WINDS_SERVICE_REGISTRY_KEY,
    0,
    KEY_READ,
    &hAppKey))
  {       
    ULONG cbSize = MAX_PATH*sizeof(TCHAR);
	//读取注册表的值
    DWORD dwFlag = RegQueryValueEx(
     hAppKey,
     DATA_FILE_SUB_KEY,
     NULL,
     NULL,
     (LPBYTE)szDataFile,&cbSize);
    RegCloseKey (hAppKey);    
    if (ERROR_SUCCESS == dwFlag)   
   {        
     CString strDate=szDataFile; //MessageBox(strDate);
     if(strDate.GetLength()>0) return strDate;    
     else return "";
   } 
    return "";
  }   
 }
  return ""; 
}

2．写注册表

// 修改注册表
BOOL ModifyRegisteTable(CString root,CString path, CString key, CString value)
{
    HKEY hAppKey;  
 DWORD dwDisposition = MAX_PATH*sizeof(TCHAR); 
  LPCTSTR WINDS_SERVICE_REGISTRY_KEY=path;
  LPCTSTR DATA_FILE_SUB_KEY=key;
 char szDataFile[80];
 strcpy(szDataFile, value);
 if(root=="HKEY_LOCAL_MACHINE")
 {
	//创建一个注册表
   if (ERROR_SUCCESS != RegCreateKeyEx (
    HKEY_LOCAL_MACHINE,
    WINDS_SERVICE_REGISTRY_KEY,        
    0,                                  
    NULL,                                
    REG_OPTION_NON_VOLATILE,             
    KEY_WRITE,                                   
    NULL,                                    
    &hAppKey,                     
    &dwDisposition))   
  {
    return false;
  }
   else   
  {
	//写注册表
    if (ERROR_SUCCESS != RegSetValueEx (hAppKey,
     DATA_FILE_SUB_KEY,
     0,
     REG_SZ,
     (LPBYTE)szDataFile,
     (lstrlen (szDataFile)+1)*sizeof(TCHAR)))
   {
     return false;
   }
    RegCloseKey (hAppKey); 
  }
 }
 if(root=="HKEY_CURRENT_USER")
 {
   if (ERROR_SUCCESS != RegCreateKeyEx (
    HKEY_CURRENT_USER,
    WINDS_SERVICE_REGISTRY_KEY,        
    0,                                  
    NULL,                                
    REG_OPTION_NON_VOLATILE,             
    KEY_WRITE,                                   
    NULL,                                    
    &hAppKey,                     
    &dwDisposition))   
  {        
    return false;
  }   
   else   
  {       
    if (ERROR_SUCCESS != RegSetValueEx (hAppKey,         
     DATA_FILE_SUB_KEY,                  
     0,                                       
     REG_SZ,                                       
     (LPBYTE)szDataFile,               
     (lstrlen (szDataFile)+1)*sizeof(TCHAR))) 
   {         
     return false;
   }    
    RegCloseKey (hAppKey); 
  }
 }
 return true;
}




//sel the special font
LOGFONT lf = {0};
GetMSShellDlgFont(lf);
CString strfontName, strCurFont;;

HKEY hKey;
DWORD dwRet;
TCHAR tcPath[MAX_PATH] = {0};
DWORD dwLen = MAX_PATH * sizeof(TCHAR);
DWORD dwType = REG_SZ;

dwRet = RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\FontSubstitutes"), &hKey);
if(dwRet == ERROR_SUCCESS)
{
	dwRet = RegQueryValueEx(hKey, lf.lfFaceName, NULL, &dwType, (BYTE *)tcPath, &dwLen);
	if(dwRet == ERROR_SUCCESS)
	{
		strfontName.Format(_T("%s"), tcPath);
		RegCloseKey(hKey);
	}
}