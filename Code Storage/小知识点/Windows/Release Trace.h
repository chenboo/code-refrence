//很多人都用过MFC中的调试宏TRACE,一个非常方便的调试宏,但它只能在调试状态在编译器的输出窗口输出消息.
//有时候我们需要在一台没装编译器的电脑上试运行程序,这时如果能实现一个像
//TRACE消息输出功能将非常有用.
//TRACE 的实现如下

void AFX_CDECL AfxTrace(LPCTSTR lpszFormat, ...)
{

#ifdef _DEBUG // all AfxTrace output is controlled by afxTraceEnabled

	if (!afxTraceEnabled)
	{
		return;
	}

#endif
	//处理个数不定的参数

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

//我在原来的基础上做了些修改

void __cdecl MyTrace(LPCTSTR lpszFormat, ...)

{
	//处理个数不定的参数
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
		//在桌面创建EDIT控件

		hEdit = CreateWindowW(_T("EDIT"),  //控件"类名称"
			_T("MYTRACE"),
			WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL| ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
			rect.right-400,
			rect.bottom-200,
			400,
			150,
			hwnd,
			NULL,
			AfxGetApp()->m_hInstance, //实例句柄
			NULL);

		SetWindowPos(hEdit,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE); //设置EDIT控件为最上面

	}

	//将szBuffer中的内容添加到EDIT控件中
	if(hEdit)
	{
		SendMessage(hEdit,EM_SETSEL ,NULL,NULL); 
		SendMessage(hEdit,EM_REPLACESEL,NULL,(LPARAM)szBuffer);
	}
}

//在任意一个cpp文件中包含MyTrace函数定义 

//在stdafx.h文件中添加 

#ifndef _DEBUG
#define TRACE MyTrace
#endif

void __cdecl MyTrace(LPCTSTR lpszFormat, ...);


//现在你的TRACE宏在release版本中可以继续发挥作用了,快去试下效果吧! 