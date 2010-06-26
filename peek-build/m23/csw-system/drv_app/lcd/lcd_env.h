/****************************************************************************/
/*                                                                          */
/*	File Name:	lcd_env.h													*/
/*                                                                          */
/*	Purpose:	This file contains prototypes for RV Manager related	*/
/*				functions used to get info, start and stop the xxx block.	*/
/*                                                                          */
/*  Version		0.1															*/
/*																			*/
/* 	Date       	Modification												*/
/*  ------------------------------------									*/
/*  09/09/2005	Create														*/
/*																			*/
/*	Author		Santosh V Kondajji						*/
/*																			*/
/* (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/
#ifndef __LCD_ENV_H_
#define __LCD_ENV_H_


#include "rvm/rvm_gen.h"

#include "lcd/lcd_pool_size.h"	/* Stack & Memory Bank sizes definitions */


/* memory bank size and watermark */	 
                                        
/* LCD_MB_PRIM_SIZE defines */
#define LCD_MB_PRIM_INC_SIZE			0
#define LCD_MB_PRIM_INC_WATERMARK		0

#define LCD_MAILBOX_USED	RVF_TASK_MBOX_0



/* generic functions declarations */
T_RVM_RETURN lcd_get_info (T_RVM_INFO_SWE  *infoSWE);

T_RVM_RETURN  lcd_set_info	(	T_RVF_ADDR_ID	addr_id,
								T_RV_RETURN		return_path[],
								T_RVF_MB_ID		bk_id_table[],
								T_RVM_CB_FUNC	call_back_error_ft);
T_RVM_RETURN lcd_init (void);

void lcd_start (void);

T_RVM_RETURN lcd_stop (void);

T_RVM_RETURN lcd_kill (void);

#endif /*__LCD_ENV_H_*/

