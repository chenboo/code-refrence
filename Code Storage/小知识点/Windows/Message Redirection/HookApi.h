#pragma once

//��������ʵ���ض���API

class CHOOKAPI 
{
public:
	LPVOID	m_pOldFunEntry;
	LPVOID	m_pNewFunEntry; // ��ʼ������ַ��HOOK��ĺ�����ַ
	BYTE	m_OldByte[5];
	BYTE	m_NewByte[5] ; // ԭʼ�ֽڡ�Ŀ���ֽ�

public:
	CHOOKAPI();
	~CHOOKAPI();

	// ʵ��HOOK API
	void Hook ( PSTR szModuleName, PSTR szFunName, FARPROC pFun );

	// ����HOOK
	void ReHook();
		
	// ����HOOK
	void UnHook();
} ;