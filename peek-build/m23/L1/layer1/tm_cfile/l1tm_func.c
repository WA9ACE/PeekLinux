/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1TM_FUNC.C
 *
 *        Filename l1tm_func.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#include "l1_macro.h"
#include "l1_confg.h"

#if TESTMODE
  #define L1TM_FUNC_C

  #include <string.h>
  #include <math.h>
  #include "abb.h"

#include "general.h"

  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_time.h"
  #include "l1_signa.h"

  #include "l1tm_defty.h"

#if (CODE_VERSION != SIMULATION)
  #include "pld.h"
 #endif


  #if (TRACE_TYPE==1) || (TRACE_TYPE==4) || (TRACE_TYPE==7) || (TRACE_TYPE==0)
    #include "rvt_gen.h"
    extern T_RVT_USER_ID  tm_trace_user_id;
  #endif

  #if (AUDIO_TASK == 1)
    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_defty.h"
  #endif

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
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "l1_varex.h"
  #include "l1_proto.h"

  #include "mem.h"

  #if (CODE_VERSION != SIMULATION)

    #if (RF_FAM == 61)
      #include "tpudrv61.h"
      #include "l1_rf61.h"
      #include "l1tm_tpu61.h"
	  #if (DRP_FW_EXT==1)
      #include "l1_drp_inc.h"
	  #else
      #include "drp_drive.h"
	  #endif
    #endif

    #if (RF_FAM == 60)
      #include "tpudrv60.h"
      #include "l1_rf60.h"
      #include "l1tm_tpu60.h"
      #include "drp_drive.h"
    #endif

    #if (RF_FAM==43)
      #include "tpudrv43.h"
      #include "l1_rf43.h"
      #include "l1tm_tpu43.h"
    #endif
    #if (RF_FAM == 35)
      #include "tpudrv35.h"
      #include "l1_rf35.h"
      #include "l1tm_tpu35.h"
    #endif

    #if (RF_FAM == 12)
      #include "tpudrv12.h"
      #include "l1_rf12.h"
      #include "l1tm_tpu12.h"
    #endif

    #if (RF_FAM == 10)
      #include "tpudrv10.h"
      #include "l1_rf10.h"
      #include "l1tm_tpu10.h"
    #endif

    #if (RF_FAM == 8)
      #include "tpudrv8.h"
      #include "l1_rf8.h"
      #include "l1tm_tpu8.h"
    #endif

    #if (RF_FAM == 2)
      #include "tpudrv2.h"
      #include "l1_rf2.h"
      #include "l1tm_tpu2.h"
    #endif

  #else

    #if (RF_FAM == 2)
      #include "l1_rf2.h"
    #endif

  #endif

  #include <assert.h>
  #include <string.h>

  #include "l1tm_msgty.h"
  #include "l1tm_signa.h"
  #include "l1tm_varex.h"
  #include "l1tm_ver.h"

  #if L1_GPRS
    #include "l1p_cons.h"
    #include "l1p_msgt.h"
    #include "l1p_deft.h"
    #include "l1p_vare.h"
    #include "l1p_sign.h"
  #endif

  #if ((L1_STEREOPATH == 1) && (OP_L1_STANDALONE == 1))
    #include "sys_dma.h"
  #endif

#if(L1_FF_MULTIBAND == 1)
extern UWORD8 tm_band;
#endif /*if (L1_FF_MULTIBAND == 1)*/
  

#if (DRP_FW_EXT==0)
  #if (L1_DRP == 1)
    #include "drp_drive.h"
    #include "drp_defines.h"
    #include "drp_api.h"
    #include "drp_extern_dependencies.h"
  #endif //L1_DRP

#if (RF_FAM == 61)
extern T_DRP_SW_DATA drp_sw_data_init;
extern T_DRP_SW_DATA drp_sw_data_calib;
extern T_DRP_SW_DATA drp_sw_data_calib_saved;
extern UINT16 g_pcb_config; //ompas00090550 #262 removal
#endif // DRP_FW_EXT==0
#endif

extern void gpio_39_toggle(UINT16 state);


// Prototypes from external functions
//------------------------------------
UWORD16 Convert_l1_radio_freq(SYS_UWORD16 radio_freq);

void Cust_tm_rf_param_write        (T_TM_RETURN *tm_return, WORD16 index, UWORD16 value);
void Cust_tm_rf_param_read         (T_TM_RETURN *tm_return, WORD16 index);
void Cust_tm_rf_table_write        (T_TM_RETURN *tm_return, WORD8 index, UWORD8 size, UWORD8 table[]);
void Cust_tm_rf_table_read         (T_TM_RETURN *tm_return, WORD8 index);
void Cust_tm_rx_param_write        (T_TM_RETURN *tm_return, WORD16 index, UWORD16 value);
void Cust_tm_rx_param_read         (T_TM_RETURN *tm_return, WORD16 index);
void Cust_tm_tx_param_write        (T_TM_RETURN *tm_return, WORD16 index, UWORD16 value, UWORD8 band);
void Cust_tm_tx_param_read         (T_TM_RETURN *tm_return, WORD16 index, UWORD8 band);
void Cust_tm_tx_template_write     (T_TM_RETURN *tm_return, WORD8 index, UWORD8 size, UWORD8 table[]);
void Cust_tm_tx_template_read      (T_TM_RETURN *tm_return, WORD8 index);
void Cust_tm_special_param_write   (T_TM_RETURN *tm_return, WORD16 index, UWORD16 value);
void Cust_tm_special_param_read    (T_TM_RETURN *tm_return, WORD16 index);
void Cust_tm_special_table_write   (T_TM_RETURN *tm_return, WORD8 index, UWORD8 size, UWORD8 table[]);
void Cust_tm_special_table_read    (T_TM_RETURN *tm_return, WORD8 index);
void Cust_tm_special_enable        (T_TM_RETURN *tm_return, WORD16 action);

#if (CODE_VERSION != SIMULATION)
  void Cust_tm_tpu_table_write       (T_TM_RETURN *tm_return, WORD8 index, UWORD8 size, UWORD8 table[]);
  void Cust_tm_tpu_table_read        (T_TM_RETURN *tm_return, WORD8 index);
#endif

#if (L1_TPU_DEV == 1)
  void Cust_tm_flexi_tpu_table_write(T_TM_RETURN * tm_return, WORD8 index, UWORD8 size, WORD16 table[ ]);
  void Cust_tm_flexi_tpu_table_read(T_TM_RETURN * tm_return, WORD8 index);

  //Flexi ABB Delays
  void Cust_tm_flexi_abb_write(T_TM_RETURN * tm_return, WORD8 index, UWORD8 size, WORD16 table[ ]);
  void Cust_tm_flexi_abb_read(T_TM_RETURN * tm_return, WORD8 index);

#endif

// DRP Calibration
  void l1tm_drp_calib_read(T_TESTMODE_PRIM * prim, T_TM_RETURN * tm_return);
  void l1tm_drp_calib_write(T_TESTMODE_PRIM * prim, T_TM_RETURN * tm_return);

//------------------------------------
// Prototypes from external functions
//------------------------------------

void Cust_tm_init                   (void);
void l1tm_reset_rx_state            (void);
void l1tm_reset_rx_stats            (void);

#if L1_GPRS
  void l1pa_reset_cr_freq_list      (void);
#endif

//------------------------------------
// Prototypes from internal functions
//------------------------------------

void l1tm_initialize_var(void);
UWORD16 l1tm_convert_arfcn2l1ch(UWORD16 arfcn, UWORD8 *error_flag);
void l1tm_stats_read(T_TM_RETURN *tm_return, WORD16 type, UWORD16 bitmask);
void tm_transmit(T_TM_RETURN *tm_ret);
void l1tm_PRBS1_generate(UWORD16 *TM_ul_data);

#if ((L1_STEREOPATH == 1) && (OP_L1_STANDALONE == 1))
  void l1tm_stereopath_DMA_handler(SYS_UWORD16 dma_status);
  void l1tm_stereopath_fill_buffer(void* buffer_address);
  UWORD16 l1tm_stereopath_get_pattern(UWORD16 sampling_freq, UWORD16 sin_freq_left,UWORD16 sin_freq_right, UWORD8 data_type);
#endif

/***********************************************************************/
/*                           TESTMODE 3.X                              */
/***********************************************************************/

static UWORD8 tx_param_band=0;  // used in tx_param_write/read; default is GSM900




// RF,(ANALOG)or other hardware dependent functions
// - work done by tmrf.c functions for each product.

// TestMode functions that modify the state variables
// within the L1A - may need to allocate space
// dynamically if this is the first time calling
// these functions.

// TestMode functions that start L1A state machines
// may need to send L1A primitives to change L1A state.

void l1tm_rf_param_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  tm_return->index = prim->u.tm_params.index;
  tm_return->size = 0;

  switch (prim->u.tm_params.index)
  {
	  #if (FF_REPEATED_SACCH == 1)
	      // Repeated SACCH mode
	   case REPEATED_SACCH_ENA_FLAG:
	   {
	    l1_config.repeat_sacch_enable = prim->u.tm_params.value;
	    #if (ETM_PROTOCOL == 1)
	            tm_return->status = -ETM_OK;
	    #else
	            tm_return->status = E_OK;
	    #endif
	          break;
	      }
	  #endif /* FF_REPEATED_SACCH */

	  #if (FF_REPEATED_DL_FACCH == 1)
	      // Repeated FACCH mode
	      case REPEATED_FACCHDL_ENA_FLAG:
	      {
	          l1_config.repeat_facch_dl_enable = prim->u.tm_params.value;
	       #if (ETM_PROTOCOL == 1)
	            tm_return->status = -ETM_OK;
	       #else
	            tm_return->status = E_OK;
	       #endif/*(ETM_PROTOCOL == 1)*/

	       break;
	      }
     #endif /* FF_REPEATED_DL_FACCH == 1 */
    case BCCH_ARFCN:
    {
      UWORD16 bcch_arfcn;
      UWORD8  error_flag;

      bcch_arfcn = l1tm_convert_arfcn2l1ch(prim->u.tm_params.value, &error_flag);

      if (error_flag)
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
      }
      else
      {
        l1_config.tmode.rf_params.bcch_arfcn = bcch_arfcn;

        // now change on the fly
        // no reason to check dedicated_active flag...
        // we just set these 2 globals for FB/SB/BCCH tests
        l1a_l1s_com.nsync.list[0].radio_freq = l1_config.tmode.rf_params.bcch_arfcn;
        l1a_l1s_com.Scell_info.radio_freq    = l1_config.tmode.rf_params.bcch_arfcn;
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_OK;
      #else
        tm_return->status = E_OK;
      #endif
      }
      break;
    }
    case TCH_ARFCN:
    {
      T_CHN_SEL *chan_sel;
      UWORD16    tch_arfcn;
      UWORD8     error_flag;

      tch_arfcn = l1tm_convert_arfcn2l1ch(prim->u.tm_params.value, &error_flag);

      if (error_flag)
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
      else
      {
        l1_config.tmode.rf_params.tch_arfcn = tch_arfcn;

        // now change on the fly if necessary
        if (l1_config.TestMode && l1tm.tmode_state.dedicated_active)
        {
          chan_sel = &(l1a_l1s_com.dedic_set.aset->achan_ptr->desc_ptr->chan_sel);
          chan_sel->rf_channel.single_rf.radio_freq = l1_config.tmode.rf_params.tch_arfcn;
        }
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_OK;
      #else
        tm_return->status = E_OK;
      #endif
      }
      break;
    }
    case MON_ARFCN:
    {
      UWORD16 mon_arfcn;
      UWORD8  error_flag;

      mon_arfcn = l1tm_convert_arfcn2l1ch(prim->u.tm_params.value, &error_flag);

      if (error_flag)
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
      else
      {
        l1_config.tmode.rf_params.mon_arfcn = mon_arfcn;
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_OK;
      #else
        tm_return->status = E_OK;
      #endif
      }
      break;
    }
    #if L1_GPRS
      case PDTCH_ARFCN:
      {
        UWORD16    pdtch_arfcn;
        UWORD8     error_flag;

        pdtch_arfcn = l1tm_convert_arfcn2l1ch(prim->u.tm_params.value, &error_flag);

        if (error_flag)
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        else
        {
          l1_config.tmode.rf_params.pdtch_arfcn = pdtch_arfcn;
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_OK;
        #else
          tm_return->status = E_OK;
        #endif
        }
        break;
      }
    #endif
    case AFC_ENA_FLAG:
    {
      l1_config.afc_enable = prim->u.tm_params.value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    case AFC_DAC_VALUE:
    {
      WORD16 afc_value = prim->u.tm_params.value;
      // 13-bit AFC DAC
     #if(RF_FAM != 61)
           if (afc_value<-4096 || afc_value>4095)
     #else
          if (afc_value<-8192 || afc_value>8191)
     #endif
       {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        break;
       }

      if (!l1_config.afc_enable)
      {
        // write AFC value to AFC DAC ASAP!! AFC DAC will be updated by any RX
        // or TX test.
        l1s.afc = afc_value;
      }

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    #if L1_GPRS
      case MULTISLOT_CLASS:
      {
        UWORD8    multislot_class;

        multislot_class = prim->u.tm_params.value;

        if ((multislot_class < 1) || (multislot_class > 12))
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        else
        {
          l1_config.tmode.rf_params.multislot_class = multislot_class;
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_OK;
        #else
          tm_return->status = E_OK;
        #endif
        }
        break;
      }
#endif // end of L1_GPRS
    default:
    {
      Cust_tm_rf_param_write(tm_return,
                             prim->u.tm_params.index,
                             prim->u.tm_params.value);
      break;
    }
  } // end switch
}

