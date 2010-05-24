/******************************************************************************
 * Power Task (pwr)
 * Design and coding by Svend Kristian Lindholm, skl@ti.com
 *
 * Main PWR Task
 *
 * $Id: pwr_task.h 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/

#ifndef __LCC_TASK_H_
#define __LCC_TASK_H_

#include "rvm/rvm_api.h"
#if (ANLG_FAM==11)
#include "abb/bspTwl3029.h"
#include "abb/bspTwl3029_I2c.h"
#include "abb/bspTwl3029_IntC.h"
#endif

// Main event handling function

void pwr_free(void *addr);
void *pwr_malloc(int size);

T_RVM_RETURN pwr_check_files(void);
T_RVM_RETURN pwr_task_init(void);
void pwr_task(void);
void pwr_chg_int_timer(void);
void pwr_chg_interrupt_handler(unsigned short id);
#if (ANLG_FAM==11)
void pwr_abb_interrupt_handler(BspTwl3029_IntC_SourceId id);
uint8 pwr_charge_start_check(void);
	int pwr_check_topping();

#endif

#endif
