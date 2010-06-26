/*****************************************************************************
 * $Id: ngos.h,v 1.12.2.1 2002/09/27 18:17:43 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Global Definitions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2002 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *    This software is  supplied  under  the terms of a
 *    license agreement or nondisclosure agreement with
 *    NexGen Software, and may not be copied or disclosed
 *    except  in  accordance  with  the  terms of  that
 *    agreement.
 *
 *----------------------------------------------------------------------------
 * 18/07/2000 - Regis Feneon
 *  new file for v1.2
 * 24/08/2000 -
 *  new data type NGcfgarg
 * 19/09/2000 -
 *  new ngInit() error flags for device/interface open failures
 * 25/01/2001 -
 *  added ngFlush()
 *  ngos/isr.h & hw.h removed - NG_INTRCTL_XXX declarations moved here
 * 04/03/2001 -
 *  added global data ngOSData, support for modules
 * 17/04/2001 -
 *  set network timer resolution (1/NG_TIMERHZ) to 500ms
 * 17/05/2001 -
 *  corrected NG_CFG_FNC() macro for IAR
 * 28/05/2001 -
 *  corrected ngInitGetFailedXXX() macros
 * 30/04/2002 -
 *  set NG_CPU_UNDEFINED if cpu type is not defined by the makefile,
 *  in this case ngdefs.h must include the right macros.h file
 * 25/06/2002 - dg
 *  add support fo h8s processor
 * 19/09/2002 - Regis Feneon
 *  added support for ARC processors
 * 25/09/2002 -
 *  added support for MIPS processors
 *****************************************************************************/

#ifndef __NG_NGOS_H_INCLUDED__
#define __NG_NGOS_H_INCLUDED__

#ifndef _WINDOWS
#define NG_CPU_ARM
#endif

/* cpu-specific includes */
#if defined(NG_CPU_I186S) || defined(NG_CPU_I186M) || defined(NG_CPU_I186C) \
 || defined(NG_CPU_I186L) || defined(NG_CPU_I286)
/* 80x86 16-bits */
#include <ngos/cpu/i186/macros.h>
#elif defined(NG_CPU_I386) || defined(NG_CPU_I486)
/* 80x86 32-bits */
#include <ngos/cpu/i386/macros.h>
#elif defined(NG_CPU_ARM)
/* ARM */
#include <ngos/cpu/arm/macros.h>
#elif defined(NG_CPU_PPC)
/* PowerPC */
#include <ngos/cpu/ppc/macros.h>
#elif defined(NG_CPU_SH3) || defined(NG_CPU_SH4)
/* Hitachi SH3/SH4 */
#include <ngos/cpu/sh7x/macros.h>
#elif defined(NG_CPU_H8S)
/* Hitachi H8S */
#include <ngos/cpu/h8s/macros.h>
#elif defined(NG_CPU_68K)
/* Motorola 680x0 and CPU32 */
#include <ngos/cpu/m68k/macros.h>
#elif defined(NG_CPU_M16C)
/* Mitsubishi M16C/60 */
#include <ngos/cpu/m16c/macros.h>
#elif defined(NG_CPU_ST20C1) || defined(NG_CPU_ST20C2)
/* STMicro ST20 */
#include <ngos/cpu/st20/macros.h>
#elif defined(NG_CPU_C166) || defined(NG_CPU_C167)
/* Infineon C166/C167/C16X - STMicro ST10 */
#include <ngos/cpu/c166/macros.h>
#elif defined(NG_CPU_ARC)
/* ARC */
#include <ngos/cpu/arc/macros.h>
#elif defined(NG_CPU_MIPS)
/* MIPS */
#include <ngos/cpu/mips/macros.h>
#else
/* auto-detection in ngdefs.h ? */
#define NG_CPU_UNDEFINED
#endif

/* rtos definitions */
#include <ngdefs.h>

/* standard includes */
#include <ngos/debug.h>
#include <ngos/errno.h>
#include <ngos/queue.h>
#include <ngos/ng_stdio.h>
#include <ngos/ng_stdlib.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/* Settings */
#define NG_TIMERHZ 2 /* 500ms stack timer */
#define NG_FD_MAX 32 /* maximum number of 'file' descriptors */

