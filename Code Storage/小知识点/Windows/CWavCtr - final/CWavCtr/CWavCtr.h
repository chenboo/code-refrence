// CWavCtr.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCWavCtrApp:
// �йش����ʵ�֣������ CWavCtr.cpp
//

class CCWavCtrApp : public CWinApp
{
public:
	CCWavCtrApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCWavCtrApp theApp;