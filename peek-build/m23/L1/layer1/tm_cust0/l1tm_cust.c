/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1TM_CUST.C
 *
 *        Filename l1tm_cust.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#include "l1_macro.h"
#include "l1_confg.h"

#if TESTMODE
#if (AUDIO_TASK == 1)
  #include "l1audio_defty.h"
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
#endif
#include "l1_types.h"
#include "tm_defs.h"
#include "l1_const.h"


#include "l1tm_defty.h"
#include "l1tm_cust.h"

#if (L1_GTT == 1)
  #include "l1gtt_const.h"
  #include "l1gtt_defty.h"
#endif

#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
#endif

#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif

#include "l1_defty.h"
#include "l1_varex.h"
#include "l1_msgty.h"
#include "l1_tabs.h"

#include "l1tm_msgty.h"
#include "l1tm_varex.h"

#include "abb.h"

#if (RF_FAM == 61)
  #include "tpudrv61.h"
  #include "l1_rf61.h"
  #include "apc.h"
#endif


#if (RF_FAM == 43)
  #include "tpudrv43.h"
  #include "l1_rf43.h"
#endif
#if (RF_FAM == 35)
  #include "tpudrv35.h"
  #include "l1_rf35.h"
#endif

#if (RF_FAM == 12)
  #include "tpudrv12.h"
  #include "l1_rf12.h"
#endif

#if (RF_FAM == 10)
  #include "tpudrv10.h"
  #include "l1_rf10.h"
#endif

#if (RF_FAM == 8)
  #include "tpudrv8.h"
  #include "l1_rf8.h"
#endif

#if (RF_FAM == 2)
  #include "l1_rf2.h"
#endif

#if ( L1_DRP == 1)
  #include "drp_drive.h"
#endif

#if (RF_FAM == 60)
  #include "tpudrv60.h"
  #include "l1_rf60.h"
#endif

// #if (FF_REPEATED_SACCH == 1 )
//  #define  REPEATED_SACCH_ENABLE  1
// #endif /*  FF_REPEATED_SACCH */
#include <string.h>

// Import band configuration from Flash module (need to replace by an access function)
//extern UWORD8       std;
extern T_L1_CONFIG  l1_config;
extern T_RF rf;
extern T_RF_BAND rf_band[];
extern UWORD16 AGC_TABLE[AGC_TABLE_SIZE];
extern T_ADC adc;
extern T_ADCCAL adc_cal;
extern UWORD16 TM_ul_data[16]; //Uplink data to be stored into ABB Uplink buffer
#if (L1_FF_MULTIBAND == 0)
extern T_STD_CONFIG std_config[];
#endif 
UWORD8 tm_band = 0;

//Flexible TPU Timings
#if (L1_TPU_DEV == 1)
extern WORD16 rf_rx_tpu_timings[];
extern WORD16 rf_rx_tpu_timings[];

extern WORD16 rf_flexi_abb_delays[];
#endif

// External function prototypes
void get_cal_from_nvmem (UWORD8 *ptr, UWORD16 len, UWORD8 id);
UWORD8 save_cal_in_nvmem (UWORD8 *ptr, UWORD16 len, UWORD8 id);
void Cust_init_std(void);
void l1_tpu_init_light(void);

enum {
  TM_RF_ID        = 0,
  TM_ADC_ID       = 1
};

typedef signed char effs_t;
// external FFS function prototypes
effs_t ffs_mkdir(const char *pathname);
void config_ffs_write(char type);

/***********************************************************************/
/*                           TESTMODE 4.X                              */
/***********************************************************************/


/*----------------------------------------------------------*/
/* Cust_tm_init()                                           */
/*----------------------------------------------------------*/
/* Parameters :                                             */
/* Return     :                                             */
/* Functionality : Init default configuration for TM params */
/*----------------------------------------------------------*/

