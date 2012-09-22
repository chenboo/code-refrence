//Init Gdi+ library

//一、添加相应的头文件包含和lib的引入(通常在stdafx.h中加入)
	#include <gdiplus.h>
	#pragma comment(lib, "gdiplus.lib")

//二、初始化
//1、在相应的app头文件中声明如下变量:

	ULONG_PTR	m_gdiplusToken;

//2、在InitInstance()中加入如下语句:

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken,&gdiplusStartupInput,NULL);
		
//三、使用
//在需要使用的地方加上如下语句就可以使用了:
	using namespace Gdiplus;
	
//四、退出
//在ExitInstance()中加入如下语句:
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
