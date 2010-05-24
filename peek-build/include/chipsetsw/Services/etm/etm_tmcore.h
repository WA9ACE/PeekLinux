/********************************************************************************
 * Enhanced TestMode (ETM)
 * @file	etm_tmcore.h (Support for TMM CORE commands)
 *
 * @author	Kim T. Peteren (ktp@ti.com)
 * @version 0.1
 *

 *
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  20/10/2003	Creation
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 *********************************************************************************/

#ifndef _ETM_TMCORE_H_
#define _ETM_TMCORE_H_

#include "rf.cfg"

#if (RF_FAMILY==43)
#include "abb.h"
#include "l1_rf43.h"
#endif

#if (RF_FAM==35)
#include "l1_rf35.h"
#endif

#if (RF_FAM==61)
#include "l1_rf61.h"
#endif

#if (RF_FAM==12)
#include "l1_rf12.h"
#endif

#if (RF_FAM==10)
#include "l1_rf10.h"
#endif

#if (RF_FAM==8)
#include "l1_rf8.h"
#endif

#if (RF_FAM==2)
#include "l1_rf2.h"
#endif

// Variables for L1 configuration
extern T_L1_CONFIG    l1_config;


enum MISC_PARAM 
{
  GPIOSTATE0                = 8,
  GPIODIR0                  = 9,
  GPIOSTATE1                = 10,
  GPIODIR1                  = 11,
  GPIOSTATE0P               = 12,
  GPIODIR0P                 = 13,
  GPIOSTATE1P               = 14,
  GPIODIR1P                 = 15,
  ADC_INTERVAL              = 18,
  ADC_ENA_FLAG              = 19,
  CONVERTED_ADC0            = 20,
  CONVERTED_ADC1            = 21,
  CONVERTED_ADC2            = 22,
  CONVERTED_ADC3            = 23,
  CONVERTED_ADC4            = 24,
  CONVERTED_ADC5            = 25,
  CONVERTED_ADC6            = 26,
  CONVERTED_ADC7            = 27,
  CONVERTED_ADC8            = 28,
  RAW_ADC0                  = 30,
  RAW_ADC1                  = 31,
  RAW_ADC2                  = 32,
  RAW_ADC3                  = 33,
  RAW_ADC4                  = 34,
  RAW_ADC5                  = 35,
  RAW_ADC6                  = 36,
  RAW_ADC7                  = 37,
  RAW_ADC8                  = 38,
  ADC0_COEFF_A              = 50,
  ADC1_COEFF_A              = 51,
  ADC2_COEFF_A              = 52,
  ADC3_COEFF_A              = 53,
  ADC4_COEFF_A              = 54,
  ADC5_COEFF_A              = 55,
  ADC6_COEFF_A              = 56,
  ADC7_COEFF_A              = 57,
  ADC8_COEFF_A              = 58,
  ADC0_COEFF_B              = 60,
  ADC1_COEFF_B              = 61,
  ADC2_COEFF_B              = 62,
  ADC3_COEFF_B              = 63,
  ADC4_COEFF_B              = 64,
  ADC5_COEFF_B              = 65,
  ADC6_COEFF_B              = 66,
  ADC7_COEFF_B              = 67,
  ADC8_COEFF_B              = 68,
  SLEEP_MODE                = 80,
  CURRENT_TM_MODE           = 127
};


enum ME_CFG_WRITE_E {
    CFG_WRITE_MKDIRS   = 100,
    CFG_WRITE_RF_CAL   = 102,
    CFG_WRITE_RF_CFG   = 103,
    CFG_WRITE_TX_CAL   = 104,
    CFG_WRITE_TX_CFG   = 105,
    CFG_WRITE_RX_CAL   = 106,
    CFG_WRITE_RX_CFG   = 107,
    CFG_WRITE_SYS_CAL  = 108,
    CFG_WRITE_SYS_CFG  = 109
};


enum VERSION_GET_E 
{
  BBCHIP_MODULE_REV         = 0x10,
  CHIPID_MODULE_REV         = 0x14,
  CHIPVER_MODULE_REV        = 0x15,
  DSPSW_MODULE_REV          = 0x22,
  ANALOGCHIP_MODULE_REV     = 0x30,
  GSM_MODULE_REV            = 0x80,
  LAYER1_MODULE_REV         = 0x84,
  RFDRIVER_MODULE_REV       = 0x88,
  TM_API_MODULE_REV         = 0xE0,
  L1_TM_CORE_MODULE_REV     = 0xE1,
  STD_MODULE_REV            = 0xE2,
  DSP_MODULE_REV            = 0xE3,
  BOARD_MODULE_REV          = 0xE4,
  RF_MODULE_REV             = 0xE5 
};


#endif // end of _ETM_TMCORE_H_
