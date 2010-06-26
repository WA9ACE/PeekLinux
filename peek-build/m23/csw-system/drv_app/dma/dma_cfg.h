/**
 * @file	dma_cfg.h
 *
 * Configuration definitions for the DMA instance.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/2/2003	 ()		Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __DMA_CFG_H_
#define __DMA_CFG_H_


#include "rv/rv_general.h"       /* General Riviera definitions. */
#include "dma/dma_pool_size.h"	/* Stack & Memory Bank sizes definitions */

/* Software version */
#define DMA_MAJOR  0
#define DMA_MINOR  8
#define DMA_BUILD  0


/* _DMA_TRACING_ON is used to set the DMA tracing on or off */
/* Default turned off because of performance issues.        */
// #define _DMA_TRACING_ON
#undef  _DMA_TRACING_ON

/* 
  For test purposes the define RV_TRACE_LEVEL_ERROR can be used as the 
  output color is red. Otherwise the define RV_TRACE_LEVEL_DEBUG_LOW 
  should be used.
*/
//#define DMA_TRACE_LEVEL   RV_TRACE_LEVEL_ERROR
#define DMA_TRACE_LEVEL RV_TRACE_LEVEL_DEBUG_LOW

/* DMA_FREE_MESSAGE is used to indicate if a message has to 
 * be freed or not after a call backfunction. This define is 
 * used in the function dma_send_status_message
 */
//#define DMA_FREE_MESSAGE
#undef  DMA_FREE_MESSAGE

/**
 * @name Mem bank
 *
 * Memory bank size and watermark. 
 * */
/*@{*/
#define DMA_MB_RX_SIZE                  4096  //old 2048
#define DMA_MB_PRIM_SIZE                DMA_MB1_SIZE
#define DMA_MB_PRIM_WATERMARK           (DMA_MB_PRIM_SIZE - 128)
#define DMA_MB_RX_WATERMARK             (DMA_MB_RX_SIZE   - 128)
/*@}*/


/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_DMA_TASK_PRIORITY
#define DMA_TASK_PRIORITY               RVM_DMA_TASK_PRIORITY	
#else
#define DMA_TASK_PRIORITY               71  //old 72
#endif

/**
  * To buffer client requests for channel reservation, memory is needed. 
  * Saving only absolute required information about the request minimizes 
  * the amount of memory. 
  */
#define DMA_MAX_QUEUED_REQUESTS     (6)

/*
  DMA suspend privileges of the emulator

  Only the processor can suspend an active DMA transfer. 
  This happens when the processor is halted during debug operation 
  (per example at a breakpoint). This feature can be enabled/disabled 
  at com-pile time with one of the following defines.
  The default value will be:  DMA_EMULATOR_SUSPEND_DISABLE
*/
/* #define DMA_EMULATOR_SUSPEND_ENABLE */
#define DMA_EMULATOR_SUSPEND_DISABLE


/*
  Power save by clock disable

  The DMA controller device is able to cut off its clocks when its internal 
  activity allows this. This allows power saving. This feature can be 
  enabled/disabled at compile time with one of the following defines. 
  Use one, out-comment the other.
  The default value will be: DMA_POWERSAVE_ENABLE
*/
/* #define DMA_POWERSAVE_ENABLE */
#define DMA_POWERSAVE_DISABLE

/*
  Access priority DSP port

  The DMA controller and the ARM processor compete for the access to the 
  DSP (API) port. These definition states whether ARM or DMA is to win 
  if both want to access at the same time. Use one, out-comment the other.

  The default value will be: DMA_DSP_PRIO_ARM
*/
/* #define DMA_DSP_PRIO_DMA */
#define DMA_DSP_PRIO_ARM

/*
  Access priority RHEA port

  The DMA controller and the ARM processor compete for the access to the 
  RHEA port. These definition states whether ARM or DMA is to win if both 
  want to access at the same time. Use one, out-comment the other.

  The default value will be: DMA_RHEA_PRIO_ARM
*/
/* #define DMA_RHEA_PRIO_DMA */
#define DMA_RHEA_PRIO_ARM

/*
  Access priority IMIF port

  The DMA controller and the ARM processor compete for the access to the IMIF 
  (Internal Memory In-terface) port. The ARM needs access to it for program 
  execution and the access priority is therefor not absolute. Cycle stealing 
  is used instead.The listed defines state the amount of ARM processor cycles 
  before the DMA steals one cycle. Use one, out-comment the others.

  The next table shows the DMA bandwidth for each define.
  #define	DMA bandwidth
  DMA_IMIF_PRIO_CPU_0	100%
  DMA_IMIF_PRIO_CPU_1	50%
  DMA_IMIF_PRIO_CPU_2	33%
  DMA_IMIF_PRIO_CPU_3	25%
  DMA_IMIF_PRIO_CPU_4	20%
  DMA_IMIF_PRIO_CPU_5	16%
  DMA_IMIF_PRIO_CPU_6	14%
  DMA_IMIF_PRIO_CPU_7	12%

  The default value will be: DMA_IMIF_PRIO_CPU_5

  IMIF devices are the crypto devices SHA/MD5 and DES,/3DES. Also random number 
  generator (RNG) and Nand flash are IMIF devices.
*/
/* #define DMA_IMIF_PRIO_CPU_0 */
/* #define DMA_IMIF_PRIO_CPU_1 */
/* #define DMA_IMIF_PRIO_CPU_2 */
/* #define DMA_IMIF_PRIO_CPU_3 */
/* #define DMA_IMIF_PRIO_CPU_4 */
#define DMA_IMIF_PRIO_CPU_5
/* #define DMA_IMIF_PRIO_CPU_6 */
/* #define DMA_IMIF_PRIO_CPU_7 */

/*
  Autogating_on:  DMA clocks auto gating
 	Autogating_on = 1: allows the DMA to cut off its clocks according to its activity (this setting allows to save power consumption).
	Autogating_on = 0: DMA clocks are always on.
*/

/* WARNING !!!
   Due to a hardware bug autogating bit should be always OFF.
   Otherwise interrupts might not be generated !
*/
// #define DMA_AUTOGATING_ON 
#define DMA_AUTOGATING_OFF 


#endif /* __DMA_CFG_H_ */
