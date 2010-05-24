/******************************************************************************
 * Power Task (pwr)
 * Design and coding by Svend Kristian Lindholm, skl@ti.com
 *
 * PWR SW Modulation
 *
 * $Id: pwr_modulate.h 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/
#ifndef __LCC_MODULATE_H__
#define __LCC_MODULATE_H__

#include "lcc/lcc.h"
#include "lcc/lcc_cfg.h"

// SW modulation using the following addresses
#if (USE_PWL_AS_MODULATOR == 1)
// D-Sample - no modulation - just envelope function to simulate GPIO 6
    #define PWL_LEVEL_REG (*(volatile unsigned char*) 0xFFFE8000)
    #define PWL_CTRL_REG  (*(volatile unsigned char*) 0xFFFE8001)
    #define BUZZ_LIGHT_REG (*(volatile short*) 0xFFFE480E) // Enable buzzer (bit 0) and PWL (bit 1)
#else
#error	USE_PWL_AS_MODULATOR node defined
#endif
/******************************************************************************
 * Function prototypes
 ******************************************************************************/

void pwr_modulate_init(void);
void pwr_modulate_on(void);
void pwr_modulate_off(void);

#endif
