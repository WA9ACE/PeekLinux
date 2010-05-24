/**
 * @file	lcd_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author	Santosh v Kondajji
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	09/09/2005	Santosh V Kondajji			Extracted from rvf_pool_size.h
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __LCD_POOL_SIZE_H_
#define __LCD_POOL_SIZE_H_


/*
 * Values used in Stack & Memory Banks
 */
#if (REMU==1)
#define LCD_STACK_SIZE   (800)
#else
#define LCD_STACK_SIZE   (600)
#endif
   #if defined (_WINDOWS)
      #define LCD_MB1_SIZE   (0)
   #else
      #define LCD_MB1_SIZE   (0)
   #endif

#define LCD_POOL_SIZE    (LCD_STACK_SIZE + LCD_MB1_SIZE)


#endif /*__LCD_POOL_SIZE_H_*/
