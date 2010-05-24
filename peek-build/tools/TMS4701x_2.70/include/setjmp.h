/*****************************************************************************/
/* setjmp.h   v2.70                                                          */
/* Copyright (c) 1993-2008 Texas Instruments Incorporated                    */
/*****************************************************************************/

#ifndef _SETJMP
#define _SETJMP

#include <linkage.h>

#ifdef __cplusplus
extern "C" namespace std {
#endif

#define setjmp(x) _setjmp(x)

    typedef int jmp_buf[10];

_CODE_ACCESS int  _setjmp(jmp_buf env); 
_CODE_ACCESS void longjmp(jmp_buf env, int val);

#ifdef __cplusplus
} /* extern "C" namespace std */

#ifndef _CPP_STYLE_HEADER
using std::jmp_buf;
using std::_setjmp;
using std::longjmp;
#endif /* _CPP_STYLE_HEADER */

#endif /* __cplusplus */

#else

#ifdef __cplusplus

#ifndef _CPP_STYLE_HEADER
using std::jmp_buf;
using std::_setjmp;
using std::longjmp;
#endif /* _CPP_STYLE_HEADER */

#endif /* __cplusplus */

#endif /* _SETJMP */