void Cust_tm_init(void)
{
  UWORD32 i;

  l1_config.adc_enable                             = ADC_ENABLE; // ADC readings enabled
  l1_config.agc_enable                             = AGC_ENABLE; // AGC algo enabled
  l1_config.afc_enable                             = AFC_ENABLE; // AFC algo enabled
  l1_config.tmode.rf_params.bcch_arfcn             = TM_BCCH_ARFCN;
  l1_config.tmode.rf_params.tch_arfcn              = TM_TCH_ARFCN;
  l1_config.tmode.rf_params.mon_arfcn              = TM_MON_ARFCN;
  l1_config.tmode.rf_params.channel_type           = TM_CHAN_TYPE; // TCH_F
  l1_config.tmode.rf_params.subchannel             = TM_SUB_CHAN;
  l1_config.tmode.rf_params.reload_ramps_flag      = 0;
  l1_config.tmode.rf_params.tmode_continuous       = TM_NO_CONTINUOUS;
  l1_config.tmode.rx_params.slot_num               = TM_SLOT_NUM; // Time Slot
  l1_config.tmode.rx_params.agc                    = TM_AGC_VALUE; //This may be outside the range of the RF chip used
  l1_config.tmode.rx_params.pm_enable              = TM_PM_ENABLE;
  l1_config.tmode.rx_params.lna_off                = TM_LNA_OFF;
  l1_config.tmode.rx_params.number_of_measurements = TM_NUM_MEAS;
  l1_config.tmode.rx_params.place_of_measurement   = TM_WIN_MEAS;
  l1_config.tmode.tx_params.txpwr                  = TM_TXPWR; // Min power level for GSM900
  l1_config.tmode.tx_params.txpwr_skip             = TM_TXPWR_SKIP;
  l1_config.tmode.tx_params.timing_advance         = TM_TA;
  l1_config.tmode.tx_params.burst_type             = TM_BURST_TYPE; // default is normal up-link burst
  l1_config.tmode.tx_params.burst_data             = TM_BURST_DATA; // default is all zeros
  l1_config.tmode.tx_params.tsc                    = TM_TSC; // Training Sequence ("BCC" on BSS)
#if (CODE_VERSION != SIMULATION)
  l1_config.tmode.stats_config.num_loops           = TM_NUM_LOOPS; // 0 actually means infinite
#else
  l1_config.tmode.stats_config.num_loops           = 4; // 0 actually means infinite
#endif
  l1_config.tmode.stats_config.auto_result_loops   = TM_AUTO_RESULT_LOOPS; // 0 actually means infinite
  l1_config.tmode.stats_config.auto_reset_loops    = TM_AUTO_RESET_LOOPS; // 0 actually means infinite
  l1_config.tmode.stats_config.stat_type           = TM_STAT_TYPE;
  l1_config.tmode.stats_config.stat_bitmask        = TM_STAT_BITMASK;

#if (CODE_VERSION != SIMULATION)

 #if(RF_FAM != 61)
      // Initialize APCDEL1 register of Omega
      ABB_Write_Register_on_page(PAGE0, APCDEL1, (C_APCDEL1 - 0x0004) >> 6);
 #endif

 #if (RF_FAM == 61)
   // Changing to write the APCDEL1, APCDEL2 and APCCTRL2 via API
   l1s_dsp_com.dsp_ndb_ptr->d_apcdel1  = l1_config.params.apcdel1;
   l1s_dsp_com.dsp_ndb_ptr->d_apcdel2  = l1_config.params.apcdel2;
   // For APCCTRL2 alone write immediately as it is different from APCDEL1 and APCDEL2
   // By Default l1_config.params.apcctrl2 (C_APCCTRL2) has automatic mode.
   l1ddsp_apc_load_apcctrl2(l1_config.params.apcctrl2);
 #endif


#endif

  l1tm.tm_msg_received   = FALSE;

  for (i=0;i<16;i++)
    TM_ul_data[i]=0;

#if L1_GPRS
  l1_config.tmode.rf_params.pdtch_arfcn          = TM_PDTCH_ARFCN;
  l1_config.tmode.rf_params.multislot_class      = TM_MULTISLOT_CLASS;
  l1_config.tmode.stats_config.stat_gprs_slots   = TM_STAT_GPRS_SLOTS;
  l1_config.tmode.rx_params.timeslot_alloc       = TM_RX_ALLOCATION;
  l1_config.tmode.rx_params.coding_scheme        = TM_RX_CODING_SCHEME;
  l1_config.tmode.tx_params.timeslot_alloc       = TM_TX_ALLOCATION;
  l1_config.tmode.tx_params.coding_scheme        = TM_TX_CODING_SCHEME;
  for (i=0; i<8; i++)
    l1_config.tmode.tx_params.txpwr_gprs[i]      = TM_TXPWR_GPRS;

  for (i=0; i<27; i++)
    l1_config.tmode.tx_params.rlc_buffer[i] = 0;
#endif

#if (L1_TPU_DEV ==1)
  l1_config.tmode.rx_params.p_rf_rx_tpu_timings = rf_rx_tpu_timings;
  l1_config.tmode.tx_params.p_rf_tx_tpu_timings = rf_tx_tpu_timings;

//Flexi ABB Delays
  l1_config.tmode.rf_params.p_rf_flexi_abb_delays= rf_flexi_abb_delays;
 #endif

 #if (FF_REPEATED_SACCH == 1)
   l1_config.repeat_sacch_enable = REPEATED_SACCH_ENABLE;  /* Repeated SACCH mode enabled*/
 #endif /* (FF_REPEATED_SACCH == 1) */
 #if (FF_REPEATED_DL_FACCH == 1)
   l1_config.repeat_facch_dl_enable = REPEATED_FACCHDL_ENABLE; /* Repeated FACCH mode enabled */
 #endif
}



