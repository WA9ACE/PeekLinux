/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_sys.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 5               $|
| CREATED: 21.09.98                     $Modtime:: 8.07.99 9:10     $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SYS

   PURPOSE : MFW system interface

   EXPORT  :

   TO DO   :

   $History:: mfw_sys.h                                             $
 *
 * *****************  Version 5  *****************
 * User: Es           Date: 8.07.99    Time: 12:04
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 4  *****************
 * User: Es           Date: 6.07.99    Time: 12:30
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 3  *****************
 * User: Es           Date: 17.02.99   Time: 19:11
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 23.12.98   Time: 16:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
*/

#ifndef _DEF_MFW_SYS_H_
#define _DEF_MFW_SYS_H_

#ifdef __BORLANDC__
#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

#define EXTERN extern
#define GLOBAL
#define LOCAL static
#define ULONG unsigned long
#define LONG long
#define USHORT unsigned short
#define SHORT short
#define UBYTE unsigned char
#define BYTE unsigned char

#define sysAlloc(s) malloc(s)
#define sysFree(m) free(m)
typedef int T_VSI_THANDLE;
typedef unsigned long T_VSI_TVALUE;
#define VSI_OK 0
#define vsi_o_ptrace(a,b)
#define vsi_c_awake(a) timTimeout()
#ifdef VSI_CALLER
#undef VSI_CALLER
#endif
#define VSI_CALLER
#ifdef TRACE_EVENT
#undef TRACE_EVENT
#endif
void TRACE_EVENT (char *a);
//#define TRACE_EVENT(a);
#ifdef TRACE_ERROR
#undef TRACE_ERROR
#endif
void TRACE_ERROR (char *a);
//#define TRACE_ERROR(a);
#ifdef _ENTITY_PREFIXED
#undef _ENTITY_PREFIXED
#define _ENTITY_PREFIXED(a) (a)
#endif

#else

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#if defined (NEW_FRAME)
 #ifdef MEMORY_SUPERVISION
   #define sysAlloc(s) vsi_c_new(VSI_CALLER s,0, __FILE__, __LINE__)
   #define sysFree(m) vsi_c_free(VSI_CALLER (T_VOID_STRUCT**) &(m), __FILE__, __LINE__)
 #else
   #define sysAlloc(s) vsi_c_new(VSI_CALLER s,0)
   #define sysFree(m) vsi_c_free(VSI_CALLER (T_VOID_STRUCT**) &(m))
 #endif
#else
#define sysAlloc(s) vsi_c_new(VSI_CALLER s)
#define sysFree(m) vsi_c_free(VSI_CALLER &(m))
#endif


#endif

#endif

