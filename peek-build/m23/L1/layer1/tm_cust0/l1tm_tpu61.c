/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1TM_TPU12.C
 *
 *        Filename l1tm_tpu61.c
 *  Copyright 2003 (C) Texas Instruments  
 *
 *
 * This file would have definitions of functions specific to UppCosto/Locosto TPU Development.
 * Currently this would be definitons of functions meant for Flexible TPU Timings. These would need to
 * be ported to earlier platforms of Syren/Rita if TPU Flexible Timings Feature is to be used there
 * 
 *
 ************* Revision Controle System Header *************/

#include "l1_macro.h"
#include "l1_confg.h"

#if (TESTMODE == 1)
   #include "general.h"  
  #include "tm_defs.h"
  #include "l1_const.h"
  #include "l1_types.h"
  					
  					
  #include "l1tm_defty.h"
  #include "l1tm_cust.h"
  #include "l1tm_tpu61.h"
  
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
  #include "l1_msgty.h"
  #include "l1_tabs.h"
  
  #include "l1tm_msgty.h"
  #include "l1tm_varex.h"
  
  #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))
    #include "spi_drv.h"
  #endif   
  
  #include "sys_types.h"
  #include "l1_time.h"
  #include "tpudrv.h"
  #include "tpudrv61.h"
  #include <string.h>
  
  #if (L1_TPU_DEV == 1)
   #include "l1_varex.h"
  #endif
  //Import TPU Definitions
  extern T_L1_CONFIG  l1_config;
  
  // External function prototypes
  
  //Function Definitions
  
  
  void Cust_tm_tpu_table_write        (T_TM_RETURN *tm_return, WORD8 index, UWORD8 size, UWORD8 table[])
  {
  /*
  	INDICES BETWEEN in [0..63] RANGE ARE RESERVED FOR TI RF MODULE
  */
  
      // fill in the cid
      tm_return->cid = TPU_TABLE_WRITE;
  
      switch (index)
      {
          default:
            tm_return->size = 0;
            tm_return->status = E_BADINDEX;
            break;
      } // end of switch
  }
  
  void Cust_tm_tpu_table_read         (T_TM_RETURN *tm_return, WORD8 index)
  {
  /*
  	INDICES BETWEEN in [0..63] RANGE ARE RESERVED FOR TI RF MODULE
  */
  
      // fill in the cid
      tm_return->cid = TPU_TABLE_READ;
  
      switch (index)
      {
          default:
            tm_return->size = 0;
            tm_return->status = E_BADINDEX;
            break;
      } // end of switch
  }
  
  
  #if (L1_TPU_DEV == 1)
    /*Flexible TPU Timings related ...
     The following function writes the Values meant for the tables (arrays rf_rx_tpu_timings & 
    rf_tx_tpu_timings into the RAM so that TPU can pick those values.
    
    */
    
    
    void Cust_tm_flexi_tpu_table_write        (T_TM_RETURN *tm_return, WORD8 index, UWORD8 size, WORD16 table16[])
    {
    /*
    	INDICES BETWEEN in [0..63] RANGE ARE RESERVED FOR TI RF MODULE
    */
    
        // fill in the cid
      tm_return->cid = FLEXI_TPU_TABLE_WRITE;
      tm_return->size = 0;
    
      #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_OK;
        #else
          tm_return->status = E_OK;
      #endif
         
    //Decide on the various Indices in the Table and do the required actions in this case
    //copy the relevant values in the TPU Table... 
    
      switch (index)
      {
    	case 2: // The entire RF_RX_TPU_TIMING Table
           memcpy(l1_config.tmode.rx_params.p_rf_rx_tpu_timings, table16, NB_TPU_TIMINGS*sizeof(WORD16));
    	break;
    
    	case 1: // The entire RF_TX_TPU_TMINGS Table
           memcpy(l1_config.tmode.tx_params.p_rf_tx_tpu_timings, table16, NB_TPU_TIMINGS*sizeof(WORD16));
         	break;
    
    	case 10: // From 10 - 41 l1_config.tmode.rx_params.p_rf_rx_tpu_timings
           memcpy( &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[0]), table16, sizeof(WORD16));
           break;
    
           case 11:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[1]), table16, sizeof(WORD16));
           break;
    
           case 12:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[2]), table16, sizeof(WORD16));
           break;
    
           case 13:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[3]), table16, sizeof(WORD16));
           break;
    
           case 14:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[4]), table16, sizeof(WORD16));
           break;
    
    
           case 15:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[5]), table16, sizeof(WORD16));
           break;
    
           case 16:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[6]), table16, sizeof(WORD16));
           break;
    
           case 17:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[7]), table16, sizeof(WORD16));
           break;
    
           case 18:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[8]), table16, sizeof(WORD16));
           break;
    	   
           case 19:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[9]), table16, sizeof(WORD16));
           break;
    
           case 20:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[10]), table16, sizeof(WORD16));
           break;
    
           case 21:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[11]), table16, sizeof(WORD16));
           break;
    
           case 22:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[12]), table16, sizeof(WORD16));
           break;
    
           case 23:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[13]), table16, sizeof(WORD16));
           break;
    
           case 24:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[14]), table16, sizeof(WORD16));
           break;
    
           case 25:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[15]), table16, sizeof(WORD16));
           break;
    
           case 26:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[16]), table16, sizeof(WORD16));
           break;
    
           case 27:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[17]), table16, sizeof(WORD16));
           break;
    
           case 28:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[18]), table16, sizeof(WORD16));
           break;
    
           case 29:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[19]), table16, sizeof(WORD16));
           break;
    
           case 30:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[20]), table16, sizeof(WORD16));
           break;
    	   
           case 31:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[21]), table16, sizeof(WORD16));
           break;
    
           case 32:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[22]), table16, sizeof(WORD16));
           break;
    
           case 33:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[23]), table16, sizeof(WORD16));
           break;
    
           case 34:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[24]), table16, sizeof(WORD16));
           break;
    
           case 35:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[25]), table16, sizeof(WORD16));
           break;
    
           case 36:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[26]), table16, sizeof(WORD16));
           break;
    
           case 37:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[27]), table16, sizeof(WORD16));
           break;
    
           case 38:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[28]), table16, sizeof(WORD16));
           break;
    
           case 39:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[29]), table16, sizeof(WORD16));
           break;
    
           case 40:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[30]), table16, sizeof(WORD16));
           break;
    
           case 41:
           memcpy(&(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[31]), table16, sizeof(WORD16));
           break;
    
           //From 42-73 l1_config.tmode.tx_params.rf_tx_tpu_timings 
    	case 42: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[0]), table16, sizeof(WORD16));
         	break;
    
    	case 43: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[1]), table16, sizeof(WORD16));
         	break;
    
    	case 44: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[2]), table16, sizeof(WORD16));
         	break;
    
    	case 45: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[3]), table16, sizeof(WORD16));
         	break;
    
    	case 46: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[4]), table16, sizeof(WORD16));
         	break;
    
    	case 47: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[5]), table16, sizeof(WORD16));
         	break;
    
    	case 48: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[6]), table16, sizeof(WORD16));
         	break;
    
    	case 49: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[7]), table16, sizeof(WORD16));
         	break;
    
    	case 50: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[8]), table16, sizeof(WORD16));
         	break;
    
    	case 51: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[9]), table16, sizeof(WORD16));
         	break;
    
    	case 52: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[10]), table16, sizeof(WORD16));
         	break;
    
    	case 53: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[11]), table16, sizeof(WORD16));
         	break;
    		
    	case 54: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[12]), table16, sizeof(WORD16));
         	break;
    
    	case 55: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[13]), table16, sizeof(WORD16));
         	break;
    
    	case 56: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[14]), table16, sizeof(WORD16));
         	break;
    
    	case 57: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[15]), table16, sizeof(WORD16));
         	break;
    
    	case 58: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[16]), table16, sizeof(WORD16));
         	break;
    
    	case 59: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[17]), table16, sizeof(WORD16));
         	break;
    
    	case 60: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[18]), table16, sizeof(WORD16));
         	break;
    
    	case 61: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[19]), table16, sizeof(WORD16));
         	break;
    
    	case 62: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[20]), table16, sizeof(WORD16));
         	break;
    
    	case 63: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[21]),table16, sizeof(WORD16));
         	break;
    
    	case 64: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[22]), table16, sizeof(WORD16));
         	break;
    
    	case 65: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[23]), table16, sizeof(WORD16));
         	break;
    
    	case 66: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[24]), table16, sizeof(WORD16));
         	break;
    
    	case 67: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[25]), table16, sizeof(WORD16));
         	break;
    
    	case 68: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[26]), table16, sizeof(WORD16));
         	break;
    
    	case 69: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[27]), table16, sizeof(WORD16));
         	break;
    
    	case 70: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[28]), table16, sizeof(WORD16));
         	break;
    
    	case 71: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[29]), table16, sizeof(WORD16));
         	break;
    
    	case 72: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[30]), table16, sizeof(WORD16));
         	break;
    
    	case 73: 
           memcpy(&(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[31]), table16, sizeof(WORD16));
         	break;
    	   
           default:
           #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_INVAL;
           #else
           tm_return->status = E_BADINDEX;
           #endif
           break;
        } // end of switch
    }
    
    void Cust_tm_flexi_tpu_table_read         (T_TM_RETURN *tm_return, WORD8 index)
    {
    /*
    	INDICES BETWEEN in [0..63] RANGE ARE RESERVED FOR TI RF MODULE
    */
    
    UWORD16 value;
    
        // fill in the cid
        tm_return->cid = FLEXI_TPU_TABLE_READ;
        tm_return->size = 2; 
        tm_return->index = index;
    
      #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_OK;
        #else
          tm_return->status = E_OK;
      #endif
    	
    
    //TBD... Copy the relevant data into the tm_return cid,index,status,size, result
        switch (index)
        {
          case 2: // The entire RF_RX_TPU_TIMING Table
    
           tm_return->size =NB_TPU_TIMINGS*sizeof(WORD16);
           memcpy(tm_return->result, (UWORD8 *) l1_config.tmode.rx_params.p_rf_rx_tpu_timings,  tm_return->size);
    	break;
    
    	case 1: // The entire RF_TX_TPU_TMINGS Table 
    	  tm_return->size = NB_TPU_TIMINGS*sizeof(WORD16);
             memcpy(tm_return->result, (UWORD8 *)  l1_config.tmode.tx_params.p_rf_tx_tpu_timings, tm_return->size);
    	break;
    
            // From 10 - 41 RF_RX_TPU_TIMINGS
          case 10: 
          memcpy(tm_return->result, (UWORD8 *) &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[0]), 2);
    	break;
    
          case 11: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[1]), tm_return->size);
    	break;
    
          case 12: 
          memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[2]), tm_return->size);
    	break;
    
          case 13: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[3]), tm_return->size);
    	break;
    
          case 14: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[4]), tm_return->size);
    	break;
    
          case 15: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[5]), tm_return->size);
    	break;
    
          case 16: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[6]), tm_return->size);
    	break;
    
          case 17: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[7]), tm_return->size);
    	break;
    
          case 18: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[8]), tm_return->size);
    	break;
    
          case 19: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[9]), tm_return->size);
    	break;
    
          case 20: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[10]), tm_return->size);
    	break;
    	
          case 21: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[11]), tm_return->size);
    	break;
    	
          case 22: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[12]), tm_return->size);
    	break;
    
          case 23: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[13]), tm_return->size);
    	break;
    
          case 24: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[14]), tm_return->size);
    	break;
    
          case 25: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[15]), tm_return->size);
    	break;
    	
          case 26: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[16]), tm_return->size);
    	break;
    
          case 27: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[17]), tm_return->size);
    	break;
    
          case 28: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[18]), tm_return->size);
    	break;
    
          case 29: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[19]), tm_return->size);
    	break;
    
          case 30: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[20]), tm_return->size);
    	break;
    
          case 31: 
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[21]), tm_return->size);
    	break;
    
          case 32:
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[22]),tm_return->size);      
    	break;
    
          case 33:
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[23]),tm_return->size);
    	break;
    
          case 34:
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[24]),tm_return->size);
    	break;
    
          case 35:
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[25]),tm_return->size);
    	break;
    
          case 36:
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[26]),tm_return->size);
    	break;
    
          case 37:
            
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[27]),tm_return->size);
    	break;
    
          case 38:
            
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[28]),tm_return->size);
    	break;
    
          case 39:
            
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[29]),tm_return->size);
    	break;
          case 40:
            
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[30]),tm_return->size);
    	break;
    
          case 41:
            
           memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.rx_params.p_rf_rx_tpu_timings[31]),tm_return->size);
    	break;
    
           //From 42-73 RF_TX_TPU_TIMINGS 
    	case 42:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[0]), tm_return->size);
    	break;
    
    	case 43:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[1]), tm_return->size);
    	break;
    
    	case 44:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[2]), tm_return->size);
    	break;
    
    	case 45:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[3]), tm_return->size);
    	break;
    
    	case 46:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[4]), tm_return->size);
    	break;
    
    	case 47:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[5]), tm_return->size);
    	break;
    
    	case 48:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[6]), tm_return->size);
    	break;
    
    	case 49:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[7]), tm_return->size);
    	break;
    
    	case 50:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[8]), tm_return->size);
    	break;
    
    	case 51:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[9]), tm_return->size);
    	break;
    
    
    	case 52:  
    	  
             memcpy( tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[10]), tm_return->size);
    	break;
    
    	case 53:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[11]), tm_return->size);
    	break;
    	
    	case 54:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[12]), tm_return->size);
    	break;
    	
    	case 55:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[13]), tm_return->size);
    	break;
    
    	case 56:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[14]), tm_return->size);
    	break;
    
    	case 57:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[15]), tm_return->size);
    	break;
    
    	case 58:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[16]), tm_return->size);
    	break;
    
    	case 59:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[17]), tm_return->size);
    	break;
    
    	case 60:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[18]), tm_return->size);
    	break;
    
    	case 61:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[19]), tm_return->size);
    	break;
    
    	case 62:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[20]), tm_return->size);
    	break;
    
    	case 63:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[21]), tm_return->size);
    	break;
    
    	case 64:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[22]), tm_return->size);
    	break;
    
    	case 65:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[23]), tm_return->size);
    	break;
    
    	case 66:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[24]), tm_return->size);
    	break;
    
    	case 67:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[25]), tm_return->size);
    	break;
    
    	case 68:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[26]), tm_return->size);
    	break;
    
    	case 69:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[27]), tm_return->size);
    	break;
    
    	case 70:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[28]), tm_return->size);
    	break;
    
    	case 71:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[29]), tm_return->size);
    	break;
    
    	case 72:  
    	  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[30]), tm_return->size);
    	break;
    
          case 73:  
             memcpy(tm_return->result, (UWORD8 *)  &(l1_config.tmode.tx_params.p_rf_tx_tpu_timings[31]), tm_return->size);
    	break;
    	
            default:
              tm_return->size = 0;
          #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_INVAL;
          #else
            tm_return->status = E_BADINDEX;
          #endif
              break;
        } // end of switch
    }
    
    //Flexi ABB Delays
    /*
     Flexible ABB DELAYS related ...
     The following function writes the Values meant for the tables (arrays rf_flexi_abb_delays)
     into the RAM so that TPU can pick those values.
    */
    
    void Cust_tm_flexi_abb_write        (T_TM_RETURN *tm_return, WORD8 index, UWORD8 size, WORD16 table16[])
    {
    
      // fill in the cid
      tm_return->cid = FLEXI_ABB_WRITE;
      tm_return->size = 0;
    
      #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_OK;
        #else
          tm_return->status = E_OK;
      #endif
         
    //Decide on the various Indices in the Table and do the required actions in this case
    if  (index == 0)
      {
    	 // The entire table
           memcpy(l1_config.tmode.rf_params.p_rf_flexi_abb_delays, table16, NB_ABB_DELAYS*sizeof(WORD16));
      }
    if (index > 0 && index < 32)
      {
           memcpy( &(l1_config.tmode.rf_params.p_rf_flexi_abb_delays[index]), table16, sizeof(WORD16));
       }
    else
      {
        #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_INVAL;
         #else
            tm_return->status = E_BADINDEX;
        #endif
       } // else
    
	switch(index)
		{
               case 1:
			    break;


		case 6: // APCDELDOWN
		{
    		  UWORD16 apcdel1 = (((APCDEL_DOWN & 0x1f)<<5)  |(APCDEL_UP & 0x1f) );
    		  UWORD16 apcdel2 = (((APCDEL_DOWN & 0x3e0)) |((APCDEL_UP >>5) & 0x1f) );
    
                 // Changing to write the APCDEL1, APCDEL2  via API 
                 l1s_dsp_com.dsp_ndb_ptr->d_apcdel1  = apcdel1;
                 l1s_dsp_com.dsp_ndb_ptr->d_apcdel2  = apcdel2;

		  break;
		}
		case 7 : // APCDELUP
		{
    		   UWORD16 apcdel1 = (((APCDEL_DOWN & 0x1f)<<5)  |(APCDEL_UP & 0x1f) );
    		   UWORD16 apcdel2 = (((APCDEL_DOWN & 0x3e0)) |((APCDEL_UP >>5) & 0x1f) );
			  
                 // Changing to write the APCDEL1, APCDEL2  via API 
                 l1s_dsp_com.dsp_ndb_ptr->d_apcdel1  = apcdel1;
                 l1s_dsp_com.dsp_ndb_ptr->d_apcdel2  = apcdel2;

		   break;
		}

		case 8 : // GUARD BIT
		{
                 // Changing to write the GUARD BITS  via API 
                  l1s_dsp_com.dsp_ndb_ptr->d_tch_mode    |= (((GUARD_BITS) & 0x000F) << 7); //Bit 7..10: guard bits
  
                 break;
		}

		default: // Do Nothing
		break;

		} // Switch
		
    }
    
    void Cust_tm_flexi_abb_read         (T_TM_RETURN *tm_return, WORD8 index)
    {
    /*
    	INDICES BETWEEN in [0..63] RANGE ARE RESERVED FOR TI RF MODULE
    */
      UWORD16 value;
    
        // fill in the cid
        tm_return->cid = FLEXI_ABB_READ;
        tm_return->size = 2; 
        tm_return->index = index;
    
      #if (ETM_PROTOCOL == 1)
          tm_return->status = -ETM_OK;
        #else
          tm_return->status = E_OK;
      #endif
    	
    //TBD... Copy the relevant data into the tm_return cid,index,status,size, result
    
    if (index == 0)
      {
       //Read the Entire table
           tm_return->size =NB_ABB_DELAYS*sizeof(WORD16);
           memcpy(tm_return->result, (UWORD8 *) l1_config.tmode.rf_params.p_rf_flexi_abb_delays,  tm_return->size);
       }
    
    if (index > 0 && index < 32)
      {
          memcpy(tm_return->result, (UWORD8 *) &(l1_config.tmode.rf_params.p_rf_flexi_abb_delays[index]), 2);
      }
    else
      {
           #if (ETM_PROTOCOL == 1)
            tm_return->status = -ETM_INVAL;
           #else
           tm_return->status = E_BADINDEX;
           #endif
    
        } // else
    }
    
  #endif // L1_TPU_DEV
#endif //TESTMODE
