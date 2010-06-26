/*****************************************************************************/
/*  FORMAT.H v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Structures and macros used in printf and scanf                            */
/*****************************************************************************/
#ifndef __FORMAT_H
#define __FORMAT_H
#include <stdarg.h>

#define _ARSIZE 30

#ifdef __cplusplus
extern "C" namespace std {
#else 
extern
#endif

  int _scanfi(void *inp, const char *_format, va_list _ap,
	      int (*_chkmbc)(void **inp, char **_format, int *num_read),
	      int (*_inpchar)(void **inp),
	      void (*_uninpchar)(void **inp, char outchar));
 
  /**************************************************************************/
  /* _FIELD STRUCTURE AND MACROS USED FOR PRINTF AND SCANF                  */
  /**************************************************************************/
  typedef struct {
    unsigned int flags;         /* Format flags */
    int          fwidth;        /* Field width */
    int          precision;     /* Field precision */
    char         conv;          /* Conversion specifier */
  } _PFIELD;
  
  typedef struct {
    unsigned int flags;         /* Format flags */
    int          fwidth;        /* Field width */
    int          precision;     /* Field precision */
    char         scanset[96];   /* Scanset used for '[' conversion */
    char         conv;          /* Conversion specifier */
  } _SFIELD;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/****************/
/* PRINTF FLAGS */
/****************/

#define _PFMINUS  0x01
#define _PFPLUS   0x02
#define _PFSPACE  0x04
#define _PFPOUND  0x08
#define _PFZERO   0x10
 
/***************/
/* SCANF FLAGS */
/***************/

#define _SFSTAR   0x01
#define _SFCIRC   0x02

/****************/
/* MUTUAL FLAGS */
/****************/

#define _MFH      0x20
#define _MFL      0x40
#define _MFLD     0x80
#define _MFLL     0x100

#endif
