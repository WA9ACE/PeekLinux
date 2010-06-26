/**
 * @file  ngdefs.h
 *
 * NexGenOS/OS Interface Definitions for Riviera NET.
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: ngdefs.h,v 1.1 2002/05/29 11:02:14 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/14/2002   Regis Feneon  Create
 *
 */

#ifndef __NG_NGDEFS_H_INCLUDED__
#define __NG_NGDEFS_H_INCLUDED__

#ifdef NG_RTOS
#error "Must be linked with POLLING libraries"
#endif

#ifdef NG_CPU_UNDEFINED

/* get type of compiler */
#ifdef _MSC_VER
#define NG_COMP_MSVC
#define NG_CPU_I486
#include <ngos/cpu/i386/macros.h>
#endif

#endif

#endif

