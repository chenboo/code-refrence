//控制台下内存泄漏

#include <iostream>
using namespace std;

#ifdef _DEBUG

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__,__LINE__)
#define new DEBUG_NEW

#endif 

int main()
{
	int i = 0;
	int j = 12;

	//_crtBreakAlloc = 63;//指定出内存泄漏的分配号，在这个内存分配号的地方，中断
	//_CrtMemState s1, s2, s3;//内存状态，用于内存快照
	//_CrtMemCheckpoint( &s1 );//内存快照，将快照保存到结构体s1中

	int* phg = new int[20];
	int* pint = new int[10];
	float* pfuh = new float[10];

	//_CrtMemCheckpoint( &s2 );//内存快照，将快照保存到结构体s2中

	//_CrtMemDumpStatistics( &s1 );//输出内存快照信息
	//_CrtMemDumpStatistics( &s2 );
	
	//_CrtMemDifference(&s3,&s1,&s2);//比较快照s1和s2，将比较结果放入到s3当中
	//_CrtMemDumpStatistics( &s3 );//输出内存快照信息
	
	cout<<"peng hao le"<<endl;
	_CrtDumpMemoryLeaks();//输出内存泄漏信息

	getchar();
	return 0;
}


//内存分配断点文件:
C:\Program Files\Microsoft Visual Studio 8\VC\crt\src\dbgheap.c

//函数:
extern "C" void * __cdecl _heap_alloc_dbg(
        size_t nSize,
        int nBlockUse,
        const char * szFileName,
        int nLine
        )
//作用语句:
/* break into debugger at specific memory allocation */
if (_crtBreakAlloc != -1L && lRequest == _crtBreakAlloc)
	 _CrtDbgBreak();
