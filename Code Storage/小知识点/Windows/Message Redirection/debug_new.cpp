/************************************************************************/
/* comment:  ���ļ���debug_new.h���ʹ��,�����ڵ����ڷ����ڴ�й©       */
/*           ����VC++������������(����Intel C++,��Ϊ��ʹ������ͬ�Ŀ�)   */
/* ����:     ������ http://blog.vckbase.com/bruceteen/                  */
/* ��Ȩ����: ��,������ ʹ��,�޸� �� ����                                */
/************************************************************************/

//#include "debug_new.h"

#ifdef _DEBUG

#include <windows.h>
#include <crtdbg.h>

class _CriSec
{
    CRITICAL_SECTION criSection;
public:
    _CriSec()    { InitializeCriticalSection( &criSection ); }
    ~_CriSec()   { DeleteCriticalSection( &criSection );     }
    void Enter() { EnterCriticalSection( &criSection );      }
    void Leave() { LeaveCriticalSection( &criSection );      }
} _cs;

void _RegDebugNew( void )
{
    _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG | _CRTDBG_LEAK_CHECK_DF );
}
void* __cdecl operator new( size_t nSize, const char* lpszFileName, int nLine )
{
    // comment 1: MFC���ṩ��debug new��Ȼ������,������ʵ�ʲ��Ե�ʱ���ֶ��̲߳���
    //            ���õ�ʱ�����׳���ϵͳ����,���������������һ���̻߳�����.
    // comment 2: debug new��debug delete֮���費��Ҫ�����Ҳ���֪��,�������,��ͬ��
    //            �����̻߳�����.
    // comment 3: ����C++��׼�涨,��operator newʧ�ܺ�Ӧ������set_new_handler���õ�
    //            ����,����MSDN��ȴ˵"ͷ�ļ�new�е�set_new_handler��stub��,��Ӧ��ʹ
    //            ��ͷ�ļ�new.h�е�_set_new_handler",���ֱ�ǻ�����֮���.
    //            ������VC++6.0�е�set_new_handler����:
    //                new_handler __cdecl set_new_handler( new_handler new_p )
    //                {
    //                    assert( new_p == 0 ); // cannot use stub to register a new handler
    //                    _set_new_handler( 0 );
    //                    return 0;
    //                }
    //            ������Ҳ�޼ƿ�ʩ,ֻ������set_new_handler������.

    _cs.Enter();
    void* p = _malloc_dbg( nSize, _NORMAL_BLOCK, lpszFileName, nLine );
    _cs.Leave();
    return p;
}
void __cdecl operator delete( void* p, const char* /*lpszFileName*/, int /*nLine*/ )
{
    _cs.Enter();
    _free_dbg( p, _CLIENT_BLOCK );
    _cs.Leave();
}

#endif