/* NexGenOS configuration table entry */
#ifndef NG_CFGARG
typedef unsigned long NGcfgarg;
#endif
typedef struct {
    u_int cfg_option; /* option or command */
    NGcfgarg cfg_arg; /* argument */
} NGcfgent;

/* configuration commands */
#define NG_CFG_NOP 0xff00
#define NG_CFG_END 0xffff
#define NG_CFG_PROTOADD 0xff01
#define NG_CFG_IFADD 0xff02
#define NG_CFG_DEVADD 0xff03
#define NG_CFG_DRIVER 0xff04
#define NG_CFG_IFADDWAIT 0xff05
/* global stack options */
#define NG_BUFO_MAX 0xfe00
#define NG_BUFO_HEADER_SIZE 0xfe01
#define NG_BUFO_DATA_SIZE 0xfe02
#define NG_BUFO_ALLOC_F 0xfe03
#define NG_BUFO_INPQ_MAX 0xfe04
#define NG_SOCKO_MAX 0xfe05
#define NG_SOCKO_TABLE 0xfe06
#define NG_SOCKO_SEMTABLE 0xfe07
#define NG_RTO_INPUT_PRIO 0xfe0a
#define NG_RTO_INPUT_STACK_SIZE 0xfe0b
#define NG_RTO_INPUT_STACK 0xfe0c
#define NG_RTO_TIMER_PRIO 0xfe0d
#define NG_RTO_TIMER_STACK 0xfe0e
#define NG_RTO_TIMER_STACK_SIZE 0xfe0f
#define NG_DEBUGO_LEVEL 0xfe10
#define NG_DEBUGO_MODULE 0xfe11
/* device io */
#define NG_DEVCBO_MAX 0xfe12
#define NG_DEVCBO_TABLE 0xfe13
#define NG_DEVCBO_SEMTABLE 0xfe14
/* extensions */
#define NG_BUFO_ALLOC_BUF_F 0xfe15
#define NG_BUFO_POOL_PTR 0xfe16
#define NG_BUFO_POOL_SIZE 0xfe17
/* v1.2 options */
#define NG_DEVCBO_PRIO 0xfe18
#define NG_DEVCBO_STACK 0xfe19
#define NG_DEVCBO_STACK_SIZE 0xfe20
#define NG_BUFO_FREE_F 0xfe21
#define NG_BUFO_FREE_BUF_F 0xfe22
/* deprecated options */
#ifdef XXX
#define NG_SOCKO_SEMSIZE 0xfe08
#define NG_RTO_CLOCK_FREQ 0xfe09
#define NG_RTO_SEMSIZE NG_SOCKO_SEMSIZE
#define NG_DEVCBO_SEMSIZE NG_SOCKO_SEMSIZE
#endif

/* arguments */
#define NG_CFG_TRUE ((NGcfgarg) ~0L)
#define NG_CFG_FALSE ((NGcfgarg) 0L)
#define NG_CFG_LNG( a) ((NGcfgarg) (a))
#define NG_CFG_ADR(a,b,c,d) ((NGcfgarg) NG_INADDR(a,b,c,d))
#ifdef NG_BIG_ENDIAN
#ifdef NG_CPU_SHORTINT
#define NG_CFG_INT( a) ((NGcfgarg) ((a)<<16))
#else
#define NG_CFG_INT( a) ((NGcfgarg) (a))
#endif
#ifdef NG_CPU_SHORTPTR
#define NG_CFG_PTR( a) ((NGcfgarg) ((a)<<16))
#else
#define NG_CFG_PTR( a) ((NGcfgarg) (a))
#endif
#ifdef NG_CPU_SHORTFNC
#ifdef NG_COMP_IAR
#define NG_CFG_FNC( a) (a<<16)
#else
#define NG_CFG_FNC( a) ((NGcfgarg) ((a)<<16))
#endif
#else
#ifdef NG_COMP_IAR
#define NG_CFG_FNC( a) a
#else
#define NG_CFG_FNC( a) ((NGcfgarg) (a))
#endif
#endif
#else
#define NG_CFG_INT( a) ((NGcfgarg) (a))
#define NG_CFG_PTR( a) ((NGcfgarg) (a))
#ifdef NG_COMP_IAR
#define NG_CFG_FNC( a) a
#else
#define NG_CFG_FNC( a) ((NGcfgarg) (a))
#endif
#endif

