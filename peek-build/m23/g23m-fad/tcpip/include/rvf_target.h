/****************************************************************************/
/*                                                                          */
/*  Name        rvf_target.h                                                */
/*                                                                          */
/*  Function    this file contains definitions that will probably      */
/*              change for each target system. This includes        */
/*              such things number of tasks,                          */
/*              little endian/big endian conversions, etc...        */
/*                                      */
/*                                                                          */
/*  NOTE        This file should always be included first.          */
/*                                      */
/*  Version    0.1                              */
/*                                      */
/*   Date         Modification                        */
/*  ------------------------------------                  */
/*  4/19/1999  Create                            */
/*  10/27/1999  remove declaration of ntohs, htons, ntohl, htonl      */
/*        in order to avoid conflict with winsock.h          */
/*  12/23/1999  add buffer and memory bank related definition        */
/*  07/12/2000  implement dynamic memory allocation.            */
/*                                      */
/*  Author    David Lamy-Charrier (dlamy@tif.ti.com)            */
/*                                      */
/* (C) Copyright 1999 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/
#ifndef _RVF_TARGET_H
#define _RVF_TARGET_H

#ifndef _SIMULATION_
  #include "rv.cfg"
  #include "swconfig.cfg"
#endif
  
/* optimizations */
#ifdef _WINDOWS
  #define INLINE __inline
#else
  #define INLINE inline
#endif

/*----------------------------------------------------------------------*/
/*                      RVF Definitions                                 */
/*----------------------------------------------------------------------*/

/* the maximum number of memory bank created by users */
#if (!GSMLITE)
#define RVF_MAX_TOTAL_MB 70
#else // GSMLITE
  #define RVF_MAX_TOTAL_MB 18
#endif // GSMLITE

/* the maximum number of real memory bank managed */
#if (!GSMLITE)
#define RVF_MAX_REAL_MB 60
#else // GSMLITE
  #define RVF_MAX_REAL_MB 18
#endif // GSMLITE

/* max length of memory bank name */
#define RVF_MAX_MB_LEN 10

/* max length of task name */
#define RVF_MAX_TASK_LEN 10

/* RVF_MB_MAPPING defines the correspondance between mb_name and mb_id */
/* WARNING!!!! When adding new memory banks in this default list, take care */
/* to always finish with the {0,0} !!! */
#if (!GSMLITE)
#define RVF_MB_MAPPING {{ "RVM_PRIM", 0},{ "TEST1", 1}, { "TEST2", 2},\
  { "TEST3", 3}, { "TEST4", 3},{"HCI_RX", 4},{"HCI_TX", 5},\
  {"L2CAP_PRIM", 6},{"L2CAP_RX", 7},{"L2CAP_TX", 8},{"SPP_TX", 9},\
  {"SPP_RX", 10},{"SPP_PRIM", 11},{"BTCTRL", 12},{"RV_TRACE", TRACE_MB_ID},\
  {"RFC_TX", 13},{"RFC_PRIM", 14},{"RFC_01_RX", 15},{"RFC_02_RX", 16},\
  {"RFC_03_RX", 17},{"RFC_04_RX", 18},{"RFC_05_RX", 20}, {"APP1_RX", 21},\
  {"APP1_TX", 22},{"APP1_PRIM", 23}, {"SDP_PRIM", 24}, {"SDP_RX", 25}, \
  {"SDP_TX", 26}, \
  {"ATP_PRIM",27},{"ATP_UART",27},{"HSG_PRIM",28},{"HSG_TX",29},{"HSG_RX",30},\
  {"HS_PRIM",28},{"HS_TX",29},{"HS_RX",30},\
  {"RVMMI_PRIM",31},{"RVMMI_TX",32},{"RVMMI_RX",33},\
  {"UWIRE_PRIM",37},\
  {"DUN_PRIM",28},{"DUN_TX",29},{"DUN_RX",30},\
  {"FAX_PRIM",28},{"FAX_TX",29},{"FAX_RX",30},{0,0}}
#else // GSMLITE
  #define RVF_MB_MAPPING {{ "RVM_PRIM", 0},{ "TEST1", 1}, { "TEST2", 2},\
              { "TEST3", 3}, { "TEST4", 3}, {0,0}}
#endif // GSMLITE

/* set the following variable to 1 to check for buffer corruption*/
#define RVF_ENABLE_BUF_CORRUPTION_CHECK 1

/* set the following variable to 1 to check for buffer linkage*/
#define RVF_ENABLE_BUF_LINKAGE_CHECK 1

/* enable or disable memory statistics functions */
#define RVF_ENABLE_STATS 1

/* The maximum number of RVF tasks allowed in the system*/
#if (!GSMLITE)
#define MAX_RVF_TASKS 20
#else // GSMLITE
  #define MAX_RVF_TASKS 9
#endif // GSMLITE

/* max number of callback functions for buffers waiting to be counted on another memory bank */
#if (!GSMLITE)
#define RVF_MAX_WAITING_BUF 300
#else // GSMLITE
  #define RVF_MAX_WAITING_BUF 20
#endif // GSMLITE
/* sum of RVF_BUFi_MAX */

/***********************************************************************
** Timer related definitions. These should be edited per system. The
** macros should convert milliseconds and seconds to system ticks.
** Applications should use these to calculate the number of ticks to
** sleep for.
*/

#ifdef _WINDOWS
#define RVF_MS_TO_TICKS(x) (x / 50)
#define RVF_SECS_TO_TICKS(x) ((x * 1000) / 50)
#define RVF_TICKS_TO_MS(x) (x * 50)
#else
#define RVF_MS_TO_TICKS(x) (x /5)
#define RVF_SECS_TO_TICKS(x) ((x * 1000) /5)
#define RVF_TICKS_TO_MS(x) (x * 5)
#endif

#endif /* _RVF_TARGET_H */

