/*****************************************************************************
 * $Id: macros.h,v 1.8 2002/05/24 10:04:10 rf Exp $
 * $Name:  $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * ARM Architecture
 * Compiler dependant macros
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
 * 19/11/99 - Regis Feneon
 * 10/02/2000 -
 *  added NG_ALIGN32 and NG_ALIGN_STRICT
 *  added NG_VA_XXX macros
 * 30/05/2000 -
 *  added NG_LITTLE_ENDIAN_BITFIELD/NG_BIG_ENDIAN_BITFIELD
 * 21/07/2000 -
 *  types.h moved here
 *  ngHTONS()/ngNTOHS() macros replaced by ngBSwap16()/ngBSwap32()
 *  standard libs moved to ngos/stdlib.h
 *  added include of unaligned data handling
 *  added support for ADS
 *  added ngCpuIntrCtl() macro
 * 14/12/2000 -
 *  added ngARMSwi() routine
 * 08/10/2001 -
 *  added support for Green Hills
 * 23/01/2002 -
 *  added support for Texas Instruments TMS470 tools
 * 23/05/2002 -
 *  corrected GHS macros
 *****************************************************************************/

#ifndef __NG_MACROS_H_INCLUDED__
#define __NG_MACROS_H_INCLUDED__

/* C++ compatibility */
#if defined(__cplusplus) || defined(__CPLUSPLUS)
#define NG_BEGIN_DECLS extern "C" {
#define NG_END_DECLS }
#else
#define NG_BEGIN_DECLS
#define NG_END_DECLS
#endif

/* NG_BEGIN_DECLS // confuses Source Insight */

/* standart c-types */
#ifndef NG_UTYPES
typedef unsigned int u_int; /* unsigned >= 16 bits */
typedef unsigned long u_long; /* unsigned >= 32 bits */
#endif

/* fixed size types */
typedef unsigned char NGubyte; /* 8 bits unsigned */
typedef unsigned short NGushort; /* 16 bits unsigned */
typedef unsigned int NGuint; /* 32 bits unsigned */
typedef signed char NGbyte; /* 8 bits signed */
typedef signed short NGshort; /* 16 bits signed */
typedef signed int NGint; /* 32 bits signed */

/* hardcoded for TI implementation */
#define NG_COMP_TI470
#define NG_LITTLE_ENDIAN

/* byte ordering */
#if defined(NG_LITTLE_ENDIAN)
#define NG_LITTLE_ENDIAN_BITFIELD
#elif defined(NG_BIG_ENDIAN)
#define NG_BIG_ENDIAN_BITFIELD
#else
#error "NG_XXX_ENDIAN macro not defined"
#endif

#define NGfar
#define NGcodefar

#define NG_ALIGN32
#define NG_ALIGN_STRICT

#if defined(NG_COMP_ASDT) || defined(NG_COMP_ADS)
/*
 * ARM Tools
 */

#define NG_CFGARG
typedef void *NGcfgarg; /* can store pointers or integers */

/* byte-swapping */
#if defined(NG_CODE_ARM)
extern __inline NGushort ngBSwap16( NGushort a)
{
  int tmp;
  __asm {
    mov tmp, a, lsr #8
    orr tmp, tmp, a, ror #24
    bic a, tmp, #0xff0000
  }
  return( a);
}
extern __inline NGuint ngBSwap32( NGuint a)
{
  int tmp;
  __asm {
    eor tmp, a, a, ror #16
    bic tmp, tmp, #0xff0000
    mov a, a, ror #8
    eor a, a, tmp, lsr #8
  }
  return( a);
}
#else
extern __inline NGushort ngBSwap16( NGushort a)
{
  int tmp;
  __asm {
    mov tmp, a
    lsr tmp, #8
    ror a, #24
    orr a, tmp
    bic a, #0xff0000
  }
  return( a);
}
extern __inline NGuint ngBSwap32( NGuint a)
{
  int tmp;
  __asm {
    mov tmp, a
    ror tmp, #16
    eor tmp, a
    bic tmp, #0xff0000
    ror a, #8
    lsr tmp, tmp, #8
    eor a, tmp
  }
  return( a);
}
#endif