UWORD16 l1tm_convert_arfcn2l1ch(UWORD16 arfcn, UWORD8 *error_flag)
#if (L1_FF_MULTIBAND == 0)    
{
  /*  Here, before we store the channel numbers to the l1_config structure,
      we convert from ETSI to TI channel numbering system.  The GGT
      ALWAYS expects the ETSI numbering system as input and output.

      We need to do the OPPOSITE of what is done in convert_l1_arfcn() in
      tpudrvX.c

      ***************************************
      *** convert arfcn's from ETSI to TI ***
      ***************************************
      **                                   **
      **     ETSI                 TI       **
      **           0   GSM          174    **
      **    1 -  124   GSM      1 - 124    **
      **  975 - 1023  E-GSM   125 - 173    **
      **  512 -  885   DCS    174 - 548    **
      **                                   **
      ***************************************      */

  *error_flag = 0;

  switch (l1_config.std.id)
  {
    case GSM:
    case DCS1800:
    case PCS1900:
    case GSM850:
      break;

    case DUAL:
      if ((arfcn >= 512) && (arfcn <= 885)) arfcn -= 337;
      else if (arfcn > 124) *error_flag = 1; // invalid arfcn
      break;

    case DUALEXT:
      if (arfcn == 0) arfcn = 174;
      else if ((arfcn >= 975) && (arfcn <= 1023)) arfcn -= 850;
      else if ((arfcn >= 512) && (arfcn <= 885))  arfcn -= 337;
      else if ((arfcn >= 1)   && (arfcn <= 124));
      else *error_flag = 1; // invalide arfcn
      break;

    case DUAL_US:  // GSM850:128-251  PCS1900:512-810
      if ((arfcn >= 128) && (arfcn <= 251)) arfcn -= 127;
      else if ((arfcn >= 512) && (arfcn <= 810)) arfcn -= 387;
      else *error_flag = 1; // invalid arfcn
      break;

    default:
      *error_flag = 1; // invalid std.id
      break;
  } // end switch
  return arfcn;
}
#else // L1_FF_MULTIBAND = 1 below

{
    *error_flag=0;
    if(tm_band == RF_PCS1900)
    {
      arfcn = arfcn + 512;
    }
    return(arfcn);
#if 0
  UWORD16 l1_radio_freq = 0;
  UWORD8 effective_band_id = 0;
  *error_flag = 1; 
  for (effective_band_id = 0; effective_band_id < RF_NB_SUBBANDS; effective_band_id ++)
  {
    if( multiband_conversion_data[effective_band_id].physical_band_id == tm_band)
    {
      if( (arfcn - multiband_conversion_data[effective_band_id].first_tpu_radio_freq) < multiband_conversion_data[effective_band_id].nbmax_carrier)
      {
        l1_radio_freq =   arfcn - multiband_conversion_data[effective_band_id].first_tpu_radio_freq + multiband_conversion_data[effective_band_id].first_radio_freq;          
        *error_flag = 0; 
        return(l1_radio_freq);
      }
    }
  }
  return(l1_radio_freq);
#endif

}

#endif // #if (L1_FF_MULTIBAND == 0) else


