//读写配置文件:

//一、往注册表中读写配置文件
//1、写入
	//写入整形
	BOOL CWinApp::WriteProfileInt(
		LPCTSTR lpszSection,	//节点名
		LPCTSTR lpszEntry,		//键名
		int nValue 				//键值
		);
	
	AfxGetApp()->WriteProfileInt(_T("Preferences"), _T("StudentFullScreen"), 1238795412);
	
	//写入字符串
	BOOL CWinApp::WriteProfileString(
		LPCTSTR lpszSection,
		LPCTSTR lpszEntry,
		LPCTSTR lpszValue 
		);

	AfxGetApp()->WriteProfileString(_T("Preferences"), _T("SingleChannel_VideoDeviceId"), _T("abcsdasd"));
	
//2、读取
	//读取整形
	UINT GetProfileInt(
		LPCTSTR lpszSection,
		LPCTSTR lpszEntry,
		int nDefault 
		);
	
	int nvalue = AfxGetApp()->GetProfileInt(_T("Preferences"),_T("StudentFullScreen"),10);//当该键值未找到时，10为默认值返回
	
	//读取字符串
	CString GetProfileString(
		LPCTSTR lpszSection,
		LPCTSTR lpszEntry,
		LPCTSTR lpszDefault = NULL 
		);
	
	AfxGetApp()->GetProfileString(_T("Preferences"), _T("SingleChannel_VideoDeviceId"), _T(""));//_T("")为默认字符串
	
	
//二、写入到自定义的ini文件中

//1、写入
	BOOL WritePrivateProfileString(
		LPCTSTR lpAppName,		//节点名
		LPCTSTR lpKeyName,		//键名
		LPCTSTR lpString,		//键值
		LPCTSTR lpFileName		//ini文件名
	);


	::WritePrivateProfileString(_T("peng shuiming"),_T("province"),_T("Hunan"),
		_T("F:/first.ini"));

	::WritePrivateProfileString(_T("peng shuiming"),_T("Age"),_T("23"),
		_T("F:/first.ini"));

	::WritePrivateProfileString(_T("peng shuiming"),_T("sex"),_T("Male"),
		_T("F:/first.ini"));
		
	//由于写入到自定义的ini文件中不提供写入整形的函数，因此如果要写入整形，
	//要将整形转换为字符串，用CString的format函数转换。
		
//2、读取
	//读取字符串
		DWORD GetPrivateProfileString(
		LPCTSTR lpAppName,			//节点值
		LPCTSTR lpKeyName,			//键值
		LPCTSTR lpDefault,			//默认值
		LPTSTR lpReturnedString,		//获取返回值的字符串指针
		DWORD nSize,				//字符大小
		LPCTSTR lpFileName			//ini文件名
		);
	
	::GetPrivateProfileString("Test","id","Error", str.GetBuffer(20), 20, _T("F:/first.ini"));
	
	//读取整数
	UINT GetPrivateProfileInt(
		LPCTSTR lpAppName,
		LPCTSTR lpKeyName,
		INT nDefault,
		LPCTSTR lpFileName
		);
	
	int nValue = ::GetPrivateProfileInt("Test","age", 0, _T("F:/first.ini"));
	
//	lpKeyName 
//	[in] Pointer to the null-terminated string specifying the name of the key whose value is to be retrieved. 
//	This value is in the form of a string; the GetPrivateProfileInt function converts the string into an integer and returns the integer.

	


		