/**********************************************************************/
/* Test mode functions used for RF calibration                        */
/**********************************************************************/
#if (L1_FF_MULTIBAND != 0)
UWORD16 tm_std_band = 6; /* Default is EU dual band, GSM900 */
#endif
void Cust_tm_rf_param_write(T_TM_RETURN *tm_return, WORD16 index, UWORD16 value)
{
  switch (index)
  {
  case STD_BAND_FLAG:
    {
      UWORD8 std_tmp, band;

      std_tmp  = value & 0xff;  // tm_band = b7..0 of value
      band = value >> 8;    // band    = b15..8 of value

#if (L1_FF_MULTIBAND == 0)
      if (9 <= std_tmp)  // std max
      {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
        break;
      }
      else if ( GSM_BANDS <= band)
      {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
        break;
      }
      else if ( BAND_NONE == std_config[std_tmp].band[band])
      {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
        break;
      }
      else
      {
        l1_config.std.id = std_tmp;
        tm_band = band;
        // update RAM struct with either default or ffs
        Cust_init_std();
        l1_tpu_init_light();

      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_OK;
      #else
        tm_return->status = E_OK;
      #endif

        break;
      }
#else
#if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
#else
      tm_return->status = E_OK;
#endif
#if(GSM900_SUPPORTED == 1)
      if((1==std_tmp)||(2==std_tmp)||(((5==std_tmp)||(6==std_tmp))&&(0==band)))
      {
        tm_band = RF_GSM900;
      }
      else
#endif
#if(DCS1800_SUPPORTED == 1)
      if((4==std_tmp)||(((5==std_tmp)||(6==std_tmp))&&(1==band)))
      {
//        tm_band = RF_GSM900;
        tm_band = RF_DCS1800;
      }
      else
#endif
#if(PCS1900_SUPPORTED == 1)
      if((3==std_tmp)||((8==std_tmp)&&(1==band)))
      {
        tm_band = RF_PCS1900;
      }
      else
#endif
#if(GSM850_SUPPORTED == 1)
      if((7==std_tmp)||((8==std_tmp)&&(0==band)))
      {
        tm_band = RF_GSM850;
      }
      else
#endif
      {
#if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
#else
        tm_return->status = E_BADINDEX;
#endif
        break;
      }
      tm_std_band = value; /* STD_BAND_FLAG parameter valid */
      break;
#endif
    }

#if (L1_FF_MULTIBAND != 0)
  case TM_BAND:
    switch (value)
    {
    #if GSM850_SUPPORTED
    case 0:
      tm_band = RF_GSM850;
      break;
    #endif
    #if GSM900_SUPPORTED
    case 1:
      tm_band = RF_GSM900;
      break;
    #endif
    #if DCS1800_SUPPORTED
    case 2:
      tm_band = RF_DCS1800;
      break;
    #endif
    #if PCS1900_SUPPORTED
    case 3:
      tm_band = RF_PCS1900;
      break;
    #endif
    default:
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_INVAL;
    #else
      tm_return->status = E_BADINDEX;
    #endif
      break;
    }
#endif

  case INITIAL_AFC_DAC:
    {
	#if(RF_FAM !=61)
      rf.afc.eeprom_afc = (WORD16) value << 3; // shift to put into F13.3 format
      #else
      rf.afc.eeprom_afc = (WORD16) value << 2; // shift to put into F14.2 format
    #endif

      l1_config.params.eeprom_afc = rf.afc.eeprom_afc;

      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_OK;
      #else
        tm_return->status = E_OK;
      #endif
      break;
    }

  default:
    {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
      break;
    }
  } // end switch
}

