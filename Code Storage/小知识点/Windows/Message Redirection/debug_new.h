/************************************************************************/
/* comment:  ���ļ���debug_new.cpp���ʹ��,�����ڵ����ڷ����ڴ�й©     */
/*           ����VC++������������(����Intel C++,��Ϊ��ʹ������ͬ�Ŀ�)   */
/* ����:     ������ http://blog.vckbase.com/bruceteen/                  */
/* ��Ȩ����: ��,������ ʹ��,�޸� �� ����                                */
/************************************************************************/

/* sample

#include <iostream>
#include "debug_new.h" // +
using namespace std;

int main( void )
{
    REG_DEBUG_NEW; // +

    char* p = new char[2];

    cout << "--End--" << endl;
    return 0;
}

��VC++ IDE�а�F5�������н�����Output���ڵ�Debugҳ�����������µ���ʾ:
Dumping objects ->
d:\test.cpp(10) : {45} normal block at 0x003410C8, 2 bytes long.
Data: <  > CD CD 
Object dump complete.

���������������ʾ��Rebuild Allһ��.

*/

#ifndef _DEBUG_NEW_H_
#define _DEBUG_NEW_H_

    #ifdef _DEBUG

        #undef new
        extern void _RegDebugNew( void );
        extern void* __cdecl operator new( size_t, const char*, int );
        extern void __cdecl operator delete( void*, const char*, int);
        #define new new(__FILE__, __LINE__)
        
        #define REG_DEBUG_NEW _RegDebugNew();

    #else

        #define REG_DEBUG_NEW

    #endif // _DEBUG

#endif // _DEBUG_NEW_H_