void l1tm_rf_param_read(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  volatile UWORD16 value;

  tm_return->index = prim->u.tm_params.index;

  switch (prim->u.tm_params.index)
  {
    #if (FF_REPEATED_SACCH == 1 )
    /* Repeated SACCH mode */
    case REPEATED_SACCH_ENA_FLAG:
   {
     value = l1_config.repeat_sacch_enable;
     break;
   }
   #endif /* FF_REPEATED_SACCH */

   #if FF_REPEATED_DL_FACCH
       // Repeated FACCH mode
       case REPEATED_FACCHDL_ENA_FLAG:
       {
           value = l1_config.repeat_facch_dl_enable;
           break;
       }
   #endif /* FF_REPEATED_DL_FACCH */


    case BCCH_ARFCN:
    {
      // return ETSI value for channel number
      value = Convert_l1_radio_freq(l1_config.tmode.rf_params.bcch_arfcn);
      break;
    }
    case TCH_ARFCN:
    {
      // return ETSI value for channel number
      value = Convert_l1_radio_freq(l1_config.tmode.rf_params.tch_arfcn);
      break;
    }
    case MON_ARFCN:
    {
      // return ETSI value for channel number
      value = Convert_l1_radio_freq(l1_config.tmode.rf_params.mon_arfcn);
      break;
    }
    #if L1_GPRS
      case PDTCH_ARFCN:
      {
        // return ETSI value for channel number
        value = Convert_l1_radio_freq(l1_config.tmode.rf_params.pdtch_arfcn);
        break;
      }
    #endif
    case AFC_ENA_FLAG:
    {
      value = l1_config.afc_enable;
      break;
    }
    case AFC_DAC_VALUE:
    {
      value = l1s.afc; // returned as F13.3
      break;
    }
    #if L1_GPRS
      case MULTISLOT_CLASS:
      {
        value = l1_config.tmode.rf_params.multislot_class;
        break;
      }
    #endif
    default:
    {
      Cust_tm_rf_param_read(tm_return, prim->u.tm_params.index);
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

void l1tm_rf_table_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  Cust_tm_rf_table_write(tm_return,
                         prim->u.tm_table.index,
                         prim->str_len_in_bytes - 1,    // subtract 8-bit index
                         prim->u.tm_table.table);
}

void l1tm_rf_table_read(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  Cust_tm_rf_table_read(tm_return, prim->u.tm_table.index);
}

void l1tm_rx_param_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  tm_return->index = prim->u.tm_params.index;
  tm_return->size = 0;

  switch (prim->u.tm_params.index)
  {
    case RX_AGC_GAIN:
    {
      WORD8 gain = prim->u.tm_params.value;

      // It is up to the user to write a valid gain,
      // one that falls within the range of gains in the current RF
      // AGC gain can only be controlled in 2dB steps as the bottom bit (bit zero)
      // corresponds to the lna_off bit
      l1_config.tmode.rx_params.agc     = gain & ~0x01;
      l1_config.tmode.rx_params.lna_off = gain &  0x01;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    case RX_TIMESLOT:
    {
      if (prim->u.tm_params.value > 7)
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        break;
      }

      l1_config.tmode.rx_params.slot_num = prim->u.tm_params.value;
      if (l1_config.TestMode && l1tm.tmode_state.dedicated_active)
      {
        // currently CANNOT change RX slot on the fly!
      }

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    case RX_PM_ENABLE:
    {
      l1_config.tmode.rx_params.pm_enable = prim->u.tm_params.value;

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    #if L1_GPRS
      case RX_GPRS_SLOTS:
      {
        // At least one DL TS needs to be allocated
        if (!prim->u.tm_params.value)
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_INVAL;
          #else
            tm_return->status = E_INVAL;
          #endif
        else
        {
          l1_config.tmode.rx_params.timeslot_alloc = prim->u.tm_params.value;
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_OK;
        #else
          tm_return->status = E_OK;
        #endif
        }
        break;
      }
      case RX_GPRS_CODING:
      {
        UWORD8 coding_scheme;

        coding_scheme = prim->u.tm_params.value;
        if ((coding_scheme < 1) || (coding_scheme > 6) || (coding_scheme == 3))
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_INVAL;
          #else
            tm_return->status = E_INVAL;
          #endif
        else
        {
          l1_config.tmode.rx_params.coding_scheme = prim->u.tm_params.value;
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_OK;
        #else
          tm_return->status = E_OK;
        #endif
        }
        break;
      }
    #endif
    case RX_AGC_ENA_FLAG:
    {
      l1_config.agc_enable = prim->u.tm_params.value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    default:
    {
      Cust_tm_rx_param_write(tm_return,
                             prim->u.tm_params.index,
                             prim->u.tm_params.value);
      break;
    }
  } // end switch
}

#if (L1_DRP == 1)
  void l1tm_drp_sw_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
  {
    WORD8 error=0;
    tm_return->cid=DRP_SW_WRITE;
    tm_return->index = prim->u.tm_table.index;
    tm_return->size = 0;

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
     #else
      tm_return->status = E_OK;
    #endif

    switch (prim->u.tm_table.index)
    {
      case FFS_TO_SRM:
      {
        #if (OP_L1_STANDALONE ==0)
          // Call function to load ref sw from FFS to SRM
          error = drp_ref_sw_upload_from_ffs(prim->u.tm_table.table);
          if(error < 0)
          {
            #if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
            #else
              tm_return->status = E_INVAL;
            #endif
          }
          else
          {
           #if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
  	      #else
  	      tm_return->status = E_OK;
  	    #endif
          }
       #else // OP_L1_STANDALONE
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_NOSYS;
  	  #else
  	    tm_return->status = (UWORD8)(-E_NOSYS); //omaps00090550
          #endif
        #endif// OP_L1_STANDALONE
        break;
      }

      default:
      {
        // nothing to do now.
        break;
      }
    } // end switch
  }
#endif

#if (L1_DRP == 1)

void l1tm_drp_calib_read(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
  {
    // Do Nothing Possible Future use. Intentionally left blank
   }

void l1tm_drp_calib_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
  {
    INT32 error=0;//ompas00090550
#if (OP_L1_STANDALONE == 1)
    UINT8  *ptrsrc, *ptrdst;
    UINT16 indx, strsize;
#endif
    UINT16 band;
    volatile UINT16 * ptr_gpio, temp16;

#define GPIO2_LATCH_IN         0xfffe5800        // GPIO32-48 input register
#define GPIO2_LATCH_OUT        0xfffe5802        // GPIO32-48 output register
#define GPIO2_CNTL             0xfffe5804        // GPIO32-48 io config, 1=inp, 0= out

#define GPIO0_LATCH_IN         0xfffe4800        // GPIO0-15 input register
#define GPIO0_LATCH_OUT        0xfffe4802        // GPIO0-15 output register
#define GPIO0_CNTL             0xfffe4804        // GPIO0-15 io config, 1=inp, 0= out

    tm_return->cid=DRP_CALIB_WRITE;
    tm_return->index = prim->u.tm_params.index;
    tm_return->size = 0;


    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
     #else
      tm_return->status = E_OK;
    #endif

//#if (OP_L1_STANDALONE == 1)
#if 0
	  // Copy drp_sw_data_init into drp_sw_data_calib
  strsize = sizeof(T_DRP_SW_DATA);
  ptrsrc = (UINT8 *)(&drp_sw_data_init);
  ptrdst = (UINT8 *)(&drp_sw_data_calib);

    for(indx=0;indx < strsize;indx++)
    *ptrdst++ = *ptrsrc++;
#endif

#if 0
  // GPIO-47 is used for timing measurement.
  // To enable GPIO - 47as an output, Bit 15 of GPIO2_CNTL = 0
  ptr_gpio = (UINT16*)GPIO2_CNTL;
  temp16 = *ptr_gpio;
  temp16 &= ~(0x8000);
  *ptr_gpio = temp16;

  // GPIO47 = 1
  ptr_gpio = (UINT16*)GPIO2_LATCH_OUT;
  temp16 = *ptr_gpio;
  temp16 |= 0x8000;
  *ptr_gpio = temp16;
#endif
    switch (tm_return->index)
    {
      case DRP_DUMMY: //Do Nothing Test Purposes
	  	break;

      case DRP_READ_FROM_FLASH: // Force the Update of drp_sw_data_calib
      {
           // Call function to load data  sw from FFS to SRM TBD Use the new Function
           error = drp_sw_data_calib_upload_from_ffs(&drp_sw_data_calib);

       //    error = drp_copy_ref_sw_to_drpsrm( (unsigned char *) &drp_ref_sw);

           error = drp_copy_sw_data_to_drpsrm(&drp_sw_data_calib);

           if(error < 0)
            {
#if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
#else
              tm_return->status = E_INVAL;
#endif
           }
          else
           {
#if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
#else
            tm_return->status = E_OK;
#endif
          	}

        break;
      }


      case DRP_DCXO_CALIB:
      {

	  #if (DRP_FW_EXT==1) //pinghua change it as 1
         band = GSM_BAND;
	  if((g_pcb_config == RF_EU_DUALBAND) || (g_pcb_config == RF_EU_TRIBAND) || (g_pcb_config == RF_PCS1900_900_DUALBAND))
           band = EGSM_BAND;

	  error = drp_dcxo_calib(band, &drp_sw_data_calib);
	  #else
	  error = drp_dcxo_calib(GSM_BAND, &drp_sw_data_calib);
	  #endif
          if(error < 0)
          {
#if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
#else
              tm_return->status = E_INVAL;
#endif
          }
          else
          {
#if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
#else
  	      tm_return->status = E_OK;
#endif
          }

        break;
      }


      case DRP_TX_RX_COMMON:
      {
          error = drp_tx_rx_common_calib(&drp_sw_data_calib);

          if(error < 0)
          {
#if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
 #else
              tm_return->status = E_INVAL;
#endif
          }
          else
          {
#if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
 #else
  	      tm_return->status = E_OK;
 #endif
          }
        break;
      }


      case DRP_LNA_CFREQ:
      {
          error = drp_lna_cfreq_calib( (UWORD16) prim->u.tm_params.value, &drp_sw_data_calib);
          if(error < 0)
          {
#if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
#else
              tm_return->status = E_INVAL;
#endif
          }
          else
          {
#if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
#else
  	      tm_return->status = E_OK;
#endif
          }
        break;
      }


      case DRP_IQMC:
      {
          error = drp_iqmc_calib(prim->u.tm_params.value, &drp_sw_data_calib);
          if(error < 0)
          {
#if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
#else
              tm_return->status = E_INVAL;
#endif
          }
          else
          {
#if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
#else
  	      tm_return->status = E_OK;
#endif
          }
        break;
      }


      case DRP_MIXER_POLE:
      {
          error =drp_mixer_pole_calib(prim->u.tm_params.value, &drp_sw_data_calib);
          if(error < 0)
          {
#if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
#else
              tm_return->status = E_INVAL;
#endif
          }
          else
          {
#if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
#else
  	      tm_return->status = E_OK;
 #endif
          }
        break;
      }

      case DRP_SCF_POLE:
      {
          error = drp_scf_pole_calib(prim->u.tm_params.value, &drp_sw_data_calib);
          if(error < 0)
          {
#if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
#else
              tm_return->status = E_INVAL;
#endif
          }
          else
          {
#if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
 #else
  	      tm_return->status = E_OK;
#endif
          }
        break;
      }


      case DRP_AFE_GAIN_1:
      {
          error = drp_afe_gain_calib1 (prim->u.tm_params.value, &drp_sw_data_calib);
          if(error < 0)
          {
#if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
 #else
              tm_return->status = E_INVAL;
#endif
          }
          else
          {
#if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
#else
  	      tm_return->status = E_OK;
#endif
          }
        break;
      }


      case DRP_AFE_GAIN_2:
      {
          error = drp_afe_gain_calib2 (prim->u.tm_params.value, &drp_sw_data_calib);
          if(error < 0)
          {
 #if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
 #else
              tm_return->status = E_INVAL;
 #endif
          }
          else
          {
 #if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
   #else //OP_L1_STANDALONE == 1
  	      tm_return->status = E_OK;
  #endif
          }
        break;
      }



   case DRP_WRITE_TO_FLASH:
      {
#if (OP_L1_STANDALONE ==0)
           // Call function to load data  sw from FFS to SRM TBD Use the new Function
           error = drp_sw_data_calib_upload_to_ffs(&drp_sw_data_calib);

          // TBD the followign immediate error handling can be removed once the PS related FFS functionality is in.
           if (error < 0)
           {
#if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_NOSYS;
 #else
  	      tm_return->status = E_NOSYS;//OMAPS00090550
 #endif
             return;
      	    }

 #else // OP_L1_STANDALONE

          if(error < 0)
          {
#if (ETM_PROTOCOL == 1)
              tm_return->status = -ETM_FATAL;
  #else
              tm_return->status = E_INVAL;
 #endif
          }
          else
          {
#if (ETM_PROTOCOL == 1)
             tm_return->status = -ETM_OK;
#else
  	      tm_return->status = E_OK;
#endif
          }

#endif //OP_L1_STANDALONE
        break;
      }

      default:
      {
        // nothing to do now.
      }
    } // end switch
#if 0
  // GPIO47 = 1
  ptr_gpio = (UINT16*)GPIO2_LATCH_OUT;
  temp16 = *ptr_gpio;
  temp16 &= ~(0x8000);
  *ptr_gpio = temp16;
#endif

  }

#endif //L1_DRP



void l1tm_rx_param_read(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  volatile UWORD16 value;

  tm_return->index = prim->u.tm_params.index;

  switch (prim->u.tm_params.index)
  {
    case RX_AGC_GAIN:
    {
      value = l1_config.tmode.rx_params.agc | l1_config.tmode.rx_params.lna_off;
      break;
    }
    case RX_TIMESLOT:
    {
      value = l1_config.tmode.rx_params.slot_num;
      break;
    }
    case RX_AGC_ENA_FLAG:
    {
      value = l1_config.agc_enable;
      break;
    }
    case RX_PM_ENABLE:
    {
      value = l1_config.tmode.rx_params.pm_enable;
      break;
    }
    #if L1_GPRS
      case RX_GPRS_SLOTS:
      {
        value = l1_config.tmode.rx_params.timeslot_alloc;
        break;
      }
      case RX_GPRS_CODING:
      {
        value = l1_config.tmode.rx_params.coding_scheme;
        break;
      }
    #endif
    default:
    {
      Cust_tm_rx_param_read(tm_return, prim->u.tm_params.index);
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

void l1tm_tx_param_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  tm_return->index = prim->u.tm_params.index;
  tm_return->size = 0;

  switch (prim->u.tm_params.index)
  {
    case TX_PWR_LEVEL:
    {
      UWORD8 temp_txpwr, temp_band;

      if (prim->u.tm_params.value < 100)  // GSM900
      {
        temp_txpwr = prim->u.tm_params.value;
        temp_band = 0;
      }
      else if (prim->u.tm_params.value < 200)  // DCS1800
      {
        temp_txpwr = prim->u.tm_params.value - 100;
        temp_band = 1;
      }
      else if (prim->u.tm_params.value < 300)  // PCS1900
      {
        temp_txpwr = prim->u.tm_params.value - 200;
        temp_band = 2;
      }
      else  // force invalid values to return -ETM_INVAL or E_INVAL
      {
        temp_txpwr = 50;
        temp_band = 10;
      }

      // Note that the pwr level is only checked for being within the range [0..31]
      // because all pwr levels should be testable.
      // For subfunctions [TX_APC_DAC..TX_DELAY_DOWN]:
      //     temp_txpwr +   0 ==> GSM900
      //     temp_txpwr + 100 ==> DCS1800
      //     temp_txpwr + 200 ==> PCS1900

      // Changing tx pwr level on the fly while in continuous mode is not supported.
      if (temp_txpwr > 31 || temp_band > 2 ||
          l1_config.tmode.rf_params.tmode_continuous == TM_CONTINUOUS)
      {
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_INVAL;
        #else
          tm_return->status = E_INVAL;
        #endif
        break;
      }

      l1_config.tmode.tx_params.txpwr = temp_txpwr;
      tx_param_band = temp_band;

      // if in TX mode, change txpwr on the fly
      if ((l1_config.TestMode) &&
          (l1tm.tmode_state.dedicated_active) &&
          (l1_config.tmode.rf_params.down_up & TMODE_UPLINK))
      {
        // this causes 'direct' changing of TXPWR, which is OK in TestMode
        l1a_l1s_com.dedic_set.aset->new_target_txpwr = l1s.applied_txpwr = l1_config.tmode.tx_params.txpwr;
      }

    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    case TX_TIMING_ADVANCE:
    {
      l1_config.tmode.tx_params.timing_advance = prim->u.tm_params.value;

      if (l1_config.TestMode && l1tm.tmode_state.dedicated_active)
      {
        // direct changing of Timing Advance
        l1a_l1s_com.dedic_set.aset->new_timing_advance = l1_config.tmode.tx_params.timing_advance;
        // new TA to take effect immediately
        l1a_l1s_com.dedic_set.aset->timing_advance = l1a_l1s_com.dedic_set.aset->new_timing_advance;
      }
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    case TX_PWR_SKIP:
    {
      l1_config.tmode.tx_params.txpwr_skip = prim->u.tm_params.value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
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
        l1_config.tmode.tx_params.txpwr_gprs[prim->u.tm_params.index - TX_GPRS_POWER0] = prim->u.tm_params.value;
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_OK;
      #else
        tm_return->status = E_OK;
      #endif
        break;
      }
      case TX_GPRS_SLOTS:
      {
        l1_config.tmode.tx_params.timeslot_alloc = prim->u.tm_params.value;
      #if (ETM_PROTOCOL == 1)
        tm_return->status = -ETM_OK;
      #else
        tm_return->status = E_OK;
      #endif
        break;
      }
      case TX_GPRS_CODING:
      {
        UWORD8 coding_scheme;

        coding_scheme = prim->u.tm_params.value;
        if ((coding_scheme < 2) || (coding_scheme > 6) || (coding_scheme == 3))
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_INVAL;
          #else
            tm_return->status = E_INVAL;
          #endif
        else
        {
          l1_config.tmode.tx_params.coding_scheme = prim->u.tm_params.value;
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_OK;
        #else
          tm_return->status = E_OK;
        #endif
        }
        break;
      }
    #endif
    default:
    {
      Cust_tm_tx_param_write(tm_return,
                             prim->u.tm_params.index,
                             prim->u.tm_params.value,
                             tx_param_band);
      break;
    }
  } // end switch
}

void l1tm_tx_param_read(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  tm_return->index = prim->u.tm_params.index;

  Cust_tm_tx_param_read(tm_return,
                        prim->u.tm_params.index,
                        tx_param_band);
}

void l1tm_tx_template_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  Cust_tm_tx_template_write(tm_return,
                            prim->u.tm_table.index,
                            prim->str_len_in_bytes - 1, // subtract 8-bit index
                            prim->u.tm_table.table);
}

void l1tm_tx_template_read(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  Cust_tm_tx_template_read(tm_return, prim->u.tm_table.index);
}

void l1tm_special_param_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  Cust_tm_special_param_write(tm_return,
                              prim->u.tm_params.index,
                              prim->u.tm_params.value);
}

void l1tm_special_param_read(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  Cust_tm_special_param_read(tm_return, prim->u.tm_params.index);
}

void l1tm_special_table_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  Cust_tm_special_table_write(tm_return,
                              prim->u.tm_table.index,
                              prim->str_len_in_bytes - 1,
                              prim->u.tm_table.table);
}

void l1tm_special_table_read(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  Cust_tm_special_table_read(tm_return, prim->u.tm_table.index);
}

void l1tm_special_enable(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  Cust_tm_special_enable(tm_return, prim->u.tm_params.index);
}

/*-------------------------------------------------------*/
/* l1tm_initialize_var()                                 */
/*-------------------------------------------------------*/
/* Parameters  :                                         */
/* -------------                                         */
/* Return      :                                         */
/* -------------                                         */
/* Description :                                         */
/* -------------                                         */
/* This routine is used to switch to TestMode by re-     */
/* initializing the l1a, l1s and l1a_l1s_com global      */
/* structures. Re-initialization is kept at a minimum.   */
/*-------------------------------------------------------*/
void l1tm_initialize_var(void)
{
  UWORD32 i;
  UWORD8  task_id;


  // L1S tasks management...
  //-----------------------------------------
  for(task_id=0; task_id<NBR_DL_L1S_TASKS; task_id++)
  {
    l1s.task_status[task_id].new_status     = NOT_PENDING;
    l1s.task_status[task_id].current_status = INACTIVE;
  }
  l1s.frame_count = 0;
  l1s.forbid_meas = 0;

  // MFTAB management variables...
  //-----------------------------------------
  l1s.afrm = 0;
  l1s_clear_mftab(l1s.mftab.frmlst);

  // Flag registers for RF task controle...
  //-----------------------------------------
  l1s.tpu_ctrl_reg = 0;
  l1s.dsp_ctrl_reg = 0;

  //++++++++++++++++++++++++++++++++++++++++++
  //  Reset "l1a" structure.
  //++++++++++++++++++++++++++++++++++++++++++

  // Downlink tasks management...
  // Uplink tasks management...
  // Measurement tasks management...
  //-----------------------------------------
  for(i=0; i<NBR_L1A_PROCESSES; i++)
  {
    l1a.l1a_en_meas[i] = 0;
    l1a.state[i]       = 0; // RESET state.
  }

  // Flag for forward/delete message management.
  //---------------------------------------------
  l1a.l1_msg_forwarded = 0;


  //++++++++++++++++++++++++++++++++++++++++++
  //  Reset "l1a_l1s_com" structure.
  //++++++++++++++++++++++++++++++++++++++++++

  l1a_l1s_com.l1a_activity_flag      = TRUE;
  l1a_l1s_com.time_to_next_l1s_task  = 0;


  // sleep management configuration
  //===============================
  l1s.pw_mgr.mode_authorized  = NO_SLEEP;

  // L1S scheduler...
  //====================

  // L1S tasks management...
  //-----------------------------------------
  for(i=0; i<NBR_DL_L1S_TASKS; i++)
  {
    l1a_l1s_com.task_param[i]  = SEMAPHORE_RESET;
    l1a_l1s_com.l1s_en_task[i] = TASK_DISABLED;
  }

  // Measurement tasks management...
  //-----------------------------------------
  l1a_l1s_com.meas_param  = 0;
  l1a_l1s_com.l1s_en_meas = 0;

  #if L1_GPRS
    // Set DSP scheduler mode
    l1a_l1s_com.dsp_scheduler_mode = GSM_SCHEDULER;
    //  Packet measurement: Reset of the frequency list.
    //-------------------------------------------------
    l1pa_reset_cr_freq_list();
    // Initialize active list used in Neighbour Measurement Transfer Process
    l1pa_l1ps_com.cres_freq_list.alist = &(l1pa_l1ps_com.cres_freq_list.list[0]);

    l1pa_l1ps_com.transfer.semaphore = TRUE;
    l1pa_l1ps_com.transfer.aset      = &(l1pa_l1ps_com.transfer.set[0]);
    l1pa_l1ps_com.transfer.fset[0]   = &(l1pa_l1ps_com.transfer.set[1]);
    l1pa_l1ps_com.transfer.fset[1]   = &(l1pa_l1ps_com.transfer.set[2]);

    for(i=0;i<3;i++)
    {
      l1pa_l1ps_com.transfer.set[i].SignalCode                   = 0;
      l1pa_l1ps_com.transfer.set[i].dl_tbf_synchro_timeslot      = 0;
      l1pa_l1ps_com.transfer.set[i].dl_tbf_synchro_timeslot      = 0;
      l1pa_l1ps_com.transfer.set[i].transfer_synchro_timeslot    = 0;
      l1pa_l1ps_com.transfer.set[i].allocated_tbf                = NO_TBF;
      l1pa_l1ps_com.transfer.set[i].assignment_command           = NO_TBF;
      l1pa_l1ps_com.transfer.set[i].multislot_class              = 0;

      l1pa_l1ps_com.transfer.set[i].packet_ta.ta                 = 255;
      l1pa_l1ps_com.transfer.set[i].packet_ta.ta_index           = 255;
      l1pa_l1ps_com.transfer.set[i].packet_ta.ta_tn              = 255;

      l1pa_l1ps_com.transfer.set[i].tsc                          = 0;

      l1pa_l1ps_com.transfer.set[i].freq_param.chan_sel.h        = 0;
      l1pa_l1ps_com.transfer.set[i].freq_param.chan_sel.
                                 rf_channel.single_rf.radio_freq = 0;

      l1pa_l1ps_com.transfer.set[i].tbf_sti.present              = FALSE;

      l1pa_l1ps_com.transfer.set[i].mac_mode                     = 0;

      l1pa_l1ps_com.transfer.set[i].ul_tbf_alloc->tfi            = 255;
      l1pa_l1ps_com.transfer.set[i].dl_tbf_alloc.tfi             = 255;

      l1pa_l1ps_com.transfer.set[i].dl_pwr_ctl.p0                = 255;
      l1pa_l1ps_com.transfer.set[i].dl_pwr_ctl.bts_pwr_ctl_mode  = 0;
      l1pa_l1ps_com.transfer.set[i].dl_pwr_ctl.pr_mode           = 0;
    }
  #endif

  // Init global test mode variables
  l1tm.tmode_state.dedicated_active = 0;
  #if L1_GPRS
    l1tm.tmode_state.packet_transfer_active = FALSE;
  #endif

  // PRBS seed initialization with a random pattern
  l1tm.tmode_prbs.prbs1_seed = 0x5613;
}

void l1tm_rf_enable(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  unsigned SignalCode =0;
  unsigned size = 0;  //omaps00090550
  xSignalHeaderRec *msg;
  UWORD8 send_prim = FALSE;  // Flag to send TestMode primitive...
                             // Do not send primitive is the default; change it if necessary
  UWORD8 band;
  tm_return->index = 0;      // don't include index in header
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_OK;
#else
  tm_return->status = E_OK;
#endif

  // Function only valid in TEST mode
  if (l1_config.TestMode == 0)
  {
  #if (ETM_PROTOCOL == 1)
    tm_return->status = -ETM_L1TESTMODE;
  #else
    tm_return->status = E_TESTMODE;
  #endif
  }
  else
  {
    // Reset all statistics
    l1tm_reset_rx_stats();

    // Reset receive state counters, unless already in dedicated mode
    if (!l1tm.tmode_state.dedicated_active)
      l1tm_reset_rx_state();

    // Reset monitor task
    l1_config.tmode.rf_params.mon_report = 0;
    l1_config.tmode.rf_params.mon_tasks  = 0;

    switch (prim->u.tm_params.index)
    {
      // Stop all RX and TX operations
      case STOP_ALL:
      {
        SignalCode = TMODE_STOP_RX_TX;
        size = sizeof(T_TMODE_STOP_RX_TX);
        l1tm.tmode_state.dedicated_active = 0;
        #if (RF_FAM == 61)
          // Reset the APC back to Automatic Mode
          l1ddsp_apc_set_automatic_mode();
        #endif

        #if (RF_FAM == 35)
          pll_tuning.enable=0;
        #endif
        // Reset down_up flag only if not in continuous mode. If in continuous mode, down_up
        // will be reset after the proper TPU scenario is loaded.
        if (l1_config.tmode.rf_params.tmode_continuous != TM_CONTINUOUS)
	  l1_config.tmode.rf_params.down_up = 0;
        send_prim = TRUE;
        break;
      }
      // RX with or without network synchronization first
      case RX_TCH:
      {
        // if already in UL-only
        if (l1tm.tmode_state.dedicated_active &&
            l1_config.tmode.rf_params.down_up == TMODE_UPLINK)
        {
          // cannot start to RX while already TXing
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_AGAIN;
        #else
          tm_return->status = E_AGAIN;
        #endif
        }
        else
        {
          l1_config.tmode.rf_params.down_up = TMODE_DOWNLINK;
          SignalCode = TMODE_IMMED_ASSIGN_REQ;
          size = sizeof(T_TMODE_IMMED_ASSIGN_REQ);
          send_prim = TRUE;
        }
        break;
      }
      // TX NB's or AB's on TCH with or without network synch. first
      case TX_TCH:
      {
        // Normal burst TX
        if (l1_config.tmode.tx_params.burst_type == 0)
        {
          // if already in DL-only, add UL
          if (l1tm.tmode_state.dedicated_active &&
              l1_config.tmode.rf_params.down_up == TMODE_DOWNLINK)
          {
            l1_config.tmode.rf_params.down_up = (TMODE_DOWNLINK | TMODE_UPLINK);
          }
          else
          {
            l1_config.tmode.rf_params.down_up = TMODE_UPLINK;
            SignalCode = TMODE_IMMED_ASSIGN_REQ;
            size = sizeof(T_TMODE_IMMED_ASSIGN_REQ);
            send_prim = TRUE;
          }
        }
        // AB TX
        else if (l1_config.tmode.tx_params.burst_type == 1)
        {
          // cannot start RACH while already in dedicated mode
          if (l1tm.tmode_state.dedicated_active)
          {
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_AGAIN;
          #else
            tm_return->status = E_AGAIN;
          #endif
          }
          else
          {
            SignalCode = TMODE_RA_START;
            size = sizeof(TMODE_RA_START);
            send_prim = TRUE;
          }
        }
        break;
      }
      // RX & TX on TCH with or without network synch. first
      case RX_TX_TCH:
      {
        // if NB TX
        if (l1_config.tmode.tx_params.burst_type == 0)
        {
          // if already in DL-only, add UL
          if (l1tm.tmode_state.dedicated_active &&
              l1_config.tmode.rf_params.down_up == TMODE_DOWNLINK)
          {
            l1_config.tmode.rf_params.down_up = (TMODE_DOWNLINK | TMODE_UPLINK);
          }
          // else if already in UL-only
          else if (l1tm.tmode_state.dedicated_active &&
                   l1_config.tmode.rf_params.down_up == TMODE_UPLINK)
          {
            // cannot start to RX while already TXing
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_AGAIN;
          #else
            tm_return->status = E_AGAIN;
          #endif
          }
          else
          {
            l1_config.tmode.rf_params.down_up = (TMODE_DOWNLINK | TMODE_UPLINK);
            SignalCode = TMODE_IMMED_ASSIGN_REQ;
            size = sizeof(T_TMODE_IMMED_ASSIGN_REQ);
            send_prim = TRUE;
          }
        }
        // else if AB TX
        else if (l1_config.tmode.tx_params.burst_type == 1)
        {
          // Cannot TX RACH and RX simultaneously
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_AGAIN;
        #else
          tm_return->status = E_AGAIN;
        #endif
        }
        break;
      }
      // Continuous (all timeslots) reception on TCH
      case RX_TCH_CONT:
      {
        // if already in UL, DL or UL+DL
        if (l1tm.tmode_state.dedicated_active &&
            l1_config.tmode.rf_params.down_up != 0)
        {
          // cannot start to continously RX while already TXing or RXing
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_AGAIN;
        #else
          tm_return->status = E_AGAIN;
        #endif
        }
        else
        {
          l1_config.tmode.rf_params.tmode_continuous = TM_START_RX_CONTINUOUS;
          l1_config.tmode.rf_params.down_up = TMODE_DOWNLINK;
          SignalCode = TMODE_IMMED_ASSIGN_REQ;
          size = sizeof(T_TMODE_IMMED_ASSIGN_REQ);
          send_prim = TRUE;
        }
        break;
      }
      // continuous (all timeslots) transmission
      case TX_TCH_CONT:
      {
        // PCS 1900 not supported yet.
#if (L1_FF_MULTIBAND == 0)       
        band = ((l1_config.tmode.rf_params.tch_arfcn >= 512) &&
                (l1_config.tmode.rf_params.tch_arfcn <= 885));
#else
        band = ( ((l1_config.tmode.rf_params.tch_arfcn >= 512) &&
                (l1_config.tmode.rf_params.tch_arfcn <= 885)) || 
                ((l1_config.tmode.rf_params.tch_arfcn >= 1024) &&
                (l1_config.tmode.rf_params.tch_arfcn <= 1322)) );
#endif        

        // if already in UL, DL or UL+DL
        if ((l1tm.tmode_state.dedicated_active && l1_config.tmode.rf_params.down_up != 0) ||
            (band == 0 && (l1_config.tmode.tx_params.txpwr < (5 + l1_config.tmode.tx_params.txpwr_skip)))  ||
            (band == 1 && (l1_config.tmode.tx_params.txpwr < (0 + l1_config.tmode.tx_params.txpwr_skip))))
        {
          // cannot start to continously TX while already TXing or RXing
          // or while adc reading are enabled
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_INVAL;
          #else
            tm_return->status = E_INVAL;
          #endif
        }
        else
        {
          #if (RF_FAM == 61)
            // Set APC in Manual Mode
            l1ddsp_apc_set_manual_mode();
          #endif
          l1_config.tmode.rf_params.tmode_continuous = TM_START_TX_CONTINUOUS;
          l1_config.tmode.rf_params.down_up = TMODE_UPLINK;
          SignalCode = TMODE_IMMED_ASSIGN_REQ;
          size = sizeof(T_TMODE_IMMED_ASSIGN_REQ);
          send_prim = TRUE;
         }
        break;
      }
      // Continuous BCCH
      case BCCH_LOOP:
      {
        SignalCode = TMODE_SCELL_NBCCH_REQ;
        size = sizeof(TMODE_SCELL_NBCCH_REQ);
        send_prim = TRUE;
        break;
      }
      // Continuous SB
      case SB_LOOP:
      {
        SignalCode = TMODE_SB_REQ;
        size = sizeof(T_TMODE_SB_REQ);
        send_prim = TRUE;
        break;
      }
      // Continuous FB1
      case FB1_LOOP:
      {
        SignalCode = TMODE_FB1_REQ;
        size = sizeof(T_TMODE_FB1_REQ);
        send_prim = TRUE;
        break;
      }
      // Continuous FB0
      case FB0_LOOP:
      {
        SignalCode = TMODE_FB0_REQ;
        size = sizeof(T_TMODE_FB0_REQ);
        send_prim = TRUE;
        break;
      }
      // TX + RX + MON on TCH
      case RX_TX_MON_TCH: // Stats collected from TCH Channel.
      case RX_TX_MON:     // Stats collected from MON Channel (except rxlev).
      {
        // Normal burst uplink
        if (l1_config.tmode.tx_params.burst_type == 0)
        {
          // If already in dedicated mode, return error
          if (l1tm.tmode_state.dedicated_active)
          {
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_AGAIN;
          #else
            tm_return->status = E_AGAIN;
          #endif
          }
          else
          {
            l1_config.tmode.rf_params.down_up = (TMODE_DOWNLINK | TMODE_UPLINK);
            l1_config.tmode.rf_params.mon_report = ((prim->u.tm_params.index & 0x08) >> 3);
            l1_config.tmode.rf_params.mon_tasks = 1;  // enable MON tasks
            SignalCode = TMODE_IMMED_ASSIGN_REQ;
            size = sizeof(T_TMODE_IMMED_ASSIGN_REQ);
            send_prim = TRUE;
          }
        }
        // else if Access burst uplink
        else if (l1_config.tmode.tx_params.burst_type == 1)
        {
          // Cannot TX RACH and RX simultaneously
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_AGAIN;
        #else
          tm_return->status = E_AGAIN;
        #endif
        }
        break;
      }
      case SINGLE_PM:
      {
        SignalCode = TMODE_RXLEV_REQ;
        size = sizeof(T_TMODE_RXLEV_REQ);
        send_prim = TRUE;
        break;
      }
      #if L1_GPRS
        // RX & TX on PDTCH with or without network synch. first
        case RX_TX_PDTCH:
        {
          UWORD8 bit_map = 0x80;

          // set uplink + downlink
          if (l1_config.tmode.tx_params.timeslot_alloc)
            l1_config.tmode.rf_params.down_up = TMODE_DOWNLINK | TMODE_UPLINK;
          else
            l1_config.tmode.rf_params.down_up = TMODE_DOWNLINK;

          while (bit_map)
          {
            if (bit_map & l1_config.tmode.stats_config.stat_gprs_slots)
              l1tm.tmode_stats.nb_dl_pdtch_slots ++;

            bit_map>>=1;
          }

          SignalCode = TMODE_PDTCH_ASSIGN_REQ;
          size = sizeof(T_TMODE_PDTCH_ASSIGN_REQ);
          send_prim = TRUE;
          break;
        }
      #endif
      #if L1_GPRS
        // RX & TX on PDTCH, FB on monitor arfcn
        case RX_TX_PDTCH_MON:
        {
          // set uplink + downlink
          if (l1_config.tmode.tx_params.timeslot_alloc)
            l1_config.tmode.rf_params.down_up = TMODE_DOWNLINK | TMODE_UPLINK;
          else
            l1_config.tmode.rf_params.down_up = TMODE_DOWNLINK;

          l1_config.tmode.rf_params.mon_report = 1;  // collect stats from MON channel
          l1_config.tmode.rf_params.mon_tasks  = 1;  // enable MON tasks

          SignalCode = TMODE_PDTCH_ASSIGN_REQ;
          size = sizeof(T_TMODE_PDTCH_ASSIGN_REQ);
          send_prim = TRUE;
          break;
        }
      #endif
      #if (RF_FAM == 35)
      case RX_PLL_TUNING:
      {
        // if already in UL-only
        if (l1tm.tmode_state.dedicated_active &&
            l1_config.tmode.rf_params.down_up == TMODE_UPLINK)
        {
          // cannot start to RX while already TXing
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_AGAIN;
        #else
          tm_return->status = E_AGAIN;
        #endif
        }
        else
        {
          pll_tuning.data[5]   = 0;
          pll_tuning.index     = 0;

          pll_tuning.enable    = 1;

          l1_config.tmode.rf_params.down_up = TMODE_DOWNLINK;
          SignalCode = TMODE_IMMED_ASSIGN_REQ;
          size = sizeof(T_TMODE_IMMED_ASSIGN_REQ);
          send_prim = TRUE;
        }
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
        break;
      }
    } //end switch
  } // end of else

  if (send_prim == TRUE)
  {
    // Allocate result message.
    msg = os_alloc_sig(size);
    DEBUGMSG(status,NU_ALLOC_ERR)
    msg->SignalCode = SignalCode;
    os_send_sig(msg, L1C1_QUEUE);
    DEBUGMSG(status,NU_SEND_QUEUE_ERR)
  }

  // always return a 0 in the result[], even if error
  tm_return->result[0] = 0;
  tm_return->size =  1;
}


void l1tm_stats_config_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  tm_return->index = prim->u.tm_params.index;
  tm_return->size = 0;

  switch (prim->u.tm_params.index)
  {
    case LOOPS:
    {
      l1_config.tmode.stats_config.num_loops = prim->u.tm_params.value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    case AUTO_RESULT_LOOPS:
    {
      l1_config.tmode.stats_config.auto_result_loops = prim->u.tm_params.value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    case AUTO_RESET_LOOPS:
    {
      l1_config.tmode.stats_config.auto_reset_loops = prim->u.tm_params.value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    case STAT_TYPE:
    {
      l1_config.tmode.stats_config.stat_type = prim->u.tm_params.value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    case STAT_BITMASK:
    {
      l1_config.tmode.stats_config.stat_bitmask = prim->u.tm_params.value;
    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    #if L1_GPRS
      case STAT_GPRS_SLOTS:
      {
        UWORD8 allocation, value;

        value = prim->u.tm_params.value;

        // Check for mismatch between DL TS allocation and stats bitmap
        allocation = value ^ l1_config.tmode.rx_params.timeslot_alloc;

        if (value & allocation)
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_INVAL;
          #else
            tm_return->status = E_INVAL;
          #endif
        else
        {
          l1_config.tmode.stats_config.stat_gprs_slots = value;
        #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_OK;
        #else
          tm_return->status = E_OK;
        #endif
        }
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
      break;
    }
  }  // end switch
}

void l1tm_stats_config_read(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
  volatile UWORD16 value;

  tm_return->index = prim->u.tm_params.index;

  switch (prim->u.tm_params.index)
  {
    case LOOPS:
    {
      value = l1_config.tmode.stats_config.num_loops;
      break;
    }
    case AUTO_RESULT_LOOPS:
    {
      value = l1_config.tmode.stats_config.auto_result_loops;
      break;
    }
    case AUTO_RESET_LOOPS:
    {
      value = l1_config.tmode.stats_config.auto_reset_loops;
      break;
    }
    case STAT_TYPE:
    {
      value = l1_config.tmode.stats_config.stat_type;
      break;
    }
    case STAT_BITMASK:
    {
      value = l1_config.tmode.stats_config.stat_bitmask;
      break;
    }
    #if L1_GPRS
      case STAT_GPRS_SLOTS:
      {
        value = l1_config.tmode.stats_config.stat_gprs_slots;
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
  }  // end switch

  memcpy(tm_return->result, (UWORD8 *) &value, 2);
  tm_return->size = 2;
#if (ETM_PROTOCOL == 1)
  tm_return->status = -ETM_OK;
#else
  tm_return->status = E_OK;
#endif
}

void l1tm_statistics(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
{
    l1tm_stats_read(tm_return,
                    prim->u.tm_params.index,
                    prim->u.tm_params.value);
}

#if L1_GPRS
void l1tm_rlc_uplink(UWORD8 tx, API *ul_data)
{
  // Cast the ul_data_buffer
  typedef struct
  {
    API a_ul_data[4][29];
  }
  T_A_UL_DATA;

  T_A_UL_DATA *ptr = (T_A_UL_DATA*) ul_data;
  UWORD8 i,j;

  for (j=0; j<tx; j++)
  {
    ptr->a_ul_data[j][0] = l1_config.tmode.tx_params.coding_scheme;

    for (i=0;i<l1_config.tmode.tx_params.rlc_buffer_size;i++)
      ptr->a_ul_data[j][i+1] = l1_config.tmode.tx_params.rlc_buffer[i];
  }
}
#endif

void l1tm_stats_read(T_TM_RETURN *tm_return, WORD16 type, UWORD16 bitmask)
{
  extern T_L1A_L1S_COM l1a_l1s_com;
  extern T_L1S_GLOBAL l1s;
  volatile UWORD32 utemp = bitmask, temp_U32;
  volatile WORD32  temp = type;
  volatile WORD32  value_signed_int;
  volatile UWORD32 value_unsigned_int;
  volatile UWORD16 value_unsigned_short;
  UWORD8  j, offset=0;  // offset is index of tm_return->result[]
  UWORD16 rssi, len;
  WORD32  count;
  WORD32  runs = l1tm.tmode_stats.loop_count;

  #if L1_GPRS
    volatile UWORD16 value_array_unsigned_short[4];
  #endif

   // Put type and bitmask in the front of tm_return->result[].
  // Use volatile vars for proper operation of memcpy().
  memcpy(&tm_return->result[offset], (UWORD8 *) &temp, 2);
  offset+=2;
  memcpy(&tm_return->result[offset], (UWORD8 *) &utemp, 2);
  offset+=2;

  switch (type)
  {
    // Accumulated receive burst stats
    case ACCUMULATED_RX_STATS:
    {
      // all stats saved when collected from TCH
      if (l1tm.tmode_state.dedicated_active && (l1_config.tmode.rf_params.mon_report == 0))
        count = l1tm.tmode_stats.loop_count;
      #if L1_GPRS
      else if (l1tm.tmode_state.packet_transfer_active && (l1_config.tmode.rf_params.mon_report == 0))
      {
        // loop_count contains the number of blocks
        // Stats (PM, TOA, SNR, ANGLE) are accumulated over all frames and all time slots
        count = l1tm.tmode_stats.loop_count * l1tm.tmode_stats.nb_dl_pdtch_slots * 4;

        // the count of runs vs. successes is accumulated over all time slots per block
        runs  = l1tm.tmode_stats.loop_count * l1tm.tmode_stats.nb_dl_pdtch_slots;
      }
      #endif
      else count = l1tm.tmode_stats.flag_count; // only PASS stats saved

      if (bitmask & RSSI) // rxlev: RSSI SF12.4 eventually (currently F7.1)
      {
        len = sizeof(l1tm.tmode_stats.rssi_fifo) / sizeof(l1tm.tmode_stats.rssi_fifo[0]);
        rssi = 0;
        for (j=0; j<len; j++)
          rssi += l1tm.tmode_stats.rssi_fifo[j];
        rssi /= len; // F7.1
        memcpy(&tm_return->result[offset], (UWORD8 *) &rssi, 2);
        offset+=2;
      }
      // pm: DSP MEAN power measurement UF10.6
      if (bitmask & DSP_PM)
      {
        if (count)
          value_unsigned_short = (UWORD16)(l1tm.tmode_stats.pm_sum) / count; //omaps00090550
        else value_unsigned_short = 0;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_short, 2);
        offset+=2;
      }
      // angle mean
      if (bitmask & ANGLE_MEAN)
      {
        if (count) // non-zero
          value_signed_int = (UWORD16)(l1tm.tmode_stats.angle_sum) / count; //omaps00090550
        else value_signed_int = 0;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_signed_int, 4);
        offset+=4;
      }
      // angle variance
      if (bitmask & ANGLE_VAR)
      {
        // VAR[X] = E[X^2] - (E[X])^2
        if (count) // non-zero
        {
	       UWORD32 u32dvsor ;
		   temp_U32 = l1tm.tmode_stats.angle_sum;
           u32dvsor = (temp_U32 /(UWORD32)count);
		   temp_U32= u32dvsor;
		u32dvsor *= (temp_U32);
		u32dvsor *= (temp_U32);
           value_unsigned_int = l1tm.tmode_stats.angle_sq_sum /  (UWORD32)count - u32dvsor;
        //  temp_U32 = (UWORD16)(l1tm.tmode_stats.angle_sum) / (count); //omaps00090550
        //  u32dvsor = count -((temp_U32) * (temp_U32 )) ;

        //  value_unsigned_int = (UWORD16)l1tm.tmode_stats.angle_sq_sum / u32dvsor ; //omaps00090550

		/*
          temp_U32 = l1tm.tmode_stats.angle_sum / count;
          value_unsigned_int = l1tm.tmode_stats.angle_sq_sum / count - (temp_U32)*(temp_U32);
		  */




        }
        else value_unsigned_int = 0;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_int, 4);
        offset+=4;
      }
      // angle minimum
      if (bitmask & ANGLE_MIN)
      {
        value_signed_int = l1tm.tmode_stats.angle_min;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_signed_int, 4);
        offset+=4;
      }
      // angle maximum
      if (bitmask & ANGLE_MAX)
      {
        value_signed_int = l1tm.tmode_stats.angle_max;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_signed_int, 4);
        offset+=4;
      }
      // SNR mean
      if (bitmask & SNR_MEAN)
      {
        if (count) // non-zero
          value_unsigned_int = (UWORD16)(l1tm.tmode_stats.snr_sum) / count;  //omaps00090550
        else value_unsigned_int = 0;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_int, 4);
        offset+=4;
      }
      // SNR variance
      if (bitmask & SNR_VAR)
      {
        if (count) // non-zero
        {
          UWORD32 u32dvsor ;
		   temp_U32 = l1tm.tmode_stats.angle_sum;
           u32dvsor = (temp_U32 /(UWORD32)count);
		   temp_U32= u32dvsor;
		u32dvsor *= (temp_U32);
		u32dvsor *= (temp_U32);
           value_unsigned_int = l1tm.tmode_stats.angle_sq_sum /  (UWORD32)count - u32dvsor;

        }
        else
        value_unsigned_int = 0;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_int, 4);
        offset+=4;
      }
      // TOA mean
      if (bitmask & TOA_MEAN)
      {
        if (count) // non-zero
          value_unsigned_int = (UWORD16)(l1tm.tmode_stats.toa_sum) / count;   //omaps00090550
        else value_unsigned_int = 0;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_int, 4);
        offset+=4;
      }
      // TOA variance
      if (bitmask & TOA_VAR)
      {
        if (count) // non-zero
        {
          UWORD32 u32dvsor ;
		   temp_U32 = l1tm.tmode_stats.angle_sum;
           u32dvsor = (temp_U32 /(UWORD32)count);
		   temp_U32= u32dvsor;
		u32dvsor *= (temp_U32);
		u32dvsor *= (temp_U32);
           value_unsigned_int = l1tm.tmode_stats.angle_sq_sum /  (UWORD32)count - u32dvsor;

        }
        else value_unsigned_int = 0;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_int, 4);
        offset+=4;
      }
      // Frame # mod 26*51
      if (bitmask & FRAME_NUMBER)
      {
        value_unsigned_int = l1s.actual_time.fn;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_int, 4);
        offset+=4;
      }
      // Tot # of runs executed so far
      if (bitmask & RUNS)
      {
        memcpy(&tm_return->result[offset], (UWORD8 *) &runs, 4);
        offset+=4;
      }
      // Tot # of successes so far
      if (bitmask & SUCCESSES)
      {
        memcpy(&tm_return->result[offset], (UWORD8 *) &l1tm.tmode_stats.flag_count, 4);
        offset+=4;
      }
      // BSIC
      if (bitmask & BSIC)
      {
        value_unsigned_short = l1tm.tmode_stats.bsic;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_short, 2);
        offset+=2;
      }

#if L1_GPRS
    if (bitmask & BLER)
        {
         UWORD8 j;
          if (count) // non-zero
          {
            float bler, remain_part;
            UWORD8  int_part, bitmap_remain;
            int i;

            //compute bler for each block
            for (j=0; j<4; j++)
            {
              bler = ((float) (l1tm.tmode_stats.bler_crc[j] * 100)) / ((float) l1tm.tmode_stats.bler_total_blocks);

              //conversion from floating to fixed format
              int_part = (UWORD8) bler;
              remain_part = bler - (float) int_part;
              bitmap_remain = 0;

              i=5;
              while (i >= 0)
              {
                bitmap_remain |= (UWORD8) (remain_part *2) << i;
                if (((UWORD8) (remain_part *2)) >= 1)
                  remain_part = (remain_part * 2) - 1;
                else
                  remain_part = (remain_part * 2);
                i--;
              }

              // Reporting the percentage of blocks in error (F10.6)
              value_array_unsigned_short[j] = bitmap_remain | (int_part << 6);
              }
          }
          // Reporting a BLER of 100, if no computation has been done
          else
          {
            for (j=0; j<4; j++)
              value_array_unsigned_short[j] = 100 << 6;
          }
          memcpy(&tm_return->result[offset], (UWORD8 *) &value_array_unsigned_short[0], 8);
          offset+=8;
    	}
#endif


    #if (ETM_PROTOCOL == 1)
      tm_return->status = -ETM_OK;
    #else
      tm_return->status = E_OK;
    #endif
      break;
    }
    // Most recent receive burst stats
    case MOST_RECENT_RX_STATS:
    {
      // rxlev: RSSI SF12.4 eventually (F7.1 currently)
      if (bitmask & RSSI)
      {
        value_unsigned_short = l1tm.tmode_stats.rssi_recent;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_short, 2);
        offset+=2;
      }
      // pm: most recent DSP power measurement UF10.6
      if (bitmask & DSP_PM)
      {
        memcpy(&tm_return->result[offset], &l1tm.tmode_stats.pm_recent, 2);
        offset+=2;
      }
      // most recent ANGLE value
      if (bitmask & ANGLE_MEAN)
      {
        value_signed_int = l1tm.tmode_stats.angle_recent;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_signed_int, 4);
        offset+=4;
      }
      // doesn't make sense.
      if (bitmask & ANGLE_VAR)
      {
      }
      // doesn't make sense.
      if (bitmask & ANGLE_MIN)
      {
      }
      // doesn't make sense.
      if (bitmask & ANGLE_MAX)
      {
      }
      // most recent SNR value
      if (bitmask & SNR_MEAN)
      {
        value_unsigned_int = l1tm.tmode_stats.snr_recent;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_int, 4);
        offset+=4;
      }
      // doesn't make sense.
      if (bitmask & SNR_VAR)
      {
      }
      // most recent TOA value
      if (bitmask & TOA_MEAN)
      {
        value_unsigned_int = l1tm.tmode_stats.toa_recent;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_int, 4);
        offset+=4;
      }
      // doesn't make sense.
      if (bitmask & TOA_VAR)
      {
      }
      // Frame # mod 26*51
      if (bitmask & FRAME_NUMBER)
      {
        value_unsigned_int = l1s.actual_time.fn;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_int, 4);
        offset+=4;
      }
      // must be '1'
      if (bitmask & RUNS)
      {
      }
      // most recent Success flag
      if (bitmask & SUCCESSES)
      {
        memcpy(&tm_return->result[offset], (UWORD8 *)&l1tm.tmode_stats.flag_recent, 4);
        offset+=4;
      }
      // BSIC
      if (bitmask & BSIC)
      {
        value_unsigned_short = l1tm.tmode_stats.bsic;
        memcpy(&tm_return->result[offset], (UWORD8 *) &value_unsigned_short, 2);
        offset+=2;
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
  }  // end switch

  tm_return->size = offset;
  tm_return->index = 0;  // don't include index in header
}

