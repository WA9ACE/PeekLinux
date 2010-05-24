/*****************************************************************************/
/* stdarg.h   v2.70                                                          */
/* Copyright (c) 1996-2008 Texas Instruments Incorporated                    */
/*****************************************************************************/

#ifndef _STDARG
#define _STDARG

#ifdef __cplusplus
#define _NS_PREFIX std::
namespace std {
#else
#define _NS_PREFIX
#endif /* __cplusplus */

typedef char *va_list;

#ifdef __cplusplus
} /* namespace std */
#endif

#define va_start(ap, parmN)                                                   \
   ((ap) = (!__va_argref(parmN) && sizeof(parmN) == sizeof(double)            \
	    ? (_NS_PREFIX va_list)((int)__va_parmadr(parmN) + 8)              \
            : (!__va_argref(parmN) && sizeof(parmN) <= sizeof(short))         \
            ? (_NS_PREFIX va_list)((int)__va_parmadr(parmN) + 4 & ~3)         \
            : (_NS_PREFIX va_list)((int)__va_parmadr(parmN) + 4 )))


#ifdef __big_endian__
#define va_arg(_ap, _type)                                                    \
        (__va_argref(_type)                                                   \
	 ? ((_ap += sizeof(_type*)),(**(_type**)(_ap-(sizeof(_type*)))))      \
         : ((sizeof(_type) == sizeof(double)                                  \
             ? ((_ap += 8), (*(_type *)(_ap - 8)))                            \
             : ((_ap += 4), (*(_type *)(_ap - (sizeof(_type))))))))
#else
#define va_arg(_ap, _type)                                                    \
        (__va_argref(_type) 						      \
	 ? ((_ap += sizeof(_type*)),(**(_type**)(_ap-(sizeof(_type*)))))      \
         : ((sizeof(_type) == sizeof(double)                                  \
             ? ((_ap += 8), (*(_type *)(_ap - 8)))                            \
             : ((_ap += 4), (*(_type *)(_ap - 4))))))
#endif

#define va_end(ap)

#ifdef __cplusplus

#ifndef _CPP_STYLE_HEADER
using std::va_list;
#endif /* _CPP_STYLE_HEADER */

#endif /* __cplusplus */

#else

#ifdef __cplusplus

#ifndef _CPP_STYLE_HEADER
using std::va_list;
#endif /* _CPP_STYLE_HEADER */

#endif /* __cplusplus */

#endif /* _STDARG */
