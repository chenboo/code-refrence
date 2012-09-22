/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Dec 15 09:10:10 2001
 */
/* Compiler settings for ELCFunction.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IActiveFunction = {0x56588044,0x6C35,0x479D,{0x83,0x18,0xF9,0x98,0x86,0x44,0x86,0x5E}};


const IID IID_IPassiveFunction = {0x391E507E,0xB99E,0x4540,{0xBA,0x8A,0xCA,0x81,0xBC,0xBD,0x44,0xFD}};


#ifdef __cplusplus
}
#endif