void Cust_tm_rf_param_read(T_TM_RETURN *tm_return, WORD16 index)
{
  volatile UWORD16 value;

  switch (index)
  {
  case STD_BAND_FLAG:
    {
#if (L1_FF_MULTIBAND == 0)
      value = ((tm_band << 8) | (l1_config.std.id) ); // return global std, tm_band (intel format)
#else
      value = tm_std_band;
#endif
      break;
    }
  case INITIAL_AFC_DAC:
    {
#if (RF_FAM != 61)
      value = rf.afc.eeprom_afc >> 3; // returned as F13.3
 #else
      value = rf.afc.eeprom_afc >> 2; // returned as F14.2
 #endif

      break;
    }


  default:
    {
      tm_return->size = 0;
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
      return;
    }
  } // end switch

  memcpy(tm_return->result, (UWORD8 *)&value, 2);
  tm_return->size = 2;
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_OK;
#else
  tm_return->status = E_OK;
#endif
}

void Cust_tm_rf_table_write(T_TM_RETURN *tm_return, WORD8 index, UWORD8 size, UWORD8 table[])
{
  UWORD8 band=0;

  tm_return->index = index;  // store index before it gets modified
  tm_return->size = 0;

  switch (index)
  {
  case RX_AGC_TABLE:
    {
      if (size != sizeof(AGC_TABLE))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif

        break;
      }

      memcpy(&AGC_TABLE[0], table, size);

      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_OK;
      #else
        tm_return->status = E_OK;
      #endif

      break;
    }
  case AFC_PARAMS:
    {

#if (VCXO_ALGO == 1)
      if (size != 24)  //  4 UWORD32 + 4 WORD16 values
#else
        if (size != 16)  // 4 UWORD32 values
#endif
        {
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_NOMEM;
          #else
            tm_return->status = E_BADSIZE;
          #endif
          break;
        }

        memcpy(&rf.afc.psi_sta_inv, table, size);
        l1_config.params.psi_sta_inv    = rf.afc.psi_sta_inv;
        l1_config.params.psi_st         = rf.afc.psi_st;
        l1_config.params.psi_st_32      = rf.afc.psi_st_32;
        l1_config.params.psi_st_inv     = rf.afc.psi_st_inv;

#if (CODE_VERSION == NOT_SIMULATION)
#if (VCXO_ALGO == 1)
        l1_config.params.afc_dac_center = rf.afc.dac_center;
        l1_config.params.afc_dac_min    = rf.afc.dac_min;
        l1_config.params.afc_dac_max    = rf.afc.dac_max;
        l1_config.params.afc_snr_thr    = rf.afc.snr_thr;
#endif
#endif

      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_OK;
      #else
        tm_return->status = E_OK;
      #endif
        break;
    }
  case RX_AGC_GLOBAL_PARAMS:
    {

#if W_A_PCTM_RX_AGC_GLOBAL_PARAMS
      // For PCTM, 10 bytes must be accepted but 8 bytes only must be written
      if (size == 10)
        size = 8;
#endif

      if (size != 8)  // 4 UWORD16 values
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&rf.rx.agc.low_agc_noise_thr, table, size);
      l1_config.params.low_agc_noise_thr  = rf.rx.agc.low_agc_noise_thr;
      l1_config.params.high_agc_sat_thr   = rf.rx.agc.high_agc_sat_thr;
      l1_config.params.low_agc            = rf.rx.agc.low_agc;
      l1_config.params.high_agc           = rf.rx.agc.high_agc;

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case RX_IL_2_AGC_MAX:
    {
      if (size != sizeof(rf.rx.agc.il2agc_max))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&rf.rx.agc.il2agc_max[0], table, size);
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case RX_IL_2_AGC_PWR:
    {
      if (size != sizeof(rf.rx.agc.il2agc_pwr))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&rf.rx.agc.il2agc_pwr[0], table, size);
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case RX_IL_2_AGC_AV:
    {
      if (size != sizeof(rf.rx.agc.il2agc_av))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&rf.rx.agc.il2agc_av[0], table, size);
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case TX_LEVELS:
    {
      if (size != sizeof(rf_band[tm_band].tx.levels))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&rf_band[tm_band].tx.levels[0], table, size);
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case TX_CAL_CHAN: // generic for all bands
    {
      if (size != sizeof(rf_band[tm_band].tx.chan_cal_table))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&rf_band[tm_band].tx.chan_cal_table[0][0], table, size);
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case TX_CAL_TEMP: // generic for all bands
    {
      if (size != sizeof(rf_band[tm_band].tx.temp))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&rf_band[tm_band].tx.temp[0], table, size);
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case RX_CAL_CHAN:  // generic for all bands
    {
      if (size != sizeof(rf_band[tm_band].rx.agc_bands))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&rf_band[tm_band].rx.agc_bands[0], table, size);
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case RX_CAL_TEMP:  // generic for all bands
    {
      if (size != sizeof(rf_band[tm_band].rx.temp))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&rf_band[tm_band].rx.temp[0], table, size);
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case RX_AGC_PARAMS:
    {
      if (size != sizeof(rf_band[tm_band].rx.rx_cal_params))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&rf_band[tm_band].rx.rx_cal_params, table, size);
#if 0 // TBD      
      if (tm_band == 0)
      {
        l1_config.std.g_magic_band1             = rf_band[tm_band].rx.rx_cal_params.g_magic;
        l1_config.std.lna_att_band1             = rf_band[tm_band].rx.rx_cal_params.lna_att;
        l1_config.std.lna_switch_thr_low_band1  = rf_band[tm_band].rx.rx_cal_params.lna_switch_thr_low;
        l1_config.std.lna_switch_thr_high_band1 = rf_band[tm_band].rx.rx_cal_params.lna_switch_thr_high;
      }
      else if (tm_band == 1)
      {
        l1_config.std.g_magic_band2             = rf_band[tm_band].rx.rx_cal_params.g_magic;
        l1_config.std.lna_att_band2             = rf_band[tm_band].rx.rx_cal_params.lna_att;
        l1_config.std.lna_switch_thr_low_band2  = rf_band[tm_band].rx.rx_cal_params.lna_switch_thr_low;
        l1_config.std.lna_switch_thr_high_band2 = rf_band[tm_band].rx.rx_cal_params.lna_switch_thr_high;
      }
      else
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        break;
      }
