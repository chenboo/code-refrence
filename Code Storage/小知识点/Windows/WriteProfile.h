//��д�����ļ�:

//һ����ע����ж�д�����ļ�
//1��д��
	//д������
	BOOL CWinApp::WriteProfileInt(
		LPCTSTR lpszSection,	//�ڵ���
		LPCTSTR lpszEntry,		//����
		int nValue 				//��ֵ
		);
	
	AfxGetApp()->WriteProfileInt(_T("Preferences"), _T("StudentFullScreen"), 1238795412);
	
	//д���ַ���
	BOOL CWinApp::WriteProfileString(
		LPCTSTR lpszSection,
		LPCTSTR lpszEntry,
		LPCTSTR lpszValue 
		);

	AfxGetApp()->WriteProfileString(_T("Preferences"), _T("SingleChannel_VideoDeviceId"), _T("abcsdasd"));
	
//2����ȡ
	//��ȡ����
	UINT GetProfileInt(
		LPCTSTR lpszSection,
		LPCTSTR lpszEntry,
		int nDefault 
		);
	
	int nvalue = AfxGetApp()->GetProfileInt(_T("Preferences"),_T("StudentFullScreen"),10);//���ü�ֵδ�ҵ�ʱ��10ΪĬ��ֵ����
	
	//��ȡ�ַ���
	CString GetProfileString(
		LPCTSTR lpszSection,
		LPCTSTR lpszEntry,
		LPCTSTR lpszDefault = NULL 
		);
	
	AfxGetApp()->GetProfileString(_T("Preferences"), _T("SingleChannel_VideoDeviceId"), _T(""));//_T("")ΪĬ���ַ���
	
	
//����д�뵽�Զ����ini�ļ���

//1��д��
	BOOL WritePrivateProfileString(
		LPCTSTR lpAppName,		//�ڵ���
		LPCTSTR lpKeyName,		//����
		LPCTSTR lpString,		//��ֵ
		LPCTSTR lpFileName		//ini�ļ���
	);


	::WritePrivateProfileString(_T("peng shuiming"),_T("province"),_T("Hunan"),
		_T("F:/first.ini"));

	::WritePrivateProfileString(_T("peng shuiming"),_T("Age"),_T("23"),
		_T("F:/first.ini"));

	::WritePrivateProfileString(_T("peng shuiming"),_T("sex"),_T("Male"),
		_T("F:/first.ini"));
		
	//����д�뵽�Զ����ini�ļ��в��ṩд�����εĺ�����������Ҫд�����Σ�
	//Ҫ������ת��Ϊ�ַ�������CString��format����ת����
		
//2����ȡ
	//��ȡ�ַ���
		DWORD GetPrivateProfileString(
		LPCTSTR lpAppName,			//�ڵ�ֵ
		LPCTSTR lpKeyName,			//��ֵ
		LPCTSTR lpDefault,			//Ĭ��ֵ
		LPTSTR lpReturnedString,		//��ȡ����ֵ���ַ���ָ��
		DWORD nSize,				//�ַ���С
		LPCTSTR lpFileName			//ini�ļ���
		);
	
	::GetPrivateProfileString("Test","id","Error", str.GetBuffer(20), 20, _T("F:/first.ini"));
	
	//��ȡ����
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

	


		