/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Sep 26 18:36:34 2006
 */
/* Compiler settings for F:\Projects\Matrix\Source\TDDesk2\TCPServer.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __TCPServer_h__
#define __TCPServer_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IELCTCPServer_FWD_DEFINED__
#define __IELCTCPServer_FWD_DEFINED__
typedef interface IELCTCPServer IELCTCPServer;
#endif 	/* __IELCTCPServer_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IELCTCPServer_INTERFACE_DEFINED__
#define __IELCTCPServer_INTERFACE_DEFINED__

/* interface IELCTCPServer */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IELCTCPServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C40E8CF4-F6BD-4127-B6E8-CCB17C9F6459")
    IELCTCPServer : public IUnknown
    {
    public:
        virtual /* [local][helpstring] */ HRESULT STDMETHODCALLTYPE SendPacket( 
            DWORD dwChannelID,
            LPVOID lpvPacket,
            DWORD cbPacket) = 0;
        
        virtual /* [local][helpstring] */ float STDMETHODCALLTYPE GetMaxBufferUsage( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IELCTCPServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IELCTCPServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IELCTCPServer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IELCTCPServer __RPC_FAR * This);
        
        /* [local][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendPacket )( 
            IELCTCPServer __RPC_FAR * This,
            DWORD dwChannelID,
            LPVOID lpvPacket,
            DWORD cbPacket);
        
        /* [local][helpstring] */ float ( STDMETHODCALLTYPE __RPC_FAR *GetMaxBufferUsage )( 
            IELCTCPServer __RPC_FAR * This);
        
        END_INTERFACE
    } IELCTCPServerVtbl;

    interface IELCTCPServer
    {
        CONST_VTBL struct IELCTCPServerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IELCTCPServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IELCTCPServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IELCTCPServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IELCTCPServer_SendPacket(This,dwChannelID,lpvPacket,cbPacket)	\
    (This)->lpVtbl -> SendPacket(This,dwChannelID,lpvPacket,cbPacket)

#define IELCTCPServer_GetMaxBufferUsage(This)	\
    (This)->lpVtbl -> GetMaxBufferUsage(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [local][helpstring] */ HRESULT STDMETHODCALLTYPE IELCTCPServer_SendPacket_Proxy( 
    IELCTCPServer __RPC_FAR * This,
    DWORD dwChannelID,
    LPVOID lpvPacket,
    DWORD cbPacket);


void __RPC_STUB IELCTCPServer_SendPacket_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [local][helpstring] */ float STDMETHODCALLTYPE IELCTCPServer_GetMaxBufferUsage_Proxy( 
    IELCTCPServer __RPC_FAR * This);


void __RPC_STUB IELCTCPServer_GetMaxBufferUsage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IELCTCPServer_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