#endif // if 0      

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case TX_CAL_EXTREME:
  case RX_CAL_LEVEL:
    {
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_NOSYS;
    #else
      tm_return->status = E_NOSUBSYS;
    #endif
      break;
    }
#if L1_GPRS
  case RLC_TX_BUFFER_CS1:
  case RLC_TX_BUFFER_CS2:
  case RLC_TX_BUFFER_CS3:
  case RLC_TX_BUFFER_CS4:
    {
      UWORD8 i, buffer_size;

      tm_return->index = index;  // store index before it gets modified
      tm_return->size  = 0;

      buffer_size = size/2 + size%2;  // bytes will be concatenated into UWORD16

      if (buffer_size > 27)  //max. number of data bytes
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      // make sure that last byte is zero in case of odd number of bytes
      table[size] = 0;

      // init the whole buffer before downloading new data
      for (i=0; i<27; i++)
        l1_config.tmode.tx_params.rlc_buffer[i] = 0;

      for (i=0; i<buffer_size; i++)
      {
        l1_config.tmode.tx_params.rlc_buffer[i] = (table[2*i+1] << 8) | table[2*i];
      }
      l1_config.tmode.tx_params.rlc_buffer_size = buffer_size;

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
#endif
  case TX_DATA_BUFFER:
    {
      UWORD8 i;

      tm_return->index = index;  // store index before it gets modified
      tm_return->size  = 0;

      if (size != 32)  // 16 UWORD16 (containing 10 data bits each)
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_NOMEM;
        #else
          tm_return->status = E_BADSIZE;
        #endif
        break;
      }

      memcpy(&TM_ul_data, table, size);

      for (i=0; i<16; i++)
      {
        TM_ul_data[i] = TM_ul_data[i] << 6;
      }

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }

  default:
    {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
      break;
    }
  } // end switch
}