/* modules control function */
#define NG_CNTL_SET 1
#define NG_CNTL_GET 0

/*
 * Global Data
 */

#ifndef NG_RTOS

/* Polling interface */
typedef struct NGpollent_S {
    struct NGpollent_S *poll_next; /* pointer to next entry */
    void (*poll_handler_f)( void *); /* polling routine */
    void *poll_handler_data; /* routine data */
} NGpollent;

#endif

typedef struct {
  const char *os_name; /* os/kernel name */
  int os_ver_num; /* os port version number*100 */
  NGubyte os_ver_letter; /* os port version letter */
  NGubyte os_date_year; /* os port build date (years since 1900) */
  NGubyte os_date_month; /* os port build date (month: 1=january) */
  NGubyte os_date_day; /* os port build date (day of the month) */
  struct NGmoddata_S *os_modlist; /* list of initialized modules */
  const struct NGmoddrv_S *os_modfailed; /* pointer to unitialized module */
  int os_moderror; /* error code of unitialized module */
  unsigned os_clockfreq; /* system clock frequency (ticks/sec) */
  unsigned long os_timernticks; /* number of ticks for the stack timer */
#ifdef NG_RTOS
  int os_mutexsize; /* size of NGOSmutex */
  int os_semsize; /* size of NGOSsem */
  int os_ieventsize; /* size of NGOSievent */
  int os_tasksize; /* size of NGOStask */
#else
  NGpollent *os_pollist; /* list of polling function */
  unsigned long os_timerlast; /* last clock value for ngYield() */
  int os_errno; /* a global errno */
#endif
} NGosdata;

extern NGosdata ngOSData;

/* NGconst NGmoddrv  *ngInitGetFailedPtr( void); */
/* int                ngInitGetFailedError( void); */
#define ngInitGetFailedPtr() (ngOSData.os_modfailed)
#define ngInitGetFailedError() (ngOSData.os_moderror)

#ifdef NG_RTOS
/* global synchronization */
extern NGOSmutex ngGlobalLock;
#define NG_LOCK() ngOSMutexLock( &ngGlobalLock)
#define NG_UNLOCK() ngOSMutexUnlock( &ngGlobalLock)
#else
#define NG_LOCK()
#define NG_UNLOCK()
#endif

/* Errors returned by ngInit() */
#define NG_EINIT_NOERROR NG_EOK /* initialization ok */
#define NG_EINIT_BUFALLOC 0x0001 /* error allocating buffers */
#define NG_EINIT_BUFOPTION 0x0002 /* bad buffer option */
#define NG_EINIT_SOCKTABLE 0x0004 /* bad socket table option */
#define NG_EINIT_DEVCBTABLE 0x0008 /* bad devive control blocks option */
#define NG_EINIT_CLOCKFREQ 0x0010 /* bad clock frequency */
#define NG_EINIT_INPUTTASK 0x0020 /* cannot create input task */
#define NG_EINIT_TIMERTASK 0x0040 /* cannot create timer task */
#define NG_EINIT_DEVIOTASK 0x0080 /* cannot create device task */
#define NG_EINIT_DEVOPEN 0x0100 /* device open failure */
#define NG_EINIT_IFOPEN 0x0200 /* interface open failure */
#define NG_EINIT_PROTOCOL 0x0400 /* bad protocol */
#define NG_EINIT_MODULE 0x0800 /* error in module initialization */

/*
 * Functions prototypes
 */

/* system interface */
#ifndef NG_EVAL_VERSION
int ngInit( const NGcfgent *cfg);
#endif
void ngExit( int status);
char *ngGetVersionString( void);
void ngSleep( u_long msec);
#ifndef NG_RTOS
void ngYield( void);
int ngPollAdd( NGpollent *pent, void (*func)( void *), void *data);
void ngUserSetWorkProc( int (*func)( void *), void *data);
#endif
#ifndef NG_RTOS_INLINE
u_long ngOSClockGetTime( void);
void ngOSExit( int status);
void ngOSInit( void);
void ngOSSleep( u_long dly);
int ngOSIsrAttach( int irno, void (*handler_f)(void *), void *data);
int ngOSIsrDetach( int handle);
int ngOSIntrCtl( int ctl);
int ngOSMemKTOM( void *ptr, NGuint *addr);
int ngOSMemMTOK( NGuint addr, int len, void **ptr);
#endif
#ifndef NG_RTOS
/* global errno */
#define ngOSGetErrno() (ngOSData.os_errno)
#define ngOSSetErrno( err) ngOSData.os_errno = (err);
#endif

