/******************************************************************************
 * Power Task (pwr)
 * Design and coding by Svend Kristian Lindholm, skl@ti.com
 *
 * PWR SW Modulation
 *
 * $Id: pwr_modulate.c 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/
#include <string.h>
#include "lcc/lcc.h"
#include "lcc/lcc_trace.h"
#include "lcc/lcc_modulate.h"
#include "lcc/lcc_cfg_i.h"
#include "lcc/lcc_cfg.h"
#include "rv/rv_defined_swe.h"
#include "chipset.cfg"

#if (ANLG_FAM!=11)
#include "abb/abb.h"
#endif

/******************************************************************************
 * Function prototypes
 ******************************************************************************/

void pwr_modulate_init(void);
void pwr_modulate_on(void);
void pwr_modulate_off(void);


#if (USE_Q401_CHG_CIRCUIT == 1)
extern T_PWR_CFG_BLOCK  *pwr_cfg;
#endif

void pwr_modulate_init(void) {
    ttw(ttr(TTrEventLow,"pwr_modulate_init(%d)" NL, 0));
	#if(ANLG_FAM!=11)
    #if (USE_PWL_AS_MODULATOR == 1)
    BUZZ_LIGHT_REG |= 0x02;
    PWL_LEVEL_REG = 0xFF; // Full envelope function
    PWL_CTRL_REG  = 0x00; // No 32KHz clock
    #else
    // Set GPIO 6 HIGH
    *((volatile uint16 *) 0xfffe4806) |= 0x0020; // Enable GPIO module
    *((volatile uint16 *) 0xfffe4804) &= 0xffbf; // Set GPIO-6 = output
    *((volatile uint16 *) 0xfffe4802) |= 0x0080; // Set GPIO-6 = HIGH
    #endif
	#endif
    ttw(ttr(TTrEventLow,"pwr_modulate_init(%d)" NL, 0xff));
}

void pwr_modulate_on(void) {
    ttw(ttr(TTrEventLow,"pwr_modulate_on(%d)" NL, 0));
#if (USE_Q401_CHG_CIRCUIT== 1)
    /* Program the DAC with the constant current value taken from /pwr/chg/chg<N>.cfg multiplied by k/255, where current k is in [1..255] */
    ABB_Write_Register_on_page(PAGE0, CHGREG, (pwr_cfg->data.k * pwr_cfg->chg.ichg_max) / 255);
#endif
#if (USE_Q402_CHG_CIRCUIT == 1)
    #if (USE_PWL_AS_MODULATOR == 1)
    BUZZ_LIGHT_REG |= 0x02;
    PWL_LEVEL_REG = 0xFF; // Full envelope function
    PWL_CTRL_REG  = 0x00; // No 32KHz clock
    #else
    // Use GPIO 6
    *((volatile uint16 *) 0xfffe4806) |= 0x0020; // Enable GPIO module
    *((volatile uint16 *) 0xfffe4804) &= 0xffbf; // Set GPIO-6 = output
    *((volatile uint16 *) 0xfffe4802) |= 0x0080; // Set GPIO-6 = HIGH
    #endif
#endif
#if (USE_BCI_CIRCUIT==1)
	/* Nothing todo */
#endif
    ttw(ttr(TTrEventLow,"pwr_modulate_on(%d)" NL, 0xFF));

}

void pwr_modulate_off(void) {
    ttw(ttr(TTrEventLow,"pwr_modulate_off(%d)" NL, 0));
#if (USE_Q401_CHG_CIRCUIT == 1)
    // Don't do anything - FET is fully controlled in pwr_modulate_on()
#endif
#if (USE_Q402_CHG_CIRCUIT == 1)
    #ifdef USE_PWL_AS_MODULATOR
    BUZZ_LIGHT_REG &= 0xfd;
    PWL_LEVEL_REG = 0x00; // No envelope function
    PWL_CTRL_REG  = 0x00; // No 32KHz clock
    #else
    // Use GPIO 6
    *((volatile uint16 *) 0xfffe4806) |= 0x0020; // Enable GPIO module
    *((volatile uint16 *) 0xfffe4804) &= 0xffbf; // Set GPIO-6 = output
    *((volatile uint16 *) 0xfffe4802) &= 0x007f; // Set GPIO-6 = LOW
    #endif
#endif
#if (USE_BCI_CIRCUIT == 1)
	/* Nothing todo */
#endif
    ttw(ttr(TTrEventLow,"pwr_modulate_off(%d)" NL, 0xFF));
}