/*-------------------------------------------------------*/
/* l1tm_fill_burst()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:  Prepare bursts for transmission in case */
/* ------------  of UL test                              */
/*                                                       */
/* Simulation of IQ Swap does the following mapping:     */
/*                                                       */
/*             00 -> 01                                  */
/*             01 -> 00                                  */
/*             10 -> 11                                  */
/*             11 -> 10                                  */
/*                                                       */
/*-------------------------------------------------------*/
#if (L1_FF_MULTIBAND == 1)
extern const WORD8 rf_subband2band[];
#endif 
void l1tm_fill_burst (UWORD16 pattern, UWORD16 *TM_ul_data)
{
  UWORD32 i;
  UWORD8  swap_iq, swap_flag;
  UWORD16 gb_front, gb_end, tb_front, tb_end, even_bits, odd_bits;
#if (L1_FF_MULTIBAND == 1) 
  UWORD8 physical_band_id;
#endif  
  UWORD8 tsc_bits_in_first_word;
  UWORD16 tsc_front_mask,tsc_end_mask;
  extern  T_RF rf;

  // training sequences list......
  UWORD32 tsc[8]=
  {
    0x00970897,
    0x00B778B7,
    0x010EE90E,
    0x011ED11E,
    0x006B906B,
    0x013AC13A,
    0x029F629F,
    0x03BC4BBC
  };
#if (L1_FF_MULTIBAND == 0)

  if(((l1_config.std.id == DUAL) || (l1_config.std.id == DUALEXT) || (l1_config.std.id == DUAL_US)) &&
      (l1_config.tmode.rf_params.tch_arfcn >= l1_config.std.first_radio_freq_band2))
  {
    swap_iq = l1_config.std.swap_iq_band2;
  }
  else
  {
    swap_iq = l1_config.std.swap_iq_band1;
  }
  
#else // L1_FF_MULTIBAND = 1 below

  physical_band_id = 
      rf_subband2band[rf_convert_rffreq_to_l1subband(l1_config.tmode.rf_params.tch_arfcn)];

  swap_iq = rf_band[physical_band_id].swap_iq;

#endif // #if (L1_FF_MULTIBAND == 0) else

  // Swap IQ definitions...
  // 0=No Swap, 1=Swap RX only, 2=Swap TX only, 3=Swap RX and TX
  if (swap_iq & 0x2)
  {
    swap_flag = 1;
  }
  else
  {
    swap_flag = 0;
  }

  //===========================================
  // define uplink patterns
  //===========================================
  if (pattern == 0)       //  0's
    pattern = 0x0000;
  else if (pattern == 1)  //  1's
    pattern = 0xffff;
  else if (pattern == 2)  // 01's
    pattern = 0x5555;

  // first replicate pattern through all buffer
  if ((pattern == 3) || (pattern == 4))
  {
    // fill the uplink burst with PRBS1
    l1tm_PRBS1_generate(TM_ul_data);
  }
  else if ((pattern != 12) && (pattern != 13))
  {
    for (i=0;i<=15;i++)
      TM_ul_data[i] = (pattern << 6);
  }

  //===========================================
  // create front-end guard and tail bits masks
  //===========================================
  // guard bits mask
  gb_front = 0xFFC0 << (10 - rf.tx.guard_bits);  // max. of 10, min. of 2 guard bits allowed

  // check if guard bits > 7
  if (rf.tx.guard_bits > 7)
  {
    // tail bits mask
    tb_front = ~((UWORD16)(0xE000 << (10 - rf.tx.guard_bits))) & 0xFFC0;  // tail bits placed in TM_ul_data[1]  //oamps00090550
    // add tail bits to uplink data
    TM_ul_data[1] = TM_ul_data[1] & tb_front;
    // add guard bits to uplink data
    TM_ul_data[0] = gb_front;
  }
  else
  {
    // tail bits mask
    tb_front = ~((UWORD16)(0xE000 >> rf.tx.guard_bits) )& 0xFFC0;  // 3 tail bits
    // add tail bits to uplink data
    TM_ul_data[0] = (TM_ul_data[0] | gb_front) & tb_front;
  }

  //===========================================
  // create back-end guard and tail bits masks
  //===========================================
  // guard bits mask
  gb_end = (0xFFC0 >> (10 - (12 - rf.tx.guard_bits))) & 0xFFC0;  // max. of 10, min. of 2 guard bits allowed

  // check if guard bits < 5
  if (rf.tx.guard_bits < 5)
  {
    //tail bits mask
    tb_end = ~(UWORD16)((0x01C0 >> (rf.tx.guard_bits - 2))) & 0xFFC0;  // tail bits placed in TM_ul_data[14]
    // add tail bits to uplink data
    TM_ul_data[14] = TM_ul_data[14] & tb_end;
    // add guard bits to uplink data
    TM_ul_data[15] = gb_end;
  }
  else
  {
    // tail bits mask
    tb_end = ~(UWORD16)((0x01C0 << (12 - rf.tx.guard_bits))) & 0xFFC0;  // 3 tail bits
    // add tail bits to uplink data
    TM_ul_data[15] = (TM_ul_data[15] | gb_end) & tb_end;
  }

  //===========================================
  // Insert the training sequence pattern .The location of TSC bits will
  // vary according to the value of guard bits used.
  //===========================================
  if ((pattern == 13)||(pattern==3))
  {
    //   TM_ul_data[6]  = (TM_ul_data[6]  & 0xFE00) | ( (UWORD8) ((tsc[l1_config.tmode.tx_params.tsc]>>24) << 6 ) );   // tsc bits 1-2
    //    TM_ul_data[7]  = (TM_ul_data[7]  & 0x0000) | ( (UWORD8) ((tsc[l1_config.tmode.tx_params.tsc]>>14) << 6 ) );   // tsc bits 3-12
    //   TM_ul_data[8]  = (TM_ul_data[8]  & 0x0000) | ( (UWORD8) ((tsc[l1_config.tmode.tx_params.tsc]>>4 ) << 6 ) );   // tsc bits 13-22
    //   TM_ul_data[9]  = (TM_ul_data[9]  & 0x07C0) | ( (UWORD8) ((tsc[l1_config.tmode.tx_params.tsc]>>0 ) << 12) );   // tsc bits 23-26

    if (rf.tx.guard_bits <4) // TSC will be in [6],[7],[8]
    {
      tsc_bits_in_first_word = 9-rf.tx.guard_bits; // 7 bits when guard is 2, 6 bit when guard is 3
      tsc_front_mask = ((0xFFC0) << tsc_bits_in_first_word); // insert zeros from right
      //tsc_bits_in_last_word = 26 -10 -tsc_bits_in_first_word = (16-tsc_bits_in_first_word)
      tsc_end_mask =  (((0xFFC0) >> (16-tsc_bits_in_first_word)) & 0xFFC0);   //insert zeros from left

      TM_ul_data[6]  = (TM_ul_data[6]  & tsc_front_mask)
                       | ( ((UWORD16) ((tsc[l1_config.tmode.tx_params.tsc]>>(26-tsc_bits_in_first_word)) << 6 )) & (~tsc_front_mask) );
      TM_ul_data[7]  = (TM_ul_data[7]  & 0x0000)
                       | ( ((UWORD16) ((tsc[l1_config.tmode.tx_params.tsc]<<(tsc_bits_in_first_word+6))>>16)) & (0xFFC0));   //next 10 bits of TSC
      TM_ul_data[8]  = (TM_ul_data[8]  & tsc_end_mask)
                       | ( ((UWORD16) ((tsc[l1_config.tmode.tx_params.tsc] << (tsc_bits_in_first_word+6+10))>>16) )& (~tsc_end_mask));
    }
    else if ((rf.tx.guard_bits >=4) && (rf.tx.guard_bits <9) )// TSC will be in [6],[7],[8],[9]
    {
      tsc_bits_in_first_word = 9-rf.tx.guard_bits; // 5 bits when guard is 4, 1 bit when guard is 8
      tsc_front_mask = ((0xFFC0) << tsc_bits_in_first_word); // insert zeros from right
      //tsc_bits_in_last_word = 26 -10 -10 -tsc_bits_in_first_word = (6-tsc_bits_in_first_word)
      tsc_end_mask =  (((0xFFC0) >> (6-tsc_bits_in_first_word)) & 0xFFC0);   //insert zeros from left

      TM_ul_data[6]  = (TM_ul_data[6]  & tsc_front_mask)
                       | ( ((UWORD16) ((tsc[l1_config.tmode.tx_params.tsc]>>(26-tsc_bits_in_first_word)) << 6 )) & (~tsc_front_mask) );
      TM_ul_data[7]  = (TM_ul_data[7]  & 0x0000)
                       | ( ((UWORD16) ((tsc[l1_config.tmode.tx_params.tsc]<<(tsc_bits_in_first_word+6))>>16)) & (0xFFC0)  );   //next 10 bits of TSC
      TM_ul_data[8]  = (TM_ul_data[8]  & 0x0000)
                       | ( ((UWORD16) ((tsc[l1_config.tmode.tx_params.tsc]<<(tsc_bits_in_first_word+6+10))>>16)) & (0xFFC0) );   //next 10 bits of TSC
      TM_ul_data[9]  = (TM_ul_data[9]  & tsc_end_mask)
                       | ( ((UWORD16) ((tsc[l1_config.tmode.tx_params.tsc] << (tsc_bits_in_first_word+6+10+10))>>16) ) & (~tsc_end_mask));
    }
    else   //(rf.tx.guard_bits>=9) : TSC will be in [7],[8],[9],
    {
      tsc_bits_in_first_word = 19-rf.tx.guard_bits; // 10 bits when guard is 9, 9 bits when guard is 10
      tsc_front_mask = ((0xFFC0) << tsc_bits_in_first_word); // insert zeros from right
      //tsc_bits_in_last_word = 26 -10 -tsc_bits_in_first_word = (16-tsc_bits_in_first_word)
      tsc_end_mask =  (((0xFFC0) >> (16-tsc_bits_in_first_word)) & 0xFFC0);   //insert zeros from left

      TM_ul_data[7]  = (TM_ul_data[7]  & tsc_front_mask)
                       | ( ((UWORD16) ((tsc[l1_config.tmode.tx_params.tsc]>>(26-tsc_bits_in_first_word)) << 6 )) & (~tsc_front_mask) );
      TM_ul_data[8]  = (TM_ul_data[8]  & 0x0000)
                       | ( ((UWORD16) ((tsc[l1_config.tmode.tx_params.tsc]<<(tsc_bits_in_first_word+6))>>16)) & (0xFFC0)  );   //next 10 bits of TSC
      TM_ul_data[9]  = (TM_ul_data[9]  & tsc_end_mask)
                       | ( ((UWORD16) ((tsc[l1_config.tmode.tx_params.tsc] << (tsc_bits_in_first_word+6+10))>>16)) & (~tsc_end_mask));
    }
  }

  // swap uplink data if IQ swap
  if(swap_flag)
  {
    for (i=0;i<=15;i++)
    {
      even_bits = TM_ul_data[i] & 0xAA80;  // keep bits in even positions
      odd_bits  = ~(TM_ul_data[i]) & 0x5540;  // keep and complement bits in odd positions
      TM_ul_data[i] = even_bits | odd_bits;  // swapped uplink data
    }
  }
}