void Cust_tm_rf_table_read(T_TM_RETURN *tm_return, WORD8 index)
{
  switch (index)
  {
  case RX_AGC_TABLE:
    {
      tm_return->size = sizeof(AGC_TABLE);
      memcpy(tm_return->result, &AGC_TABLE[0], tm_return->size);
      break;
    }
  case AFC_PARAMS:
    {
#if (VCXO_ALGO == 1)
      tm_return->size = 24;  // 4 UWORD32's + 4 WORD16
#else
      tm_return->size = 16;  // 4 UWORD32's
#endif
      memcpy(tm_return->result, &rf.afc.psi_sta_inv, tm_return->size);
      break;
    }
  case RX_AGC_GLOBAL_PARAMS:
    {

#if W_A_PCTM_RX_AGC_GLOBAL_PARAMS
      // 5 are both supported by ETM and PCTM
      // not clean -> final solution should be 4 words for ETM
      tm_return->size = 10;  // 5 UWORD16's
#else
      tm_return->size = 8;  // 4 UWORD16's
#endif
      memcpy(tm_return->result, &rf.rx.agc.low_agc_noise_thr, tm_return->size);
#if W_A_PCTM_RX_AGC_GLOBAL_PARAMS
      tm_return->result[8] = 0;
      tm_return->result[9] = 0;  // Clear last value because no more supported
#endif
      break;
    }
  case RX_IL_2_AGC_MAX:
    {
      tm_return->size = sizeof(rf.rx.agc.il2agc_max);
      memcpy(tm_return->result, &rf.rx.agc.il2agc_max[0], tm_return->size);
      break;
    }
  case RX_IL_2_AGC_PWR:
    {
      tm_return->size = sizeof(rf.rx.agc.il2agc_pwr);
      memcpy(tm_return->result, &rf.rx.agc.il2agc_pwr[0], tm_return->size);
      break;
    }
  case RX_IL_2_AGC_AV:
    {
      tm_return->size = sizeof(rf.rx.agc.il2agc_av);
      memcpy(tm_return->result, &rf.rx.agc.il2agc_av[0], tm_return->size);
      break;
    }
  case TX_LEVELS:
    {
      tm_return->size = sizeof(rf_band[tm_band].tx.levels);
      memcpy(tm_return->result, &rf_band[tm_band].tx.levels[0], tm_return->size);
      break;
    }
  case TX_CAL_CHAN: // generic for all bands
    {
      tm_return->size = sizeof(rf_band[tm_band].tx.chan_cal_table);
      memcpy(tm_return->result, &rf_band[tm_band].tx.chan_cal_table[0][0], tm_return->size);
      break;
    }
  case TX_CAL_TEMP:  // generic for all bands
    {
      tm_return->size = sizeof(rf_band[tm_band].tx.temp);
      memcpy(tm_return->result, &rf_band[tm_band].tx.temp[0], tm_return->size);
      break;
    }
  case RX_CAL_CHAN:  // generic for all bands
    {
      tm_return->size = sizeof(rf_band[tm_band].rx.agc_bands);
      memcpy(tm_return->result, &rf_band[tm_band].rx.agc_bands[0], tm_return->size);
      break;
    }
  case RX_CAL_TEMP:  // generic for all bands
    {
      tm_return->size = sizeof(rf_band[tm_band].rx.temp);
      memcpy(tm_return->result, &rf_band[tm_band].rx.temp[0], tm_return->size);
      break;
    }
  case RX_AGC_PARAMS:
    {
      // WARNING: sizeof(rf.rx.rx_cal_params[band]) returns 12 because of alignment
      tm_return->size = 10;  // five UWORD16's
      memcpy(tm_return->result, &rf_band[tm_band].rx.rx_cal_params, tm_return->size);
      break;
    }
  case TX_CAL_EXTREME:
  case RX_CAL_LEVEL:
    {
      tm_return->size = 0;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_NOSYS;
    #else
      tm_return->status = E_NOSUBSYS;
    #endif
      return;
    }
#if L1_GPRS
  case RLC_TX_BUFFER_CS1:
  case RLC_TX_BUFFER_CS2:
  case RLC_TX_BUFFER_CS3:
  case RLC_TX_BUFFER_CS4:
    {
      tm_return->size = l1_config.tmode.tx_params.rlc_buffer_size * 2; // UWORD16's
      memcpy(tm_return->result, &l1_config.tmode.tx_params.rlc_buffer[0], tm_return->size);
      break;
    }

#endif
  case TX_DATA_BUFFER:
    {
      UWORD8 i;
      for (i=0; i<16; i++)
      {
        tm_return->result[2*i]=(TM_ul_data[i] >> 6) & 0x00FF;
        tm_return->result[2*i+1]=(TM_ul_data[i] >> 14) & 0x0003;
      }

      tm_return->size = 32; //16*UWORD16
      break;

    }
    #if (RF_FAM == 35)
    case RX_PLL_TUNING_TABLE:
    {
      tm_return->size = sizeof(pll_tuning); //6*UWORD16
      memcpy(tm_return->result, &pll_tuning, tm_return->size);

      pll_tuning.enable    = 0;

      break;
    }
    #endif

  default:
    {
      tm_return->size = 0;
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
      return;
    }
  }  // end switch

  tm_return->index  = index;
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_OK;
#else
  tm_return->status = E_OK;
#endif
}

