//�ܶ��˶��ù�MFC�еĵ��Ժ�TRACE,һ���ǳ�����ĵ��Ժ�,����ֻ���ڵ���״̬�ڱ�������������������Ϣ.
//��ʱ��������Ҫ��һ̨ûװ�������ĵ����������г���,��ʱ�����ʵ��һ����
//TRACE��Ϣ������ܽ��ǳ�����.
//TRACE ��ʵ������

void AFX_CDECL AfxTrace(LPCTSTR lpszFormat, ...)
{

#ifdef _DEBUG // all AfxTrace output is controlled by afxTraceEnabled

	if (!afxTraceEnabled)
	{
		return;
	}

#endif
	//������������Ĳ���

	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	TCHAR szBuffer[512];
	nBuf = _vstprintf(szBuffer, lpszFormat, args);

	ASSERT(nBuf < _countof(szBuffer));

	if ((afxTraceFlags & traceMultiApp) && (AfxGetApp() != NULL))
	{
		afxDump << AfxGetApp()->m_pszExeName << ":";
	}

	afxDump << szBuffer;
	va_end(args);
}

#endif //_DEBUG

//����ԭ���Ļ���������Щ�޸�

void __cdecl MyTrace(LPCTSTR lpszFormat, ...)

{
	//������������Ĳ���
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[512];

	nBuf = _vstprintf(szBuffer, lpszFormat, args);
	ASSERT(nBuf < _countof(szBuffer));
	va_end(args);

	static HWND hEdit=NULL;
	if(hEdit==NULL)
	{
		HWND hwnd=GetDesktopWindow();
		RECT rect;
		GetWindowRect(hwnd,&rect);
		//�����洴��EDIT�ؼ�

		hEdit = CreateWindowW(_T("EDIT"),  //�ؼ�"������"
			_T("MYTRACE"),
			WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL| ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
			rect.right-400,
			rect.bottom-200,
			400,
			150,
			hwnd,
			NULL,
			AfxGetApp()->m_hInstance, //ʵ�����
			NULL);

		SetWindowPos(hEdit,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE); //����EDIT�ؼ�Ϊ������

	}

	//��szBuffer�е�������ӵ�EDIT�ؼ���
	if(hEdit)
	{
		SendMessage(hEdit,EM_SETSEL ,NULL,NULL); 
		SendMessage(hEdit,EM_REPLACESEL,NULL,(LPARAM)szBuffer);
	}
}

//������һ��cpp�ļ��а���MyTrace�������� 

//��stdafx.h�ļ������ 

#ifndef _DEBUG
#define TRACE MyTrace
#endif

void __cdecl MyTrace(LPCTSTR lpszFormat, ...);


//�������TRACE����release�汾�п��Լ�������������,��ȥ����Ч����! 