#ifndef __INC_OTTSDEFS_H__
#define __INC_OTTSDEFS_H__

#if 0 // neil

/*****************************************************************************
  *****************************************************************************/
#define OTTS   //added by wangran
#ifndef OTTS
    /* ------------------------------------- */
    /* **************** ARM **************** */
    /* ------------------------------------- */

    /* no static for ARM */
    #define OTTS_static

    /* enable ARM compiler "__packed" and "__inline" directives */
    #define PACKED  __packed
    #define INLINE __inline

    /* use alias for "long long" for portability between compilers */
    #define LONGLONG long long

    /* suffix for 64-bit constants is different */
    #define _64(x) x##LL
#else
    /* ------------------------------------- */
    /* ************* Visual C++ ************ */
    /* ------------------------------------- */

#ifndef __cplusplus
    #undef FALSE
    #undef TRUE
    typedef unsigned char bool;
    #define TRUE    ((unsigned char)1)
    #define FALSE   ((unsigned char)0)
#else
#ifndef TRUE
    #define TRUE    (1)
#endif
#ifndef FALSE
    #define FALSE   (0)
#endif
#endif 


    /* static scoping */
    #define OTTS_static static

    /* no "__packed" or "__inline" directives in Visual C++ */
    #define PACKED
    #define INLINE

    /* no "long long" type in Visual C++: use __int64 instead */
    typedef __int64 LONGLONG;
    typedef __int64 int64;
    typedef unsigned __int64  uint64;
    typedef unsigned short   uint16;
    /* suffix for 64-bit constants is different */
    #define _64(x) x##L

    /* Suppress some of the useless and annoying warnings.
       Also suppress some of the warnings that the ARM compiler should be
       bitching about, and which the stack programmers should pay attention to,
       but are frequent enough that they muddy the compile result listing (the
       purpose of the run through the Visual C++ compiler is *not* to catch
       this kind of stuff, just to allow their code to run and be tested under
       the simulator. These kinds of warnings should be caught and fixed at the
       ARM compile stage.
       ----------------------------------------------------------------------- */
//    #pragma warning(disable: 4761) /* integral size mismatch */
//    #pragma warning(disable: 4244) /* conversion loses precision */
//    #pragma warning(disable: 4013) /* function used without prototype, assuming extern returning int */
//    #pragma warning(disable: 4018) /* signed/unsigned mismatch */
//    #pragma warning(disable: 4091) /* extern ignored when variable not declared */
//    #pragma warning(disable: 4002) /* too many actual parameters for macro */
//    #pragma warning(disable: 4003) /* not enough actual parameters for macro */
//    #pragma warning(disable: 4554) /* check operator precedence for possible error */
//    #pragma warning(disable: 4103) /* used #pragma to change alignment */
//    #pragma warning(disable: 4142) /* bening redefinition of type */

    // [][][][][] disable traces and assertions in OTTS (for now) [][][][][]
//  #pragma warning(disable: 4005) /* macro redefinition (for disabling traces) */
//  #undef  TRACE
//  #undef  FTRACE1P
//  #undef  FTRACE2P
//  #undef  TRACEDATA
//  #undef  FTRACES
//  #undef  IASSERT
//  #undef  FSMEXTTRACE_INFO
//  #undef  MonTrace
//
//  #define TRACE(x1,x2,x3,x4,x5,x6,x7)
//  #define FTRACE1P(x1,x2,x3,x4,x5,x6,x7)
//  #define FTRACE2P(x1,x2,x3,x4,x5,x6,x7)
//  #define TRACEDATA(x1,x2,x3,x4,x5,x6,x7)
//  #define FTRACES(x1,x2,x3,x4,x5,x6,x7)
//  #define IASSERT(expr)
//  #define FSMEXTTRACE_INFO(x1,x2,x3,x4,x5,x6,x7,x8)
//  #define MonTrace(x1,x2,x3,x4,x5,x6,x7)
#endif



#endif


#endif
