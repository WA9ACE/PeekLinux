/**
 * @file	atp_pool_size.h
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

#ifndef __ATP_POOL_SIZE_H_
#define __ATP_POOL_SIZE_H_


/*
 * Values used in atp_config.h
 */
#define ATP_MB1_SIZE  (5000)
#define ATP_POOL_SIZE (ATP_MB1_SIZE)


/*
 * Values used in atp_uart_env.h
 */
#define ATP_UART_STACK_SIZE (1024)
#define ATP_UART_MB1_SIZE   (1024)
#define ATP_UART_POOL_SIZE  (ATP_UART_STACK_SIZE + ATP_UART_MB1_SIZE)


#endif /*__ATP_POOL_SIZE_H_*/
