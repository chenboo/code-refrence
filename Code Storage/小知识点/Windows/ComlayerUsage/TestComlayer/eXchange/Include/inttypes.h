#ifndef _CYGWIN_INTTYPES_H
#define _CYGWIN_INTTYPES_H

typedef char *  caddr_t;
typedef signed char int8_t;
typedef unsigned char u_int8_t;
typedef short int16_t;
typedef unsigned short u_int16_t;
typedef int int32_t;
typedef unsigned int u_int32_t;
typedef signed int intptr_t;
typedef unsigned int uintptr_t;

#ifdef _MSC_VER
typedef __int64 int64_t;
typedef unsigned __int64 u_int64_t;
#else
typedef long long int64_t;
typedef unsigned long long u_int64_t;
#endif

typedef int32_t register_t;
typedef u_int8_t uint8_t;
typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef u_int64_t uint64_t;

#define __WORDSIZE 32

#endif