void l1a_tmode_send_ul_msg(T_TM_RETURN *tm_ret)
{
  tm_transmit(tm_ret);
}

/*******************************************************************************
 *
 *                             void tm_receive(void *inbuf, int size)
 *
 * Purpose  : Parses TestMode data and copies it directly into TESTMODE_PRIM.
 *            It forwards primitive to L1, except in the case of tm_init() which
 *            gets executed in the CST.
 *
 * Arguments: In : command
 *            Out:
 *
 * Returns  : void
 *
 ******************************************************************************/

void tm_receive(UWORD8 *inbuf, UWORD16 size)
{
  UWORD8 cksum, cid, error = 0;
  BOOL msg_used=FALSE;

  #if (ETM_PROTOCOL == 1)
    UWORD8 mid = 0;
  #endif

  UWORD8 *pmsg;
  xSignalHeaderRec *msg;

  msg = os_alloc_sig(sizeof(T_TESTMODE_PRIM));
  msg->SignalCode = TESTMODE_PRIM;

  // pmsg will be used to fill up the TestMode primitive with th
  // data, in consecutive order according to the definition of T_TESTMODE_PRIM.
  pmsg = (UWORD8 *)((T_TESTMODE_PRIM *)(msg->SigP));

#if (ETM_PROTOCOL == 1) // Use of ETM protocol
  #if (OP_L1_STANDALONE == 1)
    // Check MID
    *pmsg++ = mid = *inbuf++;
    if ((mid != ETM_RF) && (mid != ETM_CORE))    // check if coming from ETM RF or ETM CORE DLL
      error = -ETM_PACKET;
  #elif (OP_L1_STANDALONE == 0)
    *pmsg++ = mid = ETM_RF;
  #endif

  // Copy CID/FID
  *pmsg++ = cid = *inbuf++;
  #if (OP_L1_STANDALONE == 1)
    // Copy data payload size (size minus MID byte and checksum byte)
    size -= 2;
  #endif
  *pmsg++ = --size; // Size of TM payload -1 for cid/fid

  // Validate data payload size: check if longer than size of testmode
  // primitive minus cid, str_len_in_bytes, and two holes FIXME: This is a
  // really bad way of doing it!
  if (size > sizeof(T_TESTMODE_PRIM) - 4)
    error = -ETM_PACKET;

  // The CID have been received. The data that follows are part of a
  // unique struct within the union of T_TESTMODE_PRIM, so we now
  // need to align at a 32-bit word boundary.
  *pmsg++ = 0;

  // In a SSA integration the cksum is done in the etm_receive function
  #if (OP_L1_STANDALONE == 1)
    if (!error)
    {
      cksum   = mid;
      cksum  ^= cid;

      while (size--)
      {
        cksum  ^= *inbuf;
        *pmsg++ = *inbuf++;
      }
      if (cksum != *inbuf)
        error = -ETM_PACKET;
    }
  #elif (OP_L1_STANDALONE == 0)
    // Copy payload without cid/fid
    while (size--)
    {
      *pmsg++ = *inbuf++;
    }
  #endif

  // At this point, all the data have been parsed and copied into
  // the TestMode primitive.  Now we send the primitive to L1.
  if (!error)
  {
    os_send_sig(msg, L1C1_QUEUE);
    msg_used=TRUE;
  }
  else
  {
    UWORD8 mymsg[4];

    // on error, return short error message; cid, error, checksum
    mymsg[0] = mid;
    mymsg[1] = cid;         // the payload fid
    mymsg[2] = error;       // status
    mymsg[3] = cid ^ error; // checksum

    #if (TRACE_TYPE==0) || (TRACE_TYPE==1) || (TRACE_TYPE==4) || (TRACE_TYPE==7)
      rvt_send_trace_cpy((T_RVT_BUFFER) mymsg,
                         tm_trace_user_id,
                         4,
                         RVT_BINARY_FORMAT);
    #endif
  }

#else // end of (ETM_PROTOCOL ==1)

  // Copy CID
  *pmsg++ = cid = *inbuf++;
  // Copy data payload size (size minus CID byte and checksum byte)
  size -= 2;
  *pmsg++ = size;

  // Validate data payload size: check if longer than size of testmode
  // primitive minus cid, str_len_in_bytes, and two holes FIXME: This is a
  // really bad way of doing it!
  if (size > sizeof(T_TESTMODE_PRIM) - 4)
    error = E_PACKET;

  // The CID have been received. The data that follows are part of a
  // unique struct within the union of T_TESTMODE_PRIM, so we now
  // need to align at a 32-bit word boundary.
  pmsg += 2;

  if (!error)
  {
    cksum = cid;
    while (size--)
    {
      cksum  ^= *inbuf;
      *pmsg++ = *inbuf++;
    }
    if (cksum != *inbuf)
      error = E_CHECKSUM;
  }

  // At this point, all the data have been parsed and copied into
  // the TestMode primitive.  Now we send the primitive to L1.
  if (!error)
  {
    os_send_sig(msg, L1C1_QUEUE);
    msg_used=TRUE;
  }
  else
  {
    UWORD8 mymsg[3];
    // on error, return short error message; error, cid/fid, checksum
    mymsg[0] = cid;
    mymsg[1] = error;
    mymsg[2] = cid ^ error; // checksum

    #if (TRACE_TYPE==1) || (TRACE_TYPE==4) || (TRACE_TYPE==7) || (TRACE_TYPE==0)
       rvt_send_trace_cpy((T_RVT_BUFFER) mymsg, tm_trace_user_id,3, RVT_BINARY_FORMAT);
    #endif
  }

#endif // end of (ETM_PROTOCOL ==0)

  // if the message allocated is not sent to L1A, it has to be deallocated
  if(msg_used==FALSE)
    os_free_sig(msg);
}

