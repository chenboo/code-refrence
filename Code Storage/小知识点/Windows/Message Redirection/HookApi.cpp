#include "HookApi.h"

CHOOKAPI::CHOOKAPI () 
{

}

CHOOKAPI::~CHOOKAPI() 
{


}

// 实现HOOK API
void CHOOKAPI::Hook ( PSTR szModuleName, PSTR szFunName, FARPROC pFun )
{ 
	HMODULE hMod = ::GetModuleHandleA ( szModuleName ) ;
	if ( hMod != NULL )
	{
		m_pNewFunEntry = (LPVOID)pFun ;
		m_pOldFunEntry = (LPVOID)GetProcAddress ( hMod, szFunName ) ;
		m_NewByte[0] = 0xE9 ;
		*((PDWORD)(&(m_NewByte[1]))) = (DWORD)m_pNewFunEntry - (DWORD)m_pOldFunEntry - 5 ;

		DWORD dwProtect, dwWriteByte, dwReadByte ;  
		VirtualProtect ( (LPVOID)m_pOldFunEntry, 5, PAGE_READWRITE, &dwProtect );
		ReadProcessMemory ( GetCurrentProcess(), (LPVOID)m_pOldFunEntry, m_OldByte, 5, &dwReadByte ) ; 
		WriteProcessMemory ( GetCurrentProcess(), (LPVOID)m_pOldFunEntry, m_NewByte, 5, &dwWriteByte ) ;
		VirtualProtect ( (LPVOID)m_pOldFunEntry, 5, dwProtect, NULL ) ;
	}
}

// 重新HOOK
void CHOOKAPI::ReHook ()
{
	DWORD dwProtect, dwWriteByte ;
	VirtualProtect ( m_pOldFunEntry, 5, PAGE_READWRITE, &dwProtect );
	WriteProcessMemory ( GetCurrentProcess(), m_pOldFunEntry, m_NewByte, 5, &dwWriteByte ) ;
	VirtualProtect ( m_pOldFunEntry, 5, dwProtect, NULL ) ;
}
// 撤消HOOK
void CHOOKAPI::UnHook ()
{
	DWORD dwProtect, dwWriteByte ;
	VirtualProtect ( m_pOldFunEntry, 5, PAGE_READWRITE, &dwProtect );
	WriteProcessMemory ( GetCurrentProcess(), m_pOldFunEntry, m_OldByte, 5, &dwWriteByte ) ;
	VirtualProtect ( m_pOldFunEntry, 5, dwProtect, NULL ) ;
}
