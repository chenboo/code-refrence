//Init Gdi+ library

//һ�������Ӧ��ͷ�ļ�������lib������(ͨ����stdafx.h�м���)
	#include <gdiplus.h>
	#pragma comment(lib, "gdiplus.lib")

//������ʼ��
//1������Ӧ��appͷ�ļ����������±���:

	ULONG_PTR	m_gdiplusToken;

//2����InitInstance()�м����������:

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken,&gdiplusStartupInput,NULL);
		
//����ʹ��
//����Ҫʹ�õĵط������������Ϳ���ʹ����:
	using namespace Gdiplus;
	
//�ġ��˳�
//��ExitInstance()�м����������:
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