void Cust_tm_rx_param_write(T_TM_RETURN *tm_return, WORD16 index, UWORD16 value)
{
  switch (index)
  {
  case RX_FRONT_DELAY:
    {
      //delay for dual band not implemented yet
      rf.tx.prg_tx = value;
      l1_config.params.prg_tx_gsm = rf.tx.prg_tx;
      l1_config.params.prg_tx_dcs = rf.tx.prg_tx;

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }


  default:
    {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
      break;
    }
  } // end switch
}

void Cust_tm_rx_param_read(T_TM_RETURN *tm_return, WORD16 index)
{
  volatile UWORD16 value;

  switch (index)
  {
  case RX_FRONT_DELAY:
    {
      value = rf.tx.prg_tx;
      break;
    }

  default:
    {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
      tm_return->size = 0;
      return;
    }
  } // end switch

  memcpy(tm_return->result, (UWORD8 *)&value, 2);
  tm_return->size = 2;
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_OK;
#else
  tm_return->status = E_OK;
#endif
}

void Cust_tm_tx_param_write(T_TM_RETURN *tm_return, WORD16 index, UWORD16 value, UWORD8 band)
{
  switch (index)
  {
  case TX_APC_DAC:
    {
      // generic for all bands
      rf_band[tm_band].tx.levels[l1_config.tmode.tx_params.txpwr].apc = value;

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case TX_RAMP_TEMPLATE:
    {
      if (value >= sizeof(rf_band[tm_band].tx.ramp_tables)/sizeof(rf_band[tm_band].tx.ramp_tables[0]))  // [0..15]
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        break;
      }

      // generic for all bands
      rf_band[tm_band].tx.levels[l1_config.tmode.tx_params.txpwr].ramp_index = value;

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      l1_config.tmode.rf_params.reload_ramps_flag = 1;
      break;
    }
  case TX_CHAN_CAL_TABLE:
    {
      if (value >= sizeof(rf_band[tm_band].tx.chan_cal_table)/sizeof(rf_band[tm_band].tx.chan_cal_table[0]))
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        break;
      }

      // generic for all bands
      rf_band[tm_band].tx.levels[l1_config.tmode.tx_params.txpwr].chan_cal_index = value;

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case TX_BURST_TYPE:
    {
      if (value > 1)  // [0..1]
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        break;
      }
      l1_config.tmode.tx_params.burst_type = value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case TX_BURST_DATA:
    {
      // range is [0..10], currently we support [0..13] at the moment
      if (value > 13)
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        break;
      }
      l1_config.tmode.tx_params.burst_data = value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
  case TX_TRAINING_SEQ:
    {
      if (value > 7)  // [0..7]
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        break;
      }
      l1_config.tmode.tx_params.tsc = value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }

  default:
    {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
      break;
    }
  } // end switch
}

void Cust_tm_tx_param_read(T_TM_RETURN *tm_return, WORD16 index, UWORD8 band)
{
  volatile UWORD16 value;

  switch (index)
  {
  case TX_PWR_LEVEL:
    {
      value = l1_config.tmode.tx_params.txpwr;
      break;
    }
  case TX_APC_DAC:
    {
      value = rf_band[tm_band].tx.levels[l1_config.tmode.tx_params.txpwr].apc;
      break;
    }
  case TX_RAMP_TEMPLATE:
    {
      value = rf_band[tm_band].tx.levels[l1_config.tmode.tx_params.txpwr].ramp_index;
      break;
    }
  case TX_CHAN_CAL_TABLE:
    {
      value = rf_band[tm_band].tx.levels[l1_config.tmode.tx_params.txpwr].chan_cal_index;
      break;
    }
  case TX_BURST_TYPE:
    {
      value = l1_config.tmode.tx_params.burst_type;
      break;
    }
  case TX_BURST_DATA:
    {
      value = l1_config.tmode.tx_params.burst_data;
      break;
    }
  case TX_TIMING_ADVANCE:
    {
      value = l1_config.tmode.tx_params.timing_advance;
      break;
    }
  case TX_TRAINING_SEQ:
    {
      value = l1_config.tmode.tx_params.tsc;
      break;
    }
  case TX_PWR_SKIP:
    {
      value = l1_config.tmode.tx_params.txpwr_skip;
      break;
    }
#if L1_GPRS
  case TX_GPRS_POWER0:
  case TX_GPRS_POWER1:
  case TX_GPRS_POWER2:
  case TX_GPRS_POWER3:
  case TX_GPRS_POWER4:
  case TX_GPRS_POWER5:
  case TX_GPRS_POWER6:
  case TX_GPRS_POWER7:
    {
      value = l1_config.tmode.tx_params.txpwr_gprs[index - TX_GPRS_POWER0];
      break;
    }
  case TX_GPRS_SLOTS:
    {
      value = l1_config.tmode.tx_params.timeslot_alloc;
      break;
    }
  case TX_GPRS_CODING:
    {
      value = l1_config.tmode.tx_params.coding_scheme;
      break;
    }


#endif
  default:
    {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
      tm_return->size = 0;
      return;
    }
  } // end switch

  memcpy(tm_return->result, (UWORD8 *)&value, 2);
  tm_return->size = 2;
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_OK;
#else
  tm_return->status = E_OK;
#endif
}