/* standard values for ngIntrCTl()/ngOSIntrCtl() */
#define NG_INTRCTL_ENABLE 0x1000
#define NG_INTRCTL_DISABLE 0x1001

/* standart libs replacement functions */
int ngRawPrintf( void (*out_f)( int, void *), void *data,
                     const char *format, NGva_list args);
void ngStdOutChar( int c, void *data);
int ngStdInChar( void *data);
int ngVSPrintf( char *buf, const char *format, NGva_list args);
int ngSPrintf( char *buf, const char *format, ...);
int ngVPrintf( const char *format, NGva_list args);
int ngPrintf( const char *format, ...);
int ngFlush( void);
int ngGetChar( void);
NGuint ngRandom( NGuint *seed);
#define NG_RAND_MAX 0xffffffffUL
int ngATOI( const char *s);
long ngATOL( const char *s);
NGuint ngATOH( const char *s);
char *ngStrTok( char *s, const char *sep, char **savs);
const char *ngStrError( int err);

#ifdef NG_RTOS
/* rtos interface */
#ifndef NG_RTOS_INLINE
int ngOSGetErrno( void);
int ngOSIsrEventDelete( NGOSievent *event);
int ngOSIsrEventInit( NGOSievent *event);
int ngOSIsrEventPost( NGOSievent *event);
int ngOSIsrEventTaskPost( NGOSievent *event);
int ngOSIsrEventWait( NGOSievent *event);
int ngOSMutexDelete( NGOSmutex *mutex);
int ngOSMutexInit( NGOSmutex *mutex);
int ngOSMutexLock( NGOSmutex *mutex);
int ngOSMutexUnlock( NGOSmutex *mutex);
int ngOSSemClear( NGOSsem *sem);
int ngOSSemDelete( NGOSsem *sem);
int ngOSSemInit( NGOSsem *sem, int value);
int ngOSSemPost( NGOSsem *sem);
int ngOSSemTimedWait( NGOSsem *sem, u_long dly);
int ngOSSemWait( NGOSsem *sem);
void ngOSSetErrno( int err);
int ngOSTaskCreate( NGOStask *th, void (*task)( void *data), void *data,
                        int prio, void *stack_ptr, int stack_size);
void ngOSTaskExit( void);
int ngOSTaskDelete( NGOStask *th);
#endif
#endif
/* NexGenOS initialization - ngOSInit() */
#define NGOS_INIT_NAME( name) { ngOSData.os_name = (name); }
#define NGOS_INIT_VERSION( maj, min, letter) { \
  ngOSData.os_ver_num = maj*100+min; \
  ngOSData.os_ver_letter = letter; }
#define NGOS_INIT_DATE( day, month, year) { \
  ngOSData.os_date_year = year-1900; \
  ngOSData.os_date_month = month; \
  ngOSData.os_date_day = day; }
#define NGOS_INIT_CLOCKFREQ( val) { ngOSData.os_clockfreq = (val); }
#define ngOSClockGetFreq() (ngOSData.os_clockfreq)
#ifdef NG_RTOS
#define NGOS_INIT_VARSIZE() { \
  ngOSData.os_mutexsize = sizeof(NGOSmutex); \
  ngOSData.os_semsize = sizeof(NGOSsem); \
  ngOSData.os_ieventsize = sizeof(NGOSievent);\
  ngOSData.os_tasksize = sizeof(NGOStask); }
#define ngOSMutexGetSize() (ngOSData.os_mutexsize)
#define ngOSSemGetSize() (ngOSData.os_semsize)
#define ngOSIsrEventGetSize() (ngOSData.os_ieventsize)
#define ngOSTaskGetSize() (ngOSData.os_tasksize)
#endif

/* standart macros */
#define NG_ABS( a) ((a)>=0?(a):-(a))
#define NG_MAX( a, b) ((a)>(b)?(a):(b))
#define NG_MIN( a, b) ((a)<(b)?(a):(b))

/* NG_END_DECLS // confuses Source Insight */

#endif

