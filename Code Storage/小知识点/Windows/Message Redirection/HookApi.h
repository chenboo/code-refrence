#pragma once

//这个类可以实现重定向API

class CHOOKAPI 
{
public:
	LPVOID	m_pOldFunEntry;
	LPVOID	m_pNewFunEntry; // 初始函数地址、HOOK后的函数地址
	BYTE	m_OldByte[5];
	BYTE	m_NewByte[5] ; // 原始字节、目标字节

public:
	CHOOKAPI();
	~CHOOKAPI();

	// 实现HOOK API
	void Hook ( PSTR szModuleName, PSTR szFunName, FARPROC pFun );

	// 重新HOOK
	void ReHook();
		
	// 撤消HOOK
	void UnHook();
} ;