/********************************************************************************
 * Enhanced TestMode (ETM)
 * @file	etm_tmcore_misc.h 
 *
 * @author	Kim T. Peteren (ktp@ti.com)
 * @version 0.1
 *
 * $Id: etm.c 1.22 Mon, 28 Apr 2003 15:35:40 +0200 ktp $

 *
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  03/12/2003	Creation
 *
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 *********************************************************************************/

#ifndef _ETM_TMCORE_MISC_H_
#define _ETM_TMCORE_MISC_H_

#include "etm/etm.h"

/******************************************************************************
 * Prototypes 
 *****************************************************************************/

int etm_tm_init(void);
int etm_tm_misc_enable(int16 action);
int etm_tm_misc_param_write(int16 index, uint16 value);
int etm_tm_misc_param_read(int16 index);

int etm_tm_special_param_write(int16 index, uint16 value);
int etm_tm_special_param_read(int16 index);
int etm_tm_special_table_write(int8 index, uint8 size, uint8 table[]);
int etm_tm_special_table_read(int8 index);
int etm_tm_special_enable(int16 action);

int etm_DBB_register_read(uint16 reg_id);
int etm_DBB_register_write(uint16 reg_id);


#endif // _ETM_MISC_H_
