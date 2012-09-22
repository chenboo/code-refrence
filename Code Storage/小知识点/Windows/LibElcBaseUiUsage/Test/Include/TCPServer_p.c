/* this ALWAYS GENERATED file contains the proxy stub code */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Sep 26 18:36:34 2006
 */
/* Compiler settings for F:\Projects\Matrix\Source\TDDesk2\TCPServer.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "TCPServer.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IELCTCPServer, ver. 0.0,
   GUID={0xC40E8CF4,0xF6BD,0x4127,{0xB6,0xE8,0xCC,0xB1,0x7C,0x9F,0x64,0x59}} */


extern const MIDL_STUB_DESC Object_StubDesc;


#pragma code_seg(".orpc")

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x10001, /* Ndr library version */
    0,
    0x50100a4, /* MIDL Version 5.1.164 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    1,  /* Flags */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

const CINTERFACE_PROXY_VTABLE(5) _IELCTCPServerProxyVtbl = 
{
    &IID_IELCTCPServer,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IELCTCPServer_SendPacket_Proxy */ ,
    0 /* IELCTCPServer_GetMaxBufferUsage_Proxy */
};


static const PRPC_STUB_FUNCTION IELCTCPServer_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

const CInterfaceStubVtbl _IELCTCPServerStubVtbl =
{
    &IID_IELCTCPServer,
    0,
    5,
    &IELCTCPServer_table[-3],
    CStdStubBuffer_METHODS
};

#pragma data_seg(".rdata")

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };

const CInterfaceProxyVtbl * _TCPServer_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IELCTCPServerProxyVtbl,
    0
};

const CInterfaceStubVtbl * _TCPServer_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IELCTCPServerStubVtbl,
    0
};

PCInterfaceName const _TCPServer_InterfaceNamesList[] = 
{
    "IELCTCPServer",
    0
};


#define _TCPServer_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _TCPServer, pIID, n)

int __stdcall _TCPServer_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_TCPServer_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo TCPServer_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _TCPServer_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _TCPServer_StubVtblList,
    (const PCInterfaceName * ) & _TCPServer_InterfaceNamesList,
    0, // no delegation
    & _TCPServer_IID_Lookup, 
    1,
    1,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
