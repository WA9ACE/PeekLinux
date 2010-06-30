/**
 * @file	kpd_pool_size.h
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

#ifndef __KPD_POOL_SIZE_H_
#define __KPD_POOL_SIZE_H_


#ifndef _WINDOWS
  #include "config/board.cfg"
  #include "config/rv.cfg"
  
#endif

  
/*
 * Values used in kpd_env.h
 */
#if (TEST==0)
#define KPD_STACK_SIZE (1000)
#else
#define KPD_STACK_SIZE (4000)
#endif

#if ((BOARD == 7) || (BOARD == 8) || (BOARD == 9))
  #define KPD_MB1_SIZE (716)
#elif ((BOARD == 34) || (BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43))
  #define KPD_MB1_SIZE (788)
#elif ( (BOARD==70) || (BOARD==71))
  #define KPD_MB1_SIZE (1124)  //800->1124
#elif defined _WINDOWS
  #define KPD_MB1_SIZE (788)
#endif /* BOARD */

#define KPD_POOL_SIZE  (KPD_STACK_SIZE + KPD_MB1_SIZE)


#endif /*__KPD_POOL_SIZE_H_*/