void tm_transmit(T_TM_RETURN *tm_ret)
{
  UWORD8 size, cksum;
  UWORD8 *pbuf, *ptmret;
  UWORD8 buf[TM_PAYLOAD_UPLINK_SIZE_MAX + TM_UPLINK_PACKET_OVERHEAD];

  pbuf = &buf[0];

  // move the header
  #if (ETM_PROTOCOL == 1)
    *pbuf++ = tm_ret->mid;
    cksum   = tm_ret->mid;
    *pbuf++ = tm_ret->status;
    cksum  ^= tm_ret->status;
    *pbuf++ = tm_ret->cid;
    cksum  ^= tm_ret->cid;

    // Include index if not equal to zero, and if not an error
    // Exception: in TX_TEMPLATE_READ we always include the index.
    if ((tm_ret->status == -ETM_OK) &&
      (tm_ret->index || tm_ret->cid == TX_TEMPLATE_READ)){
      *pbuf++ = tm_ret->index;
      cksum  ^= tm_ret->index;
    }

  #else
    *pbuf++ = tm_ret->cid;
    *pbuf++ = tm_ret->status;
    cksum = tm_ret->cid ^ tm_ret->status;

    // Include index if not equal to zero, and if not an error
    // Exception: in TX_TEMPLATE_READ we always include the index.
    if ((tm_ret->status == E_OK) &&
      (tm_ret->index || tm_ret->cid == TX_TEMPLATE_READ)){
      *pbuf++ = tm_ret->index;
      cksum  ^= tm_ret->index;
    }
  #endif

  ptmret = (UWORD8 *) &tm_ret->result[0];
  size = (((tm_ret->size) > TM_PAYLOAD_UPLINK_SIZE_MAX) ? TM_PAYLOAD_UPLINK_SIZE_MAX: (tm_ret->size));
  while (size--) {
    *pbuf++ = *ptmret;
    cksum ^= *ptmret++;
  }

  // move the checksum and append it to buf
  *pbuf++ = cksum;

  #if (TRACE_TYPE==1) || (TRACE_TYPE==4) || (TRACE_TYPE==7) || (TRACE_TYPE==0)
    rvt_send_trace_cpy(buf, tm_trace_user_id, pbuf - buf, RVT_BINARY_FORMAT);
  #endif
}