/* interrupts */
#if defined(NG_CODE_ARM)
extern __inline void ngDisable( void)
{
  int tmp;
  __asm {
    MRS tmp, CPSR
    ORR tmp, tmp, #0x80
    MSR CPSR_c, tmp
  }
}
extern __inline void ngEnable( void)
{
  int tmp;
  __asm {
    MRS tmp, CPSR
    BIC tmp, tmp, #0x80
    MSR CPSR_c, tmp
  }
}
extern __inline int ngIntrCtl( int ctl)
{
  int tmp;
  __asm {
    ANDS ctl, ctl, #0x01
    MRS tmp, CPSR
    MOV ctl, tmp, lsr #7
    AND ctl, ctl, #0x01
    ORR ctl, ctl, #0x1000
    ORRNE tmp, tmp, #0x80
    BICEQ tmp, tmp, #0x80
    MSR CPSR_c, tmp
  }
  return( ctl);
}
#elif defined(NG_CODE_THUMB)
void ngDisable( void);
void ngEnable( void);
int ngIntrCtl( int);
#else
#error "NG_CODE_XXX macro not defined"
#endif

/* interrupt routines (must be written in assembly) */
#define NG_INTERRUPT
typedef void (*NGinterruptPTR)( void);

/* Software Interrupt */
/* we must save lr in case it is called from SVC mode */
#define ngARMSwi( code) __asm { SWI code,{},{},{lr} }

/* variable argument list */
#if defined(NG_COMP_ADS)
/* use ADS compiler builtin macros */
typedef int *NGva_list[1];
#define NG_VA_START(ap,lf) (void)(*(ap)=__va_start(lf))
#define NG_VA_ARG(ap,tp) __va_arg(*(ap),tp)
#define NG_VA_END(ap) ((void)0)
#else
typedef NGubyte *NGva_list;
#define __ng_argsiz(x) ((sizeof(x)+sizeof(int)-1) & ~(sizeof(int)-1))
#define NG_VA_START(ap,lf) ((ap)=((NGva_list)(&lf)+__ng_argsiz(lf)))
#define NG_VA_ARG(ap,tp) (((ap)=(ap)+__ng_argsiz(tp)),*((tp*)(void*)((ap)-__ng_argsiz(tp))))
#define NG_VA_END(ap) ((void)0)
#endif

#elif defined(NG_COMP_GHS)
/*
 * Green Hills tools
 */

/* byte-swapping */
__inline NGushort ngBSwap16( NGushort val) {
  return (val<<8)|(val>>8);
}
#ifdef NG_CODE_THUMB
#pragma ghs nothumb
#endif
NGuint ngBSwap32( NGuint val);
void ngDisable( void);
void ngEnable( void);
int ngIntrCtl( int);
#ifdef NG_CODE_THUMB
#pragma ghs thumb
#endif
asm void ngARMSwi( int code)
{
%con code
  SWI code
%error
}

/* interrupt routines (must be written in assembly) */
#define NG_INTERRUPT
typedef void (*NGinterruptPTR)( void);

/* variable argument list */
#include <stdarg.h>
#define NGva_list va_list
#define NG_VA_START( ap, lf) va_start( ap, lf)
#define NG_VA_ARG( ap, tp) va_arg( ap, tp)
#define NG_VA_END( ap) va_end( ap)

#elif defined(NG_COMP_TI470)
/*
 * Texas Instruments tools
 */

#define ngBSwap16( val) ngConstBSwap16( val)
#define ngBSwap32( val) ngConstBSwap32( val)

/* interrupt control */
void ngDisable( void);
void ngEnable( void);
int ngIntrCtl( int);

/* interrupt routines (must be written in assembly) */
#define NG_INTERRUPT
typedef void (*NGinterruptPTR)( void);

/* variable argument list */
#include <stdarg.h>
#define NGva_list va_list
#define NG_VA_START( ap, lf) va_start( ap, lf)
#define NG_VA_ARG( ap, tp) va_arg( ap, tp)
#define NG_VA_END( ap) va_end( ap)

#else
#error "NG_COMP_XXX not defined"
#endif /* NG_COMP_XXX */

/* NULL pointer */
#undef NULL
#define NULL 0

/* physical->pointer address mapping (direct mapping) */
#define ngMTOK( addr) ((void *) (addr))
#define ngKTOM( ptr) ((NGuint) (ptr))

/* NG_END_DECLS // confuses Source Insight */

/* big/little endian handling */
#include <ngos/cpu/endian.h>

/* unaligned data handling */
#include <ngos/cpu/unalign.h>

#endif

