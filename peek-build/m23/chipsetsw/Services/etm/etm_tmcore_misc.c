/********************************************************************************
 * Enhanced TestMode (ETM)
 * @file	etm_tmcore_misc.c
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
 *  16/06/2003	Creation
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 *********************************************************************************/

#include "etm/etm.h"
#include "etm/etm_api.h"
#include "etm/etm_trace.h"
#include "etm/etm_tmcore.h"

#include <string.h>

extern T_ADC adc;
extern T_ADCCAL adc_cal;

// external FFS function prototypes
typedef signed char effs_t;
effs_t ffs_mkdir(const char *pathname);

									   
/***********************************************************************/
/*                           TESTMODE 4.X (ETM)                        */
/***********************************************************************/

// Functionality : Init default configuration for TM params

int etm_tm_init(void)
{
    extern void Cust_tm_init(void); // location layer1/tm_cust0/l1tm_cust.c
    unsigned int i;

    Cust_tm_init(); 

    return ETM_OK;
}

int etm_tm_misc_enable(int16 action)
{
    extern void config_ffs_write(char type); // location: layer1/cust0/l1_cust.c
    uint8 status;
  
    switch(action)
    {
    case CFG_WRITE_MKDIRS:
        ffs_mkdir("/gsm");
        ffs_mkdir("/pcm");
        ffs_mkdir("/sys");
        ffs_mkdir("/mmi");
        ffs_mkdir("/vos");
        ffs_mkdir("/var");
        ffs_mkdir("/gsm/rf");
        ffs_mkdir("/gsm/com");
        ffs_mkdir("/vos/vm");
        ffs_mkdir("/vos/vrm");
        ffs_mkdir("/vos/vrp");
        ffs_mkdir("/var/log");
        ffs_mkdir("/var/tst");
        ffs_mkdir("/gsm/rf/tx");
        ffs_mkdir("/gsm/rf/rx");                    break;
    case CFG_WRITE_RF_CAL:   config_ffs_write('f'); break; 
    case CFG_WRITE_RF_CFG:   config_ffs_write('F'); break;
    case CFG_WRITE_TX_CAL:   config_ffs_write('t'); break;
    case CFG_WRITE_TX_CFG:   config_ffs_write('T'); break;
    case CFG_WRITE_RX_CAL:   config_ffs_write('r'); break;
    case CFG_WRITE_RX_CFG:   config_ffs_write('R'); break;
    case CFG_WRITE_SYS_CAL:  config_ffs_write('s'); break;
    case CFG_WRITE_SYS_CFG:  config_ffs_write('S'); break;
    default:
        return ETM_INVAL;
    }

    return ETM_OK;
}

int etm_tm_misc_param_write(int16 index, uint16 value)
{
    int error;

    switch (index) {
    case GPIOSTATE0:
    case GPIODIR0:
    case GPIOSTATE1:
    case GPIODIR1:
    case GPIOSTATE0P:
    case GPIODIR0P:
    case GPIOSTATE1P:
    case GPIODIR1P:
        error = ETM_NOSYS;
        break;
    case CONVERTED_ADC0:
    case CONVERTED_ADC1:
    case CONVERTED_ADC2:
    case CONVERTED_ADC3:
    case CONVERTED_ADC4:
    case CONVERTED_ADC5:
    case CONVERTED_ADC6:
    case CONVERTED_ADC7:
    case CONVERTED_ADC8:
        adc.converted[index - CONVERTED_ADC0] = (int16) value;
        error = ETM_OK;
        break;
    case RAW_ADC0:
    case RAW_ADC1:
    case RAW_ADC2:
    case RAW_ADC3:
    case RAW_ADC4:
    case RAW_ADC5:
    case RAW_ADC6:
    case RAW_ADC7:
    case RAW_ADC8:
        adc.raw[index - RAW_ADC0]  = value;
        error = ETM_OK;
        break;
    case ADC0_COEFF_A:
    case ADC1_COEFF_A:
    case ADC2_COEFF_A:
    case ADC3_COEFF_A:
    case ADC4_COEFF_A:
    case ADC5_COEFF_A:
    case ADC6_COEFF_A:
    case ADC7_COEFF_A:
    case ADC8_COEFF_A:
        adc_cal.a[index - ADC0_COEFF_A]  = value;
        error = ETM_OK;
        break;
    case ADC0_COEFF_B:
    case ADC1_COEFF_B:
    case ADC2_COEFF_B:
    case ADC3_COEFF_B:
    case ADC4_COEFF_B:
    case ADC5_COEFF_B:
    case ADC6_COEFF_B:
    case ADC7_COEFF_B:
    case ADC8_COEFF_B:
        adc_cal.b[index - ADC0_COEFF_B]  = value;
        error = ETM_OK;
        break;
    case SLEEP_MODE:
        error = ETM_NOSYS;
        break;
    default:
        error = ETM_INVAL;
    } // end switch
    
    return error;
}