void Cust_tm_tx_template_write(T_TM_RETURN *tm_return, WORD8 index, UWORD8 size, UWORD8 table[])
{
  if (index >= (UWORD8)(sizeof(rf_band[tm_band].tx.ramp_tables)/sizeof(T_TX_RAMP)))
  {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
  }
  else if (size != sizeof(T_TX_RAMP))
  {
    // We are writing both the up and down ramps; size must be exact.
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_NOMEM;
    #else
      tm_return->status = E_BADSIZE;
    #endif
  }
  else
  {
    memcpy(rf_band[tm_band].tx.ramp_tables[index].ramp_up, &table[0], size/2);
    memcpy(rf_band[tm_band].tx.ramp_tables[index].ramp_down, &table[size/2], size/2);
  #if (ETM_PROTOCOL == 1)
    tm_return->status = -ETM_OK;
  #else
    tm_return->status = E_OK;
  #endif
    l1_config.tmode.rf_params.reload_ramps_flag = 1;
  }

  tm_return->index = index;
  tm_return->size = 0;
}

void Cust_tm_tx_template_read(T_TM_RETURN *tm_return, WORD8 index)
{
  tm_return->index = index;

//  if ( index >= (((UWORD8)sizeof(rf_band[tm_band].tx.ramp_tables)) /(UWORD8) sizeof(T_TX_RAMP)))  //omaps00090550
//Patch for OMAPS00151237
  if (index >= (UWORD8)(sizeof(rf_band[tm_band].tx.ramp_tables)/sizeof(T_TX_RAMP)))
  {
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_INVAL;
      #else
        tm_return->status = E_BADINDEX;
      #endif
    tm_return->size = 0;
    return;
  }

  memcpy(&tm_return->result[0], rf_band[tm_band].tx.ramp_tables[index].ramp_up, sizeof(rf_band[tm_band].tx.ramp_tables[index].ramp_up));
  memcpy(&tm_return->result[sizeof(rf_band[tm_band].tx.ramp_tables[index].ramp_up)], rf_band[tm_band].tx.ramp_tables[index].ramp_down, sizeof(rf_band[tm_band].tx.ramp_tables[index].ramp_down));
  tm_return->size = sizeof(rf_band[tm_band].tx.ramp_tables[index]);
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_OK;
#else
  tm_return->status = E_OK;
#endif
}

void Cust_tm_special_param_write(T_TM_RETURN *tm_return, WORD16 index, UWORD16 value)
{
  tm_return->size = 0;
  tm_return->index = index;
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_NOSYS;
#else
  tm_return->status = E_NOSYS;
#endif
}

void Cust_tm_special_param_read(T_TM_RETURN *tm_return, WORD16 index)
{
  tm_return->size = 0;
  tm_return->index = index;
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_NOSYS;
#else
  tm_return->status = E_NOSYS;
#endif
}

void Cust_tm_special_table_write(T_TM_RETURN *tm_return, WORD8 index, UWORD8 size, UWORD8 table[])
{
  tm_return->size = 0;
  tm_return->index = index;
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_NOSYS;
#else
  tm_return->status = E_NOSYS;
#endif
}

void Cust_tm_special_table_read(T_TM_RETURN *tm_return, WORD8 index)
{
  tm_return->size = 0;
  tm_return->index = index;
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_NOSYS;
#else
  tm_return->status = E_NOSYS;
#endif
}

void Cust_tm_special_enable(T_TM_RETURN *tm_return, WORD16 action)
{
  tm_return->size = 0;
  tm_return->index = action;
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_NOSYS;
#else
  tm_return->status = E_NOSYS;
#endif
}


#endif // TESTMODE