#if ((L1_STEREOPATH == 1) && (CODE_VERSION == NOT_SIMULATION) && (OP_L1_STANDALONE == 1))
/*******************************************************************************
 *
 * UWORD16 l1tm_stereopath_get_pattern(UWORD16 sampling_freq, UWORD16 sin_freq_left,UWORD16 sin_freq_right, UWORD8 data_type)
 *
 * Purpose  : this function is use to get a complete period of a sinusoide depending on
 *            the sinusoide freq (L+R), the sampling freq and the type of samples (S8,S16,S32)
 *
 * Arguments: sampling_freq  : sampling frequency
 *            sin_freq_left  : frequency of the left channel sinusoide
 *            sin_freq_right : frequency of the right channel sinusoide
 *            data_type      : type of samples
 *
 * Returns  : number of elements in the pattern
 *
 ******************************************************************************/

  UWORD16 l1tm_stereopath_get_pattern(UWORD16 sampling_freq, UWORD16 sin_freq_left,UWORD16 sin_freq_right, UWORD8 data_type)
  {
   float max_sin_period;
   float my_time;
   UWORD16 i;

   // get the lowest frequency to get the biggest period
   if (sin_freq_left > sin_freq_right)
   {
     max_sin_period =  1 / (float) sin_freq_right;
   }
   else
   {
     max_sin_period =  1 / (float) sin_freq_left;
   }

   my_time = 0;
   i = 0;

   if (data_type == AUDIO_SP_DATA_S8)
   {
     WORD8* my_ptr;

     // cast the steropath_pattern to a pointer on 8 bits samples
     my_ptr = (WORD8*) l1tm.stereopath.stereopath_pattern;

     // fill the pattern while the biggest period is not reached
     while (my_time < max_sin_period)
     {
       my_ptr[i++] = 0x7F * sin(2*3.1416*my_time*sin_freq_left);
       my_ptr[i++] = 0x7F * sin(2*3.1416*my_time*sin_freq_right);

       my_time = i/2/((float) sampling_freq);
     }
   }
   else  // S16
   {
     WORD16* my_ptr;

     // cast the steropath_pattern to a pointer on 16 bits samples
     my_ptr = (WORD16*) l1tm.stereopath.stereopath_pattern;

     // fill the pattern while the biggest period is not reached
     while (my_time < max_sin_period)
     {
       my_ptr[i++] = 0x7FFF * sin(2*3.1416*my_time*sin_freq_left);
       my_ptr[i++] = 0x7FFF * sin(2*3.1416*my_time*sin_freq_right);

       my_time = i/2/((float) sampling_freq);
     }

   }

   return (i);

  }

