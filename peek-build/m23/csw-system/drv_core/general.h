/****************************************************************************/
/*                                                                          */
/*  Name        general.h                                                   */
/*                                                                          */
/*  Function    this file contains common data type definitions used        */
/*              throughout the SWE                                          */
/*                                                                          */
/*  Date       Modification                                                 */
/*  -----------------------                                                 */
/*  3/12/99    Create                                                       */
/* **************************************************************************/
/*  10/27/1999 David Lamy-Charrier: remove declaration of ntohs, htons,     */
/*                                  ntohl, htonl in order to avoid conflict */
/*                                  with winsock.h                          */
/*                                                                          */
/*  11/30/1999 Pascal Pompei: 'string.h' included in order to define memcmp,*/
/*                            memset and memcpy functions.                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/


#ifndef GENERAL_H
#define GENERAL_H

#ifdef _WINDOWS
   #include <string.h>
#endif
#undef WIN32
//TISH modified for MSIM
//begin
#include "swconfig.cfg"

#if !defined NUCLEUS && !defined __TYPEDEFS_H__ 
/* UINT16 is already defined in the nucleus.h for the arm9 */
   typedef unsigned short  UINT16;
#endif
     

/* WINDOWS */
//#ifdef _WINDOWS
#ifdef WIN32
   typedef short           INT16; 
   typedef int             INT32;
   typedef unsigned char   BOOLEAN;
   typedef unsigned char   UINT8;
   typedef signed char     INT8;
   typedef unsigned short  UINT16;
#ifndef UINT32
   typedef unsigned int    UINT32;
#endif
#ifndef UBYTE
   typedef unsigned char   UBYTE;
#endif
#ifndef SHORT
   typedef short           SHORT;
#endif
#ifndef USHORT
   typedef unsigned short           USHORT;
#endif
#ifndef ULONG
   typedef unsigned long           ULONG;
#endif
#ifndef BOOL
   typedef int             BOOL;
#endif
   
/* BOARD */
#else

   #ifndef __INCvxTypesOldh
    #if !defined NUCLEUS || !defined PLUS_VERSION_COMP
     typedef unsigned char   BOOLEAN;
	 typedef unsigned long   UINT32;
	 typedef unsigned char   UINT8;
	 typedef signed char     INT8;
	 typedef short           INT16; 
	 typedef int             INT32;
    #endif
   #endif
   
   #ifndef __TYPEDEFS_H__ /* This #define allows to Condat to use general.h without conflict */
	  typedef unsigned char   UBYTE;
      typedef short           SHORT;
      typedef signed char     BYTE;
      #if !defined (BOOL_FLAG)
         #define BOOL_FLAG
         typedef unsigned char BOOL;
      #endif
      typedef unsigned short  USHORT;
      typedef unsigned int    ULONG;
   #endif
  #endif



typedef void (*FUNC)(void);      /* pointer to a function */

#define OK        1

#ifndef NULL
   #define NULL   0
#endif

#ifndef TRUE
   #define TRUE   1
#endif

#ifndef FALSE
   #define FALSE  0
#endif


#define htons  ntohs
#define htonl  ntohl


#if !defined(_WIN32) 
   #define ntohs(n) (n)
   #define ntohl(n) (n)
   #define ntoh6(n) (n)
#endif

/* HISR_STACK_SHARING: Create global stacks to be used by all HISRs
 * having the same priority */

#if (LOCOSTO_LITE)
#ifndef HISR_STACK_SHARING
#define HISR_STACK_SHARING
#endif
#ifdef HISR_STACK_SHARING
extern unsigned char HISR_STACK_PRIO2[1500];
#define HISR_STACK_PRIO2_SIZE sizeof(HISR_STACK_PRIO2)
#endif
#endif

#endif /* #ifndef GENERAL_H */
