//����̨���ڴ�й©

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

	//_crtBreakAlloc = 63;//ָ�����ڴ�й©�ķ���ţ�������ڴ����ŵĵط����ж�
	//_CrtMemState s1, s2, s3;//�ڴ�״̬�������ڴ����
	//_CrtMemCheckpoint( &s1 );//�ڴ���գ������ձ��浽�ṹ��s1��

	int* phg = new int[20];
	int* pint = new int[10];
	float* pfuh = new float[10];

	//_CrtMemCheckpoint( &s2 );//�ڴ���գ������ձ��浽�ṹ��s2��

	//_CrtMemDumpStatistics( &s1 );//����ڴ������Ϣ
	//_CrtMemDumpStatistics( &s2 );
	
	//_CrtMemDifference(&s3,&s1,&s2);//�ȽϿ���s1��s2�����ȽϽ�����뵽s3����
	//_CrtMemDumpStatistics( &s3 );//����ڴ������Ϣ
	
	cout<<"peng hao le"<<endl;
	_CrtDumpMemoryLeaks();//����ڴ�й©��Ϣ

	getchar();
	return 0;
}


//�ڴ����ϵ��ļ�:
C:\Program Files\Microsoft Visual Studio 8\VC\crt\src\dbgheap.c

//����:
extern "C" void * __cdecl _heap_alloc_dbg(
        size_t nSize,
        int nBlockUse,
        const char * szFileName,
        int nLine
        )
//�������:
/* break into debugger at specific memory allocation */
if (_crtBreakAlloc != -1L && lRequest == _crtBreakAlloc)
	 _CrtDbgBreak();
