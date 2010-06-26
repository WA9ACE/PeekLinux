	/******************************************************************************
	 * Power Task (pwr)
	 * Design and coding by Svend Kristian Lindholm, skl@ti.com
	 *
	 * PWR Configuration Variables NOT in FFS
	 *
	 * $Id: pwr_cfg_i.h 1.2 Wed, 20 Aug 2003 12:54:50 +0200 skl $
	 *
	 ******************************************************************************/

#ifndef _LCC_CFG_I_H_
#define _LCC_CFG_I_H_
#include "chipset.cfg"

	// The charging current used in SW precharge state
#define ICHG_PRECHG  0x40   //maximum current : 497.16 mA

	// The SW precharge start threshold - discriminator that decides between entering INI or PRE states
#define VBAT_PRECHG_START  3300 //3200

	// The SW precharge stop threshold - PRE->INI state & FFS configuration files will be read
#define VBAT_PRECHG_STOP  3600

	// Define how many consecutive charger unplugs we must detect in order to have a 'real' unplug
#define CONSECUTIVE_CHG_UNPLUGS 5
#define CONSECUTIVE_USB_UNPLUGS 5

	// Define the size of the exponential avg window when in PRE state (i.e. no FFS configuration is read)
#define PRECHG_AVG_WINDOW_SIZE  5

	// Define the battery slip number before we read the battery id
#define BATTERY_TYPE_SLIP 1     /* 2 */ /* 4 */

	// Define the charger slip number before we read the charger id
#define CHARGER_TYPE_SLIP 1    /* 1 */  /* 2 */

	// Define the lower charger id out-of-bound value (mV)
#define CHARGER_TYPE_OUT_OF_BOUND_LOW 4000

	// Define the 'typical' charger id that will be used if the charger id reading has failed
	// Make sure it's in the charger voltage interval
#define CHARGER_TYPE_TYPICAL          4201

	// The Q402 Vchg thresholds that decides a charger unplug when charging has been initiated
#define VCHG_Q402_THR  1000
#define ICHG_Q402_THR  20

// Define the safety charging reject VOLTAGE [mV] when in CV mode
#define VCHG_REJECT_CV_MODE 5700
// Define the safety charging reject VOLTAGE [mV] when in CI mode
#define VCHG_REJECT_CI_MODE 5700

// FIXME: These should go into the normal FFS configuration
// Define the safety charging reject CURRENT [mA] when in CV mode
#define ICHG_REJECT_CV_MODE 1024
// Define the safety charging reject CURRENT [mA] when in CI mode
#define ICHG_REJECT_CI_MODE 1024

#if (ANLG_FAM==11)
	/* FIXME : Change these Values after calibration */
#define VCHG_BCI_THR   4200  //mV//
#define ICHG_BCI_THR   0x0A      //f0r 15.6mA ; for the settings :OFFEN =0;GAIN4=1;CLIB=0.
#define OVER_VOLTAGE  0x292 //the CHGVREG value curresponding to 4.2V
#define OVER_VOLTAGE_MV  4225  //for over voltage stop criteria.

#define CV_CHARGE_START 4180
#define PWM_CHARGE_START 4100
#define PWM_CHARGE_STOP 4200
#define MONITOR_LEVEL 4100
#endif

#endif
