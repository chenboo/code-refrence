/*
1.菜单中选择 Project->Properties, 弹出Property Pages窗口
2.在左边栏中依次选择：Configuration Properties->C/C++->Preprocessor,
然后在右边栏的Preprocessor Definitions对应的项中删除_CONSOLE, 添加_WINDOWS.
3.在左边栏中依次选择：Configuration Properties->Linker->System,
然后在右边栏的SubSystem对应的项改为Windows(/SUBSYSTEM:WINDOWS)
*/

#include <windows.h>
#include "HookApi.h"

CHOOKAPI HookItem ;

// 定义MessageBoxA函数原型
typedef int (WINAPI* PFNMessageBoxA)( HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType ) ;

// 自定义的MessageBoxA函数
// 实现对原始MessageBoxA的输入、输出参数的监控，甚至是取消调用
int WINAPI NEW_MessageBoxA( HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType )
{
	// 撤消HOOK
	HookItem.UnHook () ;

	// 此处可以观察/修改调用参数，甚至可以取消调用直接返回。
	//

	// 取得原函数地址
	PFNMessageBoxA pfnMessageBoxA = (PFNMessageBoxA)HookItem.m_pOldFunEntry ;

	// 调用原函数，修改输入参数
	int ret = pfnMessageBoxA ( hWnd, "这是HOOK函数过程的消息框", "[测试]", uType ) ;

	// 此处可以查看/修改调用原函数的返回值
	// ……

	// 重新HOOK
	HookItem.ReHook () ;

	return ret ;
}

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// 原始API
	MessageBoxA ( 0, "正常消息框", "测试", 0 ) ;
	// HOOK API
	HookItem.Hook ( "USER32.dll", "MessageBoxA", (FARPROC)NEW_MessageBoxA ) ;
	// 调用API，测试
	MessageBoxA ( 0, "江苏省南京市", "测试", 0 ) ;

	// 撤消HOOK
	HookItem.UnHook () ;
	return 0 ;
}
