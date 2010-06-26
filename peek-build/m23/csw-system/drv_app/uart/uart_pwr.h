/*
 * @file	uart_pwr.h
 *
 * Power management implementation for UART.
 *
 * @author	santosh v kondajji
 *
 * @version 0.1
 *
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *
 *
 *(C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef	UART_PWR_H_
#define	UART_PWR_H_

#include "sys_types.h"
#include "general.h"

UINT8 uart_pwr_interface(UINT8);

#if (L1_POWER_MGT !=0)
void Uart_Vote_DeepSleepStatus(void);
#endif
#endif

