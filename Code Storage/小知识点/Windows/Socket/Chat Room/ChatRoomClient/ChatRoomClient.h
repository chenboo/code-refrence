// ChatRoomClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CChatRoomClientApp:
// �йش����ʵ�֣������ ChatRoomClient.cpp
//

class CChatRoomClientApp : public CWinApp
{
public:
	CChatRoomClientApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CChatRoomClientApp theApp;