/*******************************************************************************
 *
 * void l1tm_stereopath_fill_buffer(void* buffer_address)
 *
 * Purpose  : this function is use to fill a buffer with a predefined pattern
 *
 * Arguments: buffer_address  : address of the buffer to fill
 *
 * Returns  : none
 *
 ******************************************************************************/

  void l1tm_stereopath_fill_buffer(void* buffer_address)
  {
   static UWORD16 my_counter = 0;
   UWORD16 copied_samples;

   UWORD16 i;


   if (l1a_l1s_com.stereopath_drv_task.parameters.data_type == AUDIO_SP_DATA_S8)
   {
     WORD8* start_address;
     WORD8* my_ptr;

     // l1tm.stereopath.stereopath_buffer_number is a variable used to know which half of the buffer we have to fill
     if (l1tm.stereopath.stereopath_buffer_number == 0)
     {
      // first half
      start_address = (WORD8*) buffer_address;
      l1tm.stereopath.stereopath_buffer_number = 1;
     }
     else
     {
      // second half, add the frame number to get the half buffer address
      start_address = ((WORD8*) buffer_address) + l1a_l1s_com.stereopath_drv_task.parameters.frame_number;
      l1tm.stereopath.stereopath_buffer_number = 0;
     }

     // copied_samples is the number of samples copied to the half buffer
     copied_samples = 0;
     // cast the steropath_pattern to a pointer on 8 bits samples
     my_ptr = (WORD8*) l1tm.stereopath.stereopath_pattern;

     if (l1a_l1s_com.stereopath_drv_task.parameters.frame_number > l1tm.stereopath.stereopath_nb_samples)
     {
       // size of the half buffer to fill is bigger than the predefined pattern
       // start to fill the buffer with the end of the not complete previous pattern (from current_sample to the last one)
       memcpy(start_address,my_ptr+l1tm.stereopath.stereopath_current_sample,l1tm.stereopath.stereopath_nb_samples-l1tm.stereopath.stereopath_current_sample);
       copied_samples = l1tm.stereopath.stereopath_nb_samples-l1tm.stereopath.stereopath_current_sample;

       // while there is still enough place in the buffer to copy a complete pattern ...
       while (copied_samples<l1a_l1s_com.stereopath_drv_task.parameters.frame_number-l1tm.stereopath.stereopath_nb_samples)
       {
         // ... copy a complete pattern
         memcpy(start_address+copied_samples,my_ptr,l1tm.stereopath.stereopath_nb_samples);
         copied_samples += l1tm.stereopath.stereopath_nb_samples;
       }

       // fill the rest of the buffer with a part of the pattern
       memcpy(start_address+copied_samples,my_ptr,l1a_l1s_com.stereopath_drv_task.parameters.frame_number-copied_samples);
       // save the last pattern sample copied in the buffer for next time (to get a continuous sound)
       l1tm.stereopath.stereopath_current_sample = l1a_l1s_com.stereopath_drv_task.parameters.frame_number-copied_samples;

     }
     else
     {
       // size of the half buffer to fill is smaller than the predefined pattern
       // fill the buffer with a part of the pattern
       memcpy(start_address,my_ptr+l1tm.stereopath.stereopath_current_sample,l1a_l1s_com.stereopath_drv_task.parameters.frame_number);
       // save the last pattern sample copied in the buffer for next time (to get a continuous sound)
       l1tm.stereopath.stereopath_current_sample += l1a_l1s_com.stereopath_drv_task.parameters.frame_number;

       if (l1tm.stereopath.stereopath_current_sample > l1tm.stereopath.stereopath_nb_samples)
       {
         l1tm.stereopath.stereopath_current_sample -= l1tm.stereopath.stereopath_nb_samples;
       }
     }
   }
   else    // S16
   {
     WORD16* start_address;
     WORD16* my_ptr;

     // l1tm.stereopath.stereopath_buffer_number is a variable used to know which half of the buffer we have to fill
     if (l1tm.stereopath.stereopath_buffer_number == 0)
     {
      // first half
      start_address = (WORD16*) buffer_address;
      l1tm.stereopath.stereopath_buffer_number = 1;
     }
     else
     {
      // second half, add the frame number to get the half buffer address
      start_address = ((WORD16*) buffer_address) + l1a_l1s_com.stereopath_drv_task.parameters.frame_number;
      l1tm.stereopath.stereopath_buffer_number = 0;
     }

     // copied_samples is the number of samples copied to the half buffer
     copied_samples = 0;
     // cast the steropath_pattern to a pointer on 16 bits samples
     my_ptr = (WORD16*) l1tm.stereopath.stereopath_pattern;

     if (l1a_l1s_com.stereopath_drv_task.parameters.frame_number > l1tm.stereopath.stereopath_nb_samples)
     {
       // size of the half buffer to fill is bigger than the predefined pattern
       // start to fill the buffer with the end of the not complete previous pattern (from current_sample to the last one)
       memcpy(start_address, my_ptr+l1tm.stereopath.stereopath_current_sample,(l1tm.stereopath.stereopath_nb_samples-l1tm.stereopath.stereopath_current_sample)*2);
       copied_samples = l1tm.stereopath.stereopath_nb_samples-l1tm.stereopath.stereopath_current_sample;

       // while there is still enough place in the buffer to copy a complete pattern ...
       while (copied_samples<l1a_l1s_com.stereopath_drv_task.parameters.frame_number-l1tm.stereopath.stereopath_nb_samples)
       {
         // ... copy a complete pattern
         memcpy(start_address+copied_samples,my_ptr,l1tm.stereopath.stereopath_nb_samples*2);
         copied_samples += l1tm.stereopath.stereopath_nb_samples;
       }

       // fill the rest of the buffer with a part of the pattern
       memcpy(start_address+copied_samples,my_ptr,(l1a_l1s_com.stereopath_drv_task.parameters.frame_number-copied_samples)*2);
       // save the last pattern sample copied in the buffer for next time (to get a continuous sound)
       l1tm.stereopath.stereopath_current_sample = l1a_l1s_com.stereopath_drv_task.parameters.frame_number-copied_samples;

     }
     else
     {
       // size of the half buffer to fill is smaller than the predefined pattern
       // fill the buffer with a part of the pattern
       memcpy(start_address,my_ptr+l1tm.stereopath.stereopath_current_sample,l1a_l1s_com.stereopath_drv_task.parameters.frame_number);
       l1tm.stereopath.stereopath_current_sample += l1a_l1s_com.stereopath_drv_task.parameters.frame_number;

       // save the last pattern sample copied in the buffer for next time (to get a continuous sound)
       if (l1tm.stereopath.stereopath_current_sample > l1tm.stereopath.stereopath_nb_samples)
       {
         l1tm.stereopath.stereopath_current_sample -= l1tm.stereopath.stereopath_nb_samples;
       }

     }
   }
  }

/*******************************************************************************
 *
 * void l1tm_stereopath_DMA_handler(SYS_UWORD16 dma_status)
 *
 * Purpose  : this function is the stereopath DMA interrupt handler
 *
 * Arguments: dma_status  : type of interrupt
 *
 * Returns  : none
 *
 ******************************************************************************/
  void l1tm_stereopath_DMA_handler(SYS_UWORD16 dma_status)
  {
     // stereopath DMA handler, check which type of interrupt it is
     if (F_DMA_COMPARE_CHANNEL_IT_STATUS_BLOCK(dma_status))
       {
         l1tm.stereopath.stereopath_block++;
         // Block --> fill a new buffer
         l1tm_stereopath_fill_buffer((void*) l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address);
       }
     if (F_DMA_COMPARE_CHANNEL_IT_STATUS_HALF_BLOCK(dma_status))
       {
         l1tm.stereopath.stereopath_half_block++;
         // Half Block --> fill a new buffer
         l1tm_stereopath_fill_buffer((void*) l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address);
       }
     if (F_DMA_COMPARE_CHANNEL_IT_STATUS_TIME_OUT_SRC(dma_status))
       l1tm.stereopath.stereopath_source_timeout++;
     if (F_DMA_COMPARE_CHANNEL_IT_STATUS_TIME_OUT_DST(dma_status))
       l1tm.stereopath.stereopath_dest_timeout++;
     if (F_DMA_COMPARE_CHANNEL_IT_STATUS_DROP(dma_status))
       l1tm.stereopath.stereopath_drop++;
     if (F_DMA_COMPARE_CHANNEL_IT_STATUS_FRAME(dma_status))
       l1tm.stereopath.stereopath_frame++;
  }
#endif



#if (CODE_VERSION != SIMULATION)
  void l1tm_tpu_table_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
  {
    Cust_tm_tpu_table_write(tm_return,
      prim->u.tm_table.index,
      prim->str_len_in_bytes - 1,    // subtract 8-bit index
      prim->u.tm_table.table);
  }

  void l1tm_tpu_table_read(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
  {
    Cust_tm_tpu_table_read(tm_return, prim->u.tm_table.index);
  }
#endif // (CODE_VERSION != SIMULATION)

#if (L1_TPU_DEV == 1)
  /*******************************************************************
  * l1tm_flexi_tpu_table_write
  *
  * DESCRIPTION: The function would be called by the switch case in the etm_rf.
  *******************************************************************/
  void  l1tm_flexi_tpu_table_write(T_TESTMODE_PRIM * prim, T_TM_RETURN * tm_return)
  {
    Cust_tm_flexi_tpu_table_write(tm_return,
      prim->u.tm_table16.index,
      prim->str_len_in_bytes - 1,    // subtract 8-bit index
      prim->u.tm_table16.table);
  }

  /*******************************************************************
  * l1tm_flexi_tpu_table_read
  *
  * DESCRIPTION: The function would be called by the switch case in the etm_rf.
  *******************************************************************/
  void l1tm_flexi_tpu_table_read (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
  {
    Cust_tm_flexi_tpu_table_read(tm_return, prim->u.tm_table16.index);
  }

//Flexi ABB Delays
/*******************************************************************
  * l1tm_flexi_abb_write
  *
  * DESCRIPTION: The function would be called by the switch case in the etm_rf.
  *******************************************************************/
  void  l1tm_flexi_abb_write(T_TESTMODE_PRIM * prim, T_TM_RETURN * tm_return)
  {
    Cust_tm_flexi_abb_write(tm_return,
      prim->u.tm_table16.index,
      prim->str_len_in_bytes - 1,    // subtract 8-bit index
      prim->u.tm_table16.table);
  }

  /*******************************************************************
  * l1tm_flexi_abb_read
  *
  * DESCRIPTION: The function would be called by the switch case in the etm_rf.
  *******************************************************************/
  void l1tm_flexi_abb_read (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return)
  {
    Cust_tm_flexi_abb_read(tm_return, prim->u.tm_table16.index);
  }



 #endif // Flexible TPU

  /*------------------------------------------------------------------*/
  /* l1tm_PRBS1_generate()                                            */
  /*------------------------------------------------------------------*/
  /*                                                                  */
  /* Parameters  :         UWORD16 *TM_ul_data                        */
  /* -------------                                                    */
  /*                point to the uplink burts table to be filled      */
  /*                with the PRBS 1 of bits                           */
  /*                                                                  */
  /* Return      :                Void                                */
  /* -------------                                                    */
  /*                                                                  */
  /* Description :                                                    */
  /* -------------                                                    */
  /* This algorithm generates  a Pseudo Random Bit Sequence           */
  /* using a method called method  "Primitive Polynomial Modulo 2"    */
  /* For a sequence length of (2^15-1) we a polynomial of order 15    */
  /* is used, the coefficients are [ 15,  1,  0 ]                     */
  /* The basic idea is to generate the new bit by XORing  all the     */
  /* coefficients  of the polynomial except coeff 0                   */
  /* i.e newbit = ( B15 XOR B1  )                                     */
  /* The following notation must be used for the bit numbering:       */
  /* _______________________________________________________          */
  /*|B16|B15|B14|B13|B12|B11|B10|B9|B8|B7|B6|B5|B4|B3|B2|B1|          */
  /*------------------------------------------------------------------*/
  /*                                                                  */
  /* each word of the uplink buffer needs to be filled by new 10 bits */
  /*------------------------------------------------------------------*/
  void l1tm_PRBS1_generate(UWORD16 *TM_ul_data)
  {
    #define B15_MASK 	0x4000
    #define B1_MASK 	0x0001
    #define MASK_16BITS	0xFFFF

    UWORD16 newbit =0x0000;
    UWORD8 index ,word;

    //generate 16 words to fill Uplink table
    for (word=0;word<16;word++)
    {
      // generate new 10 bits from the sequence
      for (index =0; index< 10;index++)
      {
        // generate new bit , using the "Primitive Polynomial Modulo 2 " method with coeff. ( 15, 1, 0 )
        //XOR bit 15 with bit 1.
        newbit =     (((l1tm.tmode_prbs.prbs1_seed& B15_MASK)>>14)^(l1tm.tmode_prbs.prbs1_seed & B1_MASK));
        // insert new bit in the sequence.
        l1tm.tmode_prbs.prbs1_seed =	(l1tm.tmode_prbs.prbs1_seed << 1) | newbit;
      }
      TM_ul_data[word]=(UWORD16)((l1tm.tmode_prbs.prbs1_seed&MASK_16BITS)<<6);
    }
  }

#endif // TESTMODE