int etm_tm_misc_param_read(int16 index)
{
  volatile uint16 value;
  int error;

  switch (index)
  {
  case GPIOSTATE0:
  case GPIODIR0:
  case GPIOSTATE1:
  case GPIODIR1:
  case GPIOSTATE0P:
  case GPIODIR0P:
  case GPIOSTATE1P:
  case GPIODIR1P:
      return ETM_NOSYS;
  case CONVERTED_ADC0:
  case CONVERTED_ADC1:
  case CONVERTED_ADC2:
  case CONVERTED_ADC3:
  case CONVERTED_ADC4:
  case CONVERTED_ADC5:
  case CONVERTED_ADC6:
  case CONVERTED_ADC7:
  case CONVERTED_ADC8:
      value = adc.converted[index - CONVERTED_ADC0];
      break;
  case RAW_ADC0:
  case RAW_ADC1:
  case RAW_ADC2:
  case RAW_ADC3:
  case RAW_ADC4:
  case RAW_ADC5:
  case RAW_ADC6:
  case RAW_ADC7:
  case RAW_ADC8:
      value = adc.raw[index - RAW_ADC0];
      break;
  case ADC0_COEFF_A:
  case ADC1_COEFF_A:
  case ADC2_COEFF_A:
  case ADC3_COEFF_A:
  case ADC4_COEFF_A:
  case ADC5_COEFF_A:
  case ADC6_COEFF_A:
  case ADC7_COEFF_A:
  case ADC8_COEFF_A:
      value = adc_cal.a[index - ADC0_COEFF_A];
      break;
  case ADC0_COEFF_B:
  case ADC1_COEFF_B:
  case ADC2_COEFF_B:
  case ADC3_COEFF_B:
  case ADC4_COEFF_B:
  case ADC5_COEFF_B:
  case ADC6_COEFF_B:
  case ADC7_COEFF_B:
  case ADC8_COEFF_B:
      value = adc_cal.b[index - ADC0_COEFF_B];
      break;
  case SLEEP_MODE:
      return ETM_NOSYS;
  default:
      return ETM_INVAL;
  } // end switch
 
  return value;
}


int etm_tm_special_param_write(int16 index, uint16 value)
{
    return ETM_NOSYS;
}

int etm_tm_special_param_read(int16 index)
{
    return ETM_NOSYS;
}

int etm_tm_special_table_write(int8 index, uint8 size, uint8 table[])
{
    return ETM_NOSYS;
}

int etm_tm_special_table_read(int8 index)
{
    return ETM_NOSYS;
}

int etm_tm_special_enable(int16 action)
{
    return ETM_NOSYS;
}


/***********************************************************************/
/* Future misc functions                                               */
/***********************************************************************/

int etm_DBB_register_read(uint16 reg_id)
{
    return ETM_NOSYS;
}

int etm_DBB_register_write(uint16 reg_id)
{
    return ETM_NOSYS;
}

//#endif // TESTMODE
