/**
 * @file	rvt_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author	Vincent Oberle
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	07/08/2003	Vincent Oberle			Extracted from rvf_pool_size.h
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __RVT_POOL_SIZE_H_
#define __RVT_POOL_SIZE_H_


#ifndef _WINDOWS
  #include "rv.cfg"
  #include "swconfig.cfg"
#endif


/*
 * Values used in rvt_def_i.h
 */
#if	(LOCOSTO_LITE==1)
#define TRACE_STACK_SIZE (1000)
#else
#define TRACE_STACK_SIZE (2000)
#endif


#if (TEST==1)
  #define TRACE_MB1_SIZE (750000) /*(25000)*/
#elif (!GSMLITE)
#if	(LOCOSTO_LITE)
  #define TRACE_MB1_SIZE (5000)
#else
  #define TRACE_MB1_SIZE (25000)
#endif // LOCOSTO_LITE
#else
  #define TRACE_MB1_SIZE (8 * 1024)
#endif

#define TRACE_POOL_SIZE  (TRACE_STACK_SIZE + TRACE_MB1_SIZE)


#endif /*__RVT_POOL_SIZE_H_*/
