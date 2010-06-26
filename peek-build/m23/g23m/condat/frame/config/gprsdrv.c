/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  Driver Table for GPRS
+----------------------------------------------------------------------------- 
*/ 

#ifndef __GPRSDRV_C__
#define __GPRSDRV_C__
#endif

#ifndef _TARGET_
#define NEW_ENTITY
#endif

#ifdef _TARGET_
  #ifdef FF_TRACE_OVER_MTST
    #define MTST_TRACE
  #else
    #define TI_TRACE
  #endif
#endif

#include "gdi.h"
#include "vsi.h"
#include "pei.h"
#include "gprsconst.h"
#include "frm_defs.h"
#include "frm_types.h"

/*==== TYPES ======================================================*/


/*==== CONSTANTS ==================================================*/

#if defined _TARGET_ && !defined PCON
 #define TR_RCV_BUF_SIZE    1024
#else
 #define TR_RCV_BUF_SIZE    1024
#endif

#define TR_MAX_IND         (TR_RCV_BUF_SIZE-1)

/*==== EXTERNALS ==================================================*/

#ifdef TI_TRACE
extern USHORT TIF_Init   ( USHORT DrvHandle, T_DRV_CB_FUNC CallbackFunc,
                           T_DRV_EXPORT const **DrvInfo );
extern USHORT TR_Init    ( USHORT DrvHandle, T_DRV_CB_FUNC CallbackFunc,
                           T_DRV_EXPORT const **DrvInfo );
extern USHORT TITRC_Init ( USHORT DrvHandle, T_DRV_CB_FUNC CallbackFunc,
                           T_DRV_EXPORT const **DrvInfo );
#else
extern USHORT TIF_Init     ( USHORT DrvHandle, T_DRV_CB_FUNC CallbackFunc,
                             T_DRV_EXPORT const **DrvInfo );
extern USHORT TR_Init      ( USHORT DrvHandle, T_DRV_CB_FUNC CallbackFunc,
                             T_DRV_EXPORT const **DrvInfo );
extern USHORT SER_Init     ( USHORT DrvHandle, T_DRV_CB_FUNC CallbackFunc,
                             T_DRV_EXPORT const **DrvInfo );
#endif
extern USHORT mux_Init ( USHORT DrvHandle, T_DRV_CB_FUNC CallbackFunc,
                         T_DRV_EXPORT const **DrvInfo );
/*==== VARIABLES ==================================================*/

ULONG TR_RcvBufferSize = TR_RCV_BUF_SIZE;
ULONG TR_MaxInd = TR_MAX_IND;

const T_DRV_LIST_ENTRY DrvList[] =
{
  { NULL,   NULL,         NULL,  NULL },
#ifdef TI_TRACE
  #if defined _TARGET_
    { "TIF",  TIF_Init,     "RCV", NULL },
  #else
    { "TIF",  TIF_Init,     "RCV", "ENABLE_PCON" },
  #endif
  { "TR",   TR_Init,      NULL,  NULL },
  { "TITRC",TITRC_Init,   NULL,  "" },
#else 
  #ifdef MTST_TRACE 
  #if defined _TARGET_
    { "TIF",  TIF_Init,     "RCV", NULL },
  #else
    { "TIF",  TIF_Init,     "RCV", "ENABLE_PCON" },
  #endif
  { "TR",   TR_Init,      NULL,  NULL },
  { "MUX",  mux_Init,     NULL,  "" },
  #else
  #if defined _TARGET_
    { "TIF",  TIF_Init,     "RCV", NULL },
  #else
    { "TIF",  TIF_Init,     "RCV", "ENABLE_PCON" },
  #endif
  { "TR",   TR_Init,      NULL,  NULL },
  { "SER",  SER_Init,     NULL,  ""   },
  #endif
#endif
  { NULL,   NULL,         NULL,  NULL }
};

int vsi_o_trace (char *caller, ULONG tclass, char *text,...)
{
  return 0;
}
